#ifndef MSW_CCHRCOREDODGE_HPP_
#define MSW_CCHRCOREDODGE_HPP_

#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrCore.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    namespace CHR_CORE_DODGE {
        // (contents not known) A dodgeball character's parameters (NewCcParam_).
        struct CC_PARAM_DODGE;
    }

    class CBallDodge;

    // INCOMPLETE: the dodgeball character core (DodgeF, size 0x930: what
    // CChrManDodge::CreateObj_ allocates for it and most characters), and one
    // class per character. The virtual functions are all declared (the slots
    // of __vt__Q23msw13CChrCoreDodge); otherwise only the constructors, the
    // destructors (declared so that the vtables are not emitted from here)
    // and the sizes are known here; the padding stands for members not
    // mapped yet.
    class CChrCoreDodge : public CChrCore {
        public:
            // The work of the air trick / arts shot states, passed to the
            // SubStateArtsShot* virtuals (see below; only part is mapped).
            struct STATE_WORK_AIR_TRICK_CMN;
            // (contents not known) The work of the diving catch state.
            struct STATE_WORK_DIVING_CATCH;

            // (contents not known) Works and parameter blocks passed to the
            // virtuals below.
            struct STATE_WORK_SIDESTEP;
            struct STATE_WORK_BALL_CATCH;
            struct STATE_WORK_ITEM_SHOOT;
            struct HIT_BALL_STATE;
            struct MOT_ACT_PARAM;
            struct MOT_ACT_CONTEXT;
            // (values not known) The kind of throw (MethdShootSub) and an
            // event of the special move (ArtsProc).
            enum THROW_TYPE {};
            enum ARTS_EVENT {};

            CChrCoreDodge();
            virtual ~CChrCoreDodge();
            // Overrides of CChrCore's virtuals, in vtable order: the slots of
            // __vt__Q23msw13CChrCoreDodge (DodgeF) up to 0xC98 that name
            // CChrCoreDodge, declared with CChrCore's signatures (return types
            // as guessed there).
            virtual void DispFunc();                                   // 0xC
            virtual long GetQueueWorkSize() const;                   // 0x10
            virtual void SetConditionEffect_(long);                    // 0x24
            virtual int IsAirTrickSwingEnd() const;                    // 0x4C
            virtual int IsAirTrickFall() const;                        // 0x50
            virtual void SetDodgeArrangeArea(long);                    // 0x228
            virtual int IsDodgeInfield() const;                        // 0x230
            virtual int IsDodgeCloseRange() const;                     // 0x234
            virtual float GetDodgeCatchCollRadius() const;             // 0x238
            virtual long GetDodgeLockTargetChrIdx() const;             // 0x23C
            virtual void SwitchAutoAimTarget();                        // 0x240
            virtual int IsDodgeTargetMarkerActive();                   // 0x244
            virtual long GetDodgeTargetMarkerType() const;             // 0x248
            virtual long GetDodgeTargetMarkerFixedType() const;        // 0x24C
            virtual int GetDodgeTargetMarkerPos(bcs::FVEC3 &);         // 0x250
            virtual void DodgeForceUpdateAreaCode();                   // 0x254
            virtual int IsDodgeOnBallJump() const;                     // 0x258
            virtual int IsDodgeOnBallJumpFall() const;                 // 0x25C
            virtual int IsDodgeJumpBallChr() const;                    // 0x260
            virtual void SetDodgeJumpBallChr(int);                     // 0x264
            virtual float GetDodgeDivingCatchDist() const;             // 0x268
            virtual int IsDodgeArtsTargetMarkerActive() const;         // 0x26C
            virtual long GetDodgeArtsTargetMarkerType() const;         // 0x270
            virtual long GetDodgeArtsTargetMarkerPos(bcs::FVEC3 &, long); // 0x274
            virtual long GetDodgeArtsTargetMarkerNum() const;          // 0x278
            virtual void GetDodgeArtsTargetMarkerDefaultPos(bcs::FVEC3 &, long); // 0x27C
            virtual long GetDodgeArtsTargetMarkerMoved(bcs::FVEC3 &);  // 0x280
            virtual void SetDodgeArtsTargetMarkerPosClip(bcs::FVEC3 &); // 0x284
            virtual int MoveDodgeArtsTargetMarkerPos(float);           // 0x288
            virtual int CanbeMoveDodgeArtsTargetMarkerPos();           // 0x28C
            virtual void SetDodgeArtsTargetMarkerDisable();            // 0x290
            virtual void GetVel(bcs::FVEC3 &) const;                   // 0x294
            virtual float GetCollSize_BodyDamage() const;              // 0x33C
            virtual float GetCollSize_BodyPush() const;                // 0x340
            virtual int Util_GetIdleMotID();                           // 0x348
            virtual int Util_GetRunMotID();                            // 0x34C
            virtual int Util_GetLandMotID(long);                       // 0x354
            virtual int Util_GetJumpUpMotID(int);                      // 0x358
            virtual void Util_GetJumpLandMotID(int);                   // 0x35C
            virtual int Util_GetDamageMotID(long, long, long);         // 0x360
            virtual void Util_SetOffenseLeaderFirstState(long);        // 0x364
            virtual void Util_SetDefenseLeaderFirstState();            // 0x368
            virtual void Util_AddCoin(long);                           // 0x36C
            virtual void CreateColl_();                                // 0x374
            virtual void Create_();                                    // 0x378
            virtual void MainFunc_();                                  // 0x37C
            virtual void UpdateFunc_();                                // 0x380
            virtual void Reset_();                                     // 0x384
            virtual float GetPrmMoveSpeed() const;                     // 0x38C
            virtual long GetPrmBodySize() const;                       // 0x390
            virtual void SendMessage_(long, long, long, long);         // 0x394
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
            virtual void CtorStateSideStep();                          // 0x404
            virtual void LoopStateSideStep();                          // 0x408
            virtual void DtorStateSideStep();                          // 0x40C
            virtual void CtorStateItemShoot();                         // 0x410
            virtual void LoopStateItemShoot();                         // 0x414
            virtual void DtorStateItemShoot();                         // 0x418
            virtual void CtorStateMotPlay();                           // 0x44C
            virtual void LoopStateMotPlay();                           // 0x450
            virtual void DtorStateMotPlay();                           // 0x454
            virtual void CtorStateSizeChange();                        // 0x458
            virtual void LoopStateSizeChange();                        // 0x45C
            virtual void DtorStateSizeChange();                        // 0x460
            virtual void CtorStateDizzy();                             // 0x470
            virtual void LoopStateDizzy();                             // 0x474
            virtual void DtorStateDizzy();                             // 0x478
            virtual void CtorStateSliding();                           // 0x47C
            virtual void LoopStateSliding();                           // 0x480
            virtual void DtorStateSliding();                           // 0x484
            virtual void CtorStatePass();                              // 0x4AC
            virtual void LoopStatePass();                              // 0x4B0
            virtual void DtorStatePass();                              // 0x4B4
            virtual void CtorStateAerialPass();                        // 0x4B8
            virtual void LoopStateAerialPass();                        // 0x4BC
            virtual void DtorStateAerialPass();                        // 0x4C0
            virtual void CtorStatePassCatch();                         // 0x4C4
            virtual void LoopStatePassCatch();                         // 0x4C8
            virtual void DtorStatePassCatch();                         // 0x4CC
            virtual void CtorStateTrickDance();                        // 0x524
            virtual void LoopStateTrickDance();                        // 0x528
            virtual void DtorStateTrickDance();                        // 0x52C
            virtual void CtorStateJumpShoot();                         // 0x548
            virtual void LoopStateJumpShoot();                         // 0x54C
            virtual void DtorStateJumpShoot();                         // 0x550
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateBallCatch();                         // 0x650
            virtual void LoopStateBallCatch();                         // 0x654
            virtual void DtorStateBallCatch();                         // 0x658
            virtual void CtorStateBallCatchBack();                     // 0x65C
            virtual void LoopStateBallCatchBack();                     // 0x660
            virtual void DtorStateBallCatchBack();                     // 0x664
            virtual void CtorStateStagger();                           // 0x668
            virtual void LoopStateStagger();                           // 0x66C
            virtual void DtorStateStagger();                           // 0x670
            virtual void CtorStateImpact();                            // 0x674
            virtual void LoopStateImpact();                            // 0x678
            virtual void DtorStateImpact();                            // 0x67C
            virtual void CtorStateTurn();                              // 0x680
            virtual void LoopStateTurn();                              // 0x684
            virtual void DtorStateTurn();                              // 0x688
            virtual void CtorStateSpinStagger();                       // 0x68C
            virtual void LoopStateSpinStagger();                       // 0x690
            virtual void DtorStateSpinStagger();                       // 0x694
            virtual void CtorStateArtsOut();                           // 0x698
            virtual void LoopStateArtsOut();                           // 0x69C
            virtual void DtorStateArtsOut();                           // 0x6A0
            virtual void CtorStateLineOver();                          // 0x6A4
            virtual void LoopStateLineOver();                          // 0x6A8
            virtual void DtorStateLineOver();                          // 0x6AC
            virtual void CtorStateCrouch();                            // 0x6B0
            virtual void LoopStateCrouch();                            // 0x6B4
            virtual void DtorStateCrouch();                            // 0x6B8
            virtual void CtorStateDivingCatch();                       // 0x6BC
            virtual void LoopStateDivingCatch();                       // 0x6C0
            virtual void DtorStateDivingCatch();                       // 0x6C4
            virtual void CtorStateSpinAttack();                        // 0x6C8
            virtual void LoopStateSpinAttack();                        // 0x6CC
            virtual void DtorStateSpinAttack();                        // 0x6D0
            virtual void CtorStateFall();                              // 0x7D0
            virtual void LoopStateFall();                              // 0x7D4
            virtual void DtorStateFall();                              // 0x7D8
            virtual void CtorStateRebirth();                           // 0x7DC
            virtual void LoopStateRebirth();                           // 0x7E0
            virtual void DtorStateRebirth();                           // 0x7E4
            virtual int CanbeRun();                                    // 0x7E8
            virtual int MethdRun(float);                               // 0x7EC
            virtual int MethdRun(float, float);                        // 0x7F0
            virtual int CanbeJump();                                   // 0x800
            virtual int MethdJump();                                   // 0x804
            virtual int CanbeSliding();                                // 0x808
            virtual int MethdSliding(float);                           // 0x80C
            virtual int CanbeSizeChange(long);                         // 0x810
            virtual int MethdSizeChange(long, float);                  // 0x814
            virtual int MethdChgStarState();                           // 0x818
            virtual int CanbeChgStarState();                           // 0x81C
            virtual int MethdChgStarState(float);                      // 0x820
            virtual int CanbeSideStep(float, float);                   // 0x840
            virtual int MethdSideStep(float, float);                   // 0x844
            virtual int CanbeQuickTurnState();                         // 0x878
            virtual int CanbeQuickTurn(float);                         // 0x87C
            virtual int MethdQuickTurn(float);                         // 0x880
            virtual int CanbeTrickMove();                              // 0x884
            virtual int MethdTrickMove(float);                         // 0x888
            virtual int CanbeTrickDance();                             // 0x88C
            virtual int MethdTrickDance();                             // 0x890
            virtual int CanbeShoot();                                  // 0x8B4
            virtual int MethdShoot(float);                             // 0x8B8
            virtual int CanbeAerialShoot();                            // 0x8C8
            virtual int MethdAerialShoot();                            // 0x8CC
            virtual int CanbeLayUp();                                  // 0x8DC
            virtual int MethdLayUp();                                  // 0x8E0
            virtual int CanbeAirTrick();                               // 0x8E8
            virtual int MethdAirTrick();                               // 0x8EC
            virtual int CanbeItemBallAttack();                         // 0x900
            virtual int MethdItemBallAttack();                         // 0x904
            virtual int CanbePass(CChrCore *);                         // 0x908
            virtual int MethdPass(float, int);                         // 0x90C
            virtual int CanbePassIdx(long);                            // 0x914
            virtual int MethdPassIdx(long);                            // 0x918
            virtual int CanbeAerialPass(CChrCore *);                   // 0x93C
            virtual int MethdAerialPass(float, int);                   // 0x940
            virtual int CanbeAerialPassIdx(long);                      // 0x944
            virtual int MethdAerialPassIdx(long);                      // 0x948
            virtual int CanbeItemShoot();                              // 0x94C
            virtual int MethdItemShoot(float);                         // 0x950
            virtual int CanbeSetMoveRevise();                          // 0x954
            virtual int MethdSetMoveRevise(float);                     // 0x958
            virtual int MethdDamage(unsigned long, float);             // 0x95C
            virtual int MethdDamage(unsigned long, float, float, float, long, float, long, long); // 0x960
            virtual int MethdDamage(unsigned long, float, float, float, long, float, long, long, long, long); // 0x964
            virtual int CanbeUseItem();                                // 0x968
            virtual int MethdUseItem();                                // 0x96C
            virtual int MethdTurnOver(long);                           // 0x970
            virtual int MethdTimeOver(long);                           // 0x974
            virtual int MethdGameSet();                                // 0x978
            virtual int MethdGameResult(int);                          // 0x97C
            virtual int CanbeBallCatch();                              // 0xA00
            virtual int MethdBallCatch(float);                         // 0xA04
            virtual int CanbeLineOver();                               // 0xA08
            virtual int MethdLineOver(float);                          // 0xA0C
            virtual int CanbeCrouch();                                 // 0xA10
            virtual int MethdCrouch();                                 // 0xA14
            virtual int CanbePassCatchState();                         // 0xA6C
            virtual int MethdSetPassCatchState(CBall *, int, const bcs::FVEC3 &, long); // 0xA74
            virtual int MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &, float); // 0xA78
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
            virtual int CanbeTreadOnQPanel(const CQPanel *);           // 0xB50
            virtual int CanbeLanding() const;                          // 0xB64
            virtual int CanbeFall() const;                             // 0xB68
            virtual int MethdFall();                                   // 0xB6C
            virtual int CanbeRebirth() const;                          // 0xB70
            virtual int MethdRebirth();                                // 0xB74
            virtual void Util_SetEffect(SET_EFFECT_TYPE);              // 0xC1C
            virtual void Util_AttachEffect(ATTACH_EFFECT_TYPE, long);  // 0xC20
            virtual void Util_DetachEffect(ATTACH_EFFECT_TYPE);        // 0xC28
            virtual void CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC4C
            virtual void CallbackBallCatch_(CBall *);                  // 0xC58
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long); // 0xC5C
            virtual void CallbackBallSnatched_(CBall *);               // 0xC60
            virtual int CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, bcs::FVEC3 &, bcs::FVEC3 &); // 0xC64
            virtual void SubStateParalyzeSetEffect_();                 // 0xC94
            virtual void SubStateParalyzeEndEffect_();                 // 0xC98


            // The virtuals CChrCoreDodge adds, 0xC9C..0x119C, in vtable order
            // (names and parameters from the mangled names in
            // __vt__Q23msw13CChrCoreDodge; every vtable of a dodgeball
            // character is 0x11A0 bytes plus its own additions). Return types
            // marked "guessed" come from the DodgeF body (f1 or r3 set before
            // the return) or the name only.
            virtual void AddCountWork_(long);                          // 0xC9C, return type guessed
            virtual void SetCountWorkFlag_(long);                      // 0xCA0, return type guessed
            virtual void ClearCountWorkFlag_(long);                    // 0xCA4, return type guessed
            virtual long GetCountWorkFlag_(long) const;                // 0xCA8, return type guessed
            virtual void UpdateAreaCode_();                            // 0xCAC, return type guessed
            virtual long GetAreaCode_(const bcs::FVEC3 &) const;       // 0xCB0, return type guessed
            virtual int CheckChargePower_();                           // 0xCB4, return type guessed
            virtual void UpdatePos_();                                 // 0xCB8, return type guessed
            virtual int CheckQPanel_();                                // 0xCBC, return type guessed
            virtual int CheckQPanel_(CBall *);                         // 0xCC0, return type guessed
            virtual long GetTeamItem_() const;                         // 0xCC4, return type guessed
            virtual void PushTeamItem_(long);                          // 0xCC8, return type guessed
            virtual int PopTeamItem_();                                // 0xCCC, return type guessed
            virtual void ResetTeamItem_();                             // 0xCD0, return type guessed
            virtual int QueryUseItem_(int);                            // 0xCD4, return type guessed
            virtual void StartStar_();                                 // 0xCD8, return type guessed
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual const CHR_CORE_DODGE::CC_PARAM_DODGE *GetCcParam_() const; // 0xCE0
            virtual long GetPrmOnBallMoveSpeed() const;                // 0xCE4, return type guessed
            virtual long GetPrmCStanceMoveSpeed() const;               // 0xCE8, return type guessed
            virtual long GetPrmTStanceMoveSpeed() const;               // 0xCEC, return type guessed
            virtual long GetPrmLStanceMoveSpeed() const;               // 0xCF0, return type guessed
            virtual long GetPrmMoveSpeedAuto() const;                  // 0xCF4, return type guessed
            virtual void UpdateCmdBuf_();                              // 0xCF8, return type guessed
            virtual int IsNormalDodgeRoll_() const;                    // 0xCFC, return type guessed
            virtual void SubStateCtorDodgeRollSimple_(STATE_WORK_SIDESTEP *); // 0xD00, return type guessed
            virtual void SubStateLoopDodgeRollSimple_(STATE_WORK_SIDESTEP *); // 0xD04, return type guessed
            virtual void SubStateDtorDodgeRollSimple_(STATE_WORK_SIDESTEP *); // 0xD08, return type guessed
            virtual void SubStateCtorDodgeRollPersonality_(STATE_WORK_SIDESTEP *); // 0xD0C, return type guessed
            virtual void SubStateLoopDodgeRollPersonality_(STATE_WORK_SIDESTEP *); // 0xD10, return type guessed
            virtual void SubStateDtorDodgeRollPersonality_(STATE_WORK_SIDESTEP *); // 0xD14, return type guessed
            virtual long GetDodgeRollParam_(MOT_ACT_PARAM &, long) const; // 0xD18, return type guessed
            virtual long GetDodgeRollKeyFrame_(long, long) const;      // 0xD1C, return type guessed
            virtual void DodgeRollPersonalitySpStart_(STATE_WORK_SIDESTEP *); // 0xD20, return type guessed
            virtual void DodgeRollPersonalitySpEnd_(STATE_WORK_SIDESTEP *); // 0xD24, return type guessed
            virtual int IsNormalDivingCatch_() const;                  // 0xD28, return type guessed
            virtual void SubStateCtorDivingCatchSimple_(STATE_WORK_DIVING_CATCH *); // 0xD2C, return type guessed
            virtual void SubStateLoopDivingCatchSimple_(STATE_WORK_DIVING_CATCH *); // 0xD30, return type guessed
            virtual void SubStateDtorDivingCatchSimple_(STATE_WORK_DIVING_CATCH *); // 0xD34, return type guessed
            virtual void SubStateCtorDivingCatchPersonality_(STATE_WORK_DIVING_CATCH *); // 0xD38, return type guessed
            virtual void SubStateLoopDivingCatchPersonality_(STATE_WORK_DIVING_CATCH *); // 0xD3C, return type guessed
            virtual void SubStateDtorDivingCatchPersonality_(STATE_WORK_DIVING_CATCH *); // 0xD40, return type guessed
            virtual long GetDivingCatchParam_(MOT_ACT_PARAM &, long) const; // 0xD44, return type guessed
            virtual long GetDivingCatchKeyFrame_(long, long) const;    // 0xD48, return type guessed
            virtual void DivingCatchPersonalitySpStart_(STATE_WORK_DIVING_CATCH *); // 0xD4C, return type guessed
            virtual void DivingCatchPersonalitySpUpdate_(STATE_WORK_DIVING_CATCH *); // 0xD50, return type guessed
            virtual void DivingCatchPersonalitySpEnd_(STATE_WORK_DIVING_CATCH *); // 0xD54, return type guessed
            virtual void OnDivingCatchStart_(STATE_WORK_DIVING_CATCH *); // 0xD58
            virtual void SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK_CMN *); // 0xD5C, return type guessed
            virtual void SubStateArtsShotSetTrickFrameParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD60, return type guessed
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD64
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD68
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD6C
            virtual float SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD70
            virtual void SubStateArtsShotGetTrickRotFrame_(STATE_WORK_AIR_TRICK_CMN *); // 0xD74, return type guessed
            virtual void SubStateArtsShotGetTrickStartRot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD78, return type guessed
            virtual void SubStateArtsShotStartTargetMarker_(STATE_WORK_AIR_TRICK_CMN *); // 0xD7C, return type guessed
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD84, return type guessed
            virtual void SubStateArtsShotCalcRotParam_(ROT_LERP1 &, float, float); // 0xD88
            virtual void SubStateArtsShotClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &); // 0xD8C, return type guessed
            virtual void SubStateArtsShotDoJumpMove_(STATE_WORK_AIR_TRICK_CMN *); // 0xD90
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD94
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD9C
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA4
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual int SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xDAC
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *); // 0xDB0
            virtual void SubStateArtsShotStartArtsHitback_(STATE_WORK_AIR_TRICK_CMN *); // 0xDB4, return type guessed
            virtual void SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK_CMN *); // 0xDB8, return type guessed
            virtual void SubStateArtsShotDoLandStart_(STATE_WORK_AIR_TRICK_CMN *); // 0xDBC, return type guessed
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC4
            virtual float GetOnBallTime() const;                       // 0xDC8, return type guessed
            virtual int IsInvincibleVsBall() const;                    // 0xDCC, return type guessed
            virtual void SetInvincibleVsBall(float);                   // 0xDD0, return type guessed
            virtual void ResetInvincibleVsBall();                      // 0xDD4, return type guessed
            virtual void UpdateInvincibleCountVsBall_(float);          // 0xDD8, return type guessed
            virtual long GetDodgeArrangeArea() const;                  // 0xDDC, return type guessed
            virtual float GetArtsExplodeRadius() const;                // 0xDE0, return type guessed
            virtual int CanbeJump(int);                                // 0xDE4, return type guessed
            virtual int CanbeAutoJump();                               // 0xDE8, return type guessed
            virtual void MethdAutoJump();                              // 0xDEC, return type guessed
            virtual int CanbeJumpLanding();                            // 0xDF0, return type guessed
            virtual void MethdJumpLanding();                           // 0xDF4, return type guessed
            virtual void MethdShootSub(float, THROW_TYPE, int);        // 0xDF8, return type guessed
            virtual int CanbeCurveShot();                              // 0xDFC, return type guessed
            virtual void MethdCurveShot(float);                        // 0xE00, return type guessed
            virtual void MethdArtsOut_(long, long, long);              // 0xE04, return type guessed
            virtual int CanbeTurn(float, float);                       // 0xE08, return type guessed
            virtual void MethdTurn(float, float);                      // 0xE0C, return type guessed
            virtual int CanbeLockTarget() const; // 0xE10
            virtual int CanbeSwitchLockTarget() const; // 0xE14
            virtual long GetLockTargetChrIdx() const;                  // 0xE18, return type guessed
            virtual CChrCore *GetLockTargetChrCore() const;            // 0xE1C, return type guessed
            virtual int IsAvailLockTarget() const;                     // 0xE20, return type guessed
            virtual long GetShootTargetChrIdx() const;                 // 0xE24, return type guessed
            virtual long GetNearStanceDist(long, float) const;         // 0xE28, return type guessed
            virtual int QueryManualLock(int);                          // 0xE2C, return type guessed
            virtual int QueryItemLock(int);                            // 0xE30, return type guessed
            virtual int QueryManualAdjustTarget(float *, float *);     // 0xE34, return type guessed
            virtual float GetShootSpeed(long) const;                   // 0xE38, return type guessed
            virtual void ExportShotParam(CBallDodge *) const;          // 0xE3C, return type guessed
            virtual int IsEnable1on1Marker() const;                    // 0xE40, return type guessed
            virtual int IsEnable1on1Camera() const;                    // 0xE44, return type guessed
            virtual long Get1on1MarkerPos(bcs::FVEC3 &) const;         // 0xE48, return type guessed
            virtual long Get1on1MarkerDispPos(bcs::FVEC3 &) const;     // 0xE4C, return type guessed
            virtual int IsInner1on1Area(const bcs::FVEC3 &, float) const; // 0xE50, return type guessed
            virtual float Get1on1MarkerPosRaw_(bcs::FVEC3 &) const;    // 0xE54, return type guessed
            virtual void Update1on1Target_(float);                     // 0xE58, return type guessed
            virtual int IsNeed1on1Stance() const;                      // 0xE5C, return type guessed
            virtual int IsInner1on1AreaAnyChr() const;                 // 0xE60, return type guessed
            virtual int FindInner1on1AreaAnyChr() const;               // 0xE64, return type guessed
            virtual long GetOuterTargetDir();                          // 0xE68, return type guessed
            virtual int IsOuterAssigned() const;                       // 0xE6C, return type guessed
            virtual long GetOuterTargetPos(bcs::FVEC3 &);              // 0xE70, return type guessed
            virtual int IsNowCombinationMode() const;                  // 0xE74, return type guessed
            virtual void UpdateCombination();                          // 0xE78, return type guessed
            virtual long CalcDodgeAssumedPos(float) const;             // 0xE7C, return type guessed
            virtual int QueryCombination();                            // 0xE80, return type guessed
            virtual void CombinationStart();                           // 0xE84, return type guessed
            virtual void CombinationEnd();                             // 0xE88, return type guessed
            virtual void CombinationIconDisp(int);                     // 0xE8C, return type guessed
            virtual void CombinationIconErase_(int);                   // 0xE90, return type guessed
            virtual void CombinationIconReset();                       // 0xE94, return type guessed
            virtual void CombinationTelopDisp(int);                    // 0xE98, return type guessed
            virtual void CombinationTelopErase_(int);                  // 0xE9C, return type guessed
            virtual void CombinationTelopReset();                      // 0xEA0, return type guessed
            virtual void CombinationIconUpdate(float);                 // 0xEA4, return type guessed
            virtual void InitLife();                                   // 0xEA8, return type guessed
            // the character's lives left (CGameDodge::GetChrRestLife(m_nPlayerNo))
            virtual int GetLife() const; // 0xEAC
            virtual void SetLife(long);                                // 0xEB0, return type guessed
            virtual void AddLife(long);                                // 0xEB4, return type guessed
            virtual void SubLife(long);                                // 0xEB8, return type guessed
            virtual long GetMaxLife() const;                           // 0xEBC, return type guessed
            virtual void SetMaxLife(long);                             // 0xEC0, return type guessed
            virtual long GetRecoverLife() const;                       // 0xEC4, return type guessed
            virtual void SetRecoverLife(long);                         // 0xEC8, return type guessed
            virtual void AddRecoverLife(long);                         // 0xECC, return type guessed
            virtual long GetMaxRecoverLife() const;                    // 0xED0, return type guessed
            virtual void SetMaxRecoverLife(long);                      // 0xED4, return type guessed
            virtual float GetStamina() const;                          // 0xED8, return type guessed
            virtual void SetStamina(float);                            // 0xEDC, return type guessed
            virtual float GetMaxStamina() const;                       // 0xEE0, return type guessed
            virtual void SetMaxStamina(float);                         // 0xEE4, return type guessed
            virtual void AddStamina(float);                            // 0xEE8, return type guessed
            virtual int IsNowStaminaCharge() const;                    // 0xEEC, return type guessed
            virtual int IsHaveRainbowBall() const;                     // 0xEF0, return type guessed
            virtual void UpdateStamina(float);                         // 0xEF4, return type guessed
            virtual float GetStaminaCost(long) const;                  // 0xEF8, return type guessed
            virtual int CheckStaminaCost(long) const;                  // 0xEFC, return type guessed
            virtual int DoStaminaCost(long);                           // 0xF00, return type guessed
            virtual void UpdateGameGuide(float);                       // 0xF04, return type guessed
            virtual void UpdateGameGuide_JumpBall();                   // 0xF08, return type guessed
            virtual void UpdateGameGuide_Catch();                      // 0xF0C, return type guessed
            virtual void UpdateGameGuide_Attack();                     // 0xF10, return type guessed
            virtual void UpdateGameGuide_Uncatchable();                // 0xF14, return type guessed
            virtual void UpdateGameGuide_DodgeRoll();                  // 0xF18, return type guessed
            virtual void UpdateGameGuide_DivingCatch();                // 0xF1C, return type guessed
            virtual void SuccessGameGuide_JumpBall();                  // 0xF20, return type guessed
            virtual void SuccessGameGuide_Catch();                     // 0xF24, return type guessed
            virtual void SuccessGameGuide_Attack();                    // 0xF28, return type guessed
            virtual void SuccessGameGuide_Uncatchable();               // 0xF2C, return type guessed
            virtual void SuccessGameGuide_DodgeRoll();                 // 0xF30, return type guessed
            virtual void SuccessGameGuide_DivingCatch();               // 0xF34, return type guessed
            virtual void SuccessGameGuideSub(long);                    // 0xF38, return type guessed
            virtual int GameGuide_IsDispAny() const;                   // 0xF3C, return type guessed
            virtual void GameGuide_SetDisableTime();                   // 0xF40, return type guessed
            virtual int GameGuide_IsDisabled() const;                  // 0xF44, return type guessed
            virtual int IsAutoSubShoot_() const;                       // 0xF48, return type guessed
            virtual long GetMainShootType_() const;                    // 0xF4C, return type guessed
            virtual long GetSubShootType_() const;                     // 0xF50, return type guessed
            virtual void Util_ShootBall(float, long, long);            // 0xF54, return type guessed
            virtual void Util_ShootBall(CBall *, float, long, long);   // 0xF58, return type guessed
            virtual void Util_ShootBall(const bcs::FVEC3 &, long, long); // 0xF5C, return type guessed
            // (return type guessed) Throws pBall at rPos.
            virtual void Util_ShootBall(CBall *pBall, const bcs::FVEC3 &rPos, long, long); // 0xF60
            virtual void Util_JumpShootBall(long, long);               // 0xF64, return type guessed
            virtual void Util_JumpShootBall__(CBall *, long, long);    // 0xF68, return type guessed
            virtual void Util_PassBall(long, CChrCore *, const bcs::FVEC3 &, float, int, int); // 0xF6C, return type guessed
            virtual void Util_BallHit(CBall *, long);                  // 0xF70, return type guessed
            virtual void Util_BallHit_Judge(CBall *, long);            // 0xF74, return type guessed
            virtual void Util_BallHit_TrgTouch(CBall *, long);         // 0xF78, return type guessed
            virtual void Util_SetBallHitEffect(CBall *, long);         // 0xF7C, return type guessed
            virtual void Util_BallOutBallMode(CBall *, const bcs::FVEC3 &, long); // 0xF80, return type guessed
            virtual CBall *Util_GetNearShootBall(unsigned long) const; // 0xF84, return type guessed
            virtual CBall *Util_GetNearPassBall(unsigned long) const;  // 0xF88, return type guessed
            virtual CBall *Util_GetNearOnBall(unsigned long) const;    // 0xF8C, return type guessed
            virtual CChrCore *Util_GetNearOnBallChr(unsigned long) const; // 0xF90, return type guessed
            virtual CBall *Util_GetNearLooseBall(long) const;          // 0xF94, return type guessed
            virtual CBall *Util_GetNearReboundBall(long, long) const;  // 0xF98, return type guessed
            virtual CBall *Util_GetLookBall() const;                   // 0xF9C, return type guessed
            virtual CBall *Util_GetPassBall_() const;                  // 0xFA0, return type guessed
            virtual long Util_GetCatchState(CBall *);                  // 0xFA4, return type guessed
            virtual int Util_DispatchCatchState(long, CBall *, float, int) const; // 0xFA8, return type guessed
            virtual float Util_GetCurCatchFrame();                     // 0xFAC, return type guessed
            virtual void Util_BallCatch(CBall *, long);                // 0xFB0, return type guessed
            virtual void Util_PushBackCatch(int);                      // 0xFB4, return type guessed
            virtual void Util_CatchBack(long, float);                  // 0xFB8, return type guessed
            virtual void Util_SetCatchBackParam(STATE_WORK_BALL_CATCH *); // 0xFBC, return type guessed
            virtual int Util_GetCatchMethod(const CBall *, int) const; // 0xFC0, return type guessed
            virtual int CanbeBallPick_() const;                        // 0xFC4, return type guessed
            virtual int CanbeHitBallDamage_(const CBall *) const;      // 0xFC8, return type guessed
            virtual int CanbeHitBallDamage_(const HIT_BALL_STATE &) const; // 0xFCC, return type guessed
            virtual int CanbeHitBallDamage_(unsigned long) const;      // 0xFD0, return type guessed
            virtual void Util_DoDirMoveDodge(const CC_DATA::CHR_PARAM::MOVE_PARAM &, float, float, CChrCore::DIR_MOVE &, int); // 0xFD4, return type guessed
            virtual void Util_SetJumpParamDodge(long);                 // 0xFD8, return type guessed
            virtual void Util_SetJumpParamDodge(float, float, float);  // 0xFDC, return type guessed
            virtual void Util_DoJumpDodge(float, int);                 // 0xFE0, return type guessed
            virtual int Util_DoJumpMoveLandDodge(float);               // 0xFE4, return type guessed
            virtual long GetMotID_ItemIdle(long);                      // 0xFE8, return type guessed
            virtual long GetMotID_ItemRun(long);                       // 0xFEC, return type guessed
            virtual long GetMotID_ItemShoot(long);                     // 0xFF0, return type guessed
            virtual long GetMotIDTbl_SideStep(long) const;             // 0xFF4, return type guessed
            virtual long GetMotIDType_SideStep() const; // 0xFF8
            virtual long GetMotIDTbl_DivingCatch(long) const;          // 0xFFC, return type guessed
            virtual long GetMotIDType_DivingCatch() const;             // 0x1000, return type guessed
            virtual void AnimSetSpeedScale(unsigned long, float);      // 0x1004, return type guessed
            virtual void AnimPlayBufPush(unsigned long, long, int, float, float, float); // 0x1008, return type guessed
            virtual void AnimPlayBufClear();                           // 0x100C, return type guessed
            virtual void AnimPlayBufUpdate();                          // 0x1010, return type guessed
            virtual int AnimPlayBufPushBack_();                        // 0x1014, return type guessed
            virtual void AnimPlayBufPopFront_();                       // 0x1018, return type guessed
            virtual int AnimPlayBufGetFront_();                        // 0x101C, return type guessed
            virtual void MotActSet(MOT_ACT_CONTEXT &, long, const MOT_ACT_PARAM &, float, float); // 0x1020, return type guessed
            virtual float MotActUpdate(MOT_ACT_CONTEXT &, float);      // 0x1024, return type guessed
            virtual CChrCore *Util_GetNearOuterChr(long, float, float) const; // 0x1028, return type guessed
            virtual CChrCore *Util_GetNearInnerChr(long, float, float) const; // 0x102C, return type guessed
            virtual CChrCore *Util_GetNearOuterChrExclusive(long, float, float) const; // 0x1030, return type guessed
            virtual CChrCore *Util_GetNearInnerChrExclusive(long, float, float) const; // 0x1034, return type guessed
            virtual CChrCore *Util_GetPassToChr(float) const;          // 0x1038, return type guessed
            virtual long Util_GetPassToPos(bcs::FVEC3 *, float) const; // 0x103C, return type guessed
            virtual void OnShootSubStepChange(const STATE_WORK_ITEM_SHOOT *); // 0x1040, return type guessed
            virtual void ArtsFadeOut() const;                          // 0x1044, return type guessed
            virtual void ArtsFadeIn() const;                           // 0x1048, return type guessed
            virtual void ArtsFadeReset() const;                        // 0x104C, return type guessed
            virtual int IsArtsInputEnable() const;                     // 0x1050, return type guessed
            virtual int IsArtsBranchEnable() const;                    // 0x1054, return type guessed
            virtual void ArtsInputCheck();                             // 0x1058, return type guessed
            virtual void ArtsNextSubStep(int);                         // 0x105C, return type guessed
            virtual void ArtsNextSubStep(int, long);                   // 0x1060, return type guessed
            virtual void DoArtsNextSubStep();                          // 0x1064, return type guessed
            virtual void ArtsSignEffectKick();                         // 0x1068, return type guessed
            virtual int CanbeArtsNextSubStep(long) const;              // 0x106C, return type guessed
            virtual void ArtsParamInit();                              // 0x1070, return type guessed
            virtual void ArtsPreProc();                                // 0x1074, return type guessed
            virtual void ArtsProc(ARTS_EVENT);                         // 0x1078, return type guessed
            virtual void ArtsDoMove();                                 // 0x107C, return type guessed
            virtual void ArtsOnLanding();                              // 0x1080, return type guessed
            virtual int IsArtsTrig(long, float, float);                // 0x1084, return type guessed
            virtual float CalcParamMod_(float, float, float, float, float) const; // 0x1088, return type guessed
            virtual long GetCatchParamType_(const HIT_BALL_STATE *) const; // 0x108C, return type guessed
            virtual long GetCatchTotalFrameBase_(const HIT_BALL_STATE *) const; // 0x1090, return type guessed
            virtual long GetCatchFrameBase_(const HIT_BALL_STATE *) const; // 0x1094, return type guessed
            virtual long GetFlickFrameBase_(const HIT_BALL_STATE *) const; // 0x1098, return type guessed
            virtual long GetImpactFrameBase_(const HIT_BALL_STATE *) const; // 0x109C, return type guessed
            virtual long GetCatchTotalFrame_(const CBall *) const;     // 0x10A0, return type guessed
            virtual long GetCatchFrame_() const;                       // 0x10A4, return type guessed
            virtual long GetFlickFrame_() const;                       // 0x10A8, return type guessed
            virtual long GetImpactFrame_() const;                      // 0x10AC, return type guessed
            virtual void SaveBallParam_(const CBall *);                // 0x10B0, return type guessed
            virtual long ConvHitBallState(const CBall *) const;        // 0x10B4, return type guessed
            virtual void ConditionalClearNormalChargePower();          // 0x10B8, return type guessed
            virtual long CalcCatchAngle(const CBall *) const;          // 0x10BC, return type guessed
            virtual float CalcCatchAngleScore(float) const;            // 0x10C0, return type guessed
            virtual long GetStanceType() const;                        // 0x10C4, return type guessed
            virtual int IsNeedNearStance(const CBall *) const;         // 0x10C8, return type guessed
            virtual int IsNeedNearAlert() const;                       // 0x10CC, return type guessed
            virtual int SelectStanceType() const;                      // 0x10D0, return type guessed
            virtual int SendPassCall_();                               // 0x10D4, return type guessed
            virtual void MethdJump_(long);                             // 0x10D8, return type guessed
            virtual int CanbeOverRun_() const;                         // 0x10DC, return type guessed
            virtual void MethdLineOver_(float);                        // 0x10E0, return type guessed
            virtual int IsLineOver_(int) const;                        // 0x10E4, return type guessed
            virtual int CallbackBallCatch__(CBall *);                  // 0x10E8, return type guessed
            virtual int IsForceReflect_();                             // 0x10EC, return type guessed
            virtual void UpdateLockTargetDir_();                       // 0x10F0, return type guessed
            virtual void UnlockTargetDir_();                           // 0x10F4, return type guessed
            virtual int IsAvailLockTargetDir_() const;                 // 0x10F8, return type guessed
            virtual float GetLockTargetDir_() const;                   // 0x10FC, return type guessed
            virtual void SetLockTargetChr_(long);                      // 0x1100, return type guessed
            virtual void SetLockTargetChr_(CChrCore *);                // 0x1104, return type guessed
            virtual void UnlockTargetChr_();                           // 0x1108, return type guessed
            virtual void ClearTargetChr_();                            // 0x110C, return type guessed
            virtual long GetPrevLockTargetChrIdx_() const;             // 0x1110, return type guessed
            virtual CChrCore *GetPrevLockTargetChrCore_() const;       // 0x1114, return type guessed
            virtual int IsLockCursor_() const;                         // 0x1118, return type guessed
            virtual void OnLockTarget_();                              // 0x111C, return type guessed
            virtual void OnUnlockTarget_();                            // 0x1120, return type guessed
            virtual void LifeIconIn(float);                            // 0x1124, return type guessed
            virtual void LifeIconOut(float);                           // 0x1128, return type guessed
            virtual void LifeIconOn(long, long);                       // 0x112C, return type guessed
            virtual void LifeIconOff();                                // 0x1130, return type guessed
            virtual void LifeIconUpdate(float);                        // 0x1134, return type guessed
            virtual void InitQuickTurn();                              // 0x1138, return type guessed
            virtual void UpdateQuickTurn();                            // 0x113C, return type guessed
            virtual void UpdateAutoAim();                              // 0x1140, return type guessed
            virtual long GetAutoAimType() const;                       // 0x1144, return type guessed
            virtual int CheckBallCatchRange_(const CBall *) const;     // 0x1148, return type guessed
            virtual int CheckCrouchRange_(const CBall *);              // 0x114C, return type guessed
            virtual void UpdateTimer_();                               // 0x1150, return type guessed
            virtual void SetSmashDmg(int);                             // 0x1154, return type guessed
            virtual int IsNowSmashDmg() const;                         // 0x1158, return type guessed
            virtual int IsNowFalling() const;                          // 0x115C, return type guessed
            virtual void PlaySeSlot(long, int);                        // 0x1160, return type guessed
            virtual void StopSeSlot(long);                             // 0x1164, return type guessed
            virtual int IsPlaySeSlot(long) const;                      // 0x1168, return type guessed
            virtual void CreateEffect(long, long, unsigned long, unsigned long, unsigned long); // 0x116C, return type guessed
            virtual void CreateAttachEffectAuto(long, long, unsigned long); // 0x1170, return type guessed
            virtual void CreateAttachEffect(long, long, unsigned long, const bcs::FVEC3 *, int, int, int, unsigned long); // 0x1174, return type guessed
            virtual void CreateAttachEffectAuto_ArtsOpt(long, long, unsigned long); // 0x1178, return type guessed
            virtual void CreateAttachEffect_ArtsOpt(long, long, unsigned long, const bcs::FVEC3 *, int, int, int, unsigned long); // 0x117C, return type guessed
            virtual long GetEffectHandle(long) const;                  // 0x1180, return type guessed
            virtual void SetEffectSpeed(long, float);                  // 0x1184, return type guessed
            virtual int IsEffectAlive(long) const;                     // 0x1188, return type guessed
            virtual void StopEffect(long);                             // 0x118C, return type guessed
            virtual void EraseEffect(long);                            // 0x1190, return type guessed
            virtual void StopEffectAll();                              // 0x1194, return type guessed
            virtual void EraseEffectAll();                             // 0x1198, return type guessed
            virtual int Eff_CreateAttach_Node(long, long, long);       // 0x119C, return type guessed

        protected:
            // (return type guessed) Takes the ball for the arts shot.
            int SubStateArtsShotDoGetBallShotTrg_(STATE_WORK_AIR_TRICK_CMN *pWork);

            unsigned char m_Unk574[0x930 - 0x574];
    };

    // Size 0x930.
    class CChrCoreDodgeMario : public CChrCoreDodge {
        public:
            CChrCoreDodgeMario();
            virtual ~CChrCoreDodgeMario();
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD9C
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
    };

    // (only what Luigi's vacuum and Toad's trick use is mapped; names guessed)
    struct CChrCoreDodge::STATE_WORK_AIR_TRICK_CMN {
        unsigned char m_Unk00[0xAC];
        float m_fJumpUnk;              // 0xAC, for CChrCore::m_fJumpUnk224
        bcs::FVEC3 m_TargetPos;        // 0xB0, where the trick turns to
        unsigned char m_UnkBC[0xC8 - 0xBC];
        bcs::FVEC3 m_MovePos;          // 0xC8, where the trick moves to
        float m_fUnkD4;                // 0xD4
        unsigned char m_UnkD8[0x4];
        CC_DATA::CHR_PARAM::MOVE_PARAM m_MoveParam; // 0xDC
        unsigned char m_UnkEC[0xFC - 0xEC];
        CChrCore::ROT_LERP1 m_RotLerp; // 0xFC, the turn (SubStateArtsShotCalcRotParam_)
        float m_fRotY;                 // 0x10C, the yaw the trick turns to
        long m_nTrickStep;             // 0x110
        float m_fTrickFrame;           // 0x114, the length of the trick
        float m_fTrickCount;           // 0x118, frames of it done
        unsigned char m_Unk11C[0x4];
        float m_fAnimSpeed;            // 0x120
        long m_nArtsLevel;             // 0x124
        float m_fShotFrame;            // 0x128, the frame of the shot motion the ball leaves at (name guessed)
    };

    // Size 0x968.
    class CChrCoreDodgeLuigi : public CChrCoreDodge {
        public:
            CChrCoreDodgeLuigi();
            virtual ~CChrCoreDodgeLuigi();
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual void OnDivingCatchStart_(STATE_WORK_DIVING_CATCH *); // 0xD58
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *); // 0xDB0
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC4
            virtual int CanbeSwitchLockTarget() const;                 // 0xE14

            // (name from its destructor; what it does is not known beyond
            // that) A model and an attached effect that the destructor
            // releases. Nothing in DodgeF creates them: the code that did was
            // most likely stripped by the link (its constants are still in
            // .rodata).
            class CIntakeAir {
                private:
                    bcs::CScnObj *m_pMdl;        // 0x00
                    long m_nUnk04;               // 0x04
                    long m_nUnk08;               // 0x08
                    unsigned char m_Unk0C[0x28 - 0x0C];
                    unsigned long m_hEffect;     // 0x28, CEffectCtrl attach handle

                public:
                    // (the vptr follows the members, at 0x2C)
                    CIntakeAir()
                    {
                        m_pMdl = 0;
                        m_nUnk04 = -1;
                        m_nUnk08 = 0;
                        m_hEffect = 0;
                    }
                    virtual ~CIntakeAir();
            };

        protected:
            // (inline helper, name guessed) Deletes the vacuum task, if any.
            void DeleteVacuum_();

            CIntakeAir m_IntakeAir;      // 0x930
            unsigned long m_hVacuum;     // 0x960, task id of the vacuum, 0 for none
            int m_bVacuum;               // 0x964, the vacuum pulls
    };

    // Size 0x930.
    class CChrCoreDodgePeach : public CChrCoreDodge {
        public:
            CChrCoreDodgePeach();
            virtual ~CChrCoreDodgePeach();
    };

    // Size 0x930.
    class CChrCoreDodgeDaisy : public CChrCoreDodge {
        public:
            CChrCoreDodgeDaisy();
            virtual ~CChrCoreDodgeDaisy();
    };

    // Size 0x930.
    class CChrCoreDodgeYoshi : public CChrCoreDodge {
        public:
            CChrCoreDodgeYoshi();
            virtual ~CChrCoreDodgeYoshi();
    };

    // Size 0x930.
    class CChrCoreDodgeWario : public CChrCoreDodge {
        public:
            CChrCoreDodgeWario();
            virtual ~CChrCoreDodgeWario();
    };

    // Size 0x940.
    // His special shot splits the ball into four (the "divide balls"),
    // attached to the nodes of his extra model and thrown one by one.
    class CChrCoreDodgeWaluigi : public CChrCoreDodge {
        public:
            enum { DIVIDE_BALL_NUM = 4 };

            CChrCoreDodgeWaluigi();
            virtual ~CChrCoreDodgeWaluigi();
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD6C
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA4
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual int SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xDAC
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
            virtual long GetMotIDType_SideStep() const;                // 0xFF8
            virtual void DivideBallCreate();                           // 0x11A0
            virtual CBall *DivideBallGet(long nIndex);                 // 0x11A4
            virtual const CBall *DivideBallGet(long nIndex) const;     // 0x11A8
            virtual void DivideBallDelete(long nIndex);                // 0x11AC
            virtual void ReleaseBallDelete(long nIndex);               // 0x11B0
            virtual void DivideBallDeleteAll();                        // 0x11B4

        protected:
            // The three copies made by CBallMan::AddBall and, last, the
            // original ball.
            CBall *m_apDivideBall[DIVIDE_BALL_NUM]; // 0x930
    };

    // Size 0x930.
    class CChrCoreDodgeDonkey : public CChrCoreDodge {
        public:
            CChrCoreDodgeDonkey();
            virtual ~CChrCoreDodgeDonkey();
    };

    // Size 0x930.
    class CChrCoreDodgeDiddy : public CChrCoreDodge {
        public:
            CChrCoreDodgeDiddy();
            virtual ~CChrCoreDodgeDiddy();
    };

    // Size 0x930.
    class CChrCoreDodgeKinopio : public CChrCoreDodge {
        public:
            CChrCoreDodgeKinopio();
            virtual ~CChrCoreDodgeKinopio();
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD64
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD68
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD94
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
    };

    // Size 0x930.
    class CChrCoreDodgeKoopa : public CChrCoreDodge {
        public:
            CChrCoreDodgeKoopa();
            virtual ~CChrCoreDodgeKoopa();
    };

    // Size 0x930.
    class CChrCoreDodgeKoopaJr : public CChrCoreDodge {
        public:
            CChrCoreDodgeKoopaJr();
            virtual ~CChrCoreDodgeKoopaJr();
    };

    // Size 0x930.
    class CChrCoreDodgeMoogle : public CChrCoreDodge {
        public:
            CChrCoreDodgeMoogle();
            virtual ~CChrCoreDodgeMoogle();
    };

    // Size 0x930.
    // The cactus: his special shot is a jump during which he fires a needle
    // (CC13_DODGE::CcSabotenNeedleDodge).
    class CChrCoreDodgeSaboten : public CChrCoreDodge {
        public:
            CChrCoreDodgeSaboten();
            virtual ~CChrCoreDodgeSaboten();
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD64
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD68
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD6C
            virtual float SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK_CMN *, long); // 0xD70
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
    };

    // Size 0x944.
    class CChrCoreDodgeNinja : public CChrCoreDodge {
        public:
            CChrCoreDodgeNinja();
            virtual ~CChrCoreDodgeNinja();

        protected:
            unsigned char m_Unk930[0x944 - 0x930];
    };

    // Size 0x934.
    class CChrCoreDodgeSiroma : public CChrCoreDodge {
        public:
            CChrCoreDodgeSiroma();
            virtual ~CChrCoreDodgeSiroma();

        protected:
            unsigned char m_Unk930[0x934 - 0x930];
    };

    // Size 0x934.
    class CChrCoreDodgeSlime : public CChrCoreDodge {
        public:
            CChrCoreDodgeSlime();
            virtual ~CChrCoreDodgeSlime();

        protected:
            unsigned char m_Unk930[0x934 - 0x930];
    };

    // Size 0x934.
    // The Black Mage: his special shot puts a lock-on mark
    // (CC17_DODGE::CcKuromaLockonUnit) on every character of the other team
    // and strikes them all a while later (CC17_DODGE::CcKuromaMagicMgr).
    class CChrCoreDodgeKuroma : public CChrCoreDodge {
        public:
            CChrCoreDodgeKuroma();
            virtual ~CChrCoreDodgeKuroma();
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *); // 0xC44
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC0
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *); // 0xDC4

        protected:
            // (inline helper, name guessed) Deletes the magic task and
            // forgets it.
            void ReleaseMagic_();

            // (name guessed) The task id of the special shot's
            // CcKuromaMagicMgr, 0 for none.
            unsigned long m_hMagic; // 0x930
    };

    // Size 0x930.
    class CChrCoreDodgeMii00 : public CChrCoreDodge {
        public:
            CChrCoreDodgeMii00();
            virtual ~CChrCoreDodgeMii00();
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD9C
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
    };

    // Size 0x930.
    class CChrCoreDodgeMii01 : public CChrCoreDodge {
        public:
            CChrCoreDodgeMii01();
            virtual ~CChrCoreDodgeMii01();
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual CHR_CORE_DODGE::CC_PARAM_DODGE *NewCcParam_(long nHeap) const; // 0xCDC
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD80
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xD98
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *); // 0xD9C
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *); // 0xDA8
    };

    // Size 0x930 (the party game "dodge or die").
    class CChrCoreDodgeDodgeOrDie : public CChrCoreDodge {
        public:
            CChrCoreDodgeDodgeOrDie();
            virtual ~CChrCoreDodgeDodgeOrDie();
    };
}

#endif /* !MSW_CCHRCOREDODGE_HPP_ */
