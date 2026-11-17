#ifndef MSW_CNPCBOSSPACKUNBASKETBASE_HPP_
#define MSW_CNPCBOSSPACKUNBASKETBASE_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CNpcBossPackun.hpp"

namespace msw {

    class CNpcBossPackunBasketBase;

    // (names guessed) The task that drives the two boss Packuns of the
    // Hungry Boss Packun game (made by CChrManBasket::CreateBossPackun_ the
    // first time, size 0x94): it lets both sleep or one throw mud after a
    // random time while both are idle.
    //
    // Declared before the bosses' classes: the original's vtables of the
    // file are in that order.
    class CNpcBossPackunBasketGameMan : public CGameTask, public bcs::CFsm<CNpcBossPackunBasketGameMan> {
        public:
            enum STATE {
                STATE_INIT,
                STATE_SET_UP,
                STATE_MAIN,
                STATE_SLEEP,
                STATE_MUD_ATTACK_CHECK,
                STATE_END,
                STATE_NUM
            };

            CNpcBossPackunBasketGameMan(bcs::CTask *pParent);
            virtual ~CNpcBossPackunBasketGameMan();
            virtual void Main();

            // The instance; null when there is none.
            static CNpcBossPackunBasketGameMan *GetInstance() { return g_pInstance; }

            // (name guessed; inline in CNpcBossPackunBasketBase::SendMessage)
            // Follows the game's messages: back to the setup state on a
            // reset (3, 5, 0xF, 0x12, 0x13), to the main state on 0x16, to
            // the end on 0x17.
            void OnMessage(long nMsg);

            void CtorStateInit();
            void LoopStateInit();
            void DtorStateInit();
            void CtorStateSetUp();
            void LoopStateSetUp();
            void DtorStateSetUp();
            void CtorStateMain();
            void LoopStateMain();
            void DtorStateMain();
            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateMudAttackCheck();
            void LoopStateMudAttackCheck();
            void DtorStateMudAttackCheck();
            void CtorStateEnd();
            void LoopStateEnd();
            void DtorStateEnd();

            // Makes the instance under pParent, once.
            static void Create(bcs::CTask *pParent)
            {
                if (!g_pInstance) {
                    g_pInstance = new (0) CNpcBossPackunBasketGameMan(pParent);
                }
            }

            // The boss carrying goal nIdx.
            void SetBoss(long nIdx, CNpcBossPackunBasketBase *pBoss) { m_apBoss[nIdx] = pBoss; }

        private:
            // Not zero when no arts event plays and both bosses may fall
            // asleep.
            int IsAllBossCanbeSleep() const;

            static CNpcBossPackunBasketGameMan *g_pInstance;

            long m_nStep;                         // 0x7C, of STATE_MAIN
            long m_nMudNum;                       // 0x80, mud attacks since the counts were reset
            long m_nSleepFrame;                   // 0x84, frames asleep since the counts were reset
            float m_fTimer;                       // 0x88, frames to the next sleep or mud attack
            CNpcBossPackunBasketBase *m_apBoss[2]; // 0x8C, by goal
    };

    // The boss Piranha Plant of the basket game (BasketF), the one
    // CChrManBasket::GetBossPackun_ finds, derived from the DOL's
    // msw::CNpcBossPackun (whose bcs::CFsmQueue at 0x188 the
    // CBallManBasket::GetJustShotTiming*_ functions read). Size 0x220.
    // It stands behind a goal (nIdx 0 or 1) and catches the shots with its
    // mouth, which it opens and closes in time with a table (the lip sync).
    // The derived classes are the normal game's boss and the three party
    // stages' (70 to 72).
    //
    // Member, state work and motion names are guesses.
    class CNpcBossPackunBasketBase : public CNpcBossPackun {
        public:
            // The motions, indices into the tables of MotPlay_.
            enum MOT {
                MOT_IDLE_START,
                MOT_IDLE,
                MOT_MOUTH_OPEN,
                MOT_MOUTH_OPEN_LOOP,
                MOT_MOUTH_CLOSE,
                MOT_EAT,
                MOT_SLEEP_START,
                MOT_SLEEP,
                MOT_EAT_BIG,
                MOT_ATTACK_START,
                MOT_ATTACK_WAIT,
                MOT_ATTACK,
                MOT_MUD_START,
                MOT_MUD_THROW,
                MOT_MUD_END,
                MOT_MELT,
                MOT_FEARS,
                MOT_PARTY_EAT,
                MOT_END_IDLE_START,
                MOT_END_IDLE,
                MOT_NUM
            };

