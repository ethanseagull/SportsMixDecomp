#ifndef MSW_CCOMDRVHOCKEYGAME_HPP_
#define MSW_CCOMDRVHOCKEYGAME_HPP_

#include "game/msw/CComDrvHockey.hpp"

namespace msw {

    // INCOMPLETE: the three computer drivers the hockey character managers
    // make (game/hockey/CChrManHockey.cpp), all derived from CComDrvHockey
    // (their constructors call CComDrvHockey(long)). Only the constructors
    // and the allocation sizes are known here: CComDrvHockeyGame 0x494 (field
    // players), CComDrvHockeyKeeper 0x1B8 (keepers), CComDrvHockeyCS 0x1FC
    // (the party and mission game). The destructors are declared without
    // bodies so that no unit emits the vtables from this header.
    class CComDrvHockeyGame : public CComDrvHockey {
        public:
            CComDrvHockeyGame();
            virtual ~CComDrvHockeyGame();

        protected:
            unsigned char m_Unk1B0[0x494 - 0x1B0];
    };

    class CComDrvHockeyKeeper : public CComDrvHockey {
        public:
            CComDrvHockeyKeeper();
            virtual ~CComDrvHockeyKeeper();

        protected:
            unsigned char m_Unk1B0[0x1B8 - 0x1B0];
    };

    class CComDrvHockeyCS : public CComDrvHockey {
        public:
            CComDrvHockeyCS();
            virtual ~CComDrvHockeyCS();

        protected:
            unsigned char m_Unk1B0[0x1FC - 0x1B0];
    };
}

#endif /* !MSW_CCOMDRVHOCKEYGAME_HPP_ */
