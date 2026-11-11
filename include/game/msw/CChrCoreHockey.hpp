#ifndef MSW_CCHRCOREHOCKEY_HPP_
#define MSW_CCHRCOREHOCKEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrCore.hpp"

namespace msw {

    namespace CHR_CORE_HOCKEY {
        class CC_PARAM_HOCKEY;
    }

    // The hockey-mode character core, and one class per character.
    //
    // INCOMPLETE: the virtual functions are all declared (the 852 slots of
    // __vt__Q23msw14CChrCoreHockey, read from HockeyF, as CChrCoreVolley.hpp
    // does for the volleyball game), but few members are mapped and
    // CChrCore is incomplete too (see there). The sizes are what
    // CChrManHockey::CreateObj_ allocates. Each destructor is declared
    // first and without a body, so that it decides where the vtable goes:
    // the character units (game/hockey/CChrCoreHockey<Name>.cpp) define it
    // and so emit the vtable.
    class CChrCoreHockey : public CChrCore {
        public:
            // The work of the air trick (here the "arts shot"), passed to
            // the SubStateArtsShot* virtuals; the work of the one-on-one and
            // ball scramble queries; the events of the special shot
            // (contents mapped only where used).
            struct STATE_WORK_AIR_TRICK;
            struct HO_1ON1_WORK;
            struct HO_BALLSCRAMBLE_WORK;
            enum ARTS_EVENT {};

            CChrCoreHockey();
            virtual ~CChrCoreHockey();

