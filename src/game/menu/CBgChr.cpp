// msw::SelectMenu::CBgChr: the characters of the sports on their turning
// table in the menu background, with a name tag in front of each sport.
#include "game/msw/SelectMenu/CBgChr.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/FVEC2.hpp"
#include "game/msw/CBBoxObj.hpp"
#include "game/msw/CMenuMdlTableSet.hpp"
#include "game/msw/CSelectMenu.hpp"
#include "game/msw/CTopMenuChr.hpp"
#include "game/msw/CTournament.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/math/types.h"

namespace msw {
    namespace SelectMenu {

        // The names in this namespace are guesses; the objects have no
        // symbols except _GetAnimIdToTableAnimNo's static.
        namespace {

            // The resource files of the screen, loaded into m_ResFile.
            const char *s_ResName[2] = { "chara_table.brres", "menu_model.brres" };

            // A model or animation and the entry of s_ResName it is in.
            struct ResInfo {
                const char *pName;
                unsigned char nRes;
            };

            // The models: the table (only the first is used) and the name
            // tags, by sport.
            const ResInfo s_MdlTable[7] = {
                { "chara_set", 0 }, { "chara_set_tv", 0 }, { "tag_BK", 1 }, { "tag_VB", 1 },
                { "tag_DB", 1 },    { "tag_HK", 1 },       { "tag_MX", 1 },
            };

            // The animations of the name tags.
            const ResInfo s_TagAnimTable[7] = {
                { "Tag_mot_00", 0 },    { "Tag_mot_01", 0 },    { "Tag_mot_02", 0 },    { "tag", 0 },
                { "Tag_mot_00_on", 0 }, { "Tag_mot_01_on", 0 }, { "Tag_mot_02_on", 0 },
            };

            // The nodes of the table: where each sport's character stands,
            // then where its name tag hangs.
            const char *s_PosName[10] = {
                "BK_set",      "VB_set",      "DB_set",      "HK_set",      "MX_set",
                "BK_name_set", "VB_name_set", "DB_name_set", "HK_name_set", "MX_name_set",
            };

            // The entry of s_PosName for each character.
            const unsigned char s_CharaPosName[5] = { 0, 1, 2, 3, 4 };

            // The characters on the table. Only nGame is read here: the
            // CTopMenuChr character of the sport (5 or more for none).
            struct CharaInfo {
                unsigned char nUnk0;
                unsigned char nUnk1;
                unsigned char nUnk2;
                unsigned char nGame;
            };

            CharaInfo s_CharaInfo[5] = {
                { 1, 9, 0, 0 }, { 2, 9, 1, 1 }, { 3, 9, 2, 2 }, { 4, 9, 3, 3 }, { 5, 9, 0, 4 },
            };

            // The number of animations of the table.
            const long ANIM_NUM = 45;

