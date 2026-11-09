#ifndef CGAMEBOSS_HPP_
#define CGAMEBOSS_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Lives in module BossF. Size 0x1EC.
    class CGameBoss : public CGame {
        public:
            CGameBoss(bcs::CTask *pParent);

        private:
            unsigned char m_Unk150[0x1EC - sizeof(CGame)];
    };
}

#endif /* !CGAMEBOSS_HPP_ */
