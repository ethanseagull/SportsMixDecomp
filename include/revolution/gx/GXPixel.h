#ifndef REVOLUTION_GX_GXPIXEL_H_
#define REVOLUTION_GX_GXPIXEL_H_

#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

// The pixel engine functions of GX. Only what the game's code calls so far.

void GXSetBlendMode(GXBlendMode type, GXBlendFactor src, GXBlendFactor dst, GXLogicOp op);
void GXPixModeSync(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXPIXEL_H_ */
