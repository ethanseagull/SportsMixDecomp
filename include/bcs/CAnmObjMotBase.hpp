#ifndef BCS_CANMOBJMOTBASE_HPP_
#define BCS_CANMOBJMOTBASE_HPP_

#include "bcs/CAnmObj.hpp"

namespace bcs {

    // (guessed to be a base of bcs::CAnmMotion) The skeletal animations. A
    // DOL class known from the types its nested names give the mangled
    // names of CAnmMotion and msw::CC_DATA. It declares nothing else here.
    class CAnmObjMotBase : public CAnmObj {
        public:
            // Per-node settings of a model's motion (contents not mapped).
            struct NODE_EX_INFO;
            // A trigger in a motion (contents not mapped).
            struct MOT_TRG;

        private:
            // 0x0C to 0x34 (bcs::CAnmMotion's members start at 0x34). Not
            // mapped.
            unsigned char m_Unk0C[0x28];
    };
}

#endif /* !BCS_CANMOBJMOTBASE_HPP_ */
