# Build and testing notes

Public release **1.0** packages executable build **1.0.1**. Publication does not change the executable's internal version or its bytes.

The creator reports full-campaign playability of the established V1. Separately, the supplied updated Windows binary was freshly compiled and recorded 48 passing component checks, isolated Wine menu/gameplay/dialogue checks, a clean exit, and a repair test that preserved all six saved-data/canary files. The accompanying build report contains the exact scope and component identities.

For this publication pass, the EXE and input ZIP hashes were rechecked; ZIP CRCs, archive paths and manifest entries were checked again. The gallery was captured directly from the same EXE in a fresh Wine 9.0 test prefix. It shows the tactical movement overlay, Bumblebee's 3D tech specs and the original story sequence. No generated artwork is used in the gallery. Previews are resized/compressed; original PNGs are in the Windows package. See screenshots.json for hashes and capture details.

## Known boundaries

Actual modern-intro playback and audible game audio still require a native Windows acceptance check. Wine rejected the Media Foundation player; the game's fallback continued. Original Glu/Hasbro startup graphics remain on the no-intro/fallback path. `--no-intro` is available for that launch.

The supplied intro is 4K/60 video. Higher presentation limits do not establish unlocked simulation or animation rates. A completed full texture remaster and later unrecovered rebinding/settings work are not claimed for this exact binary.

The EXE is unsigned. No new full-campaign playthrough or native Windows GPU/performance certification is claimed by these automated checks. This does not erase the creator's reported V1 playthrough.