            // Overrides of CChrCore's virtuals, in vtable order (the slots of
            // __vt__Q23msw14CChrCoreHockey that name CChrCoreHockey; return
            // types as guessed in CChrCore).
            virtual long GetQueueWorkSize() const;                     // 0x10
            virtual void SetConditionEffect_(long);                    // 0x24
            virtual float Get1on1StartDist() const;                    // 0x48
            virtual int AutoMoveTargetMarkerPos();                     // 0x60
            virtual long CheckGuideTelopDisp();                        // 0x64
            virtual int IsDisp1On1Maker() const;                       // 0x6C
            virtual float GetBlockWorkDir() const;                     // 0x7C
            virtual int IsHockeyChgArrowMarker();                      // 0x144
            virtual float GetHockeyArrowMarkerRotY() const;            // 0x148
            virtual int IsHockeyTackleMarker() const;                  // 0x14C
            virtual int IsHockeyTgtArrowMarker() const;                // 0x150
            virtual void GetHockeyShootGoalTgtPos(bcs::FVEC3 &, float &); // 0x154
            virtual int IsHockeyDispShootMarker();                     // 0x158
            virtual void SetHockeyNoChargeCutFrame(float);             // 0x15C
            virtual float GetHockeyNoChargeCutFrame();                 // 0x160
            virtual float GetShootChargeFrameMax() const;              // 0x164
            virtual int GetHockeyKeeperTgtPos(bcs::FVEC3 &);           // 0x168
            virtual int IsKeeper() const;                              // 0x16C
            virtual void SetKeeper(int);                               // 0x170
            virtual void SetMaxSkateSpeed(float);                      // 0x174
            virtual float GetMaxSkateSpeed() const;                    // 0x178
            virtual float GetHockeyVelXZ() const;                      // 0x17C
            virtual float GetBallScramblePoint() const;                // 0x180
            virtual float GetBallScrambleNowPoint() const;             // 0x184
            virtual float GetBallScrambleWinPoint() const;             // 0x188
            virtual float GetBallScrambleMaxPoint() const;             // 0x18C
            virtual int GetBodyContactSkate() const;                   // 0x190
            virtual void SetBodyContactSkate(int);                     // 0x194
            virtual void SetPackNotCatchFrame(float);                  // 0x198
            virtual int IsHockeyBodyNoPush() const;                    // 0x19C
            virtual long GetHockeyMaxShootCourseDirPercent();          // 0x1A0
            virtual int GetHockeyShootGoalTgtPos(bcs::FVEC3 &);        // 0x1A4
            virtual int MethdHockeyKeeperQuickMove(const bcs::FVEC3 &); // 0x1A8
            virtual int IsHockeyDamage() const;                        // 0x1AC
            virtual int IsHockeyBallHit();                             // 0x1B0
            virtual int IsHockeyAttack() const;                        // 0x1B4
            virtual int IsHockeyFighting() const;                      // 0x1B8
            virtual void HockeyChrReflect(bcs::FVEC3 &, CChrCore *);   // 0x1BC
            virtual long Get1on1AttackHitType() const;                 // 0x1C0
            virtual long GetNow1on1AttackNum() const;                  // 0x1C4
            virtual float Get1on1AttackWaitTimeMax() const;            // 0x1C8
            virtual void SetUseShootMarker(int);                       // 0x1CC
            virtual int IsUseShootMarker() const;                      // 0x1D0
            virtual int MethdHockeySetMoveRevise(float, float);        // 0x1D4
            virtual void HockeyArtsEnd();                              // 0x1D8
            virtual int CanbeKeeperDivingCatch();                      // 0x1DC
            virtual int MethdKeeperDivingCatch();                      // 0x1E0
            virtual float GetAttackPower(int);                         // 0x1E4
            virtual long GetDamageCollKind() const;                    // 0x1E8
            virtual float GetHockeyBodyCollRadiusNow() const;          // 0x1EC
            virtual float GetHockeyMass() const;                       // 0x1F0
            virtual float GetFaceOffChargeMaxFrame() const;            // 0x1F4
            virtual long GetTackleCount(long) const;                   // 0x1F8
            virtual void SetAddTackleCount(long);                      // 0x1FC
            virtual void ReSetTackleCount(long);                       // 0x200
            virtual int CanbeHockeyComBlockAttack();                   // 0x204
            virtual int IsHockeyInvisible() const;                     // 0x208
            virtual void SetHockeyAttackObjNull(long);                 // 0x20C
            virtual int GetHockeyAttackObjInfo(long, bcs::FVEC3 &, float &, bcs::FVEC3 &); // 0x210
            virtual long GetHockeyAttackObjMax();                      // 0x214
            virtual void SetKeeperCatchMode();                         // 0x218
            virtual float GetHockeyPassSpeed();                        // 0x21C
            virtual float GetHockeyKeeperPassSpeed();                  // 0x220
            virtual int IsHockeyStatePaintTumble() const;              // 0x224
            virtual void GetVel(bcs::FVEC3 &) const;                   // 0x294
            virtual void SetVel(bcs::FVEC3 &, float);                  // 0x298
            virtual void SetSwitch(int);                               // 0x29C
            virtual int IsSwitch() const;                              // 0x2A0
            virtual void SetNoReflectFrame(float);                     // 0x2A4
            virtual float GetNoReflectFrame() const;                   // 0x2A8
            virtual void SetReflectNoMoveFrame(float);                 // 0x2AC
            virtual float GetReflectNoMoveFrame() const;               // 0x2B0
            virtual void SetAutoBrakeStartTime(float);                 // 0x2B4
            virtual float GetAutoBrakeStartTime() const;               // 0x2B8
            virtual float GetKeeperDivingCatchSpeed(int) const;        // 0x2BC
            virtual float GetHockeyKeeperDivingSteeleDist(int) const;  // 0x2C0
            virtual float GetHockeyKeeperDivingSteeleGoalFrame(int) const; // 0x2C4
            virtual void SetHockeySkateWork(float, float, float, float); // 0x2C8
            virtual int IsHockeyNowCombinationMode() const;            // 0x2CC
            virtual int IsCombinationShootStance() const;              // 0x2D0
            virtual void GetForceInfo(bcs::FVEC3 &, float &);          // 0x2D4
            virtual void CrashSkateMissionSetReady(int);               // 0x2D8
            virtual int IsHockeyNowKeeperMoveMode() const;             // 0x2DC
            virtual int IsHockeyTackleSteal() const;                   // 0x2E0
            virtual void PlayRunSe(unsigned long);                     // 0x2F4
            virtual void PlayWalkSe(unsigned long);                    // 0x2F8
            virtual void PlayTurnSe(unsigned long);                    // 0x300
            virtual void PlayLandSe(unsigned long);                    // 0x308
            virtual void SE_PlayArtsJumpSE(long);                      // 0x318
            virtual float GetTargetGoalXLength() const;                // 0x328
            virtual float GetTargetGoalZLength() const;                // 0x32C
            virtual float GetGuardGoalXLength() const;                 // 0x330
            virtual float GetGuardGoalZLength() const;                 // 0x334
            virtual int Util_GetIdleMotID();                           // 0x348
            virtual int Util_GetRunMotID();                            // 0x34C
            virtual int Util_GetLandMotID(long);                       // 0x354
            virtual int Util_GetJumpUpMotID(int);                      // 0x358
            virtual void Util_GetJumpLandMotID(int);                   // 0x35C
            virtual int Util_GetDamageMotID(long, long, long);         // 0x360
            virtual void Util_SetOffenseLeaderFirstState(long);        // 0x364
            virtual void Util_SetDefenseLeaderFirstState();            // 0x368
            virtual void CreateColl_();                                // 0x374
            virtual void Create_();                                    // 0x378
            virtual void MainFunc_();                                  // 0x37C
            virtual void UpdateFunc_();                                // 0x380
            virtual void Reset_();                                     // 0x384
            virtual long GetPrmBodySize() const;                       // 0x390
            virtual void CtorStateNeutral();                           // 0x3BC
            virtual void LoopStateNeutral();                           // 0x3C0
            virtual void DtorStateNeutral();                           // 0x3C4
            virtual void CtorStateRun();                               // 0x3E0
            virtual void LoopStateRun();                               // 0x3E4
            virtual void DtorStateRun();                               // 0x3E8
            virtual void CtorStateJump();                              // 0x3EC
            virtual void LoopStateJump();                              // 0x3F0
            virtual void DtorStateJump();                              // 0x3F4
            virtual void CtorStateDamage();                            // 0x3F8
            virtual void LoopStateDamage();                            // 0x3FC
            virtual void DtorStateDamage();                            // 0x400
            virtual void CtorStateItemShoot();                         // 0x410
            virtual void LoopStateItemShoot();                         // 0x414
            virtual void DtorStateItemShoot();                         // 0x418
            virtual void CtorStateBlockCut();                          // 0x41C
            virtual void LoopStateBlockCut();                          // 0x420
            virtual void DtorStateBlockCut();                          // 0x424
            virtual void CtorStateChargeCut();                         // 0x428
            virtual void LoopStateChargeCut();                         // 0x42C
            virtual void DtorStateChargeCut();                         // 0x430
            virtual void CtorStateMotPlay();                           // 0x44C
            virtual void LoopStateMotPlay();                           // 0x450
            virtual void DtorStateMotPlay();                           // 0x454
            virtual void CtorStateSizeChange();                        // 0x458
            virtual void LoopStateSizeChange();                        // 0x45C
            virtual void DtorStateSizeChange();                        // 0x460
            virtual void CtorStateDizzy();                             // 0x470
            virtual void LoopStateDizzy();                             // 0x474
            virtual void DtorStateDizzy();                             // 0x478
            virtual void CtorStatePass();                              // 0x4AC
            virtual void LoopStatePass();                              // 0x4B0
            virtual void DtorStatePass();                              // 0x4B4
            virtual void CtorStatePassCatch();                         // 0x4C4
            virtual void LoopStatePassCatch();                         // 0x4C8
            virtual void DtorStatePassCatch();                         // 0x4CC
            virtual void CtorStateQuickTurn();                         // 0x50C
            virtual void LoopStateQuickTurn();                         // 0x510
            virtual void DtorStateQuickTurn();                         // 0x514
            virtual void CtorStateTrickMove();                         // 0x518
            virtual void LoopStateTrickMove();                         // 0x51C
            virtual void DtorStateTrickMove();                         // 0x520
            virtual void CtorStateCatch();                             // 0x530
            virtual void LoopStateCatch();                             // 0x534
            virtual void DtorStateCatch();                             // 0x538
            virtual void CtorStateJumpShoot();                         // 0x548
            virtual void LoopStateJumpShoot();                         // 0x54C
            virtual void DtorStateJumpShoot();                         // 0x550
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateBallCatchBack();                     // 0x65C
            virtual void LoopStateBallCatchBack();                     // 0x660
            virtual void DtorStateBallCatchBack();                     // 0x664
            virtual void CtorStateSpinStagger();                       // 0x68C
            virtual void LoopStateSpinStagger();                       // 0x690
            virtual void DtorStateSpinStagger();                       // 0x694
            virtual void CtorStateArtsOut();                           // 0x698
            virtual void LoopStateArtsOut();                           // 0x69C
            virtual void DtorStateArtsOut();                           // 0x6A0
            virtual void CtorStateDivingCatch();                       // 0x6BC
            virtual void LoopStateDivingCatch();                       // 0x6C0
            virtual void DtorStateDivingCatch();                       // 0x6C4
            virtual void CtorStateSkate();                             // 0x6D4
            virtual void LoopStateSkate();                             // 0x6D8
            virtual void DtorStateSkate();                             // 0x6DC
            virtual void CtorStateStealStance();                       // 0x6E0
            virtual void LoopStateStealStance();                       // 0x6E4
            virtual void DtorStateStealStance();                       // 0x6E8
            virtual void CtorStateShootStance();                       // 0x6EC
            virtual void LoopStateShootStance();                       // 0x6F0
            virtual void DtorStateShootStance();                       // 0x6F4
            virtual void CtorStateBrake();                             // 0x6F8
            virtual void LoopStateBrake();                             // 0x6FC
            virtual void DtorStateBrake();                             // 0x700
            virtual void CtorStateSwitch();                            // 0x704
            virtual void LoopStateSwitch();                            // 0x708
            virtual void DtorStateSwitch();                            // 0x70C
            virtual void CtorStateDirectShootStance();                 // 0x734
            virtual void LoopStateDirectShootStance();                 // 0x738
            virtual void DtorStateDirectShootStance();                 // 0x73C
            virtual void CtorStateTackleGuard();                       // 0x74C
            virtual void LoopStateTackleGuard();                       // 0x750
            virtual void DtorStateTackleGuard();                       // 0x754
            virtual void CtorStateFallOut();                           // 0x758
            virtual void LoopStateFallOut();                           // 0x75C
            virtual void DtorStateFallOut();                           // 0x760
            virtual void CtorStateFallSafe();                          // 0x764
            virtual void LoopStateFallSafe();                          // 0x768
            virtual void DtorStateFallSafe();                          // 0x76C
            virtual void CtorStateSmashAttack();                       // 0x770
            virtual void LoopStateSmashAttack();                       // 0x774
            virtual void DtorStateSmashAttack();                       // 0x778
            virtual void CtorStateBallScramble();                      // 0x77C
            virtual void LoopStateBallScramble();                      // 0x780
            virtual void DtorStateBallScramble();                      // 0x784
            virtual void CtorStateFaceOff();                           // 0x788
            virtual void LoopStateFaceOff();                           // 0x78C
            virtual void DtorStateFaceOff();                           // 0x790
            virtual void CtorStateDivingSteal();                       // 0x794
            virtual void LoopStateDivingSteal();                       // 0x798
            virtual void DtorStateDivingSteal();                       // 0x79C
            virtual void CtorStateBodyContact();                       // 0x7A0
            virtual void LoopStateBodyContact();                       // 0x7A4
            virtual void DtorStateBodyContact();                       // 0x7A8
            virtual void CtorStateOffBallStance();                     // 0x7AC
            virtual void LoopStateOffBallStance();                     // 0x7B0
            virtual void DtorStateOffBallStance();                     // 0x7B4
            virtual void CtorStateAttackFight();                       // 0x7B8
            virtual void LoopStateAttackFight();                       // 0x7BC
            virtual void DtorStateAttackFight();                       // 0x7C0
            virtual void CtorStateKeeperCatchAction();                 // 0x7C4
            virtual void LoopStateKeeperCatchAction();                 // 0x7C8
            virtual void DtorStateKeeperCatchAction();                 // 0x7CC
            virtual int CanbeRun();                                    // 0x7E8
            virtual int MethdRun(float);                               // 0x7EC
            virtual int MethdRun(float, float);                        // 0x7F0
            virtual int CanbeJump();                                   // 0x800
            virtual int MethdJump();                                   // 0x804
            virtual int CanbeSizeChange(long);                         // 0x810
            virtual int MethdSizeChange(long, float);                  // 0x814
            virtual int MethdChgStarState();                           // 0x818
            virtual int CanbeChgStarState();                           // 0x81C
            virtual int CanbeChgSpeedUpState();                        // 0x824
            virtual int CanbeChgSpeedDownState();                      // 0x82C
            virtual int CanbeBlockCut(int);                            // 0x894
            virtual int CanbeNowBlockCut();                            // 0x898
            virtual int MethdBlockCut(float);                          // 0x89C
            virtual int CanbeChargeCut();                              // 0x8A0
            virtual int MethdChargeCut(float);                         // 0x8A4
            virtual int CanbeShoot();                                  // 0x8B4
            virtual int MethdShoot(float);                             // 0x8B8
            virtual int CanbeAirTrick();                               // 0x8E8
            virtual int MethdAirTrick();                               // 0x8EC
            virtual int CanbePass(CChrCore *);                         // 0x908
            virtual int MethdPass(CChrCore *, int);                    // 0x910
            virtual int CanbeItemShoot();                              // 0x94C
            virtual int MethdItemShoot(float);                         // 0x950
            virtual int CanbeSetMoveRevise();                          // 0x954
            virtual int MethdDamage(unsigned long, float);             // 0x95C
            virtual int MethdDamage(unsigned long, float, float, float, long, float, long, long); // 0x960
            virtual int MethdTurnOver(long);                           // 0x970
            virtual int MethdTimeOver(long);                           // 0x974
            virtual int MethdGameSet();                                // 0x978
            virtual int MethdGameResult(int);                          // 0x97C
            virtual int CanbeChgLeaderState();                         // 0x980
            virtual int CanbeChgNewLeaderState();                      // 0x984
            virtual int CanbeChgLeader(CChrCore *);                    // 0x988
            virtual int MethdChgLeader(long, long, long);              // 0x990
            virtual int MethdChgLeader();                              // 0x994
            virtual int CanbeSkate(int);                               // 0xA18
            virtual int MethdSkate(float, int, float, int);            // 0xA1C
            virtual int CanbeStealStance();                            // 0xA20
            virtual int MethdStealStance(float);                       // 0xA24
            virtual int CanbeShootStance();                            // 0xA28
            virtual int MethdShootStance(float, int);                  // 0xA2C
            virtual int CanbeBrake();                                  // 0xA30
            virtual int MethdBrake(float, long);                       // 0xA34
            virtual int CanbeSwitchBrake(float, int);                  // 0xA38
            virtual int MethdSwitchBrake(float, float);                // 0xA3C
            virtual int CanbeSwitch();                                 // 0xA40
            virtual int MethdSwitch();                                 // 0xA44
            virtual int MethdArtsDamage(unsigned long, bcs::FVEC3);    // 0xA60
            virtual float GetArtsEventRot();                           // 0xA88
            virtual void DoPlayArtsEvent();                            // 0xA8C
            virtual int CanbePassStance();                             // 0xAC8
            virtual int MethdPassStance(float, long);                  // 0xACC
            virtual int CanbeDirectShootStance();                      // 0xAD0
            virtual int MethdDirectShootStance(float, long);           // 0xAD4
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
            virtual int CanbeItemBallShoot();                          // 0xB54
            virtual int MethdItemBallShoot(float);                     // 0xB58
            virtual void Util_SetEffect(SET_EFFECT_TYPE);              // 0xC1C
            virtual void Util_AttachEffect(ATTACH_EFFECT_TYPE, long);  // 0xC20
            virtual void CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC4C
            virtual int CallbackBallDamage_(CBall *, const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC50
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *); // 0xC54
            virtual void CallbackBallCatch_(CBall *);                  // 0xC58
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long); // 0xC5C
            virtual void CallbackBallSnatched_(CBall *);               // 0xC60
            virtual void SubStateParalyzeSetEffect_();                 // 0xC94
            virtual void SubStateParalyzeEndEffect_();                 // 0xC98

