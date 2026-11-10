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

        private:
            unsigned char m_Unk34[0xC]; // 0x34, a nw4r::ut::List of the children
            float m_fSpeed;             // 0x40 the speed in force
            float m_fFrom;              // 0x44
            float m_fTo;                // 0x48
            float m_fElapsed;           // 0x4C
            float m_fTime;              // 0x50
            unsigned char m_Unk54[8];   // 0x54
            int m_nUnk5C;               // 0x5C
            long m_nUnk60;              // 0x60
            int m_nUnk64;               // 0x64
            int m_nUnk68;               // 0x68
    };
}

#endif /* !MSW_CGAMESPEEDCTRL_HPP_ */
