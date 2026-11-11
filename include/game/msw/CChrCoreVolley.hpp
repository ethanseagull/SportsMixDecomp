#ifndef MSW_CCHRCOREVOLLEY_HPP_
#define MSW_CCHRCOREVOLLEY_HPP_

#include "bcs/FVEC4.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrCore.hpp"

namespace msw {

    namespace CHR_CORE_VOLLEY {
        class CC_PARAM_VOLLEY;
    }

    // The volleyball character core, and one class per character (VolleyF).
    //
    // INCOMPLETE: the virtual functions are all declared (the 867 slots of
    // __vt__Q23msw14CChrCoreVolley, read from VolleyF, as CChrCoreBoss.hpp
    // does for the boss game), but few members are mapped and CChrCore is
    // incomplete too (see there). Each destructor is declared first so that
    // it decides where the vtable goes: the character units
    // (game/volley/CChrCoreVolley<Name>.cpp) define it and so emit the
    // vtable. The sizes are what CChrManVolley::CreateObj_ allocates.
    class CChrCoreVolley : public CChrCore {
        public:
            // The work of the block jump, receive, spike and air trick
            // states, passed to the SubState* virtuals (contents mapped only
            // where used).
            struct STATE_WORK_BLOCK_JUMP;
            struct STATE_WORK_RECEIVE;
            struct STATE_WORK_SPIKE;
            struct STATE_WORK_AIR_TRICK;

            CChrCoreVolley();
            virtual ~CChrCoreVolley();

