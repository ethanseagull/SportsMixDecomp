#ifndef CCAMERABASKETHB_HPP_
#define CCAMERABASKETHB_HPP_

#include "game/msw/CCameraBasket.hpp"

namespace msw {

    // Size 0x29C (from msw::CCameraBasket::Initialize). The camera of the
    // basketball party game (game types 2 and 3): it looks at the middle of
    // the balls and characters in play from a distance that follows how far
    // they are spread. Its own unit is game/basket/CCameraBasketHB.cpp.
    // Member names are guesses; those named by offset are not understood yet.
    class CCameraBasketHB : public CCameraBasket {
        public:
            CCameraBasketHB(long view);
            virtual ~CCameraBasketHB();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            void MainCamera_Normal();
            // Fills rLook with the point to look at: x halfway between the
            // leftmost and rightmost character, z the furthest of the balls
            // and characters in play. Returns a mask with bit n set for each
            // ball n taken into account. The other two vectors are not
            // touched.
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            // Sets the members below for the view's aspect type.
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250: the field of view.
            float m_Unk254;                 // 0x254: copied to m_Unk100 every frame
            float m_Unk258;                 // 0x258: copied to m_Unk104 every frame
            bcs::FVEC3 m_CurvePoint[3];     // 0x25C: what CalcCurveBlend blends, per axis
            float m_Unk280;                 // 0x280: passed to MakeingFrontCamData
            float m_fLookRate;              // 0x284: passed to CalcLookPos
            float m_fScaleX;                // 0x288: scales the look point's x
            float m_fScaleZ;                // 0x28C: scales the depth of balls and characters
            float m_fMinX;                  // 0x290: start values of GetFirstLookInfo_MultiPlay's
            float m_fMaxX;                  // 0x294  search for the leftmost, rightmost
            float m_fMaxZ;                  // 0x298  and furthest point
    };
}

#endif /* !CCAMERABASKETHB_HPP_ */
