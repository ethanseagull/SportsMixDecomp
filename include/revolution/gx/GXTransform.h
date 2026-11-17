#ifndef REVOLUTION_GX_GXTRANSFORM_H_
#define REVOLUTION_GX_GXTRANSFORM_H_

#include "revolution/gx/GXEnum.h"
#include "revolution/mtx.h"

#ifdef __cplusplus
extern "C" {
#endif

// The matrix functions of GX. Only what the game's code calls so far.

void GXLoadPosMtxImm(const Mtx mtx, unsigned long id);
void GXLoadTexMtxImm(const float mtx[][4], unsigned long id, GXTexMtxType type);
void GXSetCurrentMtx(unsigned long id);
void GXSetProjection(const Mtx44 mtx, GXProjectionType type);

// Added for MenuF game/menu/CBgDisplay.cpp.
void GXSetProjection(const Mtx44 mtx, GXProjectionType type);

// Added for the DOL's game/bcs/CMainApp.cpp. (GXSetViewport itself has no
// name in the DOL yet: fn_8029C7C0.)
void GXSetClipMode(GXClipMode mode);
void GXSetScissor(unsigned long left, unsigned long top, unsigned long wd, unsigned long ht);
void GXSetViewportJitter(float left, float top, float wd, float ht, float nearz, float farz, unsigned long field);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXTRANSFORM_H_ */
