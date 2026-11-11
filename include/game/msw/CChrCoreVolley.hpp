#ifndef MSW_CCHRCOREVOLLEY_HPP_
#define MSW_CCHRCOREVOLLEY_HPP_

#include "game/msw/CChrCore.hpp"

namespace msw {

    // The volleyball character core, and one class per character (VolleyF).
    //
    // INCOMPLETE: only the constructors are declared, for
    // game/volley/CChrManVolley.cpp. CChrCore is incomplete too (see there),
    // so none of these classes may be instantiated or have their vtables
    // emitted until the rest is added (the destructors are in other units).
    // The sizes are what CChrManVolley::CreateObj_ allocates.
    class CChrCoreVolley : public CChrCore {
        public:
            CChrCoreVolley();
            virtual ~CChrCoreVolley();

        protected:
            // Not mapped yet; size 0x964.
            unsigned char m_Unk574[0x964 - 0x574];
    };

    class CChrCoreVolleyMario : public CChrCoreVolley {
        public:
            CChrCoreVolleyMario();
            virtual ~CChrCoreVolleyMario();
    };

    class CChrCoreVolleyLuigi : public CChrCoreVolley {
        public:
            CChrCoreVolleyLuigi();
            virtual ~CChrCoreVolleyLuigi();

        protected:
            unsigned char m_Unk964[0x970 - 0x964]; // allocated with 0x970 bytes
    };

    class CChrCoreVolleyPeach : public CChrCoreVolley {
        public:
            CChrCoreVolleyPeach();
            virtual ~CChrCoreVolleyPeach();
    };

    class CChrCoreVolleyDaisy : public CChrCoreVolley {
        public:
            CChrCoreVolleyDaisy();
            virtual ~CChrCoreVolleyDaisy();
    };

    class CChrCoreVolleyYoshi : public CChrCoreVolley {
        public:
            CChrCoreVolleyYoshi();
            virtual ~CChrCoreVolleyYoshi();
    };

    class CChrCoreVolleyWario : public CChrCoreVolley {
        public:
            CChrCoreVolleyWario();
            virtual ~CChrCoreVolleyWario();
    };

    class CChrCoreVolleyWaluigi : public CChrCoreVolley {
        public:
            CChrCoreVolleyWaluigi();
            virtual ~CChrCoreVolleyWaluigi();

        protected:
            unsigned char m_Unk964[0x994 - 0x964]; // allocated with 0x994 bytes
    };

    class CChrCoreVolleyDonkey : public CChrCoreVolley {
        public:
            CChrCoreVolleyDonkey();
            virtual ~CChrCoreVolleyDonkey();
    };

    class CChrCoreVolleyDiddy : public CChrCoreVolley {
        public:
            CChrCoreVolleyDiddy();
            virtual ~CChrCoreVolleyDiddy();
    };

    class CChrCoreVolleyKinopio : public CChrCoreVolley {
        public:
            CChrCoreVolleyKinopio();
            virtual ~CChrCoreVolleyKinopio();

        protected:
            unsigned long m_Unk964; // allocated with 0x968 bytes
    };

    class CChrCoreVolleyKoopa : public CChrCoreVolley {
        public:
            CChrCoreVolleyKoopa();
            virtual ~CChrCoreVolleyKoopa();

        protected:
            unsigned long m_Unk964; // allocated with 0x968 bytes
    };

    class CChrCoreVolleyKoopaJr : public CChrCoreVolley {
        public:
            CChrCoreVolleyKoopaJr();
            virtual ~CChrCoreVolleyKoopaJr();
    };

    class CChrCoreVolleyMoogle : public CChrCoreVolley {
        public:
            CChrCoreVolleyMoogle();
            virtual ~CChrCoreVolleyMoogle();
    };

    class CChrCoreVolleySaboten : public CChrCoreVolley {
        public:
            CChrCoreVolleySaboten();
            virtual ~CChrCoreVolleySaboten();
    };

    class CChrCoreVolleyNinja : public CChrCoreVolley {
        public:
            CChrCoreVolleyNinja();
            virtual ~CChrCoreVolleyNinja();

        protected:
            unsigned char m_Unk964[0x970 - 0x964]; // allocated with 0x970 bytes
    };

    class CChrCoreVolleySiroma : public CChrCoreVolley {
        public:
            CChrCoreVolleySiroma();
            virtual ~CChrCoreVolleySiroma();

        protected:
            unsigned long m_Unk964; // allocated with 0x968 bytes
    };

    class CChrCoreVolleySlime : public CChrCoreVolley {
        public:
            CChrCoreVolleySlime();
            virtual ~CChrCoreVolleySlime();

        protected:
            unsigned char m_Unk964[0x96C - 0x964]; // allocated with 0x96C bytes
    };

    class CChrCoreVolleyKuroma : public CChrCoreVolley {
        public:
            CChrCoreVolleyKuroma();
            virtual ~CChrCoreVolleyKuroma();

        protected:
            unsigned char m_Unk964[0x96C - 0x964]; // allocated with 0x96C bytes
    };

    class CChrCoreVolleyMii00 : public CChrCoreVolley {
        public:
            CChrCoreVolleyMii00();
            virtual ~CChrCoreVolleyMii00();
    };

    class CChrCoreVolleyMii01 : public CChrCoreVolley {
        public:
            CChrCoreVolleyMii01();
            virtual ~CChrCoreVolleyMii01();
    };
}

#endif /* !MSW_CCHRCOREVOLLEY_HPP_ */
