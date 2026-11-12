#ifndef REVOLUTION_GX_GXFRAMEBUF_H_
#define REVOLUTION_GX_GXFRAMEBUF_H_

#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"
#include "revolution/gx/GXTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

// Copies out of the embedded frame buffer. Added for MenuF
// game/menu/CBgDisplay.cpp; signatures of the public GX API.
void GXSetCopyClear(GXColor clear_clr, unsigned long clear_z);
void GXSetTexCopySrc(unsigned short left, unsigned short top, unsigned short wd, unsigned short ht);
void GXSetTexCopyDst(unsigned short wd, unsigned short ht, GXTexFmt fmt, GXBool mipmap);
void GXSetCopyFilter(GXBool aa, const unsigned char sample_pattern[12][2], GXBool vf, const unsigned char vfilter[7]);
void GXCopyTex(void *dest, GXBool clear);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXFRAMEBUF_H_ */
