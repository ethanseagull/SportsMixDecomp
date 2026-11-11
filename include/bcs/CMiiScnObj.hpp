#ifndef BCS_CMIISCNOBJ_HPP_
#define BCS_CMIISCNOBJ_HPP_

#include "bcs/CScnObj.hpp"

namespace bcs {

    // (class name guessed; added for DOL game/bcs/CMiiMan.cpp) The DOL's
    // scene object that draws a Mii (its functions start at fn_80065F94,
    // the constructor calls CScnObj's). It holds an RFLStoreData at 0x284
    // and an RFL character model at 0x124. Only what is used so far.
    class CMiiScnObj : public CScnObj {
        public:
            // (name and return type guessed, DOL fn_80066488) Builds the
            // character model from the stored Mii. bcs::CMiiMan calls it
            // from its Update for every object queued with
            // CMiiMan::RequestCharModel.
            int InitCharModel();
    };
}

#endif /* !BCS_CMIISCNOBJ_HPP_ */
