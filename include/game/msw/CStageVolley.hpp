#ifndef MSW_CSTAGEVOLLEY_HPP_
#define MSW_CSTAGEVOLLEY_HPP_

#include "game/msw/CStageVolley01.hpp"
#include "game/msw/CStageVolley02.hpp"
#include "game/msw/CStageVolley03.hpp"
#include "game/msw/CStageVolley05.hpp"
#include "game/msw/CStageVolley06.hpp"
#include "game/msw/CStageVolley09.hpp"
#include "game/msw/CStageVolley10.hpp"
#include "game/msw/CStageVolley11.hpp"
#include "game/msw/CStageVolley12.hpp"
#include "game/msw/CStageVolley15.hpp"
#include "game/msw/CStageVolley16.hpp"
#include "game/msw/CStageVolley17.hpp"
#include "game/msw/CStageVolley42.hpp"
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

    // Stage 01 (CStageManVolley, then CStage01Base at 0x200), stages 02,
    // 03, 05, 06, 09, 10, 11, 12, 15, 16, 17 and 42 have their own headers.

    class CStageVolley13 : public CStageManVolley {
        public:
            CStageVolley13(bcs::CTask *pParent);
            virtual ~CStageVolley13();

        private:
            unsigned char m_Unk200[0x54]; // size 0x254
    };

    // Stage 42 has its own header.
}

#endif /* !MSW_CSTAGEVOLLEY_HPP_ */
