#ifndef REVOLUTION_GX_GXTEV_H_
#define REVOLUTION_GX_GXTEV_H_

#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXStruct.h"
#include "revolution/gx/GXTexture.h"

#ifdef __cplusplus
extern "C" {
#endif

// The texture environment functions of GX. Only what the game's code calls
// so far, written from the calls; argument names are guesses.

void GXSetNumIndStages(unsigned char nStages);
void GXSetTevDirect(GXTevStageID stage);
void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d);
void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d);
void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, unsigned char clamp,
                     GXTevRegID out);
void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, unsigned char clamp,
                     GXTevRegID out);
void GXSetTevColor(GXTevRegID reg, GXColor color);
// Only what is used so far (MenuF game/menu/CTitle.cpp). Declarations of the
// public GX API; the bodies are in main.dol.
void GXSetNumTevStages(unsigned char nStages);
void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color);
void GXSetTevOp(GXTevStageID id, GXTevMode mode);
void GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras_sel, GXTevSwapSel tex_sel);
void GXSetTevSwapModeTable(GXTevSwapSel table, GXTevColorChan red, GXTevColorChan green, GXTevColorChan blue,
                           GXTevColorChan alpha);

// Added for MenuF game/menu/CBgDisplay.cpp.
void GXSetAlphaCompare(GXCompare comp0, unsigned char ref0, GXAlphaOp op, GXCompare comp1, unsigned char ref1);
void GXSetZTexture(GXZTexOp op, GXTexFmt fmt, unsigned long bias);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXTEV_H_ */