            // Overrides of CChrCore's virtuals, in vtable order (the slots of
            // __vt__Q23msw14CChrCoreVolley that name CChrCoreVolley; return
            // types as guessed in CChrCore).
            virtual long GetQueueWorkSize() const;                     // 0x10
            virtual long GetMainOrderValue(long);                      // 0x18
            virtual void *GetHoldBall() const;                         // 0x1C
            virtual void SetConditionEffect_(long, unsigned long);     // 0x28
            virtual int IsHoldBall() const;                            // 0x38
            virtual int IsAirTrickSwingEnd() const;                    // 0x4C
            virtual int IsAirTrickFall() const;                        // 0x50
            virtual void GetTargetMarkerPos(bcs::FVEC3 &);             // 0x58
            virtual int MoveTargetMarkerPos(float);                    // 0x5C
            virtual long CheckGuideTelopDisp();                        // 0x64
            virtual int IsVolleyRightBallTouch(long) const;            // 0x8C
            virtual int IsVolleyServeMode() const;                     // 0x90
            virtual int IsVolleyJumpServeMode() const;                 // 0x94
            virtual int IsVolleyTargetMarkerActive() const;            // 0x98
            virtual long GetVolleyTargetMarkerNum() const;             // 0x9C
            virtual void GetVolleyTargetMarkerDefaultPos(long, bcs::FVEC3 &, int); // 0xA0
            virtual long GetVolleyTargetMarkerType() const;            // 0xA4
            virtual long GetVolleyTargetMarkerFixedType() const;       // 0xA8
            virtual int GetVolleyTargetMarkerPos(bcs::FVEC3 &, long);  // 0xAC
            virtual int GetVolleyTargetMarkerMoved(bcs::FVEC3 &);      // 0xB0
            virtual int MoveVolleyTargetMarkerPos(float);              // 0xB4
            virtual int CanbeMoveVolleyTargetMarkerPos();              // 0xB8
            virtual int IsVolleyTargetMarkerStealthMode();             // 0xBC
            virtual float GetVolleySpikeAdsorbRangeXZ(int) const;      // 0xC0
            virtual float GetVolleySpikeAdsorbRangeY(int) const;       // 0xC4
            virtual int IsVolleySpikeJumpStart() const;                // 0xC8
            virtual int IsVolleySuperSpikeJump() const;                // 0xCC
            virtual int IsVolleySpikeSwingEnd() const;                 // 0xD0
            virtual int IsVolleySpikeFall() const;                     // 0xD4
            virtual int IsVolleyAirTrickSwingEnd() const;              // 0xD8
            virtual int IsVolleyAirTrickFall() const;                  // 0xDC
            virtual int IsVolleyBlockJumpStart() const;                // 0xE0
            virtual int IsVolleyBlockFall() const;                     // 0xE4
            virtual int IsVolleyBlockSuccess() const;                  // 0xE8
            virtual int IsVolleyDispSpikeJumpMarker() const;           // 0xEC
            virtual int GetVolleySpikeFeintTargetPos(float, bcs::FVEC3 &, bcs::FVEC3 &, bcs::FVEC3 &); // 0xF0
            virtual int GetVolleyBallShotCollPos(long, bcs::FVEC3 &, float &) const; // 0xF4
            virtual void GetVolleySpikeOffsetPos(long, bcs::FVEC3 &) const; // 0xF8
            virtual CBall *GetVolleyTargetBall() const;                // 0xFC
            virtual int IsVolleySuperSpikeCurveType() const;           // 0x100
            virtual float GetVolleySuperSpikeCurveRate() const;        // 0x104
            virtual float GetVolleySuperSpikeCurveMaxDist() const;     // 0x108
            virtual float GetVolleySuperSpikeCurveMinDist() const;     // 0x10C
            virtual float GetVolleyBlockJumpDelayFrame() const;        // 0x110
            virtual int IsVolleyEnableDiveReceive() const;             // 0x114
            virtual int IsVolleyCombinationMode() const;               // 0x118
            virtual long GetVolleyCombinationRole() const;             // 0x11C
            virtual long GetVolleyCombinationStep() const;             // 0x120
            virtual void SetVolleyCombinationRoleDown();               // 0x124
            virtual int IsVolleyDispExIcon(long) const;                // 0x128
            virtual long CheckVolleyLineMsgDisp();                     // 0x12C
            virtual int IsVolleyCombinationReceiver() const;           // 0x130
            virtual int IsVolleyCombinationAttacker() const;           // 0x134
            virtual int IsVolleyArtsKoopaShotFire() const;             // 0x138
            virtual int IsVolleyArtsKoopaFireEnd() const;              // 0x13C
            virtual long GetVolleyArtsExInput();                       // 0x140
            virtual float GetCollSize_BodyDamage() const;              // 0x33C
            virtual float GetCollSize_BodyPush() const;                // 0x340
            virtual int Util_GetIdleMotID();                           // 0x348
            virtual int Util_GetRunMotID();                            // 0x34C
            virtual int Util_GetWalkMotID();                           // 0x350
            virtual int Util_GetLandMotID(long);                       // 0x354
            virtual int Util_GetJumpUpMotID(int);                      // 0x358
            virtual void Util_GetJumpLandMotID(int);                   // 0x35C
            virtual int Util_GetDamageMotID(long, long, long);         // 0x360
            virtual void Util_SetOffenseLeaderFirstState(long);        // 0x364
            virtual void Util_SetDefenseLeaderFirstState();            // 0x368
            virtual void Util_AddCoin(long);                           // 0x36C
            virtual void CreateMdl_();                                 // 0x370
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
            virtual void CtorStateItemShoot();                         // 0x410
            virtual void LoopStateItemShoot();                         // 0x414
            virtual void DtorStateItemShoot();                         // 0x418
            virtual void CtorStateMotPlay();                           // 0x44C
            virtual void LoopStateMotPlay();                           // 0x450
            virtual void DtorStateMotPlay();                           // 0x454
            virtual void CtorStateDizzy();                             // 0x470
            virtual void LoopStateDizzy();                             // 0x474
            virtual void DtorStateDizzy();                             // 0x478
            virtual void CtorStateSliding();                           // 0x47C
            virtual void LoopStateSliding();                           // 0x480
            virtual void DtorStateSliding();                           // 0x484
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateServe();                             // 0x5B4
            virtual void LoopStateServe();                             // 0x5B8
            virtual void DtorStateServe();                             // 0x5BC
            virtual void CtorStateReceive();                           // 0x5C0
            virtual void LoopStateReceive();                           // 0x5C4
            virtual void DtorStateReceive();                           // 0x5C8
            virtual void CtorStateSpike();                             // 0x5CC
            virtual void LoopStateSpike();                             // 0x5D0
            virtual void DtorStateSpike();                             // 0x5D4
            virtual void CtorStateTurn();                              // 0x680
            virtual void LoopStateTurn();                              // 0x684
            virtual void DtorStateTurn();                              // 0x688
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
            virtual int CanbeAirTrick();                               // 0x8E8
            virtual int MethdAirTrick();                               // 0x8EC
            virtual int CanbeItemShoot();                              // 0x94C
            virtual int MethdItemShoot(float);                         // 0x950
            virtual int CanbeSetMoveRevise();                          // 0x954
            virtual int MethdSetMoveRevise(float);                     // 0x958
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
            virtual int CanbeServe(long);                              // 0x998
            virtual int MethdServe(long);                              // 0x99C
            virtual int CanbeReceive(int);                             // 0x9A0
            virtual int CanbeDiveReceive();                            // 0x9A4
            virtual int MethdReceive(int, int);                        // 0x9A8
            virtual int MethdReceive(int, int, float);                 // 0x9AC
            virtual int CanbeSpike(int) const;                         // 0x9B0
            virtual int MethdSpike(int);                               // 0x9B4
            virtual int CanbeAutoSpikeJump() const;                    // 0x9B8
            virtual float GetArtsEventRot();                           // 0xA88
            virtual void DoPlayArtsEvent();                            // 0xA8C
            virtual int MethdEventDoll();                              // 0xAC4
            virtual int CanbeCombination() const;                      // 0xB5C
            virtual int MethdCombination();                            // 0xB60
            virtual int CanbeLanding() const;                          // 0xB64
            virtual int CanbeFall() const;                             // 0xB68
            virtual int MethdFall();                                   // 0xB6C
            virtual int CanbeRebirth() const;                          // 0xB70
            virtual int MethdRebirth();                                // 0xB74
            virtual void CheckCountWork__();                           // 0xC10
            virtual void CalcMoveSpeedRate_(float, long);              // 0xC18
            virtual void Util_SetEffect(SET_EFFECT_TYPE);              // 0xC1C
            virtual void Util_AttachEffect(ATTACH_EFFECT_TYPE, long);  // 0xC20
            virtual int CallbackDribbleTap_();                         // 0xC48
            virtual void CallbackBallCatch_(CBall *);                  // 0xC58
            virtual int CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, bcs::FVEC3 &, bcs::FVEC3 &); // 0xC64
            virtual void CallbackMotTrg_BodyVisible_(long, long);      // 0xC7C
            virtual void SubStateDamageCtorMame_(STATE_WORK_DAMAGE *); // 0xC88
            virtual void SubStateDamageLoopMame_(STATE_WORK_DAMAGE *); // 0xC8C
            virtual void SubStateParalyzeSetEffect_();                 // 0xC94
            virtual void SubStateParalyzeEndEffect_();                 // 0xC98

