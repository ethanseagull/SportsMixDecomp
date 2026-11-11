#ifndef CCAMERABOSS_HPP_
#define CCAMERABOSS_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CGameCamera.hpp"

namespace msw {

    // The camera of the boss game (BossF), size 0x2A4 (from Initialize). It
    // looks at the boss from the far side of the court and keeps the
    // players in view. Its functions are in game/boss/CCameraBoss.cpp. It
    // declares no destructor: the original's implicit one is emitted after
    // the inline functions.
    //
    // Member names are guesses; those named by offset are not understood.
    class CCameraBoss : public CGameCamera {
        public:
            CCameraBoss(long view);

            // Makes one camera per view.
            static void Initialize();

            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;

        private:
            void MainCamera_Normal();
            // How far back the camera has to go to keep the ball and the
            // characters in view (at most m_fMaxLenHosei).
            float GetLenHosei(const bcs::FVEC3 &rCamPos, const bcs::FVEC3 &rTarget);
            // Places m_FromPos behind the players as seen from the boss and
            // rLook a little in front of the boss. Returns the ball mask for
            // CalcLookPos (always 0).
            long GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3);
            // Follows the character the player holding the ball controls,
            // or the first one a pad controls.
            void SetLookChr();
            // Sets the members from 0x27C for the view's aspect type and
            // camera kind (bits 0x8 and 0x20 of m_UnkFC).
            void SetCamParam();

            long m_nLookChr;          // 0x254, the player number of the character followed
            bcs::FVEC3 m_FromPos;     // 0x258, the point the camera looks from (x and z)
            bcs::FVEC3 m_BossPos;     // 0x264, the boss's position on the ground
            bcs::FVEC3 m_LookPos;     // 0x270, the point looked at
            float m_fFovy;            // 0x27C
            float m_Unk280;           // 0x280 \ passed to MakeingFrontCamDataBase
            float m_Unk284;           // 0x284 /
            float m_Unk288;           // 0x288, plus the length correction
            float m_Unk28C;           // 0x28C \ passed to MakeingFrontCamDataBase
            float m_Unk290;           // 0x290 /
            float m_fLookRate;        // 0x294, passed to CalcLookPos
            float m_fLookHeightRate;  // 0x298, the followed character's height is scaled by it
            float m_fCamDataRate;     // 0x29C, passed to CalcCamData
            float m_fMaxLenHosei;     // 0x2A0, the most GetLenHosei gives
    };
}

#endif /* !CCAMERABOSS_HPP_ */
