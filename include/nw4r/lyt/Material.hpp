#ifndef NW4R_LYT_MATERIAL_HPP_
#define NW4R_LYT_MATERIAL_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/lyt/TexMap.hpp"
#include "nw4r/math/types.h"

namespace nw4r {
    namespace lyt {

        // Translation, rotation and scale of a material's texture. Size
        // 0x14 (the DOL's GetTexSRTAry steps by 0x1C over the TexMaps, then
        // by this). Member names follow the public ogws decompilation's
        // description (from memory, no code copied).
        struct TexSRT {
            math::VEC2 translate; // 0x00
            float rotate;         // 0x08
            math::VEC2 scale;     // 0x0C
        };

        // The material of a pane. Lives in the DOL. Only what the game's
        // code needs so far (VolleyF game/volley/CCockpitChrIcon2DVolley.cpp,
        // inline in its SetIconFace, and the DOL's GetTexMapAry /
        // GetTexSRTAry): the counts at 0x38 and 0x3C and the pointer to the
        // TexMap / TexSRT arrays at 0x40. Opaque otherwise, so it must not
        // be instantiated. Names follow the public ogws decompilation's
        // description of the class (from memory, no code copied).
        class Material {
            public:
                TexMap *GetTexMapAry();
                const TexSRT *GetTexSRTAry() const;
                TexSRT *GetTexSRTAry();

                // Inline (the game's code holds their assertion strings as
                // @STRING@ objects, file "material.h").
                void SetTexture(unsigned char texMapIdx, const TexMap &rTexMap)
                {
                    if (!(texMapIdx < mGXMemNum.texMap)) {
                        db::Panic("material.h", 136, "NW4R:Failed assertion texMapIdx < mGXMemNum.texMap");
                    }
                    GetTexMapAry()[texMapIdx].Set(rTexMap);
                }

                const TexSRT &GetTexSRT(unsigned long texSRTIdx) const
                {
                    if (!(texSRTIdx < mGXMemNum.texSRT)) {
                        db::Panic("material.h", 156, "NW4R:Failed assertion texSRTIdx < mGXMemNum.texSRT");
                    }
                    return GetTexSRTAry()[texSRTIdx];
                }

                void SetTexSRT(unsigned long texSRTIdx, const TexSRT &rTexSRT)
                {
                    if (!(texSRTIdx < mGXMemNum.texSRT)) {
                        db::Panic("material.h", 162, "NW4R:Failed assertion texSRTIdx < mGXMemNum.texSRT");
                    }
                    GetTexSRTAry()[texSRTIdx] = rTexSRT;
                }

                // (name guessed) How many textures the material has.
                unsigned char GetTextureNum() const { return mGXMemNum.texMap; }

            private:
                struct MemNums {
                    unsigned long texMap : 4;
                    unsigned long texSRT : 4;
                    unsigned long others : 24;
                };

                unsigned char m_Unk00[0x38];
                MemNums mGXMemCap; // 0x38, capacity of each array
                MemNums mGXMemNum; // 0x3C, entries in use
                void *mpGXMem;     // 0x40
        };
    }
}

#endif /* !NW4R_LYT_MATERIAL_HPP_ */
