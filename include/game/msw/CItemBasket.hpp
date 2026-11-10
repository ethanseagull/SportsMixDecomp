#ifndef MSW_CITEMBASKET_HPP_
#define MSW_CITEMBASKET_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // INCOMPLETE: the basketball game's items (BasketF), one per DOL item
    // kind, as in HockeyF (CItemHockey.hpp). Their constructors are inline
    // (the item manager's CreateItem calls the DOL constructor and stores
    // the two vtable pointers itself) and add no members. The destructor is
    // the first virtual and is defined in the unit, so the unit emits the
    // vtable.

    class CItemBasketBomb : public CItemBomb {
        public:
            CItemBasketBomb(CItemMan *pMan) : CItemBomb(pMan) {}
            virtual ~CItemBasketBomb();

            virtual float GetExplosionCollSize() const;
            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &pos, int);
            virtual void GetExplosionParam(float &, float &, float &, long &, long &, unsigned long &);
    };

    class CItemBasketMushroom : public CItemMushroom {
        public:
            CItemBasketMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            virtual ~CItemBasketMushroom();

            virtual void ExParamUpdate_();
    };

    class CItemBasketToadstool : public CItemToadstool {
        public:
            CItemBasketToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            virtual ~CItemBasketToadstool();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &pos, int);
    };

    class CItemBasketGShell : public CItemGShell {
        public:
            CItemBasketGShell(CItemMan *pMan) : CItemGShell(pMan) {}
            virtual ~CItemBasketGShell();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemBasketRShell : public CItemRShell {
        public:
            CItemBasketRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            virtual ~CItemBasketRShell();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemBasketBanana : public CItemBanana {
        public:
            CItemBasketBanana(CItemMan *pMan) : CItemBanana(pMan) {}
            virtual ~CItemBasketBanana();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };
}

#endif /* !MSW_CITEMBASKET_HPP_ */
