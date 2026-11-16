#ifndef BCS_CMDLCOPY_HPP_
#define BCS_CMDLCOPY_HPP_

#include "bcs/CMdlCopyBase.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // (added for HockeyF game/hockey/CcNinjaCloneBody.cpp) A model drawn
    // with the pose of another (SetLink). A DOL class, allocated with 0x1D8
    // bytes; only the constructor the modules use is declared.
    class CMdlCopy : public CMdlCopyBase {
        public:
            // (meanings of the last three arguments not known; the hockey
            // ninja passes 0 for all)
            CMdlCopy(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, unsigned long, unsigned long, long);
            virtual ~CMdlCopy();
    };
}

#endif /* !BCS_CMDLCOPY_HPP_ */
