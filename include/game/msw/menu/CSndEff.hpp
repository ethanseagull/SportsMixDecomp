#ifndef MENU_CSNDEFF_HPP_
#define MENU_CSNDEFF_HPP_

namespace msw {
    namespace menu {

        // Lives in the DOL. Size 8 (s_Instance in .sbss). The menu's sound
        // effects. Only the members used so far; argument names are guesses.
        class CSndEff {
            public:
                void Initialize();
                void Finalize();
                // Returns the handle CGameSnd::Play gives, or 0.
                unsigned long Play(unsigned long nSe, unsigned long nUnk);
                void Stop(unsigned long nSe);

                static CSndEff s_Instance;

            private:
                unsigned char m_Unk00[0x8];
        };
    }
}

#endif /* !MENU_CSNDEFF_HPP_ */
