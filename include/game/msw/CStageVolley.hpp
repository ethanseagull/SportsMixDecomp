#ifndef MSW_CSTAGEVOLLEY_HPP_
#define MSW_CSTAGEVOLLEY_HPP_

#include "game/msw/CStageVolley01.hpp"
#include "game/msw/CStageVolley17.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {

    // INCOMPLETE: the volley stages (VolleyF) that CStageFactoryVolley::Create
    // makes, declared only as far as Create needs them: the constructor and
    // the size (from the allocation there). Their members are not mapped;
    // each unit that writes one of these classes should move it to a header
    // of its own and map the padding.
    //
    // The destructors are declared so that they are the key functions: this
    // header must not make a unit emit the vtables.

    // Stage 01 (CStageManVolley, then CStage01Base at 0x200) and stage 17
    // have their own headers.

    class CStageVolley02 : public CStageManVolley {
        public:
            CStageVolley02(bcs::CTask *pParent);
            virtual ~CStageVolley02();

        private:
            unsigned char m_Unk200[0x34]; // size 0x234
    };

    class CStageVolley03 : public CStageManVolley {
        public:
            CStageVolley03(bcs::CTask *pParent);
            virtual ~CStageVolley03();

        private:
            unsigned char m_Unk200[0x44]; // size 0x244
    };

    class CStageVolley05 : public CStageManVolley {
        public:
            CStageVolley05(bcs::CTask *pParent);
            virtual ~CStageVolley05();

        private:
            unsigned char m_Unk200[0x70]; // size 0x270
    };

    class CStageVolley06 : public CStageManVolley {
        public:
            CStageVolley06(bcs::CTask *pParent);
            virtual ~CStageVolley06();

        private:
            unsigned char m_Unk200[0x30]; // size 0x230
    };

    class CStageVolley09 : public CStageManVolley {
        public:
            CStageVolley09(bcs::CTask *pParent);
            virtual ~CStageVolley09();

        private:
            unsigned char m_Unk200[0x1C]; // size 0x21C
    };

    class CStageVolley10 : public CStageManVolley {
        public:
            // Made with 4 for stages 10, 37 and 38 (meaning not known).
            CStageVolley10(bcs::CTask *pParent, long);
            virtual ~CStageVolley10();

        private:
            unsigned char m_Unk200[0x28]; // size 0x228
    };

    class CStageVolley11 : public CStageManVolley {
        public:
            CStageVolley11(bcs::CTask *pParent);
            virtual ~CStageVolley11();

        private:
            unsigned char m_Unk200[0x430]; // size 0x630
    };

    class CStageVolley12 : public CStageManVolley {
        public:
            CStageVolley12(bcs::CTask *pParent);
            virtual ~CStageVolley12();

        private:
            unsigned char m_Unk200[0x28]; // size 0x228
    };

    class CStageVolley13 : public CStageManVolley {
        public:
            CStageVolley13(bcs::CTask *pParent);
            virtual ~CStageVolley13();

        private:
            unsigned char m_Unk200[0x54]; // size 0x254
    };

    class CStageVolley15 : public CStageManVolley {
        public:
            CStageVolley15(bcs::CTask *pParent);
            virtual ~CStageVolley15();

        private:
            unsigned char m_Unk200[0x8C]; // size 0x28C
    };

    class CStageVolley16 : public CStageManVolley {
        public:
            CStageVolley16(bcs::CTask *pParent);
            virtual ~CStageVolley16();

        private:
            unsigned char m_Unk200[0x1C]; // size 0x21C
    };

    class CStageVolley42 : public CStageManVolley {
        public:
            CStageVolley42(bcs::CTask *pParent);
            virtual ~CStageVolley42();
    };
}

#endif /* !MSW_CSTAGEVOLLEY_HPP_ */
