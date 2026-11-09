#ifndef CGAMEDODGE_HPP_
#define CGAMEDODGE_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Both live in module DodgeF.

    // Size 0x420.
    class CGameDodge : public CGame {
        public:
            CGameDodge(bcs::CTask *pParent);

        private:
            unsigned char m_Unk150[0x420 - sizeof(CGame)];
    };

    // The dodgeball party game. Size 0x4A8.
    class CGameDodgeDodgeOrDie : public CGameDodge {
        public:
            CGameDodgeDodgeOrDie(bcs::CTask *pParent);

        private:
            unsigned char m_Unk420[0x4A8 - 0x420];
    };
}

#endif /* !CGAMEDODGE_HPP_ */
