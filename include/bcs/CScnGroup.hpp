#ifndef BCS_CSCNGROUP_HPP_
#define BCS_CSCNGROUP_HPP_

#include "bcs/CScnObj.hpp"

namespace bcs {

    // A scene object holding other scene objects. A DOL class; taken to be
    // built on bcs::CScnObj (its AppendChild keeps a count at 0x120, past
    // CScnObj's 0x118 bytes). Only the members used so far.
    class CScnGroup : public CScnObj {
        public:
            // (DOL; parameter meanings not known) Size 0x124 (VolleyF's
            // CC15_VOLLEY::CcSiromaMagicCircle allocates it with (0x10, 0)).
            CScnGroup(unsigned long, long);
            // (DOL) CScnObj's pure slot 0x10C, so that the group can be
            // created; what it does for a group is not known.
            virtual void UpdateFrame(float);
            // Adds pObj to the group. (return type guessed: the DOL body
            // returns nothing the callers read)
            void AppendChild(CScnObj *pObj);

        private:
            // 0x118 to the size of 0x124 (the count AppendChild keeps is at
            // 0x120).
            unsigned char m_Unk118[0x124 - 0x118];
    };
}

#endif /* !BCS_CSCNGROUP_HPP_ */
