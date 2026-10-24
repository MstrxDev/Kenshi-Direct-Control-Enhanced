// The runner extracts production functions verbatim; these fakes isolate their
// state/input decisions. Real Ogre transforms and in-game rendering need playtesting.
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
static void DebugLog(const char*) {}
namespace MyGUI {
struct InputManager {
    static InputManager* getInstancePtr() { static InputManager m; return &m; }
    void injectMouseMove(int,int,int) {}
};
}
static bool pointerCaptured=false;
static void dcUpdateCrosshair(bool want) { pointerCaptured=want; }
static bool s_diReady=true;
static float mouseX=0,mouseY=0;
static int mouseConsumes=0;
static void fpEnsureDInput() {}
static void fpStartDInputThread() {}
static void fpTakeMouseAccum(float* x,float* y) { ++mouseConsumes; *x=mouseX; *y=mouseY; mouseX=mouseY=0; }
static const float FP_RAD_PER_PIXEL=0.002f;
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
    static const Vector3 ZERO;
};
const Vector3 Vector3::ZERO;
struct Radian { float value; explicit Radian(float f): value(f) {} };
struct Node { enum TransformSpace { TS_LOCAL, TS_PARENT, TS_WORLD }; };
struct SceneNode {
    float heading, pitchValue; int turns; bool lens;
    SceneNode(): heading(0), pitchValue(0), turns(0), lens(false) {}
    void yaw(Radian r, Node::TransformSpace space) {
        if (space != Node::TS_WORLD) abort();
        heading += r.value; ++turns;
    }
    void _getFullTransformUpdated() {}
    Vector3 _getDerivedPositionUpdated() { return lens ? Vector3(0,10,10) : Vector3(); }
    void pitch(Radian r, Node::TransformSpace space) { if(space!=Node::TS_LOCAL) abort(); pitchValue+=r.value; }
};
struct Camera {
    SceneNode* parent;
    SceneNode* getParentSceneNode() { return parent; }
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
    Ogre::Camera realCamera; Ogre::Camera* camera;
    bool isRotating, terrainLoaded, initialized, freeCamera;
    Character* follow;
    float pitch, altitude; // sentinels: controller must never write these
    CameraClass(): camera(&realCamera), isRotating(false), terrainLoaded(true),
        initialized(true), freeCamera(false), follow(0), pitch(0.7f), altitude(50) { realCamera.parent=&lens; lens.lens=true; }
    Ogre::Vector3 getFacingDirection() { return Ogre::Vector3(-sinf(center.heading),0,-cosf(center.heading)); }
    Ogre::SceneNode* getCenterNode() { return &center; }
    Ogre::SceneNode* getCameraNode() { return &lens; }
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
static void reset() {
    normalReleaseMouseLook(false);
    normalResetAutoAlign(); camera.center.heading=0; camera.center.turns=0;
    camera.isRotating=false; camera.follow=&character;
    player.camera=&camera; world.player=&player; world.loading=world.paused=false;
    character.movement=&movement; movement.pos=Ogre::Vector3::ZERO;
    s_freeMoveAnchor=&character; s_anchorMovement=&movement;
    s_normalAutoAlign=true; s_wHeld=true; s_aHeld=s_sHeld=s_dHeld=false;
    s_autoCameraEnabled=true; s_steeringMode=s_steeringValid=s_steeringIntent=false;
    s_steeringLegacyMovement=false;
    s_normalLookMoved=false;
    s_normalMouseLook=false; altDown=false; mouseX=mouseY=0;
    ++s_otsGestureSerial; clockTick+=1000000;
}
static bool frame(float dx=0, float dz=0, float dt=0.016f, float manualYaw=0) {
    clockTick+=(LONGLONG)(dt*1000000); movement.pos.x+=dx; movement.pos.z+=dz;
    float baseline=0; bool active=normalAutoAlignBeforeUpdate(&camera,true,baseline);
    camera.center.heading+=manualYaw; // fake native manual camera rotation
    if (active) normalAutoAlignAfterUpdate(&camera,baseline);
    return active;
}
int main() {
    reset(); iniFloatValue="nan";
    check(readCameraFloat("","","90.0",10,360)==90,"nonfinite tuning uses default");
    iniFloatValue="90junk"; check(readCameraFloat("","","90.0",10,360)==90,"invalid numeric suffix rejected");
    iniFloatValue="999"; check(readCameraFloat("","","90.0",10,360)==360,"turn tuning upper clamp");
    iniFloatValue=" 0.01 "; check(readCameraFloat("","","1.0",0.1f,5)==0.1f,"sensitivity lower clamp");
    iniFloatValue=0;
    reset(); s_normalMouseLook=true; pointerPos.x=120; pointerPos.y=240;
    mouseX=500; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && pointerCaptured && near(camera.center.heading,0),"entry discards raw backlog and captures cursor");
    float steeringSentinel=s_steeringYaw;
    mouseX=20; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(camera.center.heading<0 && near(s_steeringYaw,steeringSentinel),"mouse orbit does not steer character");
    float lookYaw=camera.center.heading;
    altDown=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && !pointerCaptured && pointerPos.x==120 && pointerPos.y==240,"ALT restores pointer and click position");
    pointerPos.x=700; pointerPos.y=600; mouseX=10000;
    altDown=false; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(s_normalLookCaptured && near(camera.center.heading,lookYaw),"ALT resume discards backlog without camera jump");
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
        *captureGates[g]=false; mouseX=9000; float before=camera.center.heading;
        clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
        check(s_normalLookCaptured && near(camera.center.heading,before),"UI/load return rebaselines");
    }
    reset(); s_normalMouseLook=true; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    foreground=false; pointerPos.x=900; pointerPos.y=60;
    clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(!s_normalLookCaptured && !pointerCaptured && pointerPos.x==900,"focus loss does not warp other app pointer"); foreground=true;
    mouseX=9000; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(near(camera.center.heading,0),"focus resume drops relative backlog");
    ++s_cameraFocusSerial; mouseX=9000; clockTick+=16000; normalMouseLookBeforeUpdate(&camera,true);
    check(near(camera.center.heading,0),"focus epoch catches loss while camera hook was stopped");
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
    camera.center.heading=2; clockTick+=20000; steeringFrame();
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
    check(!computeWASDDirection(false,true,false,false,curveDir),"A alone has no translation");
    check(!computeWASDDirection(true,true,true,false,curveDir),"W+S cancels translation");
    for(int i=0;i<50;++i) {
        camera.center.heading=i*0.1f; s_otsYaw=-i*0.05f;
        s_otsCamActive=(i%2)==0;
        computeWASDDirection(true,true,false,false,curveDir);
        check((curveDir-forwardDir).length()<0.0001f,"normal and OTS view yaw cannot feed steering");
    }
    s_otsCamActive=false;
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
    check(normalLookPitchDelta(-1.3f,-1)<0 && normalLookPitchDelta(-1.3f,-1)>-0.1f,"lower pitch clamp");
    check(normalLookPitchDelta(-0.1f,1)>0 && normalLookPitchDelta(-0.1f,1)<0.02f,"upper pitch clamp");
    check(normalLookPitchDelta(0,0.1f)==0 && near(normalLookPitchDelta(0,-0.02f),-0.02f),"out-of-range vanilla pitch returns without snapping");
    reset(); s_steeringMode=s_steeringValid=s_steeringIntent=true; s_steeringYaw=0.7f;
    frame(); for(int i=0;i<300;++i) frame();
    check(near(camera.center.heading,0.7f),"stationary intentional steering aligns camera to heading");
    reset(); frame(); Ogre::Vector3 heldBefore,heldAfter;
    computeWASDDirection(true,false,false,false,heldBefore);
    s_autoCameraEnabled=false; camera.center.heading=0.7f;
    frame(0.16f,0); computeWASDDirection(true,false,false,false,heldAfter);
    check(camera.center.turns==0 && (heldBefore-heldAfter).length()<0.0001f,"auto master off preserves held movement");
    keys.rotate=true; frame(0.16f,0,0.016f,0.2f); keys.rotate=false;
    check(near(camera.center.heading,0.9f),"manual rotation still works with auto master off");
    reset(); s_normalAutoAlign=false; frame(1,0); frame(1,0);
    check(camera.center.turns==0,"default disabled");
    Ogre::Vector3 vanillaDir;
    camera.center.heading=0.8f; computeWASDDirection(true,false,false,false,vanillaDir);
    check(near(atan2f(-vanillaDir.x,-vanillaDir.z),0.8f),"disabled uses live vanilla heading");
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
        check(near(camera.center.heading,atan2f(-expected.x,-expected.z)),"view converges to actual travel");
    }
    reset(); frame(); frame(0.16f,0); int turns=camera.center.turns;
    for(int i=0;i<20;++i) frame();
    check(camera.center.turns==turns,"stationary immediately stops turning");
    frame(0,0.16f,0.016f); check(camera.center.turns>turns,"travel resumes alignment");
    turns=camera.center.turns; frame(100,100); frame(1,0,0.5f);
    frame(0,0,0.016f); check(camera.center.turns==turns,"teleports and long stalls rejected");
    reset(); camera.center.heading=3.12413936f; frame();
    frame(sinf(0.01745329f)*0.16f,cosf(0.01745329f)*0.16f);
    check(camera.center.heading>3.12413936f && camera.center.heading<3.13f,"shortest turn across yaw seam");
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
    reset(); frame(); camera.center.heading=1;
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
    puts("PASS: steering, settings, runtime toggles, mouse capture lifecycle, normal alignment, input feedback and safety gates");
    return 0;
}
