#ifndef MSW_CNPCGHOSTVOLLEYBASE_HPP_
#define MSW_CNPCGHOSTVOLLEYBASE_HPP_

#include "bcs/BCS_Rand.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcGhost.hpp"

namespace msw {

    class CBall;
    class CChrCore;
    class CChrCoreVolley;
    class CChrMan;
    class CCollAtkUnit;
    class CCollBodyUnit;

    // The ghost (Boo) of the basketball game's haunted court (BasketF, size
    // 0x220, made by CChrManVolley::CreateGhost_). It floats about its
    // team's half, catches loose balls and passes them to its team, and
    // vanishes for a while when it is hit.
    //
    // Member and state work names are guesses.
    class CNpcGhostVolleyBase : public CNpcGhost {
        public:
            // The work memory of the states (0x24 bytes each, see
            // GetQueueWorkSize).
            struct STATE_WORK_SLEEP {
                long m_nStep;
                unsigned long m_nFlag;  // bit 0: ready to appear again
                float m_fTimer;         // frames left; -1 for none
            };
            struct STATE_WORK_MOVE {
                long m_nStep;
                int m_bIllumiOn;        // whether the lights over the target goal were on
                float m_fDir;           // 1 or -1 (not read here)
                bcs::FVEC3 m_vTarget;
            };
            struct STATE_WORK_GET_LOOSE_BALL {
                unsigned long m_hBall;  // the ball's task id
            };
            struct STATE_WORK_MOT_PLAY {
                long m_nStep;
                long m_nMotDir;         // +4 or -4: the side of the motion
                long m_nLoop;           // loops of the middle motion; -1 for endless
                float m_fRotY;          // the yaw to turn to
            };
            struct STATE_WORK_PURSUIT {
                long m_nStep;
                CBall *m_pBall;
            };
            struct STATE_WORK_DISAPPEARS {
                long m_nStep;
                float m_fTimer;
                float m_fSleep;         // frames to stay away
                float m_fAlpha;         // the alpha when the state began
                float m_fScale;
            };
            struct STATE_WORK_APPEARANCE {
                long m_nStep;
                float m_fTimer;
                float m_fWait;
                bcs::FVEC3 m_vPos;
            };
            struct STATE_WORK_PASS {
                long m_nStep;
                long m_nTarget;         // the character to pass to; -1 to throw it away
                long m_nPassType;
                float m_fWait;
                float m_fFrame;
                CChrCoreVolley *m_pChr;
                bcs::FVEC3 m_vTarget;
            };
            struct STATE_WORK_PASS_CATCH {
                long m_nStep;
                float m_fTimer;
                long m_nPassType;
                float m_fRotY;          // the yaw towards the ball
                CBall *m_pBall;
                bcs::FVEC3 m_vPos;
            };
            struct STATE_WORK_CATCH {
                long m_nStep;
                float m_fTimer;
            };

            // (parameter names guessed) nType is the NPC type, nTeam the
            // team the ghost plays for.
            // (The original defines it in the .cpp; the module link strips
            // that copy, as nothing calls it: CreateGhost_ has it inline.)
            CNpcGhostVolleyBase(CChrMan *pMan, long nType, long nTeam);
            virtual ~CNpcGhostVolleyBase();