            // CChrCoreHockey's own virtuals, 0xC9C to the end of the vtable at
            // 0xD4C. Return types guessed, from the bodies where looked at
            // (GetShootGoalTgtPos copies a vector out) and otherwise from the
            // volleyball and boss slots of the same kind.
            virtual bcs::FVEC3 GetShootGoalTgtPos() const;             // 0xC9C
            virtual long GetBallScrambleCutType() const;               // 0xCA0
            virtual long GetCombinationTropIdx_() const;               // 0xCA4
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual const CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *GetCcParam_() const; // 0xCAC
            virtual void CtorStateAirTrick_();                         // 0xCB0
            virtual void LoopStateAirTrick_();                         // 0xCB4
            virtual void DtorStateAirTrick_();                         // 0xCB8
            virtual void SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK *); // 0xCBC
            virtual float SubStateArtsShotGetSetbackWaitFrame_(long);  // 0xCC0
            virtual void SubStateArtsShotSetSetbackFrame_(STATE_WORK_AIR_TRICK *, long); // 0xCC4
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual int SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCCC
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCD4
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCDC
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotAskTargetMarkerMove_(STATE_WORK_AIR_TRICK *); // 0xCE4
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCE8
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long); // 0xCEC
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long); // 0xCF0
            virtual float SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long); // 0xCF4
            virtual float SubStateArtsShotGetTrickRotFrame_(STATE_WORK_AIR_TRICK *); // 0xCF8
            virtual void SubStateArtsShotCalcSetbackPos_(bcs::FVEC3 &); // 0xCFC
            virtual void SubStateArtsShotCalcRotParam_(CChrCore::ROT_LERP1 &, float, float); // 0xD00
            virtual void SubStateArtsShotClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &); // 0xD04
            virtual void SubStateArtsShotDoJumpMove_(STATE_WORK_AIR_TRICK *); // 0xD08
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD0C
            virtual int SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *); // 0xD14
            virtual void SubStateArtsShotStartArtsHitback_(STATE_WORK_AIR_TRICK *); // 0xD18
            virtual int SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *); // 0xD1C
            virtual void SubStateArtsShotDoLandStart_(STATE_WORK_AIR_TRICK *); // 0xD20
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD24
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD28
            virtual void ChgLeader_(CChrCore *);                       // 0xD2C
            virtual int MethdPassCall_();                              // 0xD30
            virtual int GetHockey1on1Work(HO_1ON1_WORK &) const;       // 0xD34
            virtual int GetHockeyBallScrambleWork(HO_BALLSCRAMBLE_WORK &) const; // 0xD38
            virtual long SIDtoSeID_(long);                             // 0xD3C
            virtual void ArtsParamInit();                              // 0xD40
            virtual void ArtsProc(ARTS_EVENT);                         // 0xD44
            virtual void ArtsFadeOut() const;                          // 0xD48
            virtual void ArtsFadeIn() const;                           // 0xD4C

            // Added for the character units game/hockey/CChrCoreHockey<Name>.cpp
            // (in HockeyF; return types and parameter meanings guessed).
            // The number of pad-driven characters in the game.
            long GetExistPTPadDrvNum_();
            // Not zero when rPos2 is inside the oval in front of rPos1 that
            // faces fRotY (fLength long, fWidth wide).
            int HitCheckOval_(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2, float fRotY, float fLength,
                              float fWidth, float);
            // The signed difference between two yaws.
            float CalcRotDiff_(float fRot1, float fRot2) const;

            // (in HockeyF; return type and meaning guessed) Lets the
            // character join the fight over the puck; zero when it cannot.
            int MethdAddFighting_();

            // (in HockeyF; added for CChrCoreHockeyWaluigi, parameter
            // meanings guessed) Shoots pBall with shot kind nType towards
            // the yaw fRotY.
            void Util_ShootBall_(CBall *pBall, long nType, float, float fRotY);
            // (in HockeyF) The shot trigger of the special shot's next
            // motion, fFrame frames into it (meaning guessed).
            void SubStateArtsShotDoGetBallShotTrg_(STATE_WORK_AIR_TRICK *pWork, float fFrame);

        protected:
            // Not mapped yet. The size, 0x76C, is what CreateObj_ allocates
            // for most characters.
            unsigned char m_Unk574[0x634 - 0x574];
            // 0x634 (name guessed): the point the special shot's extra
            // pucks are aimed at (CChrCoreHockeyWaluigi reads x and z).
            bcs::FVEC3 m_ArtsTargetPos;
            unsigned char m_Unk640[0x76C - 0x640];
    };

    // (field names guessed; only what the character units read is mapped)
    struct CChrCoreHockey::STATE_WORK_AIR_TRICK {
        unsigned long m_nFlag;       // 0x00, bit 0 set while Luigi's vacuum pulls
        float m_fRotY;               // 0x04, the yaw the trick turns to
        unsigned char m_Unk08[0x4];
        float m_fUnk0C;              // 0x0C, Luigi clears it when his target is lost
        unsigned char m_Unk10[0x40 - 0x10];
        CChrCore::ROT_LERP1 m_RotLerp; // 0x40, the turn (SubStateArtsShotCalcRotParam_)
        // 0x50 (name guessed): the kind of jump; Donkey Kong sets 2, or 0
        // for none (SubStateArtsShotSetTrickJumpParam_).
        long m_nJumpType;
        long m_nTrickStep;           // 0x54
        unsigned char m_Unk58[0x68 - 0x58];
        float m_fShotFrame;          // 0x68, the frame of the motion the ball leaves at
        // 0x6C: the yaw the shot aims at; negative for straight at the goal
        // (Luigi's target marker sets it).
        float m_fTargetRotY;
        long m_nShotStep;            // 0x70, Waluigi's step through his throws
        float m_fShotTimer;          // 0x74, Waluigi's frames since the last throw
    };

    class CChrCoreHockeyMario : public CChrCoreHockey {
        public:
            CChrCoreHockeyMario();
            virtual ~CChrCoreHockeyMario();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
    };

    class CChrCoreHockeyLuigi : public CChrCoreHockey {
        public:
            CChrCoreHockeyLuigi();
            virtual ~CChrCoreHockeyLuigi();
            virtual int MoveTargetMarkerPos(float fRotY);              // 0x5C
            virtual int AutoMoveTargetMarkerPos();                     // 0x60
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *); // 0xD14
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD24
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD28

        protected:
            // (inline helpers, names guessed) Deletes the vacuum task, if
            // any; the computer's level for its parameters (CComMan::GetDatF).
            void DeleteVacuum_();
            float GetComDatLevel_();

            // (names guessed; allocated with 0x784 bytes)
            unsigned long m_hVacuum;     // 0x76C, task id of the vacuum, 0 for none
            int m_bVacuum;               // 0x770, the vacuum pulls
            // 0x774: frames the computer waits before it gives up on a target
            // in front; 0x778: frames it keeps aiming after that; 0x77C: set
            // once the first has run out; 0x780: frames before it starts to
            // aim at all.
            float m_fComAimWait;
            float m_fComAimKeep;
            int m_bComAimTimeUp;
            float m_fComAimDelay;
    };

    class CChrCoreHockeyPeach : public CChrCoreHockey {
        public:
            CChrCoreHockeyPeach();
            virtual ~CChrCoreHockeyPeach();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD0C
    };

    class CChrCoreHockeyDaisy : public CChrCoreHockey {
        public:
            CChrCoreHockeyDaisy();
            virtual ~CChrCoreHockeyDaisy();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
    };

    class CChrCoreHockeyYoshi : public CChrCoreHockey {
        public:
            CChrCoreHockeyYoshi();
            virtual ~CChrCoreHockeyYoshi();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *); // 0xD14
    };

    class CChrCoreHockeyWario : public CChrCoreHockey {
        public:
            CChrCoreHockeyWario();
            virtual ~CChrCoreHockeyWario();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
    };

    // Waluigi: his special shot splits the puck in four (game/hockey/
    // CChrCoreHockeyWaluigi.cpp).
    class CChrCoreHockeyWaluigi : public CChrCoreHockey {
        public:
            enum {
                SPLIT_BALL_NUM = 3 // the copies of the puck
            };

            CChrCoreHockeyWaluigi();
            virtual ~CChrCoreHockeyWaluigi();
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateAirTrick();                          // 0x584
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual int SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD24

            void SubStateArtsShotCreateSplitBall_();

        private:
            // Deletes the copies of the puck that are left.
            void DeleteSplitBall_();
            // Throws copy idx at m_ArtsTargetPos.
            void ShootSplitBall_(long idx);

            long m_anSplitBall[SPLIT_BALL_NUM]; // 0x76C, the copies' ball numbers, -1 for none
    };

    class CChrCoreHockeyDonkey : public CChrCoreHockey {
        public:
            CChrCoreHockeyDonkey();
            virtual ~CChrCoreHockeyDonkey();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCE8
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD0C
    };

    class CChrCoreHockeyDiddy : public CChrCoreHockey {
        public:
            CChrCoreHockeyDiddy();
            virtual ~CChrCoreHockeyDiddy();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
        protected:
            unsigned char m_Unk76C[0x770 - 0x76C];
    };

    class CChrCoreHockeyKinopio : public CChrCoreHockey {
        public:
            CChrCoreHockeyKinopio();
            virtual ~CChrCoreHockeyKinopio();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCE8
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long); // 0xCEC
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD0C
    };

    class CChrCoreHockeyKoopa : public CChrCoreHockey {
        public:
            CChrCoreHockeyKoopa();
            virtual ~CChrCoreHockeyKoopa();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD0C
    };

    class CChrCoreHockeyKoopaJr : public CChrCoreHockey {
        public:
            CChrCoreHockeyKoopaJr();
            virtual ~CChrCoreHockeyKoopaJr();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
    };

    class CChrCoreHockeyMoogle : public CChrCoreHockey {
        public:
            CChrCoreHockeyMoogle();
            virtual ~CChrCoreHockeyMoogle();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *); // 0xD14
    };

    class CChrCoreHockeySaboten : public CChrCoreHockey {
        public:
            CChrCoreHockeySaboten();
            virtual ~CChrCoreHockeySaboten();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD0C
    };

    class CChrCoreHockeyNinja : public CChrCoreHockey {
        public:
            CChrCoreHockeyNinja();
            virtual ~CChrCoreHockeyNinja();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
        protected:
            unsigned char m_Unk76C[0x788 - 0x76C];
    };

    // The Black Mage: his shot trick locks on to the other team and strikes
    // them (CC17_HOCKEY::CcKuromaMagicMgr).
    class CChrCoreHockeyKuroma : public CChrCoreHockey {
        public:
            CChrCoreHockeyKuroma();
            virtual ~CChrCoreHockeyKuroma();
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD28

        protected:
            // (inline helper, name guessed) Deletes the strike task, if any.
            void ReleaseMagic_();

            // 0x76C (name guessed; allocated with 0x770 bytes): the task id
            // of the CcKuromaMagicMgr, 0 for none.
            unsigned long m_hMagic;
    };

    class CChrCoreHockeySiroma : public CChrCoreHockey {
        public:
            CChrCoreHockeySiroma();
            virtual ~CChrCoreHockeySiroma();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
        protected:
            unsigned char m_Unk76C[0x780 - 0x76C];
    };

    class CChrCoreHockeySlime : public CChrCoreHockey {
        public:
            CChrCoreHockeySlime();
            virtual ~CChrCoreHockeySlime();
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
        protected:
            unsigned char m_Unk76C[0x770 - 0x76C];
    };

    class CChrCoreHockeyMii00 : public CChrCoreHockey {
        public:
            CChrCoreHockeyMii00();
            virtual ~CChrCoreHockeyMii00();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
    };

    class CChrCoreHockeyMii01 : public CChrCoreHockey {
        public:
            CChrCoreHockeyMii01();
            virtual ~CChrCoreHockeyMii01();
            virtual void Reset_();                                     // 0x384
            virtual CHR_CORE_HOCKEY::CC_PARAM_HOCKEY *NewCcParam_(long nHeap) const; // 0xCA8
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCC8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCD8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCE0
    };
}

#endif /* !MSW_CCHRCOREHOCKEY_HPP_ */
