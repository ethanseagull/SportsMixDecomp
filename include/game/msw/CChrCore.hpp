#ifndef MSW_CCHRCORE_HPP_
#define MSW_CCHRCORE_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrDrv.hpp"

struct _GXTexObj;

namespace bcs {
    class CMdlObj;
    class CTask;
}

namespace msw {

    class CBall;
    class CChrDrv;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CCollDefUnit;
    class CDropCoin;
    class CItem;
    class CQPanel;

    // One character on the court. A DOL class; each game module derives its
    // own (CChrCoreBoss, CChrCoreBasket, ...). bcs::CFsmQueue<CChrCore> is at
    // 0x00 and the two share the vptr at 0x5C; own members start at 0x60.
    //
    // INCOMPLETE: only what game/boss/CChrMarkerBoss.cpp needs is here.
    //  - Only some members are named; the rest up to the size, 0x574, is
    //    padding, so that new of a derived class gets the right size. The
    //    class must not be instantiated or embedded until it is mapped.
    //  - The virtual functions are all declared, 0x08 to 0xC98 (the vtable
    //    of a derived class goes on from 0xC9C). Their order
    //    and names are the slots of __vt__Q23msw12CChrCoreBoss (BossF), read
    //    with the class name set back to CChrCore. Most of them are inline
    //    functions in the DOL (the modules emit weak copies); they are
    //    declared without bodies here, so a derived class's vtable refers to
    //    them by name only.
    //  - Return types are not part of the mangled names: "int" stands for a
    //    DOL body that returns a word, "float" for one that loads f1, "void"
    //    for an empty one. All are guesses, as are the member names.
    class CChrCore : public bcs::CFsmQueue<CChrCore> {
        public:
            // (contents not known; the MethdRDB slots take a pointer to one)
            struct INPUT_WORK;
            // (contents not known) The work of the damage state, passed to the
            // SubStateDamage* virtuals.
            struct STATE_WORK_DAMAGE;
            // Kinds of effect for Util_SetEffect / Util_AttachEffect (values not known).
            enum SET_EFFECT_TYPE {};
            enum ATTACH_EFFECT_TYPE {};

