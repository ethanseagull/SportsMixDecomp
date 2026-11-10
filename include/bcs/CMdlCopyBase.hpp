#ifndef BCS_CMDLCOPYBASE_HPP_
#define BCS_CMDLCOPYBASE_HPP_

#include "bcs/CMdlObj.hpp"

namespace bcs {

    // Base of the models that copy the pose of another one (bcs::CMdlRef).
    // A DOL class. Only what the modules use so far is declared.
    class CMdlCopyBase : public CMdlObj {
        public:
            // (meaning of the second argument not known)
            void SetLink(const CMdlObj *pMdl, unsigned long);
    };
}

#endif /* !BCS_CMDLCOPYBASE_HPP_ */
