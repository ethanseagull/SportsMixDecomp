#ifndef NW4R_LYT_MATERIAL_HPP_
#define NW4R_LYT_MATERIAL_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/lyt/TexMap.hpp"
#include "nw4r/lyt/types.h"
#include "nw4r/ut/Color.hpp"
#include "revolution/gx/GXStruct.h"
#include "nw4r/math/types.h"

namespace nw4r {
    namespace lyt {
        class AnimationLink;
        class AnimResource;
        class AnimTransform;

        // Translation, rotation and scale of a material's texture. Size
        // 0x14 (the DOL's GetTexSRTAry steps by 0x1C over the TexMaps, then
        // by this). Member names follow the public ogws decompilation's
        // description (from memory, no code copied).
        struct TexSRT {
            // Added for MenuF game/menu/CBgDisplay.cpp, which emits it out of
            // line (__ct__Q34nw4r3lyt6TexSRTFv, an empty body).
            TexSRT() {}

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
                // 0x08 to 0x24 (ogws order), added for MenuF
                // game/menu/CBgDisplay.cpp, which calls the last one.
                virtual ~Material();
                virtual bool SetupGX(bool bModulate, unsigned char alpha);
                virtual void BindAnimation(AnimTransform *pAnimTrans);
                virtual void UnbindAnimation(AnimTransform *pAnimTrans);
                virtual void UnbindAllAnimation();
                virtual void Animate();
                virtual AnimationLink *FindAnimationLink(AnimTransform *pAnimTrans);
                virtual AnimationLink *FindAnimationLink(const AnimResource &rAnimRes);

                // Added for MenuF game/menu/CBgDisplay.cpp (out of line in
                // the DOL; the argument meanings follow the ogws description).
                void SetName(const char *pName);
                void ReserveGXMem(unsigned char texMapNum, unsigned char texSRTNum, unsigned char texCoordGenNum,
                                  unsigned char tevStageNum, bool allocTevSwap, unsigned char indStageNum,
                                  unsigned char indSRTNum, bool allocChanCtrl, bool allocMatCol,
                                  bool allocAlpComp, bool allocBlendMode);
                void SetTextureNum(unsigned char num);
                void SetTexCoordGenNum(unsigned char num);
                void SetTevStageNum(unsigned char num);
                const TexCoordGen *GetTexCoordGenAry() const;
                TexCoordGen *GetTexCoordGenAry();
                TevStage *GetTevStageAry();
                TevSwapMode *GetTevSwapAry();
                BlendMode *GetBlendModePtr();

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

                // Added for MenuF game/menu/CBgDisplay.cpp (inline, with
                // their assertion strings; file "material.h").
                // By value: the original returns the four bytes in r3.
                TexCoordGen GetTexCoordGen(unsigned long idx) const
                {
                    NW4R_ASSERT_AT((idx < mGXMemNum.texCoordGen), "material.h", 168, "NW4R:Failed assertion idx < mGXMemNum.texCoordGen");
                    return GetTexCoordGenAry()[idx];
                }

                void SetTexCoordGen(unsigned long idx, TexCoordGen value)
                {
                    NW4R_ASSERT_AT((idx < mGXMemNum.texCoordGen), "material.h", 174, "NW4R:Failed assertion idx < mGXMemNum.texCoordGen");
                    GetTexCoordGenAry()[idx] = value;
                }

                void SetTevStage(unsigned long idx, const TevStage &value)
                {
                    NW4R_ASSERT_AT((idx < mGXMemNum.tevStage), "material.h", 186, "NW4R:Failed assertion idx < mGXMemNum.tevStage");
                    GetTevStageAry()[idx] = value;
                }

                void SetTevColor(unsigned long idx, const GXColorS10 &value)
                {
                    NW4R_ASSERT_AT((idx < TevColorMax), "material.h", 143, "NW4R:Failed assertion idx < TevColorMax");
                    mTevCols[idx] = value;
                }

                void SetTevKColor(unsigned long idx, ut::Color value)
                {
                    NW4R_ASSERT_AT((idx < TevKColorMax), "material.h", 150, "NW4R:Failed assertion idx < TevKColorMax");
                    mTevKCols[idx] = value;
                }

                bool IsTevSwapCap() const { return mGXMemCap.tevSwap != 0; }
                bool IsBlendModeCap() const { return mGXMemCap.blendMode != 0; }

                void SetTevSwapMode(unsigned long idx, TevSwapMode value)
                {
                    NW4R_ASSERT_AT((IsTevSwapCap() && idx < 4), "material.h", 198, "NW4R:Failed assertion IsTevSwapCap() && idx < 4");
                    GetTevSwapAry()[idx] = value;
                }

                void SetBlendMode(BlendMode value)
                {
                    NW4R_ASSERT_AT(IsBlendModeCap(), "material.h", 270, "NW4R:Failed assertion IsBlendModeCap()");
                    *GetBlendModePtr() = value;
                }

                // (name guessed) How many textures the material has.
                unsigned char GetTextureNum() const { return mGXMemNum.texMap; }

            private:
                enum {
                    TevColorMax = 3,
                    TevKColorMax = 4
                };

                // Bit fields from the top bit down; tevSwap (bit 17 from
                // the top), tevStage (18 to 22) and blendMode (26) are what
                // the inline functions above read.
                struct MemNums {
                    unsigned long texMap : 4;
                    unsigned long texSRT : 4;
                    unsigned long texCoordGen : 4;
                    unsigned long indSRT : 2;
                    unsigned long indStage : 3;
                    unsigned long tevSwap : 1;
                    unsigned long tevStage : 5;
                    unsigned long chanCtrl : 1;
                    unsigned long matCol : 1;
                    unsigned long alpComp : 1;
                    unsigned long blendMode : 1;
                    unsigned long others : 5;
                };

                unsigned char m_Unk04[0xC];      // 0x04 (the vptr is at 0x00)
                GXColorS10 mTevCols[TevColorMax]; // 0x10
                ut::Color mTevKCols[TevKColorMax]; // 0x28
                MemNums mGXMemCap; // 0x38, capacity of each array
                MemNums mGXMemNum; // 0x3C, entries in use
                void *mpGXMem;     // 0x40
        };
    }
}

#endif /* !NW4R_LYT_MATERIAL_HPP_ */
