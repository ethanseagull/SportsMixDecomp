#ifndef MSW_CITEMVOLLEY_HPP_
#define MSW_CITEMVOLLEY_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // The volley game's items (VolleyF), one per DOL item kind; the RR
    // game's are in CItemVolleyRR.hpp. All are written in
    // game/volley/CItemManVolley.cpp.
    //
    // Their constructors are inline (the item managers call the DOL
    // constructor and store the two vtable pointers themselves).
    //
    // Destructors are all inline in the class: the key function is
    // ExParamUpdate_, and each destructor is emitted at the head of its
    // vtable's group of inline functions at the end of the unit. The
    // toadstool's and the banana's come earlier, right after
    // CItemManVolleyRR::CreateThrowItem, because the RR classes derived from
    // them are constructed there.

    class CItemVolleyMushroom : public CItemMushroom {
        public:
            CItemVolleyMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            virtual ~CItemVolleyMushroom() {}

            virtual void ExParamUpdate_();
    };

    class CItemVolleyToadstool : public CItemToadstool {
        public:
            CItemVolleyToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            virtual ~CItemVolleyToadstool() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &rDir, int arg);
    };

    class CItemVolleyGShell : public CItemGShell {
        public:
            CItemVolleyGShell(CItemMan *pMan) : CItemGShell(pMan) {}
            virtual ~CItemVolleyGShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemVolleyRShell : public CItemRShell {
        public:
            CItemVolleyRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            virtual ~CItemVolleyRShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemVolleyBomb : public CItemBomb {
        public:
            CItemVolleyBomb(CItemMan *pMan) : CItemBomb(pMan) {}
            virtual ~CItemVolleyBomb() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemVolleyBanana : public CItemBanana {
        public:
            CItemVolleyBanana(CItemMan *pMan) : CItemBanana(pMan) {}
            virtual ~CItemVolleyBanana() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };
}

#endif /* !MSW_CITEMVOLLEY_HPP_ */