            // The work memory of the states (GetQueueWorkSize: 0x1C bytes).
            struct STATE_WORK_SLEEP {
                long m_nStep;
                int m_bEffect;          // the sleep effects were made (party stages)
                float m_fTimer;         // frames left asleep
            };
            struct STATE_WORK_IDLE {
                long m_nStep;
                long m_nLipTbl;         // the first index of GetLipSyncFrame
                long m_nLipKind;        // 0: open the mouth, 1: close it, 2: throw mud
                float m_fUnk0C;
                float m_fUnk10;
                float m_fTimer;         // frames left of this lip sync entry
            };
            struct STATE_WORK_ATTACK {
                long m_nStep;
                float m_fRotY;          // the yaw to attack in
            };
            struct STATE_WORK_MUD_ATTACK {
                long m_nStep;
                long m_nNum;            // the bombs to throw
                float m_fRotY;          // the yaw to turn to
                float m_fThrowRotY;     // the yaw of the throw
                bcs::FVEC3 m_vTarget;   // where the bombs go
            };
            struct STATE_WORK_EAT_WAIT {
                long m_nStep;
            };
            struct STATE_WORK_EAT {
                long m_nChr;            // the character whose shot was eaten
                long m_nKind;           // the kind of shot
                long m_nNextState;      // (party stages) the state after it
            };

            // The mud bomb of the basket game: a CNpcBossPackun::CMudBomb with
            // its own parameters and an attack collision. Size 0x30C.
            class CMudBombBasket : public CNpcBossPackun::CMudBomb {
                public:
                    CMudBombBasket(bcs::CTask *pParent)
                        : CMudBomb(pParent, -1)
                        , m_fExplodeMdlRadius(1.4f)
                    {
                    }
                    virtual ~CMudBombBasket() {}

                    virtual void CreateAtkColl();
                    virtual void UpdateAtkColl();
                    virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                             const bcs::FVEC3 &rPos, unsigned long);
                    virtual void Explode();
                    virtual const CStgMdl::MDL_PARAM *MdlParam_Bullet() const;
                    virtual const CStgMdl::MDL_PARAM *MdlParam_Explode() const;
                    virtual long GetParam_EffectID(long nEff) const;
                    virtual float GetParam_FadeOutTime() const;
                    virtual float GetParam_LifeLimit() const;
                    virtual float GetParam_ExplodeTime() const;
                    virtual float GetParam_GroundHeight() const;
                    virtual float GetParam_ExplodeMdlRadius() const;
                    virtual float GetParam_ExplodeAtkCollRadius() const;
                    virtual float GetParam_ExplodeAtkCollHeight() const;
                    virtual float GetParam_BulletJumpHeight() const;

                    // Throws the bomb from rFrom towards rTo: entry nIdx of
                    // the mud table nTbl, turned by fRotY, lands around rTo
                    // after fFrame frames.
                    void SetAppendParam(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, long nTbl, long nIdx,
                                        long nGoal, float fFrame, float fRotY);

                private:
                    CAtkCollision m_AtkColl;     // 0x2FC
                    float m_fExplodeMdlRadius;   // 0x308
            };

            CNpcBossPackunBasketBase(CChrMan *pMan, const bcs::FVEC3 &rPos, long nIdx);
            virtual ~CNpcBossPackunBasketBase();

