#ifndef REVOLUTION_GX_GXSTRUCT_H_
#define REVOLUTION_GX_GXSTRUCT_H_

// Only what is used so far. Written for this project from the offsets the
// game's code reads (bcs::CMainApp::g_Rmode in BasketF
// game/basket/CCockpitBasketHarapeko.cpp); field names follow the public
// description of the SDK's render mode object.
typedef struct _GXRenderModeObj {
    unsigned long viTVmode;   // 0x00
    unsigned short fbWidth;   // 0x04
    unsigned short efbHeight; // 0x06
    unsigned char m_Unk08[0x34];
} GXRenderModeObj;

#endif /* !REVOLUTION_GX_GXSTRUCT_H_ */
