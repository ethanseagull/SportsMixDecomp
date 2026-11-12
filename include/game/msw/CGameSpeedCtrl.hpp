#ifndef MSW_CGAMESPEEDCTRL_HPP_
#define MSW_CGAMESPEEDCTRL_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // Changes the speed of the whole game over time (slow motion). Lives in
    // the DOL; its destructor is fn_801895E4, which resets bcs::CMainApp's
    // universal speed to GameWork's value at 0x34. Only the members used so
    // far. A task of 0x6C bytes (guessed from the last member).
    class CGameSpeedCtrl : public bcs::CTask {
        public:
            // Moves the speed from `from` to `to` over `time` frames; a time
            // of zero or less sets `to` at once. The meaning of the last two
            // arguments is not known (they go to 0x68 and 0x60).
            void StartSpeedAnim(float from, float to, float time, int nUnk68, long nUnk60);

            // Moves the speed from its current value back to GameWork's speed
            // over `time` frames. Same arguments as above.
            void RestoreSpeed(float time, int nUnk68, long nUnk60);

            // (fn_801891AC; added for game/msw/CGame.cpp)
            CGameSpeedCtrl(bcs::CTask *pParent);

            // (name guessed) The slow motion of CGame::StartSlowEffect_:
            // from fSpeed to fTo over fTime frames; the meaning of the last
            // two values is not known.
            void StartSlow(float fSpeed, float fTo, float fTime, float f54, float f58)
            {
                m_fSpeed = fSpeed;
                m_fTo = fTo;
                m_fUnk54 = f54;
                m_fUnk58 = f58;
                m_fTime = fTime;
                m_fElapsed = 0.0f;
                m_nUnk60 = 0;
                m_nUnk5C = 1;
                m_nUnk64 = 0;
                m_nUnk68 = 1;
            }

        private:
            unsigned char m_Unk34[0xC]; // 0x34, a nw4r::ut::List of the children
            float m_fSpeed;             // 0x40 the speed in force
            float m_fFrom;              // 0x44
            float m_fTo;                // 0x48
            float m_fElapsed;           // 0x4C
            float m_fTime;              // 0x50
            float m_fUnk54;             // 0x54
            float m_fUnk58;             // 0x58
            int m_nUnk5C;               // 0x5C
            long m_nUnk60;              // 0x60
            int m_nUnk64;               // 0x64
            int m_nUnk68;               // 0x68
    };
}

#endif /* !MSW_CGAMESPEEDCTRL_HPP_ */
