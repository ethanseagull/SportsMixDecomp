#ifndef MSW_CSTAGEDODGE_HPP_
#define MSW_CSTAGEDODGE_HPP_

#include "game/msw/CStageDodge01.hpp"
#include "game/msw/CStageDodge02.hpp"
#include "game/msw/CStageDodge04.hpp"
#include "game/msw/CStageDodge05.hpp"
#include "game/msw/CStageDodge07.hpp"
#include "game/msw/CStageDodge12.hpp"
#include "game/msw/CStageDodge16.hpp"
#include "game/msw/CStageManDodge.hpp"

namespace msw {

    // INCOMPLETE: the dodge stages (DodgeF) that CStageFactoryDodge::Create
    // makes, declared only as far as Create needs them: the constructor and
    // the size (from the allocation there). Their members are not mapped;
    // each unit that writes one of these classes should move it to a header
    // of its own and map the padding.
    //
    // The destructors are declared so that they are the key functions: this
    // header must not make a unit emit the vtables.

    // Stage 01 (CStageManDodge, then CStage01Base at 0x244) has its own
    // header, game/msw/CStageDodge01.hpp.

    // Stage 02 has its own header, game/msw/CStageDodge02.hpp.

    class CStageDodge03 : public CStageManDodge {
        public:
            CStageDodge03(bcs::CTask *pParent);
            virtual ~CStageDodge03();

        private:
            unsigned char m_Unk244[0x88]; // size 0x2CC
    };

    // Stage 04 has its own header, game/msw/CStageDodge04.hpp.

    // Stage 05 has its own header, game/msw/CStageDodge05.hpp.

    // Stage 07 has its own header, game/msw/CStageDodge07.hpp.

    class CStageDodge09 : public CStageManDodge {
        public:
            CStageDodge09(bcs::CTask *pParent);
            virtual ~CStageDodge09();

        private:
            unsigned char m_Unk244[0x24]; // size 0x268
    };

    class CStageDodge11 : public CStageManDodge {
        public:
            CStageDodge11(bcs::CTask *pParent);
            virtual ~CStageDodge11();

        private:
            unsigned char m_Unk244[0x438]; // size 0x67C
    };

    // Stage 12 has its own header, game/msw/CStageDodge12.hpp.

    class CStageDodge13 : public CStageManDodge {
        public:
            CStageDodge13(bcs::CTask *pParent);
            virtual ~CStageDodge13();

        private:
            unsigned char m_Unk244[0x54]; // size 0x298
    };

    class CStageDodge15 : public CStageManDodge {
        public:
            CStageDodge15(bcs::CTask *pParent);
            virtual ~CStageDodge15();

        private:
            unsigned char m_Unk244[0x8C]; // size 0x2D0
    };

    // Stage 16 has its own header, game/msw/CStageDodge16.hpp.

    class CStageDodge17 : public CStageManDodge {
        public:
            CStageDodge17(bcs::CTask *pParent);
            virtual ~CStageDodge17();

            // Static: CEventDemoDodge::CameraEvent leaves r3 unset. Stage
            // 0x11 (GameWork::GetStage) is the one with the trolley.
            static void TrolleyForceReset();

        private:
            unsigned char m_Unk244[0x5C]; // size 0x2A0
    };

    // The DOD stages (55-57); 55 adds nothing to CStageManDodge.
    class CStageDodgeDOD55 : public CStageManDodge {
        public:
            CStageDodgeDOD55(bcs::CTask *pParent);
            virtual ~CStageDodgeDOD55();
    };

    class CStageDodgeDOD56 : public CStageManDodge {
        public:
            CStageDodgeDOD56(bcs::CTask *pParent);
            virtual ~CStageDodgeDOD56();

        private:
            unsigned char m_Unk244[0x8C]; // size 0x2D0
    };

    class CStageDodgeDOD57 : public CStageManDodge {
        public:
            CStageDodgeDOD57(bcs::CTask *pParent);
            virtual ~CStageDodgeDOD57();

        private:
            unsigned char m_Unk244[0x5C]; // size 0x2A0
    };
}

#endif /* !MSW_CSTAGEDODGE_HPP_ */
