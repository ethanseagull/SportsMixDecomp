// msw::CStageDodge01: dodge stage 01. The file also emits msw::CStageMan's
// and msw::CStageManDodge's inline virtuals for CStageDodge01's vtable.
#include "game/msw/CStageDodge01.hpp"

#include "game/msw/CStageMdlListNode.hpp"

msw::CStageDodge01::CStageDodge01(bcs::CTask *pParent) : CStageManDodge(pParent), CStage01Base(this)
{
}

msw::CStageDodge01::~CStageDodge01()
{
}

void msw::CStageDodge01::Main_(float dt)
{
    CStageManDodge::Main_(dt);
    Stage01_Main(dt);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodge01::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ef_flash_[0-9]+$", 0x1E, 0, 0, NewMdl },
        { "^s[0-9]+_guest01_[0-9]+$", 0x1E, 0, 0, NewMdl },
        { "^s[0-9]+_flag_([0-9]+)$", 0x1F, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageManDodge::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageDodge01::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
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

void msw::CStageDodge01::SendMessage(long msg, long a, long type, long b)
{
    CStageManDodge::SendMessage(msg, a, type, b);

    switch (msg) {
        case 3:
        case 0x13:
            PlayAnim_AudienceFlash(1, -1.0f);
            InitAnim_AudienceFlag();
            break;
        case 0x3F:
            PlayAnim_AudienceFlash(2, 120.0f);
            break;
        case 0x42: {
            long teamA = GetTeamOf(a);
            long teamB = GetTeamOf(b);

            if (IsTeamHitType(type) && (teamB < 0 || teamB != teamA)) {
                PlayAnim_AudienceFlash(2, 120.0f);
            } else if (type == 5 && teamA != teamB) {
                PlayAnim_AudienceFlash(2, 120.0f);
            }
            break;
        }
    }
}

void msw::CStageDodge01::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            // The crowd loops in slot 0 once started.
            if (!SE_IsAlive(0)) {
                SE_Play(0, 0xD07, 0.0f, 3);
                SE_SetVolume(0, 1.0f, 0.0f);
            }
            break;
        case 4:
            SE_Play(4, 0xD09, 0.0f, 0);
            SE_SetVolume(4, 1.0f, 0.0f);
            break;
        case 5:
            SE_Play(4, 0xD0A, 0.0f, 0);
            SE_SetVolume(4, 1.0f, 0.0f);
            break;
        case 6:
            SE_Play(4, 0xD0A, 0.0f, 0);
            SE_SetVolume(4, 1.0f, 0.0f);
            break;
        case 2:
            SE_Play(4, 0xD08, 0.0f, 0);
            SE_SetVolume(4, 1.0f, 0.0f);
            break;
        case 3:
            SE_Play(4, 0xD09, 0.0f, 0);
            SE_SetVolume(4, 1.0f, 0.0f);
            break;
        case 1:
            SE_Play(4, 0xD08, 0.0f, 0);
            SE_SetVolume(4, 1.0f, 0.0f);
            break;
    }
}
