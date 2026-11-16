#ifndef MSW_CNPCBEHEMOTHBOSS_HPP_
#define MSW_CNPCBEHEMOTHBOSS_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CNpcBase.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"

namespace bcs {
    class CAnmMatClr;
}

namespace msw {

    class CBall;
    class CChrMan;
    class CCollAtkUnit;
    class CCollBodyUnit;

    // The Behemoth, the boss of the boss game (BossF), size 0x36C (what
    // CChrManBoss::Create_ allocates). Its actions run on
    // bcs::CFsmQueue<CNpcBehemothBoss> at 0x188 (CComDrvBoss reads the state
    // at the head of the queue: state 9 sends the characters away from it).
    // BossF game/boss/CNpcBehemothFiga.cpp holds the constructor, the
    // destructor, the vtable and the first part of the functions;
    // game/boss/CNpcBehemothBoss.cpp the rest (most of the states).
    //
    // Member names are guesses; the members are mapped only where the
    // functions of CNpcBehemothFiga.cpp use them.
    class CNpcBehemothBoss : public CNpcBase, public bcs::CFsmQueue<CNpcBehemothBoss> {
        public:
            // The states (InitFsm_ registers them in this order).
            enum {
                STATE_DEFAULT,
                STATE_SLEEP,
                STATE_NEUTRAL,
                STATE_MOVE,
                STATE_START_WAR_CRY,
                STATE_ATTACK_FANG,
                STATE_ATTACK_CLAW,
                STATE_ATTACK_HORN,
                STATE_ATTACK_TAIL,
                STATE_ATTACK_METEOR,
                STATE_ATTACK_FIGA,
                STATE_ATTACK_THUNDERGA,
                STATE_ATTACK_JUMP,
                STATE_DAMAGE,
                STATE_KO,
                STATE_DOLL,
                STATE_ESCAPE_JUMP,
                STATE_RUSH,
                STATE_ATTACK_SMASH,
                STATE_ATTACK_TURN,
                STATE_THINK,
                STATE_DO_NOTHING,
                STATE_ATTACK_HOWLING,
                STATE_WAIT_DUNK,
                STATE_WANDERING,
                STATE_ANGER,
                STATE_ARTS_EVENT,
                STATE_NUM
            };

            // The model nodes GetVNodePos reads (CreateMdlObj_ looks them up
            // by the names in the file's table).
            enum {
                VNODE_HIP = 0,    // kosi_jnt
                VNODE_HEAD = 3,   // atama_jnt
                VNODE_HAND_L = 4, // L_te_jnt
                VNODE_FOOT = 4,   // four feet from here
                VNODE_HAND_R = 5, // R_te_jnt
                VNODE_HORN = 8,   // eff_horn
                VNODE_MAGIC = 9,  // eff_magic_point
                VNODE_TAIL = 13,  // sippo_00_jnt, five of them
                VNODE_NUM = 18
            };

            // The colour animations of the model (SetColorAnim_).
            enum {
                ANMCLR_NUM = 11
            };

            // (parameter names guessed) CChrManBoss::Create_ passes itself,
            // the start position and rotation, and party game parameter 0
            // (the level: 2 and up is the hard Behemoth).
            CNpcBehemothBoss(CChrMan *pMan, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nLevel);
            virtual ~CNpcBehemothBoss();

