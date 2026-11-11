// msw::SelectMenu::CCupChr: the cup on its stand on the cup selection
// screen.
#include "game/msw/SelectMenu/CCupChr.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CChrMenuCtrl.hpp"
#include "game/msw/CChrMenuObj.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuMdlTableSet.hpp"
#include "game/msw/CTrophyObj.hpp"
#include "game/msw/EvDemo.hpp"
#include "game/msw/SelectMenu/CCourtCharaObj.hpp"
#include "game/msw/SelectMenu/CCourtTableSetObj.hpp"


// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The resource files of the screen (one).
    const char *s_ResName[] = { "chara_table.brres" };

    // The stand's model in it.
    const char *const s_StandMdlName[] = { "chara_set_non_tv", 0 };

    // The node of the stand the cup hangs from.
    const char *s_TrophyNodeName = "trophy_set";

    // The stand's animations: [0] with the layout as it is, [1] when
    // msw::CLytCtrl::IsLocationAdjust is set.
    const msw::CMenuMdlTableSet::AnimInfo s_AnimTable[2][16] = {
        {
            { "m_cat04", 0, 0 },
            { "m_cat04L", 1, 0 },
            { "m_cat04L_T", 1, 0 },
            { "m_cat05_S", 0, 0 },
            { "m_cat05_SL", 1, 0 },
            { "m_cat05_D", 0, 0 },
            { "m_cat05_DL", 1, 0 },
            { "m_cat06_S", 0, 0 },
            { "m_cat06_S_2", 0, 0 },
            { "m_cat06_D", 0, 0 },
            { "m_cat06_L_IN", 0, 0 },
            { "m_cat06_L_OUT", 0, 0 },
            { "m_cat06_R_IN", 0, 0 },
            { "m_cat06_R_OUT", 0, 0 },
            { "m_cat07", 0, 0 },
            { "m_cat08", 0, 0 },
        },
        {
            { "cat04", 0, 0 },
            { "cat04L", 1, 0 },
            { "cat04L_T", 1, 0 },
            { "cat05_S", 0, 0 },
            { "cat05_SL", 1, 0 },
            { "cat05_D", 0, 0 },
            { "cat05_DL", 1, 0 },
            { "cat06_S", 0, 0 },
            { "cat06_S_2", 0, 0 },
            { "cat06_D", 0, 0 },
            { "cat06_L_IN", 0, 0 },
            { "cat06_L_OUT", 0, 0 },
            { "cat06_R_IN", 0, 0 },
            { "cat06_R_OUT", 0, 0 },
            { "cat07", 0, 0 },
            { "cat08", 0, 0 },
        },
    };

    // The playing speed of each animation of the stand.
    struct AnimRate {
        unsigned short nAnim;
        unsigned short nUnk; // always nAnim
        float fRate;
    };

    const AnimRate s_AnimRate[16] = {
        { 0, 0, 2.0f },   { 1, 1, 2.0f },   { 2, 2, 2.0f },   { 3, 3, 1.0f },
        { 4, 4, 1.0f },   { 5, 5, 1.0f },   { 6, 6, 1.0f },   { 7, 7, 1.0f },
        { 8, 8, 1.0f },   { 9, 9, 1.0f },   { 10, 10, 1.0f }, { 11, 11, 1.0f },
        { 12, 12, 1.0f }, { 13, 13, 1.0f }, { 14, 14, 1.0f }, { 15, 15, 1.0f },
    };

    // [0] is the entry of m_hMdl shown after Set. A table, not a scalar:
    // the original reads the word at run time.
    const long s_DefaultShown[1] = { 0 };

    // The colours of the cup: the silhouette and the base colour; [1] for
    // kind 5.
    bcs::BCS_Color s_SilhouetteColor[2] = {
        bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xFF),
        bcs::BCS_Color(0x60, 0x60, 0x60, 0xFF),
    };
    bcs::BCS_Color s_BaseColor[2] = {
        bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xFF),
        bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF),
    };

    // How far the cup turns each frame, in radians.
    bcs::FVEC3 s_RotSpeed(0.0f, 0.02f, 0.0f);

    // The speed of animation nAnim of the stand; 1 if it has none.
    inline float GetAnimRate(long nAnim)
    {
        for (long i = 0; i < 16; i++) {
            if (nAnim == s_AnimRate[i].nAnim) {
                return s_AnimRate[i].fRate;
            }
        }
        return 1.0f;
    }

    // The cup type msw::CTrophyObj::CreateModel takes for type nType.
    inline long GetTrophyType(long nType)
    {
        switch (nType) {
            default:
                return 1;
            case 2:
                return 2;
            case 3:
                return 3;
            case 4:
                return 4;
        }
    }

    // The same for the kind.
    inline long GetTrophyKind(long nKind)
    {
        switch (nKind) {
            default:
                return 0;
            case 1:
                return 1;
            case 2:
                return 2;
            case 3:
                return 3;
            case 4:
                return 4;
        }
    }

    // Brings an angle into [fMin, fMax].
    inline float WrapAngle(float fAngle, float fMax, float fMin)
    {
        while (fAngle > fMax) {
            fAngle -= fMax;
        }
        while (fAngle < fMin) {
            fAngle += fMax;
        }
        return fAngle;
    }

    // Hides or shows a model.
    inline void SetMdlHide(bcs::CScnObj *pMdl, int bHide)
    {
        if (bHide) {
            pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        } else {
            pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        }
    }
}

