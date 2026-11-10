#ifndef MSW_CITEMBALLCHANGEEFFECT_HPP_
#define MSW_CITEMBALLCHANGEEFFECT_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // The effect played while a ball changes into an item ball: for a while
    // the item behaviour of the ball is shown and hidden in turn, then the
    // task deletes itself. A DOL class, size 0x48, vtable lbl_8046B700; each
    // game module derives its own (CItemBallChangeEffectBasket,
    // CItemBallChangeEffectBoss) to play its sounds in SetStartEffect and
    // SetEndEffect. Member names are guessed.
    class CItemBallChangeEffect : public bcs::CTask {
        public:
            CItemBallChangeEffect(bcs::CTask *pParent, unsigned long nBallId);
            virtual ~CItemBallChangeEffect();
            virtual void Main();
            // Called once on the second frame and once at the end. Empty in
            // the DOL (fn_800CFC88, fn_800CFC8C).
            virtual void SetStartEffect() {}
            virtual void SetEndEffect() {}

        protected:
            // Shows or hides the item behaviour of the ball: shown at the end
            // (bEnd), otherwise in the second half of each blink period.
            // (fn_800CFA9C; no exported name, name guessed)
            void UpdateItemDisp(int bEnd);

            unsigned long m_nBallId; // 0x34: task id of the ball
            long m_nStep;            // 0x38
            float m_fTime;           // 0x3C: how long the effect runs (60 frames)
            float m_fTimer;          // 0x40
            float m_fBlinkTimer;     // 0x44: runs four times as fast, wraps at m_fTime
    };
}

#endif /* !MSW_CITEMBALLCHANGEEFFECT_HPP_ */