            CChrCore();
            virtual ~CChrCore();                                     // 0x08
            virtual void DispFunc();                                 // 0x0C
            virtual long GetQueueWorkSize() const;                   // 0x10
            virtual long GetQueueStackSize() const;                  // 0x14
            virtual long GetMainOrderValue(long);                    // 0x18
            virtual void *GetHoldBall() const;                       // 0x1C
            virtual unsigned long GetHoldBallHandle() const;         // 0x20
            virtual void SetConditionEffect_(long);                  // 0x24
            virtual void SetConditionEffect_(long, unsigned long);   // 0x28
            virtual void GetSpRootPos(bcs::FVEC3 &) const;           // 0x2C
            virtual int IsNowBlockWork() const;                      // 0x30
            virtual int IsNowFallOut() const;                        // 0x34
            virtual int IsHoldBall() const;                          // 0x38
            virtual int IsHoldItem() const;                          // 0x3C
            virtual int IsHoldAny() const;                           // 0x40
            virtual int Is1on1() const;                              // 0x44
            virtual float Get1on1StartDist() const;                  // 0x48
            virtual int IsAirTrickSwingEnd() const;                  // 0x4C
            virtual int IsAirTrickFall() const;                      // 0x50
            virtual float GetAirTrickStatePassFrame() const;         // 0x54
            virtual void GetTargetMarkerPos(bcs::FVEC3 &);           // 0x58
            virtual int MoveTargetMarkerPos(float);                  // 0x5C
            virtual int AutoMoveTargetMarkerPos();                   // 0x60
            virtual long CheckGuideTelopDisp();                      // 0x64
            virtual int IsBattle() const;                            // 0x68
            virtual int IsDisp1On1Maker() const;                     // 0x6C
            virtual int IsDunkBlockEnable() const;                   // 0x70
            virtual int CanbePassRequest();                          // 0x74
            virtual int MethdPassRequest();                          // 0x78
            virtual float GetBlockWorkDir() const;                   // 0x7C
            virtual float GetJumpBlockLocalFrame() const;            // 0x80
            virtual int GetStarRemainderRate() const;                // 0x84
            virtual int IsAirTrickDunkMode() const;                  // 0x88
            virtual int IsVolleyRightBallTouch(long) const;          // 0x8C
            virtual int IsVolleyServeMode() const;                   // 0x90
            virtual int IsVolleyJumpServeMode() const;               // 0x94
            virtual int IsVolleyTargetMarkerActive() const;          // 0x98
            virtual long GetVolleyTargetMarkerNum() const;           // 0x9C
            virtual void GetVolleyTargetMarkerDefaultPos(long, bcs::FVEC3 &, int); // 0xA0
            virtual long GetVolleyTargetMarkerType() const;          // 0xA4
            virtual long GetVolleyTargetMarkerFixedType() const;     // 0xA8
            virtual int GetVolleyTargetMarkerPos(bcs::FVEC3 &, long); // 0xAC
            virtual int GetVolleyTargetMarkerMoved(bcs::FVEC3 &);    // 0xB0
            virtual int MoveVolleyTargetMarkerPos(float);            // 0xB4
            virtual int CanbeMoveVolleyTargetMarkerPos();            // 0xB8
            virtual int IsVolleyTargetMarkerStealthMode();           // 0xBC
            virtual float GetVolleySpikeAdsorbRangeXZ(int) const;    // 0xC0
            virtual float GetVolleySpikeAdsorbRangeY(int) const;     // 0xC4
            virtual int IsVolleySpikeJumpStart() const;              // 0xC8
            virtual int IsVolleySuperSpikeJump() const;              // 0xCC
            virtual int IsVolleySpikeSwingEnd() const;               // 0xD0
            virtual int IsVolleySpikeFall() const;                   // 0xD4
            virtual int IsVolleyAirTrickSwingEnd() const;            // 0xD8
            virtual int IsVolleyAirTrickFall() const;                // 0xDC
            virtual int IsVolleyBlockJumpStart() const;              // 0xE0
            virtual int IsVolleyBlockFall() const;                   // 0xE4
            virtual int IsVolleyBlockSuccess() const;                // 0xE8
            virtual int IsVolleyDispSpikeJumpMarker() const;         // 0xEC
            // 0xF0-0x250: names from __vt__Q23msw13CChrCoreDodge (DodgeF);
            // return types from the weak CChrCore bodies the modules emit.
            virtual int GetVolleySpikeFeintTargetPos(float, bcs::FVEC3 &, bcs::FVEC3 &, bcs::FVEC3 &); // 0xF0
            virtual int GetVolleyBallShotCollPos(long, bcs::FVEC3 &, float &) const; // 0xF4
            virtual void GetVolleySpikeOffsetPos(long, bcs::FVEC3 &) const; // 0xF8
            virtual CBall *GetVolleyTargetBall() const;              // 0xFC
            virtual int IsVolleySuperSpikeCurveType() const;         // 0x100
            virtual float GetVolleySuperSpikeCurveRate() const;      // 0x104
            virtual float GetVolleySuperSpikeCurveMaxDist() const;   // 0x108
            virtual float GetVolleySuperSpikeCurveMinDist() const;   // 0x10C
            virtual float GetVolleyBlockJumpDelayFrame() const;      // 0x110
            virtual int IsVolleyEnableDiveReceive() const;           // 0x114
            virtual int IsVolleyCombinationMode() const;             // 0x118
            virtual long GetVolleyCombinationRole() const;           // 0x11C
            virtual long GetVolleyCombinationStep() const;           // 0x120
            virtual void SetVolleyCombinationRoleDown();             // 0x124
            virtual int IsVolleyDispExIcon(long) const;              // 0x128
            virtual long CheckVolleyLineMsgDisp();                   // 0x12C
            virtual int IsVolleyCombinationReceiver() const;         // 0x130
            virtual int IsVolleyCombinationAttacker() const;         // 0x134
            virtual int IsVolleyArtsKoopaShotFire() const;           // 0x138
            virtual int IsVolleyArtsKoopaFireEnd() const;            // 0x13C
            virtual long GetVolleyArtsExInput();                     // 0x140
            virtual int IsHockeyChgArrowMarker();                    // 0x144
            virtual float GetHockeyArrowMarkerRotY() const;          // 0x148
            virtual int IsHockeyTackleMarker() const;                // 0x14C
            virtual int IsHockeyTgtArrowMarker() const;              // 0x150
            virtual void GetHockeyShootGoalTgtPos(bcs::FVEC3 &, float &); // 0x154
            virtual int IsHockeyDispShootMarker();                   // 0x158
            virtual void SetHockeyNoChargeCutFrame(float);           // 0x15C
            virtual float GetHockeyNoChargeCutFrame();               // 0x160
            virtual float GetShootChargeFrameMax() const;            // 0x164
            virtual int GetHockeyKeeperTgtPos(bcs::FVEC3 &);         // 0x168
            virtual int IsKeeper() const;                            // 0x16C
            virtual void SetKeeper(int);                             // 0x170
            virtual void SetMaxSkateSpeed(float);                    // 0x174
            virtual float GetMaxSkateSpeed() const;                  // 0x178
            virtual float GetHockeyVelXZ() const;                    // 0x17C
            virtual float GetBallScramblePoint() const;              // 0x180
            virtual float GetBallScrambleNowPoint() const;           // 0x184
            virtual float GetBallScrambleWinPoint() const;           // 0x188
            virtual float GetBallScrambleMaxPoint() const;           // 0x18C
            virtual int GetBodyContactSkate() const;                 // 0x190
            virtual void SetBodyContactSkate(int);                   // 0x194
            virtual void SetPackNotCatchFrame(float);                // 0x198
            virtual int IsHockeyBodyNoPush() const;                  // 0x19C
            virtual long GetHockeyMaxShootCourseDirPercent();        // 0x1A0
            virtual int GetHockeyShootGoalTgtPos(bcs::FVEC3 &);      // 0x1A4
            virtual int MethdHockeyKeeperQuickMove(const bcs::FVEC3 &); // 0x1A8
            virtual int IsHockeyDamage() const;                      // 0x1AC
            virtual int IsHockeyBallHit();                           // 0x1B0
            virtual int IsHockeyAttack() const;                      // 0x1B4
            virtual int IsHockeyFighting() const;                    // 0x1B8
            virtual void HockeyChrReflect(bcs::FVEC3 &, CChrCore *); // 0x1BC
            virtual long Get1on1AttackHitType() const;               // 0x1C0
            virtual long GetNow1on1AttackNum() const;                // 0x1C4
            virtual float Get1on1AttackWaitTimeMax() const;          // 0x1C8
            virtual void SetUseShootMarker(int);                     // 0x1CC
            virtual int IsUseShootMarker() const;                    // 0x1D0
            virtual int MethdHockeySetMoveRevise(float, float);      // 0x1D4
            virtual void HockeyArtsEnd();                            // 0x1D8
            virtual int CanbeKeeperDivingCatch();                    // 0x1DC
            virtual int MethdKeeperDivingCatch();                    // 0x1E0
            virtual float GetAttackPower(int);                       // 0x1E4
            virtual long GetDamageCollKind() const;                  // 0x1E8
            virtual float GetHockeyBodyCollRadiusNow() const;        // 0x1EC
            virtual float GetHockeyMass() const;                     // 0x1F0
            virtual float GetFaceOffChargeMaxFrame() const;          // 0x1F4
            virtual long GetTackleCount(long) const;                 // 0x1F8
            virtual void SetAddTackleCount(long);                    // 0x1FC
            virtual void ReSetTackleCount(long);                     // 0x200
            virtual int CanbeHockeyComBlockAttack();                 // 0x204
            virtual int IsHockeyInvisible() const;                   // 0x208
            virtual void SetHockeyAttackObjNull(long);               // 0x20C
            virtual int GetHockeyAttackObjInfo(long, bcs::FVEC3 &, float &, bcs::FVEC3 &); // 0x210
            virtual long GetHockeyAttackObjMax();                    // 0x214
            virtual void SetKeeperCatchMode();                       // 0x218
            virtual float GetHockeyPassSpeed();                      // 0x21C
            virtual float GetHockeyKeeperPassSpeed();                // 0x220
            virtual int IsHockeyStatePaintTumble() const;            // 0x224
            virtual void SetDodgeArrangeArea(long);                  // 0x228
            virtual long GetDdogeArrangeArea() const;                // 0x22C (sic)
            virtual int IsDodgeInfield() const;                      // 0x230
            virtual int IsDodgeCloseRange() const;                   // 0x234
            virtual float GetDodgeCatchCollRadius() const;           // 0x238
            virtual long GetDodgeLockTargetChrIdx() const;           // 0x23C
            virtual void SwitchAutoAimTarget();                      // 0x240
            virtual int IsDodgeTargetMarkerActive();                 // 0x244
            virtual long GetDodgeTargetMarkerType() const;           // 0x248
            virtual long GetDodgeTargetMarkerFixedType() const;      // 0x24C
            // Not zero when it wrote the marker's position to the vector.
            virtual int GetDodgeTargetMarkerPos(bcs::FVEC3 &);       // 0x250
            // 0x254 to 0x844: names and parameters from the slots of
            // __vt__Q23msw13CChrCoreDodge (DodgeF), class name set back to
            // CChrCore. Return types are guesses from the body each slot
            // points at (f1 set: float; r3 set: int, long for Get*;
            // otherwise void or a guess from the name).
            virtual void DodgeForceUpdateAreaCode();                  // 0x254
            virtual int IsDodgeOnBallJump() const;                    // 0x258
            virtual int IsDodgeOnBallJumpFall() const;                // 0x25C
            virtual int IsDodgeJumpBallChr() const;                   // 0x260
            virtual void SetDodgeJumpBallChr(int);                    // 0x264
            virtual float GetDodgeDivingCatchDist() const;            // 0x268
            virtual int IsDodgeArtsTargetMarkerActive() const;        // 0x26C
            virtual long GetDodgeArtsTargetMarkerType() const;         // 0x270
            virtual long GetDodgeArtsTargetMarkerPos(bcs::FVEC3 &, long); // 0x274
            virtual long GetDodgeArtsTargetMarkerNum() const;          // 0x278
            virtual void GetDodgeArtsTargetMarkerDefaultPos(bcs::FVEC3 &, long); // 0x27C
            virtual long GetDodgeArtsTargetMarkerMoved(bcs::FVEC3 &);   // 0x280
            virtual void SetDodgeArtsTargetMarkerPosClip(bcs::FVEC3 &); // 0x284
            virtual int MoveDodgeArtsTargetMarkerPos(float);          // 0x288
            virtual int CanbeMoveDodgeArtsTargetMarkerPos();          // 0x28C
            virtual void SetDodgeArtsTargetMarkerDisable();           // 0x290
            virtual void GetVel(bcs::FVEC3 &) const;                   // 0x294
            virtual void SetVel(bcs::FVEC3 &, float);                  // 0x298
            virtual void SetSwitch(int);                              // 0x29C
            virtual int IsSwitch() const;                             // 0x2A0
            virtual void SetNoReflectFrame(float);                    // 0x2A4
            virtual float GetNoReflectFrame() const;                  // 0x2A8
            virtual void SetReflectNoMoveFrame(float);                // 0x2AC
            virtual float GetReflectNoMoveFrame() const;              // 0x2B0
            virtual void SetAutoBrakeStartTime(float);                // 0x2B4
            virtual float GetAutoBrakeStartTime() const;              // 0x2B8
            virtual float GetKeeperDivingCatchSpeed(int) const;       // 0x2BC
            virtual float GetHockeyKeeperDivingSteeleDist(int) const; // 0x2C0
            virtual float GetHockeyKeeperDivingSteeleGoalFrame(int) const; // 0x2C4
            virtual void SetHockeySkateWork(float, float, float, float); // 0x2C8
            virtual int IsHockeyNowCombinationMode() const;           // 0x2CC
            virtual int IsCombinationShootStance() const;             // 0x2D0
            virtual void GetForceInfo(bcs::FVEC3 &, float &);           // 0x2D4
            virtual void CrashSkateMissionSetReady(int);              // 0x2D8
            virtual int IsHockeyNowKeeperMoveMode() const;            // 0x2DC
            virtual int IsHockeyTackleSteal() const;                  // 0x2E0
            virtual int IsVsBossDead() const;                         // 0x2E4
            virtual long GetVsBossMaxHP() const;                       // 0x2E8
            virtual long GetVsBossNowHP() const;                       // 0x2EC
            virtual float GetVsBossHPRate() const;                    // 0x2F0
            virtual void PlayRunSe(unsigned long);                    // 0x2F4
            virtual void PlayWalkSe(unsigned long);                   // 0x2F8
            virtual void PlaySkateSe(unsigned long);                  // 0x2FC
            virtual void PlayTurnSe(unsigned long);                   // 0x300
            virtual void PlayBrakeSe(unsigned long);                  // 0x304
            virtual void PlayLandSe(unsigned long);                   // 0x308
            virtual void PlayJumpSe(unsigned long);                   // 0x30C
            virtual void PlayFtSe_(long, unsigned long);              // 0x310
            virtual void PlayFsSe_(long, unsigned long);              // 0x314
            virtual void SE_PlayArtsJumpSE(long);                     // 0x318
            virtual void SE_PlayArtsShootSE(const bcs::FVEC3 &);       // 0x31C
            virtual int SE_PlayArtsLoopSE();                          // 0x320
            virtual void SE_PlayArtsLandSE();                         // 0x324
            virtual float GetTargetGoalXLength() const;               // 0x328
            virtual float GetTargetGoalZLength() const;               // 0x32C
            virtual float GetGuardGoalXLength() const;                // 0x330
            virtual float GetGuardGoalZLength() const;                // 0x334
            virtual void CheckMapHit(bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &); // 0x338
            virtual float GetCollSize_BodyDamage() const;             // 0x33C (float: CComManVolleyRR::IsExistItemBall)
            virtual float GetCollSize_BodyPush() const;               // 0x340
            virtual void GetAlleyoopPassTargetPos(bcs::FVEC3 &, bcs::FVEC3 &, bcs::FVEC3 &) const; // 0x344
            virtual int Util_GetIdleMotID();                          // 0x348
            virtual int Util_GetRunMotID();                           // 0x34C
            virtual int Util_GetWalkMotID();                          // 0x350
            virtual int Util_GetLandMotID(long);                      // 0x354
            virtual int Util_GetJumpUpMotID(int);                     // 0x358
            virtual void Util_GetJumpLandMotID(int);                  // 0x35C
            virtual int Util_GetDamageMotID(long, long, long);        // 0x360
            virtual void Util_SetOffenseLeaderFirstState(long);       // 0x364
            virtual void Util_SetDefenseLeaderFirstState();           // 0x368
            virtual void Util_AddCoin(long);                          // 0x36C
            virtual void CreateMdl_();                                // 0x370
            virtual void CreateColl_();                               // 0x374
            virtual void Create_();                                   // 0x378
            virtual void MainFunc_();                                 // 0x37C
            virtual void UpdateFunc_();                               // 0x380
            virtual void Reset_();                                    // 0x384
            virtual void ResetCmn_();                                 // 0x388
            virtual float GetPrmMoveSpeed() const;                    // 0x38C
            virtual long GetPrmBodySize() const;                      // 0x390
            virtual void SendMessage_(long, long, long, long);        // 0x394
            virtual void SetScoreBookCountTeamKind_();                // 0x398
            virtual int IsScoreBookCountMode_() const;                // 0x39C
            virtual int IsScoreBookCountChr_() const;                 // 0x3A0
            virtual int IsScoreBookCountKindMyTeam_() const;          // 0x3A4
            virtual int IsScoreBookCountKindEnTeam_() const;          // 0x3A8
            virtual void AddScoreBookCount_(long, long, long);        // 0x3AC
            virtual void CtorStateDefault();                          // 0x3B0
            virtual void LoopStateDefault();                          // 0x3B4
            virtual void DtorStateDefault();                          // 0x3B8
            virtual void CtorStateNeutral();                          // 0x3BC
            virtual void LoopStateNeutral();                          // 0x3C0
            virtual void DtorStateNeutral();                          // 0x3C4
            virtual void CtorStateBlockWork();                        // 0x3C8
            virtual void LoopStateBlockWork();                        // 0x3CC
            virtual void DtorStateBlockWork();                        // 0x3D0
            virtual void CtorStateAutoBlockWork();                    // 0x3D4
            virtual void LoopStateAutoBlockWork();                    // 0x3D8
            virtual void DtorStateAutoBlockWork();                    // 0x3DC
            virtual void CtorStateRun();                              // 0x3E0
            virtual void LoopStateRun();                              // 0x3E4
            virtual void DtorStateRun();                              // 0x3E8
            virtual void CtorStateJump();                             // 0x3EC
            virtual void LoopStateJump();                             // 0x3F0
            virtual void DtorStateJump();                             // 0x3F4
            virtual void CtorStateDamage();                           // 0x3F8
            virtual void LoopStateDamage();                           // 0x3FC
            virtual void DtorStateDamage();                           // 0x400
            virtual void CtorStateSideStep();                         // 0x404
            virtual void LoopStateSideStep();                         // 0x408
            virtual void DtorStateSideStep();                         // 0x40C
            virtual void CtorStateItemShoot();                        // 0x410
            virtual void LoopStateItemShoot();                        // 0x414
            virtual void DtorStateItemShoot();                        // 0x418
            virtual void CtorStateBlockCut();                         // 0x41C
            virtual void LoopStateBlockCut();                         // 0x420
            virtual void DtorStateBlockCut();                         // 0x424
            virtual void CtorStateChargeCut();                        // 0x428
            virtual void LoopStateChargeCut();                        // 0x42C
            virtual void DtorStateChargeCut();                        // 0x430
            virtual void CtorStateJumpUpper();                        // 0x434
            virtual void LoopStateJumpUpper();                        // 0x438
            virtual void DtorStateJumpUpper();                        // 0x43C
            virtual void CtorStateDropCut();                          // 0x440
            virtual void LoopStateDropCut();                          // 0x444
            virtual void DtorStateDropCut();                          // 0x448
            virtual void CtorStateMotPlay();                          // 0x44C
            virtual void LoopStateMotPlay();                          // 0x450
            virtual void DtorStateMotPlay();                          // 0x454
            virtual void CtorStateSizeChange();                       // 0x458
            virtual void LoopStateSizeChange();                       // 0x45C
            virtual void DtorStateSizeChange();                       // 0x460
            virtual void CtorStateFreeze();                           // 0x464
            virtual void LoopStateFreeze();                           // 0x468
            virtual void DtorStateFreeze();                           // 0x46C
            virtual void CtorStateDizzy();                            // 0x470
            virtual void LoopStateDizzy();                            // 0x474
            virtual void DtorStateDizzy();                            // 0x478
            virtual void CtorStateSliding();                          // 0x47C
            virtual void LoopStateSliding();                          // 0x480
            virtual void DtorStateSliding();                          // 0x484
            virtual void CtorStateParalyze();                         // 0x488
            virtual void LoopStateParalyze();                         // 0x48C
            virtual void DtorStateParalyze();                         // 0x490
            virtual void CtorStateMameChange();                       // 0x494
            virtual void LoopStateMameChange();                       // 0x498
            virtual void DtorStateMameChange();                       // 0x49C
            virtual void CtorStateEventDoll();                        // 0x4A0
            virtual void LoopStateEventDoll();                        // 0x4A4
            virtual void DtorStateEventDoll();                        // 0x4A8
            virtual void CtorStatePass();                             // 0x4AC
            virtual void LoopStatePass();                             // 0x4B0
            virtual void DtorStatePass();                             // 0x4B4
            virtual void CtorStateAerialPass();                       // 0x4B8
            virtual void LoopStateAerialPass();                       // 0x4BC
            virtual void DtorStateAerialPass();                       // 0x4C0
            virtual void CtorStatePassCatch();                        // 0x4C4
            virtual void LoopStatePassCatch();                        // 0x4C8
            virtual void DtorStatePassCatch();                        // 0x4CC
            virtual void CtorStateArtsPass();                         // 0x4D0
            virtual void LoopStateArtsPass();                         // 0x4D4
            virtual void DtorStateArtsPass();                         // 0x4D8
            virtual void CtorStateSDB();                              // 0x4DC
            virtual void LoopStateSDB();                              // 0x4E0
            virtual void DtorStateSDB();                              // 0x4E4
            virtual void CtorStateSDB_Chain();                        // 0x4E8
            virtual void LoopStateSDB_Chain();                        // 0x4EC
            virtual void DtorStateSDB_Chain();                        // 0x4F0
            virtual void CtorStateRDB();                              // 0x4F4
            virtual void LoopStateRDB();                              // 0x4F8
            virtual void DtorStateRDB();                              // 0x4FC
            virtual void CtorStateRDB_Chain();                        // 0x500
            virtual void LoopStateRDB_Chain();                        // 0x504
            virtual void DtorStateRDB_Chain();                        // 0x508
            virtual void CtorStateQuickTurn();                        // 0x50C
            virtual void LoopStateQuickTurn();                        // 0x510
            virtual void DtorStateQuickTurn();                        // 0x514
            virtual void CtorStateTrickMove();                        // 0x518
            virtual void LoopStateTrickMove();                        // 0x51C
            virtual void DtorStateTrickMove();                        // 0x520
            virtual void CtorStateTrickDance();                       // 0x524
            virtual void LoopStateTrickDance();                       // 0x528
            virtual void DtorStateTrickDance();                       // 0x52C
            virtual void CtorStateCatch();                            // 0x530
            virtual void LoopStateCatch();                            // 0x534
            virtual void DtorStateCatch();                            // 0x538
            virtual void CtorStateSDB_Catch();                        // 0x53C
            virtual void LoopStateSDB_Catch();                        // 0x540
            virtual void DtorStateSDB_Catch();                        // 0x544
            virtual void CtorStateJumpShoot();                        // 0x548
            virtual void LoopStateJumpShoot();                        // 0x54C
            virtual void DtorStateJumpShoot();                        // 0x550
            virtual void CtorStateAerialShoot();                      // 0x554
            virtual void LoopStateAerialShoot();                      // 0x558
            virtual void DtorStateAerialShoot();                      // 0x55C
            virtual void CtorStateReboundDunk();                      // 0x560
            virtual void LoopStateReboundDunk();                      // 0x564
            virtual void DtorStateReboundDunk();                      // 0x568
            virtual void CtorStateLayUp();                            // 0x56C
            virtual void LoopStateLayUp();                            // 0x570
            virtual void DtorStateLayUp();                            // 0x574
            virtual void CtorStateAlleyoop();                         // 0x578
            virtual void LoopStateAlleyoop();                         // 0x57C
            virtual void DtorStateAlleyoop();                         // 0x580
            virtual void CtorStateAirTrick();                         // 0x584
            virtual void LoopStateAirTrick();                         // 0x588
            virtual void DtorStateAirTrick();                         // 0x58C
            virtual void CtorStateDunk();                             // 0x590
            virtual void LoopStateDunk();                             // 0x594
            virtual void DtorStateDunk();                             // 0x598
            virtual void CtorStateItemBallAttack();                   // 0x59C
            virtual void LoopStateItemBallAttack();                   // 0x5A0
            virtual void DtorStateItemBallAttack();                   // 0x5A4
            virtual void CtorStateBigDunk();                          // 0x5A8
            virtual void LoopStateBigDunk();                          // 0x5AC
            virtual void DtorStateBigDunk();                          // 0x5B0
            virtual void CtorStateServe();                            // 0x5B4
            virtual void LoopStateServe();                            // 0x5B8
            virtual void DtorStateServe();                            // 0x5BC
            virtual void CtorStateReceive();                          // 0x5C0
            virtual void LoopStateReceive();                          // 0x5C4
            virtual void DtorStateReceive();                          // 0x5C8
            virtual void CtorStateSpike();                            // 0x5CC
            virtual void LoopStateSpike();                            // 0x5D0
            virtual void DtorStateSpike();                            // 0x5D4
            virtual void CtorStateBlockWorkLose();                    // 0x5D8
            virtual void LoopStateBlockWorkLose();                    // 0x5DC
            virtual void DtorStateBlockWorkLose();                    // 0x5E0
            virtual void CtorStateAutoDribblePosture();               // 0x5E4
            virtual void LoopStateAutoDribblePosture();               // 0x5E8
            virtual void DtorStateAutoDribblePosture();               // 0x5EC
            virtual void CtorStateJumpBlock();                        // 0x5F0
            virtual void LoopStateJumpBlock();                        // 0x5F4
            virtual void DtorStateJumpBlock();                        // 0x5F8
            virtual void CtorStateSwatIt();                           // 0x5FC
            virtual void LoopStateSwatIt();                           // 0x600
            virtual void DtorStateSwatIt();                           // 0x604
            virtual void CtorStateSmashCut();                         // 0x608
            virtual void LoopStateSmashCut();                         // 0x60C
            virtual void DtorStateSmashCut();                         // 0x610
            virtual void CtorStateArtsShotCatch();                    // 0x614
            virtual void LoopStateArtsShotCatch();                    // 0x618
            virtual void DtorStateArtsShotCatch();                    // 0x61C
            virtual void CtorStateDash();                             // 0x620
            virtual void LoopStateDash();                             // 0x624
            virtual void DtorStateDash();                             // 0x628
            virtual void CtorStateJumpPass();                         // 0x62C
            virtual void LoopStateJumpPass();                         // 0x630
            virtual void DtorStateJumpPass();                         // 0x634
            virtual void CtorStateJumpCatch();                        // 0x638
            virtual void LoopStateJumpCatch();                        // 0x63C
            virtual void DtorStateJumpCatch();                        // 0x640
            virtual void CtorStateDribbleMiss();                      // 0x644
            virtual void LoopStateDribbleMiss();                      // 0x648
            virtual void DtorStateDribbleMiss();                      // 0x64C
            virtual void CtorStateBallCatch();                        // 0x650
            virtual void LoopStateBallCatch();                        // 0x654
            virtual void DtorStateBallCatch();                        // 0x658
            virtual void CtorStateBallCatchBack();                    // 0x65C
            virtual void LoopStateBallCatchBack();                    // 0x660
            virtual void DtorStateBallCatchBack();                    // 0x664
            virtual void CtorStateStagger();                          // 0x668
            virtual void LoopStateStagger();                          // 0x66C
            virtual void DtorStateStagger();                          // 0x670
            virtual void CtorStateImpact();                           // 0x674
            virtual void LoopStateImpact();                           // 0x678
            virtual void DtorStateImpact();                           // 0x67C
            virtual void CtorStateTurn();                             // 0x680
            virtual void LoopStateTurn();                             // 0x684
            virtual void DtorStateTurn();                             // 0x688
            virtual void CtorStateSpinStagger();                      // 0x68C
            virtual void LoopStateSpinStagger();                      // 0x690
            virtual void DtorStateSpinStagger();                      // 0x694
            virtual void CtorStateArtsOut();                          // 0x698
            virtual void LoopStateArtsOut();                          // 0x69C
            virtual void DtorStateArtsOut();                          // 0x6A0
            virtual void CtorStateLineOver();                         // 0x6A4
            virtual void LoopStateLineOver();                         // 0x6A8
            virtual void DtorStateLineOver();                         // 0x6AC
            virtual void CtorStateCrouch();                           // 0x6B0
            virtual void LoopStateCrouch();                           // 0x6B4
            virtual void DtorStateCrouch();                           // 0x6B8
            virtual void CtorStateDivingCatch();                      // 0x6BC
            virtual void LoopStateDivingCatch();                      // 0x6C0
            virtual void DtorStateDivingCatch();                      // 0x6C4
            virtual void CtorStateSpinAttack();                       // 0x6C8
            virtual void LoopStateSpinAttack();                       // 0x6CC
            virtual void DtorStateSpinAttack();                       // 0x6D0
            virtual void CtorStateSkate();                            // 0x6D4
            virtual void LoopStateSkate();                            // 0x6D8
            virtual void DtorStateSkate();                            // 0x6DC
            virtual void CtorStateStealStance();                      // 0x6E0
            virtual void LoopStateStealStance();                      // 0x6E4
            virtual void DtorStateStealStance();                      // 0x6E8
            virtual void CtorStateShootStance();                      // 0x6EC
            virtual void LoopStateShootStance();                      // 0x6F0
            virtual void DtorStateShootStance();                      // 0x6F4
            virtual void CtorStateBrake();                            // 0x6F8
            virtual void LoopStateBrake();                            // 0x6FC
            virtual void DtorStateBrake();                            // 0x700
            virtual void CtorStateSwitch();                           // 0x704
            virtual void LoopStateSwitch();                           // 0x708
            virtual void DtorStateSwitch();                           // 0x70C
            virtual void CtorStateBlockStance();                      // 0x710
            virtual void LoopStateBlockStance();                      // 0x714
            virtual void DtorStateBlockStance();                      // 0x718
            virtual void CtorStateChangeStance();                     // 0x71C
            virtual void LoopStateChangeStance();                     // 0x720
            virtual void DtorStateChangeStance();                     // 0x724
            virtual void CtorStateAutoChangeStance();                 // 0x728
            virtual void LoopStateAutoChangeStance();                 // 0x72C
            virtual void DtorStateAutoChangeStance();                 // 0x730
            virtual void CtorStateDirectShootStance();                // 0x734
            virtual void LoopStateDirectShootStance();                // 0x738
            virtual void DtorStateDirectShootStance();                // 0x73C
            virtual void CtorStateDirectPassStance();                 // 0x740
            virtual void LoopStateDirectPassStance();                 // 0x744
            virtual void DtorStateDirectPassStance();                 // 0x748
            virtual void CtorStateTackleGuard();                      // 0x74C
            virtual void LoopStateTackleGuard();                      // 0x750
            virtual void DtorStateTackleGuard();                      // 0x754
            virtual void CtorStateFallOut();                          // 0x758
            virtual void LoopStateFallOut();                          // 0x75C
            virtual void DtorStateFallOut();                          // 0x760
            virtual void CtorStateFallSafe();                         // 0x764
            virtual void LoopStateFallSafe();                         // 0x768
            virtual void DtorStateFallSafe();                         // 0x76C
            virtual void CtorStateSmashAttack();                      // 0x770
            virtual void LoopStateSmashAttack();                      // 0x774
            virtual void DtorStateSmashAttack();                      // 0x778
            virtual void CtorStateBallScramble();                     // 0x77C
            virtual void LoopStateBallScramble();                     // 0x780
            virtual void DtorStateBallScramble();                     // 0x784
            virtual void CtorStateFaceOff();                          // 0x788
            virtual void LoopStateFaceOff();                          // 0x78C
            virtual void DtorStateFaceOff();                          // 0x790
            virtual void CtorStateDivingSteal();                      // 0x794
            virtual void LoopStateDivingSteal();                      // 0x798
            virtual void DtorStateDivingSteal();                      // 0x79C
            virtual void CtorStateBodyContact();                      // 0x7A0
            virtual void LoopStateBodyContact();                      // 0x7A4
            virtual void DtorStateBodyContact();                      // 0x7A8
            virtual void CtorStateOffBallStance();                    // 0x7AC
            virtual void LoopStateOffBallStance();                    // 0x7B0
            virtual void DtorStateOffBallStance();                    // 0x7B4
            virtual void CtorStateAttackFight();                      // 0x7B8
            virtual void LoopStateAttackFight();                      // 0x7BC
            virtual void DtorStateAttackFight();                      // 0x7C0
            virtual void CtorStateKeeperCatchAction();                // 0x7C4
            virtual void LoopStateKeeperCatchAction();                // 0x7C8
            virtual void DtorStateKeeperCatchAction();                // 0x7CC
            virtual void CtorStateFall();                             // 0x7D0
            virtual void LoopStateFall();                             // 0x7D4
            virtual void DtorStateFall();                             // 0x7D8
            virtual void CtorStateRebirth();                          // 0x7DC
            virtual void LoopStateRebirth();                          // 0x7E0
            virtual void DtorStateRebirth();                          // 0x7E4
            virtual int CanbeRun();                                   // 0x7E8
            virtual int MethdRun(float);                              // 0x7EC
            virtual int MethdRun(float, float);                       // 0x7F0
            virtual int CanbeBlockWork();                             // 0x7F4
            virtual int MethdBlockWork(float);                        // 0x7F8
            virtual int MethdBlockWork(float, float);                 // 0x7FC
            virtual int CanbeJump();                                  // 0x800
            virtual int MethdJump();                                  // 0x804
            virtual int CanbeSliding();                               // 0x808
            virtual int MethdSliding(float);                          // 0x80C
            virtual int CanbeSizeChange(long);                        // 0x810
            virtual int MethdSizeChange(long, float);                 // 0x814
            virtual int MethdChgStarState();                          // 0x818
            virtual int CanbeChgStarState();                          // 0x81C
            virtual int MethdChgStarState(float);                     // 0x820
            virtual int CanbeChgSpeedUpState();                       // 0x824
            virtual int MethdChgSpeedUpState(float, float);           // 0x828
            virtual int CanbeChgSpeedDownState();                     // 0x82C
            virtual int MethdChgSpeedDownState(float, float);         // 0x830
            virtual int CanbeJumpUpper() const;                       // 0x834
            virtual int CanbeNowJumpUpper() const;                    // 0x838
            virtual int MethdJumpUpper();                             // 0x83C
            virtual int CanbeSideStep(float, float);                  // 0x840
            virtual int MethdSideStep(float, float);                  // 0x844
            // 0x848-0x974 added for HockeyF game/hockey/CChrManHockey.cpp, named
            // from __vt__Q23msw14CChrCoreHockey (return types guessed as above;
            // CChrManHockeyCS::AllSetState_ calls MethdTimeOver).
            virtual int CanbeCatch();                                 // 0x848
            virtual int MethdCatch();                                 // 0x84C
            virtual int CanbeCatchToSDB();                            // 0x850
            virtual int MethdCatchToSDB();                            // 0x854
            virtual int CanbeSDBCatch();                              // 0x858
            virtual int MethdSDBCatch();                              // 0x85C
            virtual int CanbeDribble();                               // 0x860
            virtual int CanbeSDB();                                   // 0x864
            virtual int MethdSDB(float);                              // 0x868
            virtual int CanbeRDB();                                   // 0x86C
            virtual int MethdRDB(INPUT_WORK *, float);                // 0x870
            virtual int MethdRDB(INPUT_WORK *, float, float);         // 0x874
            virtual int CanbeQuickTurnState();                        // 0x878
            virtual int CanbeQuickTurn(float);                        // 0x87C
            virtual int MethdQuickTurn(float);                        // 0x880
            virtual int CanbeTrickMove();                             // 0x884
            virtual int MethdTrickMove(float);                        // 0x888
            virtual int CanbeTrickDance();                            // 0x88C
            virtual int MethdTrickDance();                            // 0x890
            virtual int CanbeBlockCut(int);                           // 0x894
            virtual int CanbeNowBlockCut();                           // 0x898
            virtual int MethdBlockCut(float);                         // 0x89C
            virtual int CanbeChargeCut();                             // 0x8A0
            virtual int MethdChargeCut(float);                        // 0x8A4
            virtual int CanbeDropCut();                               // 0x8A8
            virtual int CanbeNowDropCut();                            // 0x8AC
            virtual int MethdDropCut();                               // 0x8B0
            virtual int CanbeShoot();                                 // 0x8B4
            virtual int MethdShoot(float);                            // 0x8B8
            virtual int CanbeJumpShoot();                             // 0x8BC
            virtual int MethdJumpShoot();                             // 0x8C0
            virtual int MethdJumpShoot(float, int);                   // 0x8C4
            virtual int CanbeAerialShoot();                           // 0x8C8
            virtual int MethdAerialShoot();                           // 0x8CC
            virtual int CanbeRebound();                               // 0x8D0
            virtual int CanbeReboundDunk();                           // 0x8D4
            virtual int MethdReboundDunk();                           // 0x8D8
            virtual int CanbeLayUp();                                 // 0x8DC
            virtual int MethdLayUp();                                 // 0x8E0
            virtual int MethdLayUp(int);                              // 0x8E4
            virtual int CanbeAirTrick();                              // 0x8E8
            virtual int MethdAirTrick();                              // 0x8EC
            virtual int CanbeAirTrickSub();                           // 0x8F0
            virtual int MethdAirTrickSub(long, long);                 // 0x8F4
            virtual int CanbeDunk();                                  // 0x8F8
            virtual int MethdDunk();                                  // 0x8FC
            virtual int CanbeItemBallAttack();                        // 0x900
            virtual int MethdItemBallAttack();                        // 0x904
            virtual int CanbePass(CChrCore *);                        // 0x908
            virtual int MethdPass(float, int);                        // 0x90C
            virtual int MethdPass(CChrCore *, int);                   // 0x910
            virtual int CanbePassIdx(long);                           // 0x914
            virtual int MethdPassIdx(long);                           // 0x918
            virtual int CanbeDash() const;                            // 0x91C
            virtual int MethdDash();                                  // 0x920
            virtual int CanbeJumpPass();                              // 0x924
            virtual int MethdJumpPass(long);                          // 0x928
            virtual int CanbeJumpCatch();                             // 0x92C
            virtual int MethdJumpCatch(float);                        // 0x930
            virtual int CanbeDribbleMiss();                           // 0x934
            virtual int MethdDribbleMiss(float);                      // 0x938
            virtual int CanbeAerialPass(CChrCore *);                  // 0x93C
            virtual int MethdAerialPass(float, int);                  // 0x940
            virtual int CanbeAerialPassIdx(long);                     // 0x944
            virtual int MethdAerialPassIdx(long);                     // 0x948
            virtual int CanbeItemShoot();                             // 0x94C
            virtual int MethdItemShoot(float);                        // 0x950
            virtual int CanbeSetMoveRevise();                         // 0x954
            virtual int MethdSetMoveRevise(float);                    // 0x958
            virtual int MethdDamage(unsigned long, float);            // 0x95C
            virtual int MethdDamage(unsigned long, float, float, float, long, float, long, long);// 0x960
            virtual int MethdDamage(unsigned long, float, float, float, long, float, long, long, long, long);// 0x964
            virtual int CanbeUseItem();                               // 0x968
            virtual int MethdUseItem();                               // 0x96C
            virtual int MethdTurnOver(long);                          // 0x970
            virtual int MethdTimeOver(long);                          // 0x974
            // 0x978-0xA2C added for BossF game/boss/CChrDrvPadBoss.cpp, named
            // from __vt__Q23msw12CChrCoreBoss (return types guessed as above).
            virtual int MethdGameSet();                               // 0x978
            virtual int MethdGameResult(int);                         // 0x97C
            virtual int CanbeChgLeaderState();                        // 0x980
            virtual int CanbeChgNewLeaderState();                     // 0x984
            virtual int CanbeChgLeader(CChrCore *);                   // 0x988
            virtual int MethdChgLeader(float, long, long, int);       // 0x98C
            virtual int MethdChgLeader(long, long, long);             // 0x990
            virtual int MethdChgLeader();                             // 0x994
            virtual int CanbeServe(long);                             // 0x998
            virtual int MethdServe(long);                             // 0x99C
            virtual int CanbeReceive(int);                            // 0x9A0
            virtual int CanbeDiveReceive();                           // 0x9A4
            virtual int MethdReceive(int, int);                       // 0x9A8
            virtual int MethdReceive(int, int, float);                // 0x9AC
            virtual int CanbeSpike(int) const;                        // 0x9B0
            virtual int MethdSpike(int);                              // 0x9B4
            virtual int CanbeAutoSpikeJump() const;                   // 0x9B8
            virtual int Canbe1on1();                                  // 0x9BC
            virtual int Methd1on1(int);                               // 0x9C0
            virtual int CanbeAutoDPM();                               // 0x9C4
            virtual int MethdAutoDPM(float, float);                   // 0x9C8
            virtual int CanbeBlockWorkLose();                         // 0x9CC
            virtual int MethdBlockWorkLose();                         // 0x9D0
            virtual int CanbeJumpBlock();                             // 0x9D4
            virtual int MethdJumpBlock(float);                        // 0x9D8
            virtual int CanbeSwatIt() const;                          // 0x9DC
            virtual int MethdSwatIt();                                // 0x9E0
            virtual int CanbeSmashCut();                              // 0x9E4
            virtual int MethdSmashCut(float);                         // 0x9E8
            virtual int MethdAirTrickDownSpeed(float);                // 0x9EC
            virtual int CanbeSpecialBattle();                         // 0x9F0
            virtual int MethdSpecialBattle();                         // 0x9F4
            virtual int CanbeArtsShotCatch();                         // 0x9F8
            virtual int MethdArtsShotCatch();                         // 0x9FC
            virtual int CanbeBallCatch();                             // 0xA00
            virtual int MethdBallCatch(float);                        // 0xA04
            virtual int CanbeLineOver();                              // 0xA08
            virtual int MethdLineOver(float);                         // 0xA0C
            virtual int CanbeCrouch();                                // 0xA10
            virtual int MethdCrouch();                                // 0xA14
            virtual int CanbeSkate(int);                              // 0xA18
            virtual int MethdSkate(float, int, float, int);           // 0xA1C
            virtual int CanbeStealStance();                           // 0xA20
            virtual int MethdStealStance(float);                      // 0xA24
            virtual int CanbeShootStance();                           // 0xA28
            virtual int MethdShootStance(float, int);                 // 0xA2C
            // 0xA30-0xC98, the rest of CChrCore's vtable, added for BossF
            // game/boss/CChrCoreBossLuigi.cpp and its siblings, named from
            // __vt__Q23msw12CChrCoreBoss (return types guessed as above;
            // CChrCoreBoss's own virtuals start at 0xC9C).
            virtual int CanbeBrake();                                  // 0xA30
            virtual int MethdBrake(float, long);                       // 0xA34
            virtual int CanbeSwitchBrake(float, int);                  // 0xA38
            virtual int MethdSwitchBrake(float, float);                // 0xA3C
            virtual int CanbeSwitch();                                 // 0xA40
            virtual int MethdSwitch();                                 // 0xA44
            virtual int CanbeBlockStance();                            // 0xA48
            virtual int MethdBlockStance(float, float, long);          // 0xA4C
            virtual int CanbeAutoBlockStance();                        // 0xA50
            virtual int MethdAutoBlockStance(float, float, long);      // 0xA54
            virtual int CanbeAutoChangeStance();                       // 0xA58
            virtual int MethdAutoChangeStance(float, float, long, float); // 0xA5C
            virtual int MethdArtsDamage(unsigned long, bcs::FVEC3);    // 0xA60
            virtual int CanbeChangeStance();                           // 0xA64
            virtual int MethdChangeStance(float, float, long, float);  // 0xA68
            virtual int CanbePassCatchState();                         // 0xA6C
            virtual int CanbeAlleyoop();                               // 0xA70
            virtual int MethdSetPassCatchState(CBall *, int, const bcs::FVEC3 &, long); // 0xA74
            virtual int MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &, float); // 0xA78
            virtual int MethdAlleyoop();                               // 0xA7C
            virtual int MethdSetAlleyoopCatchState(float, float, const bcs::FVEC3 &, const bcs::FVEC3 &); // 0xA80
            virtual void GetTpInputDribble(INPUT_WORK &, float, float); // 0xA84
            virtual float GetArtsEventRot();                           // 0xA88
            virtual void DoPlayArtsEvent();                            // 0xA8C
            virtual int CanbeSubShoot();                               // 0xA90
            virtual int MethdSubShoot(float);                          // 0xA94
            virtual int CanbeNormalShoot();                            // 0xA98
            virtual int MethdNormalShoot(float);                       // 0xA9C
            virtual int CanbePowerShot();                              // 0xAA0
            virtual int MethdPowerShot(float);                         // 0xAA4
            virtual int CanbeSideShot();                               // 0xAA8
            virtual int MethdSideShot(float);                          // 0xAAC
            virtual int CanbeDivingCatch();                            // 0xAB0
            virtual int MethdDivingCatch(float);                       // 0xAB4
            virtual int CanbeSpinAttack();                             // 0xAB8
            virtual int MethdSpinAttack();                             // 0xABC
            virtual int CanbeEventDoll();                              // 0xAC0
            virtual int MethdEventDoll();                              // 0xAC4
            virtual int CanbePassStance();                             // 0xAC8
            virtual int MethdPassStance(float, long);                  // 0xACC
            virtual int CanbeDirectShootStance();                      // 0xAD0
            virtual int MethdDirectShootStance(float, long);           // 0xAD4
            virtual int CanbeDirectPassStance();                       // 0xAD8
            virtual int MethdDirectPassStance(float, long);            // 0xADC
            virtual int CanbePassChgLeader();                          // 0xAE0
            virtual int MethdPassChgLeader();                          // 0xAE4
            virtual int CanbeHockeyTrickMove();                        // 0xAE8
            virtual int MethdHockeyTrickMove(float, long);             // 0xAEC
            virtual int CanbeTackleGuard();                            // 0xAF0
            virtual int MethdTackleGuard(float, int, int);             // 0xAF4
            virtual int CanbeSmashAttack();                            // 0xAF8
            virtual int MethdSmashAttack(float);                       // 0xAFC
            virtual int CanbeBallScramble(CBall *);                    // 0xB00
            virtual int MethdBallScramble(CBall *, long, long);        // 0xB04
            virtual int CanbeDivingSteal(int);                         // 0xB08
            virtual int MethdDivingSteal(float, int, int);             // 0xB0C
            virtual int CanbeBodyContact();                            // 0xB10
            virtual int MethdBodyContact(float, long, int);            // 0xB14
            virtual int CanbeOffBallStance();                          // 0xB18
            virtual int MethdOffBallStance(float, int);                // 0xB1C
            virtual int CanbeBlockAttack();                            // 0xB20
            virtual int MethdBlockAttack(int);                         // 0xB24
            virtual int CanbeTackleAttack();                           // 0xB28
            virtual int MethdTackleAttack(float);                      // 0xB2C
            virtual int CanbeAttackFight();                            // 0xB30
            virtual int MethdAttackFight(long, int);                   // 0xB34
            virtual int CanbePassCatch();                              // 0xB38
            virtual int MethdPassCatch(long, const bcs::FVEC3 &, float); // 0xB3C
            virtual int CanbeFreeShoot();                              // 0xB40
            virtual int MethdbFreeShoot(float);                        // 0xB44
            virtual int CanbeFeintShoot();                             // 0xB48
            virtual int MethdFeintShoot(float);                        // 0xB4C
            virtual int CanbeTreadOnQPanel(const CQPanel *);           // 0xB50
            virtual int CanbeItemBallShoot();                          // 0xB54
            virtual int MethdItemBallShoot(float);                     // 0xB58
            virtual int CanbeCombination() const;                      // 0xB5C
            virtual int MethdCombination();                            // 0xB60
            virtual int CanbeLanding() const;                          // 0xB64
            virtual int CanbeFall() const;                             // 0xB68
            virtual int MethdFall();                                   // 0xB6C
            virtual int CanbeRebirth() const;                          // 0xB70
            virtual int MethdRebirth();                                // 0xB74
            virtual void ArtsOptAliveCheck_();                         // 0xB78
            virtual void *GetArtsOpt() const;                         // 0xB7C
            virtual void *GetArtsOptMdl() const;                      // 0xB80
            virtual void ArtsOptCreate(int, int, int);                 // 0xB84
            virtual void ArtsOptDestroy();                             // 0xB88
            virtual void ArtsOptSetDisp(int);                          // 0xB8C
            virtual void ArtsOptFadeIn(float, float, int);             // 0xB90
            virtual void ArtsOptFadeOut(float, float, int);            // 0xB94
            virtual void ArtsOptSetFadeInSE(long);                     // 0xB98
            virtual void ArtsOptSetFadeOutSE(long);                    // 0xB9C
            virtual void ArtsOptSetAliveSE(long);                      // 0xBA0
            virtual long ArtsOptGetNodeIdx(const char *) const;        // 0xBA4
            virtual void ArtsOptAnimPlay(long, int, float, float);     // 0xBA8
            virtual void ArtsOptAnimStop();                            // 0xBAC
            virtual void ArtsOptAnimSetSpeed(float);                   // 0xBB0
            virtual float ArtsOptAnimGetFrame() const;                 // 0xBB4
            virtual float ArtsOptAnimGetFrameNum() const;              // 0xBB8
            virtual float ArtsOptAnimGetResFrameNum(long) const;       // 0xBBC
            virtual int ArtsOptAnimGetMotID() const;                   // 0xBC0
            virtual void ArtsOptAnimSetFrame(float);                   // 0xBC4
            virtual void ArtsOptAnimResetLoopedFlag();                 // 0xBC8
            virtual int ArtsOptIsAnimLoop() const;                     // 0xBCC
            virtual int ArtsOptIsAnimLooped() const;                   // 0xBD0
            virtual int ArtsOptIsAnimEnd() const;                      // 0xBD4
            virtual int ArtsOptIsAnimInter() const;                    // 0xBD8
            virtual int ArtsOptIsAnimResLoopType(long) const;          // 0xBDC
            virtual int ArtsOptCreateAttachEffect(long, long, unsigned long, const bcs::FVEC3 *, unsigned long, unsigned long); // 0xBE0
            virtual int ArtsOptCreateAttachEffect(long, long, const char *, const bcs::FVEC3 *, unsigned long, unsigned long); // 0xBE4
            virtual int ArtsOptCreateAttachEffectAuto(long, long, unsigned long); // 0xBE8
            virtual long ArtsOptGetEffectHandle(long) const;           // 0xBEC
            virtual void ArtsOptSetEffectSpeed(long, float);           // 0xBF0
            virtual int ArtsOptIsEffectAlive(long) const;              // 0xBF4
            virtual void ArtsOptStopEffect(long);                      // 0xBF8
            virtual void ArtsOptEraseEffect(long);                     // 0xBFC
            virtual void ArtsOptStopEffectAll();                       // 0xC00
            virtual void ArtsOptEraseEffectAll();                      // 0xC04
            virtual void ArtsOptCreate(long, long, long, long);        // 0xC08
            virtual void Util_DropCoin(float, long, long);             // 0xC0C
            virtual void CheckCountWork__();                           // 0xC10
            virtual int CheckWater_(int);                              // 0xC14
            virtual void CalcMoveSpeedRate_(float, long);              // 0xC18
            virtual void Util_SetEffect(SET_EFFECT_TYPE);              // 0xC1C
            virtual void Util_AttachEffect(ATTACH_EFFECT_TYPE, long);  // 0xC20
            virtual void Util_DetachEffectAll();                       // 0xC24
            virtual void Util_DetachEffect(ATTACH_EFFECT_TYPE);        // 0xC28
            virtual void Util_DetachEffect_(unsigned long &);          // 0xC2C
            virtual void Util_EraseEffectAll();                        // 0xC30
            virtual void Util_EraseEffect(ATTACH_EFFECT_TYPE);         // 0xC34
            virtual void Util_EraseEffect_(unsigned long &);           // 0xC38
            virtual void CallbackDropCoinPickup_(const CDropCoin *);   // 0xC3C
            virtual void CallbackItemPickup_(const CItem *);           // 0xC40
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *); // 0xC44
            virtual int CallbackDribbleTap_();                         // 0xC48
            virtual void CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC4C
            virtual int CallbackBallDamage_(CBall *, const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC50
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *); // 0xC54
            virtual void CallbackBallCatch_(CBall *);                  // 0xC58
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long); // 0xC5C
            virtual void CallbackBallSnatched_(CBall *);               // 0xC60
            virtual int CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, bcs::FVEC3 &, bcs::FVEC3 &); // 0xC64
            virtual int CallbackDefObstructHit_(const CCollDefUnit *, const CBall *, bcs::FVEC3 &, bcs::FVEC3 &); // 0xC68
            virtual int CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, const bcs::FVEC3 &); // 0xC6C
            virtual void CallbackMotTrg_SeFoot_(long, long);           // 0xC70
            virtual void CallbackMotTrg_SeLand_(long, long);           // 0xC74
            virtual void CallbackMotTrg_SeJump_(long, long);           // 0xC78
            virtual void CallbackMotTrg_BodyVisible_(long, long);      // 0xC7C
            virtual void CallbackMotTrg_AtkOn_(long, long);            // 0xC80
            virtual void CallbackMotTrg_AtkOff_(long, long);           // 0xC84
            virtual void SubStateDamageCtorMame_(STATE_WORK_DAMAGE *); // 0xC88
            virtual void SubStateDamageLoopMame_(STATE_WORK_DAMAGE *); // 0xC8C
            virtual int SubStateParalyzeStateSet_(unsigned long, float); // 0xC90
            virtual void SubStateParalyzeSetEffect_();                // 0xC94
            virtual void SubStateParalyzeEndEffect_();                 // 0xC98

            // Not zero when the character takes no part in the game: it has
            // no model yet (the word at 0x90 is zero) or bit 30 of the word
            // at 0xEC is set.
            int IsDisable() const;
            // (name guessed) Inline in the modules: BasketF's
            // CStageManBasket::GetBallTeam tests the negation as a value
            // (cntlzw, then srwi.), not with a compare of IsDisable().
            int IsEnable() const { return !IsDisable(); }
            // The type the current driver reports; zero without a driver.
            // CChrMarkerBoss only shows markers for type 1 (most likely the
            // human players; not checked).
            long DriverGetType() const;
            // DOL. Driver nDrv of the character (CChrCore keeps a small
            // table of them, m_pDriver).
            CChrDrv *DriverGetEntry(long nDrv) const;
            // (name guessed) The driver in use. Inline in the modules: each
            // CComDrv<Sport>::GetComDrv reads m_nDriver itself and passes it
            // to DriverGetEntry.
            CChrDrv *DriverGetCurrent() const { return DriverGetEntry(m_nDriver); }
            // (name guessed) m_nChrId as stored, signed; the modules'
            // CComDrv<Sport>::GetChrTypeID return it (lbz, extsb on the same
            // register: a signed field, not a cast of an unsigned one).
            // GetChrId reads the same byte unsigned.
            signed char GetChrTypeID() const { return m_nChrId; }
            // Not zero when a pad drives the character. (meaning guessed from
            // the name; CChrMarkerVolley::AppearCheck asks it outside the
            // party game) In the DOL.
            int IsPadCtrl() const;
            // DOL. The goal the character's team attacks (BasketF
            // CStageBasket06::SendMessage lights that goal's side).
            long GetTargetGoalIdx() const;
            // DOL. The yaw (radians) from the character towards a point,
            // in the XZ plane.
            float GetRotY_ToPosXZ(const bcs::FVEC3 &pos) const;

            // (names guessed) Inline in the original; the module reads the
            // members directly (CChrMarkerBoss::AppearCheck loads the byte
            // unsigned).
            long GetChrId() const { return (unsigned char)m_nChrId; }
            unsigned long GetStatusFlag() const { return m_nStatusFlag; }
            unsigned long GetDispFlag() const { return m_nDispFlag; }
            // (name guessed) BossF's CEventDemoBoss::CreateDemo sets bit 2
            // (no drawing) on each character the demo replaces by a model.
            void OnDispFlag(unsigned long nBit) { m_nDispFlag |= nBit; }

            // The texture of the character's radar icon; null when
            // IsDisable() is true. In the DOL. (that the pointer is to a
            // GXTexObj is read from the caller, which makes a
            // bcs::G2D::CTexMap from it; const guessed)
            const _GXTexObj *GetRadarIconTexObj() const;
            // In the DOL (return type guessed). Writes the character's
            // position to rPos (DodgeF's damage demo aims the camera there).
            void GetCompPos(bcs::FVEC3 &rPos) const;
            // (names guessed) Inline in the original; the modules read the
            // bytes directly, as signed values.
            long GetPlayerNo() const { return m_nPlayerNo; }
            // DOL. (name from the DOL; what height it gives is not checked)
            float GetPosY() const;
            long GetTeamNo() const { return m_nTeamNo; }
            // (names guessed) Added for BasketF
            // game/basket/CCockpitBasketHarapeko.cpp, which reads both bytes
            // signed: the character number (as GetChrId, which other modules
            // read unsigned) passed to menu::GetCockpitFaceIconScale, and the
            // pad of a human player (0 to 3; the cockpit labels "1P" to "4P").
            long GetChrIdS() const { return (signed char)m_nChrId; }
            // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp (name
            // guessed): the character's face icon texture for the cockpit;
            // null when there is none. In the DOL.
            const _GXTexObj *GetCockpitIconTexObj() const;
            // (name guessed) The pad driving the character, read signed
            // (msw::cockpit::CSPGaugeInputDisp::SetDispAnim in the DOL passes
            // it to CGame::GetPadDefaultType and CMenuPad::IsNunchakuStyle).
            long GetPadNo() const { return m_nPadNo; }
            // Not zero for the star player of a team (names guessed). The
            // parameter files of the sports modules call it, so it is inline
            // and emitted there.
            int IsStarPlayer() const { return m_bStarPlayer; }

            // How a character is stopped (CChrMan::AllStop_). Values not known.
            enum STOP_MODE {};

            // Added for BossF game/boss/CChrManBoss.cpp (all in the DOL;
            // return types and parameter names guessed). Create sets the
            // character up under task pParent as player nPlayer, character
            // nChrType (CGame::GetCharType) in colour nColor.
            void Create(bcs::CTask *pParent, long nPlayer, long nChrType, long nColor);
            // Puts pDrv in driver slot nDrv (0: the pad, 1: the computer).
            void DriverSetEntry(long nDrv, CChrDrv *pDrv);
            // Makes driver slot nDrv the one in use.
            void DriverConnect(long nDrv, long, long, unsigned long, unsigned long);
            void SetTeamNo(long nTeam);
            void SetPadID(long nPad);
            void GetPos(bcs::FVEC3 &rPos);
            // (names guessed) Inline in the modules: the character managers
            // write the members directly.
            void SetCamNo(long nCam) { m_nCamNo = nCam; }
            // Sets the bit IsDisable tests: the character takes no part.
            void SetDisable() { m_UnkEC |= 0x40000000; }
            void OnStatusFlag(unsigned long nBit) { m_nStatusFlag |= nBit; }

            // Added for HockeyF game/hockey/CChrManHockey.cpp (all in the
            // DOL; return types and parameter meanings guessed).
            // The position, as a reference to the member (GetPos(FVEC3 &)
            // copies it).
            const bcs::FVEC3 &GetPos();
            float GetRotY() const;
            // The goal the character's team attacks, as a point.
            const bcs::FVEC3 &GetTargetGoalPos() const;
            // The distance to rPos in the XZ plane.
            float GetDist_ToPosXZ(const bcs::FVEC3 &rPos) const;
            // The yaw (radians) from the character towards pChr.
            float GetRotY_ToChr(const CChrCore *pChr) const;
            // Frame, motion id and play of animation slot nSlot (the
            // fighting code uses slot 3).
            float AnimGetFrame(unsigned long nSlot) const;
            long AnimGetMotID(unsigned long nSlot, long) const;
            void AnimPlay(unsigned long nSlot, long nMotId, int, float, float);
            // (names guessed) Inline in the modules.
            long DriverGetNo() const { return m_nDriver; }
            bcs::CMdlObj *GetMdl() const { return m_pMdl; }
            long GetComLevel() const { return m_nComLevel; }

            // Added for BossF game/boss/CChrDrvPadBoss.cpp (names guessed).
            // Inline in the module: the pad driver reads both words directly.
            float GetCamRotY() const { return m_fCamRotY; }
            // The word at 0xEC; the pad driver ignores input while bit 25 or
            // bit 27 is set (IsDisable tests bit 30).
            unsigned long GetStatusFlag2() const { return m_UnkEC; }
            // DOL.
            int IsStepAirTrick() const;

            // Added for HockeyF game/hockey/CChrDrvPadHockey.cpp (names
            // guessed). Inline: the pad driver reads the word directly.
            float GetFrameStep() const { return m_fFrameStep; }
            // DOL. The yaw towards the goal the character's team defends.
            float GetGuardGoalYRot() const;
            // DOL. Sends message id to the character's driver.
            void DriverSendMessage(CChrDrv::MSG_ID id, long, long, unsigned long, unsigned long);

            // Added for BossF game/boss/CChrCoreBoss<Name>.cpp (all in the
            // DOL; return types and parameter meanings guessed).
            // Effects at a node of the character (nNode) or at a point.
            unsigned long Eff_CreateAttach(long nSlot, long nEff, unsigned long nNode, const bcs::FVEC3 *pOfs,
                                           unsigned long, unsigned long, unsigned long);
            unsigned long Eff_CreateSimple(long nEff, const bcs::FVEC3 &pos, float fScale, unsigned long,
                                           unsigned long, unsigned long);
            unsigned long Eff_CreateSimple(long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, unsigned long,
                                           unsigned long, unsigned long);
            // Positions of the model's trans node, of a named node and of a
            // virtual node.
            void GetTransNodePos(bcs::FVEC3 &rPos) const;
            void GetNodePos(const char *pName, bcs::FVEC3 &rPos) const;
            void GetVNodePos(long nNode, bcs::FVEC3 &rPos) const;
            const bcs::FVEC3 &GetRot();
            // The yaw from the character towards a point, in the XZ plane.
            float GetRotY_FromPosXZ(const bcs::FVEC3 &pos) const;
            // Turns towards fRotY by fRate of the difference.
            void RotateYRate_(float fRotY, float fRate);
            int IsAnimEnd(unsigned long nSlot) const;
            void SetMoveMotType(long nType);
            void PlayVoice(long nVoice, unsigned long);
            int IsInvincible() const;
            // The ball the character holds.
            CBall *GetAttachBall() const;
            long GetGuardCourtIdx() const;

            // Added for VolleyF game/volley/CChrCoreVolley<Name>.cpp (all in
            // the DOL; return types and parameter meanings guessed).
            unsigned long Eff_CreateSimple(long nEff, const bcs::FVEC3 &pos, unsigned long, unsigned long,
                                           unsigned long);
            // An effect attached to the character's default node.
            unsigned long Eff_CreateAttachAuto(long nSlot, long nEff, unsigned long);
            // Stops the effect in slot nSlot.
            void Eff_Stop(long nSlot);
            void SetRotY(float fRotY);
            // The length in frames of motion nMotId.
            float AnimGetResFrameNum(long nMotId) const;
            // Takes ball hBall (its handle) into the character's hand.
            void AttachBall(long hBall, long, float, unsigned long);

        protected:
            unsigned long m_Unk60;
            CChrDrv *m_pDriver[2];       // 0x64 (count guessed from the index's place)
            long m_nDriver;              // 0x6C, index of the driver in use, negative for none
            // 0x70 (name guessed): the yaw the pad's stick directions are
            // relative to; CChrDrvPadBoss adds it to the stick angle.
            float m_fCamRotY;
            // 0x74 (name guessed): the frames one update advances;
            // CChrDrvPadHockey::CheckStickParam counts its timers with it.
            float m_fFrameStep;
            // 0x78 (name guessed): a speed factor for the character's
            // motions and turns (AnimPlay, RotateYRate_ read it);
            // CChrCoreVolleyLuigi turns its vacuum by it times m_fFrameStep.
            float m_fTimeScale;
            unsigned char m_Unk7C[0x90 - 0x7C];
            // 0x90 (name guessed): the character's model; IsDisable is true
            // while it is null. CChrManHockey::ArtsStart_ fades it out.
            bcs::CMdlObj *m_pMdl;
            unsigned char m_Unk94[0xE8 - 0x94];
            unsigned long m_nStatusFlag; // 0xE8, meaning of the bits not known
            unsigned long m_UnkEC;
            // 0xF0 (name guessed): CChrCore::UpdateDisp_ draws nothing while
            // bit 2 is set, and CChrMarkerDodge shows no marker then.
            unsigned long m_nDispFlag;
            unsigned char m_UnkF4[4];
            int m_bStarPlayer;           // 0xF8, see IsStarPlayer
            unsigned char m_UnkFC[0x78];
            signed char m_nChrId;        // 0x174, a character number (meaning guessed; signed, see GetChrTypeID)
            // 0x175 (name guessed): the character's colour variant, read
            // signed (CChrCoreBossSlime::Reset_ picks its colour from it).
            signed char m_nColor;
            // (names guessed) What CRadarIcon2D<Game>::CreateCharaIcon passes
            // to CGame::GetCharType and to CGame::GetTeamColorIdx.
            signed char m_nPlayerNo;     // 0x176
            signed char m_nPadNo;        // 0x177, see GetPadNo
            signed char m_nTeamNo;       // 0x178
            // 0x179 (name guessed): the CChrManBoss::CreateDriver_ stores the
            // entry of SetCamIdAssign_'s table here.
            signed char m_nCamNo;
            unsigned char m_Unk17A[0x186 - 0x17A];
            // 0x186 (name guessed): the model node effects attach to by
            // default (the volley characters pass it to Eff_CreateAttach).
            unsigned short m_nEffNode;
            unsigned char m_Unk188[0x230 - 0x188];
            // 0x230 (name guessed): the level CComMan::GetDatF reads the
            // computer's parameters for (CComManVolleyGame::GetConditionHandicapDis).
            long m_nComLevel;
            // The rest, up to the size of 0x574: the DOL constructor writes
            // up to 0x570 and CChrCoreBoss's own members start at 0x574.
            unsigned char m_Unk234[0x574 - 0x234];
    };
}

#endif /* !MSW_CCHRCORE_HPP_ */