// NOTE: the original constructs m_ResFile with a one-pass store loop
// (addi/stw/cmplw/blt); ours peels the store and adds a dead counted loop.
// Same effect; no source form found that gives the original's loop.
msw::SelectMenu::CCupChr::CCupChr(long nId)
    : CSelectMenuLayoutBase(nId)
{
    Initialize();
}

msw::SelectMenu::CCupChr::~CCupChr()
{
    Remove();
    if (m_pCharaObj[0] != 0) {
        delete m_pCharaObj[0];
    }
}

void msw::SelectMenu::CCupChr::Initialize()
{
    static const long resLen = 1;

    m_Unk34 = 0;
    m_pTableSet = 0;
    m_hMdl[0] = 0;
    m_hMdl[1] = 0;
    m_bClose = 0;
    m_Unk6C = 0;
    m_Unk78 = 0.0f;
    m_pCharaObj[0] = new CCourtCharaObj(0);
    bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
    for (long i = 0; i < resLen; i++) {
        m_ResFile[i] = pRes->GetG3DResFile(s_ResName[i]);
    }
    m_nAnim = -1;
    if (CLoadData::IsLoaded(0x5C)) {
        EvDemo::InitArcResData(CLoadData::GetPtr(0x5C), 0);
    }
}

int msw::SelectMenu::CCupChr::Set()
{
    if (m_Unk2C != 0) {
        return 0;
    }

    nw4r::g3d::ResFile file = m_ResFile[0];
    bcs::CMdlObj *pMdl = CreateMdlObj(s_StandMdlName[0], file);
    if (pMdl != 0) {
        CCourtTableSetObj *pObj = new CCourtTableSetObj(pMdl);
        if (pObj != 0) {
            CMenuMdlTableSet *pTableSet = new CMenuMdlTableSet;
            if (pTableSet != 0) {
                pTableSet->SetObj(pObj);
                pTableSet->SetResFile(file);
                pTableSet->SetAnimTable(s_AnimTable[CLytCtrl::IsLocationAdjust() != 0], 16);
                pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
            m_pTableSet = pTableSet;
        }
    }

    for (long i = 0; i < 1; i++) {
        if (s_DefaultShown[i] == 0) {
            m_nShown[i] = s_DefaultShown[i];
        }
    }
    if (m_pTableSet != 0) {
        m_nAnim = -1;
    }
    m_bClose = 0;

    CChrMenuObj *pChrObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[m_nShown[0]]);
    if (pChrObj != 0 && pChrObj->GetMdl() != 0) {
        pChrObj->GetMdl()->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    }
    if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
        m_pTableSet->GetObj()->SetVisible(0);
    }

    m_Unk2C = 1;
    m_Unk30 = STATE_NONE;
    return 0;
}

bcs::CMdlObj *msw::SelectMenu::CCupChr::CreateMdlObj(const char *pName, nw4r::g3d::ResFile file)
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

