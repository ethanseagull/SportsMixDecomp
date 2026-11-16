#ifndef NW4R_G3D_G3DSTATE_HPP_
#define NW4R_G3D_G3DSTATE_HPP_

#include "revolution/gx/GXStruct.h"

namespace nw4r {
    namespace g3d {
        class Camera;

        // Only what is used so far.
        namespace G3DState {
            // Marks the cached GX state in `flags` as unknown, so g3d sets it
            // again before its next draw.
            void Invalidate(unsigned long flags);
            // (added for msw::CEventModel.cpp) Loads the camera's matrices
            // into the g3d state. (parameter meanings not known)
            void SetCameraProjMtx(const Camera &rCamera, int, bool);
            // (DOL fn_804166F0; added for bcs::CWorldEnv) The render mode
            // g3d was given by SetRenderModeObj (the address of its copy).
            const GXRenderModeObj &GetRenderModeObj();
            // (added for the DOL's game/bcs/CMainApp.cpp) Gives g3d the
            // render mode (it keeps a copy).
            void SetRenderModeObj(const GXRenderModeObj &rRmode);
        }
    }
}

#endif /* !NW4R_G3D_G3DSTATE_HPP_ */
