// msw::SelectMenu::CCharaMdl: the character models of the select screen.
//
// Most of this file's code (Set, the per-player name plates, ...) is not in
// the module: the link stripped it and kept its data. The unread tables and
// strings are named below in the original's order and kept by MenuF's
// force_active list in config.yml; nothing reads them. See
// notes/toolchain.md, "Unread constants of stripped code". Their names, and
// the members of the tables, are guesses.
#include "game/msw/SelectMenu/CCharaMdl.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CChrMenuCtrl.hpp"
#include "game/msw/CChrMenuObj.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace {

    // A resource of the model files: its name and the file (index into
    // CCharaMdl's m_ResFile) it is in.
    struct ResEntry {
        const char *pName;
        unsigned char nFile;
    };
}

namespace CCharaMdl_cpp {
    extern const ResEntry UnusedMdlTable[8] = {
        { "chara_select_menu_line", 0 },
        { "cha_name_r1", 0 },
        { "cha_name_r2", 0 },
        { "cha_name_r3", 0 },
        { "cha_name_r4", 0 },
        { "cha_name_r5", 0 },
        { "cha_name_r6", 0 },
        { "mii_hatena", 0 },
    };
}

namespace {

    // The placement motion for 1 to 3 players (entry 0 is the plain one).
    const ResEntry s_DeployAnm[4] = {
        { "cha_name_r", 0 },
        { "chara_set_pos_1p", 0 },
        { "chara_set_pos_2p", 0 },
        { "chara_set_pos_3p", 0 },
    };
}

namespace CCharaMdl_cpp {
    extern const char UnusedPlayerNode[6][12] = {
        "player01", "player02", "player03", "player04", "player05", "player06",
    };
    extern const unsigned char UnusedPlayerTable[6][5] = {
        { 1, 0, 0, 1, 7 },
        { 3, 0, 2, 3, 9 },
        { 5, 0, 4, 5, 11 },
        { 2, 0, 1, 2, 8 },
        { 4, 0, 3, 4, 10 },
        { 6, 0, 5, 6, 12 },
    };
    extern const char UnusedMatName[] = "color_change_mat";
}

namespace {

    // The name plate texture.
    ResEntry s_NameTex = { "name_t", 0 };

    // The s_DeployAnm entry for 1 to 3 players.
    const unsigned char s_DeployAnmIdx[3] = { 1, 2, 3 };
}

namespace CCharaMdl_cpp {
    extern const long UnusedMotTbl[3] = { 1, 2, 3 };
    // Set()'s static local motTblLen in the original.
    extern const long UnusedMotTblLen = 3;
}

namespace {

    // The player colours (names guessed): red, blue, yellow, green, grey.
    bcs::BCS_Color s_PlayerColor[5] = {
        bcs::BCS_Color(0xFF, 0x1E, 0x1E, 0xFF),
        bcs::BCS_Color(0x00, 0x64, 0xFF, 0xFF),
        bcs::BCS_Color(0xFF, 0xFF, 0x00, 0xFF),
        bcs::BCS_Color(0x00, 0xFF, 0x00, 0xFF),
        bcs::BCS_Color(0x80, 0x80, 0x80, 0xFF),
    };
}

namespace {

    // (guessed) Inline helpers of the original: a missing file gives an
    // invalid animation, and the motion is played from its first frame at
    // normal speed.
    inline nw4r::g3d::ResAnmChr GetResAnmChr(nw4r::g3d::ResFile file, const ResEntry &entry)
    {
        if (!file.IsValid()) {
            return nw4r::g3d::ResAnmChr();
        }
        return file.GetResAnmChr(entry.pName);
    }

    inline void PlayMotion(bcs::CMdlObj *pMdl, nw4r::g3d::ResAnmChr anm)
    {
        if (!anm.IsValid()) {
            return;
        }
        bcs::CAnmMotion *pMot = static_cast<bcs::CAnmMotion *>(pMdl->GetAnmObjChr());
        if (pMot != 0) {
            pMot->Play(anm, 2, 1.0f, 0.0f, 0.0f);
            pMot->SetRate(1.0f);
        }
    }
}

nw4r::g3d::ResTexObj msw::SelectMenu::CCharaMdl::GetResTexObj()
{
    bcs::CMdlObj *pMdl = m_pNameMdl;
    if (pMdl == 0) {
        return nw4r::g3d::ResTexObj();
    }
    nw4r::g3d::ResFile file = pMdl->GetResMdl().GetParent();
    if (!file.IsValid()) {
        return nw4r::g3d::ResTexObj();
    }
    if (file.GetResTexNumEntries() == 0) {
        return nw4r::g3d::ResTexObj();
    }
    if (!file.GetResTex(s_NameTex.pName).IsValid()) {
        return nw4r::g3d::ResTexObj();
    }
    bcs::CMdlObj::CopiedMatAccess mat(pMdl, 0);
    if (!mat.IsValid()) {
        return nw4r::g3d::ResTexObj();
    }
    return mat.GetResTexObj(0);
}

void msw::SelectMenu::CCharaMdl::SetPlayerDeploy(long nPlayerNum)
{
    long nAnm = s_DeployAnmIdx[nPlayerNum - 1];
    bcs::CMdlObj *pMdl = m_pMdl;
    if (pMdl != 0) {
        const ResEntry &entry = s_DeployAnm[nAnm];
        nw4r::g3d::ResAnmChr anm = GetResAnmChr(m_ResFile[entry.nFile], entry);
        if (anm.IsValid()) {
            PlayMotion(pMdl, anm);
        }
    }
}

void msw::SelectMenu::CCharaMdl::SetStateChrMenuObj(long nPlayer, long nState)
{
    CChrMenuObj *pObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hChrMenuObj[nPlayer]);
    if (pObj == 0) {
        return;
    }
    bcs::CAnmMotion *pMot = static_cast<bcs::CAnmMotion *>(pObj->GetMdl()->GetAnmObjChr());
    if (pMot == 0) {
        return;
    }
    switch (nState) {
        default:
            pMot->Play(1, 2, 1.0f, 0.0f, 0.0f);
            break;
        case 1:
            pMot->Play(2, 0, 1.0f, 0.0f, 0.0f);
            break;
        case 2:
            pMot->Play(3, 2, 1.0f, 0.0f, 0.0f);
            break;
    }
}

// The rest of the stripped code's constants: they follow the functions'
// literals (1.0f, 0.0f) in the original.
namespace CCharaMdl_cpp {
    extern const float UnusedFloat0 = 0.5f;
    extern const float UnusedFloat1 = -0.4f;
    // GetResTexObj()'s static local texId (0) in the original; the
    // function here passes the 0 directly.
    extern const long UnusedTexId = 0;
}
