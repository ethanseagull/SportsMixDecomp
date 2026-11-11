#ifndef REVOLUTION_GX_GXTEV_H_
#define REVOLUTION_GX_GXTEV_H_

#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far (MenuF game/menu/CTitle.cpp). Declarations of the
// public GX API; the bodies are in main.dol.
void GXSetNumTevStages(unsigned char nStages);
void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color);
void GXSetTevOp(GXTevStageID id, GXTevMode mode);
void GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras_sel, GXTevSwapSel tex_sel);
void GXSetTevSwapModeTable(GXTevSwapSel table, GXTevColorChan red, GXTevColorChan green, GXTevColorChan blue,
                           GXTevColorChan alpha);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXTEV_H_ */
