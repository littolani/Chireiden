#pragma once
#include "AnmVm.h"
#include "AnmLoaded.h"
#include "Chireiden.h"
#include "Chain.h"
#include "Macros.h"
#include "ShotFile.h"
#include "Shottypes.h"
#include "Vectors.h"

struct BoundingBox3
{
    Float3 minPos;
    Float3 maxPos;
};

struct PlayerBullet
{
    Timer timer0;
    Float3 position;
    Float3 velocity;
    float speed;
    float angle;
    float smth;
    float d_smth_dt;
    int idk[2];
    uint8_t flags;
    uint8_t probablyPadding[3];
    int isActive;
    AnmId anmId4c;
    AnmId anmId50;
    int idk4[3];
    int damage;
    int idk5;
    Shooter* shooter;
};

struct PlayerAbility
{
    int anmSlotIndexMaybe;                     // <0x00>
    int unk0[6];                               // <0x04>
    float targetAngleUnfocused;                // <0x1c>
    float targetDistanceUnfocused;             // <0x20>
    float targetAngleFocused;                  // <0x24>
    float targetDistanceFocused;               // <0x28>
    int unk2;                                  // <0x2c>
    float someAngleFloat;                      // <0x30>
    int unk3[5];                               // <0x34> 
    AnmId anotherAnmId;                        // <0x48>
    int unk4;                                  // <0x4c>
    Int2 currentPosSubpixel;                   // <0x50>
    Int2 previousPosSubpixel;                  // <0x58>
    int g;                                     // <0x60>
    Int2 targetOffsetsUnfocused;               // <0x64>
    Int2 targetOffsetsFocused;                 // <0x6c>
    Int2 marisaBPreferredOffsetsByFormtion[5];
    int idk5[3];
    float angle;
    int idk6;
    AnmId anmIds[2];
    int e;
    int d;
    int c;
    int b;
    Float2 someOtherFloat;
    int optionId;
    int resetFlag;
    int idk8;
    void(*updateCallback)(PlayerAbility*);
    void(*drawCallback)(PlayerAbility*);
};
ASSERT_SIZE(PlayerAbility, 0xe4);

struct PlayerDamageSource
{
    float argf0;
    float argf1;
    int idk[4];
    Float3 centerPosition;
    int idk2[10];
    Timer timer;
    int argi1;
    int someInt;
    int someInt999999;
    int someInt6c;
    uint32_t flags;
};
ASSERT_SIZE(PlayerDamageSource, 0x74);

