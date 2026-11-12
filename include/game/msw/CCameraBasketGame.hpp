#ifndef CCAMERABASKETGAME_HPP_
#define CCAMERABASKETGAME_HPP_

#include "game/msw/CCameraBasket.hpp"

namespace msw {

    // Size 0x88C (from msw::CCameraBasket::Initialize). The camera of a
    // basketball match; its unit is game/basket/CCameraBasketGame.cpp.
    // Every number it uses is set by SetCamParam for the view's aspect type
    // (bit 0x8 of m_UnkFC) and bit 0x20 of m_UnkFC. Member names are
    // guesses.
    //
    // The camera mode (CGameCamera::m_Unk228) runs from 1 to 16 in pairs:
    // the odd mode of a pair is the attacking view, the even one the
    // defending view; each pair has its own ModeParam. Mode 0 is the normal
    // view. 3/4: a long one-on-one, 9/10 and 11/12: exceptional camera
    // types 4 and 5, 13/14: type 3, 15/16: the special mode of IsSpModeForBaHo.
    class CCameraBasketGame : public CCameraBasket {
        public:
            // The curve one camera mode gives CalcCurveBlend, and the numbers
            // that go with it (0x5C bytes).
            struct CurveParam {
                bcs::FVEC3 m_Point[6]; // 0x00: blended per axis by GetPosYTarYLen
                float m_fSlant;        // 0x48: passed to the slant target helpers
                float m_fLookScaleX;   // 0x4C: scales the look point's x
                float m_fHeightBase;   // 0x50: the height the look point rises from
                float m_fHeightRate;   // 0x54: how much it rises per unit above it (ball)
                float m_fHeightRateChr; // 0x58: the same when a character holds the ball
            };

            // One pair of camera modes (0xBC bytes).
            struct ModeParam {
                CurveParam m_Curve[2]; // 0x00: attacking, defending
                float m_fLerpRate;     // 0xB8: passed to CalcCamData
            };

            CCameraBasketGame(long view);
            virtual ~CCameraBasketGame();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool IsNormalCamMode() const;

        private:
            long GetFirstLookInfo_SinglePlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            inline const bcs::FVEC3 &CalcLookInfo(bcs::FVEC3 &rLook, const bcs::FVEC3 &rArg2,
                                                  const bcs::FVEC3 &rArg3, float rate, long mask);
            void GetPosYTarYLen(float *pPosY, float *pTarY, float *pLen, float x, float z);
            void GetOffDefParam(float *pScaleX, float *pSlant);
            void GetYParam(float *pBase, float *pRate, float *pRateChr);
            float GetCamModeLerpRate() const;
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250: the look point's z offset.
            float m_f1on1Time;      // 0x254: frames of one-on-one view
            float m_fFreeBallTime;  // 0x258: frames without a one-on-one, longer while no one holds the ball
            float m_fFovy;          // 0x25C
            float m_fNear;          // 0x260: copied to m_Unk100 every frame
            float m_fFar;           // 0x264: copied to m_Unk104 every frame
            float m_fBack;          // 0x268: m_Unk110 is -(m_fFar + m_fBack)
            float m_fFrontRate;     // 0x26C: passed to MakeingFrontCamData
            float m_fLookRate;      // 0x270: passed to CalcLookPos
            float m_fLookOffZOff;   // 0x274: m_Unk250 in attacking modes
            float m_fLookOffZDef;   // 0x278: m_Unk250 in defending modes
            float m_fMarginX;       // 0x27C: GetFirstLookInfo_MultiPlay's
            float m_fLimitX;        // 0x280  sideways shift: margin, limit
            float m_fRateX;         // 0x284  and rate
            float m_fMarginZ;       // 0x288: and the same for depth
            float m_fLimitZ;        // 0x28C
            float m_fRateZ;         // 0x290
            float m_fNormalPosY;    // 0x294: MakeingFrontCamData's numbers
            float m_fNormalTarY;    // 0x298  in mode 0
            float m_fNormalLen;     // 0x29C
            float m_fNormalOffX;    // 0x2A0: target x is look x minus it in mode 0
            float m_f1on1Wait;      // 0x2A4: one-on-one time before modes 3/4
            float m_fFreeBallWait;  // 0x2A8: modes 3/4 are left after this time
            ModeParam m_ModeParam[8]; // 0x2AC
    };
}

#endif /* !CCAMERABASKETGAME_HPP_ */
