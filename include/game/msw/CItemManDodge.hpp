#ifndef MSW_CITEMMANDODGE_HPP_
#define MSW_CITEMMANDODGE_HPP_

#include "game/msw/CItemMan.hpp"

namespace msw {

    // The item manager of the dodgeball game (DodgeF). It overrides only
    // CreateItem, to create the dodgeball item classes, and
    // ExParamInitialize_. The destructor is inline: the binary has it among
    // the weak functions at the end of the file. So the vtable's key
    // function is CreateItem.
    class CItemManDodge : public CItemMan {
        public:
            virtual ~CItemManDodge() {}

            virtual CItem *CreateItem(long kind, const bcs::FVEC3 &pos);
            virtual void ExParamInitialize_();
    };
}

#endif /* !MSW_CITEMMANDODGE_HPP_ */
