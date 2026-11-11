#ifndef MSW_CITEMMANHOCKEY_HPP_
#define MSW_CITEMMANHOCKEY_HPP_

#include "game/msw/CItemMan.hpp"

namespace msw {

    // The item manager of the hockey game (HockeyF). It overrides only
    // CreateItem, to create the hockey item classes, and ExParamInitialize_.
    // HockeyF's CGimmickManHockey::Initialize_ allocates 0x344 bytes, so it adds no members.
    class CItemManHockey : public CItemMan {
        public:
            // Inline: the binary has it after CItemMan's inline virtuals,
            // where the vtable's weak functions go, and it stores no vtable
            // pointer. So the vtable's key function is CreateItem and
            // CItemManHockey.cpp emits the vtable.
            // Inline (CGimmickManHockey::Initialize_ calls CItemMan's
            // constructor and stores the vtable itself); size 0x344, the
            // same as CItemMan.
            CItemManHockey(bcs::CTask *pParent) : CItemMan(pParent) {}
            virtual ~CItemManHockey() {}

            virtual CItem *CreateItem(long kind, const bcs::FVEC3 &pos);
            virtual void ExParamInitialize_();
    };
}

#endif /* !MSW_CITEMMANHOCKEY_HPP_ */
