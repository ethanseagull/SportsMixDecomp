#ifndef MSW_CITEMBALL_HPP_
#define MSW_CITEMBALL_HPP_

#include "bcs/CUnivTask.hpp"

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
    class CItemBall : public bcs::CUnivTask {
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
            virtual long GetVolleySpikeTailEffect(long, long) const = 0;
            virtual long GetVolleyPassTailEffect(long, long, long) const = 0;
            virtual float GetVolleyCatchCollRadius() const { return 0.0f; }
            virtual float GetVolleyAttachOffsetY() const { return 0.0f; }

            virtual void MainFunc_() = 0;
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

            CBall *GetBall() const;

        private:
            unsigned long m_Unk34;       // 0x34, copied from the ball's 0x04
            unsigned long m_nTrgFlag;    // 0x38, one bit per trigger received (name guessed)
            void *m_Unk3C;               // 0x3C
            unsigned long m_Unk40[3];    // 0x40
            float m_Unk4C[3];            // 0x4C, copied from the ball's 0xF8
            long m_nType;                // 0x58 (name guessed)
            int m_Unk5C;                 // 0x5C, returned by IsChgDemoDisp
            unsigned long m_Unk60;       // 0x60
    };
}

#endif /* !MSW_CITEMBALL_HPP_ */
