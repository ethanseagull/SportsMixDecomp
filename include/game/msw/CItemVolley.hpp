#ifndef MSW_CITEMVOLLEY_HPP_
#define MSW_CITEMVOLLEY_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // INCOMPLETE: the volley game's items (HockeyF), one per DOL item kind.
    // Their constructors are inline (CItemManVolley::CreateItem calls the
    // DOL constructor and stores the two vtable pointers itself) and add no
    // members: each class has the size of its DOL base.
    //
    // Destructors: CItemVolleyBomb's and CItemVolleyBanana's are defined in
    // their units. The mushroom, toadstool and both shells have theirs inline
    // in the class (as in BasketF's CItemBasket.hpp), so the key function is
    // ExParamUpdate_ and each destructor is emitted at the head of its
    // vtable's group of inline functions at the end of the unit.

    class CItemVolleyMushroom : public CItemMushroom {
        public:
            CItemVolleyMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemVolleyMushroom() {}

            virtual void ExParamUpdate_();
    };

    class CItemVolleyToadstool : public CItemToadstool {
        public:
            CItemVolleyToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemVolleyToadstool() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemVolleyBomb : public CItemBomb {
        public:
            CItemVolleyBomb(CItemMan *pMan) : CItemBomb(pMan) {}
            virtual ~CItemVolleyBomb();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void GetExplosionParam(float &, float &, float &, long &, long &, unsigned long &);
    };

    class CItemVolleyGShell : public CItemGShell {
        public:
            CItemVolleyGShell(CItemMan *pMan) : CItemGShell(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemVolleyGShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemVolleyRShell : public CItemRShell {
        public:
            CItemVolleyRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemVolleyRShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemVolleyBanana : public CItemBanana {
        public:
            CItemVolleyBanana(CItemMan *pMan) : CItemBanana(pMan) {}
            virtual ~CItemVolleyBanana();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };
}

#endif /* !MSW_CITEMVOLLEY_HPP_ */
