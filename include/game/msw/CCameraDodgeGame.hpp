#ifndef CCAMERADODGEGAME_HPP_
#define CCAMERADODGEGAME_HPP_

#include "game/msw/CCameraDodge.hpp"

namespace msw {

    // Size 0x6D4 (from msw::CCameraDodge::Initialize). The camera of a
    // dodgeball match; its unit is game/dodge/CCameraDodgeGame.cpp.
    // Overrides from its vtable (DodgeF). Every number it uses is set by
    // SetCamParam for the view's aspect type (bit 0x8 of m_UnkFC) and bit
    // 0x20 of m_UnkFC. Member names are guesses.
    //
    // The camera mode (CGameCamera::m_Unk228) runs from 1 to 14 in pairs:
    // the odd mode of a pair is the attacking view, the even one the
    // defending view; each pair has its own ModeParam. Mode 0 is the
    // normal view. 9/10: a team mate jumping for the ball, 11/12: a
    // character in an air trick, 13/14: the exceptional camera type 2.
    class CCameraDodgeGame : public CCameraDodge {
        public:
            // The curve one camera mode gives CalcCurveBlend, and the numbers
            // that go with it (0x4C bytes).
            struct CurveParam {
                bcs::FVEC3 m_Point[4]; // 0x00: blended per axis by GetPosYTarYLen
                // 0x30: the camera height, target height and distance
                // MainCamera_Normal uses instead of the curve's in modes 3/4
                // (the ball's thrower out of the court) and 13/14.
                float m_fHoldPosY;
                float m_fHoldTarY;
                float m_fHoldLen;
                float m_fSlant;      // 0x3C: passed to the slant target helpers
                float m_fLookScaleX; // 0x40: scales the look point's x
                float m_fHeightBase; // 0x44: the height the look point rises from
                float m_fHeightRate; // 0x48: how much it rises per unit above it
            };

            // One pair of camera modes (0x9C bytes).
            struct ModeParam {
                CurveParam m_Curve[2]; // 0x00: attacking, defending
                float m_fLerpRate;     // 0x98: passed to CalcCamData
            };

            CCameraDodgeGame(long view);
            virtual ~CCameraDodgeGame();
            virtual void MainCamera();
            virtual void Reset();
            virtual int IsOffense() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            int MakeingCamZoom();
            void MainCamera_Normal();
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            void GetPosYTarYLen(float *pPosY, float *pTarY, float *pLen, float x, float z);
            void GetOffDefParam(float *pScaleX, float *pSlant);
            void GetYParam(float *pBase, float *pRate);
            float GetCamModeLerpRate() const;
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250 (cleared as a word here).
            long m_nOffense;        // 0x254: the camera's team attacks (IsOffense)
            float m_fFovy;          // 0x258
            float m_fFrontRate;     // 0x25C: passed to MakeingFrontCamData
            float m_fLookRate;      // 0x260: passed to CalcLookPos
            float m_fUnk264;        // 0x264: set, never read
            float m_fMarginX;       // 0x268: GetFirstLookInfo_MultiPlay's
            float m_fLimitX;        // 0x26C  sideways shift: margin, limit
            float m_fRateX;         // 0x270  and rate
            float m_fUnk274;        // 0x274: set, never read
            float m_fUnk278;        // 0x278: set, never read
            float m_fUnk27C;        // 0x27C: set, never read
            float m_fNormalPosY;    // 0x280: MakeingFrontCamData's numbers
            float m_fNormalTarY;    // 0x284  in mode 0
            float m_fNormalLen;     // 0x288
            float m_fNormalOffX;    // 0x28C: target x is look x minus it in mode 0
            ModeParam m_ModeParam[7]; // 0x290
    };
}

#endif /* !CCAMERADODGEGAME_HPP_ */
