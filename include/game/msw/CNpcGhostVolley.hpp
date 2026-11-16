#ifndef MSW_CNPCGHOSTVOLLEY_HPP_
#define MSW_CNPCGHOSTVOLLEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CNpcGhost.hpp"

namespace msw {

    class CBall;
    class CChrMan;
    class CCollBodyUnit;
    class CCollDefUnit;

    // The ghost (Boo) of volleyball stage 6 (VolleyF, size 0x22C, made by
    // CChrManVolley::Create_). It floats along the net on the side of its
    // team, blocks the opponents' shots with a wall of defence collision
    // and spikes the balls that come down near it into the other court; it
    // vanishes when the lights are on or after it stayed its time.
    //
    // Member, state work and helper names are guesses.
    class CNpcGhostVolley : public CNpcGhost {
        public:
            // The work memory of the states (0x40 bytes each, see
            // GetQueueWorkSize).
            struct STATE_WORK_MOVE {
                long m_nStep;
                bcs::FVEC3 m_vTarget;   // 0x04, the point to float to
                bcs::FVEC3 m_vDir;      // 0x10, towards the target in the XZ plane, length 1
                float m_fSpeed;         // 0x1C, rises each frame up to a maximum
            };
            struct STATE_WORK_DISAPPEARS {
                long m_nStep;
            };
            struct STATE_WORK_APPEARANCE {
                long m_nStep;
            };
            struct STATE_WORK_HIDDEN {
                long m_nStep;
                float m_fWait;          // 0x04, frames before the ghost may come back
                int m_bCanAppear;       // 0x08, set by messages 0x0E and 0x16
            };
            struct STATE_WORK_BLOCK {
                long m_nStep;
                float m_fWait;          // 0x04, frames the block is held
                bcs::FVEC3 m_vFrom;     // 0x08
                bcs::FVEC3 m_vTo;       // 0x14
                float m_fRotFrom;       // 0x20
                float m_fRotTo;         // 0x24
                float m_fFrameNum;      // 0x28, frames of the move
                float m_fFrame;         // 0x2C
            };
            struct STATE_WORK_SPIKE {
                long m_nStep;
                bcs::FVEC3 m_vFrom;     // 0x04
                bcs::FVEC3 m_vTo;       // 0x10
                float m_fRotFrom;       // 0x1C
                float m_fRotTo;         // 0x20
                float m_fFrameNum;      // 0x24, frames of the move
                float m_fFrame;         // 0x28
                bcs::FVEC3 m_vTarget;   // 0x2C, where the spike goes
                int m_bHitEnable;       // 0x38, the hand's ball catch unit is on
                int m_bSpiked;          // 0x3C
            };

            // (parameter meaning guessed) CChrManVolley::Create_ passes
            // itself and 5.
            CNpcGhostVolley(CChrMan *pMan, long nType);
            virtual ~CNpcGhostVolley();

            virtual void Create();
            virtual long SendMessage(long nMsg, long nArg0, long nArg1, long nArg2, long nArg3);
            virtual void MainFunc();
            virtual void CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall);
            virtual long GetQueueWorkSize() const { return 0x40; }
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateMove();
            virtual void LoopStateMove();
            virtual void DtorStateMove();
            virtual void CtorStateDisappears();
            virtual void LoopStateDisappears();
            virtual void DtorStateDisappears();
            virtual void CtorStateAppearance();
            virtual void LoopStateAppearance();
            virtual void DtorStateAppearance();
            virtual void CtorStateHidden();
            virtual void LoopStateHidden();
            virtual void DtorStateHidden();
            virtual void CtorStateBlock();
            virtual void LoopStateBlock();
            virtual void DtorStateBlock();
            virtual void CtorStateSpike();
            virtual void LoopStateSpike();
            virtual void DtorStateSpike();

            // Own virtual function, 0x160: a ball hit the block wall.
            // Returns the reflect flags.
            virtual unsigned long CallbackDefReflectHit_(const CCollDefUnit *pDef, const CBall *pBall,
                                                         bcs::FVEC3 &rPos, bcs::FVEC3 &rVel);

            // Registered with the block wall; pArg is the ghost.
            static unsigned long CallbackDefReflectHit(const CCollDefUnit *pDef, const CBall *pBall,
                                                       bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, void *pArg);

        protected:
            // The ball nearest the ghost that is not scored; null without
            // balls.
            CBall *GetTargetBall_() const;
            // Not zero when an opponents' shot comes at the ghost's block.
            int DoBlockCheck_();
            // Not zero when a ball on the ghost's side comes down within
            // reach of its spike.
            int DoSpikeCheck_();
            // Spikes the held ball at rTarget.
            void SpikeBall_(const bcs::FVEC3 &rTarget);
            // Picks where in the opponents' court a ball at rBall is spiked
            // to (into rTarget) and returns the yaw towards it.
            float CalcTargetPos_(const bcs::FVEC3 &rBall, bcs::FVEC3 &rTarget);

            // The z of the line the ghost floats along: one unit from the
            // net, on the side of its team.
            float GetNetSideZ_() const;
            // Not zero when rPos is on the opponents' side of the net at
            // rNet.
            int IsOverNet_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rNet) const;

            CDefCollision m_DefColl;    // 0x204, the block wall
            float m_fFloatPhase;        // 0x210, radians of the up and down float
            float m_fRotY;              // 0x214, the yaw towards the opponents (0 or pi)
            bcs::FVEC3 m_vHandOfs;      // 0x218, the spiking hand from the ghost, before the yaw
            float m_fSpikeFrame;        // 0x224, frames from the spike motion's start to the hit
            float m_fStayTime;          // 0x228, frames left before the ghost leaves
    };
}

#endif /* !MSW_CNPCGHOSTVOLLEY_HPP_ */
