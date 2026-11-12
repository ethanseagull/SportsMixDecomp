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
    // 0x08 to 0x3C named for MenuF game/menu/CBgDisplay.cpp (the copy
    // filter settings); names of the public GX API.
    unsigned short xfbHeight;          // 0x08
    unsigned short viXOrigin;          // 0x0A
    unsigned short viYOrigin;          // 0x0C
    unsigned short viWidth;            // 0x0E
    unsigned short viHeight;           // 0x10
    unsigned long xFBmode;             // 0x14
    unsigned char field_rendering;     // 0x18
    unsigned char aa;                  // 0x19
    unsigned char sample_pattern[12][2]; // 0x1A
    unsigned char vfilter[7];          // 0x32
} GXRenderModeObj;

// An RGBA colour as GX takes it (passed by value).
typedef struct _GXColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} GXColor;

// Added for MenuF game/menu/CBgDisplay.cpp (nw4r::lyt::Material::SetTevColor).
typedef struct _GXColorS10 {
    short r;
    short g;
    short b;
    short a;
} GXColorS10;

#endif /* !REVOLUTION_GX_GXSTRUCT_H_ */
