// msw::CStageBasket07: basket stage 07, the boss Packun stage. A boss Packun
// stands at each goal and throws mud bombs; the stage's parameter table
// (GetPrmTbl) hands the bosses their tuning values. HockeyF's CStageHockey07
// and DodgeF's CStageDodge07 are the same stage on their own stage managers.
#include "game/msw/CStageBasket07.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CNpcBossPackun.hpp"

namespace {

    // (names guessed) One entry of the parameter table that GetPrmTbl gives
    // the bosses: the type of a tuning value, its address, and the offset of
    // the word of the boss's parameters it fills. The table ends with a zero
    // entry.
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
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_PUSH_COL_R, 0x00 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_PUSH_COL_OFFSET_Z, 0x04 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_PUSH2_COL_R, 0x08 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_CHR_ATK_COL_R, 0x0C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_CHR_ATK_HEAD_COL_R, 0x10 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_CHR_CATCH_COL_R, 0x14 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_EAT_RATE, 0x18 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_OFFSET_Z, 0x1C },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_MELT_TIME_MIN, 0x20 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_MELT_TIME_MAX, 0x24 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_SLEEP_TIME_MIN, 0x28 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_SLEEP_TIME_MAX, 0x2C },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_MAX_DIR, 0x30 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_MAX_DIR_LIP, 0x34 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_LIP_SCALE, 0x38 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_STATE_SLEEP_NUM, 0x3C },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_STATE_MUD_NUM, 0x40 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_BOUND_RANG_DEG, 0x44 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_BOUND_ANGLE_MIN, 0x48 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_BOUND_ANGLE_MAX, 0x4C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_BOUND_DIST_MIN, 0x50 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_BOUND_DIST_MAX, 0x54 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_REBOUND_FRM_MIN, 0x58 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_REBOUND_FRM_MAX, 0x5C },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_REBOUND_FRM_ATTENUATION, 0x60 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_BOUND_HEIGHT_MIN, 0x64 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_BOUND_HEIGHT_MAX, 0x68 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_REBOUND_HEIGHT_ATTENUATION, 0x6C },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_BLT_NUM, 0x70 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_SCALE, 0x74 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_BLT_APPEND_DIR, 0x78 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_DIST_MIN, 0x7C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_DIST_MAX, 0x80 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_HEIGHT, 0x84 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_TIME, 0x88 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_TIME_RND, 0x8C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_FALL_SPEED, 0x90 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_BLT_LIFE, 0x94 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_EXPLODE_TIME, 0x98 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_PACKUN_EXPLODE_SIZE, 0x9C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_NEAR_RANG, 0xA0 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_FAR_RANG, 0xA4 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_PACKUN_ANGLE_ALPHA, 0xA8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL_IDX, 0xAC },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE0, 0xB0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE1, 0xB4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE2, 0xB8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE3, 0xBC },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE4, 0xC0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE5, 0xC4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE6, 0xC8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE7, 0xCC },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE8, 0xD0 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_LIP_TBL0_STATE9, 0xD4 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_MUD_TBL_IDX, 0xD8 },
        { PRM_TYPE_LONG, &BASKET_PRM_S07_MUD_TBL_ROTATE_ENABLE, 0xDC },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_SCALE0, 0xE0 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_POS_X0, 0xE4 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_POS_Z0, 0xE8 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_SCALE1, 0xEC },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_POS_X1, 0xF0 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_POS_Z1, 0xF4 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_SCALE2, 0xF8 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_POS_X2, 0xFC },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL0_POS_Z2, 0x100 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_SCALE0, 0x104 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_POS_X0, 0x108 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_POS_Z0, 0x10C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_SCALE1, 0x110 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_POS_X1, 0x114 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_POS_Z1, 0x118 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_SCALE2, 0x11C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_POS_X2, 0x120 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL1_POS_Z2, 0x124 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_SCALE0, 0x128 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_POS_X0, 0x12C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_POS_Z0, 0x130 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_SCALE1, 0x134 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_POS_X1, 0x138 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_POS_Z1, 0x13C },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_SCALE2, 0x140 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_POS_X2, 0x144 },
        { PRM_TYPE_FLOAT, &BASKET_PRM_S07_MUD_TBL2_POS_Z2, 0x148 },
        { PRM_TYPE_END, 0, 0 },
    };
}

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

const void *msw::CStageBasket07::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageBasket07::CStageBasket07(bcs::CTask *pParent) : CStageManBasket(pParent)
{
    for (int i = 0; i < 4; i++) {
        m_apNpc[i] = 0;
    }
    for (int i = 0; i < 4; i++) {
        m_apGoal[i] = 0;
    }
}

msw::CStageBasket07::~CStageBasket07()
{
}

void msw::CStageBasket07::Main_(float frame)
{
    CStageManBasket::Main_(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket07::GetMdlGenRule_() const
{
    // Only the end entry, which chains to the table of CStageManBasket.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

bool msw::CStageBasket07::OnPreCreateMdl(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, nw4r::g3d::ResFile, CArn &, long)
{
    return true;
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

void msw::CStageBasket07::OnGimmickReset()
{
    CancelMudBombs();
}

void msw::CStageBasket07::OnGimmickCancel()
{
    CancelMudBombs();
}

void msw::CStageBasket07::OnGimmickStop()
{
    CancelMudBombs();
}

// The two goals, each with a boss Packun standing where the goal is.
void msw::CStageBasket07::CreateGoal()
{
    for (long i = 0; i < 2; i++) {
        CGoalObjBasket_BossPackun *pGoal = new CGoalObjBasket_BossPackun(this, i, 0);

        if (pGoal != 0) {
            pGoal->Create();
            pGoal->SetDefaultOffset(GetGoalOffsetPos());
            pGoal->SetPosAuto();
            pGoal->SetRotAuto();
            pGoal->SceneEntry();
            m_apGoal[i] = pGoal;

            bcs::FVEC3 pos = GetGoalPos(i);
            const bcs::FVEC3 &rRot = GetGoalRot(i); // not used

            m_apNpc[i] = static_cast<CChrManBasket *>(CChrMan::GetInstance())->CreateBossPackun_(pos, i);
        } else if (pGoal != 0) {
            // Never reached. The original has this guarded delete in the
            // failure branch (most likely a clean-up macro); it is kept for
            // the code it produces.
            delete pGoal;
        }
    }
}

void msw::CStageBasket07::OnSoundEvent(long event)
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
            SE_Play(3, 0xD48, 0.0f, 0);
            break;
    }
}

CGoalObj *msw::CStageBasket07::GetGoal(long goal)
{
    return m_apGoal[goal];
}

const CGoalObj *msw::CStageBasket07::GetGoal(long goal) const
{
    return m_apGoal[goal];
}
