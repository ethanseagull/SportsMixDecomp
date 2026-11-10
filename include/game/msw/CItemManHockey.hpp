#ifndef MSW_CITEMMANHOCKEY_HPP_
#define MSW_CITEMMANHOCKEY_HPP_

#include "game/msw/CItemMan.hpp"

namespace msw {

    // The item manager of the hockey game (HockeyF). It overrides only
    // CreateItem, to create the hockey item classes, and ExParamInitialize_.
    // No allocation of it was found, so whether it adds members is not known.
    class CItemManHockey : public CItemMan {
        public:
            // Inline: the binary has it after CItemMan's inline virtuals,
            // where the vtable's weak functions go, and it stores no vtable
            // pointer. So the vtable's key function is CreateItem and
            // CItemManHockey.cpp emits the vtable.
            virtual ~CItemManHockey() {}

            virtual CItem *CreateItem(long kind, const bcs::FVEC3 &pos);
            virtual void ExParamInitialize_();
    };
}

#endif /* !MSW_CITEMMANHOCKEY_HPP_ */
