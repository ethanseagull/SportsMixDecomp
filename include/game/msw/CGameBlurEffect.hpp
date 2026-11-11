#ifndef CGAMEBLUREFFECT_HPP_
#define CGAMEBLUREFFECT_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // A full-screen blur that fades in, stays and fades out, then deletes
    // itself (DOL, game/msw/CGameExDemoBase.cpp; vtable lbl_8048D568). The
    // constructor's name is exported (BossF makes one); the member names
    // are guesses.
    class CGameBlurEffect : public bcs::CTask {
        public:
            // The three durations are in frames; a phase of zero frames is
            // skipped.
            CGameBlurEffect(bcs::CTask *pParent, float fFadeIn, float fKeep, float fFadeOut, float fScale,
                            bcs::BCS_Color color);
            virtual ~CGameBlurEffect();
            virtual void Main();

        private:
            long m_nStep;          // 0x34: 0 fade in, 1 keep, 2 fade out
            float m_fFadeIn;       // 0x38
            float m_fKeep;         // 0x3C
            float m_fFadeOut;      // 0x40
            float m_fScale;        // 0x44
            float m_fTime;         // 0x48: length of the current phase
            float m_fCount;        // 0x4C: frames into it
            bcs::BCS_Color m_Color; // 0x50
    };
}

#endif /* !CGAMEBLUREFFECT_HPP_ */
