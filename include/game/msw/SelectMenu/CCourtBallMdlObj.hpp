#ifndef SELECTMENU_CCOURTBALLMDLOBJ_HPP_
#define SELECTMENU_CCOURTBALLMDLOBJ_HPP_

#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"

namespace msw {
    namespace SelectMenu {

        // The ball model of the court selection screen (MenuF; created by
        // msw::SelectMenu::CCourtChr). It slides from one position to
        // another and spins about Y. Size 0x200. Member names are guessed.
        class CCourtBallMdlObj : public bcs::CMdlObj {
            public:
                CCourtBallMdlObj();
                virtual ~CCourtBallMdlObj();

                void Main();
                // Plays the skeletal animation from the current frame
                // (flag bit 2 (value 2) is always added).
                void Play(nw4r::g3d::ResAnmChr res, float rate, unsigned long flag);
                // Replace the colour and texture animations. The float is
                // not used.
                void Play(nw4r::g3d::ResAnmClr res, float);
                void Play(nw4r::g3d::ResAnmTexSrt res, float);
                // Sets or restores konstant colours 0 and 1.
                void SetGoldColor(int bGold);

            private:
                // (guessed) Konstant colour 0, and the alpha of konstant
                // colour 1: both colours are built before the first call.
                void SetKColors(bcs::BCS_Color color0, bcs::BCS_Color color1)
                {
                    SetMatTevKColor(GX_KCOLOR0, color0);
                    SetMatTevKColorA(GX_KCOLOR1, color1.a);
                }

                float m_fMoveTime;        // 0x1C8, length of the slide in frames
                float m_fMoveTimer;       // 0x1CC, frames left of the slide
                bcs::FVEC3 m_vStartPos;   // 0x1D0
                bcs::FVEC3 m_vEndPos;     // 0x1DC
                bcs::FVEC3 m_vRot;        // 0x1E8, x and z are used
                float m_fRotSpeedMax;     // 0x1F4
                unsigned char m_nRotMode; // 0x1F8, 1: no spin, 2: spin down
                float m_fRotSpeed;        // 0x1FC
        };
    }
}

#endif /* !SELECTMENU_CCOURTBALLMDLOBJ_HPP_ */
