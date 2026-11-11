#ifndef MSW_CCHRCOREHOCKEY_HPP_
#define MSW_CCHRCOREHOCKEY_HPP_

#include "game/msw/CChrCore.hpp"

namespace msw {

    // The hockey-mode character core, and one class per character.
    //
    // INCOMPLETE: only the constructors and the sizes that
    // CChrManHockey::CreateObj_ allocates (game/hockey/CChrManHockey.cpp),
    // and the one member function CFighting calls. CChrCore is incomplete
    // too (see there), so none of these classes may be instantiated other
    // than through new, or have their vtables emitted. Each destructor is
    // declared without a body, so that it keeps the vtable out of the units
    // that include this header (the destructors are in other units).
    class CChrCoreHockey : public CChrCore {
        public:
            CChrCoreHockey();
            virtual ~CChrCoreHockey();

            // (in HockeyF; return type and meaning guessed) Lets the
            // character join the fight over the puck; zero when it cannot.
            int MethdAddFighting_();

        protected:
            // Not mapped yet. The size, 0x76C, is what CreateObj_ allocates
            // for most characters.
            unsigned char m_Unk574[0x76C - 0x574];
    };

    class CChrCoreHockeyMario : public CChrCoreHockey {
        public:
            CChrCoreHockeyMario();
            virtual ~CChrCoreHockeyMario();
    };

    class CChrCoreHockeyLuigi : public CChrCoreHockey {
        public:
            CChrCoreHockeyLuigi();
            virtual ~CChrCoreHockeyLuigi();
        protected:
            unsigned char m_Unk76C[0x784 - 0x76C];
    };

    class CChrCoreHockeyPeach : public CChrCoreHockey {
        public:
            CChrCoreHockeyPeach();
            virtual ~CChrCoreHockeyPeach();
    };

    class CChrCoreHockeyDaisy : public CChrCoreHockey {
        public:
            CChrCoreHockeyDaisy();
            virtual ~CChrCoreHockeyDaisy();
    };

    class CChrCoreHockeyYoshi : public CChrCoreHockey {
        public:
            CChrCoreHockeyYoshi();
            virtual ~CChrCoreHockeyYoshi();
    };

    class CChrCoreHockeyWario : public CChrCoreHockey {
        public:
            CChrCoreHockeyWario();
            virtual ~CChrCoreHockeyWario();
    };

    class CChrCoreHockeyWaluigi : public CChrCoreHockey {
        public:
            CChrCoreHockeyWaluigi();
            virtual ~CChrCoreHockeyWaluigi();
        protected:
            unsigned char m_Unk76C[0x778 - 0x76C];
    };

    class CChrCoreHockeyDonkey : public CChrCoreHockey {
        public:
            CChrCoreHockeyDonkey();
            virtual ~CChrCoreHockeyDonkey();
    };

    class CChrCoreHockeyDiddy : public CChrCoreHockey {
        public:
            CChrCoreHockeyDiddy();
            virtual ~CChrCoreHockeyDiddy();
        protected:
            unsigned char m_Unk76C[0x770 - 0x76C];
    };

    class CChrCoreHockeyKinopio : public CChrCoreHockey {
        public:
            CChrCoreHockeyKinopio();
            virtual ~CChrCoreHockeyKinopio();
    };

    class CChrCoreHockeyKoopa : public CChrCoreHockey {
        public:
            CChrCoreHockeyKoopa();
            virtual ~CChrCoreHockeyKoopa();
    };

    class CChrCoreHockeyKoopaJr : public CChrCoreHockey {
        public:
            CChrCoreHockeyKoopaJr();
            virtual ~CChrCoreHockeyKoopaJr();
    };

    class CChrCoreHockeyMoogle : public CChrCoreHockey {
        public:
            CChrCoreHockeyMoogle();
            virtual ~CChrCoreHockeyMoogle();
    };

    class CChrCoreHockeySaboten : public CChrCoreHockey {
        public:
            CChrCoreHockeySaboten();
            virtual ~CChrCoreHockeySaboten();
    };

    class CChrCoreHockeyNinja : public CChrCoreHockey {
        public:
            CChrCoreHockeyNinja();
            virtual ~CChrCoreHockeyNinja();
        protected:
            unsigned char m_Unk76C[0x788 - 0x76C];
    };

    class CChrCoreHockeyKuroma : public CChrCoreHockey {
        public:
            CChrCoreHockeyKuroma();
            virtual ~CChrCoreHockeyKuroma();
        protected:
            unsigned char m_Unk76C[0x770 - 0x76C];
    };

    class CChrCoreHockeySiroma : public CChrCoreHockey {
        public:
            CChrCoreHockeySiroma();
            virtual ~CChrCoreHockeySiroma();
        protected:
            unsigned char m_Unk76C[0x780 - 0x76C];
    };

    class CChrCoreHockeySlime : public CChrCoreHockey {
        public:
            CChrCoreHockeySlime();
            virtual ~CChrCoreHockeySlime();
        protected:
            unsigned char m_Unk76C[0x770 - 0x76C];
    };

    class CChrCoreHockeyMii00 : public CChrCoreHockey {
        public:
            CChrCoreHockeyMii00();
            virtual ~CChrCoreHockeyMii00();
    };

    class CChrCoreHockeyMii01 : public CChrCoreHockey {
        public:
            CChrCoreHockeyMii01();
            virtual ~CChrCoreHockeyMii01();
    };
}

#endif /* !MSW_CCHRCOREHOCKEY_HPP_ */