            // The table's animations: [0] with four sports, [1] with the
            // mixed sports. Each has the waits on every place, the single
            // steps forward and (reversed) back, then the turns from one
            // place to another.
            const CMenuMdlTableSet::AnimInfo s_AnimTable[2][ANIM_NUM] = {
                {
                    { "cat00_BK", 0, 0 },    { "cat00_VB", 0, 0 },    { "cat00_CL", 0, 0 },
                    { "cat00_DB", 0, 0 },    { "cat00_HK", 0, 0 },    { "cat00_BK_VB", 0, 0 },
                    { "cat00_VB_DB", 0, 0 }, { "cat00_HK_CL", 0, 0 }, { "cat00_DB_HK", 0, 0 },
                    { "cat00_HK_BK", 0, 0 }, { "cat00_BK_VB", 0, 1 }, { "cat00_VB_DB", 0, 1 },
                    { "cat00_HK_CL", 0, 1 }, { "cat00_DB_HK", 0, 1 }, { "cat00_HK_BK", 0, 1 },
                    { "cat01_BK", 0, 0 },    { "cat01_VB", 0, 0 },    { "cat01_CL", 0, 0 },
                    { "cat01_DB", 0, 0 },    { "cat01_HK", 0, 0 },    { "cat01_BK", 0, 1 },
                    { "cat01_VB", 0, 1 },    { "cat01_CL", 0, 1 },    { "cat01_DB", 0, 1 },
                    { "cat01_HK", 0, 1 },    { "cat01_BK_VB", 0, 0 }, { "cat01_BK_DB", 0, 0 },
                    { "cat01_BK_HK", 0, 0 }, { "cat01_BK_MX", 0, 0 }, { "cat01_VB_BK", 0, 0 },
                    { "cat01_VB_DB", 0, 0 }, { "cat01_VB_HK", 0, 0 }, { "cat01_VB_MX", 0, 0 },
                    { "cat01_DB_BK", 0, 0 }, { "cat01_DB_VB", 0, 0 }, { "cat01_DB_HK", 0, 0 },
                    { "cat01_DB_MX", 0, 0 }, { "cat01_HK_BK", 0, 0 }, { "cat01_HK_VB", 0, 0 },
                    { "cat01_HK_DB", 0, 0 }, { "cat01_HK_MX", 0, 0 }, { "cat01_MX_BK", 0, 0 },
                    { "cat01_MX_VB", 0, 0 }, { "cat01_MX_DB", 0, 0 }, { "cat01_MX_HK", 0, 0 },
                },
                {
                    { "MX_cat00_BK", 0, 0 },    { "MX_cat00_VB", 0, 0 },    { "MX_cat00_MX", 0, 0 },
                    { "MX_cat00_DB", 0, 0 },    { "MX_cat00_HK", 0, 0 },    { "MX_cat00_BK_VB", 0, 0 },
                    { "MX_cat00_VB_MX", 0, 0 }, { "MX_cat00_MX_DB", 0, 0 }, { "MX_cat00_DB_HK", 0, 0 },
                    { "MX_cat00_HK_BK", 0, 0 }, { "MX_cat00_BK_VB", 0, 1 }, { "MX_cat00_VB_MX", 0, 1 },
                    { "MX_cat00_MX_DB", 0, 1 }, { "MX_cat00_DB_HK", 0, 1 }, { "MX_cat00_HK_BK", 0, 1 },
                    { "MX_cat01_BK", 0, 0 },    { "MX_cat01_VB", 0, 0 },    { "MX_cat01_MX", 0, 0 },
                    { "MX_cat01_DB", 0, 0 },    { "MX_cat01_HK", 0, 0 },    { "MX_cat01_BK", 0, 1 },
                    { "MX_cat01_VB", 0, 1 },    { "MX_cat01_MX", 0, 1 },    { "MX_cat01_DB", 0, 1 },
                    { "MX_cat01_HK", 0, 1 },    { "MX_cat01_BK_VB", 0, 0 }, { "MX_cat01_BK_DB", 0, 0 },
                    { "MX_cat01_BK_HK", 0, 0 }, { "MX_cat01_BK_MX", 0, 0 }, { "MX_cat01_VB_BK", 0, 0 },
                    { "MX_cat01_VB_DB", 0, 0 }, { "MX_cat01_VB_HK", 0, 0 }, { "MX_cat01_VB_MX", 0, 0 },
                    { "MX_cat01_DB_BK", 0, 0 }, { "MX_cat01_DB_VB", 0, 0 }, { "MX_cat01_DB_HK", 0, 0 },
                    { "MX_cat01_DB_MX", 0, 0 }, { "MX_cat01_HK_BK", 0, 0 }, { "MX_cat01_HK_VB", 0, 0 },
                    { "MX_cat01_HK_DB", 0, 0 }, { "MX_cat01_HK_MX", 0, 0 }, { "MX_cat01_MX_BK", 0, 0 },
                    { "MX_cat01_MX_VB", 0, 0 }, { "MX_cat01_MX_DB", 0, 0 }, { "MX_cat01_MX_HK", 0, 0 },
                },
            };

            // The animation ids of the name tags and their entries in
            // s_TagAnimTable.
            struct AnimIdInfo {
                unsigned short nAnimId;
                unsigned short nTableNo;
            };

            const AnimIdInfo s_AnimIdTable[7] = {
                { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 },
            };

            // The entry of s_TagAnimTable for animation id nAnimId, -1 if
            // there is none.
            long _GetAnimIdToTableAnimNo(long nAnimId)
            {
                static const long len = 7;
                for (long i = 0; i < len; i++) {
                    if (s_AnimIdTable[i].nAnimId == nAnimId) {
                        return s_AnimIdTable[i].nTableNo;
                    }
                }
                return -1;
            }

