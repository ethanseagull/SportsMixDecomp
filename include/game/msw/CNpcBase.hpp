#ifndef MSW_CNPCBASE_HPP_
#define MSW_CNPCBASE_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // A character that is not a player (the bosses and the stage's
    // creatures). A DOL class derived from bcs::CTask; its constructor
    // writes members up to 0x184, so it is taken to be 0x188 bytes (BasketF's
    // CNpcBossPackunBasketBase has a second base there).
    //
    // INCOMPLETE: only the position at 0x60 (BossF CCameraBoss::MainCamera
    // reads it) is mapped; no virtual function of the class is declared, so
    // it must not be instantiated or derived from in a unit that emits a
    // vtable.
    class CNpcBase : public bcs::CTask {
        public:
            // (name guessed) Where the character stands.
            const bcs::FVEC3 &GetPos() const { return m_Pos; }

        private:
            unsigned char m_Unk34[0x60 - 0x34];
            bcs::FVEC3 m_Pos;                    // 0x60
            unsigned char m_Unk6C[0x188 - 0x6C];
    };
}

#endif /* !MSW_CNPCBASE_HPP_ */