            // CChrCoreVolley's own virtuals, 0xC9C to the end of the vtable at
            // 0xD88. Return types guessed from the bodies where known,
            // otherwise from CChrCoreBoss's slots of the same name.
            virtual CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *NewCcParam_(long nHeap) const; // 0xC9C
            virtual const CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *GetCcParam_() const; // 0xCA0
            virtual void GetVolleyTargetMarkerDefaultPos(long, const bcs::FVEC3 &, bcs::FVEC3 &, int); // 0xCA4
            virtual void SetVolleyTargetMarkerPosClip(long, bcs::FVEC3 &); // 0xCA8
            virtual void SubStateBlockJumpCollSet_(STATE_WORK_BLOCK_JUMP *); // 0xCAC
            virtual int SubStateBlockIsNormalType_() const;            // 0xCB0
            virtual int SubStateBlockIsJumpType_() const;              // 0xCB4
            virtual int SubStateBlockIsNoFlickType_() const;           // 0xCB8
            virtual void SubStateBlockJumpJumpStart_(STATE_WORK_BLOCK_JUMP *); // 0xCBC
            virtual void SubStateBlockJumpDoJumpMove_(STATE_WORK_BLOCK_JUMP *); // 0xCC0
            virtual int SubStateBlockJumpIsJumpEnd_(STATE_WORK_BLOCK_JUMP *); // 0xCC4
            virtual void SubStateBlockJumpJumpEnd_(STATE_WORK_BLOCK_JUMP *); // 0xCC8
            virtual void SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *); // 0xCCC
            virtual int SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *); // 0xCD0
            virtual int SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *);  // 0xCD4
            virtual void SubStateBlockJumpLandStart_(STATE_WORK_BLOCK_JUMP *); // 0xCD8
            virtual void SubStateBlockJumpColSlide_(STATE_WORK_BLOCK_JUMP *); // 0xCDC
            virtual void SubStateBlockJumpBlockHit_(const CCollDefUnit *); // 0xCE0
            virtual int SubStateBlockIsSuperType_() const;             // 0xCE4
            virtual void SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *); // 0xCE8
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xCEC
            virtual void SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *); // 0xCF0
            virtual void SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *); // 0xCF4
            virtual float SubStateAirTrickSetTrickFrameParam_(STATE_WORK_AIR_TRICK *, long); // 0xCF8
            virtual void SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCFC
            virtual void SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long);  // 0xD00
            virtual float SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD04
            virtual float SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD08
            virtual float SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *); // 0xD0C
            virtual void SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual int SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *); // 0xD14
            virtual void SubStateAirTrickStartTMDisp_(STATE_WORK_AIR_TRICK *); // 0xD18
            virtual int SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD1C
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xD20
            virtual void SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *, CBall *); // 0xD24
            virtual float SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *); // 0xD28
            virtual void SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *); // 0xD2C
            virtual void SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *); // 0xD30
            virtual void SubStateAirTrickReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD34
            virtual void SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xD38
            virtual int SubStateAirTrickIsStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD3C
            virtual void SubStateAirTrickStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD40
            virtual void SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xD44
            virtual int SubStateAirTrickIsEndArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD48
            virtual void SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *); // 0xD4C
            virtual void SubStateAirTrickStartArtsHitback_(STATE_WORK_AIR_TRICK *); // 0xD50
            virtual int SubStateAirTrickIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *); // 0xD54
            virtual void SubStateAirTrickDoLandStart_(STATE_WORK_AIR_TRICK *); // 0xD58
            virtual void SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD5C
            virtual void SubStateAirTrickPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD60
            virtual int SubStateReceiveDoCheckReactFlag_(STATE_WORK_RECEIVE *, CBall *, unsigned long); // 0xD64
            virtual int SubStateReceiveDoCheckDiveReactFlag_(STATE_WORK_RECEIVE *, CBall *, unsigned long); // 0xD68
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD6C
            virtual void SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *); // 0xD70
            virtual int CanbeRunCondition();                           // 0xD74
            virtual int CanbeJumpCondition() const;                    // 0xD78
            virtual int CanbeReceiveCondition() const;                 // 0xD7C
            virtual void SetSmashDmg(int);                             // 0xD80
            virtual int IsNowSmashDmg() const;                         // 0xD84
            virtual int IsNowFalling() const;                          // 0xD88

            // Added for the character units game/volley/CChrCoreVolley<Name>.cpp
            // (in VolleyF; return types and parameter meanings guessed).
            // The rectangle of the court half the character defends that an
            // attack aims at (x left, y right, z front, w back as in FVEC4).
            void Util_GetGuardCourtAttackZoneRect_(bcs::FVEC4 &rRect);
            // The ball can be caught at a point near the character until
            // SetBallCatchDisable_.
            void SetBallCatchDisable_();
            void SetBallCatchEnableWorld_(float fRange, const bcs::FVEC3 &pos1, const bcs::FVEC3 &pos2);
            // Takes the ball in after a receive.
            void ReceiveBall_(long, long nFlag, float, long nType, long, int);
            // The direction the ball is knocked to.
            float GetFlickDir_(const CBall *pBall);
            // Not zero while a star makes the character's receives safe.
            int IsStarConditionReceive_() const;
            void SetBallCatchEnableWorld_(float fRange, float, const bcs::FVEC3 &pos);
            void SetBallCatchEnableWorld_(float fRange, const bcs::FVEC3 &pos);
            // Places the block's collision at rPos (block size, yaw).
            void UpdateCollDefSetParam_(const bcs::FVEC3 &rPos, float fSize, float fRotY);
            float GetBallShotTrgFrame_(long nMotId, float) const;
            CBall *GetTargetBall_() const;
            // The yaw the block faces.
            float GetBlockTargetRot_() const;
            // The rectangle of the court half the character defends.
            void Util_GetGuardCourtRect_(bcs::FVEC4 &rRect);
            void ClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam);
            void SubStateAirTrickDoJumpMove_(STATE_WORK_AIR_TRICK *pWork);

        protected:
            // Not mapped yet; size 0x964.
            unsigned char m_Unk574[0x888 - 0x574];
            // 0x888, 0x88C (names guessed): cleared when a block is hit
            // (CChrCoreVolleyKuroma::SubStateBlockJumpBlockHit_).
            float m_fBlockUnk888;
            float m_fBlockUnk88C;
            // 0x890 (name guessed): two values the receive states leave for
            // CheckCountWork__ (from the work's 0xD4 and 0xD8).
            float m_afCountWork[2];
            unsigned char m_Unk898[0x964 - 0x898];
    };

    // (field names guessed; only what the character units read is mapped)
    struct CChrCoreVolley::STATE_WORK_RECEIVE {
        long m_nStep;                // 0x00
        long m_nSubStep;             // 0x04, counts the motions of a step
        unsigned char m_Unk08[0x8];
        unsigned long m_nFlag;       // 0x10
        float m_fTrgFrame;           // 0x14, a frame of the receive motion
        float m_fTargetRotY;         // 0x18, negative for none
        float m_fRotY;               // 0x1C
        unsigned char m_Unk20[0x8];
        bcs::FVEC3 m_Dir;            // 0x28, the direction the ball is sent (only x and z read)
        unsigned char m_Unk34[0x58 - 0x34];
        long m_nReceiveType;         // 0x58, passed to ReceiveBall_
        long m_nFlickType;           // 0x5C, passed instead when the ball is flicked away
        unsigned char m_Unk60[0xD4 - 0x60];
        float m_afCountWork[2];      // 0xD4, see CChrCoreVolley::m_afCountWork
    };

    // (field names guessed; only what the character units read is mapped)
    struct CChrCoreVolley::STATE_WORK_AIR_TRICK {
        unsigned char m_Unk00[0xC];
        float m_fJumpUnk;            // 0x0C, for CChrCore::m_fJumpUnk224
        bcs::FVEC3 m_TargetPos;      // 0x10, where the trick goes to
        float m_fRotY;               // 0x1C
        float m_fUnk20;              // 0x20
        CC_DATA::CHR_PARAM::MOVE_PARAM m_MoveParam; // 0x24
        CChrCore::DIR_MOVE m_DirMove; // 0x34
        long m_nTrickStep;           // 0x54
        float m_fTrickFrame;         // 0x58, the length of the trick
        float m_fTrickCount;         // 0x5C, frames of it done
        unsigned char m_Unk60[0x4];
        float m_fAnimSpeed;          // 0x64
        long m_nArtsLevel;           // 0x68
        unsigned char m_Unk6C[0x4];
        short m_nAdsorbStep;         // 0x70
        unsigned char m_Unk72[0x2];
        float m_fAdsorbFrame;        // 0x74
        float m_fAdsorbFrameMax;     // 0x78
        bcs::FVEC3 m_AdsorbOfs;      // 0x7C
    };

    // (field names guessed)
    struct CChrCoreVolley::STATE_WORK_BLOCK_JUMP {
        unsigned char m_Unk00[0x4];
        unsigned long m_nFlag;       // 0x04
        float m_fRotY;               // 0x08, negative for none
        unsigned char m_Unk0C[0x38 - 0xC];
        float m_fCollSize;           // 0x38, passed to UpdateCollDefSetParam_ (name guessed)
        float m_fActStartFrame;      // 0x3C
        float m_fActTrgFrame;        // 0x40, negative once done
        bcs::FVEC3 m_EffOfs;         // 0x44, the block effect's offset
    };

    // (field names guessed)
    struct CChrCoreVolley::STATE_WORK_SPIKE {
        long m_nStep;                // 0x00
        unsigned long m_nFlag;       // 0x04
    };

    class CChrCoreVolleyMario : public CChrCoreVolley {
        public:
            CChrCoreVolleyMario();
            virtual ~CChrCoreVolleyMario();
    };

    // Luigi's air trick pulls the ball in with his vacuum
    // (CC_CMN::LUIGI::CcOvacuumForce, a task in the DOL).
    class CChrCoreVolleyLuigi : public CChrCoreVolley {
        public:
            CChrCoreVolleyLuigi();
            virtual ~CChrCoreVolleyLuigi();
            virtual CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateJump();                              // 0x3F4
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void SubStateBlockJumpJumpStart_(STATE_WORK_BLOCK_JUMP *); // 0xCBC
            virtual void SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *); // 0xCF0
            virtual void SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long);  // 0xD00
            virtual void SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual void SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *, CBall *); // 0xD24
            virtual float SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *); // 0xD28
            virtual void SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *); // 0xD2C
            virtual void SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xD38
            virtual int SubStateAirTrickIsStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD3C
            virtual void SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xD44
            virtual void SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *); // 0xD4C
            virtual void SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD5C
            virtual void SubStateAirTrickPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD60
            virtual void SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *); // 0xD70

        protected:
            // (inline helper, name guessed) Deletes the vacuum task, if any.
            void DeleteVacuum_();

            // (names guessed; allocated with 0x970 bytes)
            int m_bVacuum;               // 0x964, the vacuum pulls
            float m_fVacuumRot;          // 0x968, swings the vacuum side to side (radians)
            unsigned long m_hVacuum;     // 0x96C, task id of the vacuum, 0 for none
    };

    class CChrCoreVolleyPeach : public CChrCoreVolley {
        public:
            CChrCoreVolleyPeach();
            virtual ~CChrCoreVolleyPeach();
    };

    class CChrCoreVolleyDaisy : public CChrCoreVolley {
        public:
            CChrCoreVolleyDaisy();
            virtual ~CChrCoreVolleyDaisy();
    };

    class CChrCoreVolleyYoshi : public CChrCoreVolley {
        public:
            CChrCoreVolleyYoshi();
            virtual ~CChrCoreVolleyYoshi();
            virtual CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateJump();                              // 0x3F4
            virtual void DtorStateSpike();                             // 0x5D4
            virtual void SubStateBlockJumpJumpStart_(STATE_WORK_BLOCK_JUMP *); // 0xCBC
            virtual void SubStateBlockJumpLandStart_(STATE_WORK_BLOCK_JUMP *); // 0xCD8
            virtual void SubStateBlockJumpBlockHit_(const CCollDefUnit *); // 0xCE0
            virtual void SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *); // 0xCE8
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xCEC
            virtual void SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *); // 0xCF0
            virtual void SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual void SubStateAirTrickReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD34
            virtual void SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xD44
            virtual void SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *); // 0xD4C
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD6C (not zero when the state is over)
    };

    class CChrCoreVolleyWario : public CChrCoreVolley {
        public:
            CChrCoreVolleyWario();
            virtual ~CChrCoreVolleyWario();
    };

    class CChrCoreVolleyWaluigi : public CChrCoreVolley {
        public:
            CChrCoreVolleyWaluigi();
            virtual ~CChrCoreVolleyWaluigi();

        protected:
            unsigned char m_Unk964[0x994 - 0x964]; // allocated with 0x994 bytes
    };

    class CChrCoreVolleyDonkey : public CChrCoreVolley {
        public:
            CChrCoreVolleyDonkey();
            virtual ~CChrCoreVolleyDonkey();
    };

    class CChrCoreVolleyDiddy : public CChrCoreVolley {
        public:
            CChrCoreVolleyDiddy();
            virtual ~CChrCoreVolleyDiddy();
    };

    // Toad does not dive for a receive: he puts a mushroom (the receiver,
    // CC09_VOLLEY::CcKinopioReceiver) on the ground where the ball comes
    // down. His air trick puts two bouncing mushrooms
    // (CC09_VOLLEY::CcKinopioBoundMushroom) in the other court.
    class CChrCoreVolleyKinopio : public CChrCoreVolley {
        public:
            CChrCoreVolleyKinopio();
            virtual ~CChrCoreVolleyKinopio();
            virtual CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateReceive();                           // 0x5C8
            virtual int SubStateBlockIsNormalType_() const { return 0; } // 0xCB0
            virtual void SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *); // 0xCCC
            virtual int SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *); // 0xCD0
            virtual int SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *);  // 0xCD4
            virtual void SubStateBlockJumpBlockHit_(const CCollDefUnit *); // 0xCE0
            virtual int SubStateBlockIsSuperType_() const { return 1; } // 0xCE4
            virtual void SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *); // 0xCF4
            virtual void SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCFC
            virtual void SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long); // 0xD00
            virtual float SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD04
            virtual float SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD08
            virtual void SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual int SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *); // 0xD14
            virtual int SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD1C
            virtual void SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xD38
            virtual void SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xD44
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD6C (not zero when the state is over)

            // Puts the two mushrooms of the air trick in the other court.
            void CreateBoundMushroom_();
            // Puts down the receiver the ball is taken on.
            void FireReceiver_();

        protected:
            // (inline helpers, names guessed) Not zero while the receiver
            // task still exists; forgets the handle once it is gone.
            int IsReceiverAlive_();
            // Forgets the receiver (the original looks the task up first and
            // does nothing with it).
            void ReleaseReceiver_();

            // 0x964 (name guessed; allocated with 0x968 bytes): the task id
            // of the receiver, 0 for none.
            unsigned long m_hReceiver;
    };

    class CChrCoreVolleyKoopa : public CChrCoreVolley {
        public:
            CChrCoreVolleyKoopa();
            virtual ~CChrCoreVolleyKoopa();

        protected:
            unsigned long m_Unk964; // allocated with 0x968 bytes
    };

    class CChrCoreVolleyKoopaJr : public CChrCoreVolley {
        public:
            CChrCoreVolleyKoopaJr();
            virtual ~CChrCoreVolleyKoopaJr();
    };

    class CChrCoreVolleyMoogle : public CChrCoreVolley {
        public:
            CChrCoreVolleyMoogle();
            virtual ~CChrCoreVolleyMoogle();
    };

    class CChrCoreVolleySaboten : public CChrCoreVolley {
        public:
            CChrCoreVolleySaboten();
            virtual ~CChrCoreVolleySaboten();
    };

    class CChrCoreVolleyNinja : public CChrCoreVolley {
        public:
            CChrCoreVolleyNinja();
            virtual ~CChrCoreVolleyNinja();

        protected:
            unsigned char m_Unk964[0x970 - 0x964]; // allocated with 0x970 bytes
    };

    class CChrCoreVolleySiroma : public CChrCoreVolley {
        public:
            CChrCoreVolleySiroma();
            virtual ~CChrCoreVolleySiroma();

        protected:
            unsigned long m_Unk964; // allocated with 0x968 bytes
    };

    class CChrCoreVolleySlime : public CChrCoreVolley {
        public:
            CChrCoreVolleySlime();
            virtual ~CChrCoreVolleySlime();

        protected:
            unsigned char m_Unk964[0x96C - 0x964]; // allocated with 0x96C bytes
    };

    // The Black Mage: his diving receive throws a spell the ball is taken
    // on (CC17_VOLLEY::CcKuromaReceiver), his air trick locks on to the
    // other team and strikes them (CC17_VOLLEY::CcKuromaMagicMgr).
    class CChrCoreVolleyKuroma : public CChrCoreVolley {
        public:
            CChrCoreVolleyKuroma();
            virtual ~CChrCoreVolleyKuroma();
            virtual CHR_CORE_VOLLEY::CC_PARAM_VOLLEY *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void DtorStateReceive();                           // 0x5C8
            virtual void SubStateBlockJumpCollSet_(STATE_WORK_BLOCK_JUMP *); // 0xCAC
            virtual int SubStateBlockIsNormalType_() const { return 0; } // 0xCB0
            virtual int SubStateBlockIsJumpType_() const { return 0; } // 0xCB4
            virtual int SubStateBlockIsNoFlickType_() const { return 1; } // 0xCB8
            virtual void SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *); // 0xCCC
            virtual int SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *); // 0xCD0
            virtual int SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *);  // 0xCD4
            virtual void SubStateBlockJumpBlockHit_(const CCollDefUnit *); // 0xCE0
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xCEC
            virtual void SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCFC
            virtual void SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long); // 0xD00
            virtual float SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD04
            virtual float SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD08
            virtual void SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual int SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD1C
            virtual void SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xD38
            virtual void SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xD44
            virtual void SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD5C
            virtual void SubStateAirTrickPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD60
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD6C (not zero when the state is over)

            // Throws the receiver towards where the ball comes down.
            void FireReceiver_();

        protected:
            // (inline helpers, names guessed) Not zero while the receiver
            // task still exists; forgets the handle once it is gone.
            int IsReceiverAlive_();
            // Delete the task and forget it.
            void ReleaseMagic_();
            void ReleaseReceiver_();
            // The same, with a vanishing effect when it is still in flight.
            void VanishReceiver_();

            // (names guessed; allocated with 0x96C bytes) Task ids, 0 for
            // none.
            unsigned long m_hMagic;    // 0x964, the air trick's CcKuromaMagicMgr
            unsigned long m_hReceiver; // 0x968
    };

    class CChrCoreVolleyMii00 : public CChrCoreVolley {
        public:
            CChrCoreVolleyMii00();
            virtual ~CChrCoreVolleyMii00();
    };

    class CChrCoreVolleyMii01 : public CChrCoreVolley {
        public:
            CChrCoreVolleyMii01();
            virtual ~CChrCoreVolleyMii01();
    };
}

#endif /* !MSW_CCHRCOREVOLLEY_HPP_ */