            // The animations pName of file, invalid when the file is not loaded.
            inline nw4r::g3d::ResAnmChr GetAnmChr(const char *pName, nw4r::g3d::ResFile file)
            {
                if (!file.IsValid()) {
                    return nw4r::g3d::ResAnmChr();
                }
                return file.GetResAnmChr(pName);
            }

            inline nw4r::g3d::ResAnmClr GetAnmClr(const char *pName, nw4r::g3d::ResFile file)
            {
                if (!file.IsValid()) {
                    return nw4r::g3d::ResAnmClr();
                }
                return file.GetResAnmClr(pName);
            }

            // The half size of the screen box of a name tag, before scaling.
            bcs::FVEC2 s_TagSize(1.5f, 0.34f);

            // The entry of m_hMdl of the name tag of sport nTag, or -1.
            inline long GetTagMdlId(long nTag)
            {
                long nMdl;
                switch (nTag) {
                    case 0:
                        nMdl = CBgChr::MDL_TAG + 0;
                        break;
                    case 1:
                        nMdl = CBgChr::MDL_TAG + 1;
                        break;
                    case 2:
                        nMdl = CBgChr::MDL_TAG + 2;
                        break;
                    case 3:
                        nMdl = CBgChr::MDL_TAG + 3;
                        break;
                    case 4:
                        nMdl = CBgChr::MDL_TAG + 4;
                        break;
                    default:
                        nMdl = -1;
                        break;
                }
                return nMdl;
            }

            // Detaches and deletes the animations of a model.
            inline void DeleteAnm(bcs::CMdlObj *pMdl)
            {
                bcs::CAnmObj *pAnmObj;

                pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_CHR);
                if (pAnmObj != 0) {
                    delete pAnmObj;
                }
                pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                if (pAnmObj != 0) {
                    delete pAnmObj;
                }
                pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
                if (pAnmObj != 0) {
                    delete pAnmObj;
                }
                pAnmObj = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_3);
                if (pAnmObj != 0) {
                    delete pAnmObj;
                }
            }

            // A screen box from the top left corner to centred and upwards.
            inline void ToScreenCenter(nw4r::math::AABB &rBox, float fHalfW, float fHeight, float fHalfH)
            {
                rBox.min.x -= fHalfW;
                rBox.min.y = fHeight - rBox.min.y - fHalfH;
                rBox.max.x -= fHalfW;
                rBox.max.y = fHeight - rBox.max.y - fHalfH;
            }
        }
    }
}

// Lights the name tag in slot nSel, darkens the others.
inline void msw::SelectMenu::CBgChr::SetTagColor(long nSel)
{
    bcs::BCS_Color normal(0x00, 0x00, 0x00, 0x00);
    bcs::BCS_Color select(0x50, 0x50, 0x00, 0x80);
    for (long i = 0; i < 5; i++) {
        bcs::CScnObj *pTag = bcs::CScnObj::GetScnObj(m_hMdl[MDL_TAG + i]);
        if (pTag != 0) {
            pTag->SetMatTevColor(GX_TEVREG1, nSel == i ? select : normal);
        }
    }
}

inline void msw::SelectMenu::CBgChr::PlayTagMotion(bcs::CMdlObj *pMdl, long nAnimId)
{
    long nNo = _GetAnimIdToTableAnimNo(nAnimId);
    nw4r::g3d::ResAnmChr res = GetAnmChr(s_TagAnimTable[nNo].pName, m_ResFile[s_TagAnimTable[nNo].nRes]);
    if (res.IsValid()) {
        bcs::CAnmMotion *pAnm = static_cast<bcs::CAnmMotion *>(pMdl->GetAnmObjChr());
        if (pAnm != 0) {
            pAnm->Play(res, 2, 1.0f, 0.0f, 0.0f);
            pAnm->SetRate(1.0f);
            pAnm->SetFrame(0.0f);
        }
    }
}

msw::SelectMenu::CBgChr::~CBgChr()
{
    Remove();
}

