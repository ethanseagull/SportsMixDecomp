// msw::CStageDodge04: dodge stage 04, the stage with the cart and the
// pylons. The pylons are a CStgGmk_PylonCluster_Dodge member; the cart is
// handled by the second base class, CStage04Base. HockeyF has the same
// stage as CStageHockey04.
#include "game/msw/CStageDodge04.hpp"

#include "bcs/CMdlObj.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMdlListNode.hpp"
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
        long m_nType;    // 0x00
        void *m_pValue;  // 0x04
        long m_nOffset;  // 0x08
    };

    const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_MDL_RADIUS, 0x00 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_MDL_HEIGHT, 0x04 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_RADIUS, 0x08 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_HEIGHT, 0x0C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_LIFE, 0x10 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_FIELD_MARGIN, 0x14 },
        { PRM_TYPE_LONG, &msw::DODGE_PRM_S04_PYLON_PUSH_POWER, 0x18 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_PUSH_RESIST, 0x1C },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_ACC, 0x20 },
        { PRM_TYPE_FLOAT, &msw::DODGE_PRM_S04_PYLON_DEC, 0x24 },
        { PRM_TYPE_LONG, &msw::DODGE_PRM_S04_PYLON_LOC_PAT_TYPE, 0x28 },
        { PRM_TYPE_LONG, &msw::DODGE_PRM_S04_PYLON_LOC_PAT_NO, 0x2C },
        { PRM_TYPE_END, 0, 0 },
    };
}

const void *msw::CStageDodge04::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageDodge04::CStageDodge04(bcs::CTask *pParent)
    : CStageManDodge(pParent), CStage04Base(this), m_pBgNode(0), m_PylonCluster(this)
{
}

msw::CStageDodge04::~CStageDodge04()
{
}

void msw::CStageDodge04::Main_(float frame)
{
    CStageManDodge::Main_(frame);
    m_PylonCluster.Main(frame);
    Stage04_Main(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge04::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg$", 0x0A, 0, 0, NewMdl },
        { "^s[0-9]+_ef_flash_[0-9]+$", 0x1E, 0, 0, NewMdl },
        { "^s[0-9]+_flag_([0-9]+)$", 0x1F, 0, 0, NewMdl },
        { "^s[0-9]+_cart$", 0x32, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManDodge::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodge04::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
                                          nw4r::g3d::ResFile &, CArn &, long)
{
    switch (pRule->m_nId) {
        case 0x0A:
            m_pBgNode = pNode;
            break;
        case 0x1E:
            pNode->OnFlag(CStageMdlListNode::FLAG_4);
            break;
        case 0x1F:
            pNode->OnFlag(CStageMdlListNode::FLAG_8);
            break;
        case 0x32:
            if (pNode->GetObj() != 0) {
                Stage04_RegistCart(static_cast<bcs::CMdlObj *>(pNode->GetObj()), 4);
            }
            break;
    }
}

void msw::CStageDodge04::OnSceneEntry()
{
    static const long nNum = 3;

    bcs::CScnObj *pObj = m_pBgNode != 0 ? m_pBgNode->GetObj() : 0;
    if (pObj != 0) {
        pObj->SetRotY(3.14159265f / 2.0f);

        // The background stands at -14, 0 or 14 in x, by m_nUnk430.
        long n = CGame::g_GameWork.m_nUnk430 % nNum - 1;
        long side = n > 1 ? 1 : (n < -1 ? -1 : n);
        pObj->m_vTrans.x = 14.0f * side;
        pObj->CalcRootMtx();
    }

    m_PylonCluster.CreateGmk();
}

void msw::CStageDodge04::OnSceneLeave()
{
}

void msw::CStageDodge04::OnTurnInit()
{
    m_PylonCluster.Reset();
}

void msw::CStageDodge04::OnGimmickReset()
{
    m_PylonCluster.InactivateAll();
}

void msw::CStageDodge04::OnGimmickStop()
{
    m_PylonCluster.InactivateAll();
}

void msw::CStageDodge04::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(1)) {
                SE_RepeatPlay(1, 0xD22, 180.0f, 420.0f, -1.0f, 0.0f, 12);
            }
            if (!SE_IsAlive(2)) {
                SE_Play(2, 0xD1B, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            SE_Play(3, 0xD21, 0.0f, 0);
            break;
    }
}

long msw::CStageDodge04::GetPylonMaxNum()
{
    CStageDodge04 *pStage = static_cast<CStageDodge04 *>(CStageMan::GetInstance());
    return pStage->m_PylonCluster.GetMaxNum();
}

const msw::CStgGmk_Pylon *msw::CStageDodge04::GetPylon(long idx)
{
    const CStageDodge04 *pStage = static_cast<const CStageDodge04 *>(CStageMan::GetInstance());
    if (idx < 0 || idx >= pStage->m_PylonCluster.GetMaxNum()) {
        return 0;
    }

    const CStgGmk_Pylon *pPylon = pStage->m_PylonCluster.GetPylon(idx);
    if (pPylon == 0 || !pPylon->IsActive()) {
        return 0;
    }
    return pPylon;
}

void msw::CStageDodge04::Disp()
{
    CStageManDodge::Disp();
}

long msw::CStageDodge04::SE_GetSlotNum() const
{
    return 9;
}

unsigned long msw::CStageDodge04::OnCalcSum() const
{
    return m_PylonCluster.CalcSum();
}

const msw::CStageMan *msw::CStageDodge04::GetStageMan() const
{
    return this;
}
