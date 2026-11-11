#ifndef MSW_CITEMMANBASKET_HPP_
#define MSW_CITEMMANBASKET_HPP_

#include "game/msw/CItemMan.hpp"

namespace msw {

    // The item manager of the basketball game (BasketF). The same shape as
    // CItemManHockey: it overrides only CreateItem, to create the basketball
    // item classes, and ExParamInitialize_.
    class CItemManBasket : public CItemMan {
        public:
            // Inline: the binary has it after CItemMan's inline virtuals and
            // it stores no vtable pointer, so the vtable's key function is
            // CreateItem and CItemManBasket.cpp emits the vtable.
            // Inline (CGimmickManBasket::Initialize_ calls CItemMan's
            // constructor and stores the vtable itself); size 0x344.
            CItemManBasket(bcs::CTask *pParent) : CItemMan(pParent) {}
            virtual ~CItemManBasket() {}

            virtual CItem *CreateItem(long kind, const bcs::FVEC3 &pos);
            virtual void ExParamInitialize_();
    };
}

#endif /* !MSW_CITEMMANBASKET_HPP_ */
