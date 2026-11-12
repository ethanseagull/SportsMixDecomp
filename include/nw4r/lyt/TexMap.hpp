#ifndef NW4R_LYT_TEXMAP_HPP_
#define NW4R_LYT_TEXMAP_HPP_

#include "revolution/gx/GXTexture.h"

namespace nw4r {
    namespace lyt {

        // A texture as a layout material uses it. Lives in the DOL. Size
        // 0x1C, no vptr. The offsets and the bit field are what the DOL's
        // Set(const GXTexObj &) (fn_80428610) writes; member names follow
        // the public ogws decompilation's description of the class (from
        // memory, no code copied).
        class TexMap {
            public:
                // Added for MenuF game/menu/CBgDisplay.cpp: an empty texture
                // with linear filtering.
                TexMap() : mpImage(0), mWidth(0), mHeight(0), mMinLOD(0.0f), mMaxLOD(0.0f), mLODBias(0)
                {
                    mBits.textureFormat = 0;
                    mBits.mipmap = 0;
                    mBits.wrapS = 0;
                    mBits.wrapT = 0;
                    mBits.minFilter = 1;
                    mBits.magFilter = 1;
                    mBits.biasClampEnable = 0;
                    mBits.edgeLODEnable = 0;
                    mBits.anisotropy = 0;
                    SetNoPalette();
                }

                TexMap(const _GXTexObj &rTexObj)
                {
                    Set(rTexObj);
                    SetNoPalette();
                }

                void Set(const _GXTexObj &rTexObj);
                // Added for VolleyF game/volley/CCockpitChrIcon2DVolley.cpp
                // (Material::SetTexture): a member-wise copy.
                void Set(const TexMap &rTexMap) { *this = rTexMap; }

                // Added for MenuF game/menu/CBgDisplay.cpp (out of line in the DOL).
                void SetTexParam(void *pImage, unsigned short width, unsigned short height, GXTexFmt format,
                                 GXTexWrapMode wrapS, GXTexWrapMode wrapT, bool bMipmap);
                void SetLODParam(GXTexFilter minFilter, GXTexFilter magFilter, float minLOD, float maxLOD,
                                 float lodBias, bool bBiasClamp, bool bEdgeLOD, GXAnisotropy anisotropy);

                void *GetImage() const { return mpImage; }
                void SetImage(void *pImage) { mpImage = pImage; }

                void SetNoPalette()
                {
                    mBits.paletteFormat = 0;
                    mpPalette = 0;
                    mPaletteEntryNum = 0;
                }

            private:
                void *mpImage;                   // 0x00
                void *mpPalette;                 // 0x04
                unsigned short mWidth;           // 0x08
                unsigned short mHeight;          // 0x0A
                float mMinLOD;                   // 0x0C
                float mMaxLOD;                   // 0x10
                unsigned short mLODBias;         // 0x14, LOD bias * 100
                unsigned short mPaletteEntryNum; // 0x16
                struct {
                    unsigned long textureFormat : 4;
                    unsigned long mipmap : 1;
                    unsigned long wrapS : 2;
                    unsigned long wrapT : 2;
                    unsigned long minFilter : 3;
                    unsigned long magFilter : 1;
                    unsigned long biasClampEnable : 1;
                    unsigned long edgeLODEnable : 1;
                    unsigned long anisotropy : 2;
                    unsigned long paletteFormat : 2;
                } mBits;                         // 0x18
        };
    }
}

#endif /* !NW4R_LYT_TEXMAP_HPP_ */
