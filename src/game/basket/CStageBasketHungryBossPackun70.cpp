// msw::CStageBasketHungryBossPackun70: Hungry Boss Packun stage 70 of the
// basket game.
#include "game/msw/CStageBasketHungryBossPackun70.hpp"
#include "bcs/BCS_Math.hpp"
#include "game/msw/BasketPrm.hpp"

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl gives
    // the boss: a tuning value and the offset of the word it fills. All
    // entries here have type 1, a long; the table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG
    };

    struct PRM_TBL {
        long m_nType;    // 0x00
        long *m_pValue;  // 0x04
        long m_nOffset;  // 0x08
    };

    static const PRM_TBL s_PrmTbl[] = {
        // For each of the three states, ten tables and their indices.
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL0_IDX, 0x00 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL0, 0x04 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL1_IDX, 0x08 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL1, 0x0C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL2_IDX, 0x10 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL2, 0x14 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL3_IDX, 0x18 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL3, 0x1C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL4_IDX, 0x20 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL4, 0x24 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL5_IDX, 0x28 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL5, 0x2C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL6_IDX, 0x30 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL6, 0x34 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL7_IDX, 0x38 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL7, 0x3C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL8_IDX, 0x40 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL8, 0x44 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL9_IDX, 0x48 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE0_TBL9, 0x4C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL0_IDX, 0x50 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL0, 0x54 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL1_IDX, 0x58 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL1, 0x5C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL2_IDX, 0x60 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL2, 0x64 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL3_IDX, 0x68 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL3, 0x6C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL4_IDX, 0x70 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL4, 0x74 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL5_IDX, 0x78 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL5, 0x7C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL6_IDX, 0x80 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL6, 0x84 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL7_IDX, 0x88 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL7, 0x8C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL8_IDX, 0x90 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL8, 0x94 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL9_IDX, 0x98 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE1_TBL9, 0x9C },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL0_IDX, 0xA0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL0, 0xA4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL1_IDX, 0xA8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL1, 0xAC },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL2_IDX, 0xB0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL2, 0xB4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL3_IDX, 0xB8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL3, 0xBC },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL4_IDX, 0xC0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL4, 0xC4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL5_IDX, 0xC8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL5, 0xCC },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL6_IDX, 0xD0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL6, 0xD4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL7_IDX, 0xD8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL7, 0xDC },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL8_IDX, 0xE0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL8, 0xE4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL9_IDX, 0xE8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_STATE2_TBL9, 0xEC },
        { PRM_TYPE_LONG, &BASKET_PRM_S70_DELIVERY_PATTERN, 0xF0 },
        { PRM_TYPE_END, 0, 0 },
    };
}

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

const void *msw::CStageBasketHungryBossPackun70::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasketHungryBossPackun70::CStageBasketHungryBossPackun70(bcs::CTask *pParent)
    : CStageBasketHungryBossPackun(pParent)
{
}

msw::CStageBasketHungryBossPackun70::~CStageBasketHungryBossPackun70()
{
}

void msw::CStageBasketHungryBossPackun70::OnSoundEvent(long event)
{
    if (event == 0 && !SE_IsAlive(2)) {
        SE_Play(0, 0xDA7, 0.0f, 3);
    }
}
