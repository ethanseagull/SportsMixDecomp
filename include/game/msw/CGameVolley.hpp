#ifndef CGAMEVOLLEY_HPP_
#define CGAMEVOLLEY_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Both live in module VolleyF.

    // Size 0x25C.
    class CGameVolley : public CGame {
        public:
            CGameVolley(bcs::CTask *pParent);

        private:
            unsigned char m_Unk150[0x25C - sizeof(CGame)];
    };

    // The volleyball party game. Size 0x2BC.
    class CGameVolleyRhythmReceive : public CGameVolley {
        public:
            CGameVolleyRhythmReceive(bcs::CTask *pParent);

        private:
            unsigned char m_Unk25C[0x2BC - 0x25C];
    };
}

#endif /* !CGAMEVOLLEY_HPP_ */
