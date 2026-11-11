#ifndef CCAMERAVOLLEYBOSS_HPP_
#define CCAMERAVOLLEYBOSS_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCameraVolley.hpp"

namespace msw {

    // Size 0x274 (from msw::CCameraVolley::Initialize). The camera of game
    // type 1 (the volley boss game): it looks at the boss from behind the
    // camera's team. Functions in game/volley/CCameraVolleyBoss.cpp.
    // Member names are guesses.
    class CCameraVolleyBoss : public CCameraVolley {
        public:
            CCameraVolleyBoss(long view);
            virtual ~CCameraVolleyBoss();

            virtual void MainCamera();
            virtual void Reset();

        private:
            // Zooms in (m_Unk1C0) while the team's leader stands close to
            // where the ball comes down and may touch it. Returns 1, or 0
            // when there is no leader or no ball.
            int MakeingCamZoom();
            void MainCamera_Normal();
            // How far back the camera has to go to keep the ball and the
            // other characters in view (at most m_fMaxLenHosei).
            float GetLenHosei(const bcs::FVEC3 &rCamPos, const bcs::FVEC3 &rTarget);
            // The fixed camera parameters (constructor and Reset).
            void SetCamParam()
            {
                m_fFovy = 40.0f;
                m_Unk258 = 4.79f;
                m_Unk25C = 3.51f;
                m_Unk260 = 11.0f;
                m_Unk264 = 0.92f;
                m_Unk268 = 0.99f;
                m_fLookRate = 0.937f;
                m_fMaxLenHosei = 4.0f;
            }

            float m_fFovy;          // 0x254
            float m_Unk258;         // 0x258 \ passed to MakeingFrontCamDataBase
            float m_Unk25C;         // 0x25C /
            float m_Unk260;         // 0x260, plus the length correction
            float m_Unk264;         // 0x264 \ passed to MakeingFrontCamDataBase
            float m_Unk268;         // 0x268 /
            float m_fLookRate;      // 0x26C, passed to CalcLookPos
            float m_fMaxLenHosei;   // 0x270, the most GetLenHosei gives
    };
}

#endif /* !CCAMERAVOLLEYBOSS_HPP_ */
