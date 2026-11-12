#ifndef CCAMERAVOLLEYGAME_HPP_
#define CCAMERAVOLLEYGAME_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCameraVolley.hpp"

namespace msw {

    // Size 0x398 (from msw::CCameraVolley::Initialize). The camera of a
    // volley match (every other game type); its unit is
    // game/volley/CCameraVolleyGame.cpp. Every number it uses is set by
    // SetCamParam from bits 0x8, 0x10 and 0x20 of m_UnkFC. Member names are
    // guesses.
    //
    // Camera modes (CGameCamera::m_Unk228): 0 normal, 1 waiting to serve,
    // 2 serve thrown, 3 spike, 4 air trick, 5 block, 6 endure.
    class CCameraVolleyGame : public CCameraVolley {
        public:
            // The numbers of one camera mode (0x18 bytes): heights and
            // length for MakeingFrontCamData, the two floats for
            // MakeingSlantCamTar and the scale of the camera's x.
            struct ModeParam {
                float m_fPosY;   // 0x00
                float m_fTarY;   // 0x04
                float m_fLen;    // 0x08
                float m_fSlantB; // 0x0C: MakeingSlantCamTar's second float
                float m_fSlantA; // 0x10: its first
                float m_fScaleX; // 0x14
            };

            // A mode placed by CalcCurveBlend (0x3C bytes): four points
            // (x the camera height, y the target height, z the length),
            // blended over the court as {P0, P1, P0, P1, P2, P3}.
            struct CurveParam {
                bcs::FVEC3 m_Point[4]; // 0x00
                float m_fSlantB;       // 0x30
                float m_fSlantA;       // 0x34
                float m_fScaleX;       // 0x38
            };

            CCameraVolleyGame(long view);
            virtual ~CCameraVolleyGame();
            virtual void MainCamera();
            virtual void Reset();

        private:
            int MakeingCamZoom();
            void MainCamera_ServeWait();
            void MainCamera_ServeThrow();
            void MainCamera_Spike();
            void MainCamera_AirTrick();
            void MainCamera_Block();
            void MainCamera_Endure();
            void MainCamera_Normal();
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            void SetCamParam();

            // 0x250 is CGameCamera::m_Unk250: the field of view here.
            float m_fFrontRate;         // 0x254: MakeingFrontCamData's rate in mode 0
            float m_fLookRate;          // 0x258: CalcLookPos's rate in mode 0
            float m_Unk25C;             // 0x25C: set, never read
            float m_fZoomSpeed;         // 0x260: the zoom grows by this per 55 frames
            float m_fAirTrickRateY;     // 0x264: look height per unit of an air trick's height
            float m_fMarginX;           // 0x268: GetFirstLookInfo_MultiPlay's
            float m_fLimitX;            // 0x26C  sideways shift: margin, limit
            float m_fRateX;             // 0x270  and rate
            float m_fMarginZ;           // 0x274: and the same for depth
            float m_fLimitZ;            // 0x278
            float m_fRateZ;             // 0x27C
            ModeParam m_ServeWait;      // 0x280: mode 1
            float m_fServeRateY;        // 0x298: look heights per unit of the
            float m_fServeTarRateY;     // 0x29C  server's height in mode 1
            float m_fServeRateYBig;     // 0x2A0: the same for characters 6, 7
            float m_fServeTarRateYBig;  // 0x2A4  and 10
            ModeParam m_ServeThrow;     // 0x2A8: mode 2
            ModeParam m_Spike;          // 0x2C0: mode 3
            ModeParam m_SuperSpike;     // 0x2D8: mode 3, super spike
            ModeParam m_Block;          // 0x2F0: mode 5
            ModeParam m_Endure;         // 0x308: mode 6
            CurveParam m_Normal;        // 0x320: mode 0
            CurveParam m_AirTrick;      // 0x35C: mode 4
    };
}

#endif /* !CCAMERAVOLLEYGAME_HPP_ */
