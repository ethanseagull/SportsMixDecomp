#ifndef REVOLUTION_GX_GXPIXEL_H_
#define REVOLUTION_GX_GXPIXEL_H_

#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far (MenuF game/menu/CTitle.cpp). Declarations of the
// public GX API; the bodies are in main.dol.
void GXSetZMode(GXBool compare_enable, GXCompare func, GXBool update_enable);
void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor, GXLogicOp op);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXPIXEL_H_ */
