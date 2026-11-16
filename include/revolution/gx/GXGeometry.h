#ifndef REVOLUTION_GX_GXGEOMETRY_H_
#define REVOLUTION_GX_GXGEOMETRY_H_

#include "revolution/gx/GXEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

// The vertex and primitive functions of GX. Only what the game's code calls
// so far, written from the calls; argument names are guesses.

void GXClearVtxDesc(void);
void GXSetLineWidth(unsigned char width, GXTexOffset tex_offsets);
void GXSetCullMode(GXCullMode mode);
// fn_80296AE0 in main.dol (unnamed there).
void GXSetVtxDesc(GXAttr attr, GXAttrType type);
void GXSetVtxAttrFmt(GXVtxFmt fmt, GXAttr attr, GXCompCnt cnt, GXCompType type, unsigned char frac);
void GXSetNumTexGens(unsigned char nTexGens);
void GXSetTexCoordGen2(GXTexCoordID dst, GXTexGenType func, GXTexGenSrc src, unsigned long mtx, unsigned char normalize,
                       unsigned long ptMtx);

void GXBegin(GXPrimitive type, GXVtxFmt fmt, unsigned short nVerts);
// Nothing to do on the hardware; kept for the shape of the calls.
static inline void GXEnd(void) {}

// Added for MenuF game/menu/CBgDisplay.cpp.
void GXSetCoPlanar(GXBool enable);

// Added for the DOL's game/bcs/CMainApp.cpp (bcs::CMainApp::ResetGP).
void GXSetPointSize(unsigned char pointSize, GXTexOffset texOffsets);
void GXEnableTexOffsets(GXTexCoordID coord, GXBool line_enable, GXBool point_enable);
void GXSetTexCoordScaleManually(GXTexCoordID coord, GXBool enable, unsigned short ss, unsigned short ts);
void GXSetTexCoordCylWrap(GXTexCoordID coord, GXBool s_enable, GXBool t_enable);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXGEOMETRY_H_ */
