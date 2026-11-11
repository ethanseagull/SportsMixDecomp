#ifndef MSW_CSTAGEHOCKEY_HPP_
#define MSW_CSTAGEHOCKEY_HPP_

#include "game/msw/CStageHockey01.hpp"
#include "game/msw/CStageHockey04.hpp"
#include "game/msw/CStageHockey07.hpp"
#include "game/msw/CStageHockey17.hpp"
#include "game/msw/CStageHockeyCrushSkate85.hpp"
#include "game/msw/CStageHockeyCrushSkate87.hpp"
#include "game/msw/CStage10Base.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace msw {

    // INCOMPLETE: the hockey stages (HockeyF) that CStageFactoryHockey::Create
    // makes, declared only as far as Create needs them: the constructor and
    // the size (from the allocation there). Their members are not mapped;
    // each unit that writes one of these classes should move it to a header
    // of its own and map the padding.

    // The destructors are declared so that they are the key functions: this
    // header must not make a unit emit the vtables.

    // Stages 01, 07, 17, 85 and 87 have their own headers.

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

    // Stage 04 has its own header, game/msw/CStageHockey04.hpp.

    class CStageHockey05 : public CStageManHockey {
        public:
            CStageHockey05(bcs::CTask *pParent);
            virtual ~CStageHockey05();

        private:
            unsigned char m_Unk218[0x87C]; // size 0xA94
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

            // Of the running stage, which must be this one (used by
            // CComManHockeyGame; meanings guessed): the number of panels,
            // the panel after pPanel (the first for 0), and whether the
            // fever time is on.
            static long GetPanelMaxNum();
            static CStage10Base::CPanelInfoBase *GetNextPanel(const CStage10Base::CPanelInfoBase *pPanel);
            static int IsNowFeverTime();

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

    class CStageHockeyCrushSkate86 : public CStageManHockey {
        public:
            CStageHockeyCrushSkate86(bcs::CTask *pParent);
            virtual ~CStageHockeyCrushSkate86();

        private:
            unsigned char m_Unk218[0x400]; // size 0x618
    };

}

#endif /* !MSW_CSTAGEHOCKEY_HPP_ */
