#ifndef MSW_CCHRCOREBOSS_HPP_
#define MSW_CCHRCOREBOSS_HPP_

#include "game/msw/CChrCore.hpp"
#include "game/msw/CC_PARAM_BOSS.hpp"

namespace msw {

    // The boss-mode character core, and one class per character.
    //
    // INCOMPLETE: only the parameter factory (game/boss/CC_PARAM_BOSS.cpp)
    // is declared. CChrCore is incomplete too (see there), so none of these
    // classes may be instantiated or have their vtables emitted until the
    // rest is added. Each destructor is declared first so that it, not
    // NewCcParam_, decides where the vtable goes (the destructors are in
    // other units).
    //
    // NewCcParam_ is virtual (slot 807 of __vt__Q23msw12CChrCoreBoss). It
    // allocates the character's parameter block for the current game level
    // with operator new(size, nHeap). Its return type is a guess.
    class CChrCoreBoss : public CChrCore {
        public:
            virtual ~CChrCoreBoss();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMario : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossMario();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossLuigi : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossLuigi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossPeach : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossPeach();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDaisy : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossDaisy();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossYoshi : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossYoshi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossWario : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossWario();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossWaluigi : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossWaluigi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDonkey : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossDonkey();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDiddy : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossDiddy();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKinopio : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossKinopio();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKoopa : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossKoopa();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKoopaJr : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossKoopaJr();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMoogle : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossMoogle();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSaboten : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossSaboten();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossNinja : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossNinja();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSiroma : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossSiroma();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSlime : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossSlime();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKuroma : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossKuroma();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMii00 : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossMii00();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMii01 : public CChrCoreBoss {
        public:
            virtual ~CChrCoreBossMii01();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };
}

#endif /* !MSW_CCHRCOREBOSS_HPP_ */
