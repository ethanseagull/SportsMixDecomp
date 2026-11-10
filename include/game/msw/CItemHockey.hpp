#ifndef MSW_CITEMHOCKEY_HPP_
#define MSW_CITEMHOCKEY_HPP_

#include "game/msw/CItemKinds.hpp"

namespace msw {

    // INCOMPLETE: the hockey game's items (HockeyF), one per DOL item kind.
    // Their constructors are inline (CItemManHockey::CreateItem calls the
    // DOL constructor and stores the two vtable pointers itself) and add no
    // members: each class has the size of its DOL base. The destructors and
    // vtables are defined in the CItemHockey*.cpp units, so the destructor
    // is the first virtual and is only declared here.

    class CItemHockeyMushroom : public CItemMushroom {
        public:
            CItemHockeyMushroom(CItemMan *pMan) : CItemMushroom(pMan) {}
            virtual ~CItemHockeyMushroom();

            virtual void ExParamUpdate_();
    };

    class CItemHockeyToadstool : public CItemToadstool {
        public:
            CItemHockeyToadstool(CItemMan *pMan) : CItemToadstool(pMan) {}
            virtual ~CItemHockeyToadstool();

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
            virtual ~CItemHockeyGShell();

            virtual void ExParamUpdate_();
            virtual long GetParamThrowType() const;
            virtual float GetParamThrowSearchDirRangeMax() const;
            virtual float GetParamThrowSearchDistRangeMax() const;
    };

    class CItemHockeyRShell : public CItemRShell {
        public:
            CItemHockeyRShell(CItemMan *pMan) : CItemRShell(pMan) {}
            virtual ~CItemHockeyRShell();

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
