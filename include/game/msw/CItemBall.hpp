#ifndef MSW_CITEMBALL_HPP_
#define MSW_CITEMBALL_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
    class CMdlRef;
    class CMdlShadow;
}

namespace msw {

    class CBall;

    namespace BALL {
        namespace VOLLEY {
            struct SPIKE_SHOT_ARG;
        }
    }

    // The behaviour an item gives the ball it is attached to. A DOL class;
    // each game module derives its own set from it.
    //
    // The order of the virtual functions is the order of the slots in the
    // vtables the modules emit (BossF, __vt__Q23msw17CItemBallBossStar).
    // Bodies given here are the ones the modules emit as weak functions.
    // Return types are guessed from those bodies unless noted.
    class CItemBall : public bcs::CUnivTaskEx {
        public:
            // Triggers handled by DoTrg (0x1C); each sets a bit in m_nTrgFlag
            // and calls the Trg*_ function of the same position.
            enum {
                TRG_SHOOT_START,
                TRG_REBOUND,
                TRG_REBOUND_TO_FREE,
                TRG_GOAL_IN,
                TRG_FREE_MODE,
                TRG_CHR_HIT,
                TRG_ATTACH,
                TRG_DAMAGE,
                TRG_DESTROY,
                TRG_CONSTRUCT,
                TRG_NUM
            };

            CItemBall(CBall *pBall, long type);
            virtual ~CItemBall();
            virtual void Main();

            virtual long GetType() const { return m_nType; }
            virtual bool DoTrg(long trg);
            // (guessed) no module emits the base version.
            virtual bool IsDisableRebound() const { return false; }
            virtual long GetBoundSe() const { return -1; }
            virtual bool IsMustHide() const { return true; }
            virtual float GetScale() const { return 1.0f; }
            virtual int IsChgDemoDisp() const;
            virtual void ChgDemoDisp(int);
            virtual long GetJumpShootTailEffect() const { return -1; }
            virtual void GetVolleySpikeParam(BALL::VOLLEY::SPIKE_SHOT_ARG &) const {}
            virtual unsigned long GetVolleyReceiveVariation(unsigned long variation) const { return variation; }
            virtual unsigned long GetVolleyValidSpikeVariation(unsigned long variation) const { return variation; }
            virtual unsigned long GetVolleyReceiveReaction(long, unsigned long reaction) const { return reaction; }
            virtual unsigned long GetVolleyBlockReaction(long, unsigned long reaction) const { return reaction; }
            virtual float GetVolleyFlickDownPower(long, float power) const { return power; }
            virtual long GetVolleyPassParam(long param, long) const { return param; }
            virtual long GetVolleyChanceBallParam(long param) const { return param; }
            // Not pure: the DOL's vtable of CItemBall has weak bodies for
            // both (fn_800CFCA8, fn_800CFCA0) that return the first argument.
            virtual long GetVolleySpikeTailEffect(long effect, long) const { return effect; }
            virtual long GetVolleyPassTailEffect(long effect, long, long) const { return effect; }
            virtual float GetVolleyCatchCollRadius() const { return 0.0f; }
            virtual float GetVolleyAttachOffsetY() const { return 0.0f; }

            // Empty in the DOL (fn_800CF158, defined in CItemBall.cpp).
            virtual void MainFunc_();
            virtual bool TrgShootStart_();
            virtual bool TrgRebound_();
            virtual bool TrgReboundToFree_();
            virtual bool TrgGoalIn_();
            virtual bool TrgFreeMode_();
            virtual bool TrgChrHit_();
            virtual bool TrgAttach_();
            virtual bool TrgDamage_();
            virtual bool TrgDestroy_();
            virtual bool TrgConstruct_();

            // The ball, or null when it is gone or being deleted.
            CBall *GetBall() const;
            // (fn_800CF4AC, name guessed) Not zero once the task is being
            // deleted or TrgDestroy_ has run. msw::CBall drops an item
            // behaviour for which this holds.
            int IsEnd() const;

        protected:
            // (fn_800CEC70, name guessed) Builds the item model of `type`
            // (IB*.brres in the item archive), its shadow and its
            // reflection, and attaches the model to the ball's.
            void CreateMdl_(long type);

            // Bits of m_nTrgFlag: bit 0 is set by TrgDestroy_, bit n + 1 by
            // trigger n (TRG_SHOOT_START to TRG_DAMAGE).
            unsigned long m_nBallId;     // 0x34, task id of the ball (its 0x04)
            unsigned long m_nTrgFlag;    // 0x38, one bit per trigger received (name guessed)
            bcs::CMdlObj *m_pMdl;        // 0x3C (names of the three models guessed)
            bcs::CMdlShadow *m_pShadow;  // 0x40
            bcs::CMdlRef *m_pRef;        // 0x44
            long m_nStep;                // 0x48
            bcs::FVEC3 m_vPos;           // 0x4C, follows the ball's position (0xF8)
            long m_nType;                // 0x58 (name guessed)
            int m_bChgDemoDisp;          // 0x5C, returned by IsChgDemoDisp
            // 0x60. Set by the derived classes; Main keeps it half a unit
            // above the ball. (name guessed)
            bcs::CMdlObj *m_pEffectMdl;
    };
}

#endif /* !MSW_CITEMBALL_HPP_ */