            virtual void Create();
            virtual void Reset();
            virtual long SendMessage(long nMsg, long, long, long, long);
            virtual int CanbeCatch() const;
            virtual void MainFunc();
            virtual unsigned long CallbackBodyDamage_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                      const bcs::FVEC3 &rPos);
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual void CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall);
            virtual long GetQueueWorkSize() const { return 0x24; }
            virtual bcs::CMultViewAlphaMdlObj *NewMdlObj_(nw4r::g3d::ResMdl mdl, unsigned long flag);
            virtual unsigned long CallbackBallDamage_(CBall *pBall, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateMove();
            virtual void LoopStateMove();
            virtual void DtorStateMove();
            virtual void CtorStateGetLooseBall();
            virtual void LoopStateGetLooseBall();
            virtual void DtorStateGetLooseBall();
            virtual void CtorStateMotPlay();
            virtual void LoopStateMotPlay();
            virtual void CtorStatePursuit();
            virtual void LoopStatePursuit();
            virtual void DtorStatePursuit();
            virtual void CtorStateDisappears();
            virtual void LoopStateDisappears();
            virtual void DtorStateDisappears();
            virtual void CtorStateAppearance();
            virtual void LoopStateAppearance();
            virtual void DtorStateAppearance();
            virtual void CtorStatePass();
            virtual void LoopStatePass();
            virtual void DtorStatePass();
            virtual void CtorStatePassCatch();
            virtual void LoopStatePassCatch();
            virtual void DtorStatePassCatch();
            virtual void CtorStateCatch();
            virtual void LoopStateCatch();
            virtual void DtorStateCatch();

            // Own virtual functions, 0x158 on.
            // Not zero when the ghost can take a pass (the characters' pass
            // code asks it).
            virtual int CanbePassCatch() const;
            // Starts catching the pass of pBall aimed at rPos.
            virtual int MethdSetPassCatchState(CBall *pBall, const bcs::FVEC3 &rPos, long nPassType);
            // Run before the state machine each frame: changes the state on
            // what goes on around the ghost.
            virtual void ActInterrup();
            // (spelt so in the symbol; nothing calls it here)
            virtual void DtroStateMotPlay();
            // Moves towards rTarget at fSpeed, turning to it; not zero once
            // within fDist of it.
            virtual int MoveRun(const bcs::FVEC3 &rTarget, float fDist, float fSpeed);
            // The same without the turn rate of the parameters: turns to
            // fRotY by fRotRate.
            virtual int MoveRun(const bcs::FVEC3 &rTarget, float fDist, float fSpeed, float fRotY, float fRotRate);

            static void CallbackBallCatch2(CCollBodyUnit *pBody, CBall *pBall, void *pArg);

        protected:
            // Inline helpers of this file (names guessed; each shows as one
            // block in every function that uses it).

            // Leaves for state 0 with a sleep of fTime frames (-1: wake at
            // once).
            void ChangeStateSleep_(float fTime)
            {
                if (GetState() != STATE_SLEEP) {
                    STATE_WORK_SLEEP *pWork = static_cast<STATE_WORK_SLEEP *>(FsmqSetState(STATE_SLEEP));
                    pWork->m_nStep = fTime == -1.0f;
                    pWork->m_nFlag = 0;
                    pWork->m_fTimer = fTime;
                }
            }
            int CanbeDisappears_() const
            {
                switch (GetState()) {
                case STATE_SLEEP:
                case STATE_DISAPPEARS:
                    return 0;
                }
                return 1;
            }
            void ChangeStateDisappears_()
            {
                if (CanbeDisappears_()) {
                    FsmqSetState(STATE_DISAPPEARS);
                }
            }
            void ChangeStateMove_()
            {
                if (GetState() != STATE_MOVE) {
                    FsmqSetState(STATE_MOVE);
                }
            }
            int CanbePass_() const
            {
                if (!IsHoldBall()) {
                    return 0;
                }
                switch (GetState()) {
                case STATE_DISAPPEARS:
                case STATE_APPEARANCE:
                case STATE_PASS:
                case STATE_PASS_CATCH:
                case STATE_SLEEP:
                    return 0;
                }
                return 1;
            }
            // Passes to character nTarget (-1: throws the ball away); some
            // passes go astray anyway.
            void ChangeStatePass_(long nTarget)
            {
                if (CanbePass_()) {
                    STATE_WORK_PASS *pWork = static_cast<STATE_WORK_PASS *>(FsmqSetState(STATE_PASS));
                    pWork->m_nStep = 0;
                    pWork->m_nTarget = nTarget;
                    if (bcs::BCS_RandMax(99L) < VOLLEY_PRM_S06_PASS_RANDUM_PARCENT) {
                        pWork->m_nTarget = -1;
                    }
                }
            }
            int CanbeCatchState_() const
            {
                if (!IsHoldBall()) {
                    return 0;
                }
                switch (GetState()) {
                case STATE_DISAPPEARS:
                case STATE_APPEARANCE:
                case STATE_CATCH:
                case STATE_SLEEP:
                    return 0;
                }
                return 1;
            }
            int CheckCatch_()
            {
                if (!CanbeCatchState_()) {
                    return 0;
                }
                FsmqSetState(STATE_CATCH);
                return 1;
            }
            int CanbeMotPlay_() const
            {
                switch (GetState()) {
                case STATE_SLEEP:
                case STATE_DISAPPEARS:
                case STATE_MOT_PLAY:
                    return 0;
                }
                return 1;
            }
            int CanbePursuit_() const
            {
                if (IsHoldBall()) {
                    return 0;
                }
                if (!CanbeCatch()) {
                    return 0;
                }
                return (unsigned long)(GetState() - STATE_IDLE) <= STATE_MOVE - STATE_IDLE;
            }
            // Not zero when the ghost should go for the loose ball pBall.
            int CanbeGetLooseBall_(const CBall *pBall) const;
            int IsBallNear_(const CBall *pBall) const;
            // (name guessed) Bit 0 of CGame::g_GameWork's flags: a loose
            // ball is in play.
            static bool IsLooseBallMode() { return CGame::g_GameWork.m_nFlag18 & 1; }
            // Wakes the ghost when its sleep is over; not zero when it did.
            int CheckAppearance_();
            void PlayWaitMot_();
            static float CalcEaseRate_(float fRate);
            float GetMoveSpeed_() const
            {
                float fSpeed = VOLLEY_PRM_S06_MOVE_SPEED;
                if (GetState() == STATE_PURSUIT) {
                    fSpeed = VOLLEY_PRM_S06_MOVE_SPEED_SHOOT_CATCH;
                }
                return fSpeed;
            }
            // The area (0 to 3, see GetMoveTgtPosRand) the ghost is in.
            long GetArea_() const;
            // Takes a hit: drops the ball and vanishes.
            void CheckDamage_(CBall *pBall, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);

            void UpDateCollBody_();
            // A random point to float to, in front of the goal the ghost's
            // team attacks.
            bcs::FVEC3 GetMoveTgtPosRand() const;

            unsigned long m_nFlag;      // 0x204, bit 0: the ghost passed and waits for the next call
            float m_fPassCallWait;      // 0x208, frames before the ghost passes on its own
            float m_fFloatPhase;        // 0x20C, frames of the up and down float
            float m_fDamageWait;        // 0x210, frames it cannot catch after a hit
            float m_Unk214;
            float m_Unk218;
            float m_Unk21C;
    };
}

#endif /* !MSW_CNPCGHOSTVOLLEYBASE_HPP_ */
