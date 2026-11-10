#ifndef MENU_CVIBEFF_HPP_
#define MENU_CVIBEFF_HPP_

namespace msw {
    namespace menu {

        // Lives in the DOL. Size 0x2C (s_Instance in .bss). Controller
        // rumble in the menus. Only the members used so far; argument names
        // and the return types are guesses.
        class CVibEff {
            public:
                void Initialize();
                void Finalize();
                // Starts CGameVib pattern nPattern of table 0 on every pad
                // of the bit mask nPadMask whose wait time has run out.
                void Play(long nPattern, long nUnk, unsigned long nPadMask);
                // Stops the rumble on every pad of the bit mask.
                void Stop(unsigned long nPadMask);
                // Counts the wait times down by the frame time.
                void MainLoop();

                static CVibEff s_Instance;

            private:
                int m_bEnable;           // 0x00: Initialize sets 1, Finalize 0
                float m_fWait[4];        // 0x04: per pad, frames before Play may start again
                float m_fInterval;       // 0x14: the wait Play sets (15)
                unsigned char m_Unk18[0x2C - 0x18];
        };
    }
}

#endif /* !MENU_CVIBEFF_HPP_ */
