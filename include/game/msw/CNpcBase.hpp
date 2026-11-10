#ifndef MSW_CNPCBASE_HPP_
#define MSW_CNPCBASE_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // INCOMPLETE: a non-player character on the court (the boss Packun,
    // ...). A DOL class and a task (its constructor
    // __ct__Q23msw8CNpcBaseFPQ23msw7CChrManl calls bcs::CTask's). Size
    // 0x188: the constructor's last store is to 0x184, and
    // msw::CNpcBossPackun's state queue starts at 0x188. None of its
    // virtual functions or members are declared yet, so it must not be
    // instantiated or have virtuals added through it.
    class CNpcBase : public bcs::CTask {
        public:
            // (names guessed) Read by CStageBasketHungryBossPackun::Main_ in
            // BasketF to make the goal follow the boss.
            const bcs::FVEC3 &GetPos() const { return m_vPos; }
            const bcs::FVEC3 &GetRot() const { return m_vRot; }

        protected:
            unsigned char m_Unk34[0x60 - 0x34];
            bcs::FVEC3 m_vPos;   // 0x60
            bcs::FVEC3 m_vRot;   // 0x6C, written by RotateYRate
            bcs::FVEC3 m_vScale; // 0x78, written by SetScale
            unsigned char m_Unk84[0x188 - 0x84];
    };
}

#endif /* !MSW_CNPCBASE_HPP_ */
