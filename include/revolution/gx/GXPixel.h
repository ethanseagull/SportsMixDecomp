#ifndef REVOLUTION_GX_GXPIXEL_H_
#define REVOLUTION_GX_GXPIXEL_H_

#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

// The pixel engine functions of GX. Only what the game's code calls so far.

void GXPixModeSync(void);
void GXSetColorUpdate(GXBool update_enable);
void GXSetAlphaUpdate(GXBool update_enable);
// Only what is used so far (MenuF game/menu/CTitle.cpp). Declarations of the
// public GX API; the bodies are in main.dol.
void GXSetZMode(GXBool compare_enable, GXCompare func, GXBool update_enable);
void GXSetBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor, GXLogicOp op);

// Added for MenuF game/menu/CBgDisplay.cpp.
void GXSetFog(GXFogType type, float startz, float endz, float nearz, float farz, GXColor color);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXPIXEL_H_ */
