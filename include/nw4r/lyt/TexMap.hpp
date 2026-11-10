#ifndef NW4R_LYT_TEXMAP_HPP_
#define NW4R_LYT_TEXMAP_HPP_

// The SDK's texture object (GXTexObj). Only named here.
struct _GXTexObj;

namespace nw4r {
    namespace lyt {

        // A texture as a layout material uses it. Lives in the DOL. Size
        // 0x1C, no vptr. The offsets and the bit field are what the DOL's
        // Set(const GXTexObj &) (fn_80428610) writes; member names follow
        // the public ogws decompilation's description of the class (from
        // memory, no code copied).
        class TexMap {
            public:
                TexMap(const _GXTexObj &rTexObj)
                {
                    Set(rTexObj);
                    SetNoPalette();
                }

                void Set(const _GXTexObj &rTexObj);

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
