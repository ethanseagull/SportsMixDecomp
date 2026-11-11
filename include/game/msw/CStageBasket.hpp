#ifndef MSW_CSTAGEBASKET_HPP_
#define MSW_CSTAGEBASKET_HPP_

#include "game/msw/CStageBasket01.hpp"
#include "game/msw/CStageBasket02.hpp"
#include "game/msw/CStageBasket03.hpp"
#include "game/msw/CStageBasket05.hpp"
#include "game/msw/CStageBasket06.hpp"
#include "game/msw/CStageBasket07.hpp"
#include "game/msw/CStageBasket09.hpp"
#include "game/msw/CStageBasket11.hpp"
#include "game/msw/CStageBasket13.hpp"
#include "game/msw/CStageBasket15.hpp"
#include "game/msw/CStageBasket16.hpp"
#include "game/msw/CStageBasket17.hpp"
#include "game/msw/CStageBasketHungryBossPackun70.hpp"
#include "game/msw/CStageBasketHungryBossPackun71.hpp"
#include "game/msw/CStageBasketHungryBossPackun72.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // INCOMPLETE: the basket stages (BasketF) that CStageFactoryBasket::Create
    // makes, declared only as far as Create needs them: the constructor and
    // the size (from the allocation there). Their members are not mapped;
    // each unit that writes one of these classes should move it to a header
    // of its own and map the padding.

    // The destructors are declared so that they are the key functions: this
    // header must not make a unit emit the vtables.

    // Stages 01, 02, 03, 05, 06, 07, 09, 11, 13, 15, 16, 17 and 70 to 72 have their own headers.

    // Made with -1 (meaning not known).
    class CStageBasket10 : public CStageManBasket {
        public:
            CStageBasket10(bcs::CTask *pParent, long);
            virtual ~CStageBasket10();

        private:
            unsigned char m_Unk218[0x464]; // size 0x67C
    };

    // Stage 31: BasketF's own CStageManTournament (its constructor is in
    // BasketF). Assumed to derive from CStageManBasket, since Create returns
    // it as one; TourF may have a class of the same name.
    class CStageManTournament : public CStageManBasket {
        public:
            CStageManTournament(bcs::CTask *pParent);
            virtual ~CStageManTournament();

        private:
            unsigned char m_Unk218[0x4]; // size 0x21C
    };
}

#endif /* !MSW_CSTAGEBASKET_HPP_ */
