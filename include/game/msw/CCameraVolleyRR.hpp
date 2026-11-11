#ifndef CCAMERAVOLLEYRR_HPP_
#define CCAMERAVOLLEYRR_HPP_

#include "game/msw/CCameraVolley.hpp"

namespace msw {

    // Size 0x290 (from msw::CCameraVolley::Initialize). The camera of game
    // types 2 and 3: it looks at the middle of the characters and at where
    // the balls are heading, from a distance that follows how far they are
    // spread. Same layout as HockeyF's CCameraHockeyCS. Member names are
    // guesses; those named by offset are not understood yet.
    class CCameraVolleyRR : public CCameraVolley {
        public:
            CCameraVolleyRR(long view);
            virtual ~CCameraVolleyRR();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            void MainCamera_Normal();
            // Fills rLook with the point to look at: x halfway between the
            // leftmost and rightmost character, z the furthest character or
            // ball target. Returns the mask of the balls taken into account.
            // The other two vectors are not touched.
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            // Sets the members below for the view's aspect type.
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250: the field of view.
            float m_Unk254;                 // 0x254: copied to m_Unk100 every frame
            float m_Unk258;                 // 0x258: copied to m_Unk104 every frame
            bcs::FVEC3 m_CurvePoint[2];     // 0x25C: what CalcCurveBlend blends, per axis
            float m_Unk274;                 // 0x274: passed to MakeingFrontCamData
            float m_fLookRate;              // 0x278: passed to CalcLookPos
            float m_fScaleX;                // 0x27C: scales the look point's x
            float m_fScaleZ;                // 0x280: scales the depth of the characters
            float m_fMinX;                  // 0x284: start values of GetFirstLookInfo_MultiPlay's
            float m_fMaxX;                  // 0x288  search for the leftmost, rightmost
            float m_fMaxZ;                  // 0x28C  and furthest point
    };
}

#endif /* !CCAMERAVOLLEYRR_HPP_ */
