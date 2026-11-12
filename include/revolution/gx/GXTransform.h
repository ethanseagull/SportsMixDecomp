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

// Added for MenuF game/menu/CBgDisplay.cpp.
void GXSetProjection(const Mtx44 mtx, GXProjectionType type);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXTRANSFORM_H_ */
