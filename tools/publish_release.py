#!/usr/bin/env python3
"""Publish only the checksum-paired full game and source; requires authenticated GitHub CLI."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import stat
import subprocess
import sys
import tempfile
import zipfile

REPOSITORY = 'iammrmikemanmods/G1-Awakening-PC'
TAG = 'v1.0'
ROOT = Path(__file__).resolve().parents[1]


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def load_plan() -> dict:
    plan = json.loads((ROOT / 'releases/v1.0/manifest.json').read_text(encoding='utf-8'))
    if plan.get('repository') != REPOSITORY or plan.get('tag') != TAG:
        raise ValueError('Wrong publication repository/tag.')
    names = []
    for asset in plan['assets']:
        name = asset['name']
        if not name or Path(name).name != name or any(c in name for c in '/\\\r\n'):
            raise ValueError('Unsafe asset filename.')
        if not re.fullmatch(r'[0-9a-f]{64}', asset['sha256']) or asset['bytes'] <= 0:
            raise ValueError('Invalid asset identity.')
        names.append(name.casefold())
    if len(names) != len(set(names)):
        raise ValueError('Duplicate asset names.')
    if 'g1awakening.exe' not in names or 'g1_awakening_pc_1.0_source.zip' not in names:
        raise ValueError('The complete game and matching source are mandatory.')
    return plan


def check_file(path: Path, asset: dict) -> None:
    if not path.is_file() or path.is_symlink():
        raise ValueError(f'Missing or unsafe file: {path.name}')
    if path.stat().st_size != asset['bytes'] or sha256(path) != asset['sha256']:
        raise ValueError(f'Checksum/size mismatch: {path.name}; nothing will be published.')


def cli(*args: str, interactive: bool = False) -> str:
    executable = shutil.which('gh')
    if executable is None:
        conventional = Path(os.environ.get('ProgramFiles', r'C:\Program Files')) / 'GitHub CLI/gh.exe'
        if conventional.is_file():
            executable = str(conventional)
    if executable is None:
        raise RuntimeError('Install GitHub CLI first: winget install --id GitHub.cli --exact')
    result = subprocess.run([executable, *args], text=True, encoding='utf-8',
                            errors='replace', stdout=None if interactive else subprocess.PIPE,
                            stderr=None if interactive else subprocess.PIPE)
    if result.returncode:
        detail = (result.stderr or 'See the console output.').strip()
        raise RuntimeError(f'GitHub CLI failed ({result.returncode}): {detail}')
    return result.stdout or ''


def release() -> dict | None:
    # This repository's first release. Refuse ambiguity instead of overwriting any existing release.
    releases = json.loads(cli('api', f'repos/{REPOSITORY}/releases?per_page=100'))
    matches = [item for item in releases if item['tag_name'] == TAG]
    if len(matches) > 1:
        raise RuntimeError('Ambiguous release identity.')
    if not matches and len(releases) == 100:
        raise RuntimeError('Release listing requires pagination; refusing to create a duplicate.')
    return matches[0] if matches else None


def ensure_draft(plan: dict) -> dict:
    existing = release()
    if existing:
        return existing
    cli('release', 'create', TAG, '--repo', REPOSITORY, '--draft', '--target', 'main',
        '--title', plan['title'], '--notes-file', str(ROOT / 'releases/v1.0/notes.md'))
    created = release()
    if created is None or not created['draft']:
        raise RuntimeError('Could not confirm the draft release.')
    return created


def check_remote(asset: dict, expected: dict) -> None:
    if asset.get('state') != 'uploaded' or asset.get('size') != expected['bytes']:
        raise ValueError(f'Incomplete or conflicting remote asset: {expected["name"]}')
    digest = asset.get('digest')
    if digest:
        if digest != 'sha256:' + expected['sha256']:
            raise ValueError(f'Remote checksum mismatch: {expected["name"]}')
    else:
        # Older API responses: verify downloaded bytes, never run the downloaded EXE.
        with tempfile.TemporaryDirectory(prefix='g1-release-verify-') as directory:
            cli('release', 'download', TAG, '--repo', REPOSITORY, '--pattern', expected['name'], '--dir', directory)
            check_file(Path(directory) / expected['name'], expected)


def verify_remote(current: dict, plan: dict) -> None:
    for expected in plan['assets']:
        matches = [item for item in current['assets'] if item['name'] == expected['name']]
        if len(matches) != 1:
            raise ValueError(f'Missing/duplicate release asset: {expected["name"]}')
        check_remote(matches[0], expected)


def prepare_local(directory: Path, plan: dict) -> None:
    mapping = {a['name']: a for a in plan['assets']}
    package = directory / 'G1_Awakening_PC_1.0_Windows.zip'
    executable = directory / 'G1Awakening.exe'
    check_file(package, mapping[package.name])
    if not executable.exists():
        # Extract only the declared EXE member, not arbitrary archive paths.
        with zipfile.ZipFile(package) as archive:
            info = archive.getinfo(plan['exe_member'])
            if info.file_size != mapping[executable.name]['bytes']:
                raise ValueError('Embedded EXE size mismatch.')
            fd, temporary = tempfile.mkstemp(prefix='g1-exe-', suffix='.tmp', dir=directory)
            try:
                with os.fdopen(fd, 'wb') as output, archive.open(info) as source:
                    shutil.copyfileobj(source, output)
                check_file(Path(temporary), mapping[executable.name])
                # Do not overwrite a file created concurrently.
                with executable.open('xb') as output, Path(temporary).open('rb') as source:
                    shutil.copyfileobj(source, output)
            finally:
                Path(temporary).unlink(missing_ok=True)
    for asset in plan['assets']:
        check_file(directory / asset['name'], asset)


def extract_source(archive_path: Path, destination: Path) -> Path:
    prefix = 'G1_Awakening_PC_1.0_Source'
    with zipfile.ZipFile(archive_path) as archive:
        names = set()
        total = 0
        for item in archive.infolist():
            path = PurePosixPath(item.filename)
            if not path.parts or path.is_absolute() or '..' in path.parts or '\\' in item.filename or path.parts[0] != prefix:
                raise ValueError('Unsafe source archive path.')
            if item.filename.casefold() in names or stat.S_ISLNK(item.external_attr >> 16):
                raise ValueError('Duplicate/symlink source entry.')
            names.add(item.filename.casefold())
            total += item.file_size
        if total > 512 * 1024 * 1024 or len(names) > 30000:
            raise ValueError('Unexpected source archive size.')
        if archive.testzip() is not None:
            raise ValueError('Source ZIP CRC error.')
        archive.extractall(destination)
    source_root = destination / prefix
    inventory = json.loads((source_root / 'SOURCE_SHA256.json').read_text(encoding='utf-8'))
    expected = set(inventory) | {'SOURCE_SHA256.json'}
    actual = {p.relative_to(source_root).as_posix() for p in source_root.rglob('*') if p.is_file()}
    if actual != expected:
        raise ValueError('Source manifest does not describe all archive files.')
    for name, identity in inventory.items():
        check_file(source_root / name, dict(identity, name=name))
    return source_root


def import_source(plan: dict) -> None:
    current = release()
    if current is None or current['draft']:
        raise RuntimeError('No published complete release to import.')
    verify_remote(current, plan)
    name = 'G1_Awakening_PC_1.0_Source.zip'
    identity = next(a for a in plan['assets'] if a['name'] == name)
    with tempfile.TemporaryDirectory(prefix='g1-source-') as directory:
        temp = Path(directory)
        cli('release', 'download', TAG, '--repo', REPOSITORY, '--pattern', name, '--dir', directory)
        check_file(temp / name, identity)
        source_root = extract_source(temp / name, temp / 'extracted')
        for child in ('source', 'tools', 'provenance'):
            shutil.copytree(source_root / child, ROOT / child, dirs_exist_ok=True)
        for child in ('README.md', 'PLAYER_README.md', 'PUBLICATION_NOTES.md'):
            shutil.copy2(source_root / child, ROOT / 'docs' / ('SOURCE_' + child))
    readme = ROOT / 'README.md'
    text = readme.read_text(encoding='utf-8')
    start, end = '<!-- release-status:start -->', '<!-- release-status:end -->'
    if text.count(start) != 1 or text.count(end) != 1:
        raise ValueError('README publication markers changed; refusing broad replacement.')
    before, remainder = text.split(start, 1)
    _, after = remainder.split(end, 1)
    links = ('\n**[Download 1.0](https://github.com/' + REPOSITORY + '/releases/tag/' + TAG + ')**'
             ' — complete Windows EXE, player package, matching source and checksums.\n')
    readme.write_text(before + start + links + end + after, encoding='utf-8')


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    modes = parser.add_mutually_exclusive_group(required=True)
    modes.add_argument('--verify-only', action='store_true')
    modes.add_argument('--prepare-draft', action='store_true')
    modes.add_argument('--publish', action='store_true')
    modes.add_argument('--import-source', action='store_true')
    parser.add_argument('--assets', type=Path, default=ROOT / 'assets')
    parser.add_argument('--login', action='store_true', help='Use GitHub CLI browser login if required.')
    args = parser.parse_args()
    plan = load_plan()
    if args.verify_only or args.publish:
        prepare_local(args.assets.resolve(), plan)
        print('All five publication assets match their exact size and SHA-256.')
    if args.verify_only:
        return 0
    try:
        cli('auth', 'status', '--hostname', 'github.com')
    except RuntimeError:
        if not args.login:
            raise
        cli('auth', 'login', '--hostname', 'github.com', '--git-protocol', 'https', '--web', interactive=True)
    if args.import_source:
        import_source(plan)
        print('Verified full matching source imported; README download links enabled.')
        return 0
    current = ensure_draft(plan)
    if args.prepare_draft:
        print('Draft prepared; complete-game assets must be uploaded before publication.')
        return 0
    if not current['draft']:
        verify_remote(current, plan)
        print('The matching complete release is already published; nothing changed.')
        return 0
    # Never clobber an existing asset, and never publish after an upload/hash failure.
    for expected in plan['assets']:
        matches = [a for a in current['assets'] if a['name'] == expected['name']]
        if len(matches) > 1:
            raise ValueError('Duplicate remote asset.')
        if matches:
            check_remote(matches[0], expected)
        else:
            cli('release', 'upload', TAG, str(args.assets.resolve() / expected['name']), '--repo', REPOSITORY)
    current = release()
    if current is None:
        raise RuntimeError('Draft disappeared.')
    verify_remote(current, plan)
    cli('release', 'edit', TAG, '--repo', REPOSITORY, '--draft=false', '--prerelease=false', '--latest',
        '--title', plan['title'], '--notes-file', str(ROOT / 'releases/v1.0/notes.md'))
    final = release()
    if final is None or final['draft']:
        raise RuntimeError('Publication was not confirmed.')
    verify_remote(final, plan)
    print('Published and verified: ' + final['html_url'])
    return 0


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, KeyError, zipfile.BadZipFile) as error:
        print('STOP: ' + str(error), file=sys.stderr)
        raise SystemExit(1)