msw::CTrophyObj *msw::SelectMenu::CCupChr::CreateChrMenuObj(long nId, long nType, long nKind)
{
    long nChrType = nType;
    if (nChrType < 1 || nChrType > 4) {
        nChrType = 0;
    }
    long nChrKind = nKind;
    if (nChrKind < 0 || nChrKind > 5) {
        nChrKind = 5;
    }

    CTrophyObj *pTrophy = CTrophyObj::CreateModel(GetTrophyType(nChrType), GetTrophyKind(nChrKind), 1, 2, 0);
    if (pTrophy == 0) {
        return 0;
    }
    pTrophy->SetDisp(1);

    CChrMenuObj *pChrObj = CChrMenuCtrl::s_Instance.CreateChrMenuObj(nChrType, nChrKind, pTrophy);
    if (pChrObj == 0) {
        return 0;
    }
    pChrObj->SetId(nId);

    long nSilhouette;
    long nBase;
    if (nKind == 5) {
        nSilhouette = 1;
        nBase = 1;
    } else {
        nSilhouette = 0;
        nBase = 0;
    }
    pTrophy->SetSilhouetteColor(s_SilhouetteColor[nSilhouette]);
    pTrophy->SetBaseColor(s_BaseColor[nBase]);
    return pTrophy;
}

void msw::SelectMenu::CCupChr::Remove()
{
    if (m_Unk2C == 0) {
        return;
    }

    // The cups (entry 0 of m_hMdl is the empty one).
    for (long i = 1; i < 2; i++) {
        if (m_hMdl[i] != 0) {
            CChrMenuCtrl::s_Instance.DeleteChrMenuObj(m_hMdl[i]);
        }
        m_hMdl[i] = 0;
    }

    CMenuMdlTableSet *pTableSet = m_pTableSet;
    if (pTableSet != 0) {
        if (pTableSet->GetObj() != 0) {
            delete pTableSet->GetObj();
        }
        delete pTableSet;
        m_pTableSet = 0;
    }
    m_bClose = 0;
    m_Unk30 = STATE_NONE;
    m_Unk2C = 0;
}

void msw::SelectMenu::CCupChr::Main()
{
    StepControl();
    if (m_pCharaObj[0] != 0) {
        m_pCharaObj[0]->StateCheck();
    }
    if (m_pTableSet != 0) {
        m_pTableSet->MainLoop();
    }
}

void msw::SelectMenu::CCupChr::StepControl()
{
    if (m_Unk2C == 1) {
        CCourtCharaObj *pChara = m_pCharaObj[0];
        bcs::FVEC3 rot = s_RotSpeed;
        float fMin = 0.0f;
        float fMax = 6.2831855f;
        if (pChara != 0) {
            bcs::CMdlObj *pMdl = pChara->GetMdlObj(pChara->GetMdlHandle());
            if (pMdl != 0) {
                float y = rot.y;
                float x = rot.x;
                float z = rot.z;
                pMdl->m_fRotX = WrapAngle(pMdl->m_fRotX + x, fMax, fMin);
                pMdl->m_fRotY = WrapAngle(pMdl->m_fRotY + y, fMax, fMin);
                pMdl->m_fRotZ = WrapAngle(pMdl->m_fRotZ + z, fMax, fMin);
            }
            pMdl = pChara->GetMdlObj(pChara->GetNextMdlHandle());
            if (pMdl != 0) {
                float y = rot.y;
                float x = rot.x;
                float z = rot.z;
                pMdl->m_fRotX = WrapAngle(pMdl->m_fRotX + x, fMax, fMin);
                pMdl->m_fRotY = WrapAngle(pMdl->m_fRotY + y, fMax, fMin);
                pMdl->m_fRotZ = WrapAngle(pMdl->m_fRotZ + z, fMax, fMin);
            }
        }
    }
    if (m_bClose) {
        m_Unk30 = STATE_CLOSE;
    }
}

