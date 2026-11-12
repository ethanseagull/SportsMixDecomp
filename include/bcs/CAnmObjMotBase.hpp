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

    // (field names guessed) The settings of one node of a model's motion;
    // the motion takes an array of them. BasketF's goal model
    // (game/basket/CGoalMdlBasket.cpp) gives tables of node, a kind (0 to
    // 3) and a flag that is 0 for the root and 1 for the others.
    struct CAnmObjMotBase::NODE_EX_INFO {
        unsigned short nNode;   // 0x00
        unsigned char nKind;    // 0x02
        unsigned char nFlag;    // 0x03
        unsigned short nUnk04;  // 0x04, 0 in every table seen
    };

    // (field names guessed) A trigger in a motion, as msw::CChrCore's
    // CallbackMotTrg_ reads it: the kind of trigger at 0x04 (it switches on
    // it) and two parameters at 0x06 and 0x08.
    struct CAnmObjMotBase::MOT_TRG {
        unsigned char m_Unk00[0x4];
        unsigned char m_nType;  // 0x04
        unsigned char m_Unk05;
        short m_nParam0;        // 0x06
        short m_nParam1;        // 0x08
        // 0x0A-0x0F: not known. The size, 0x10, is from BasketF's
        // CChrCoreBasketNinja, which clears a whole trigger on the stack.
        unsigned char m_Unk0A[0x10 - 0xA];
    };
}

#endif /* !BCS_CANMOBJMOTBASE_HPP_ */
