#ifndef NW4R_G3D_G3DSTATE_HPP_
#define NW4R_G3D_G3DSTATE_HPP_

namespace nw4r {
    namespace g3d {
        // Only what is used so far.
        namespace G3DState {
            // Marks the cached GX state in `flags` as unknown, so g3d sets it
            // again before its next draw.
            void Invalidate(unsigned long flags);
        }
    }
}

#endif /* !NW4R_G3D_G3DSTATE_HPP_ */
