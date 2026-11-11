// msw::CStageHockey01: hockey stage 01. Same frame as BasketF's
// CStageBasket01 (the CStage01Base part, the table chained to the stage
// manager's); the background and the audience models are hockey's own.
#include "game/msw/CStageHockey01.hpp"

#include "bcs/CRegex.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CArn.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CStageMdl.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include <stdio.h>

msw::CStageHockey01::CStageHockey01(bcs::CTask *pParent) : CStageManHockey(pParent), CStage01Base(this)
{
    m_pBgNode = 0;
}

msw::CStageHockey01::~CStageHockey01()
{
}

// The background faces the goal the camera looks at.
void msw::CStageHockey01::Main_(float frame)
{
    CStageManHockey::Main_(frame);
    Stage01_Main(frame);

    bcs::CScnObj *pBg = m_pBgNode != 0 ? m_pBgNode->GetObj() : 0;
    if (pBg != 0) {
        float rot = 0.0f;
        CGameCamera *pCamera = CGameCamera::GetInstance(0);
        if (pCamera != 0 && pCamera->GetTargetGoalIdx() != 0) {
            rot = 3.1415927f;
        }
        pBg->SetRotY(rot);
        pBg->CalcRootMtx();
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey01::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManHockey.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^stage([0-9]+)_bg$", 0xA, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_ef_flash_[0-9]+$", 0x1E, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest02_[0-9]+$", 0x32, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest05_[0-9]+$", 0x33, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest01_([0-9]+)$", 0x3C, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest03_[0-9]+$", 0x3D, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_flag_([0-9]+)$", 0x1F, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()) },
    };

    return tbl;
}

// Flags the models of the table (what the flags mean is not known; 0x20
// and 0x40 are read by SwitchAudienceMdl_). Of the "guest01" models, those
// numbered below 5 get 0x60 as well.
void msw::CStageHockey01::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
{
    switch (pRule->m_nId) {
        case 0xA:
            m_pBgNode = pNode;
            break;
        case 0x1E:
            pNode->OnFlag(0x4);
            break;
        case 0x1F:
            pNode->OnFlag(0x8);
            break;
        case 0x32:
            pNode->OnFlag(0xA4);
            break;
        case 0x33:
            pNode->OnFlag(0xA0);
            break;
        case 0x3C: {
            long no;
            if (sscanf(rArn.GetName(n) + rRegex.GetSubMatchPos(1), "%d", &no) > 0 && no < 5) {
                pNode->OnFlag(0x60);
            }
            pNode->OnFlag(0x4);
            break;
        }
        case 0x3D:
            pNode->OnFlag(0x60);
            break;
    }
    CStageManHockey::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageHockey01::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManHockey::SendMessage(msg, arg1, arg2, arg3);
}

void msw::CStageHockey01::OnSceneEntry()
{
    SwitchAudienceMdl_(1);
}

// (what cameras 1 and 3 are is not known)
void msw::CStageHockey01::OnActiveCameraChange(long, long nCamera)
{
    switch (nCamera) {
        case 3:
            SwitchAudienceMdl_(1);
            break;
        case 1:
            break;
        default:
            SwitchAudienceMdl_(0);
            break;
    }
}

void msw::CStageHockey01::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(0)) {
                SE_Play(0, 0xD07, 0.0f, 3);
            }
            break;
        case 4:
            SE_Play(3, 0xD08, 0.0f, 0);
            break;
        case 1:
            SE_Play(3, 0xD09, 0.0f, 0);
            break;
        case 2:
            SE_Play(3, 0xD0A, 0.0f, 0);
            break;
        case 3:
            SE_Play(3, 0xD0B, 0.0f, 0);
            break;
    }
}

void msw::CStageHockey01::SwitchAudienceMdl_(int bOn)
{
    CMdlGroup *pGroup = GetMdlGroup(0);

    for (bcs::CListNodeBase *pIt = pGroup->GetList().GetFirst(); pIt != pGroup->GetList().GetEnd();
         bcs::CListNodeBase::Step(pIt)) {
        bcs::CListBase &rList = static_cast<CMdlGroup::CNode *>(pIt)->GetMdl()->GetNodeList()->GetList();

        for (bcs::CListNodeBase *pNodeIt = rList.GetFirst(); pNodeIt != rList.GetEnd();
             bcs::CListNodeBase::Step(pNodeIt)) {
            CStageMdlListNode *pNode = static_cast<CStageMdlListNode *>(pNodeIt);

            if ((pNode->GetFlag() & 0x20) == 0x20) {
                int bDisp = bOn;
                if ((pNode->GetFlag() & 0x40) != 0x40) {
                    bDisp = !bOn;
                }
                bcs::CScnObj *pObj = pNode->GetObj();
                if (pObj != 0) {
                    pObj->SetHide(!bDisp);
                }
            }
        }
    }
}

long msw::CStageHockey01::SE_GetSlotNum() const
{
    return 4;
}
