#ifndef MSW_CITEMDODGE_HPP_
#define MSW_CITEMDODGE_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // INCOMPLETE: the dodgeball game's items (DodgeF), one per DOL item kind.
    // Their constructors are inline (CItemManDodge::CreateItem calls the DOL
    // constructor and stores the two vtable pointers itself) and add no
    // members: each class has the size of its DOL base. The overrides are
    // those of the DodgeF vtables (__vt__Q23msw18CItemDodgeMushroom, ...).
    //
    // Destructors: CItemDodgeBomb's is defined in a .cpp (it sits among the
    // ordinary functions); the others are emitted as weak functions after
    // the code, so they are inline.

    class CItemDodgeMushroom : public CItemMushroom {
        public:
            CItemDodgeMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            virtual ~CItemDodgeMushroom() {}

            virtual void ExParamUpdate_();
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
    };

    class CItemDodgeToadstool : public CItemToadstool {
        public:
            CItemDodgeToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            virtual ~CItemDodgeToadstool() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
    };

    class CItemDodgeBomb : public CItemBomb {
        public:
            CItemDodgeBomb(CItemMan *pMan) : CItemBomb(pMan) {}
            virtual ~CItemDodgeBomb();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void GetExplosionParam(float &, float &, float &, long &, long &, unsigned long &);
    };

    // The DOL's green and red shells share a base, CItemShellCmn, that
    // CItemKinds.hpp does not have yet.
    class CItemDodgeGShell : public CItemGShell {
        public:
            CItemDodgeGShell(CItemMan *pMan) : CItemGShell(pMan) {}
            virtual ~CItemDodgeGShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
    };

    class CItemDodgeRShell : public CItemRShell {
        public:
            CItemDodgeRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            virtual ~CItemDodgeRShell() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
    };

    class CItemDodgeBanana : public CItemBanana {
        public:
            CItemDodgeBanana(CItemMan *pMan) : CItemBanana(pMan) {}
            virtual ~CItemDodgeBanana() {}

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
    };
}

#endif /* !MSW_CITEMDODGE_HPP_ */
