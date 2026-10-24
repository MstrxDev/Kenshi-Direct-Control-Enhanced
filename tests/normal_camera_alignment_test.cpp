// The runner extracts production functions verbatim; these fakes isolate their
// state/input decisions. The quaternion rig below models native +Z optics and
// hierarchical transforms; actual Ogre rendering/collision still need playtesting.
#include <cmath>
#include <float.h>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <cstring>
#include <cctype>
typedef unsigned long DWORD;
typedef long LONG;
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
struct LARGE_INTEGER { LONGLONG QuadPart; };
struct POINT { long x, y; };
struct RECT { long left, top, right, bottom; };
typedef int HWND;
enum { VK_MENU=18 };
static bool altDown=false;
static short GetAsyncKeyState(int) { return altDown ? (short)0x8000 : 0; }
static POINT pointerPos={120,240};
static HWND GetForegroundWindow() { return 1; }
static bool GetClientRect(HWND, RECT* r) { r->left=r->top=0; r->right=1000; r->bottom=800; return true; }
static bool ScreenToClient(HWND, POINT*) { return true; }
static bool ClientToScreen(HWND, POINT*) { return true; }
static bool GetCursorPos(POINT* p) { *p=pointerPos; return true; }
static bool SetCursorPos(int x,int y) { pointerPos.x=x; pointerPos.y=y; return true; }
static LONG InterlockedExchange(volatile LONG* p, LONG v) { LONG old=*p; *p=v; return old; }
static int captureLogs=0, writeLogs=0, pitchLogs=0;
static char lastPitchLog[1024];
static char lastWriteLog[256];
static void DebugLog(const char* msg) {
    if (strstr(msg,"dc_normal_capture captured=1")) ++captureLogs;
    if (strstr(msg,"dc_normal_yaw_write")) { ++writeLogs; strcpy_s(lastWriteLog,msg); }
    if (strstr(msg,"dc_normal_pitch")) { ++pitchLogs; strcpy_s(lastPitchLog,msg); }
}
namespace MyGUI {
struct InputManager {
    static InputManager* getInstancePtr() { static InputManager m; return &m; }
    void injectMouseMove(int,int,int) {}
};
}
static bool pointerCaptured=false;
static void dcUpdateCrosshair(bool want) { pointerCaptured=want; }
static bool s_diReady=true;
static bool s_fpRawMouse=true;
static float mouseX=0,mouseY=0;
static int mouseConsumes=0, inputEnsures=0, inputStarts=0;
static void fpEnsureDInput() { ++inputEnsures; }
static void fpStartDInputThread() { ++inputStarts; }
static void fpTakeMouseAccum(float* x,float* y) { ++mouseConsumes; *x=mouseX; *y=mouseY; mouseX=mouseY=0; }
static const float FP_RAD_PER_PIXEL=0.003f;
static const char* iniFloatValue=0;
static void GetPrivateProfileStringA(const char*,const char*,const char* fallback,char* out,size_t cap,const char*) {
    strncpy_s(out,cap,iniFloatValue?iniFloatValue:fallback,_TRUNCATE);
}
static LONGLONG clockTick = 1000000;
static LONGLONG qpcNow() { return clockTick; }
static bool QueryPerformanceFrequency(LARGE_INTEGER* f) { f->QuadPart = 1000000; return true; }
static ULONGLONG GetTickCount64() { return clockTick / 1000; }
namespace Ogre {
struct Vector3 {
    float x, y, z;
    Vector3(float a=0, float b=0, float c=0): x(a), y(b), z(c) {}
    Vector3 operator-(const Vector3& b) const { return Vector3(x-b.x,y-b.y,z-b.z); }
    Vector3 operator+(const Vector3& b) const { return Vector3(x+b.x,y+b.y,z+b.z); }
    Vector3& operator+=(const Vector3& b) { x+=b.x; y+=b.y; z+=b.z; return *this; }
    Vector3& operator-=(const Vector3& b) { x-=b.x; y-=b.y; z-=b.z; return *this; }
    Vector3& operator/=(float n) { x/=n; y/=n; z/=n; return *this; }
    Vector3 operator/(float n) const { return Vector3(x/n,y/n,z/n); }
    float squaredLength() const { return x*x+y*y+z*z; }
    float length() const { return sqrtf(squaredLength()); }
    float dotProduct(const Vector3& b) const { return x*b.x+y*b.y+z*b.z; }
    static const Vector3 ZERO, UNIT_X, UNIT_Y, UNIT_Z;
};
const Vector3 Vector3::ZERO;
const Vector3 Vector3::UNIT_X(1,0,0), Vector3::UNIT_Y(0,1,0), Vector3::UNIT_Z(0,0,1);
struct Radian { float value; explicit Radian(float f): value(f) {} };
struct Quaternion {
    float w,x,y,z;
    Quaternion(float a=1,float b=0,float c=0,float d=0):w(a),x(b),y(c),z(d) {}
    Quaternion(Radian a,const Vector3& axis):w(cosf(a.value/2)),x(axis.x*sinf(a.value/2)),
        y(axis.y*sinf(a.value/2)),z(axis.z*sinf(a.value/2)) {}
    Quaternion operator*(const Quaternion& b) const {
        return Quaternion(w*b.w-x*b.x-y*b.y-z*b.z, w*b.x+x*b.w+y*b.z-z*b.y,
            w*b.y+y*b.w+z*b.x-x*b.z, w*b.z+z*b.w+x*b.y-y*b.x);
    }
    float Norm() const { return w*w+x*x+y*y+z*z; }
    void normalise() { float n=sqrtf(Norm()); w/=n;x/=n;y/=n;z/=n; }
    Quaternion Inverse() const { float n=Norm(); return Quaternion(w/n,-x/n,-y/n,-z/n); }
    Vector3 operator*(const Vector3& v) const {
        Quaternion p=(*this)*Quaternion(0,v.x,v.y,v.z)*Inverse(); return Vector3(p.x,p.y,p.z);
    }
    Vector3 xAxis() const { return (*this)*Vector3::UNIT_X; }
    Vector3 yAxis() const { return (*this)*Vector3::UNIT_Y; }
    Vector3 zAxis() const { return (*this)*Vector3::UNIT_Z; }
    static const Quaternion IDENTITY;
};
const Quaternion Quaternion::IDENTITY;
struct Node { enum TransformSpace { TS_LOCAL, TS_PARENT, TS_WORLD }; };
struct SceneNode {
    Quaternion orientation;
    Vector3 position;
    int turns;
    SceneNode* parent;
    SceneNode(): turns(0), parent(0) {}
    SceneNode* getParentSceneNode() { return parent; }
    Quaternion getOrientation() { return orientation; }
    void setOrientation(Quaternion q) { orientation=q; }
    Quaternion _getDerivedOrientationUpdated() { return parent ? parent->_getDerivedOrientationUpdated()*orientation : orientation; }
    void yaw(Radian r, Node::TransformSpace space) {
        if (space != Node::TS_WORLD) abort();
        Quaternion p=parent?parent->_getDerivedOrientationUpdated():Quaternion();
        orientation=p.Inverse()*Quaternion(r,Vector3::UNIT_Y)*p*orientation;
        orientation.normalise(); ++turns;
    }
    void _getFullTransformUpdated() {}
    Vector3 _getDerivedPositionUpdated() {
        return parent ? parent->_getDerivedPositionUpdated()+parent->_getDerivedOrientationUpdated()*position : position;
    }
    void pitch(Radian r, Node::TransformSpace space) {
        if(space!=Node::TS_LOCAL) abort(); orientation=orientation*Quaternion(r,Vector3::UNIT_X); orientation.normalise();
    }
};
struct Camera {
    Quaternion orientation;
    SceneNode* parent;
    SceneNode* getParentSceneNode() { return parent; }
    void setOrientation(Quaternion q) { orientation=q; }
    Quaternion getDerivedOrientation() { return parent->_getDerivedOrientationUpdated()*orientation; }
    Vector3 getDerivedDirection() { return getDerivedOrientation()*Vector3(0,0,-1); }
    Vector3 getDerivedUp() { return getDerivedOrientation().yAxis(); }
};
}
struct CharMovement {
    Ogre::Vector3 pos, facing;
    CharMovement(): facing(0,0,-1) {}
    Ogre::Vector3 getFacingDirection() { return facing; }
};
struct Character {
    CharMovement* movement;
    bool dead, unconscious, protectedAnim, ragdoll, carried, furniture, committed, downed;
    Character(): movement(0), dead(false), unconscious(false), protectedAnim(false), ragdoll(false), carried(false), furniture(false), committed(false), downed(false) {}
    bool isDead() { return dead; } bool isUnconcious() { return unconscious; }
    bool isRagdoll() { return ragdoll; } bool isBeingCarried() { return carried; }
};
struct CameraClass {
    Ogre::SceneNode center, lens;
    Ogre::SceneNode* centerPtr; Ogre::SceneNode* nodePtr;
    Ogre::Camera realCamera; Ogre::Camera* camera;
    bool isRotating, terrainLoaded, initialized, freeCamera;
    Character* follow;
    float yaw, pitch, altitude; // transient native inputs / zoom sentinels
    // The actual native constructor creates the attached rig with BOTH flags
    // false. The previous fixture set both true and masked the runtime failure.
    CameraClass(): centerPtr(&center), nodePtr(&lens), camera(&realCamera), isRotating(false), terrainLoaded(false),
        initialized(false), freeCamera(false), follow(0), yaw(0), pitch(0.7f), altitude(50) {
        realCamera.parent=&lens; lens.parent=&center; lens.position=Ogre::Vector3(0,0,-150);
        realCamera.orientation=Ogre::Quaternion(Ogre::Radian(3.14159265f),Ogre::Vector3::UNIT_Y);
    }
    Ogre::Vector3 getFacingDirection() {
        Ogre::Vector3 v=center._getDerivedPositionUpdated()-lens._getDerivedPositionUpdated();
        v.y=0; return v/v.length();
    }
    Ogre::SceneNode* getCenterNode() { return centerPtr; }
    Ogre::SceneNode* getCameraNode() { return nodePtr; }
    bool isInitialised() { return initialized; } bool isFreeCameraMode() { return freeCamera; }
    Character* getFollowObject() { return follow; }
};
struct Player { CameraClass* camera; };
struct World {
    Player* player; bool loading, paused; int messages;
    bool isLoadingFromASaveGame() { return loading; } bool isPaused() { return paused; }
    void showPlayerAMessage(const char*,bool) { ++messages; }
};
struct Gui { bool inventory; bool isAnyInventoryWindowOpen() { return inventory; } };
struct Keys { bool rotate, rotLeft, rotRight, tiltUp, tiltDown; };
static World world; static World* ou=&world;
static Player player; static Gui guiObject; static Gui* gui=&guiObject;
static Keys keys; static Keys* key=&keys;
static CameraClass camera;
static CharMovement movement;
static Character character;
static Character* s_freeMoveAnchor=&character;
static CharMovement* s_anchorMovement=&movement;
static int gameWorld; static int* s_gwFrame=&gameWorld;
enum { MODE_VANILLA, MODE_FREE_MOVE };
static int s_mode=MODE_FREE_MOVE;
static bool s_fpActive, s_firstPersonActive, s_otsCamActive, s_camRotateToggle,
    s_otsRestorePending, s_loadGuardActive, s_dcShutdownInProgress, s_dcPtrLossActive,
    s_realLoadLatched, s_menuSuspendActive, s_cameraLockTurretSuspend,
    s_lootUiSuspendActive, s_camRotateUiOpen, s_wHeld, s_aHeld, s_sHeld, s_dHeld;