void msw::SelectMenu::CBgChr::Initialize()
{
    static const long resLen = 2;

    m_pTableSet = 0;
    for (long i = 0; i < MDL_NUM; i++) {
        m_pObj[i] = 0;
    }
    for (long i = 0; i < MDL_NUM; i++) {
        m_hMdl[i] = 0;
    }
    m_pTopMenuChr = 0;
    m_nRotDir = 0;
    m_nReqPos = -1;
    m_nRotState = 0;
    m_bMixMode = 0;
    SetPosChara(0);
    SetSelectGameChara();
    SetSelectGameTag(m_nPos[0]);
    bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
    for (long i = 0; i < resLen; i++) {
        m_ResFile[i] = pRes->GetG3DResFile(s_ResName[i]);
    }
}

int msw::SelectMenu::CBgChr::Set()
{
    // The screen boxes of the name tags.
    static const float scale = 0.75f;
    static const float offsetY = 0.0f;

    if (m_Unk2C != 0) {
        return 0;
    }

    nw4r::g3d::ResFile file = m_ResFile[0];
    SetPosChara(0);
    bcs::CMdlObj *pMdl = CreateMdlObj(s_MdlTable[0].pName, file);
    if (pMdl != 0) {
        m_hMdl[MDL_TABLE] = pMdl->GetHandle();
        CBgChrObj *pObj = new CBgChrObj(pMdl);
        if (pObj != 0) {
            m_pObj[0] = pObj;
            CMenuMdlTableSet *pTableSet = new CMenuMdlTableSet;
            pTableSet->SetObj(pObj);
            pTableSet->SetResFile(file);
            pTableSet->SetAnimTable(s_AnimTable[m_bMixMode != 0], ANIM_NUM);
            m_pTableSet = pTableSet;
        }
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CTopMenuChr *pTopMenuChr = new CTopMenuChr(pSelectMenu);
    if (pTopMenuChr != 0) {
        pTopMenuChr->EnableDisp(1);
        pTopMenuChr->EnableMixDisp(m_bMixMode != 0);
        m_pTopMenuChr = pTopMenuChr;
        unsigned long gold = 0;
        for (long i = 0; i <= 3; i++) {
            if (CTournament::IsUnlockTopMenuChr(i, -1)) {
                gold |= 1 << i;
            }
        }
        for (long i = 0; i <= 3; i++) {
            pTopMenuChr->EnableGoldModel(i, (gold & (1 << i)) != 0);
        }
    }

    // The characters, each on the node of its sport.
    CTopMenuChr *pTop = m_pTopMenuChr;
    if (pTop != 0) {
        bcs::CMdlObj *pTable = m_pTableSet->GetObj()->GetMdl();
        for (long i = 0; i < GetPosNum(); i++) {
            long nChara = GetCharaIdFromPosChara(m_nPos[i]);
            long nGame = s_CharaInfo[nChara].nGame;
            if (nGame < 5) {
                nw4r::g3d::ResNode node = pTable->GetResNode(s_PosName[s_CharaPosName[nChara]]);
                pTop->AttachTo(nGame, pTable, node.GetID());
            }
        }
    }

    // The name tags.
    bcs::CMdlObj *pTable = 0;
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
        pTable = m_pTableSet->GetObj()->GetMdl();
    }
    if (pTable != 0) {
        nw4r::g3d::ResFile tagFile = m_ResFile[1];
        if (tagFile.IsValid()) {
            for (long i = 0; i < 5; i++) {
                if (i == 4 && !m_bMixMode) {
                    continue;
                }
                bcs::CMdlObj *pTag = CreateMdlObj(s_MdlTable[2 + i].pName, tagFile);
                if (pTag != 0) {
                    m_hMdl[MDL_TAG + i] = pTag->GetHandle();
                    if (pTag != 0 && pTable != 0) {
                        pTag->AttachToObject(pTable, pTable->GetNodeID(s_PosName[5 + i]), bcs::CScnObj::ATTACHMODE_1);
                    }
                    PlayTagMotion(pTag, 0);
                }
                CBBoxObj *pBox = new CBBoxObj;
                if (pBox != 0) {
                    m_hMdl[MDL_BBOX + i] = pBox->GetHandle();
                    bcs::FVEC2 size(s_TagSize.x * scale, s_TagSize.y * scale);
                    pBox->SetSize(-size.x, size.x, offsetY - size.y, offsetY + size.y, 0.0f, 0.0f);
                    pBox->AttachToObject(pTag, 0, bcs::CScnObj::ATTACHMODE_0);
                    bcs::CRenderMan::PushBack(pBox, 2);
                }
            }
        }
    }

    // The table waits with the sport in front.
    if (m_bMixMode) {
        const unsigned char anim[POS_NUM_MIX] = { 0, 1, 2, 3, 4 };
        PlayAnim(anim[m_nPos[0]], 1.0f, 0.0f);
    } else {
        const unsigned char anim[POS_NUM] = { 0, 1, 3, 4 };
        PlayAnim(anim[m_nPos[0]], 1.0f, 0.0f);
    }
    m_nRotState = 0;
    m_Unk2C = 1;
    m_Unk30 = STATE_SET;
    return 0;
}

