# Native attached-camera pitch investigation

Inspected read-only on 2026-10-24: the original Kenshi 1.0.65 GOG executable
retained at `C:\GOG Games\Kenshi\RE_Kenshi\kenshi_GOG_x64.exe`, using PE imports
and x64 disassembly. This is the original executable, not the rewritten launcher
in the parent game directory. The header RVAs match this original image.

SHA-256: `2DEFC287C813798B9682B93ACF68D340DA895CF5164D342145BA0FAEB9024681`.

## Native evidence

| RVA | Observed operation | Implication |
| --- | --- | --- |
| `0x6AF4E3` through `0x6AF4FF` | Ogre camera position ZERO; `lookAt(UNIT_Z)` | Lens optics face local +Z, with world/local +Y up at identity parent. Ogre's default -Z view convention alone is insufficient. |
| `0x6AF540`, `0x6AF599` through `0x6AF60F` | Store center at `+0x58`; create its child at `+0x70`; attach Ogre camera there | Native hierarchy is center -> lens node -> camera. Both nodes start with identity orientation. |
| `0x6AF615` through `0x6AF62A` | Lens node `setPosition(0,0,-150)` | Negative local Z is the camera's radial offset from the tracked pivot. |
| `0x6AF630` through `0x6AF66C` | Construct +30-degree X quaternion; assign center orientation | Native positive pitch looks down, placing the lens above the pivot. |
| `0x6AF7D9` through `0x6AF7F4` | Process fields at `+0x18` and `+0x1C` | CameraClass yaw/pitch are input deltas, not an absolute pose representation. |
| `0x6AF83A` through `0x6AF872` | Scale yaw input by sensitivity and frame time, degrees-to-radians, center yaw with transform space 2 | Center owns world-Y yaw (`TS_WORLD`). |
| `0x6AF882` through `0x6AF905` | Read actual Ogre `getRealDirection().y`, gate pitch by direction/input sign, center pitch with space 0 | Same center owns local-X pitch (`TS_LOCAL`). Native pitch bounds are checked before applying a delta, not by an absolute target clamp. |
| `0x6AF8BC`, `0x6AF8CB` | Direction-Y guards compare against +0.2 / -0.92 | Vanilla normally confines the view but its per-delta guard is not a replacement for our absolute clamp. |
| `0x6AF915`, `0x6AF940` | Zero both input fields; rotate simply writes these fields then dispatches rotationUpdate | Reading CameraClass yaw/pitch after update usually reads zero. |
| `0x6B0718` through `0x6B0772`, `0x6B09CC` through `0x6B09D7` | Mouse/keyboard yaw and tilt accumulate; write input fields; dispatch rotationUpdate | Native update can rotate the same center after our pre-update write, even when native mouse rotate is suppressed. |
| `0x6B0998` through `0x6B09BB` | Update followed center position and refresh its transform | Following stays native. |
| `0x6AE0E0` through `0x6AE138` | getFacingDirection reads cached center minus camera-parent positions, clears Y, normalizes | Movement heading is horizontal and position-derived; it is not the absolute pitch or a reliable optical roll test. |
| `0x6AE160` through `0x6AE1C7` | manuallySetOrientationAndZoom assigns center orientation, lens Z position and initialized flag | Calling it every mouse frame would also take over zoom/placement. The correction uses orientation writes only. |

## Cause and corrected basis

For an upright rig with yaw Y and positive down-pitch P:

```text
centerWorld = yaw(+Y, Y) * pitch(+X, P)
viewForward = centerWorld * (0, 0, +1)
viewUp      = centerWorld * (0, +1, 0)
lensOffset  = centerWorld * (0, 0, -radius)
absoluteDownPitch = atan2(-viewForward.y, length(viewForward.xz))
```

At +30 degrees, the camera looks downward with direction Y = -0.5 and sits
above the pivot with radial offset Y = +radius/2. A positive local pitch
delta **decreases** view elevation. The removed normalLookPitchDelta used
`elevation + delta`, reversing that relationship. After crossing vertical,
`asin(direction.y)` folds the angle and does not reveal an upside-down up vector.
Incremental rotations could therefore continue on an invalid center quaternion.
F10 OFF previously performed no orientation repair.

The correction measures center orientation directly, clamps absolute positive
down-pitch to 10..75 degrees before writing, and normalizes a fresh yaw*pitch
quaternion. It converts world orientation to center-parent space when needed.
Derived optical direction/up are checked independently against the native basis,
including world-up inversion and roll. A corrupt optical basis can be restored
to native identity lens orientation plus the camera's +Z-looking orientation;
positions and attachment are preserved. Nonfinite/inverted poses use a normal
30-degree down-pitch. This pitch range prevents a pitch-induced underside view;
terrain and object intersection remain the native collision system's job.

The native mouse rotate flag is suppressed while normal look owns relative input.
Native keyboard yaw/tilt, follow, wheel zoom and collision still run. We check
immediately after native update and at hook exit, after later plugin restores.
The follow-wake watchdog also now preserves actual center orientation rather
than rebuilding it from zeroed input deltas. FP/OTS detached drivers and their
quaternion writes remain outside normal-look ownership.

## Validation and limits

Run `uv run --no-project python tests/run_normal_camera_tests.py`. The runner
extracts the production helpers and controllers verbatim and compiles with VC++
2010 x64. The fixture now models quaternion multiplication, normalized axes,
parent-space conversion, hierarchical positions, -Z lens placement and +Z camera
optics. This replaces the old fixed lens position / scalar pitch-counter fake
that could not detect the sign error or inversion.

Coverage includes 3,200 extreme mixed-input frames across all F6/NormalAutoAlign
combinations, reversal at each pitch boundary, 12,000 horizontal-only frames
(over six full turns per combination), actual-orientation recapture after ALT,
focus/load/mode transitions, native late tilt, final-hook late writes, displaced
lens positions, nonidentity parents, negative/vertical/overhead/inverted pitch,
roll, invalid optical basis, nonfinite/zero quaternions, F10/V exit repair, and
rate-limited diagnostic content. Existing controls/steering/gate tests still run.

Build with:

```powershell
& 'C:\Windows\Microsoft.NET\Framework64\v4.0.30319\MSBuild.exe' WASDCombatPlugin.sln /t:Build /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=Windows7.1SDK /v:minimal /nologo
git diff --check
```

No game was started or controlled, no DLL was deployed to Kenshi, and no commit
was made for this correction. Controller tests cannot confirm actual Ogre scene
scheduling, rendering or collision behavior. The user has confirmed the earlier
normal F10 activation fix; the pitch correction has **not** been playtested.

Required playtest: repeated fast mouse-up/down and full orbits with F6 off/on;
ALT selection and recapture; F10 off/on; V, OTS and FP transitions; native tilt
keys; wheel zoom, tracking and terrain/interior collision; pause/UI/focus changes;
save loading; recovery of an already-invalid view. Enable CameraDiagnostics to
collect dc_normal_pitch (radians, recovery bits 1=pre, 2=post/final, 4=release,
8=inversion/nonfinite detection) with actual derived direction/up/quaternion.
