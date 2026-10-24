"""Compile the actual camera controller functions against a small fake game API.

Run: uv run --no-project python tests/run_normal_camera_tests.py
Requires the same VC++ 2010 x64 compiler and Windows 7.1 SDK as the plugin.
No game process is started and no installed mod files are written.
"""
from pathlib import Path
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "x64" / "Release" / "normal-camera-tests"
OUT.mkdir(parents=True, exist_ok=True)
source = (ROOT / "WASDCombatPlugin.cpp").read_text(encoding="utf-8-sig")

def function(name):
    match = re.search(r"^static [^\n]+\b" + name + r"\([^\n]*\)\n\{.*?^\}",
                      source, re.M | re.S)
    if not match:
        raise RuntimeError("Production function not found: " + name)
    return match.group(0)

state = source.split("// Normal V-only camera:", 1)[1].split(
    "static void normalResetAutoAlign()", 1)[0]
state = state[state.index("static bool"):]
names = ["otsWrapYaw", "normalResetAutoAlign", "normalMovementReferenceYaw",
         "computeWASDDirection", "steeringAdvanceYaw", "normalCameraYaw", "normalCameraGates",
         "normalLookSmooth", "normalLookClampPitch", "normalLookQuaternionValid",
         "normalLookReadPose", "normalLookPoseSafe", "normalLookAttached", "normalLookWritePose",
         "normalLookRecover", "normalLookPitchDiagnostic", "normalMouseLookAfterUpdate", "normalCaptureDiagnostic",
         "normalYawWriteDiagnostic", "normalReleaseMouseLook",
         "normalMouseLookBeforeUpdate", "normalAutoAlignBeforeUpdate",
         "normalAutoAlignAfterUpdate", "cameraToggleRequests", "steeringFrame", "readCameraFloat"]
(OUT / "normal_camera_under_test.h").write_text(
    state + "\n" + "\n\n".join(function(name) for name in names), encoding="utf-8")

vc = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / "Microsoft Visual Studio 10.0" / "VC"
sdk = Path(os.environ.get("ProgramFiles", r"C:\Program Files")) / "Microsoft SDKs" / "Windows" / "v7.1"
env = os.environ.copy()
env["INCLUDE"] = str(vc / "include") + ";" + str(sdk / "Include")
env["LIB"] = str(vc / "lib" / "amd64") + ";" + str(sdk / "Lib" / "x64")
compiler = vc / "bin" / "amd64" / "cl.exe"
subprocess.run([str(compiler), "/nologo", "/W3", "/EHsc", "/MD", "/O2",
                "/I" + str(OUT), str(ROOT / "tests" / "normal_camera_alignment_test.cpp"),
                "/Fe" + str(OUT / "normal_camera_tests.exe"),
                "/Fo" + str(OUT / "normal_camera_tests.obj")],
               cwd=OUT, env=env, check=True)
subprocess.run([str(OUT / "normal_camera_tests.exe")], cwd=OUT, check=True)

# Both shipped and generated defaults must stay opt-in, with the same key names.
ini = (ROOT / "WASDCombatPlugin.ini").read_text(encoding="utf-8-sig")
for name, default in [("NormalAutoAlign", "false"), ("NormalAutoAlignSpeed", "4.0"),
                      ("NormalManualOverrideMs", "600"), ("SteeringMode", "false"),
                      ("SteeringTurnSpeed", "90.0"), ("NormalMouseLook", "false"),
                      ("NormalMouseLookSensitivity", "1.0"), ("CameraDiagnostics", "false")]:
    assert re.search(r"^" + name + r"\s*=\s*" + re.escape(default) + r"$", ini, re.M)
    assert '"' + name + ' = ' + default + r'\r\n"' in source
    assert ('"Settings", "' + name + '", "' + default + '"' in source
            or 'readCameraFloat(path, "' + name + '", "' + default + '"' in source)
for name, default in [("ToggleAutoCamera", "F6"), ("ToggleNormalMouseLook", "F10")]:
    assert re.search(r"^" + name + r"\s*=\s*" + default + r"$", ini, re.M)
    assert '"' + name + ' = ' + default + r'\r\n"' in source
# Existing layout migration must remain opt-in once per old file.
assert '"KeyLayout", 1, path) < 2' in source
# Native mouse rotate is suppressed only over the normal captured update.
hook = source.split("static void cameraUpdate_hook(", 1)[1].split("// ANY open UI", 1)[0]
assert hook.index("normalMouseLookBeforeUpdate(") < hook.index("normalAutoAlignBeforeUpdate(") < hook.index("s_cameraUpdateOrig(thisptr")
assert "if (key) key->rotate = false" in hook
assert "if (normalCapture && key) key->rotate = savedRotate" in hook
assert hook.index("s_cameraUpdateOrig(thisptr") < hook.index("~NormalLookFinalCheck() { normalMouseLookAfterUpdate(cam); }")
assert "Ogre::Quaternion qRt = thisptr->getCenterNode()->getOrientation();" in source
assert "getCenterNode()->pitch(" not in function("normalMouseLookBeforeUpdate")
# Normal look owns one atomic drain only after its eligibility gate; FP/OTS
# retain the original thread/device/accumulator. Native update remains enabled.
look = function("normalMouseLookBeforeUpdate")
assert look.count("fpTakeMouseAccum(") == 1
assert look.index("if (!allowed)") < look.index("fpTakeMouseAccum(")
assert "s_autoCameraEnabled" not in look and "s_normalAutoAlign" not in look
assert "if (s_fpRawMouse) { fpStartDInputThread(); fpEnsureDInput(); }" in look
assert "bool ctlEnabled = (s_fpActive || s_firstPersonActive) ? false : controlEnabled;" in hook
movement = source.split("bool steeringOwns = dirOk &&", 1)[1].split("// Post-original re-assert", 1)[0]
assert "steeringOwns && !dirOk" not in source
assert "thisptr->setDirectMovement(wasdDir, limit)" in movement
assert "float limit     = wasdMoveLimit(turning)" in movement
assert "s_charMovUpdateOrig(thisptr, time)" in movement
print("PASS: shipped defaults, generated defaults and INI load keys agree")
print("PASS: single normal input consumer, native update and movement integration wiring")
