#ifndef CCOCKPITMENUWINBTN_HPP_
#define CCOCKPITMENUWINBTN_HPP_

#include "game/msw/CCockpitMenuBase.hpp"

namespace msw {

    // The message windows of the menus, in the DOL (constructors at
    // 0x801F6480, 0x801F4AB4, 0x801F5304 and 0x801F4240): without a button,
    // with one (OK), with two (yes and no) and answered with A. Added for
    // MenuF game/menu/CSelectMenu.cpp with only what it uses: the
    // constructors (id, sort depth, meaning of the last word not known)
    // and, through CCockpitMenuBase, Setup (opens the window) and MenuOut
    // (closes it). Sizes 0x9C, 0x9C, 0x9C and 0xA0 (the allocations).
    // CSelectMenu sets members of the base (0x88, 0x90) before opening.
    class CCockpitMenuWinBtn0 : public CCockpitMenuBase {
        public:
            CCockpitMenuWinBtn0(long nId, float fSortZ, unsigned long nUnk);
            virtual int Set();

        private:
            friend class CSelectMenu;
            unsigned long m_Unk98; // 0x98
    };

    class CCockpitMenuWinBtn1 : public CCockpitMenuBase {
        public:
            CCockpitMenuWinBtn1(long nId, float fSortZ, unsigned long nUnk);
            virtual int Set();

        private:
            friend class CSelectMenu;
            unsigned long m_Unk98; // 0x98
    };

    class CCockpitMenuWinBtn2 : public CCockpitMenuBase {
        public:
            CCockpitMenuWinBtn2(long nId, float fSortZ, unsigned long nUnk);
            virtual int Set();

        private:
            friend class CSelectMenu;
            unsigned long m_Unk98; // 0x98
    };

    class CCockpitMenuWinBtnA : public CCockpitMenuBase {
        public:
            CCockpitMenuWinBtnA(long nId, float fSortZ, unsigned long nUnk);
            virtual int Set();

        private:
            friend class CSelectMenu;
            unsigned long m_Unk98; // 0x98
            unsigned long m_Unk9C; // 0x9C
    };

    // (DOL constructor 0x80266344; added for MenuF game/menu/CSelectMenu.cpp,
    // which only makes one on a test screen) A cockpit object of size 0x90.
    // Its base is not known: CSelectMenu adds it to the cockpit task and
    // calls the virtual at slot 0x18 of the vtable at 0x18 (Set).
    class CockpitItemInfoTab : public CCockpitLayoutBase {
        public:
            CockpitItemInfoTab(long nId, long nUnk, float fSortZ);
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x30]; // 0x60
    };
}

#endif /* !CCOCKPITMENUWINBTN_HPP_ */
