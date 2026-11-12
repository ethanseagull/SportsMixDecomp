#ifndef REVOLUTION_GX_GXTEXTURE_H_
#define REVOLUTION_GX_GXTEXTURE_H_

// The texture object functions of GX. Only what the game's code calls so
// far, written from the calls; argument names are guesses.

#ifdef __cplusplus
extern "C" {
#endif

// Texture formats (only those used so far).
typedef enum _GXTexFmt {
    GX_TF_I8 = 1,
    GX_TF_RGB565 = 4,
    GX_TF_RGB5A3 = 5
} GXTexFmt;

typedef enum _GXTexWrapMode {
    GX_CLAMP,
    GX_REPEAT,
    GX_MIRROR
} GXTexWrapMode;

typedef enum _GXTexFilter {
    GX_NEAR,
    GX_LINEAR,
    GX_NEAR_MIP_NEAR,
    GX_LIN_MIP_NEAR,
    GX_NEAR_MIP_LIN,
    GX_LIN_MIP_LIN
} GXTexFilter;

typedef enum _GXAnisotropy {
    GX_ANISO_1,
    GX_ANISO_2,
    GX_ANISO_4
} GXAnisotropy;

// Size 0x20. Its contents are only touched by GX itself.
typedef struct _GXTexObj {
    unsigned long data[8];
} GXTexObj;

void GXInitTexObj(GXTexObj *pObj, void *pImage, unsigned short nWidth, unsigned short nHeight, GXTexFmt format,
                  GXTexWrapMode wrapS, GXTexWrapMode wrapT, unsigned char bMipmap);
// Added for the DOL's game/bcs/CMdlShadow.cpp: bytes of a texture image.
unsigned long GXGetTexBufferSize(unsigned short nWidth, unsigned short nHeight, unsigned long format,
                                 unsigned char bMipmap, unsigned char nMaxLod);
// Added for MenuF game/menu/CCharaName.cpp: the size of a texture object.
unsigned short GXGetTexObjWidth(const GXTexObj *pObj);
unsigned short GXGetTexObjHeight(const GXTexObj *pObj);
// Added for the DOL's game/msw/CGameExDemoBase.cpp.
void GXLoadTexObj(const GXTexObj *pObj, int map);

void GXInitTexObjLOD(GXTexObj *pObj, GXTexFilter minFilter, GXTexFilter magFilter, float fMinLod, float fMaxLod,
                     float fLodBias, unsigned char bBiasClamp, unsigned char bEdgeLod, GXAnisotropy maxAniso);

// Added for MenuF game/menu/CBgDisplay.cpp.
void *GXGetTexObjData(const GXTexObj *pObj);
unsigned char GXGetTexObjMipMap(const GXTexObj *pObj);
GXTexFmt GXGetTexObjFmt(const GXTexObj *pObj);
void GXInvalidateTexAll(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_GX_GXTEXTURE_H_ */
