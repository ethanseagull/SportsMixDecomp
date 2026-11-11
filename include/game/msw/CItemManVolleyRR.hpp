#ifndef MSW_CITEMMANVOLLEYRR_HPP_
#define MSW_CITEMMANVOLLEYRR_HPP_

#include "game/msw/CItemManVolley.hpp"

namespace msw {

    // The item manager of the beach-volley ("RR") game: items thrown in it
    // are the RR toadstool (kind 2) or the RR banana (any other kind).
    // Nothing is known of its members.
    // Declared after the CItemVolley* classes in CItemManVolley.cpp: its
    // vtable lies between theirs and the RR items' in .data.
    class CItemManVolleyRR : public CItemManVolley {
        public:
            CItemManVolleyRR(bcs::CTask *pParent) : CItemManVolley(pParent) {}
            virtual ~CItemManVolleyRR() {}

            virtual CItem *CreateThrowItem(long kind, const bcs::FVEC3 &pos);
    };
}

#endif /* !MSW_CITEMMANVOLLEYRR_HPP_ */
