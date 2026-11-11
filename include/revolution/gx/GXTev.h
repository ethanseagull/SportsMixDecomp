#ifndef REVOLUTION_GX_GXTEV_H_
#define REVOLUTION_GX_GXTEV_H_

#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

// The texture environment functions of GX. Only what the game's code calls
// so far, written from the calls; argument names are guesses.

void GXSetNumTevStages(unsigned char nStages);
void GXSetNumIndStages(unsigned char nStages);
void GXSetTevDirect(GXTevStageID stage);
void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color);
void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d);
void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d);
void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, unsigned char clamp,
                     GXTevRegID out);
void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, unsigned char clamp,
                     GXTevRegID out);
void GXSetTevColor(GXTevRegID reg, GXColor color);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXTEV_H_ */
