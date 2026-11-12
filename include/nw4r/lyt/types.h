#ifndef NW4R_LYT_TYPES_H_
#define NW4R_LYT_TYPES_H_

#include "nw4r/db/Panic.hpp"
#include "revolution/gx/GXEnum.h"
#include "revolution/gx/GXTexture.h"

// The small value types of a layout material. The name of this file is part
// of the binary: the setters pass "types.h" to their assertions (MenuF
// game/menu/CBgDisplay.cpp emits them out of line, with their @STRING@
// objects; the line numbers are what the original passes). Written for this
// project from that code; the names follow the public ogws decompilation's
// description of nw4r::lyt (from memory, no code copied).

namespace nw4r {
    namespace lyt {

        // How a texture coordinate is generated. Four bytes.
        struct TexCoordGen {
            TexCoordGen() {}
            TexCoordGen(GXTexGenType aTexGenType, GXTexGenSrc aTexGenSrc, unsigned long aTexMtx) : reserve(0)
            {
                Set(aTexGenType, aTexGenSrc, aTexMtx);
            }

            void Set(GXTexGenType aTexGenType, GXTexGenSrc aTexGenSrc, unsigned long aTexMtx)
            {
                texGenType = aTexGenType;
                texGenSrc = aTexGenSrc;
                texMtx = aTexMtx;
            }

            unsigned char texGenType; // 0x00
            unsigned char texGenSrc;  // 0x01
            unsigned char texMtx;     // 0x02
            unsigned char reserve;    // 0x03
        };

        // The inputs and the operation of one half (colour or alpha) of a
        // TEV stage, packed into four bytes.
        struct TevStageInOp {
            void SetIn(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
            {
                NW4R_ASSERT_AT((a <= 0xF), "types.h", 348, "NW4R:Failed assertion a <= 0xF");
                NW4R_ASSERT_AT((b <= 0xF), "types.h", 349, "NW4R:Failed assertion b <= 0xF");
                NW4R_ASSERT_AT((c <= 0xF), "types.h", 350, "NW4R:Failed assertion c <= 0xF");
                NW4R_ASSERT_AT((d <= 0xF), "types.h", 351, "NW4R:Failed assertion d <= 0xF");
                ab = (unsigned char)(a | (b << 4));
                cd = (unsigned char)(c | (d << 4));
            }

            void SetOp(unsigned char aOp, unsigned char bias, unsigned char scale, bool clamp,
                       unsigned char outReg, unsigned char kSel)
            {
                NW4R_ASSERT_AT((aOp <= 0xF), "types.h", 372, "NW4R:Failed assertion aOp <= 0xF");
                NW4R_ASSERT_AT((bias <= 0x3), "types.h", 373, "NW4R:Failed assertion bias <= 0x3");
                NW4R_ASSERT_AT((scale <= 0x3), "types.h", 374, "NW4R:Failed assertion scale <= 0x3");
                NW4R_ASSERT_AT((outReg <= 0x3), "types.h", 375, "NW4R:Failed assertion outReg <= 0x3");
                NW4R_ASSERT_AT((kSel <= 0x1F), "types.h", 376, "NW4R:Failed assertion kSel <= 0x1F");
                op = (unsigned char)(aOp | (bias << 4) | (scale << 6));
                cl = (unsigned char)(clamp | (outReg << 1) | (kSel << 3));
            }

            unsigned char ab; // 0x00, inputs a and b
            unsigned char cd; // 0x01, inputs c and d
            unsigned char op; // 0x02, operation, bias and scale
            unsigned char cl; // 0x03, clamp, output register and constant
        };