bcs::CMdlObj *msw::SelectMenu::CBgChr::CreateMdlObj(const char *pName, nw4r::g3d::ResFile file)
{
    if (!file.IsValid()) {
        return 0;
    }
    nw4r::g3d::ResMdl mdl = file.GetResMdl(pName);
    if (!mdl.IsValid()) {
        return 0;
    }

    bcs::CMdlObj *pMdl = new bcs::CMdlObj;
    if (pMdl == 0) {
        return 0;
    }
    pMdl->Create(mdl, 0x103, 0);
    bcs::CRenderMan::PushBack(pMdl, 2);
    pMdl->OnFlag(0x2000);
    pMdl->m_vTrans.Set(0.0f, 0.0f, 0.0f);
    pMdl->SetRot(0.0f, 0.0f, 0.0f);
    pMdl->m_vScale.Set(1.0f, 1.0f, 1.0f);

    bcs::CAnmMotion *pAnm = new bcs::CAnmMotion;
    if (pAnm != 0) {
        pAnm->Create(pMdl, file, 0, 3, 0);
        pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_CHR);
    }
    return pMdl;
}

void msw::SelectMenu::CBgChr::Remove()
{
    for (long i = 0; i < MDL_NUM; i++) {
        if (m_pObj[i] != 0) {
            delete m_pObj[i];
            m_pObj[i] = 0;
        }
    }
    for (long i = 0; i < MDL_NUM; i++) {
        bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(m_hMdl[i]));
        if (pMdl != 0) {
            // The screen boxes have no animations.
            if (i < MDL_BBOX || i >= MDL_BBOX + 5) {
                DeleteAnm(pMdl);
            }
            pMdl->DetachFromParent();
            if (pMdl != 0) {
                bcs::CScnObj::AppendDelayDeleteObject(pMdl);
            }
        }
        m_hMdl[i] = 0;
    }
    if (m_pTopMenuChr != 0) {
        m_pTopMenuChr->Delete();
        m_pTopMenuChr = 0;
    }
    if (m_pTableSet != 0) {
        delete m_pTableSet;
        m_pTableSet = 0;
    }
    m_nRotState = 0;
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
}

void msw::SelectMenu::CBgChr::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_Unk30 = STATE_IN;
            break;
        case STATE_IN:
            m_bSeEnable = 1;
            m_Unk30 = STATE_ACTIVE;
            break;
        case STATE_CLOSE:
            Remove();
            m_Unk30 = STATE_OUT;
            break;
        case STATE_OUT:
            m_Unk08 = 2;
            break;
    }
    StepControl();
    if (m_pTableSet != 0) {
        m_pTableSet->MainLoop();
    }
}

