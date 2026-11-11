// msw::CStageDodgeDOD55: Dodge or Die stage 55. It adds nothing to its base
// but a model table, two model flags, an audience flash on entering a scene
// and one stage sound.
#include "game/msw/CStageDodgeDOD55.hpp"

#include "game/msw/CStageMdlListNode.hpp"

// Model kinds of the stage's MDL_GEN_RULE table (names guessed).
enum {
    MDL_AUDIENCE = 0x1E, // "s<n>_ef_flash_<k>", "s<n>_guest01_<k>": flash with the audience
    MDL_FLAG = 0x1F      // "s<n>_flag_<k>": the audience's flags
};

// Sound slot and sound of the stage.
enum {
    SE_SLOT_AMBIENT = 2,
    SE_AMBIENT = 0xD9C
};

msw::CStageDodgeDOD55::CStageDodgeDOD55(bcs::CTask *pParent) : CStageManDodgeDodgeOrDie(pParent)
{
}

msw::CStageDodgeDOD55::~CStageDodgeDOD55()
{
}

void msw::CStageDodgeDOD55::Main_(float frame)
{
    CStageManDodgeDodgeOrDie::Main_(frame);
}

void msw::CStageDodgeDOD55::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManDodgeDodgeOrDie::SendMessage(msg, arg1, arg2, arg3);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageDodgeDOD55::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_ef_flash_[0-9]+$", MDL_AUDIENCE, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_guest01_[0-9]+$", MDL_AUDIENCE, 0, 0, CStageMan::NewMdl },
        { "^s[0-9]+_flag_([0-9]+)$", MDL_FLAG, 0, 0, CStageMan::NewMdl },
        { 0, reinterpret_cast<long>(CStageManDodgeDodgeOrDie::GetMdlGenRule_()) },
    };

    return tbl;
}

void msw::CStageDodgeDOD55::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                             CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                             long n)
{
    switch (pRule->m_nId) {
        case MDL_AUDIENCE:
            pNode->OnFlag(CStageMdlListNode::FLAG_4);
            break;
        case MDL_FLAG:
            pNode->OnFlag(CStageMdlListNode::FLAG_8);
            break;
    }
}

void msw::CStageDodgeDOD55::OnSceneEntry()
{
    PlayAnim_AudienceFlash(1, -1.0f);
    InitAnim_AudienceFlag();
}

void msw::CStageDodgeDOD55::OnSceneReentry()
{
    PlayAnim_AudienceFlash(1, -1.0f);
    InitAnim_AudienceFlag();
}

void msw::CStageDodgeDOD55::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
            SE_Play(SE_SLOT_AMBIENT, SE_AMBIENT, 0.0f, 3);
        }
    }
}
