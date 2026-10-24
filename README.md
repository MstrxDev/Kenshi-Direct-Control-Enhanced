# Direct Control for Kenshi

**Full WASD movement, true first- and third-person views, and hands-on manual combat for Kenshi.**

Press `V`, move with WASD, and your character responds instantly — overriding click-to-move, combat AI pathing, and squad orders. Press `P` to drop into your character's eyes, or `Ctrl` for an over-the-shoulder action camera. And with **v1.5**, press `Z` and every swing, block, and dodge is yours — timed presses on top of Kenshi's own skill system.

Built for precise, hands-on control during combat, retreats, ambushes, city navigation, and immersive exploration — without losing any of Kenshi's charm.

**Current version: v1.5.0 — Manual Combat**

## Downloads

| | |
|---|---|
| Steam Workshop | https://steamcommunity.com/sharedfiles/filedetails/?id=3737240806 |
| Nexus Mods | https://www.nexusmods.com/kenshi/mods/2017 |

## Controls

| Key | Action |
|---|---|
| `V` | Toggle Direct Control on / off |
| `W` `A` `S` `D` | Move (camera-relative) |
| `X` | Cycle speed (Walk / Jog / Run) |
| `G` | Hand control to the character under the cursor |
| Double-click portrait | Switch control via a squad portrait |
| `P` | Toggle first-person view (while Direct Control is on) |
| `Ctrl` | Toggle the third-person action camera |
| `H` | Swap the third-person camera to the other shoulder |
| `Shift`+`C` | Toggle sneak (first person or third-person camera) |
| `Alt` (hold) | Free mouse cursor in first / third person |
| `Z` | Manual combat on / off |
| `E` | Attack — one press, one swing at the enemy you point at |
| `F` | Block — hold to guard; tap as a hit lands for a perfect block |
| `Q` | Dodge — press as an attack comes in |
| Right-click an enemy | Make them your target at any distance (in combat) |