void msw::SelectMenu::CBgChr::StepControl()
{
    switch (m_nRotState) {
        case 1:
            if (IsAnimEnd()) {
                long nPos = m_nPos[0];
                if (m_bMixMode) {
                    const unsigned char anim[POS_NUM_MIX] = { 0, 1, 2, 3, 4 };
                    PlayAnim(anim[nPos], 1.0f, 0.0f);
                } else {
                    const unsigned char anim[POS_NUM] = { 0, 1, 3, 4 };
                    PlayAnim(anim[nPos], 1.0f, 0.0f);
                }
                m_nRotState = 0;
            }
            break;
        case 2:
            if (IsAnimEnd()) {
                m_nRotState = 0;
            }
            break;
        case 0: {
            long nPos = m_nPos[0];
            if (m_nRotDir > 0) {
                m_nRotDir = 0;
                long nNext = nPos + 1;
                if (nNext >= GetPosNum()) {
                    nNext = 0;
                }
                SetPosChara(nNext);
                SetSelectGameChara();
                if (m_bMixMode) {
                    const unsigned char anim[POS_NUM_MIX] = { 9, 5, 6, 7, 8 };
                    PlayAnim(anim[nNext], 1.0f, 0.0f);
                } else {
                    const unsigned char anim[POS_NUM] = { 9, 5, 6, 8 };
                    PlayAnim(anim[nNext], 1.0f, 0.0f);
                }
                SE_Play(0xBC0, 1);
                m_nRotState = 1;
            } else if (m_nRotDir < 0) {
                long nNext = nPos - 1;
                m_nRotDir = 0;
                if (nNext < 0) {
                    nNext = GetPosNum() - 1;
                }
                SetPosChara(nNext);
                SetSelectGameChara();
                if (m_bMixMode) {
                    const unsigned char anim[POS_NUM_MIX] = { 10, 11, 12, 13, 14 };
                    PlayAnim(anim[nNext], 1.0f, 0.0f);
                } else {
                    const unsigned char anim[POS_NUM] = { 10, 11, 13, 14 };
                    PlayAnim(anim[nNext], 1.0f, 0.0f);
                }
                SE_Play(0xBC0, 1);
                m_nRotState = 1;
            } else {
                long nReq = m_nReqPos;
                if (nReq >= 0 && nReq != nPos) {
                    m_nReqPos = -1;
                    SetPosChara(nReq);
                    SetSelectGameChara();
                    if (m_bMixMode) {
                        const unsigned char anim[POS_NUM_MIX][POS_NUM_MIX] = {
                            { 25, 25, 26, 27, 28 }, { 29, 29, 30, 31, 32 }, { 33, 34, 34, 35, 36 },
                            { 37, 38, 39, 39, 40 }, { 41, 42, 43, 44, 44 },
                        };
                        PlayAnim(anim[nPos][nReq], 1.0f, 0.0f);
                    } else {
                        const unsigned char anim[POS_NUM][POS_NUM] = {
                            { 25, 25, 26, 27 },
                            { 29, 29, 30, 31 },
                            { 33, 34, 34, 35 },
                            { 37, 38, 39, 39 },
                        };
                        PlayAnim(anim[nPos][nReq], 1.0f, 0.0f);
                    }
                    SE_Play(0xBC0, 1);
                    m_nRotState = 2;
                }
            }
            break;
        }
        case 3:
            m_Unk30 = STATE_CLOSE;
            break;
        default:
            m_Unk30 = STATE_CLOSE;
            break;
    }
}

long msw::SelectMenu::CBgChr::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_DISP: {
            int bDisp = (int)pParam;
            CTopMenuChr *pTop = m_pTopMenuChr;
            if (pTop != 0) {
                pTop->EnableDisp(bDisp);
                if (m_bMixMode) {
                    pTop->EnableMixDisp(bDisp);
                }
            }
            if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
                m_pTableSet->GetObj()->SetVisible(bDisp);
            }
            SetDispTagName(bDisp);
            break;
        }
        case EVENT_CHARA_SPEED:
            SetCharaSpeed(*(float *)pParam);
            break;
    }
    return 0;
}

void msw::SelectMenu::CBgChr::SetPosChara(long nPos)
{
    int bMix = m_bMixMode;
    static const long charaIdTable_Normal[7] = { 0, 1, 2, 3, 0, 1, 2 };
    const long *pTable = charaIdTable_Normal;
    if (bMix) {
        static const long charaIdTable_Mix[9] = { 0, 1, 2, 3, 4, 0, 1, 2, 3 };
        pTable = charaIdTable_Mix;
    }
    long num = bMix ? 5 : 4; // GetPosNum(), with the flag read once
    for (long i = 0; i < num; i++, nPos++) {
        m_nPos[i] = pTable[nPos];
    }
}

inline long msw::SelectMenu::CBgChr::GetCharaIdFromPosChara(long nPos)
{
    if (m_bMixMode) {
        static const long charaIdTable_Mix[6] = { 0, 1, 4, 2, 3, 0 };
        return charaIdTable_Mix[nPos];
    } else {
        static const long charaIdTable_Normal[4] = { 0, 1, 2, 3 };
        return charaIdTable_Normal[nPos];
    }
}

long msw::SelectMenu::CBgChr::GetPosChara(long nSlot)
{
    return m_nPos[nSlot];
}

