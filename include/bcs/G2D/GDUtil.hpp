#ifndef BCS_G2D_GDUTIL_HPP_
#define BCS_G2D_GDUTIL_HPP_

#include "revolution/gx/GXEnum.h"

// Free functions of the engine that write GX state into the current GD
// display list (added for DOL game/bcs/G2D/CLineStripDL.cpp). They live in
// the DOL with no symbols; names, namespace and argument names are guesses
// from what they write.

namespace bcs {
    namespace G2D {

        // (DOL 0x800A21B8) Two colour channels from the vertex colour, a
        // single TEV stage passing the rasterised colour, alpha test off,
        // no textures, and a vertex format of 2D float positions with a
        // direct RGBA8 colour.
        void SetGDColorVtxState();

        // (DOL 0x800A22E8) The blend mode, then the line width and point
        // size (in sixths of a pixel, as GDSetLPSize takes them).
        void SetGDLineState(unsigned long nLineWidth, unsigned long nPointSize, GXBlendMode type,
                            GXBlendFactor src, GXBlendFactor dst, GXLogicOp op);

        // (DOL 0x800A2350) The scissor box.
        void SetGDScissor(unsigned long nLeft, unsigned long nTop, unsigned long nWidth, unsigned long nHeight);
    }
}

#endif /* !BCS_G2D_GDUTIL_HPP_ */
