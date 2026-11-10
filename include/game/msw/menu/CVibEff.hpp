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
                void Play(long nPad, long nPattern, unsigned long nUnk);
                void Stop(unsigned long nPad);
                void MainLoop();

                static CVibEff s_Instance;

            private:
                unsigned char m_Unk00[0x2C];
        };
    }
}

#endif /* !MENU_CVIBEFF_HPP_ */
