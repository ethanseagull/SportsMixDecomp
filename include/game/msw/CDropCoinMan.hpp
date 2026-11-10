#ifndef MSW_CDROPCOINMAN_HPP_
#define MSW_CDROPCOINMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CDropCoin;

    // (namespace or class: not known from the symbols)
    namespace DROP_COIN {
        // The kind of a dropped coin. Only 0 is seen in BossF (the score
        // coins); the names are guesses.
        enum CoinType {
            COIN_TYPE_COIN = 0
        };
    }

    // Owns the coins lying on the court. A DOL class, size 0x50 (every
    // module's CGimmickMan<Game>::Initialize_ allocates that much).
    //
    // INCOMPLETE: only what game/boss/CEffectCoinScore.cpp needs. The vtable
    // is not mapped past the bcs::CTask slots.
    class CDropCoinMan : public bcs::CUnivTask {
        public:
            CDropCoinMan(bcs::CTask *pParent);
            virtual ~CDropCoinMan();
            virtual void Main();

            // (meaning of the floats guessed) Drops nNum coins at rPos.
            // BossF passes 3.0f and 1.0f. Returns the first coin (type
            // guessed; the gimmick manager returns it as is).
            CDropCoin *CreateCoin(const bcs::FVEC3 &rPos, long nNum, float, float);

        private:
            unsigned char m_Unk34[0x50 - 0x34]; // 0x34
    };
}

#endif /* !MSW_CDROPCOINMAN_HPP_ */
