#ifndef MSW_CSTAGEDODGE_HPP_
#define MSW_CSTAGEDODGE_HPP_

#include "game/msw/CStageDodge01.hpp"
#include "game/msw/CStageDodge02.hpp"
#include "game/msw/CStageDodge03.hpp"
#include "game/msw/CStageDodge04.hpp"
#include "game/msw/CStageDodge05.hpp"
#include "game/msw/CStageDodge07.hpp"
#include "game/msw/CStageDodge09.hpp"
#include "game/msw/CStageDodge11.hpp"
#include "game/msw/CStageDodge12.hpp"
#include "game/msw/CStageDodge15.hpp"
#include "game/msw/CStageDodge16.hpp"
#include "game/msw/CStageDodge17.hpp"
#include "game/msw/CStageDodgeDOD55.hpp"
#include "game/msw/CStageDodgeDOD56.hpp"
#include "game/msw/CStageDodgeDOD57.hpp"
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

    // Stage 03 has its own header, game/msw/CStageDodge03.hpp.

    // Stage 04 has its own header, game/msw/CStageDodge04.hpp.

    // Stage 05 has its own header, game/msw/CStageDodge05.hpp.

    // Stage 07 has its own header, game/msw/CStageDodge07.hpp.

    // Stage 09 has its own header, game/msw/CStageDodge09.hpp.

    // Stage 11 has its own header, game/msw/CStageDodge11.hpp.

    // Stage 12 has its own header, game/msw/CStageDodge12.hpp.

    class CStageDodge13 : public CStageManDodge {
        public:
            CStageDodge13(bcs::CTask *pParent);
            virtual ~CStageDodge13();

        private:
            unsigned char m_Unk244[0x54]; // size 0x298
    };

    // Stage 15 has its own header, game/msw/CStageDodge15.hpp.

    // Stage 16 has its own header, game/msw/CStageDodge16.hpp.

    // Stage 17 has its own header, game/msw/CStageDodge17.hpp.

    // The DOD stages (55-57). Stage 55 has its own header,
    // game/msw/CStageDodgeDOD55.hpp.

    // Stage 56 has its own header, game/msw/CStageDodgeDOD56.hpp.

    // Stage 57 has its own header, game/msw/CStageDodgeDOD57.hpp.
}

#endif /* !MSW_CSTAGEDODGE_HPP_ */
