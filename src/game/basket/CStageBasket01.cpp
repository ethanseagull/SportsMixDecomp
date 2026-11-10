// msw::CStageBasket01: basket stage 01.
#include "game/msw/CStageBasket01.hpp"
#include "game/msw/CStageMdlListNode.hpp"

msw::CStageBasket01::CStageBasket01(bcs::CTask *pParent) : CStageManBasket(pParent), CStage01Base(this)
{
}

msw::CStageBasket01::~CStageBasket01()
{
}

void msw::CStageBasket01::Main_(float frame)
{
    CStageManBasket::Main_(frame);
    Stage01_Main(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageBasket01::GetMdlGenRule_() const
{
    // The end entry chains to the table of CStageManBasket.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ef_flash_[0-9]+$", 0x1E, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest01_[0-9]+$", 0x1E, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_flag_([0-9]+)$", 0x1F, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageManBasket::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageBasket01::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                           CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                           long n)
{
    switch (pRule->m_nId) {
        case 0x1E:
            pNode->OnFlag(CStageMdlListNode::FLAG_4);
            break;
        case 0x1F:
            pNode->OnFlag(CStageMdlListNode::FLAG_8);
            break;
    }
    CStageManBasket::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

void msw::CStageBasket01::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManBasket::SendMessage(msg, arg1, arg2, arg3);
}

void msw::CStageBasket01::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(0)) {
                SE_Play(0, 0xD07, 0.0f, 0);
            }
            break;
        case 3:
            SE_Play(3, 0xD08, 0.0f, 0);
            break;
        case 1:
            SE_PlayEx(3, 0xD09, 0, 1, -1, 0.0f, 1.0f / 3.0f, 0.0f);
            break;
        case 2:
            SE_PlayEx(3, 0xD0A, 0, 1, -1, 0.0f, 1.0f / 3.0f, 0.0f);
            break;
    }
}

long msw::CStageBasket01::SE_GetSlotNum() const
{
    return 4;
}