            virtual void Create();
            virtual long GetNpcType() const { return 0; }
            virtual void Reset();
            virtual long SendMessage(long, long, long, long, long);
            virtual long GetThinkLevel() const { return m_nLevel; }
            virtual void AnimPlay(long nMot, int bLoop, float fBlend, float fFrame);
            virtual void AnimStop();
            virtual void AnimSetSpeed(float fSpeed);
            virtual void AnimSetFrame(float fFrame);
            virtual float GetBodyCollInfo(bcs::FVEC3 &rTop, bcs::FVEC3 &rBottom);
            virtual void MainFunc();
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg);
            virtual unsigned long CallbackBodyDamage_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                      const bcs::FVEC3 &rPos);
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual void CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall);

            // Its own virtual functions after the state queue's part of the
            // vtable (from 0x80). Return types of the HP getters read from
            // CComDrvBoss::ThkMain. GetQueueWorkSize is inline: InitFsm_
            // calls it first, and its weak copy follows InitFsm_.
            virtual unsigned long GetQueueWorkSize() const { return 0x3C; } // 0x80
            virtual float GetHP() const { return m_fHP; }                  // 0x84
            virtual float GetMaxHP() const { return m_fMaxHP; }            // 0x88
            virtual float AnimSearchTrgFrame(long nMot, unsigned long nTrg, float fFrame) const; // 0x8C
            virtual void CreateMdlObj_();                                  // 0x90
            virtual void InitFsm_();                                       // 0x94
            virtual void SetColorAnim_(long nAnm);                         // 0x98
            virtual float GetParamScale() const;                           // 0x9C
            virtual void CallbackBallReflect_(CCollBodyUnit *pBody, CBall *pBall); // 0xA0

            // The state hooks (0xA4 on). The empty ones are inline: their
            // weak copies follow the wrappers below.
            virtual void CtorStateDefault();
            virtual void LoopStateDefault() {}
            virtual void DtorStateDefault() {}
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateNeutral();
            virtual void LoopStateNeutral();
            virtual void DtorStateNeutral() {}
            virtual void CtorStateMove();
            virtual void LoopStateMove();
            virtual void DtorStateMove();
            virtual void CtorStateStartWarCry();
            virtual void LoopStateStartWarCry();
            virtual void DtorStateStartWarCry();
            virtual void CtorStateAttackFang();
            virtual void LoopStateAttackFang();
            virtual void DtorStateAttackFang();
            virtual void CtorStateAttackClaw();
            virtual void LoopStateAttackClaw();
            virtual void DtorStateAttackClaw();
            virtual void CtorStateAttackHorn();
            virtual void LoopStateAttackHorn();
            virtual void DtorStateAttackHorn();
            virtual void CtorStateAttackTail();
            virtual void LoopStateAttackTail();
            virtual void DtorStateAttackTail();
            virtual void CtorStateAttackMeteor();
            virtual void LoopStateAttackMeteor();
            virtual void DtorStateAttackMeteor();
            virtual void CtorStateAttackFiga();
            virtual void LoopStateAttackFiga();
            virtual void DtorStateAttackFiga();
            virtual void CtorStateAttackThunderga();
            virtual void LoopStateAttackThunderga();
            virtual void DtorStateAttackThunderga();
            virtual void CtorStateAttackJump();
            virtual void LoopStateAttackJump();
            virtual void DtorStateAttackJump();
            virtual void CtorStateDamage();
            virtual void LoopStateDamage();
            virtual void DtorStateDamage();
            virtual void CtorStateKO();
            virtual void LoopStateKO();
            virtual void DtorStateKO();
            virtual void CtorStateDoll();
            virtual void LoopStateDoll();
            virtual void DtorStateDoll();
            virtual void CtorStateEscapeJump();
            virtual void LoopStateEscapeJump();
            virtual void DtorStateEscapeJump();
            virtual void CtorStateRush();
            virtual void LoopStateRush();
            virtual void DtorStateRush();
            virtual void CtorStateAttackSmash();
            virtual void LoopStateAttackSmash();
            virtual void DtorStateAttackSmash();
            virtual void CtorStateAttackTurn();
            virtual void LoopStateAttackTurn();
            virtual void DtorStateAttackTurn();
            virtual void CtorStateThink();
            virtual void LoopStateThink();
            virtual void DtorStateThink();
            virtual void CtorStateDoNothing();
            virtual void LoopStateDoNothing();
            virtual void DtorStateDoNothing();
            virtual void CtorStateAttackHowling();
            virtual void LoopStateAttackHowling();
            virtual void DtorStateAttackHowling();
            virtual void CtorStateWaitDunk();
            virtual void LoopStateWaitDunk();
            virtual void DtorStateWaitDunk();
            virtual void CtorStateWandering();
            virtual void LoopStateWandering();
            virtual void DtorStateWandering();
            virtual void CtorStateAnger();
            virtual void LoopStateAnger();
            virtual void DtorStateAnger();
            virtual void CtorStateArtsEvent();
            virtual void LoopStateArtsEvent();
            virtual void DtorStateArtsEvent();

            // Sets the HP and the per-game values back to the start of a game.
            void GameResetingInitialize();
            // The world position of model node nNode (one of VNODE_NUM).
            void GetVNodePos(long nNode, bcs::FVEC3 &rPos) const;

            // The function registered with the body collision for balls
            // that bounce off; pArg is the boss.
            static void CallbackBallReflect(CCollBodyUnit *pBody, CBall *pBall, void *pArg);

        protected:
            // What InitFsm_ registers: each calls the virtual hook of the
            // same name.
            void CtorStateDefaultCall() { CtorStateDefault(); }
            void LoopStateDefaultCall() { LoopStateDefault(); }
            void DtorStateDefaultCall() { DtorStateDefault(); }
            void CtorStateSleepCall() { CtorStateSleep(); }
            void LoopStateSleepCall() { LoopStateSleep(); }
            void DtorStateSleepCall() { DtorStateSleep(); }
            void CtorStateNeutralCall() { CtorStateNeutral(); }
            void LoopStateNeutralCall() { LoopStateNeutral(); }
            void DtorStateNeutralCall() { DtorStateNeutral(); }
            void CtorStateMoveCall() { CtorStateMove(); }
            void LoopStateMoveCall() { LoopStateMove(); }
            void DtorStateMoveCall() { DtorStateMove(); }
            void CtorStateStartWarCryCall() { CtorStateStartWarCry(); }
            void LoopStateStartWarCryCall() { LoopStateStartWarCry(); }
            void DtorStateStartWarCryCall() { DtorStateStartWarCry(); }
            void CtorStateAttackFangCall() { CtorStateAttackFang(); }
            void LoopStateAttackFangCall() { LoopStateAttackFang(); }
            void DtorStateAttackFangCall() { DtorStateAttackFang(); }
            void CtorStateAttackClawCall() { CtorStateAttackClaw(); }
            void LoopStateAttackClawCall() { LoopStateAttackClaw(); }
            void DtorStateAttackClawCall() { DtorStateAttackClaw(); }
            void CtorStateAttackHornCall() { CtorStateAttackHorn(); }
            void LoopStateAttackHornCall() { LoopStateAttackHorn(); }
            void DtorStateAttackHornCall() { DtorStateAttackHorn(); }
            void CtorStateAttackTailCall() { CtorStateAttackTail(); }
            void LoopStateAttackTailCall() { LoopStateAttackTail(); }
            void DtorStateAttackTailCall() { DtorStateAttackTail(); }
            void CtorStateAttackMeteorCall() { CtorStateAttackMeteor(); }
            void LoopStateAttackMeteorCall() { LoopStateAttackMeteor(); }
            void DtorStateAttackMeteorCall() { DtorStateAttackMeteor(); }
            void CtorStateAttackFigaCall() { CtorStateAttackFiga(); }
            void LoopStateAttackFigaCall() { LoopStateAttackFiga(); }
            void DtorStateAttackFigaCall() { DtorStateAttackFiga(); }
            void CtorStateAttackThundergaCall() { CtorStateAttackThunderga(); }
            void LoopStateAttackThundergaCall() { LoopStateAttackThunderga(); }
            void DtorStateAttackThundergaCall() { DtorStateAttackThunderga(); }
            void CtorStateAttackJumpCall() { CtorStateAttackJump(); }
            void LoopStateAttackJumpCall() { LoopStateAttackJump(); }
            void DtorStateAttackJumpCall() { DtorStateAttackJump(); }
            void CtorStateDamageCall() { CtorStateDamage(); }
            void LoopStateDamageCall() { LoopStateDamage(); }
            void DtorStateDamageCall() { DtorStateDamage(); }
            void CtorStateKOCall() { CtorStateKO(); }
            void LoopStateKOCall() { LoopStateKO(); }
            void DtorStateKOCall() { DtorStateKO(); }
            void CtorStateDollCall() { CtorStateDoll(); }
            void LoopStateDollCall() { LoopStateDoll(); }
            void DtorStateDollCall() { DtorStateDoll(); }
            void CtorStateEscapeJumpCall() { CtorStateEscapeJump(); }
            void LoopStateEscapeJumpCall() { LoopStateEscapeJump(); }
            void DtorStateEscapeJumpCall() { DtorStateEscapeJump(); }
            void CtorStateRushCall() { CtorStateRush(); }
            void LoopStateRushCall() { LoopStateRush(); }
            void DtorStateRushCall() { DtorStateRush(); }
            void CtorStateAttackSmashCall() { CtorStateAttackSmash(); }
            void LoopStateAttackSmashCall() { LoopStateAttackSmash(); }
            void DtorStateAttackSmashCall() { DtorStateAttackSmash(); }
            void CtorStateAttackTurnCall() { CtorStateAttackTurn(); }
            void LoopStateAttackTurnCall() { LoopStateAttackTurn(); }
            void DtorStateAttackTurnCall() { DtorStateAttackTurn(); }
            void CtorStateThinkCall() { CtorStateThink(); }
            void LoopStateThinkCall() { LoopStateThink(); }
            void DtorStateThinkCall() { DtorStateThink(); }
            void CtorStateDoNothingCall() { CtorStateDoNothing(); }
            void LoopStateDoNothingCall() { LoopStateDoNothing(); }
            void DtorStateDoNothingCall() { DtorStateDoNothing(); }
            void CtorStateAttackHowlingCall() { CtorStateAttackHowling(); }
            void LoopStateAttackHowlingCall() { LoopStateAttackHowling(); }
            void DtorStateAttackHowlingCall() { DtorStateAttackHowling(); }
            void CtorStateWaitDunkCall() { CtorStateWaitDunk(); }
            void LoopStateWaitDunkCall() { LoopStateWaitDunk(); }
            void DtorStateWaitDunkCall() { DtorStateWaitDunk(); }
            void CtorStateWanderingCall() { CtorStateWandering(); }
            void LoopStateWanderingCall() { LoopStateWandering(); }
            void DtorStateWanderingCall() { DtorStateWandering(); }
            void CtorStateAngerCall() { CtorStateAnger(); }
            void LoopStateAngerCall() { LoopStateAnger(); }
            void DtorStateAngerCall() { DtorStateAnger(); }
            void CtorStateArtsEventCall() { CtorStateArtsEvent(); }
            void LoopStateArtsEventCall() { LoopStateArtsEvent(); }
            void DtorStateArtsEventCall() { DtorStateArtsEvent(); }

            // Defined in game/boss/CNpcBehemothBoss.cpp (names from the
            // symbols; meanings guessed).
            // The level clamped to 0..2 (0 above 2).
            long GetLvI();
            // Parameter nId of the boss's CC_PARAM table at level nLv.
            float GetDatF(long nId, long nLv);
            void ResetNeglect();
            void SetStickDamageCount();
            void SetStopAttackTime(float fTime);
            void StartCameraQuake(long nQuake);
            // Integer parameter nId at level nLv.
            long GetDatI(long nId, long nLv);
            // A random rate (compared with the GetDatI chances).
            long LotRate();
            // How many stick hits it takes before it turns round.
            long GetStickDamageCount();
            // The character it goes after next.
            long SearchingForTargetPlayer();
            // Restarts the time one character may hold the ball.
            void SetTimeMonitoring();
            // Rumble nType on the pads for nFrames (guessed).
            void StartPadVibration(long nType, long nFrames);
            // A random value between the two.
            float GetRandValueF(float fMin, float fMax);
            // Ends the state, with fWait frames before the next attack.
            void ExitNowStatus(float fWait);
            // The position the boss looks at (bAlt: the second choice).
            void GetTargetPos(bcs::FVEC3 &rPos, int bAlt);
            // m_bLookAt from parameter nId (UpdateLook_).
            void SetLookAtEnable_(long nId)
            {
                if (GetDatI(nId, 0) == 0) {
                    m_bLookAt = 0;
                } else {
                    m_bLookAt = 1;
                }
            }

            // Defined in this file.
            void GetOffsetPos_(bcs::FVEC3 vOfs, bcs::FVEC3 &rPos);
            void GetBasePos_(bcs::FVEC3 &rPos);
            float GetDistFromBasePos_(const bcs::FVEC3 &rPos);
            float GetDistFromCenterPos_(const bcs::FVEC3 &rPos);
            void SetupLookAtParam_();
            void UpdateLook_();
            void UpdateLookAt_(int bEnable, const bcs::FVEC3 &rPos, float fRate);
            void UpdateCollAtk_();
            void UpdateCollBody_();
            float GetBallShotTrgFrame_(long nMot, float fDefault) const;
            void DoDamage_(unsigned long nAtkType, long nChr, const bcs::FVEC3 &rPos);
            int IsBallReflect_(CBall *pBall);
            void CalcDamagePoint_(CBall *pBall, float &rPoint, long &rStrong);
            void ShotMeteor_();
            void ShotFiga_();
            void ShotThunderga_();

            // State work (FsmqGetWork) of the simple states.
            struct STATE_WORK {
                long m_nStep;  // 0x00
                float m_fWait; // 0x04
                float m_f08;   // 0x08
                float m_f0C;   // 0x0C
            };

            // State work of STATE_DAMAGE; the one who queues it sets the kind.
            struct DAMAGE_WORK {
                unsigned char m_Unk00[0x4];
                bcs::FVEC3 m_vPos; // 0x04, where it was hit
                long m_nKind;   // 0x10, 1 and 2: the heavier reactions
                long m_nStep;   // 0x14
            };

            // State work of STATE_KO: it falls to the ground first.
            struct KO_WORK {
                long m_nStep;      // 0x00
                bcs::FVEC3 m_vVel; // 0x04, per frame
            };

            // State work of STATE_ARTS_EVENT.
            struct ARTS_EVENT_WORK {
                long m_nStep;   // 0x00
                float m_fWait;  // 0x04
                long m_nCount;  // 0x08, set by message 0x24 (1: missed, 2 and up: hit)
            };

            long m_nLevel;                       // 0x1E8, what the constructor got
            long m_bHard;                        // 0x1EC, 1 from level 2 on
            float m_fHP;                         // 0x1F0
            float m_fMaxHP;                      // 0x1F4
            unsigned long m_Unk1F8;              // 0x1F8
            unsigned short m_nVNode[VNODE_NUM];  // 0x1FC, see GetVNodePos
            bcs::CAnmMatClr *m_pAnmClr;          // 0x220, the colour animation of the model
            nw4r::g3d::ResAnmClr m_aAnmClr[ANMCLR_NUM]; // 0x224, see SetColorAnim_
            long m_nAnmClr;                      // 0x250, the one playing (-1: none)
            CBall *m_pBall;                      // 0x254, the ball in play (MainFunc)
            long m_Unk258;                       // 0x258
            long m_Unk25C;                       // 0x25C
            long m_Unk260;                       // 0x260
            long m_Unk264;                       // 0x264
            long m_Unk268;                       // 0x268
            long m_Unk26C;                       // 0x26C
            bcs::FVEC3 m_vUnk270;                // 0x270
            bcs::FVEC3 m_vLookDir;               // 0x27C, the front of the look-at node in its own space
            // 0x288: the head's turn now (x pitch, y yaw; z is pi once
            // it has been set up).
            bcs::FVEC3 m_vLookRot;
            long m_nLookNode;                    // 0x294, the node that turns to look (VNODE 2)
            float m_fCourtRadius;                // 0x298
            float m_fUnk29C;                     // 0x29C
            unsigned char m_Unk2A0[0x2AC - 0x2A0];
            long m_Unk2AC;                       // 0x2AC
            float m_fUnk2B0;                     // 0x2B0
            float m_fUnk2B4;                     // 0x2B4
            long m_Unk2B8;                       // 0x2B8
            long m_Unk2BC;                       // 0x2BC
            long m_Unk2C0;                       // 0x2C0
            float m_fUnk2C4;                     // 0x2C4
            long m_Unk2C8;                       // 0x2C8
            long m_Unk2CC;                       // 0x2CC
            float m_fNeglect;                    // 0x2D0, set by ResetNeglect
            long m_Unk2D4;                       // 0x2D4
            long m_Unk2D8;                       // 0x2D8
            unsigned char m_Unk2DC[0x338 - 0x2DC];
            long m_bLookAt;                      // 0x338, the head turns to the target (UpdateLook_)
            long m_Unk33C;                       // 0x33C
            long m_Unk340[8];                    // 0x340
            float m_fUnk360;                     // 0x360
            long m_nSubStep;                     // 0x364, cleared by every state's constructor hook
            long m_Unk368;                       // 0x368
    };
}

#endif /* !MSW_CNPCBEHEMOTHBOSS_HPP_ */