static bool uiOpen, foreground=true, live=true;
static bool dcUiWantsCursor() { return uiOpen; }
static bool isKenshiForegroundMain() { return foreground; }
static bool dcInUpdateList(int*, Character*) { return live; }
static bool isProtectedAnimationState(Character* c) { return c->protectedAnim; }
static bool isAnchoredToFurniture(Character* c) { return c->furniture; }
static bool isCommittedAction(Character* c) { return c->committed; }
static bool isCommittedCombatClip(Character* c) { return c->committed; }
static bool isDownedButMovable(Character* c) { return c->downed; }
static bool slaveOrderDriven(Character*) { return false; }
static bool s_healingJobActive=false;
static LONGLONG s_otsAlignLastTick=0;
static LONG s_otsGestureSerial;
static float s_fpYaw, s_otsYaw;
static bool s_otsAutoAlign;
static float otsMovementReferenceYaw(bool) { return s_otsYaw; }
static Ogre::Vector3 s_lastCamFwd;
static struct { bool normalizeDiagonalMovement; } g_loco={true};
#include "normal_camera_under_test.h"

static void check(bool ok, const char* name) { if (!ok) { printf("FAIL: %s\n",name); exit(1); } }
static bool near(float a, float b) { return fabsf(otsWrapYaw(a-b)) < 0.0001f; }
static float cameraHeading() { float yaw=0; normalCameraYaw(&camera,yaw); return yaw; }
static void setCameraHeading(float yaw) {
    float pitch=normalLookReadPose(&camera).pitch;
    camera.center.setOrientation(Ogre::Quaternion(Ogre::Radian(yaw+3.14159265f),Ogre::Vector3::UNIT_Y)
        * Ogre::Quaternion(Ogre::Radian(pitch),Ogre::Vector3::UNIT_X));
}
static void reset() {
    normalReleaseMouseLook(false);
    normalResetAutoAlign(); camera.center.parent=0;
    camera.center.orientation=Ogre::Quaternion(Ogre::Radian(0.523598776f),Ogre::Vector3::UNIT_X);
    camera.lens.orientation=Ogre::Quaternion::IDENTITY;
    camera.realCamera.orientation=Ogre::Quaternion(Ogre::Radian(3.14159265f),Ogre::Vector3::UNIT_Y);
    camera.lens.position=Ogre::Vector3(0,0,-150);
    setCameraHeading(0); camera.center.turns=0;
    s_normalLookNeedsCheck=false; s_normalLookRecovery=0;
    camera.isRotating=false; camera.follow=&character;
    camera.initialized=camera.terrainLoaded=camera.freeCamera=false;
    camera.camera=&camera.realCamera; camera.centerPtr=&camera.center; camera.nodePtr=&camera.lens;
    camera.realCamera.parent=&camera.lens; camera.lens.parent=&camera.center;
    player.camera=&camera; world.player=&player; world.loading=world.paused=false;
    character.movement=&movement; movement.pos=Ogre::Vector3::ZERO;
    s_freeMoveAnchor=&character; s_anchorMovement=&movement;
    s_normalAutoAlign=true; s_wHeld=true; s_aHeld=s_sHeld=s_dHeld=false;
    s_autoCameraEnabled=true; s_steeringMode=s_steeringValid=s_steeringIntent=false;
    s_steeringLegacyMovement=false;
    s_normalLookMoved=false;
    s_normalMouseLook=false; altDown=false; mouseX=mouseY=0;
    s_fpRawMouse=s_diReady=true; s_cameraDiagnostics=false;
    ++s_otsGestureSerial; clockTick+=1000000;
}
static bool frame(float dx=0, float dz=0, float dt=0.016f, float manualYaw=0) {
    clockTick+=(LONGLONG)(dt*1000000); movement.pos.x+=dx; movement.pos.z+=dz;
    float baseline=0; bool active=normalAutoAlignBeforeUpdate(&camera,true,baseline);
    setCameraHeading(cameraHeading()+manualYaw); // fake native manual camera rotation
    if (active) normalAutoAlignAfterUpdate(&camera,baseline);
    return active;
}
static void lookFrame(float dx=0, float dy=0, float nativePitch=0, float nativeYaw=0, float latePitch=0) {
    mouseX=dx; mouseY=dy; clockTick+=16000;
    normalMouseLookBeforeUpdate(&camera,true);
    float yawBefore=0;
    bool observed=normalAutoAlignBeforeUpdate(&camera,true,yawBefore);
    // Stand in for rotationUpdate on the same center, after our pre-native write.
    if (nativeYaw) camera.center.yaw(Ogre::Radian(nativeYaw),Ogre::Node::TS_WORLD);
    if (nativePitch) camera.center.pitch(Ogre::Radian(nativePitch),Ogre::Node::TS_LOCAL);
    if (s_normalLookNeedsCheck && normalLookAttached(&camera))
        normalLookRecover(&camera,s_normalLookCaptured ? 2 : 4);
    if (observed) normalAutoAlignAfterUpdate(&camera,yawBefore);
    // Mode restores / follow wake can also write after native update.
    if (latePitch) camera.center.pitch(Ogre::Radian(latePitch),Ogre::Node::TS_LOCAL);
    normalMouseLookAfterUpdate(&camera);
}
static void checkLookPose(const char* name) {
    NormalLookPose p=normalLookReadPose(&camera);
    check(normalLookPoseSafe(p) && p.pitch>=0.174532925f-0.000001f
        && p.pitch<=1.308996939f+0.000001f && p.upY>0.25f && fabsf(p.roll)<0.00001f,name);
    Ogre::Vector3 center=camera.center._getDerivedPositionUpdated();
    Ogre::Vector3 lens=camera.lens._getDerivedPositionUpdated();
    check(lens.y>center.y,"native -Z lens stays above pivot at clamped positive pitch");
}
static void testNormalPitch() {
    // Analytic native constructor convention: +30 center pitch, -150 lens Z,
    // camera lookAt(+Z) => lens above pivot and actual view down toward +Z.
    reset();
    camera.center.setOrientation(Ogre::Quaternion(Ogre::Radian(0.523598776f),Ogre::Vector3::UNIT_X));
    NormalLookPose native=normalLookReadPose(&camera);
    Ogre::Vector3 view=camera.camera->getDerivedDirection();
    check(near(native.pitch,0.523598776f) && near(native.viewPitch,native.pitch)
        && near(view.y,-0.5f) && near(view.z,0.866025404f)
        && near(camera.lens._getDerivedPositionUpdated().y,75),"native +Z optical/pitch sign convention");
    // Demonstrate the old error independently: adding +pitch to center makes
    // view elevation DECREASE, whereas the old clamp assumed it increased.
    camera.center.pitch(Ogre::Radian(0.2f),Ogre::Node::TS_LOCAL);
    check(near(normalLookReadPose(&camera).viewPitch,native.pitch+0.2f),"center pitch has opposite sign to old elevation clamp");

    for(int master=0;master<2;++master) for(int align=0;align<2;++align) {
        reset(); s_normalMouseLook=true; s_autoCameraEnabled=master!=0; s_normalAutoAlign=align!=0;
        s_wHeld=false; lookFrame(100000,100000); // first sample is discarded
        float startPitch=normalLookReadPose(&camera).pitch;
        check(near(startPitch,0.523598776f) && near(cameraHeading(),0),"entry discards both mouse axes");
        for(int pass=0;pass<4;++pass) {
            float dy=(pass%2)==0 ? 1000000.0f : -1000000.0f;
            for(int i=0;i<200;++i) {
                lookFrame(i%2 ? 2000.0f : -2000.0f,dy);
                checkLookPose("repeated large mixed input cannot cross either absolute boundary");
            }
            check(near(normalLookReadPose(&camera).pitch,dy>0 ? 1.308996939f : 0.174532925f),
                "large vertical motion reaches exact clamp with F6/auto-align on or off");
            float boundary=normalLookReadPose(&camera).pitch;
            lookFrame(0,dy>0 ? -10.0f : 10.0f);
            check(dy>0 ? normalLookReadPose(&camera).pitch<boundary : normalLookReadPose(&camera).pitch>boundary,
                "reversing at boundary responds immediately without stored vertical backlog");
        }
        // Multiple complete horizontal orbits at a fixed non-boundary pitch.
        normalLookWritePose(&camera,0.4f,0.7f,false);
        s_normalLookYawPending=s_normalLookPitchPending=0;
        float accumulated=0, last=normalLookReadPose(&camera).yaw;
        for(int i=0;i<3000;++i) {
            lookFrame(12,0);
            NormalLookPose p=normalLookReadPose(&camera);
            accumulated+=otsWrapYaw(p.yaw-last); last=p.yaw;
            checkLookPose("yaw through every quadrant never rolls or inverts");
            check(near(p.pitch,0.7f),"horizontal 360-degree yaw preserves absolute pitch");
        }
        check(accumulated<-12.0f*3.14159265f,"continuous yaw completes more than six full turns");
    }

    reset(); s_normalMouseLook=true; lookFrame(); lookFrame(0,40);
    altDown=true; lookFrame();
    normalLookWritePose(&camera,1.2f,0.9f,false); // native mouse rotation while cursor is released
    altDown=false; lookFrame(100000,-100000);
    check(near(normalLookReadPose(&camera).pitch,0.9f) && near(normalLookReadPose(&camera).yaw,1.2f),
        "ALT recapture seeds actual native pose and discards both mouse baselines");
    lookFrame(0,1); check(normalLookReadPose(&camera).pitch>0.9f,"mouse down uses positive native down-pitch");
    s_normalLookYawPending=s_normalLookPitchPending=0;
    float beforeNative=normalLookReadPose(&camera).pitch;
    lookFrame(0,0,0.05f); lookFrame();
    check(near(normalLookReadPose(&camera).pitch,beforeNative+0.05f),
        "native in-range tilt becomes next frame's absolute pitch without repeated accumulation");
    s_fpRawMouse=false; pointerPos.y+=100000;
    float beforeHandoff=normalLookReadPose(&camera).pitch; lookFrame(100000,100000);
    check(near(normalLookReadPose(&camera).pitch,beforeHandoff),"raw-to-cursor handoff discards vertical baseline");
    s_fpRawMouse=true; lookFrame(100000,100000);
    check(near(normalLookReadPose(&camera).pitch,beforeHandoff),"cursor-to-raw handoff discards vertical baseline");
    bool* transitions[]={&world.loading,&s_loadGuardActive,&s_firstPersonActive,&s_otsCamActive};
    for(unsigned i=0;i<sizeof(transitions)/sizeof(transitions[0]);++i) {
        *transitions[i]=true; lookFrame();
        *transitions[i]=false; normalLookWritePose(&camera,-0.6f,0.6f,false);
        lookFrame(-100000,100000);
        check(near(normalLookReadPose(&camera).pitch,0.6f) && near(normalLookReadPose(&camera).yaw,-0.6f),
            "load and detached-mode reentry reseed native orientation without stale input");
    }
    ++s_cameraFocusSerial; normalLookWritePose(&camera,0.2f,0.8f,false); lookFrame(100000,-100000);
    check(near(normalLookReadPose(&camera).pitch,0.8f),"focus epoch resets vertical baseline");

    // Collision/zoom may alter the lens's local POSITION. Pitch is an orientation
    // measurement, never an angle inferred from that displaced center/lens ray.
    reset(); s_normalMouseLook=true; camera.lens.position=Ogre::Vector3(3,8,-90); lookFrame();
    float initial=normalLookReadPose(&camera).pitch;
    lookFrame(25,0);
    check(near(normalLookReadPose(&camera).pitch,initial) && camera.lens.position.x==3
        && camera.lens.position.y==8 && camera.lens.position.z==-90,"zoom/collision offset cannot corrupt pitch measurement or be overwritten");

    // A transformed parent requires converting canonical WORLD yaw/pitch back
    // to center's parent space; multiplying local quaternions directly rolls.
    Ogre::SceneNode root;
    root.orientation=Ogre::Quaternion(Ogre::Radian(0.3f),Ogre::Vector3::UNIT_Z)
        * Ogre::Quaternion(Ogre::Radian(1.1f),Ogre::Vector3::UNIT_Y);
    camera.center.parent=&root;
    check(normalLookWritePose(&camera,-1.0f,0.5f,false),"world basis converted to parent space");
    s_normalLookYawPending=s_normalLookPitchPending=0;
    for(int i=0;i<100;++i) { lookFrame(15,0); checkLookPose("nonidentity parent cannot add world roll"); }
    check(near(normalLookReadPose(&camera).pitch,0.5f),"parent transform leaves absolute world pitch invariant");
    camera.center.parent=0;

    // Bad native/external poses must recover even without another mouse sample.
    const float badPitch[]={-0.3f,0,1.56f,1.570796327f,2.2f,3.14159265f};
    for(unsigned i=0;i<sizeof(badPitch)/sizeof(badPitch[0]);++i) {
        reset(); s_normalMouseLook=true;
        camera.center.orientation=Ogre::Quaternion(Ogre::Radian(0.3f),Ogre::Vector3::UNIT_Y)
            * Ogre::Quaternion(Ogre::Radian(badPitch[i]),Ogre::Vector3::UNIT_X);
        lookFrame(100000,100000); checkLookPose("capture repairs old under-ground/overhead/inverted orientation");
    }
    reset(); s_normalMouseLook=true; lookFrame();
    lookFrame(0,0,2.5f,0.6f); checkLookPose("post-native guard repairs keyboard tilt beyond vertical in same frame");
    check((s_normalLookRecovery&2)!=0,"post-native recovery event latched");
    lookFrame(0,0,0,0,2.5f); checkLookPose("final guard also repairs later restore/watchdog writes before rendering");
    camera.center.orientation=camera.center.orientation
        * Ogre::Quaternion(Ogre::Radian(3.14159265f),Ogre::Vector3::UNIT_Z);
    s_normalMouseLook=false; lookFrame();
    check(!s_normalLookCaptured && !pointerCaptured,"F10 OFF releases cursor during recovery");
    checkLookPose("F10 OFF repairs known-invalid roll without manual rotation");
    check(!s_normalLookNeedsCheck,"release obligation completed after final native update");
    reset(); s_normalMouseLook=true; lookFrame(); s_mode=MODE_VANILLA;
    camera.center.pitch(Ogre::Radian(2),Ogre::Node::TS_LOCAL); lookFrame();
    checkLookPose("V exit repairs invalid pose on still-attached player rig"); s_mode=MODE_FREE_MOVE;
    reset(); s_normalMouseLook=true; lookFrame(); normalReleaseMouseLook(false);
    camera.center.pitch(Ogre::Radian(2),Ogre::Node::TS_LOCAL);
    s_normalMouseLook=false; lookFrame(); checkLookPose("world-hook cursor release retains safe recovery obligation");
    reset(); s_normalMouseLook=true; lookFrame(); lookFrame(20,30);
    NormalLookPose beforeOff=normalLookReadPose(&camera); s_normalMouseLook=false; lookFrame();
    NormalLookPose afterOff=normalLookReadPose(&camera);
    check(near(beforeOff.pitch,afterOff.pitch) && near(beforeOff.yaw,afterOff.yaw),"F10 OFF preserves a valid native pose");
    reset(); s_normalMouseLook=true; lookFrame(); s_normalMouseLook=false;
    lookFrame(0,0,2.5f,0,2.5f); checkLookPose("F10 release checks both native and late writes before finishing");
    reset(); s_normalMouseLook=true;
    camera.center.orientation=Ogre::Quaternion(std::numeric_limits<float>::quiet_NaN(),0,0,0);
    lookFrame(); checkLookPose("nonfinite center orientation recovers to upright native pose");
    camera.lens.orientation=Ogre::Quaternion(Ogre::Radian(0.5f),Ogre::Vector3::UNIT_Z);
    camera.realCamera.orientation=Ogre::Quaternion::IDENTITY;
    lookFrame(); checkLookPose("invalid lens/optics orientation restored to native +Z basis");
    camera.center.orientation=Ogre::Quaternion(0,0,0,0);
    lookFrame(); checkLookPose("zero quaternion recovers without division by zero");

    reset(); s_normalMouseLook=true; s_cameraDiagnostics=true; s_normalPitchDiagLastMs=0;
    int logs=pitchLogs; lookFrame(); check(pitchLogs==logs+1,"pitch diagnostic emitted at capture");
    lookFrame(5,6); check(pitchLogs==logs+1,"pitch diagnostics rate limited");
    s_normalPitchDiagLastMs=GetTickCount64()-1000; lookFrame(7,8,2.5f);
    check(pitchLogs==logs+2 && strstr(lastPitchLog,"dx=7.00 dy=8.00") && strstr(lastPitchLog,"nativeInput=")
        && strstr(lastPitchLog,"before=") && strstr(lastPitchLog,"requested=") && strstr(lastPitchLog,"clamped=")
        && strstr(lastPitchLog,"viewQ=") && strstr(lastPitchLog,"upY=") && strstr(lastPitchLog,"roll=")
        && strstr(lastPitchLog,"recovery=0xA"),"rate-limited pitch log includes input, native deltas, absolute clamp, actual optics and inversion recovery");
    reset(); s_loadGuardActive=true; ou=(World*)1;
    check(!normalLookAttached(&camera),"recovery load guard precedes unsafe world dereference");
    ou=&world; s_loadGuardActive=false;
    reset();
    puts("PASS: native +Z pitch sign, absolute limits, repeated extreme input, six-turn yaw, F6 combinations, ALT/load/mode recapture, world basis, post-native/F10 recovery and pitch diagnostics");
}
int main() {
    testNormalPitch();
    reset();
    check(camera.camera->getParentSceneNode()==camera.getCameraNode()
        && camera.getCenterNode()!=camera.getCameraNode(),"observed runtime parent==node with distinct center");
    for(int flags=0;flags<4;++flags) {
        camera.initialized=(flags&1)!=0; camera.terrainLoaded=(flags&2)!=0;
        check(normalCameraGates(&camera,true)==0,"native attached rig accepted independently of legacy readiness flags");
        frame(); frame(0.16f,0);
        check(camera.center.turns>0,"native attached rig receives auto yaw writes");
    }
    reset();
    check(!camera.isInitialised() && !camera.terrainLoaded && normalCameraGates(&camera,true)==0,
        "regression: old 0x400 false rejection removed");
    check(normalCameraGates(0,true)==2,"null player-camera argument rejected");
    camera.camera=0; check((normalCameraGates(&camera,true)&0x800)!=0,"missing Ogre camera rejected");
    camera.camera=&camera.realCamera;
    camera.centerPtr=0; check((normalCameraGates(&camera,true)&0x800)!=0,"missing center rejected");
    camera.centerPtr=&camera.center;
    camera.nodePtr=0; check((normalCameraGates(&camera,true)&0x800)!=0,"missing lens node rejected");
    camera.nodePtr=&camera.center; check((normalCameraGates(&camera,true)&0x800)!=0,"aliased center and lens rejected");
    camera.nodePtr=&camera.lens;
    camera.realCamera.parent=&camera.center; check((normalCameraGates(&camera,true)&0x800)!=0,"detached or foreign parent rejected");
    camera.realCamera.parent=&camera.lens;
    camera.lens.parent=0; check((normalCameraGates(&camera,true)&0x800)!=0,"orphan native lens rejected");
    camera.lens.parent=&camera.center;
    camera.freeCamera=true; check(normalCameraGates(&camera,true)==0x400,"actual free-camera mode still rejected");
    reset(); camera.initialized=true; // exact live snapshot: init=1, terrain=0, free=0
    s_cameraDiagnostics=true; s_normalCaptureDiagLastMs=s_normalWriteDiagLastMs=0;
    int capturesBefore=captureLogs, writesBefore=writeLogs;
    s_normalLookToggleRequest=1; cameraToggleRequests(&camera,true);
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalMouseLook && s_normalLookCaptured && pointerCaptured && captureLogs==capturesBefore+1,
        "F10 request captures actual attached rig and logs activation");
    check(pointerPos.x==500 && pointerPos.y==400,"capture recenters cursor using shared lifecycle");
    s_autoCameraToggleRequest=1; cameraToggleRequests(&camera,true);
    check(!s_autoCameraEnabled && s_normalLookCaptured,"F6 leaves active capture intact");
    int capturedDrains=mouseConsumes;
    mouseX=25; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(cameraHeading()<0 && writesBefore+1==writeLogs && strstr(lastWriteLog,"source=mouse")
        && !strstr(lastWriteLog,"actual=0.000000") && mouseConsumes==capturedDrains+1,
        "F6 off consumes one mouse sample and logs actual measured yaw write");
    mouseX=25; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(writeLogs==writesBefore+1,"yaw diagnostics rate limited within one second");
    s_normalLookToggleRequest=1; cameraToggleRequests(&camera,true);
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && !pointerCaptured,"F10 OFF releases cursor after F6 OFF");
    s_normalLookToggleRequest=1; cameraToggleRequests(&camera,true);
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && captureLogs==capturesBefore+1,"rapid recapture diagnostics rate limited");
    altDown=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    altDown=false; clockTick+=1000000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && captureLogs==capturesBefore+2,"capture activation logs again after rate interval");
    reset(); s_cameraDiagnostics=true; s_normalWriteDiagLastMs=0;
    writesBefore=writeLogs; frame(); frame(0.16f,0);
    check(writeLogs==writesBefore+1 && strstr(lastWriteLog,"source=auto")
        && !strstr(lastWriteLog,"actual=0.000000"),"auto diagnostics observe actual yaw write on native rig");
    frame(0.16f,0); check(writeLogs==writesBefore+1,"auto write diagnostics rate limited");
    reset(); s_normalAutoAlign=false; s_autoCameraEnabled=false; s_normalMouseLook=true;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    mouseX=20; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && cameraHeading()<0,"mouse-look independent of INI auto-align and F6 master");
    reset(); s_normalMouseLook=true; s_fpRawMouse=false;
    int drainsBefore=mouseConsumes, ensuresBefore=inputEnsures, startsBefore=inputStarts;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    pointerPos.x+=20; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && !s_normalLookRaw && cameraHeading()<0 && mouseConsumes==drainsBefore
        && inputEnsures==ensuresBefore && inputStarts==startsBefore,"RawMouse off uses OTS warp fallback without raw consumer");
    s_fpRawMouse=true; mouseX=9000; float fallbackYaw=cameraHeading();
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookRaw && near(cameraHeading(),fallbackYaw),"warp to raw handoff drops baseline");
    reset(); s_normalMouseLook=true; s_diReady=false;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    pointerPos.x+=20; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && !s_normalLookRaw && cameraHeading()<0,"unavailable raw device falls back to recentered cursor");
    reset(); s_normalMouseLook=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    int drains=mouseConsumes;
    CameraClass preview; normalMouseLookBeforeUpdate(&preview,true);
    check(s_normalLookCaptured && mouseConsumes==drains,"portrait camera never consumes shared input or releases player capture");
    normalMouseLookBeforeUpdate(&camera,false);
    check(!s_normalLookCaptured && mouseConsumes==drains,"engine UI focus releases capture without draining input");
    reset(); iniFloatValue="nan";
    check(readCameraFloat("","","90.0",10,360)==90,"nonfinite tuning uses default");
    iniFloatValue="90junk"; check(readCameraFloat("","","90.0",10,360)==90,"invalid numeric suffix rejected");
    iniFloatValue="999"; check(readCameraFloat("","","90.0",10,360)==360,"turn tuning upper clamp");
    iniFloatValue=" 0.01 "; check(readCameraFloat("","","1.0",0.1f,5)==0.1f,"sensitivity lower clamp");
    iniFloatValue=0;
    reset(); s_normalMouseLook=true; pointerPos.x=120; pointerPos.y=240;
    mouseX=500; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && pointerCaptured && near(cameraHeading(),0),"entry discards raw backlog and captures cursor");
    float steeringSentinel=s_steeringYaw;
    mouseX=20; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(cameraHeading()<0 && near(s_steeringYaw,steeringSentinel),"mouse orbit does not steer character");
    float lookYaw=cameraHeading();
    altDown=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && !pointerCaptured && pointerPos.x==120 && pointerPos.y==240,"ALT restores pointer and click position");
    pointerPos.x=700; pointerPos.y=600; mouseX=10000;
    altDown=false; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && near(cameraHeading(),lookYaw),"ALT resume discards backlog without camera jump");
    int consumed=mouseConsumes;
    s_firstPersonActive=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && mouseConsumes==consumed,"FP owns accumulator after mode switch");
    s_firstPersonActive=false;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    consumed=mouseConsumes; s_otsCamActive=true;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && mouseConsumes==consumed,"OTS owns accumulator after mode switch"); s_otsCamActive=false;
    bool* captureGates[]={&world.paused,&uiOpen,&guiObject.inventory,&world.loading,&s_loadGuardActive,
        &s_dcShutdownInProgress,&s_dcPtrLossActive,&s_menuSuspendActive};
    for(unsigned g=0;g<sizeof(captureGates)/sizeof(captureGates[0]);++g) {
        reset(); s_normalMouseLook=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
        *captureGates[g]=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
        check(!s_normalLookCaptured && !pointerCaptured && s_normalMouseLook,"UI/load suspends capture and retains preference");
        *captureGates[g]=false; mouseX=9000; float before=cameraHeading();
        clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
        check(s_normalLookCaptured && near(cameraHeading(),before),"UI/load return rebaselines");
    }
    reset(); s_normalMouseLook=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    foreground=false; pointerPos.x=900; pointerPos.y=60;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && !pointerCaptured && pointerPos.x==900,"focus loss does not warp other app pointer"); foreground=true;
    mouseX=9000; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(near(cameraHeading(),0),"focus resume drops relative backlog");
    ++s_cameraFocusSerial; mouseX=9000; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(near(cameraHeading(),0),"focus epoch catches loss while camera hook was stopped");
    s_normalMouseLook=false; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && !pointerCaptured,"toggle off releases capture");
    reset(); s_autoCameraToggleRequest=1; int messages=world.messages;
    cameraToggleRequests(&camera,true);
    check(!s_autoCameraEnabled && world.messages==messages+1,"runtime auto toggle emits feedback");
    s_normalLookToggleRequest=1; cameraToggleRequests(&camera,true);
    check(s_normalMouseLook,"runtime normal mouse toggle");
    uiOpen=true; s_normalLookToggleRequest=1; cameraToggleRequests(&camera,true);
    check(s_normalMouseLook && !s_normalLookToggleRequest,"menu toggle edge consumed without changing state"); uiOpen=false;
    s_loadGuardActive=true; s_autoCameraToggleRequest=1; cameraToggleRequests(&camera,true);
    check(!s_autoCameraEnabled && !s_autoCameraToggleRequest,"load discards queued toggle rather than replaying on resume"); s_loadGuardActive=false;
    reset(); s_steeringMode=true; s_aHeld=true;
    movement.facing=Ogre::Vector3(-sinf(0.3f),0,-cosf(0.3f));
    steeringFrame(); check(s_steeringValid && near(s_steeringYaw,0.3f),"steering seeds actual body heading");
    setCameraHeading(2); clockTick+=20000; steeringFrame();
    check(near(s_steeringYaw,0.3f+0.02f*1.5707963f),"world frame turns only from elapsed time and steering input");
    world.paused=true; clockTick+=1000000; steeringFrame();
    check(!s_steeringValid && !s_steeringIntent,"pause invalidates steering history"); world.paused=false;
    steeringFrame(); check(near(s_steeringYaw,0.3f),"resume seeds body without catchup");
    s_anchorMovement=0; steeringFrame(); check(!s_steeringValid,"steering rejects lost movement ownership"); s_anchorMovement=&movement;
    character.downed=true; steeringFrame();
    check(s_steeringLegacyMovement && !s_steeringValid,"downed uses established locomotion");
    Ogre::Vector3 legacyDir;
    check(computeWASDDirection(false,true,false,false,legacyDir),"downed retains original strafe movement"); character.downed=false;
    reset(); s_steeringMode=s_steeringValid=s_steeringIntent=true;
    s_steeringAnchor=&character; s_steeringMovement=&movement; s_steeringYaw=0.4f;
    Ogre::Vector3 forwardDir, backwardDir, curveDir;
    check(computeWASDDirection(true,false,false,false,forwardDir),"steering forward");
    check(computeWASDDirection(false,false,true,false,backwardDir),"steering backward");
    check((forwardDir+backwardDir).length()<0.0001f,"S reverses velocity without flipping steering heading");
    check(computeWASDDirection(false,true,false,false,curveDir) && near(curveDir.length(),1),"A alone has full forward translation");
    check(computeWASDDirection(false,false,false,true,curveDir) && near(curveDir.length(),1),"D alone has full forward translation");
    check(!computeWASDDirection(false,true,false,true,curveDir),"A+D alone cancels movement");
    check(!computeWASDDirection(true,true,true,false,curveDir),"W+S cancels translation");
    for(int i=0;i<50;++i) {
        setCameraHeading(i*0.1f); s_otsYaw=-i*0.05f;
        s_otsCamActive=(i%2)==0;
        computeWASDDirection(true,true,false,false,curveDir);
        check((curveDir-forwardDir).length()<0.0001f,"normal and OTS view yaw cannot feed steering");
    }
    s_otsCamActive=false;
    // Integrate actual production direction + once-per-world-frame heading for
    // A, D, W+A, W+D. The engine still owns speed, animation and collision.
    for(int which=0;which<4;++which) {
        reset(); s_steeringMode=true; s_wHeld=which>=2;
        s_aHeld=(which%2)==0; s_dHeld=!s_aHeld;
        movement.facing=Ogre::Vector3(0,0,-1); steeringFrame();
        Ogre::Vector3 startDir, endDir, translated;
        check(computeWASDDirection(s_wHeld,s_aHeld,false,s_dHeld,startDir),"arc has translation on first frame");
        for(int i=0;i<50;++i) {
            setCameraHeading(2.5f-i*0.03f); s_otsYaw=-2.5f+i*0.02f;
            clockTick+=20000; steeringFrame();
            check(computeWASDDirection(s_wHeld,s_aHeld,false,s_dHeld,endDir),"A/D arc keeps translating");
            translated+=endDir;
            check(near(endDir.length(),1),"arc direction keeps existing unit speed scale");
        }
        check(translated.length()>40 && fabsf(s_steeringYaw)>1.5f && (startDir-endDir).length()>1,
            "A/D and W+A/D produce nonzero translation and continuous turn independent of camera");
        check(which%2==0 ? translated.x<0 : translated.x>0,"A arcs left and D arcs right");
        movement.facing=endDir; // emulate faceDirection after native locomotion
        s_wHeld=s_sHeld=true; float stoppedYaw=s_steeringYaw;
        clockTick+=20000; steeringFrame();
        check(!s_steeringIntent && near(stoppedYaw,s_steeringYaw)
            && !computeWASDDirection(true,s_aHeld,true,s_dHeld,endDir),"cancelled translation never rotates in place");
    }
    float slow=0,fast=0;
    for(int i=0;i<30;++i) slow=steeringAdvanceYaw(slow,true,false,1.0f/30.0f);
    for(int i=0;i<144;++i) fast=steeringAdvanceYaw(fast,true,false,1.0f/144.0f);
    check(near(slow,fast) && near(slow,1.5707963f),"steering degrees/sec independent of frame rate");
    check(near(steeringAdvanceYaw(0.4f,true,true,0.02f),0.4f),"opposing turns cancel");
    check(near(steeringAdvanceYaw(0.4f,true,false,0.5f),0.4f),"steering rejects long stalls");
    float pending=0,applied=0;
    for(int i=0;i<144;++i) applied+=normalLookSmooth(pending,i==0?0.5f:0,1.0f/144.0f);
    check(fabsf(applied-0.5f)<0.0001f,"mouse smoothing preserves integrated displacement");
    check(normalLookSmooth(pending,10,1)==0 && pending==0,"mouse smoothing drops stall backlog");
    check(near(normalLookClampPitch(-100),0.174532925f),"absolute lower pitch clamp");
    check(near(normalLookClampPitch(100),1.308996939f),"absolute upper pitch clamp");
    check(near(normalLookClampPitch(0.7f),0.7f),"valid absolute pitch preserved");
    reset(); s_steeringMode=s_steeringValid=s_steeringIntent=true; s_steeringYaw=0.7f;
    frame(); for(int i=0;i<300;++i) frame();
    check(near(cameraHeading(),0.7f),"stationary intentional steering aligns camera to heading");
    reset(); frame(); Ogre::Vector3 heldBefore,heldAfter;
    computeWASDDirection(true,false,false,false,heldBefore);
    s_autoCameraEnabled=false; setCameraHeading(0.7f);
    frame(0.16f,0); computeWASDDirection(true,false,false,false,heldAfter);
    check(camera.center.turns==0 && (heldBefore-heldAfter).length()<0.0001f,"auto master off preserves held movement");
    keys.rotate=true; frame(0.16f,0,0.016f,0.2f); keys.rotate=false;
    check(near(cameraHeading(),0.9f),"manual rotation still works with auto master off");
    reset(); s_normalAutoAlign=false; frame(1,0); frame(1,0);
    check(camera.center.turns==0,"default disabled");
    Ogre::Vector3 vanillaDir;
    setCameraHeading(0.8f); computeWASDDirection(true,false,false,false,vanillaDir);
    check(near(atan2f(-vanillaDir.x,-vanillaDir.z),0.8f),"disabled uses live vanilla heading");
    Ogre::Vector3 leftDir,rightDir;
    check(computeWASDDirection(false,true,false,false,leftDir)
        && computeWASDDirection(false,false,false,true,rightDir)
        && (leftDir+rightDir).length()<0.0001f
        && near(atan2f(-leftDir.x,-leftDir.z),0.8f+1.5707963f),
        "SteeringMode false preserves original camera-relative A/D strafe");
    // Sustained A/D/S/diagonal must retain a world heading as the view aligns.
    for (int which=0; which<4; ++which) {
        reset(); s_wHeld=which==3; s_sHeld=which==2; s_aHeld=which==0; s_dHeld=which==1 || which==3;
        Ogre::Vector3 expected, actual;
        computeWASDDirection(s_wHeld,s_aHeld,s_sHeld,s_dHeld,expected);
        frame();
        for (int i=0;i<300;++i) {
            frame(expected.x*0.16f,expected.z*0.16f);
            computeWASDDirection(s_wHeld,s_aHeld,s_sHeld,s_dHeld,actual);
            check((actual-expected).length()<0.0001f,"held WASD cannot chase automatic yaw");
        }
        check(near(cameraHeading(),atan2f(-expected.x,-expected.z)),"view converges to actual travel");
    }
    reset(); frame(); frame(0.16f,0); int turns=camera.center.turns;
    for(int i=0;i<20;++i) frame();
    check(camera.center.turns==turns,"stationary immediately stops turning");
    frame(0,0.16f,0.016f); check(camera.center.turns>turns,"travel resumes alignment");
    turns=camera.center.turns; frame(100,100); frame(1,0,0.5f);
    frame(0,0,0.016f); check(camera.center.turns==turns,"teleports and long stalls rejected");
    reset(); setCameraHeading(3.12413936f); frame();
    frame(sinf(0.01745329f)*0.16f,cosf(0.01745329f)*0.16f);
    check(cameraHeading()>3.12413936f && cameraHeading()<3.13f,"shortest turn across yaw seam");
    reset(); s_wHeld=false; frame(); frame(0.16f,0);
    check(camera.center.turns>0 && !s_normalMoveRefValid,"actual travel also works without held WASD");
    reset(); frame(); frame(0.16f,0); float ref=s_normalMoveRefYaw;
    keys.rotate=true; turns=camera.center.turns; frame(0.16f,0,0.016f,0.3f);
    check(camera.center.turns==turns && near(s_normalMoveRefYaw,ref+0.3f),"manual yaw alone updates reference");
    keys.rotate=false; frame(0.16f,0,0.1f);
    check(camera.center.turns==turns,"manual override delay");
    for(int i=0;i<8;++i) frame(0.16f,0,0.1f);
    check(camera.center.turns>turns,"alignment resumes after override");
    reset(); frame(); frame(0.16f,0,0.016f,0.02f);
    ref=s_normalMoveRefYaw; turns=camera.center.turns;
    frame(0.16f,0,0.016f,0.02f);
    check(camera.center.turns>turns && near(ref,s_normalMoveRefYaw),
        "native drift without input cannot extend override or steer held movement");
    reset(); frame(); keys.tiltUp=true; frame(0.16f,0); keys.tiltUp=false;
    turns=camera.center.turns; frame(0.16f,0);
    check(camera.center.turns==turns,"pitch-only manual input suppresses alignment");
    reset(); frame(); setCameraHeading(1);
    check(near(normalMovementReferenceYaw(&camera,1,true),0),"same gesture remains frozen");
    s_wHeld=false; frame(); s_wHeld=true; ++s_otsGestureSerial;
    check(near(normalMovementReferenceYaw(&camera,1,true),1),"release/repress adopts current view");
    CameraClass portrait; float baseline=0; s_normalAlignLastTick=clockTick;
    check(!normalAutoAlignBeforeUpdate(&portrait,true,baseline) && s_normalAlignLastTick==clockTick,
        "portrait updates do not reset player state");
    bool* gates[]={&world.loading,&world.paused,&uiOpen,&guiObject.inventory,&s_loadGuardActive,
        &s_dcShutdownInProgress,&s_dcPtrLossActive,&s_realLoadLatched,&s_menuSuspendActive,
        &s_cameraLockTurretSuspend,&s_lootUiSuspendActive,&s_fpActive,&s_firstPersonActive,
        &s_otsCamActive,&s_camRotateToggle,&s_otsRestorePending,&character.dead,
        &character.unconscious,&character.protectedAnim,&character.ragdoll,&character.carried,&camera.freeCamera};
    for(unsigned i=0;i<sizeof(gates)/sizeof(gates[0]);++i) {
        reset(); frame(); *gates[i]=true; turns=camera.center.turns;
        check(!frame(0.16f,0) && camera.center.turns==turns && !s_normalMoveRefValid && !s_normalAlignLastTick,
            "suspension resets history and stops rotation"); *gates[i]=false;
        frame(0.16f,0); check(camera.center.turns==turns,"resume seeds history without a snap");
    }
    reset(); frame(); check(!normalAutoAlignBeforeUpdate(&camera,false,baseline),"engine UI focus gate");
    foreground=false; check(!frame(1,0),"background gate"); foreground=true;
    live=false; check(!frame(1,0),"anchor liveness gate"); live=true;
    s_anchorMovement=0; check(!frame(1,0),"movement handoff gate"); s_anchorMovement=&movement;
    s_gwFrame=0; s_freeMoveAnchor=(Character*)1;
    check(!frame(),"invalid anchor never dereferenced without liveness");
    s_freeMoveAnchor=&character; s_gwFrame=&gameWorld;
    s_loadGuardActive=true; ou=(World*)1;
    check(!normalAutoAlignBeforeUpdate(&camera,true,baseline),"load guard precedes world/player access");
    ou=&world; s_loadGuardActive=false;
    reset(); frame(); CharMovement replacement; character.movement=&replacement;
    s_anchorMovement=&replacement; frame();
    check(s_normalAlignMovement==&replacement,"new movement identity reseeds state");
    character.movement=&movement; s_anchorMovement=&movement;
    reset(); frame(); movement.pos.x=std::numeric_limits<float>::quiet_NaN(); frame();
    check(!s_normalAlignLastTick,"nonfinite position rejected");
    check(camera.pitch==0.7f && camera.altitude==50 && camera.realCamera.parent==&camera.lens
        && camera.follow==&character,"pitch zoom attachment and tracking remain untouched");
    puts("PASS: actual attached camera structure, legacy flag regression, F10/F6, raw/fallback ownership, ALT/UI lifecycle, yaw diagnostics, A/D arcs, normal alignment and safety gates");
    return 0;
}
