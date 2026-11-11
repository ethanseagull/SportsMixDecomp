#ifndef MSW_CCHRCOREBOSS_HPP_
#define MSW_CCHRCOREBOSS_HPP_

#include "game/msw/CChrCore.hpp"
#include "game/msw/CC_PARAM_BOSS.hpp"

namespace msw {

    namespace CC09_BOSS {
        class CcKinopioReceiver;
    }
    class CNpcBase;

    // The boss-mode character core, and one class per character.
    //
    // INCOMPLETE: the virtual functions are all declared (the 872 slots of
    // __vt__Q23msw12CChrCoreBoss and of each character's vtable, read from
    // BossF), but few members are mapped and CChrCore is incomplete too (see
    // there). Each destructor is declared first so that it decides where the
    // vtable goes: the character units (game/boss/CChrCoreBoss<Name>.cpp)
    // define it and so emit the vtable.
    //
    // NewCcParam_ is virtual (slot 807 of __vt__Q23msw12CChrCoreBoss). It
    // allocates the character's parameter block for the current game level
    // with operator new(size, nHeap). Its return type is a guess.
    class CChrCoreBoss : public CChrCore {
        public:
            // The work of the receive, spike and air trick states, passed to
            // the SubState* virtuals (contents mapped only where used).
            struct STATE_WORK_RECEIVE;
            struct STATE_WORK_SPIKE;
            struct STATE_WORK_AIR_TRICK;
            // (added for game/boss/unit_636D4.cpp, which defines them) The
            // work of the side step and of the jump shot states.
            struct STATE_WORK_SIDESTEP;
            struct STATE_WORK_JUMP_SHOOT;
            // (added for game/boss/CStickObj.cpp, which defines it) The
            // work of the shoot stance state.
            struct STATE_WORK_SHOOT_STANCE;
            // (added for game/boss/unit_5FF80.cpp, which defines it) The
            // work of the dunk state.
            struct STATE_WORK_DUNK;

            CChrCoreBoss();
            virtual ~CChrCoreBoss();

