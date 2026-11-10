#ifndef MSW_CDROPCOIN_HPP_
#define MSW_CDROPCOIN_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // A coin dropped in the game (created by CDropCoinMan::CreateCoin). Lives
    // in the DOL. Only the members used so far. It is a task (the 0x0C word
    // is bcs::CTask's flag word); the members from 0x44 are 0x44 a flag word,
    // 0x48 something that is zero while the coin has no model (guessed), and
    // from 0x54 the throw state that DoParabolaThrow works on.
    class CDropCoin : public bcs::CTask {
        public:
            // True while the coin is switched on, has its 0x48 member and was
            // not asked to be deleted.
            bool CanbePickup() const;

        private:
            unsigned char m_Unk34[0x10]; // 0x34
            unsigned long m_nFlag;       // 0x44, bit 0: enabled (name guessed)
            void *m_pUnk48;              // 0x48
    };
}

#endif /* !MSW_CDROPCOIN_HPP_ */
