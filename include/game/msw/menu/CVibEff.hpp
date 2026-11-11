#ifndef MENU_CVIBEFF_HPP_
#define MENU_CVIBEFF_HPP_

namespace msw {
    namespace menu {

        // Lives in the DOL (game/msw/menu/CVibEff.cpp, with CSndEff and
        // CSndBGM). Size 0x1C (s_Instance in .bss), vptr at 0x18.
        // Controller rumble in the menus. Argument names are guesses.
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
                // (added for MenuF game/menu/unit_61C00.cpp; inline, no
                // symbol)
                void SetEnable(int bEnable) { m_bEnable = bEnable; }

                static CVibEff s_Instance;

            private:
                int m_bEnable;           // 0x00: Initialize sets 1, Finalize 0
                float m_fWait[4];        // 0x04: per pad, frames before Play may start again
                float m_fInterval;       // 0x14: the wait Play sets (15)

            public:
                // Declared after the data: the vptr is at 0x18. Empty, but
                // defined in the .cpp: __sinit registers it for s_Instance.
                virtual ~CVibEff();
        };
    }
}

#endif /* !MENU_CVIBEFF_HPP_ */