long msw::SelectMenu::CCupChr::SendEvent(long nEvent, void *pParam)
{
    long nAnim = (long)pParam;

    switch (nEvent) {
        case EVENT_SHOW: {
            int bHide = (pParam == 0);
            CChrMenuObj *pChrObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[m_nShown[0]]);
            if (pChrObj != 0 && pChrObj->GetMdl() != 0) {
                SetMdlHide(pChrObj->GetMdl(), bHide);
            }
            if (m_pTableSet != 0 && m_pTableSet->GetObj() != 0) {
                m_pTableSet->GetObj()->SetVisible(0);
            }
            if (pParam == 0) {
                SetCharaModel(0, 0, 5, -1);
                m_nAnim = -1;
            }
            break;
        }
        case EVENT_PLAY_ANIM:
            if (nAnim == m_nAnim) {
                break;
            }
            // fall through
        case EVENT_PLAY_ANIM_FORCE: {
            float rate = GetAnimRate(nAnim);
            if (m_pTableSet != 0) {
                if (nAnim != -1) {
                    m_pTableSet->Play(nAnim, rate, 0.0f, 0, 0.0f);
                }
                m_nAnim = nAnim;
            }

            // The loop that follows each opening animation.
            long nNext;
            switch (nAnim) {
                case 0:
                    nNext = 1;
                    break;
                case 5:
                    nNext = 6;
                    break;
                case 3:
                    nNext = 4;
                    break;
                default:
                    nNext = -1;
                    break;
            }
            if (nNext >= 0) {
                rate = GetAnimRate(nNext);
                if (m_pTableSet != 0 && nNext != -1) {
                    m_pTableSet->PushBack(nNext, rate, 0.0f);
                }
            }
            break;
        }
        case EVENT_PUSH_ANIM: {
            float rate = GetAnimRate(nAnim);
            if (m_pTableSet != 0 && nAnim != -1) {
                m_pTableSet->PushBack(nAnim, rate, 0.0f);
            }
            break;
        }
        case EVENT_CHANGE_ANIM: {
            float wait = CCourtCharaObj::s_FadeInOutTime;
            float rate = GetAnimRate(nAnim);
            CMenuMdlTableSet *pTableSet = m_pTableSet;
            if (pTableSet != 0) {
                pTableSet->SetWait(wait);
                pTableSet->ClearList();
                pTableSet->PushBack(nAnim, rate, 0.0f);
            }
            break;
        }
    }
    return 0;
}

int msw::SelectMenu::CCupChr::IsAnimEnd()
{
    if (m_pTableSet == 0) {
        return 1;
    }
    return m_pTableSet->GetObj()->IsAnmEnd();
}

int msw::SelectMenu::CCupChr::SetCharaModel(long nIndex, long nType, long nKind, long nSlot)
{
    bcs::CScnObj *pParent;
    unsigned long hMdl = 0;
    CTrophyObj *pShown = 0;
    CTrophyObj *pTrophy = 0;

    if (nType >= 1 && nType <= 4) {
        if (nSlot < 0 || nIndex == m_nShown[nSlot]) {
            // Nothing to do if this cup is already shown.
            CChrMenuObj *pChrObj = CChrMenuCtrl::s_Instance.GetChrMenuObj(m_hMdl[1]);
            if (pChrObj != 0 && pChrObj->GetMdl() != 0 &&
                !(pChrObj->GetMdl()->GetFlag() & bcs::CScnObj::FLAG_HIDE_40) && nType == pChrObj->GetChr() &&
                nKind == pChrObj->GetSub()) {
                return 0;
            }
        }
        pTrophy = CreateChrMenuObj(1, nType, nKind);
    }

    if (pTrophy != 0) {
        hMdl = pTrophy->GetHandle();
        bcs::CMdlObj *pStand = m_pTableSet->GetObj()->GetMdl();
        pParent = pTrophy->GetParentObj();
        if (pParent != 0 && pStand != 0) {
            pParent->AttachToObject(pStand, pStand->GetNodeID(s_TrophyNodeName), bcs::CScnObj::ATTACHMODE_0);
        }
        pShown = pTrophy;
    }

    m_hMdl[1] = hMdl;
    CCourtCharaObj *pChara = m_pCharaObj[nIndex];
    if (pChara != 0) {
        pChara->SetNextMdlHandle(hMdl);
        if (pShown != 0) {
            pShown->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            pShown->SetAlphaRateF(0.0f);
        }
        if (nType < 5) {
            pChara->ChangeState(pChara->GetMdlHandle() == 0 ? CCourtCharaObj::STATE_IN
                                                             : CCourtCharaObj::STATE_CHARA_CHANGE,
                                1);
        }
        if (nSlot == 0) {
            m_nShown[nSlot] = nIndex;
        }
    }
    return 1;
}
