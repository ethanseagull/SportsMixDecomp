#ifndef MSW_CITEMMANVOLLEY_HPP_
#define MSW_CITEMMANVOLLEY_HPP_

#include "game/msw/CItemMan.hpp"

namespace msw {

    // The item manager of the volley game (VolleyF). It overrides only
    // CreateItem, to create the volley item classes, and ExParamInitialize_.
    // No allocation of it was found, so whether it adds members is not known.
    class CItemManVolley : public CItemMan {
        public:
            // Inline: the binary has it after CItemMan's inline virtuals,
            // where the vtable's weak functions go, and it stores no vtable
            // pointer. So the vtable's key function is CreateItem and
            // CItemManVolley.cpp emits the vtable.
            virtual ~CItemManVolley() {}

            virtual CItem *CreateItem(long kind, const bcs::FVEC3 &pos);
            virtual void ExParamInitialize_();
    };
}

#endif /* !MSW_CITEMMANVOLLEY_HPP_ */
