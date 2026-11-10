#ifndef BCS_CSCNGROUP_HPP_
#define BCS_CSCNGROUP_HPP_

#include "bcs/CScnObj.hpp"

namespace bcs {

    // A scene object holding other scene objects. A DOL class; taken to be
    // built on bcs::CScnObj (its AppendChild keeps a count at 0x120, past
    // CScnObj's 0x118 bytes). Only the members used so far.
    class CScnGroup : public CScnObj {
        public:
            // Adds pObj to the group. (return type guessed: the DOL body
            // returns nothing the callers read)
            void AppendChild(CScnObj *pObj);
    };
}

#endif /* !BCS_CSCNGROUP_HPP_ */
