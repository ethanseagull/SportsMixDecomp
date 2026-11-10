#ifndef MSW_CSTAGEHOCKEY_HPP_
#define MSW_CSTAGEHOCKEY_HPP_

#include "game/msw/CStageHockey01.hpp"
#include "game/msw/CStageHockey17.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace msw {

    // INCOMPLETE: the hockey stages (HockeyF) that CStageFactoryHockey::Create
    // makes, declared only as far as Create needs them: the constructor and
    // the size (from the allocation there). Their members are not mapped;
    // each unit that writes one of these classes should move it to a header
    // of its own and map the padding.

    // The destructors are declared so that they are the key functions: this
    // header must not make a unit emit the vtables.

    // Stages 01 and 17 have their own headers.

    class CStageHockey02 : public CStageManHockey {
        public:
            CStageHockey02(bcs::CTask *pParent);
            virtual ~CStageHockey02();

        private:
            unsigned char m_Unk218[0x34]; // size 0x24C
    };

    class CStageHockey03 : public CStageManHockey {
        public:
            CStageHockey03(bcs::CTask *pParent);
            virtual ~CStageHockey03();

        private:
            unsigned char m_Unk218[0xA0]; // size 0x2B8
    };

    class CStageHockey04 : public CStageManHockey {
        public:
            CStageHockey04(bcs::CTask *pParent);
            virtual ~CStageHockey04();

        private:
            unsigned char m_Unk218[0xA4C]; // size 0xC64
    };

    class CStageHockey05 : public CStageManHockey {
        public:
            CStageHockey05(bcs::CTask *pParent);
            virtual ~CStageHockey05();

        private:
            unsigned char m_Unk218[0x87C]; // size 0xA94
    };

    class CStageHockey07 : public CStageManHockey {
        public:
            CStageHockey07(bcs::CTask *pParent);
            virtual ~CStageHockey07();

        private:
            unsigned char m_Unk218[0x10]; // size 0x228
    };

    class CStageHockey09 : public CStageManHockey {
        public:
            CStageHockey09(bcs::CTask *pParent);
            virtual ~CStageHockey09();

        private:
            unsigned char m_Unk218[0x28]; // size 0x240
    };

    // Made with -1 (meaning not known).
    class CStageHockey10 : public CStageManHockey {
        public:
            CStageHockey10(bcs::CTask *pParent, long);
            virtual ~CStageHockey10();

        private:
            unsigned char m_Unk218[0x464]; // size 0x67C
    };

    class CStageHockey11 : public CStageManHockey {
        public:
            CStageHockey11(bcs::CTask *pParent);
            virtual ~CStageHockey11();

        private:
            unsigned char m_Unk218[0x43C]; // size 0x654
    };

    class CStageHockey12 : public CStageManHockey {
        public:
            CStageHockey12(bcs::CTask *pParent);
            virtual ~CStageHockey12();

        private:
            unsigned char m_Unk218[0x1C]; // size 0x234
    };

    class CStageHockey13 : public CStageManHockey {
        public:
            CStageHockey13(bcs::CTask *pParent);
            virtual ~CStageHockey13();

        private:
            unsigned char m_Unk218[0x54]; // size 0x26C
    };

    class CStageHockey15 : public CStageManHockey {
        public:
            CStageHockey15(bcs::CTask *pParent);
            virtual ~CStageHockey15();

        private:
            unsigned char m_Unk218[0x8C]; // size 0x2A4
    };

    class CStageHockey16 : public CStageManHockey {
        public:
            CStageHockey16(bcs::CTask *pParent);
            virtual ~CStageHockey16();

        private:
            unsigned char m_Unk218[0x1C]; // size 0x234
    };

    // The CrushSkate stages (85 to 87).
    class CStageHockeyCrushSkate85 : public CStageManHockey {
        public:
            CStageHockeyCrushSkate85(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate85();

        private:
            unsigned char m_Unk218[0xA5C]; // size 0xC74
    };

    class CStageHockeyCrushSkate86 : public CStageManHockey {
        public:
            CStageHockeyCrushSkate86(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate86();

        private:
            unsigned char m_Unk218[0x400]; // size 0x618
    };

    class CStageHockeyCrushSkate87 : public CStageManHockey {
        public:
            CStageHockeyCrushSkate87(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate87();

        private:
            unsigned char m_Unk218[0x40]; // size 0x258
    };
}

#endif /* !MSW_CSTAGEHOCKEY_HPP_ */