long msw::SelectMenu::CBgChr::RotPosChara(long nDir)
{
    m_nRotDir = nDir;
    return 0;
}

long msw::SelectMenu::CBgChr::GetCharaPos(long nPos)
{
    for (long i = 0; i < GetPosNum(); i++) {
        if (nPos == m_nPos[i]) {
            return i;
        }
    }
    return -1;
}

long msw::SelectMenu::CBgChr::SetSelectGameChara()
{
    return SetSelectGameChara(m_nPos[0]);
}

long msw::SelectMenu::CBgChr::SetSelectGameChara(long nPos)
{
    CTopMenuChr *pTop = m_pTopMenuChr;
    if (pTop == 0) {
        return -1;
    }
    if (nPos < 0) {
        SetTagColor(-1);
        return 0;
    }
    long nChara = GetCharaIdFromPosChara(nPos);
    if (nChara >= 0) {
        long nGame = s_CharaInfo[nChara].nGame;
        if (nGame < 5) {
            pTop->SetSelectGame(nGame, 8.0f);
            return 0;
        }
    }
    pTop->SetSelectGame(-1, 8.0f);
    return 0;
}

long msw::SelectMenu::CBgChr::SetSelectGameTag(long nPos)
{
    if (nPos < 0) {
        SetTagColor(-1);
        return 0;
    }
    if (GetCharaIdFromPosChara(nPos) >= 0) {
        SetTagColor(nPos);
        return 0;
    }
    SetTagColor(-1);
    return 0;
}

void msw::SelectMenu::CBgChr::PlayAnim(long nAnim, float fRate, float fFrame)
{
    if (m_pTableSet != 0) {
        m_pTableSet->Play(nAnim, fRate, fFrame, 0, 0.0f);
    }
}

int msw::SelectMenu::CBgChr::IsAnimEnd()
{
    if (m_pTableSet == 0) {
        return 1;
    }
    return m_pTableSet->GetObj()->IsAnmEnd();
}

long msw::SelectMenu::CBgChr::PrepareDecideAnimChara()
{
    if (m_pTopMenuChr == 0) {
        return -1;
    }
    m_pTopMenuChr->PrepareDecideAnim();
    return 0;
}

long msw::SelectMenu::CBgChr::IsEndPrepareDecideAnimChara()
{
    if (m_pTopMenuChr == 0) {
        return -1;
    }
    return m_pTopMenuChr->IsReadyDecideAnim();
}

long msw::SelectMenu::CBgChr::PlayDecideAnimChara()
{
    if (m_pTopMenuChr == 0) {
        return -1;
    }
    m_pTopMenuChr->PlayDecideAnim(8.0f);
    return 0;
}

long msw::SelectMenu::CBgChr::IsEndDecideAnimChara()
{
    if (m_pTopMenuChr == 0) {
        return 1;
    }
    return m_pTopMenuChr->IsEndDecideAnim();
}

void msw::SelectMenu::CBgChr::ResetChara()
{
    if (m_pTopMenuChr != 0) {
        m_pTopMenuChr->Reset();
    }
}

int msw::SelectMenu::CBgChr::IsRotAnim()
{
    return m_nRotState == 0;
}

void msw::SelectMenu::CBgChr::SetDispTagName(int bDisp)
{
    int bHide = !bDisp;
    for (long i = 0; i < 5; i++) {
        bcs::CScnObj *pTag = bcs::CScnObj::GetScnObj(m_hMdl[MDL_TAG + i]);
        if (pTag != 0) {
            if (bHide) {
                pTag->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            } else {
                pTag->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
    }
}

void msw::SelectMenu::CBgChr::PlayClr(bcs::CMdlObj *pMdl, long nAnimId, float fRate)
{
    long nNo = _GetAnimIdToTableAnimNo(nAnimId);
    nw4r::g3d::ResAnmClr res = GetAnmClr(s_TagAnimTable[nNo].pName, m_ResFile[s_TagAnimTable[nNo].nRes]);
    if (!res.IsValid()) {
        return;
    }

    if (pMdl->GetAnmObjMatClr() != 0) {
        bcs::CAnmObj *pOld = pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_MATCLR);
        if (pOld != 0) {
            delete pOld;
        }
    }
    bcs::CAnmMatClr *pAnm = new bcs::CAnmMatClr;
    if (pAnm->Create(pMdl, res, 0, 0) == 1) {
        pAnm->SetRate(fRate);
        if (fRate >= 0.0f) {
            pAnm->SetFrame(0.0f);
        } else {
            pAnm->SetFrame(pAnm->VFunc28() - 1.0f);
        }
        pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
    } else if (pAnm != 0) {
        delete pAnm;
    }
}

void msw::SelectMenu::CBgChr::SetTagNameAnim(long nTag, long nAnimId)
{
    long nMdl = GetTagMdlId(nTag);
    if (nMdl >= 0) {
        bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(m_hMdl[nMdl]));
        if (pMdl != 0) {
            PlayTagMotion(pMdl, nAnimId);
        }
    }
}

