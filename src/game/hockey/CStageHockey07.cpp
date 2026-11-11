// msw::CStageHockey07: hockey stage 07, the boss Packun stage. The boss
// stands beside the court and throws mud bombs; the stage's parameter table
// (GetPrmTbl) hands it its tuning values. BasketF's CStageBasket07 and
// DodgeF's CStageDodge07 are the same stage on their own stage managers.
#include "game/msw/CStageHockey07.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrManHockey.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace {

    // (names guessed) One entry of the parameter table that GetPrmTbl gives
    // the boss: a tuning value, its type, and the offset of the word of the
    // boss's parameters it fills. The table ends with a zero entry. BasketF's
    // CStageBasketHungryBossPackun70 has a table of the same shape.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };
}

namespace msw {

    static const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SCALE, 0x00 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_COLL_RAD, 0x04 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_LOCATE_X, 0x08 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SPEED_MAX, 0x0C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_ROT_WAIT_TIME, 0x10 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_ROT_TIME, 0x14 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MOVE_DIST, 0x18 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MOVE_WIN_RATE, 0x1C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MOVE_LOSE_RATE, 0x20 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MOVE_NOMAL_RATE, 0x24 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MOVE_BALL_RATE, 0x28 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SEARCH_DIST, 0x2C },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S07_PACKUN_SEARCH_CHR, 0x30 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SEARCH_NO_TIME, 0x34 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_WAIT_TIME_MIN, 0x38 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_WAIT_TIME_MAX, 0x3C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SLEEP_RATE, 0x40 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SLEEP_TIME_MIN, 0x44 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_SLEEP_TIME_MAX, 0x48 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_ATK_Z, 0x4C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_ATK_X, 0x50 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_NUM, 0x54 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_MAX, 0x58 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_START_POS_Y, 0x64 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_START_POS_Z, 0x68 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_ROT_MAX, 0x6C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_SEARCH_ROT, 0x70 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_SEARCH_ROT_MAX, 0x74 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_BLT_TIME, 0x78 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_BLT_TIME_RND, 0x7C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_SCL_MAX, 0x5C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_SCL_MIN, 0x60 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_MAD_TIME, 0x80 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S07_PACKUN_EXPLODE_SIZE, 0x84 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_NEAR_RANG, 0x88 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_FAR_RANG, 0x8C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_PACKUN_ANGLE_ALPHA, 0x90 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_GOAL_X1_LENGTH, 0x94 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_SCALE_MIN0, 0x98 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_SCALE_MAX0, 0x9C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_X_MIN0, 0xA0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_X_MAX0, 0xA4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MIN0, 0xA8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MAX0, 0xAC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_SCALE_MIN1, 0xB0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_SCALE_MAX1, 0xB4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_X_MIN1, 0xB8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_X_MAX1, 0xBC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MIN1, 0xC0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MAX1, 0xC4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_SCALE_MIN2, 0xC8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_SCALE_MAX2, 0xCC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_X_MIN2, 0xD0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_X_MAX2, 0xD4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MIN2, 0xD8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL0_POS_Z_MAX2, 0xDC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_SCALE_MIN0, 0xE0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_SCALE_MAX0, 0xE4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_X_MIN0, 0xE8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_X_MAX0, 0xEC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MIN0, 0xF0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MAX0, 0xF4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_SCALE_MIN1, 0xF8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_SCALE_MAX1, 0xFC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_X_MIN1, 0x100 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_X_MAX1, 0x104 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MIN1, 0x108 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MAX1, 0x10C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_SCALE_MIN2, 0x110 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_SCALE_MAX2, 0x114 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_X_MIN2, 0x118 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_X_MAX2, 0x11C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MIN2, 0x120 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL1_POS_Z_MAX2, 0x124 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_SCALE_MIN0, 0x128 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_SCALE_MAX0, 0x12C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_X_MIN0, 0x130 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_X_MAX0, 0x134 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MIN0, 0x138 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MAX0, 0x13C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_SCALE_MIN1, 0x140 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_SCALE_MAX1, 0x144 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_X_MIN1, 0x148 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_X_MAX1, 0x14C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MIN1, 0x150 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MAX1, 0x154 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_SCALE_MIN2, 0x158 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_SCALE_MAX2, 0x15C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_X_MIN2, 0x160 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_X_MAX2, 0x164 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MIN2, 0x168 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S07_MUD_TBL2_POS_Z_MAX2, 0x16C },
        { PRM_TYPE_END, 0, 0 },
    };
}

const void *msw::CStageHockey07::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey07::CStageHockey07(bcs::CTask *pParent) : CStageManHockey(pParent)
{
    m_pPackun = 0;
    m_Unk21C = 0;
    m_Unk220 = 0;
    m_Unk224 = 0;
}

msw::CStageHockey07::~CStageHockey07()
{
}

void msw::CStageHockey07::Main_(float frame)
{
    CStageManHockey::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey07::GetMdlGenRule_() const
{
    // Only the end entry, which chains to the table of CStageManHockey.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

// The goals, then the boss Packun at its place beside the court.
void msw::CStageHockey07::CreateGoal()
{
    CStageManHockey::CreateGoal();

    bcs::FVEC3 pos(HOCKEY_PRM_S07_PACKUN_LOCATE_X, 0.0f, HOCKEY_PRM_S07_PACKUN_LOCATE_Z);
    m_pPackun = CChrManHockey::GetInstance()->CreateBossPackun_(pos, 0);
}

float msw::CStageHockey07::GetGoalX1Length(long) const
{
    return HOCKEY_PRM_S07_GOAL_X1_LENGTH;
}

// Cancels every mud bomb that is not being deleted. The next bomb is taken
// before the call, so a bomb may leave the list when it is cancelled.
static inline void CancelMudBombs()
{
    msw::CNpcBossPackun::CMudBombMan *pMan = msw::CNpcBossPackun::CMudBombMan::GetInstance();
    if (pMan == 0) {
        return;
    }

    bcs::CList<msw::CNpcBossPackun::CMudBomb> &rList = pMan->GetList();
    msw::CNpcBossPackun::CMudBomb *pBomb = rList.GetFirst();
    while (pBomb != 0) {
        msw::CNpcBossPackun::CMudBomb *pNext = rList.GetNext(pBomb);
        if (!pBomb->IsDelete()) {
            pBomb->Cancel();
        }
        pBomb = pNext;
    }
}

void msw::CStageHockey07::OnGimmickReset()
{
    CancelMudBombs();
}

void msw::CStageHockey07::OnGimmickCancel()
{
    CancelMudBombs();
}

void msw::CStageHockey07::OnGimmickStop()
{
    CancelMudBombs();
}

void msw::CStageHockey07::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(1)) {
                SE_RepeatPlay(1, 0xD46, 180.0f, 420.0f, -1.0f, 0.0f, 0xC);
            }
            if (!SE_IsAlive(2)) {
                SE_Play(2, 0xD47, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            SE_Play(3, 0xD48, 0.0f, 0);
            break;
    }
}
