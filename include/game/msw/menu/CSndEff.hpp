#ifndef MENU_CSNDEFF_HPP_
#define MENU_CSNDEFF_HPP_

namespace msw {
    namespace menu {

        // Lives in the DOL (game/msw/menu/CVibEff.cpp). Size 8 (s_Instance
        // in .sbss), vptr at 0x04. The menu's sound effects: every effect
        // played is kept in a list so it can be stopped by number.
        // Argument names are guesses.
        class CSndEff {
            public:
                // Sets the enable flag and stops every effect still listed.
                void Initialize();
                // Clears the enable flag and stops every effect still listed.
                void Finalize();
                // Plays effect nSe (stopping earlier copies of it) and
                // returns the id CGameSnd::Play gives, or 0 when disabled.
                unsigned long Play(unsigned long nSe, unsigned long nUnk);
                // Stops every listed copy of effect nSe.
                void Stop(unsigned long nSe);

                static CSndEff s_Instance;
                // (added for MenuF game/menu/CSelectMenu.cpp, which emits it;
                // name from the symbol)
                static CSndEff *GetInstance() { return &s_Instance; }

                // (inline, no symbol; added for MenuF
                // game/menu/unit_6EE84.cpp)
                void SetEnable(int bEnable) { m_bEnable = bEnable; }

            private:
                int m_bEnable; // 0x00

            public:
                // Declared after the data: the vptr is at 0x04. Empty, but
                // defined in the .cpp: __sinit registers it for s_Instance.
                virtual ~CSndEff();
        };
    }
}

#endif /* !MENU_CSNDEFF_HPP_ */