            virtual void Create();
            virtual void Reset();
            virtual long SendMessage(long nMsg, long, long, long, long);
            virtual void MainFunc();
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush) {}
            virtual void CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall);
            virtual void CallbackAttackHit_(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                            const bcs::FVEC3 &rPos, unsigned long);
            virtual long GetGoalIdx() const;
            virtual int IsMouthOpen() const;
            virtual float GetRemainderMouthOpenFrame();
            virtual float GetRemainderMouthCloseFrame();
            virtual void CreateMdlObj_();
            virtual unsigned long GetQueueWorkSize() const { return sizeof(STATE_WORK_MUD_ATTACK); }
            virtual bcs::CMdlObj *NewMdlObj_(nw4r::g3d::ResMdl mdl, unsigned long heap);
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateRun() {}
            virtual void LoopStateRun() {}
            virtual void DtorStateRun() {}
            virtual void CtorStateAttack();
            virtual void LoopStateAttack();
            virtual void DtorStateAttack();
            virtual void CtorStateMudAttack();
            virtual void LoopStateMudAttack();
            virtual void DtorStateMudAttack();
            virtual void CtorStateEatWait();
            virtual void LoopStateEatWait();
            virtual void DtorStateEatWait();
            virtual void CtorStateEat();
            virtual void LoopStateEat();
            virtual void DtorStateEat();
            virtual void CtorStateMelt();
            virtual void LoopStateMelt();
            virtual void DtorStateMelt();
            virtual void CtorStateFears();
            virtual void LoopStateFears();
            virtual void DtorStateFears();
            virtual void CtorStateEndIdle();
            virtual void LoopStateEndIdle();
            virtual void DtorStateEndIdle();
            virtual void CalcRoot_();
            virtual void CheckDisp_();

            // Own virtual functions, 0x13C on.
            // Leaves the idle state for the fears when the arts event plays.
            virtual void ActInterrup();
            // The index of the sound tables: 0 for the game, 1 to 3 for the
            // party stages 70 to 72.
            virtual long GetType() const;
            // The sizes of the lip sync table (GetLipSyncFrame).
            virtual long GetLipFirstIdxNum() const { return 0; }
            virtual long GetLipSecondIdxNum() const;
            // The frames the mouth stays open or shut for entry
            // (nFirst, nSecond) of the lip sync table, and the kind of the
            // entry (rKind: see STATE_WORK_IDLE::m_nLipKind); -1 when there
            // is no table.
            virtual long GetLipSyncFrame(long nFirst, long nSecond, long &rKind);
            virtual int CanbeSleep();
            virtual int MethdSleep();
            virtual int CanbeIdle();
            virtual int MethdIdle();
            virtual int CanbeRun() { return 0; }
            virtual int MethdRun() { return 0; }
            virtual int CanbeAttack();
            virtual int MethdAttack();
            virtual int CanbeMudAttack();
            virtual int MethdMudAttack();
            virtual int CanbeEatWait();
            virtual int MethdEatWait();
            virtual int CanbeEat();
            virtual int MethdEat(long nChr, long nKind, long);
            virtual int CanbeMelt();
            virtual int MethdMelt();
            virtual int CanbeFears();
            virtual int MethdFears();
            virtual int CanbeEndIdle();
            virtual int MethdEndIdle();

            // The position the boss stands at.
            const bcs::FVEC3 &GetBasePos() const { return m_vBasePos; }

        protected:
            // The motion index (MOT) of the motion now playing; 0 for none.
            long GetMotIdx();
            // Plays motion nMot on the model and the matching one on the
            // second model, blending over fBlend frames (a negative fBlend
            // plays it backwards); returns the motion ID played.
            long MotPlay_(long nMot, int bLoop, float fBlend, float fFrame);
            // Sets the speed of both motions.
            void MotSetRate_(float fRate);
            // Plays the sound of table pSe for the boss's GetType at the
            // boss.
            void PlaySe_(const short *pSe);
            // Empties the state queue and starts the idle state.
            void ChangeStateIdle_();
            // Empties the state queue and starts state nState.
            void ResetState_(long nState);
            // The sleep sound, and the waking up (the derived classes
            // repeat these two states).
            void SleepStart_();
            void SleepEnd_();
            // Steps the second index of the lip sync table, back to 0 at
            // its end.
            void NextLipIdx_();
            // The speed that plays motion ID nMotID in fFrame frames,
            // between 1 and 3.
            float GetMotRateFit_(long nMotID, float fFrame);
            // The motion ID on the second model of motion nMot.
            static long GetSubMotID_(long nMot);
            // fRotY turned towards the court by at most nMaxDeg degrees from
            // the boss's base yaw.
            float LimitRotY_(float fRotY, long nMaxDeg) const;
            // (message 0xC, 0x10, 0x30) The game starts: the second model
            // shows when the boss faces the player's view.
            void OnStart_();
            // The lip sync of the idle state, shared by the party stages.
            void UpdateLipSync_(STATE_WORK_IDLE *pWork);
            float GetBasePosZ_();
            // The yaw to attack in: towards the shots and dunks in flight.
            float CalcAttackDir_();
            void UpDateCollAtk_();
            void UpDateCollBody_();

            bcs::CAnmMotion *m_pSubAnm;     // 0x1FC, the motion of m_pSubMdl
            long m_nGoalIdx;                // 0x200, the goal the boss carries
            // 0x204: flag bits; bit 0 is set once the game started (message
            // 0xC, 0x10 or 0x30).
            unsigned long m_nFlag;
            long m_nLipIdx;                 // 0x208, the second index of the lip sync table
            int m_bEnd;                     // 0x20C, set by message 0x11: no sleep, mud or melt any more
            float m_fBaseRotY;              // 0x210, the yaw the boss faces the court with
            bcs::FVEC3 m_vBasePos;          // 0x214
    };

    // The boss of the normal Hungry Boss Packun game: it sleeps until
    // CNpcBossPackunBasketGameMan wakes it.
    class CNpcBossPackunBasketGame : public CNpcBossPackunBasketBase {
        public:
            CNpcBossPackunBasketGame(CChrMan *pMan, const bcs::FVEC3 &rPos, long nIdx)
                : CNpcBossPackunBasketBase(pMan, rPos, nIdx)
            {
            }
            virtual ~CNpcBossPackunBasketGame() {}

            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateMelt();
            virtual void DtorStateMelt() {}
            virtual long GetType() const { return 0; }
            virtual long GetLipFirstIdxNum() const { return 1; }
            virtual long GetLipSecondIdxNum() const { return 10; }
            virtual long GetLipSyncFrame(long nFirst, long nSecond, long &rKind);
    };

    // The boss of party stage 70 (and the base of 71 and 72): the lip sync
    // table depends on the party level.
    class CNpcBossPackunBasketParty70 : public CNpcBossPackunBasketBase {
        public:
            CNpcBossPackunBasketParty70(CChrMan *pMan, const bcs::FVEC3 &rPos, long nIdx)
                : CNpcBossPackunBasketBase(pMan, rPos, nIdx)
            {
            }
            virtual ~CNpcBossPackunBasketParty70() {}

            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void CtorStateEat();
            virtual void LoopStateEat();
            virtual void CtorStateEndIdle();
            virtual void LoopStateEndIdle();
            virtual void DtorStateEndIdle() {}
            virtual void CheckDisp_();
            virtual void ActInterrup() {}
            virtual long GetType() const { return 1; }
            virtual long GetLipFirstIdxNum() const { return 3; }
            virtual long GetLipSecondIdxNum() const { return 10; }
            virtual long GetLipSyncFrame(long nFirst, long nSecond, long &rKind);
            virtual int CanbeEat();
            virtual int MethdEat(long nChr, long nKind, long);
            virtual int CanbeEndIdle() { return 1; }
    };

    class CNpcBossPackunBasketParty71 : public CNpcBossPackunBasketParty70 {
        public:
            CNpcBossPackunBasketParty71(CChrMan *pMan, const bcs::FVEC3 &rPos, long nIdx)
                : CNpcBossPackunBasketParty70(pMan, rPos, nIdx)
            {
            }
            virtual ~CNpcBossPackunBasketParty71() {}

            virtual long GetType() const { return 2; }
            virtual long GetLipFirstIdxNum() const { return 3; }
            virtual long GetLipSecondIdxNum() const { return 10; }
            virtual long GetLipSyncFrame(long nFirst, long nSecond, long &rKind);
    };

    class CNpcBossPackunBasketParty72 : public CNpcBossPackunBasketParty70 {
        public:
            CNpcBossPackunBasketParty72(CChrMan *pMan, const bcs::FVEC3 &rPos, long nIdx)
                : CNpcBossPackunBasketParty70(pMan, rPos, nIdx)
            {
            }
            virtual ~CNpcBossPackunBasketParty72() {}

            virtual long GetType() const { return 3; }
            virtual long GetLipFirstIdxNum() const { return 3; }
            virtual long GetLipSecondIdxNum() const { return 10; }
            virtual long GetLipSyncFrame(long nFirst, long nSecond, long &rKind);
    };
}

#endif /* !MSW_CNPCBOSSPACKUNBASKETBASE_HPP_ */