void msw::SelectMenu::CBgChr::TagNameFadeInOut(int bOut)
{
    for (long i = 0; i < 5; i++) {
        long nMdl = GetTagMdlId(i);
        if (nMdl >= 0) {
            bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(bcs::CScnObj::GetScnObj(m_hMdl[nMdl]));
            if (pMdl != 0) {
                PlayClr(pMdl, 3, bOut ? -1.0f : 1.0f);
            }
        }
    }
}

long msw::SelectMenu::CBgChr::GetBBoxInfo(long nGame, nw4r::math::AABB &rChara, nw4r::math::AABB &rTag)
{
    rChara.min.x = 1.0f;
    rChara.min.y = 1.0f;
    rChara.min.z = 1.0f;
    rChara.max.x = -1.0f;
    rChara.max.y = -1.0f;
    rChara.max.z = -1.0f;
    rTag.min.x = 1.0f;
    rTag.min.y = 1.0f;
    rTag.min.z = 1.0f;
    rTag.max.x = -1.0f;
    rTag.max.y = -1.0f;
    rTag.max.z = -1.0f;
    if (nGame < 0 || nGame > 4) {
        return -2;
    }
    if (m_pTopMenuChr == 0) {
        return -1;
    }

    long nWidth = bcs::CMainApp::g_Rmode.fbWidth;
    long nHeight = bcs::CMainApp::g_Rmode.efbHeight;
    float fHalfW = 0.5f * nWidth;
    float fHeight = nHeight;
    float fHalfH = 0.5f * fHeight;
    if (m_pTopMenuChr->GetBBoxInfo(nGame, rChara) == 1) {
        ToScreenCenter(rChara, fHalfW, fHeight, fHalfH);
    }
    CBBoxObj *pBox = static_cast<CBBoxObj *>(bcs::CScnObj::GetScnObj(m_hMdl[MDL_BBOX + nGame]));
    if (pBox != 0 && pBox->GetResult(rTag) == 1) {
        ToScreenCenter(rTag, fHalfW, fHeight, fHalfH);
    }
    return 0;
}

void msw::SelectMenu::CBgChr::SetTableAnimSpeed(float fSpeed)
{
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0 && m_pTableSet->GetObj()->GetMdl() != 0) {
        bcs::CAnmObj *pAnm = m_pTableSet->GetObj()->GetMdl()->GetAnmObjChr();
        if (pAnm != 0) {
            pAnm->SetRate(fSpeed);
        }
    }
}

float msw::SelectMenu::CBgChr::GetTableAnimSpeed()
{
    if (m_pTableSet == 0) {
        return 0.0f;
    }
    if (m_pTableSet->GetObj() == 0) {
        return 0.0f;
    }
    if (m_pTableSet->GetObj()->GetMdl() == 0) {
        return 0.0f;
    }
    bcs::CAnmObj *pAnm = m_pTableSet->GetObj()->GetMdl()->GetAnmObjChr();
    if (pAnm == 0) {
        return 0.0f;
    }
    return pAnm->GetRate();
}

void msw::SelectMenu::CBgChr::SetCharaSpeed(float fSpeed)
{
    if (m_pTopMenuChr != 0) {
        m_pTopMenuChr->SetMotionSpeedRate(fSpeed);
    }
}

msw::SelectMenu::CBgChrTop::CBgChrTop(long nId)
    : CBgChr(nId)
{
}

msw::SelectMenu::CBgChrTop::~CBgChrTop()
{
}

msw::SelectMenu::CBgChrObj::~CBgChrObj()
{
}