            // Overrides of CChrCore's virtuals, in vtable order (the slots of
            // __vt__Q23msw12CChrCoreBoss that name CChrCoreBoss; return types
            // as guessed in CChrCore).
            virtual long GetQueueWorkSize() const;                     // 0x10
            virtual long GetMainOrderValue(long);                      // 0x18
            virtual void *GetHoldBall() const;                        // 0x1C
            virtual void SetConditionEffect_(long, unsigned long);     // 0x28
            virtual int IsHoldBall() const;                            // 0x38
            virtual int IsAirTrickSwingEnd() const;                    // 0x4C
            virtual int IsAirTrickFall() const;                        // 0x50
            virtual float GetVolleySpikeAdsorbRangeXZ(int) const;      // 0xC0
            virtual float GetVolleySpikeAdsorbRangeY(int) const;       // 0xC4
            virtual void GetVolleySpikeOffsetPos(long, bcs::FVEC3 &) const; // 0xF8
            virtual int IsVsBossDead() const;                          // 0x2E4
            virtual long GetVsBossMaxHP() const;                       // 0x2E8
            virtual long GetVsBossNowHP() const;                       // 0x2EC
            virtual float GetVsBossHPRate() const;                     // 0x2F0
            virtual float GetCollSize_BodyDamage() const;              // 0x33C
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
            virtual void CtorStateDizzy();                             // 0x470
            virtual void LoopStateDizzy();                             // 0x474
            virtual void DtorStateDizzy();                             // 0x478
            virtual void CtorStateSliding();                           // 0x47C
            virtual void LoopStateSliding();                           // 0x480
            virtual void DtorStateSliding();                           // 0x484
            virtual void CtorStatePass();                              // 0x4AC
            virtual void LoopStatePass();                              // 0x4B0
            virtual void DtorStatePass();                              // 0x4B4
            virtual void CtorStateJumpShoot();                         // 0x548
            virtual void LoopStateJumpShoot();                         // 0x54C
            virtual void DtorStateJumpShoot();                         // 0x550
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateDunk();                              // 0x590
            virtual void LoopStateDunk();                              // 0x594
            virtual void DtorStateDunk();                              // 0x598
            virtual void CtorStateReceive();                           // 0x5C0
            virtual void LoopStateReceive();                           // 0x5C4
            virtual void DtorStateReceive();                           // 0x5C8
            virtual void CtorStateSpike();                             // 0x5CC
            virtual void LoopStateSpike();                             // 0x5D0
            virtual void DtorStateSpike();                             // 0x5D4
            virtual void CtorStateTurn();                              // 0x680
            virtual void LoopStateTurn();                              // 0x684
            virtual void DtorStateTurn();                              // 0x688
            virtual void CtorStateShootStance();                       // 0x6EC
            virtual void LoopStateShootStance();                       // 0x6F0
            virtual void DtorStateShootStance();                       // 0x6F4
            virtual int CanbeRun();                                    // 0x7E8
            virtual int MethdRun(float);                               // 0x7EC
            virtual int MethdRun(float, float);                        // 0x7F0
            virtual int CanbeSliding();                                // 0x808
            virtual int MethdSliding(float);                           // 0x80C
            virtual int CanbeSizeChange(long);                         // 0x810
            virtual int MethdSizeChange(long, float);                  // 0x814
            virtual int MethdChgStarState();                           // 0x818
            virtual int CanbeChgStarState();                           // 0x81C
            virtual int CanbeSideStep(float, float);                   // 0x840
            virtual int MethdSideStep(float, float);                   // 0x844
            virtual int CanbeShoot();                                  // 0x8B4
            virtual int MethdShoot(float);                             // 0x8B8
            virtual int CanbeAirTrick();                               // 0x8E8
            virtual int MethdAirTrick();                               // 0x8EC
            virtual int CanbeAirTrickSub();                            // 0x8F0
            virtual int MethdAirTrickSub(long, long);                  // 0x8F4
            virtual int CanbeDunk();                                   // 0x8F8
            virtual int MethdDunk();                                   // 0x8FC
            virtual int CanbePass(CChrCore *);                         // 0x908
            virtual int MethdPass(CChrCore *, int);                    // 0x910
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
            virtual int CanbeReceive(int);                             // 0x9A0
            virtual int CanbeDiveReceive();                            // 0x9A4
            virtual int MethdReceive(int, int);                        // 0x9A8
            virtual int MethdReceive(int, int, float);                 // 0x9AC
            virtual int CanbeSpike(int) const;                         // 0x9B0
            virtual int MethdSpike(int);                               // 0x9B4
            virtual int CanbeShootStance();                            // 0xA28
            virtual int MethdShootStance(float, int);                  // 0xA2C
            virtual float GetArtsEventRot();                           // 0xA88
            virtual void DoPlayArtsEvent();                            // 0xA8C
            virtual int MethdEventDoll();                              // 0xAC4
            virtual void CheckCountWork__();                           // 0xC10
            virtual void CalcMoveSpeedRate_(float, long);              // 0xC18
            virtual void Util_SetEffect(SET_EFFECT_TYPE);              // 0xC1C
            virtual void Util_AttachEffect(ATTACH_EFFECT_TYPE, long);  // 0xC20
            virtual int CallbackDribbleTap_();                         // 0xC48
            virtual void CallbackBallCatch_(CBall *);                  // 0xC58
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long); // 0xC5C
            virtual int CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, bcs::FVEC3 &, bcs::FVEC3 &); // 0xC64
            virtual void CallbackMotTrg_BodyVisible_(long, long);      // 0xC7C
            virtual void SubStateDamageCtorMame_(STATE_WORK_DAMAGE *); // 0xC88
            virtual void SubStateDamageLoopMame_(STATE_WORK_DAMAGE *); // 0xC8C

            // CChrCoreBoss's own virtuals, 0xC9C to the end of the vtable at
            // 0xD9C. Return types guessed from the bodies, as in CChrCore.
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const; // 0xC9C
            virtual const CHR_CORE_BOSS::CC_PARAM_BOSS *GetCcParam_() const;                        // 0xCA0
            virtual int IsBossRightBallTouch(long) const;              // 0xCA4
            virtual int IsBossServeMode() const;                       // 0xCA8
            virtual int IsBossTargetMarkerActive() const;              // 0xCAC
            virtual long GetBossTargetMarkerNum() const;               // 0xCB0
            virtual void GetBossTargetMarkerDefaultPos(long, bcs::FVEC3 &, int); // 0xCB4
            virtual long GetBossTargetMarkerPos(bcs::FVEC3 &, long);   // 0xCB8
            virtual long GetBossTargetMarkerMoved(bcs::FVEC3 &);       // 0xCBC
            virtual int MoveBossTargetMarkerPos(float);                // 0xCC0
            virtual int CanbeMoveBossTargetMarkerPos();                // 0xCC4
            virtual void GetBossTargetMarkerDefaultPos(long, const bcs::FVEC3 &, bcs::FVEC3 &, int); // 0xCC8
            virtual void SetBossTargetMarkerPosClip(long, bcs::FVEC3 &); // 0xCCC
            virtual void GetBossBallShotCollPos(long, bcs::FVEC3 &, float &) const; // 0xCD0
            virtual CBall *GetBossTargetBall() const;                  // 0xCD4
            virtual int IsBossSpikeJumpStart() const;                  // 0xCD8
            virtual int IsBossSuperSpikeJump() const;                  // 0xCDC
            virtual int IsBossSpikeSwingEnd() const;                   // 0xCE0
            virtual int IsBossSpikeFall() const;                       // 0xCE4
            virtual int IsBossAirTrickSwingEnd() const;                // 0xCE8
            virtual int IsBossAirTrickFall() const;                    // 0xCEC
            virtual int IsBossBlockJumpStart() const;                  // 0xCF0
            virtual int IsBossBlockFall() const;                       // 0xCF4
            virtual int IsBossBlockSuccess() const;                    // 0xCF8
            virtual int IsBossDispSpikeJumpMarker() const;             // 0xCFC
            virtual int IsBossArtsKoopaShotFire() const;               // 0xD00
            virtual int IsBossArtsKoopaFireEnd() const;                // 0xD04
            virtual int IsBossSuperSpikeCurveType() const;             // 0xD08
            virtual float GetBossSuperSpikeCurveRate() const;          // 0xD0C
            virtual float GetBossSuperSpikeCurveMaxDist() const;       // 0xD10
            virtual float GetBossSuperSpikeCurveMinDist() const;       // 0xD14
            virtual float GetBossBlockJumpDelayFrame() const;          // 0xD18
            virtual int IsBossEnableDiveReceive() const;               // 0xD1C
            virtual void SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *); // 0xD20
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xD24
            virtual void SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *); // 0xD28
            virtual int SubStateReceiveDoCheckReactFlag_(STATE_WORK_RECEIVE *, CBall *, unsigned long); // 0xD2C
            virtual int SubStateReceiveDoCheckDiveReactFlag_(STATE_WORK_RECEIVE *, CBall *, unsigned long); // 0xD30
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD34 (not zero when the state is over)
            virtual void SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *); // 0xD38
            virtual void SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *); // 0xD3C
            virtual void SubStateAirTrickSetTrickFrameParam_(STATE_WORK_AIR_TRICK *, long); // 0xD40
            virtual void SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xD44
            virtual void SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long); // 0xD48
            virtual float SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD4C
            virtual float SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long); // 0xD50
            virtual float SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *); // 0xD54
            virtual void SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xD58
            virtual int SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD5C
            virtual void SubStateAirTrickStartBallAdsorb_(STATE_WORK_AIR_TRICK *, CBall *); // 0xD60
            virtual float SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *); // 0xD64
            virtual void SubStateAirTrickDoBallAdsorb_(STATE_WORK_AIR_TRICK *); // 0xD68
            virtual void SubStateAirTrickTrgBallAdsorbEnd_(STATE_WORK_AIR_TRICK *); // 0xD6C
            virtual void SubStateAirTrickReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD70
            virtual int SubStateAirTrickIsStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD74
            virtual void SubStateAirTrickStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD78
            virtual void SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xD7C
            virtual int SubStateAirTrickIsEndArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD80
            virtual void SubStateAirTrickShootBall_(STATE_WORK_AIR_TRICK *); // 0xD84
            virtual void SubStateAirTrickStartArtsHitback_(STATE_WORK_AIR_TRICK *); // 0xD88
            virtual int SubStateAirTrickIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *); // 0xD8C
            virtual void SubStateAirTrickDoLandStart_(STATE_WORK_AIR_TRICK *); // 0xD90
            virtual int SubStateAirTrickIsOver_(STATE_WORK_AIR_TRICK *); // 0xD94
            virtual int CanbeRunCondition();                           // 0xD98
            virtual int CanbeReceiveCondition() const;                 // 0xD9C

            // Added for the character units game/boss/CChrCoreBoss<Name>.cpp
            // (in game/boss; return types and parameter meanings guessed).
            // The ball can be caught at a point near the character until
            // SetBallCatchDisable_.
            void SetBallCatchDisable_();
            void SetBallCatchEnableWorld_(float fRange, const bcs::FVEC3 &pos1, const bcs::FVEC3 &pos2);
            void SetBallCatchEnableWorld_(float fRange, float, const bcs::FVEC3 &pos);
            void SetBallCatchEnableWorld_(float fRange, const bcs::FVEC3 &pos);
            void SetBallCatchEnableRoot_(float fRange, float, const bcs::FVEC3 &pos);
            // Takes the ball in after a receive.
            void ReceiveBall_(long, long nFlag, float, long nType, long);
            // The direction the ball is knocked to.
            float GetFlickDir_(const CBall *pBall);
            float GetReceiveBaseDir_();
            // The receive pose (an index) for a direction.
            long GetReceivePoseDir_(float fDir);
            // Starts the receive state; returns its work.
            STATE_WORK_RECEIVE *SetStateReceive_(CBall *pBall, long, long nPose);
            float GetBallShotTrgFrame_(long, float) const;
            // (added for game/boss/unit_636D4.cpp; as in CChrCoreVolley) The
            // yaw to the spike's target, whose position goes to pPos when
            // not null.
            float GetSpikeTargetPosRot_(bcs::FVEC3 *pPos) const;
            // (added for game/boss/unit_636D4.cpp; meaning of the second
            // float not known) Adds fRate of a full gauge to the arts gauge.
            void AddArtsGauge_(float fRate, float);
            CBall *GetTargetBall_() const;
            // Added for game/boss/CChrCoreBossMethod.cpp (all but the last
            // two are in other units; return types and meanings guessed).
            // Not zero when the shoot stance may be left for another action.
            int SubStateShootStanceCanbeCancel_() const;
            // (added for game/boss/CStickObj.cpp) Lets the shoot stance's
            // stick fade out over fFrame frames, or deletes it at once when
            // fFrame is not positive, and forgets it.
            void StickObjDestroy_(float fFrame);
            // How a receive of the target ball would be made (1: diving,
            // 4: none possible). pWork may be null.
            long SubStateReceiveChoiceProcType_(STATE_WORK_RECEIVE *pWork);
            int IsChargeBall_(const CBall *pBall) const;
            // Drops the held ball towards fRotY.
            void DropBall_(float fRotY);
            void SetServeMode_(int bServe);
            // The yaw to the point a dunk aims at, which goes to pPos.
            float GetDunkTargetPosRot_(bcs::FVEC3 *pPos) const;
            // Sets the receive state's work up for a ball shot by a special
            // move (the character endures it).
            void SetReceiveEndureWork_(CBall *pBall, STATE_WORK_RECEIVE *pWork);
            // Not zero when the stocked item may be used now.
            int CanbeUseItem_(CBall *pBall);
            // Added for game/boss/unit_5FF80.cpp (the dunk and air trick
            // states; names from the symbols, meanings guessed). The boss,
            // null when there is none.
            const CNpcBase *Util_GetBoss_() const;
            // Sets rRot up to turn to fRotY over fFrame frames.
            void CalcRotParam_(ROT_LERP1 &rRot, float fRotY, float fFrame);
            void ClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam);
            // The dunk: the point aimed at (rTarget, rBase) and the point
            // the character homes in on (rMid, its y the range), fFrame
            // frames into the jump.
            void GetAirTrickTargetPos_(bcs::FVEC3 &rTarget, bcs::FVEC3 &rBase, bcs::FVEC3 &rMid, float fFrame) const;
            void CtorStateDunk_(STATE_WORK_DUNK *pWork);
            void SubStateDunkCtor_(STATE_WORK_DUNK *pWork, float fFrame, long nType, float, long nMot);
            // (inline in the original) The dunk's loop, and one step of its
            // run-up towards the target.
            void LoopStateDunk_(STATE_WORK_DUNK *pWork);
            void DunkHomingMove_();
            void SubStateDunkCmnLandInit_(STATE_WORK_DUNK *pWork);
            void SubStateDunkCmnMove_(STATE_WORK_DUNK *pWork);
            void SubStateDunkCmnDunk_(STATE_WORK_DUNK *pWork);
            void SubStateDunkCmnLand_(STATE_WORK_DUNK *pWork);
            void Util_DunkStart_(long nMot, long, float, long);
            // (inline in the original) One step of the air trick's jump and
            // of its move; returns what Util_DoJumpMoveEX returned.
            int SubStateAirTrickDoJumpMove_(STATE_WORK_AIR_TRICK *pWork);

        protected:
            // Not mapped yet. The size, 0x818, is what CChrManBoss::CreateObj_
            // allocates for most characters.
            unsigned char m_Unk574[0x6FC - 0x574];
            // 0x6FC, 0x708 (names guessed; GetVolleySpikeOffsetPos reads
            // them): where the ball is hit in a spike, from the character,
            // for a normal and for a charged ball.
            bcs::FVEC3 m_SpikeOfs;
            bcs::FVEC3 m_SpikeOfsCharge;
            unsigned char m_Unk714[0x734 - 0x714];
            // 0x734 (name and count guessed: the constructor clears four
            // arrays of eight floats at 0x714, 0x734, 0x754 and 0x774). The
            // height above the character a receive in each pose takes the
            // ball at (CChrCoreBossSiroma adds GetPosY to it).
            float m_afReceiveHeight[8];
            unsigned char m_Unk754[0x808 - 0x754];
            // 0x808 (name guessed): the task id of the shoot stance's stick
            // (CHR_CORE_BOSS::CStickObj), 0 when there is none.
            unsigned long m_hStickObj;
            // 0x80C, 0x810 (names guessed): the character's full and
            // remaining life in the boss battle (GetVsBossHPRate).
            float m_fHPMax;
            float m_fHP;
            // 0x814 (name guessed): set to 1200 frames when the life runs out.
            float m_fDownTime;
    };

    // (field names guessed; only what the character units read is mapped)
    struct CChrCoreBoss::STATE_WORK_RECEIVE {
        long m_nStep;                // 0x00
        long m_nSubStep;             // 0x04, counts the motions of a step
        // 0x08 (name guessed): the kind of receive (SetStateReceive_'s
        // second argument; CanbeRun lets a receive of kind 4 be left).
        long m_nKind;
        long m_nPose;                // 0x0C, the receive pose (see GetReceivePoseDir_)
        unsigned long m_nFlag;       // 0x10
        float m_fTrgFrame;           // 0x14, a frame of the receive motion (name guessed)
        float m_fTargetRotY;         // 0x18, negative for none
        float m_fRotY;               // 0x1C
        unsigned char m_Unk20[0x8];
        bcs::FVEC3 m_Dir;            // 0x28, the direction the ball is sent (only x and z read)
        unsigned char m_Unk34[0x4];
        float m_fUnk38;              // 0x38 (meaning not known)
        float m_fUnk3C;              // 0x3C (meaning not known)
        float m_fUnk40;              // 0x40, from CChrCore::m_fUnk268 (meaning not known)
        unsigned char m_Unk44[0xC];
        float m_fUnk50;              // 0x50 (meaning not known)
        long m_nReceiveType;         // 0x54, passed to ReceiveBall_
        long m_nFlickType;           // 0x58, passed instead when the ball is flicked away
        unsigned char m_Unk5C[0xC4 - 0x5C];
        bcs::FVEC3 m_BallPos;        // 0xC4, where the ball will be taken
    };

    // (field names guessed)
    struct CChrCoreBoss::STATE_WORK_SPIKE {
        long m_nStep;                // 0x00
        unsigned long m_nFlag;       // 0x04
    };

    class CChrCoreBossMario : public CChrCoreBoss {
        public:
            CChrCoreBossMario();
            virtual ~CChrCoreBossMario();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xD24
    };

    class CChrCoreBossLuigi : public CChrCoreBoss {
        public:
            CChrCoreBossLuigi();
            virtual ~CChrCoreBossLuigi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *); // 0xD28
            virtual void SubStateReceiveDoDiveReceiveDiveStart_(STATE_WORK_RECEIVE *); // 0xD38
    };

    class CChrCoreBossPeach : public CChrCoreBoss {
        public:
            CChrCoreBossPeach();
            virtual ~CChrCoreBossPeach();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDaisy : public CChrCoreBoss {
        public:
            CChrCoreBossDaisy();
            virtual ~CChrCoreBossDaisy();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
    };

    class CChrCoreBossYoshi : public CChrCoreBoss {
        public:
            CChrCoreBossYoshi();
            virtual ~CChrCoreBossYoshi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD34 (not zero when the state is over)
    };

    class CChrCoreBossWario : public CChrCoreBoss {
        public:
            CChrCoreBossWario();
            virtual ~CChrCoreBossWario();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossWaluigi : public CChrCoreBoss {
        public:
            CChrCoreBossWaluigi();
            virtual ~CChrCoreBossWaluigi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
    };

    class CChrCoreBossDonkey : public CChrCoreBoss {
        public:
            CChrCoreBossDonkey();
            virtual ~CChrCoreBossDonkey();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDiddy : public CChrCoreBoss {
        public:
            CChrCoreBossDiddy();
            virtual ~CChrCoreBossDiddy();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKinopio : public CChrCoreBoss {
        public:
            CChrCoreBossKinopio();
            virtual ~CChrCoreBossKinopio();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateReceive();                           // 0x5C8
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD34 (not zero when the state is over)

            // Puts down the receiver (CC09_BOSS::CcKinopioReceiver) the ball
            // is taken on in the diving receive.
            void FireReceiver_();

        protected:
            // (inline helpers, names guessed) Not zero while the receiver
            // task still exists; forgets the handle once it is gone.
            int IsReceiverAlive_();
            // Forgets the receiver (the original looks the task up first and
            // does nothing with it).
            void ReleaseReceiver_();

            // 0x818 (name guessed; allocated with 0x81C bytes): the task id
            // of the receiver, 0 for none.
            unsigned long m_hReceiver;
    };

    class CChrCoreBossKoopa : public CChrCoreBoss {
        public:
            CChrCoreBossKoopa();
            virtual ~CChrCoreBossKoopa();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xD24
    };

    class CChrCoreBossKoopaJr : public CChrCoreBoss {
        public:
            CChrCoreBossKoopaJr();
            virtual ~CChrCoreBossKoopaJr();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xD24
    };

    class CChrCoreBossMoogle : public CChrCoreBoss {
        public:
            CChrCoreBossMoogle();
            virtual ~CChrCoreBossMoogle();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSaboten : public CChrCoreBoss {
        public:
            CChrCoreBossSaboten();
            virtual ~CChrCoreBossSaboten();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossNinja : public CChrCoreBoss {
        public:
            CChrCoreBossNinja();
            virtual ~CChrCoreBossNinja();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void CallbackMotTrg_BodyVisible_(long, long);      // 0xC7C
    };

    class CChrCoreBossSiroma : public CChrCoreBoss {
        public:
            CChrCoreBossSiroma();
            virtual ~CChrCoreBossSiroma();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateSpike();                             // 0x5D4
            virtual void SubStateSpikeDoSmashSpikeJumpEffect_(STATE_WORK_SPIKE *); // 0xD20
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xD24
            virtual void SubStateSpikeDoLandStart_(STATE_WORK_SPIKE *); // 0xD28
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD34 (not zero when the state is over)
    };

    class CChrCoreBossSlime : public CChrCoreBoss {
        public:
            CChrCoreBossSlime();
            virtual ~CChrCoreBossSlime();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384

        protected:
            // 0x818 (name guessed; allocated with 0x81C bytes): 1 or 2 for
            // colour variants 1 and 2, otherwise 0 (Reset_).
            long m_nColorType;
    };

    class CChrCoreBossKuroma : public CChrCoreBoss {
        public:
            CChrCoreBossKuroma();
            virtual ~CChrCoreBossKuroma();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
            virtual void Reset_();                                     // 0x384
            virtual void DtorStateReceive();                           // 0x5C8
            virtual void SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *); // 0xD24
            virtual int SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *); // 0xD34 (not zero when the state is over)

            // Throws the receiver (CC17_BOSS::CcKuromaReceiver, a spell the
            // ball is taken on in the diving receive) towards where the
            // ball comes down.
            void FireReceiver_();

        protected:
            // (inline helpers, names guessed) Not zero while the receiver
            // task still exists; forgets the handle once it is gone.
            int IsReceiverAlive_();
            // Deletes the receiver and forgets it.
            void ReleaseReceiver_();
            // The same, with a vanishing effect when it is still in flight.
            void VanishReceiver_();

            // 0x818 (name guessed; allocated with 0x81C bytes): the task id
            // of the receiver, 0 for none.
            unsigned long m_hReceiver;
    };

    class CChrCoreBossMii00 : public CChrCoreBoss {
        public:
            CChrCoreBossMii00();
            virtual ~CChrCoreBossMii00();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMii01 : public CChrCoreBoss {
        public:
            CChrCoreBossMii01();
            virtual ~CChrCoreBossMii01();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };
}

#endif /* !MSW_CCHRCOREBOSS_HPP_ */