        // One TEV stage of a material. Size 0x10.
        struct TevStage {
            TevStage()
            {
                SetOrder(GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0, GX_TEV_SWAP0, GX_TEV_SWAP0);
                SetColorIn(GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
                SetAlphaIn(GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
                SetColorOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KCSEL_K0);
                SetAlphaOp(GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV, GX_TEV_KASEL_K0_R);
                SetIndirect(GX_INDTEXSTAGE0, GX_ITF_8, GX_ITB_NONE, GX_ITM_OFF, GX_ITW_OFF, GX_ITW_OFF, false, false,
                            GX_ITBA_OFF);
            }

            void SetOrder(GXTexCoordID aTexCoordGen, GXTexMapID aTexMap, GXChannelID aColChan, GXTevSwapSel rasSel,
                          GXTevSwapSel texSel)
            {
                NW4R_ASSERT_AT((0 <= aTexMap && aTexMap <= 0x1FF), "types.h", 429, "NW4R:Failed assertion 0 <= aTexMap && aTexMap <= 0x1FF");
                NW4R_ASSERT_AT((0 <= rasSel && rasSel <= 0x3), "types.h", 430, "NW4R:Failed assertion 0 <= rasSel && rasSel <= 0x3");
                NW4R_ASSERT_AT((0 <= texSel && texSel <= 0x3), "types.h", 431, "NW4R:Failed assertion 0 <= texSel && texSel <= 0x3");
                texCoordGen = aTexCoordGen;
                colChan = aColChan;
                texMap = aTexMap;
                swapSel = (unsigned char)((aTexMap >> 8) | (rasSel << 1) | (texSel << 3));
            }

            void SetColorIn(GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d)
            {
                colorInOp.SetIn(a, b, c, d);
            }

            void SetAlphaIn(GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d)
            {
                alphaInOp.SetIn(a, b, c, d);
            }

            void SetColorOp(GXTevOp op, GXTevBias bias, GXTevScale scale, bool clamp, GXTevRegID outReg,
                            GXTevKColorSel kSel)
            {
                colorInOp.SetOp(op, bias, scale, clamp, outReg, kSel);
            }

            void SetAlphaOp(GXTevOp op, GXTevBias bias, GXTevScale scale, bool clamp, GXTevRegID outReg,
                            GXTevKAlphaSel kSel)
            {
                alphaInOp.SetOp(op, bias, scale, clamp, outReg, kSel);
            }

            void SetIndirect(GXIndTexStageID texStage, GXIndTexFormat format, GXIndTexBiasSel biasSel,
                             GXIndTexMtxID mtxSel, GXIndTexWrap wrapS, GXIndTexWrap wrapT, bool addPrev,
                             bool utcLod, GXIndTexAlphaSel alphaSel)
            {
                NW4R_ASSERT_AT((format <= 0x3), "types.h", 499, "NW4R:Failed assertion format <= 0x3");
                NW4R_ASSERT_AT((biasSel <= 0x7), "types.h", 500, "NW4R:Failed assertion biasSel <= 0x7");
                NW4R_ASSERT_AT((mtxSel <= 0xF), "types.h", 501, "NW4R:Failed assertion mtxSel <= 0xF");
                NW4R_ASSERT_AT((wrapS <= 0x7), "types.h", 502, "NW4R:Failed assertion wrapS <= 0x7");
                NW4R_ASSERT_AT((wrapT <= 0x7), "types.h", 503, "NW4R:Failed assertion wrapT <= 0x7");
                NW4R_ASSERT_AT((alphaSel <= 0x3), "types.h", 504, "NW4R:Failed assertion alphaSel <= 0x3");
                indStage = texStage;
                indBiMt = (unsigned char)(biasSel | (mtxSel << 3));
                indWrap = (unsigned char)(wrapS | (wrapT << 3));
                indFoAdUtAl = (unsigned char)(format | (addPrev << 2) | (utcLod << 3) | (alphaSel << 4));
            }

            unsigned char texCoordGen;  // 0x00
            unsigned char colChan;      // 0x01
            unsigned char texMap;       // 0x02, low byte of the map
            unsigned char swapSel;      // 0x03, map bit 8, rasSel, texSel
            TevStageInOp colorInOp;     // 0x04
            TevStageInOp alphaInOp;     // 0x08
            unsigned char indStage;     // 0x0C
            unsigned char indBiMt;      // 0x0D
            unsigned char indWrap;      // 0x0E
            unsigned char indFoAdUtAl;  // 0x0F
        };

        // The channel swap of one swap table entry. One byte.
        struct TevSwapMode {
            TevSwapMode() {}
            TevSwapMode(GXTevColorChan r, GXTevColorChan g, GXTevColorChan b, GXTevColorChan a)
            {
                Set(r, g, b, a);
            }

            void Set(GXTevColorChan r, GXTevColorChan g, GXTevColorChan b, GXTevColorChan a)
            {
                NW4R_ASSERT_AT((GX_CH_RED <= r && r <= GX_CH_ALPHA), "types.h", 601, "NW4R:Failed assertion GX_CH_RED <= r && r <= GX_CH_ALPHA");
                NW4R_ASSERT_AT((GX_CH_RED <= g && g <= GX_CH_ALPHA), "types.h", 602, "NW4R:Failed assertion GX_CH_RED <= g && g <= GX_CH_ALPHA");
                NW4R_ASSERT_AT((GX_CH_RED <= b && b <= GX_CH_ALPHA), "types.h", 603, "NW4R:Failed assertion GX_CH_RED <= b && b <= GX_CH_ALPHA");
                NW4R_ASSERT_AT((GX_CH_RED <= a && a <= GX_CH_ALPHA), "types.h", 604, "NW4R:Failed assertion GX_CH_RED <= a && a <= GX_CH_ALPHA");
                swap = (unsigned char)(r | (g << 2) | (b << 4) | (a << 6));
            }

            unsigned char swap; // 0x00
        };

        // How a material is blended into the frame. Four bytes.
        struct BlendMode {
            BlendMode() {}
            BlendMode(GXBlendMode aType, GXBlendFactor aSrcFactor, GXBlendFactor aDstFactor, GXLogicOp aOp)
            {
                Set(aType, aSrcFactor, aDstFactor, aOp);
            }

            void Set(GXBlendMode aType, GXBlendFactor aSrcFactor, GXBlendFactor aDstFactor, GXLogicOp aOp)
            {
                type = aType;
                srcFactor = aSrcFactor;
                dstFactor = aDstFactor;
                op = aOp;
            }

            unsigned char type;      // 0x00
            unsigned char srcFactor; // 0x01
            unsigned char dstFactor; // 0x02
            unsigned char op;        // 0x03
        };
    }
}

#endif /* !NW4R_LYT_TYPES_H_ */