Every key can be changed in-game under **Options → Direct Control**, or in the `.ini` — see [Configuration](#configuration).

> **Keys changed in v1.5:** Block is `F`, and taking control of the character under the cursor moved from `F` to `G`. Existing configs are updated automatically, once.

## Manual Combat (new in v1.5)

Press `Z` and your character stops swinging on their own. Kenshi still picks the technique for your weapon and skill, rolls the dice, and awards the XP — you decide *when*.

- **Attack** (`E`) — one press, one swing. Out of reach? Your character steps in and swings when they get there. Pressed during a dodge, the swing fires the instant the dodge ends.
- **Block** (`F`) — **hold** to guard with your Block skill (the game's own +20 defence), for up to about a second. **Tap** just as a hit lands for a **perfect block**: stopped regardless of skill, with the parry sound and sparks — both strikes of a two-hit technique included. Perfect blocks cover hits from the front.
- **Perfect-block reward** — land a perfect block and your weapon glows blue. While it glows, your next Block press counts instantly with no cooldown, so well-timed taps can chain.
- **Dodge** (`Q`) — a timed press, rolled against your Dodge skill with a bonus for the timing. Works from any direction.
- **Aim focus** — your character fights whoever you point at: the enemy under the mouse cursor in the normal camera, or nearest the crosshair in first and third person. The camera never moves on its own. Right-click an enemy in combat to target them at any distance.
- **Enemy health at a glance** — the target's name floats above them in red, with thin bars for blood, head, chest, and stomach. Not pointing at anyone? The bars follow whoever you're fighting.
- **Weapon glint** — the moment an enemy starts a swing at you, their weapon glows, brightest just before the blow lands.
- **Getting hit matters** — a stagger plays out and nothing pressed during it counts. Accidental double-presses are forgiven.

By default your character still blocks on their own while manual combat is on; turn **"Character still blocks on their own"** off in the settings for a fully manual defence. With `Z` off, the AI fights exactly as before.

## Features

- **Instant movement override** — WASD immediately cancels click-to-move destinations and overrides combat AI pathing. No startup delay, crisp stop on release, indoors and out. Movement, speed, and camera carry straight through new map areas loading in.
- **First-person view** (`P`) — a true first-person camera at your character's eyes. Your own body and arms stay in view as you run, fight, block, and heal; your head, hair, and headgear are hidden so nothing blocks the lens. Knocked down or thrown, you see your own body in front of you. The cursor frees itself for the map, dialogue, inventory, and menus — or hold `Alt` any time.
- **Third-person action camera** (`Ctrl`) — over-the-shoulder view with a centred crosshair, mouse-look, and scroll-wheel zoom. `H` swaps shoulders. Eases clear of walls behind you, ignores your own gear, and shows the familiar cursor icon when the crosshair rests on something you can interact with.
- **Sneak in first and third person** (`Shift`+`C`) — presses the game's own SNEAK button, so stealth skill, detection, and XP work exactly as vanilla.
- **A combat camera that stays clean** — your own swings, blocks, and heals never clip through the lens; the camera eases off the steps on stairs; an enemy pressing into your face can't slice the view open.
- **Enemies keep up** — pursuers chase at full speed and attack on the move. No risk-free kiting.
- **Combat that gets out of your way** (manual combat off) — stand still and the AI fights on its own, earning XP as normal. Hold WASD and it yields so movement stays smooth. A committed animation always finishes before you move.
- **Move while you loot and trade** (optional) — turn the face-cam off to keep moving with WASD while an inventory or trade window is open. In first and third person the mouse keeps looking around; hold `Alt` to move items. Walk away from a merchant and the trade closes on its own.
- **Inventory face-cam** (on by default) — open a character's own inventory and the camera swings to face them. Auto-disabled in combat.
- **Get up and go** — hold WASD while sitting, lying in a bed, or operating a workstation and your character stands up and walks off.
- **In-game settings tab** — Options → Direct Control: keys first, then settings grouped by mode (movement, manual combat, third person, first person), each with a plain description. Changes apply instantly and save on their own.
- **Per-character control** — Direct Control follows your selected character; switch with `G` or a double-click. If your only conscious character is knocked out, Direct Control switches itself off.

## Installation

1. Install [RE_Kenshi](https://github.com/BFrizzleFoShizzle/RE_Kenshi/releases) (required mod loader).
2. Copy the `WASDCombatPlugin` folder from the release zip into `Kenshi/mods/` so you have
   `Kenshi/mods/WASDCombatPlugin/WASDCombatPlugin.dll` — keep the included `materials` folder (it carries the first-person head-hiding shader).
3. Launch the game. RE_Kenshi loads the plugin automatically.
4. **Enable only one copy.** If you have both the Workshop item and a manual install, untick one — the mod refuses to load twice and says so in `RE_Kenshi_log.txt`.

Or subscribe on the Steam Workshop and skip the manual copy.

### Requirements

- Kenshi (Steam or GOG)
- RE_Kenshi mod loader (must be installed and active)

## Configuration

Everything is easiest to change in-game under **Options → Direct Control**. For hand-editing, `WASDCombatPlugin.ini` lives in the mod folder:

- **`[Keybinds]`** — rebind any key. Valid names: letters (`W`), digits (`5`), `F1`..`F24`, `SPACE`, `TAB`, `SHIFT`, `CONTROL`, arrow keys, `NUMPAD0`..`9`, or `OEM_1`..`8` for international layouts. Sneak is always `Shift` + the SneakToggle key. Leave `KeyLayout = 2` alone — it stops the one-time v1.5 key update from running again.
- **`[Settings]`** — highlights:
  - `InventoryFaceCam = true` — camera faces your character on inventory. `false` = keep moving while looting/trading.
  - `WasdSpeedCap = true` / `WasdSpeedMult = 1.0` — cap WASD speed at the character's real top speed (injuries, encumbrance, shackles), and scale it.
  - `EnemyPursuit = true` — enemies chase and hit a moving player.
  - `ManualBlock = true` / `AiBlocksInManual = true` — manual block and dodge keys; whether the character still blocks on their own in manual combat.
  - `BlockHoldMs = 1200`, `BlockCooldownMs = 800` — longest a held block lasts, and the wait before it counts again.
  - `PerfectBlockWindowMs = 1000`, `PerfectBlockRewardMs = 1200` — perfect-block window after a tap, and how long the next press is free afterwards.
  - `DodgeWindowMs = 600`, `DodgeSkillBonus = 20` — dodge timing and the bonus for a timed press.
  - `AimNameTag = true`, `WeaponGlint = true` — enemy name/health bars and the incoming-swing glint.
  - `VerboseLog = false` — detailed log for bug reports.
- **`[FirstPerson]`**
  - `Sensitivity = 1.0`, `FOV = 75` (50–110).
  - `NearClip = 1.0` — clipping distance: lower shows more of your body, higher calms distant grass shimmer when you turn.
  - `HeadPixelHide = 1`, `HideHeadgear = 1`, `HideHair = 1` — hide your own head, headgear, and hair. `HideHead = 0` is a legacy head hide for custom races.
  - `FloorRevealBelow = 1` — render the storeys below you inside multi-floor buildings.

## Steering, automatic camera and normal mouse-look

The new controls are opt-in. Add these entries to the **existing** sections in
`WASDCombatPlugin.ini` next to the DLL. These are the shipped defaults:

```ini
[Keybinds]
ToggleAutoCamera = F6
ToggleNormalMouseLook = F10

[Settings]
SteeringMode = false
SteeringTurnSpeed = 90.0
NormalAutoAlign = false
NormalAutoAlignSpeed = 4.0
NormalManualOverrideMs = 600
NormalMouseLook = false
NormalMouseLookSensitivity = 1.0
OtsAutoAlign = false
OtsAutoAlignSpeed = 4.0
OtsMouseOverrideMs = 600
CameraDiagnostics = false
```

For character steering with automatic camera follow in both third-person views,
set `SteeringMode`, `NormalAutoAlign` and `OtsAutoAlign` to `true`, then restart.
Camera/steering tuning is INI-only. The two hotkeys also appear under
**Options > Direct Control**, can be rebound there without restarting, and save
to the existing `[Keybinds]` section. Existing custom movement keys are used by
role, including `KeyLayout=2`; the new roles do not change that layout.

**F6** toggles a session-wide automatic-camera master while Direct Control is
active. Each view still respects its own `NormalAutoAlign` / `OtsAutoAlign`
setting: the hotkey cannot enable a mode disabled in the INI. An in-game message
and log line report the new state. Manual rotation, mouse-look and movement
continue with the master off. The held-key movement reference is retained across
the toggle. The master starts on at launch; runtime changes do not rewrite the
mode settings or persist across restarts.

F7 was rejected because it is Kenshi's `toggle_bar` key; F8 is `screenshot`.
The installed `controls.cfg` had no plain F6/F10 binding when checked. Ctrl+F6
is a separate vanilla developer command, so the new toggles ignore Shift/Ctrl/Alt
combinations. Check other mods and your own bindings when choosing replacements;
the existing duplicate-DC-key warning also covers these two roles.

With **`SteeringMode=true`**, your configured forward/backward keys move along
an independent character heading. Left/right alone move forward along a
continuously turning heading; forward+left/right also makes a curved path.
Backward reverses velocity. Opposing forward/backward inputs cancel movement
and turning; left+right cancel turning (and movement when pressed alone). Camera rotation never supplies
the steering yaw. `SteeringTurnSpeed` is degrees per real second (10–360,
default 90). It advances once per world frame; stalls longer than 250 ms and
resume frames do not accumulate catch-up turns. First person retains its
existing gaze-relative movement.

Left/right alone use the same `setDirectMovement()` path, speed limits, native
locomotion update and accompanying `faceDirection()` request as forward movement.
The former stationary-turn path has been removed. Visible curved movement,
backward animation and collision still need testing on actual characters/races
in Kenshi. Committed
actions and protected states retain control of the body. Crawling, leaving
furniture and slave-order locomotion use the established camera-relative path
instead of forcing standing steering into those special cases.

While steering keys are held, automatic follow prefers the steering heading,
including curved A/D-only movement and backward travel, so S does not ask the camera
to flip 180 degrees. Otherwise it uses validated measured horizontal travel,
including point-and-click travel, and stops rotating when travel stops.
`NormalAutoAlignSpeed` and `OtsAutoAlignSpeed` are exponential turn response per
second (0.1–20). Manual mouse/keyboard rotation takes priority, with the configured
override delays (0–10000 ms). In strafe mode, auto-follow retains the original
held-gesture movement reference; releasing and pressing again adopts the current
view, and intentional manual yaw updates the reference. `SteeringMode=false`
retains that original strafe behavior.

**F10** toggles mouse-look only in normal V-only gameplay. `NormalMouseLook`
sets its launch preference; runtime changes last for the session. The existing
Kenshi orbit stays attached to the followed character. Relative input is smoothed
with a short time-based response; `NormalMouseLookSensitivity` is 0.1–5. The
normal orbit pitch has an absolute hard clamp at 10–75 degrees below horizontal.
Capture repairs an out-of-range or inverted pose to an upright native basis.
Wheel zoom, tracking,
native terrain processing and interaction commands remain active.

Hold **ALT** for a regular pointer and vanilla selection/click behavior. Normal
mouse-look always releases for inventory (including move-through inventory),
menus, map/stats, dialogue, context menus, pause, focus loss, loading and mode
switches. Release restores the saved pointer location when the game is focused.
Returning to eligible normal gameplay resumes only if the preference is still
on. Entry/resume, owner/focus changes and raw-input acquisition discard a baseline
sample, avoiding recenter/backlog turns. FP and detached OTS keep their existing
mouse consumers; the normal mode does not consume their accumulator samples.

### Normal mouse-look pitch correction

Normal F10 activation and mouse response have been confirmed in-game. The
subsequent inverted/under-ground pitch bug is corrected in source and regression
tests; **this pitch correction still needs an in-game playtest**.

The native attached rig is `center -> lens node -> Ogre camera`. The center owns
both world-Y yaw and local-X pitch. The lens sits on local **-Z**, while the
camera's local orientation looks along **+Z**. Positive center pitch therefore
means looking **down**. The old code clamped negative view elevation as though
adding a local pitch rotation increased that elevation. Its sign was reversed;
`asin(direction.y)` also folded angles after vertical and could not identify an
inverted up vector. Once corrupted, F10 OFF merely stopped input and left the
bad quaternion behind. A position-based elevation sample could additionally be
affected by native collision/zoom displacement.

Normal look now reads actual derived center orientation each frame, measures
positive down-pitch with `atan2`, clamps the absolute target before any rotation,
and writes normalized `worldYaw * localPitch` with zero roll. It converts back
through the center parent's orientation, preserving an upright world basis even
under a rotated parent. No accumulated pitch offset is kept. Entry, ALT release,
load/mode transitions and focus/raw-input changes still discard the first mouse
sample. Saturating a pitch boundary clears pending vertical smoothing so reversal
responds immediately.

Recovery checks run before native update, immediately after it, and at hook exit
after later restore/watchdog writes. They repair invalid/inverted poses on the
current attached player rig, including F10/V exit; they never dereference a saved
camera pointer during loading or take over detached FP/OTS. The normal 30-degree
constructor pitch is the inversion/nonfinite fallback. Recovery restores native
lens/camera orientations only if their optical basis is corrupt; native positions,
zoom, follow, collision and FOV remain owned by Kenshi. The existing follow-wake
path now reuses the center's actual quaternion instead of interpreting the
zeroed `CameraClass::yaw/pitch` input deltas as an absolute pose.

With `CameraDiagnostics=true`, `dc_normal_pitch` adds a sample at most once per
second, independent of `VerboseLog`. It includes mouse dx/dy, native input and
remaining yaw/pitch deltas, absolute pitch before/requested/clamped, derived rig
yaw/pitch, actual camera quaternion/direction/world-up, roll, validity and
inversion/recovery flags. Angles are radians; native delta pairs are yaw,pitch.
`worldUp` is the camera's up vector in world coordinates and `upY` is its dot
product with world +Y. Recovery bits are `0x1` pre-native, `0x2` post-native/final,
`0x4` release and `0x8` inversion/nonfinite detection; events stay latched until
the next diagnostic sample. The final sample reports the corrected orientation.

Native RVAs, orientation derivation, remaining playtest checks and regression
coverage are recorded in [tests/normal_camera_native_rig.md](tests/normal_camera_native_rig.md).

### NormalAutoAlign investigation and runtime diagnostics

The real runtime log now identifies a persistent `gate=0x400`, with normal
mouse-look enabled but `captured=0`, and the native structure `parent == node`
with a distinct center. In the failing source, `0x400` was exactly:

```cpp
if (!cam->isInitialised() || !cam->terrainLoaded || cam->isFreeCameraMode())
    gates |= 1024;
```

The attachment mismatch was a separate `0x800` bit, set when `parent != node`.
The logged equal pointers therefore PASSED that ownership check. Both normal
mouse capture and auto-alignment returned before consuming input/sampling travel
because they required `normalCameraGates() == 0`.

Read-only inspection of `KenshiLib/Include/kenshi/CameraClass.h` and the original
1.0.65 executable retained in `RE_Kenshi/kenshi_GOG_x64.exe` establishes:

| Native evidence (RVA) | Meaning |
| --- | --- |
| Constructor `0x6AF44E`: clears the word at `this + 0x20` | Both `initialised` and `terrainLoaded` start false, although the constructor creates the rig. |
| Constructor `0x6AF599` through `0x6AF60F` | Creates the lens node under the center, stores it at `+0x70`, and calls `Ogre::SceneNode::attachObject(node, camera)`. `parent == node` is the native attachment. |
| `isInitialised`, `0xA19880` | Reads only byte `+0x20`; it does not inspect scene nodes or camera attachment. |
| `manuallySetOrientationAndZoom`, `0x6AE1C7` | Sets `initialised` after applying a pose. This flag cannot prove scene lifetime. |
| `isFreeCameraMode`, `0x6ADD90` | Reads the independent free-camera flag at `+0xBF`. |
| Native `update`, `0x6B05E0` | Normal following/rotation does not require the two readiness flags; free-camera mode branches to a different update. |

The fix removes the two unsupported readiness requirements. It retains all
world/loading, player-camera, character liveness/movement ownership, UI/focus,
follow-target and explicit free-camera gates. Structural validation requires
the native `center -> node -> camera` chain, non-null distinct center/lens nodes,
and the correct lens parent. It continues rejecting detached/foreign rigs.

The old combined log alone does **not** identify which boolean produced `0x400`.
A read-only snapshot of the already running game on 2026-10-24 resolved that
uncertainty: `initialised=1`, **`terrainLoaded=0`**, `freeCameraMode=0`. It followed
the exported `ou` pointer through `GameWorld::player` (`+0x580`) and
`PlayerInterface::camera` (`+0x30`), then read the header-defined camera fields.
The native center (`0x53D13660`) and lens node (`0x53D1CFB0`) matched the current
session's earlier normal-mode `gate=0x400` log samples. OTS was active at snapshot
time; no mode, input, pointer or camera state was changed to collect it. This
demonstrates the bad `!terrainLoaded` operand on the same gameplay rig; it is not
a playtest of the rebuilt DLL. The new sample also exposes `init`, `terrain` and
`free` separately. That snapshot did not itself validate the rebuilt DLL. The
user has since confirmed normal F10 activation and mouse movement; the newer
pitch correction above remains untested in-game. The build and regression runner
write to the project workspace only.

Normal look uses the existing OTS/FP DirectInput device, polling thread and atomic
accumulator, respecting `[FirstPerson] RawMouse` and the same cursor-warp fallback.
It consumes one sample only when normal mode owns input, discards entry/resume
samples, and uses the shared crosshair/cursor lifecycle. It does not detach or
change camera zoom, collision, tracking, FOV or perspective. F6 affects automatic
following only; mouse-look also works with `NormalAutoAlign=false`.

To collect evidence after installing a test build yourself:

1. Set `[Settings] CameraDiagnostics=true`, `NormalAutoAlign=true`, and initially
   leave `NormalMouseLook=false` and `SteeringMode=false`. Restart the game.
2. Enter V-only control, move in several directions for 10–15 seconds, release
   movement, then test manual rotation. Repeat with steering enabled in a second
   run, including A/D alone and S. Use F6 to test follow off/on.
3. Copy `RE_Kenshi_log.txt` from the Kenshi directory after exiting. Include
   `dc_camera_config`, `dc_camera_controls`, `dc_normal_camera`,
   `dc_camera_guard`, `dc_normal_capture`, `dc_normal_yaw_write` and the toggle
   lines. Turn diagnostics off afterward.

Diagnostics are independent of `VerboseLog`. Each of camera samples, capture
transitions and yaw-write observations is limited to one line per second, plus
startup configuration and toggle events. The startup line gives
the exact loaded INI path and resolved values. Runtime samples include the
configuration/master, control/UI gates, cursor/raw state, manual override time,
sample dt/speed, requested yaw, yaw immediately after the write, yaw across native
update, rendered view yaw, steering heading, and camera/anchor ownership pointers.
`written` is reset when there is no auto write. `dc_normal_yaw_write` identifies
`source=mouse` or `source=auto` and reports the requested delta, measured headings
before/after the write and their wrapped `actual` delta. Yaws/deltas are radians.

The `gate` mask adds these reasons (hex): `0x1` inactive mode/load/world guard;
`0x2` non-player camera; `0x4` paused; `0x8` engine control disabled; `0x10`
UI/inventory; `0x20` plugin UI/turret suspension; `0x40` background; `0x80` anchor
not live; `0x100` movement mismatch; `0x200` incapacitated/protected character;
`0x400` explicit free-camera mode; `0x800` missing/aliased nodes or rig/parent mismatch; `0x1000` follow
target mismatch. A stuck load guard has its own pointer-safe heartbeat. Preview
cameras do not reset the player's history or generate normal sample lines.

### Verification

Focused regression tests extract the production controllers and compile them
against fake input/game/Ogre APIs using VC++ 2010 x64:

```powershell
uv run --no-project python tests/run_normal_camera_tests.py
```

They cover the observed `parent == node != center` structure with all four
readiness-flag combinations (including both false), invalid/detached rigs,
steering time scaling and camera independence, nonzero translation and turn for
A/D-only and W+A/D arcs, backwards/cancelled intent, original strafe behavior,
tuning validation, F10 capture/release, F6 and auto-align-setting independence,
raw/fallback handoffs, ALT/UI/focus release/resume, accumulator ownership,
rate-limited capture/yaw diagnostics, normal alignment/manual priority,
pitch/smoothing, discontinuities, liveness, UI/load/focus gates, and
shipped/generated INI defaults. These are controller regressions, not evidence of
actual Ogre rendering, character animation or collision behavior in the game.
The camera fixture now performs quaternion and hierarchical position transforms,
using native -Z lens placement and +Z optics. Pitch regressions exercise repeated
million-pixel input in both directions, more than six full yaw turns at fixed
pitch, all F6/auto-align combinations, baseline recapture, collision-offset and
parent-space independence, post-native/final recovery, F10/V exit, nonfinite and
rolled optics, and rate-limited pitch diagnostics.

In-game checks remain required: V-only/OTS curved runs, S and A/D-only arcs,
custom movement/camera keys, mouse-look/ALT selection and loot, wheel zoom and
terrain/interior collision, map/dialogue/inventory/pause, focus/minimize/restore,
character changes/save loading, first person, and existing manual combat.

## Compatibility & Notes

- Tested with RE_Kenshi 0.3.5 (Kenshi 1.0.65). No game data files are modified on disk, so it is safe alongside large mod lists and most combat, AI, and faction mods.
- Manual combat uses the game's own techniques, skill rolls, and XP. Nothing is added to your character's numbers except the timed-dodge bonus (`DodgeSkillBonus`) and the game's own block-mode bonus while you hold Block.
- The mod ships a copy of the character shader (`materials/deferred/character.hlsl`) with a small addition for first-person head hiding; it behaves identically to vanilla outside first person. Another mod replacing the same file may conflict — whichever loads last wins, and the plugin falls back to a legacy head shrink.
- **Character Highlight** also replaces that shader: install the separate *Character Highlight Compatibility Patch*, enable it, and put it **last** in your load order.
- Direct Control drives one character at a time — the selected one. Other squad members follow their normal orders.
- With manual combat off you cannot attack while actively moving — stop, and the AI fights on its own.
- During knockdown, stagger, or get-up animations the character cannot be moved; movement resumes when the animation completes.
- Downed or crippled characters can still be moved using the game's crawl / limp system.
- While manning a turret, aiming stays vanilla until you press WASD, which steps you off the turret.
- First person: distant grass can still shimmer slightly when you turn — the `NearClip` slider trades a calmer horizon against seeing more of your own body.

## Changelog

### v1.5.0 — Manual Combat
**New**
- Manual combat (`Z`): Attack (`E`), Block (`F`) with timed perfect blocks and the blue-glow reward, Dodge (`Q`). Staggers play through, double-presses are forgiven, an Attack pressed during a dodge fires when it ends.
- Aim focus with enemy name and blood / head / chest / stomach bars; right-click to target at any distance; bars follow your opponent when you aren't pointing at anyone.
- Weapon glint on incoming swings.
- Hold `Alt` for a free cursor in first and third person.
- Settings tab reorganised by mode with a plain description on every row; "Detailed log for bug reports" option.

**Keys changed**
- Block is `F`; taking control of the character under the cursor moved from `F` to `G`. Existing configs update automatically.

**Improved**
- First person: much less grass shimmer when turning (`NearClip` default 1.0, adjustable).
- First person: knocked down, ragdolled, or getting up, you see your own body instead of through it.
- Moving with an inventory open keeps mouse-look in first and third person.
- Crossing map areas no longer stops your character, drops your speed, or detaches the first- or third-person camera.
- Direct Control switches off if your only conscious character is knocked out.
- Sneak works in the third-person camera too.

**Fixed**
- The world map arrow pointed the wrong way in first person.
- A queued heal job could keep your character walking after you let go of the keys.
- `F2` (and keys sharing a slot with a Direct Control key) stopped working while Direct Control was on.
- The 180-degree spin some characters did when stopping.
- Quick-loading mid-fight could freeze on the loading screen.
- Two copies of the mod loaded at once scrambled the first-person view — the mod now refuses to load twice.

### v1.4.1
- The crosshair shows the familiar cursor icon when aiming at something you can interact with.
- Fixed a crash while loading a save alongside the Character Highlight mod (separate compatibility patch).
- Third person: area sounds follow your character again; exiting the camera after a long trip no longer flips the view.
- First person: hills no longer sink the camera into your armor.
- Slavery: WASD behaves like a right-click move order, so guards react by the game's own rules.
- The camera follows your character when knocked down, unconscious, ragdolled, or carried.
- New "Hide head (legacy shrink)" toggle for custom race mods.

### v1.4.0
- In-game settings tab, third-person action camera (`Ctrl`), fully headless first person, real enemy pursuit while you move, and camera defaults retuned for the headless view.

### v1.3.1
- Fixed a crash when right-clicking while controlling a pack animal inside a hive home.
- Move-while-looting mode: pausing the game while a loot or trade window is open now works properly.
- First person: buildings now look solid from the outside.

### v1.3.0
- First-person view (`P`) with smooth mouse-look, WASD movement, and your own body and arms visible.
- Sneak from first person with `Shift`+`C`.
- Combat camera polish, correct multi-floor interiors, automatic cursor release for menus, and first person surviving area loads and character switches.

### v1.2.2
- `F` hands control to the selected squad member (rebindable) — moved to `G` in v1.5.
- `InventoryFaceCam = false` move-through mode.
- Keybind and settings INI added.

### Earlier versions
See the Nexus Mods changelog for the full history.

## Building from Source

The plugin is a single C++ source file built as an x64 DLL against the RE_Kenshi plugin API.

1. Open `WASDCombatPlugin.sln` in Visual Studio (Desktop development with C++ workload).
2. The project expects [KenshiLib](https://github.com/KenshiReclaimer/KenshiLib/) headers and libraries — either set the `KENSHILIB_DIR` environment variable, or place the [KenshiLib example dependencies](https://github.com/BFrizzleFoShizzle/KenshiLib_Examples_deps) (KenshiLib, Ogre, MyGUI, Boost 1.60) in a sibling `KenshiLib_Examples_deps` folder as referenced by the `.vcxproj`.
3. Build **Release | x64**. Put the output `WASDCombatPlugin.dll` in `Kenshi/mods/WASDCombatPlugin/` alongside the `.ini`, `.mod`, `RE_Kenshi.json`, and the `materials` folder.

The established command-line build uses VC++ 2010 with the Windows 7.1 SDK:

```powershell
& 'C:\Windows\Microsoft.NET\Framework64\v4.0.30319\MSBuild.exe' WASDCombatPlugin.sln /t:Build /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=Windows7.1SDK /v:minimal /nologo
```

This produces `WASDCombatPlugin/WASDCombatPlugin.dll` inside the development
repository. The build does not deploy it to an installed mod.

## License

[GPL-3.0](LICENSE)

---

*Developed as "WASDCombatPlugin" using the RE_Kenshi SDK.*
