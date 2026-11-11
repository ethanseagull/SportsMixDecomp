#ifndef MSW_CCHRCOREBOSS_HPP_
#define MSW_CCHRCOREBOSS_HPP_

#include "game/msw/CChrCore.hpp"
#include "game/msw/CC_PARAM_BOSS.hpp"

namespace msw {

    // The boss-mode character core, and one class per character.
    //
    // INCOMPLETE: only the parameter factory (game/boss/CC_PARAM_BOSS.cpp)
    // and the constructors (game/boss/CChrManBoss.cpp) are declared. CChrCore is incomplete too (see there), so none of these
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
            CChrCoreBoss();
            virtual ~CChrCoreBoss();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;

        protected:
            // Not mapped yet. The size, 0x818, is what CChrManBoss::CreateObj_
            // allocates for most characters.
            unsigned char m_Unk574[0x818 - 0x574];
    };

    class CChrCoreBossMario : public CChrCoreBoss {
        public:
            CChrCoreBossMario();
            virtual ~CChrCoreBossMario();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossLuigi : public CChrCoreBoss {
        public:
            CChrCoreBossLuigi();
            virtual ~CChrCoreBossLuigi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossPeach : public CChrCoreBoss {
        public:
            CChrCoreBossPeach();
            virtual ~CChrCoreBossPeach();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDaisy : public CChrCoreBoss {
        public:
            CChrCoreBossDaisy();
            virtual ~CChrCoreBossDaisy();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossYoshi : public CChrCoreBoss {
        public:
            CChrCoreBossYoshi();
            virtual ~CChrCoreBossYoshi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossWario : public CChrCoreBoss {
        public:
            CChrCoreBossWario();
            virtual ~CChrCoreBossWario();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossWaluigi : public CChrCoreBoss {
        public:
            CChrCoreBossWaluigi();
            virtual ~CChrCoreBossWaluigi();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDonkey : public CChrCoreBoss {
        public:
            CChrCoreBossDonkey();
            virtual ~CChrCoreBossDonkey();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossDiddy : public CChrCoreBoss {
        public:
            CChrCoreBossDiddy();
            virtual ~CChrCoreBossDiddy();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKinopio : public CChrCoreBoss {
        public:
            CChrCoreBossKinopio();
            virtual ~CChrCoreBossKinopio();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;

        protected:
            unsigned long m_Unk818; // 0x818; allocated with 0x81C bytes
    };

    class CChrCoreBossKoopa : public CChrCoreBoss {
        public:
            CChrCoreBossKoopa();
            virtual ~CChrCoreBossKoopa();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossKoopaJr : public CChrCoreBoss {
        public:
            CChrCoreBossKoopaJr();
            virtual ~CChrCoreBossKoopaJr();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMoogle : public CChrCoreBoss {
        public:
            CChrCoreBossMoogle();
            virtual ~CChrCoreBossMoogle();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSaboten : public CChrCoreBoss {
        public:
            CChrCoreBossSaboten();
            virtual ~CChrCoreBossSaboten();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossNinja : public CChrCoreBoss {
        public:
            CChrCoreBossNinja();
            virtual ~CChrCoreBossNinja();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSiroma : public CChrCoreBoss {
        public:
            CChrCoreBossSiroma();
            virtual ~CChrCoreBossSiroma();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossSlime : public CChrCoreBoss {
        public:
            CChrCoreBossSlime();
            virtual ~CChrCoreBossSlime();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;

        protected:
            unsigned long m_Unk818; // 0x818; allocated with 0x81C bytes
    };

    class CChrCoreBossKuroma : public CChrCoreBoss {
        public:
            CChrCoreBossKuroma();
            virtual ~CChrCoreBossKuroma();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;

        protected:
            unsigned long m_Unk818; // 0x818; allocated with 0x81C bytes
    };

    class CChrCoreBossMii00 : public CChrCoreBoss {
        public:
            CChrCoreBossMii00();
            virtual ~CChrCoreBossMii00();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };

    class CChrCoreBossMii01 : public CChrCoreBoss {
        public:
            CChrCoreBossMii01();
            virtual ~CChrCoreBossMii01();
            virtual CHR_CORE_BOSS::CC_PARAM_BOSS *NewCcParam_(long nHeap) const;
    };
}

#endif /* !MSW_CCHRCOREBOSS_HPP_ */
