#ifndef MSW_CCHRMARKERBASKET_HPP_
#define MSW_CCHRMARKERBASKET_HPP_

#include "game/msw/CChrMarker.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    // The character markers of the basket game (BasketF
    // game/basket/CArrowSignalBasket.cpp). Both are allocated with
    // CChrMarker's 0xA0 bytes by CChrMarkerBasketFactory::CreateMarker,
    // which inlines the constructors.
    class CChrMarkerBasket : public CChrMarker {
        public:
            // (what fog 0 and stage 6 are is not known)
            CChrMarkerBasket(bcs::CTask *pParent, long nChr, long nType, long nArg)
                : CChrMarker(pParent, nChr, nType, nArg)
            {
                if (CGame::g_GameWork.GetStage() == 6) {
                    SetFogIdx(0);
                }
            }
            virtual ~CChrMarkerBasket();

            virtual void ArrowSignalCheck();
    };

    // The marker of character 14 (name from the vtable; which character
    // that is, is a guess from the name).
    class CChrMarkerBasketNinja : public CChrMarkerBasket {
        public:
            CChrMarkerBasketNinja(bcs::CTask *pParent, long nChr, long nType, long nArg)
                : CChrMarkerBasket(pParent, nChr, nType, nArg)
            {
            }
            virtual ~CChrMarkerBasketNinja();

            virtual int AppearCheck();
            virtual int DisappearCheck();
    };

    class CChrMarkerBasketFactory {
        public:
            // Creates the marker of character nChr, and in the plain game
            // (game type 0) for marker types 0, 2 and 3 also the one-on-one
            // marker of his block moves.
            static CChrMarker *CreateMarker(bcs::CTask *pParent, long nChr, long nType, long nArg);
    };
}

#endif /* !MSW_CCHRMARKERBASKET_HPP_ */
