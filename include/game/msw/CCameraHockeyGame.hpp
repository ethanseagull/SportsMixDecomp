#ifndef CCAMERAHOCKEYGAME_HPP_
#define CCAMERAHOCKEYGAME_HPP_

#include "game/msw/CCameraHockey.hpp"

namespace msw {

    // Size 0x900 (from msw::CCameraHockey::Initialize). The camera of a
    // hockey match; its unit is game/hockey/CCameraHockeyGame.cpp. Every
    // number it uses is set by SetCamParam for the view's aspect type (bit
    // 0x8 of m_UnkFC) and bit 0x20 of m_UnkFC. Member names are guesses.
    //
    // The camera mode (CGameCamera::m_Unk228) runs from 1 to 18 in pairs:
    // the odd mode of a pair is the attacking view, the even one the
    // defending view; each pair has its own ModeParam.
    class CCameraHockeyGame : public CCameraHockey {
        public:
            // The curve one camera mode gives CalcCurveBlend, and the numbers
            // that go with it (0x58 bytes).
            struct CurveParam {
                bcs::FVEC3 m_Point[6]; // 0x00: blended per axis by GetPosYTarYLen
                float m_fSlant;        // 0x48: passed to the slant target helpers
                float m_fLookScaleX;   // 0x4C: scales the look point's x
                float m_fHeightBase;   // 0x50: the height the look point rises from
                float m_fHeightRate;   // 0x54: how much it rises per unit above it
            };

            // One pair of camera modes (0xB4 bytes).
            struct ModeParam {
                CurveParam m_Curve[2]; // 0x00: attacking, defending
                float m_fLerpRate;     // 0xB0: passed to CalcCamData
            };

            CCameraHockeyGame(long view);
            virtual ~CCameraHockeyGame();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool IsNormalCamMode() const;

        private:
            long GetFirstLookInfo_SinglePlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            const bcs::FVEC3 &CalcLookInfo(const bcs::FVEC3 &rLook, const bcs::FVEC3 &rArg2,
                                           const bcs::FVEC3 &rArg3, float rate, long mask);
            void GetPosYTarYLen(float *pPosY, float *pTarY, float *pLen, float x, float z);
            void GetOffDefParam(float *pScaleX, float *pSlant);
            void GetYParam(float *pBase, float *pRate);
            float GetCamModeLerpRate() const;
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250: the look point's z offset.
            float m_fFovy;          // 0x254
            float m_fNear;          // 0x258: copied to m_Unk100 every frame
            float m_fFar;           // 0x25C: copied to m_Unk104 every frame
            float m_fBack;          // 0x260: m_Unk110 is -(m_fFar + m_fBack)
            float m_fFrontRate;     // 0x264: passed to MakeingFrontCamData
            float m_fLookRate;      // 0x268: passed to CalcLookPos
            float m_fLookOffZOff;   // 0x26C: m_Unk250 in attacking modes
            float m_fLookOffZDef;   // 0x270: m_Unk250 in defending modes (and 15, 16)
            float m_fLookLimitZ;    // 0x274: the look point's z stays within it
            float m_fLookLimitZSp;  // 0x278: the same in modes 15 and 16
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
            unsigned char m_Unk2A4[0x2AC - 0x2A4];
            ModeParam m_ModeParam[9]; // 0x2AC
    };
}

#endif /* !CCAMERAHOCKEYGAME_HPP_ */
