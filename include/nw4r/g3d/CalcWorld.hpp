#ifndef NW4R_G3D_CALCWORLD_HPP_
#define NW4R_G3D_CALCWORLD_HPP_

#include "nw4r/g3d/ResMdl.hpp"
#include "nw4r/math/types.h"

namespace nw4r {
    namespace g3d {

        // (DOL fn_80417C90, unnamed, in g3d_calcworld; name and parameter
        // meanings guessed from the public ogws decompilation's layout of
        // that file) Runs the envelope byte code pByteCode of mdl over the
        // node matrices pMtxArray and their attributes pAttribArray.
        // Added for DOL game/bcs/CMdlRef.cpp.
        void CalcSkinning(math::MTX34 *pMtxArray, unsigned long *pAttribArray, ResMdl mdl, const unsigned char *pByteCode);
    }
}

#endif /* !NW4R_G3D_CALCWORLD_HPP_ */
