#ifndef CCAMERADODGEBD_HPP_
#define CCAMERADODGEBD_HPP_

#include "game/msw/CCameraDodge.hpp"

namespace msw {

    // Size 0x290 (from msw::CCameraDodge::Initialize). The camera of the
    // dodgeball party game. Only what other units need so far; its own unit
    // is game/dodge/CCameraDodgeBD.cpp. Overrides from its vtable (DodgeF).
    class CCameraDodgeBD : public CCameraDodge {
        public:
            CCameraDodgeBD(long view);
            virtual ~CCameraDodgeBD();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            void MainCamera_Normal();
            // Fills rLook with the point to look at: x halfway between the
            // leftmost and rightmost of the balls and characters in play, z
            // the furthest. Returns a mask with bit n set for each ball n
            // held by a character. The other two vectors are not touched.
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            // Sets the members below for the view's aspect type.
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250: the field of view.
            // Member names are guesses (the layout follows CCameraBasketHB's).
            float m_Unk254;                 // 0x254: copied to m_Unk100 every frame
            float m_Unk258;                 // 0x258: copied to m_Unk104 every frame
            bcs::FVEC3 m_CurvePoint[2];     // 0x25C: what CalcCurveBlend blends, per axis
            float m_Unk274;                 // 0x274: passed to MakeingFrontCamData
            float m_fLookRate;              // 0x278: passed to CalcLookPos
            float m_fScaleX;                // 0x27C: scales the look point's x
            float m_fScaleZ;                // 0x280: scales the depth of balls and characters
            float m_Unk284;                 // 0x284: set by SetCamParam, read nowhere here
            float m_Unk288;                 // 0x288
            float m_Unk28C;                 // 0x28C
    };
}

#endif /* !CCAMERADODGEBD_HPP_ */
