#ifndef MSW_CITEMHOCKEY_HPP_
#define MSW_CITEMHOCKEY_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // INCOMPLETE: the hockey game's items (HockeyF), one per DOL item kind.
    // Their constructors are inline (CItemManHockey::CreateItem calls the
    // DOL constructor and stores the two vtable pointers itself) and add no
    // members: each class has the size of its DOL base.
    //
    // Destructors: CItemHockeyBomb's and CItemHockeyBanana's are defined in
    // their units. The mushroom, toadstool and both shells have theirs inline
    // in the class (as in BasketF's CItemBasket.hpp), so the key function is
    // ExParamUpdate_ and each destructor is emitted at the head of its
    // vtable's group of inline functions at the end of the unit.

    class CItemHockeyMushroom : public CItemMushroom {
        public:
            CItemHockeyMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemHockeyMushroom() {}

            virtual void ExParamUpdate_();
    };

    class CItemHockeyToadstool : public CItemToadstool {
        public:
            CItemHockeyToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemHockeyToadstool() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemHockeyBomb : public CItemBomb {
        public:
            CItemHockeyBomb(CItemMan *pMan) : CItemBomb(pMan) {}
            virtual ~CItemHockeyBomb();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void GetExplosionParam(float &, float &, float &, long &, long &, unsigned long &);
    };

    class CItemHockeyGShell : public CItemGShell {
        public:
            CItemHockeyGShell(CItemMan *pMan) : CItemGShell(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemHockeyGShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemHockeyRShell : public CItemRShell {
        public:
            CItemHockeyRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            // Inline: see the note at the top of this file.
            virtual ~CItemHockeyRShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemHockeyBanana : public CItemBanana {
        public:
            CItemHockeyBanana(CItemMan *pMan) : CItemBanana(pMan) {}
            virtual ~CItemHockeyBanana();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };
}

#endif /* !MSW_CITEMHOCKEY_HPP_ */
