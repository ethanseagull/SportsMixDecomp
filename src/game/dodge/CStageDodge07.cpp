// msw::CStageDodge07: dodge stage 07, the boss Packun stage. The boss is a
// stage gimmick (CStgGmk_BossPackun_Dodge) made after the stage is set up;
// it stands beside the court and throws mud bombs, which the gimmick hooks
// cancel. The stage's parameter table (GetPrmTbl) hands the boss its tuning
// values. HockeyF's CStageHockey07 and BasketF's CStageBasket07 are the
// same stage in the other modes.
#include "game/msw/CStageDodge07.hpp"

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CNpcBossPackun.hpp"
#include "game/msw/CStgGmk_BossPackun_Dodge.hpp"
#include "game/msw/DodgePrm.hpp"

namespace {

    // (names guessed) One entry of the parameter table GetPrmTbl gives: the
    // type of a tuning value, the value, and the offset of the word it
    // fills. The table ends with a zero entry.
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

    const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_SCALE, 0x00 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_COLL_SIZE_XZ, 0x04 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_COLL_SIZE_Y, 0x08 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_TURN_SPEED, 0x0C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_TGT_TURN_SPEED, 0x10 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_CLOSE_STANDBY_TIME, 0x14 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_CLOSE_STANDBY_TIME_RND, 0x18 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_CLOSE_PASSCATCH_FRM, 0x1C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_OPEN_STANDBY_TIME, 0x20 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_OPEN_STANDBY_TIME_RND, 0x24 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_OPEN_PASSCATCH_FRM, 0x28 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_OPEN_PASSCATCH_FRM_RND, 0x2C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_SHOOT_N_VEL, 0x30 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_SHOOT_TRACE_ANGLE, 0x34 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_SHOOT_TRACE_PITCH, 0x38 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_BLT_NUM, 0x3C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_BLT_HEIGHT, 0x40 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_BLT_TIME, 0x44 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_BLT_TIME_RND, 0x48 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_EXPLODE_TIME, 0x4C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_EXPLODE_DELAY, 0x50 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_EXPLODE_MDL_SIZE, 0x54 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S07_PACKUN_EXPLODE_SIZE, 0x58 },
        { PRM_TYPE_END, 0, 0 },
    };
}

const void *msw::CStageDodge07::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge07::CStageDodge07(bcs::CTask *pParent) : CStageManDodge(pParent)
{
    m_pBoss = 0;
}

msw::CStageDodge07::~CStageDodge07()
{
    if (m_pBoss != 0) {
        m_pBoss->Delete();
    }
    m_pBoss = 0;
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge07::GetMdlGenRule_() const
{
    // Only the end entry, which chains to CStageMan's table (not
    // CStageManDodge's).
    static STAGE::MDL_GEN_RULE tbl[] = {
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

// The boss, at its place beside the court.
void msw::CStageDodge07::OnPostInitialize()
{
    CStageManDodge::OnPostInitialize();

    CStgGmk_BossPackun_Dodge *pBoss = new CStgGmk_BossPackun_Dodge(this, 0);
    if (pBoss != 0) {
        pBoss->Create();
        pBoss->SetDefaultPos(bcs::FVEC3(6.05f, 0.0f, 0.0f));
        pBoss->SceneEntry();
        m_pBoss = pBoss;
    }
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

void msw::CStageDodge07::OnGimmickReset()
{
    if (m_pBoss != 0) {
        m_pBoss->Reset();
    }
    CancelMudBombs();
}

void msw::CStageDodge07::OnGimmickCancel()
{
    if (m_pBoss != 0) {
        m_pBoss->Reset();
    }
    CancelMudBombs();
}

void msw::CStageDodge07::OnGimmickStop()
{
    if (m_pBoss != 0) {
        m_pBoss->Reset();
    }
    CancelMudBombs();
}

// The boss's sum, the bomb count, and the bit patterns of every bomb's
// position.
unsigned long msw::CStageDodge07::OnCalcSum() const
{
    unsigned long sum = 0;
    if (m_pBoss != 0) {
        sum = m_pBoss->CalcSum();
    }

    CNpcBossPackun::CMudBombMan *pMan = CNpcBossPackun::CMudBombMan::GetInstance();
    if (pMan != 0) {
        sum += pMan->GetNum();

        bcs::CList<CNpcBossPackun::CMudBomb> &rList = pMan->GetList();
        CNpcBossPackun::CMudBomb *pBomb = rList.GetFirst();
        while (pBomb != 0) {
            bcs::FVEC3 pos = pBomb->GetPos();
            const unsigned long *pWords = reinterpret_cast<const unsigned long *>(&pos);
            sum += pWords[0];
            sum += pWords[1];
            sum += pWords[2];
            pBomb = rList.GetNext(pBomb);
        }
    }

    return sum;
}

// A pass from an infield player of type 1 goes to the boss, at height 2.
bool msw::CStageDodge07::GetPassToPos(bcs::FVEC3 *pPos, long nChr, long type) const
{
    CChrCore *pChr = nChr >= 0 ? CChrMan::GetChrCore(nChr) : 0;
    if (pChr == 0 || !pChr->IsDodgeInfield()) {
        return false;
    }

    if (pPos != 0 && m_pBoss != 0 && type == 1) {
        *pPos = m_pBoss->GetDefaultPos();
        pPos->y = 2.0f;
        return true;
    }
    return false;
}

void msw::CStageDodge07::OnSoundEvent(long event)
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
        case 5:
        case 6:
            SE_Play(3, 0xD48, 0.0f, 0);
            break;
    }
}

msw::CStgGmk_BossPackun_Dodge *msw::CStageDodge07::GetBossPackun()
{
    return static_cast<CStageDodge07 *>(CStageMan::GetInstance())->m_pBoss;
}

void msw::CStageDodge07::Disp()
{
    CStageManDodge::Disp();
}
