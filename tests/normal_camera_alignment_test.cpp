// The runner extracts production functions verbatim; these fakes isolate their
// state/input decisions. Real Ogre transforms and in-game rendering need playtesting.
#include <cmath>
#include <float.h>
#include <cstdio>
#include <cstdlib>
#include <limits>
typedef unsigned long DWORD;
typedef long LONG;
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
struct LARGE_INTEGER { LONGLONG QuadPart; };
static LONGLONG clockTick = 1000000;
static LONGLONG qpcNow() { return clockTick; }
static bool QueryPerformanceFrequency(LARGE_INTEGER* f) { f->QuadPart = 1000000; return true; }
static ULONGLONG GetTickCount64() { return clockTick / 1000; }
namespace Ogre {
struct Vector3 {
    float x, y, z;
    Vector3(float a=0, float b=0, float c=0): x(a), y(b), z(c) {}
    Vector3 operator-(const Vector3& b) const { return Vector3(x-b.x,y-b.y,z-b.z); }
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
    float heading; int turns;
    SceneNode(): heading(0), turns(0) {}
    void yaw(Radian r, Node::TransformSpace space) {
        if (space != Node::TS_WORLD) abort();
        heading += r.value; ++turns;
    }
    void _getFullTransformUpdated() {}
};
struct Camera {
    SceneNode* parent;
    SceneNode* getParentSceneNode() { return parent; }
};
}
struct CharMovement { Ogre::Vector3 pos; };
struct Character {
    CharMovement* movement;
    bool dead, unconscious, protectedAnim, ragdoll, carried;
    Character(): movement(0), dead(false), unconscious(false), protectedAnim(false), ragdoll(false), carried(false) {}
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
        initialized(true), freeCamera(false), follow(0), pitch(0.7f), altitude(50) { realCamera.parent=&lens; }
    Ogre::Vector3 getFacingDirection() { return Ogre::Vector3(-sinf(center.heading),0,-cosf(center.heading)); }
    Ogre::SceneNode* getCenterNode() { return &center; }
    Ogre::SceneNode* getCameraNode() { return &lens; }
    bool isInitialised() { return initialized; } bool isFreeCameraMode() { return freeCamera; }
    Character* getFollowObject() { return follow; }
};
struct Player { CameraClass* camera; };
struct World {
    Player* player; bool loading, paused;
    bool isLoadingFromASaveGame() { return loading; } bool isPaused() { return paused; }
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
    normalResetAutoAlign(); camera.center.heading=0; camera.center.turns=0;
    camera.isRotating=false; camera.follow=&character;
    player.camera=&camera; world.player=&player; world.loading=world.paused=false;
    character.movement=&movement; movement.pos=Ogre::Vector3::ZERO;
    s_freeMoveAnchor=&character; s_anchorMovement=&movement;
    s_normalAutoAlign=true; s_wHeld=true; s_aHeld=s_sHeld=s_dHeld=false;
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
    puts("PASS: normal camera convergence, held WASD, manual input, discontinuities, mode/UI/load/liveness gates");
    return 0;
}
