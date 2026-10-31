@echo off
setlocal
cd /d "%~dp0"
where py >nul 2>&1
if %errorlevel%==0 (
  py -3 tools\publish_release.py --publish --login --assets assets
) else (
  python tools\publish_release.py --publish --login --assets assets
)
if errorlevel 1 (
  echo.
  echo Publication stopped. No incomplete release is intentionally published.
  echo Python 3 and GitHub CLI are required. Use gh auth login for account sign-in.
  pause
  exit /b 1
)
echo.
echo The complete release has been published and its remote hashes verified.
pause
