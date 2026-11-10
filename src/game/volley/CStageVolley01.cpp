// msw::CStageVolley01: volley stage 01. Same frame as DodgeF's
// CStageDodge01 (the CStage01Base part, the table chained to CStageMan's);
// the goal placement and the sounds are volley's own.
#include "game/msw/CStageVolley01.hpp"

#include "bcs/FVEC3.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/CStageMdlListNode.hpp"

msw::CStageVolley01::CStageVolley01(bcs::CTask *pParent) : CStageManVolley(pParent), CStage01Base(this)
{
}

msw::CStageVolley01::~CStageVolley01()
{
}

void msw::CStageVolley01::Main_(float frame)
{
    CStageMan::Main_(frame);
    Stage01_Main(frame);
}

// Both goals stand 4.5 from their default place and are placed again.
void msw::CStageVolley01::OnPostInitialize()
{
    CStageManVolley::OnPostInitialize();

    for (long i = 0; i < 2; i++) {
        CGoalObj *pGoal = GetGoal(i);
        if (pGoal != 0) {
            pGoal->SetDefaultOffset(bcs::FVEC3(0.0f, 0.0f, 4.5f));
            pGoal->SetPosAuto();
        }
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley01::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageMan.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ef_flash_[0-9]+$", 0x1E, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest01_[0-9]+$", 0x1E, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_flag_([0-9]+)$", 0x1F, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageMan::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageVolley01::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
                                          nw4r::g3d::ResFile &, CArn &, long)
{
    switch (pRule->m_nId) {
        case 0x1E:
            pNode->OnFlag(CStageMdlListNode::FLAG_4);
            break;
        case 0x1F:
            pNode->OnFlag(CStageMdlListNode::FLAG_8);
            break;
    }
}

void msw::CStageVolley01::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManVolley::SendMessage(msg, arg1, arg2, arg3);
}

void msw::CStageVolley01::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            // The crowd loops in slot 0 once started.
            if (!SE_IsAlive(0)) {
                SE_Play(0, 0xD07, 0.0f, 3);
            }
            break;
        case 1:
            SE_Play(3, 0xD09, 0.0f, 0);
            break;
        case 2:
            SE_Play(3, 0xD0A, 0.0f, 0);
            break;
        case 3:
            SE_Play(3, 0xD08, 0.0f, 0);
            break;
        case 4:
            SE_Play(3, 0xD08, 0.0f, 0);
            break;
        case 5:
            SE_Play(3, 0xD0A, 0.0f, 0);
            break;
    }
}

long msw::CStageVolley01::SE_GetSlotNum() const
{
    return 4;
}