class EnemyManager;
class Player
{
public:
    uint32_t idk;                                          // <0x0>
    uint32_t maybeUnused;                                  // <0x4>
    ChainElem* onTick10;                                   // <0x8>
    ChainElem* onDraw16;                                   // <0xc>
    AnmLoaded* playerAnm;                                  // <0x10>
    AnmVm vm0;                                             // <0x14>
    AnmVm vm1;                                             // <0x448>
    Float3 position;                                       // <0x87c>
    Int2 posSubpixel;                                      // <0x888>
    int speedSubpixel;                                     // <0x990>
    int focusedSpeedSubpixel;                              // <0x894>
    int normalizedSpeedSubpixel;        // speed/sqrt(2)   // <0x898>
    int normalizedFocusedSpeedSubpixel; // speed/sqrt(2)   // <0x89c>
    Float3 attemptedDeltaPosSubpixel;                      // <0x8a0>
    Float3 lastNonzeroAttemptedDeltaPosSubpixel;           // <0x8ac>
    Int2 attemptedDeltaPosISubpixel;                       // <0x8b8>
    int idk2;                                              // <0x8c0>
    int idk3;                                              // <0x8c4>
    int idk4;                                              // <0x8c8>
    BoundingBox3 hurtBox;                                  // <0x8cc>
    Float3 hurtboxHalfSize;                                // <0x8e4>
    Float3 itemAttractBoxUnfocusedHalfSize;                // <0x8f0>
    Float3 itemAttractBoxFocusedHalfSize;                  // <0x8fc>
    int idk5[3];                                           // <0x908>
    Int2 attemptedVelocityInternal;                        // <0x914>
    int attemptedDirection;                                // <0x91c>
    int reimuAGappingState;                                // <0x920>
    int reimuAFramesInGappingState;                        // <0x924>
    int state;                                             // <0x928>
    ShotFile* shotFile;                                    // <0x92c>
    Timer timer0;                                          // <0x930>
    Timer timer1;                                          // <0x944>
    Timer timer2;                                          // <0x958>
    PlayerBullet playerBullets[255];                       // <0x96c>
    AnmId anmIdFocusedHitbox;                              // <0x7500>
    int unk[23];                                           // <0x7504>
    int anotherSpecialField;                               // <0x7560>
    int a;                                                 // <0x7564>
    int b;                                                 // <0x7568>
    AnmId vm_id_0x756c;                                    // <0x756c>
    PlayerAbility playerAbilities[8];                      // <0x7570>
    int reimu_c_flag_probably_0x7c90;                      // <0x7c90>
    int idk7;                                              // <0x7c94>
    uint8_t idk8;                                          // <0x7c98>
    uint8_t padding[3];                                    // <0x7c99>
    PlayerDamageSource damageSources[33];                  // <0x7c9c>
    int shooterOptions[4];                                 // <0x8b90>
    int percentMovedByOptions;                             // <0x8ba0>
    int reimuB_FramesWithoutInput;                         // <0x8ba4>
    int reimuC_FramesWithout_Z_Or_Shift;                   // <0x8ba8>
    int marisaB_Formation;                                 // <0x8bac>
    Timer timerIFrames;                                    // <0x8bb0>
    uint32_t someFlag;                                     // <0x8bc4>
    int someCounter;                                       // <0x8bc8>
    BoundingBox3 itemCollectBox;                           // <0x8bcc>
    BoundingBox3 itemAttractBoxFocused;                    // <0x8be4>
    BoundingBox3 itemAttractBoxUnfocused;                  // <0x8bfc>
    float reimuCOptionAngle;                               // <0x8c14>
    Int2 unused[33];                                       // <0x8c18>
    int isFocused;                                         // <0x8c20>

    // 0x433a90
    static PlayerDamageSource* createDamageSource(Player* This, Float3* x, float argF0, float argF1, int currentTime, int argi1);

    // 0x4327d0
    static void idk0(Player* This);

    // 0x42f8a0
    static int initialize(Player* This);

    // 0x431c70
    static int loadShotFile(Player* This, const char* filename);

    // 0x430290
    static void move(Player* This);

    // 0x42f760 
    // Player();

    // 0x430e50
    static void reimuATickGappingState(Player* This);

    // 0x430270
    static void release(Player* This);

    // 0x432cc0
    static void repopulateAbilities(Player* This);

    // 0x410610
    static void resetOptions(Player* This);

    // 0x410610
    static void setIframes(int currentTime);

    // 0x410610
    static void shoot(Player* This, int currentTime);

    // 0x434380
    static int shootingTick(Player* This);

    // 0x433f90
    static int shootOneBullet(Player* This, Float3* position, int currentTime, Shooter* shooter);

    // 0x434440
    static int tickBullets(Player* This);

    // 0x432a90
    static void timerRelated(Player* This, uint8_t di);

    // 0x432a90
    static int useBomb();

    static ChainCallbackResult __fastcall onTick10Stub(void* This)
    {
        return ChainCallbackResult::Continue; //onTick(reinterpret_cast<Player*>(This));
    }

    static ChainCallbackResult __fastcall onDraw16Stub(void* This)
    {
        return ChainCallbackResult::Continue; //onTick(reinterpret_cast<Player*>(This));
    }

    // 0x4336a0
    static void abilityDrawCallbackReimuA(PlayerAbility* ability);

    static void abilityCallbackReimuB(PlayerAbility* ability);
    static void abilityCallbackReimuC(PlayerAbility* ability);
    static void abilityCallbackMarisaA(PlayerAbility* ability);
    static void abilityCallbackMarisaB(PlayerAbility* ability);
    static void abilityCallbackMarisaC(PlayerAbility* ability);

};
ASSERT_SIZE(Player, 0x8d24);