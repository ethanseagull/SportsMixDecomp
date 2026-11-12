// msw::CChrMenuObj and msw::CChrMenuCtrl: the character models of the menus.
//
// The vtables are in .data in the reverse order of the class definitions,
// so CChrMenuObj's header comes first.
#include "game/msw/CChrMenuObj.hpp"
#include "game/msw/CChrMenuCtrl.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrMdlObj.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CLoadData.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/ut/List.hpp"

namespace {

    // The silhouette colours SetSilhouette chooses from, by index (names
    // guessed). The original unit also emits nw4r::ut::Color's empty
    // destructor, and its static initialiser loads its constants in the
    // order a BCS_Color derived from nw4r::ut::Color gives: the base's
    // default constructor stores white, then the bytes are set. Our build
    // also emits a weak implicit BCS_Color destructor the original lacks,
    // which keeps this unit Equivalent rather than Matching.
    bcs::BCS_Color s_SilhouetteColor[4] = {
        bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xFF),
        bcs::BCS_Color(0x60, 0x60, 0x60, 0xFF),
        bcs::BCS_Color(0x69, 0xAD, 0xF5, 0xFF),
        bcs::BCS_Color(0xF7, 0x7B, 0x83, 0xFF),
    };
    bcs::BCS_Color s_SilhouetteBaseColor[4] = {
        bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xFF),
        bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF),
        bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF),
        bcs::BCS_Color(0x00, 0x00, 0x00, 0xFF),
    };
}

msw::CChrMenuCtrl msw::CChrMenuCtrl::s_Instance;

msw::CChrMenuObj::~CChrMenuObj()
{
    CChrMdlObj *pMdl = m_pMdl;
    if (pMdl != 0) {
        pMdl->DetachFromParent();
        if (pMdl != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(pMdl);
        }
        m_pMdl = 0;
    }
    CEffectCtrl::EraseAttachEffect(m_hEffect);
}

void msw::CChrMenuObj::SetSilhouette(long nColor)
{
    CChrMdlObj *pMdl = m_pMdl;
    bcs::BCS_Color color = s_SilhouetteColor[nColor];
    bcs::BCS_Color baseColor = s_SilhouetteBaseColor[nColor];
    if (pMdl != 0) {
        pMdl->SetSilhouetteColor(baseColor, color);
    }
}

void msw::CChrMenuObj::SetAlpha(float alpha)
{
    // (guess) Dead comparison: the original's .rodata has 0.0f ahead of
    // 1.0f although the clamp below tests 1.0f first. Behaviour-free; see
    // "Dead float comparison" in notes/decisions.md.
    if (alpha == 0.0f) {
    }
    if (m_pMdl != 0) {
        if (alpha > 1.0f) {
            alpha = 1.0f;
        } else if (alpha < 0.0f) {
            alpha = 0.0f;
        }
        m_pMdl->SetAlphaRateF(alpha);
    }
}

void msw::CChrMenuObj::AnimPlayMot(long nMot, long nFlag, float rate, float blend, float frame)
{
    bcs::CAnmMotion *pMot = static_cast<bcs::CAnmMotion *>(m_pMdl->GetAnmObjChr());
    if (pMot != 0) {
        pMot->Play(nMot, nFlag, rate, blend, frame);
        m_nMot = nMot;
        m_nMotFlag = nFlag;
        m_fMotRate = rate;
        m_fMotBlend = blend;
        m_fMotFrame = frame;
    }
}

void msw::CChrMenuObj::SetAttachEffect(long nEff)
{
    CChrMdlObj *pMdl = m_pMdl;
    if (pMdl != 0) {
        nw4r::g3d::ResNode node = pMdl->GetResNode("trans");
        if (node.IsValid()) {
            m_hEffect = CEffectCtrl::CreateAttachEffect(nEff, pMdl, 0, node.GetID(), 0, 0, 0, 3, 0, 0, 0);
        }
    }
}

void msw::CChrMenuObj::StopAttachEffect()
{
    CEffectCtrl::StopAttachEffect(m_hEffect);
}

void msw::CChrMenuObj::SetMiiNoface(int bNoFace)
{
    if (m_pMdl != 0) {
        m_pMdl->SetMiiNoFace(bNoFace);
    }
}

msw::CChrMenuCtrl::~CChrMenuCtrl()
{
}

void msw::CChrMenuCtrl::Initialize()
{
    nw4r::ut::List_Init(&m_List, 0);
    m_nCount = 0;
}

void msw::CChrMenuCtrl::Finalize()
{
    CChrMenuObj *pObj = static_cast<CChrMenuObj *>(nw4r::ut::List_GetNext(&m_List, 0));
    while (pObj != 0) {
        CChrMenuObj *pNext = static_cast<CChrMenuObj *>(nw4r::ut::List_GetNext(&m_List, pObj));
        nw4r::ut::List_Remove(&m_List, pObj);
        if (pObj != 0) {
            delete pObj;
        }
        pObj = pNext;
    }
}

msw::CChrMenuObj *msw::CChrMenuCtrl::CreateChrMenuObj(long nChr, long nSub, RFLDataSource source, long nUnk,
                                                      long nLayer)
{
    CChrMenuObj *pObj = new CChrMenuObj;
    if (pObj == 0) {
        return 0;
    }

    CChrMdlObj *pMdl = CreateMdlObj(nChr, nSub, source, nUnk, nLayer);
    if (pMdl == 0) {
        if (pObj != 0) {
            delete pObj;
        }
        return 0;
    }

    pObj->m_pMdl = pMdl;
    pObj->m_nType = 1;
    pObj->m_nChr = nChr;
    pObj->m_nSub = nSub;
    pObj->m_Source = source;
    pObj->m_Unk1C = nUnk;
    pObj->m_nId = m_nCount++;
    nw4r::ut::List_Append(&m_List, pObj);
    return pObj;
}

msw::CChrMenuObj *msw::CChrMenuCtrl::CreateChrMenuObj(long nChr, long nSub, bcs::CMdlObj *pMdl)
{
    if (pMdl == 0) {
        return 0;
    }

    CChrMenuObj *pObj = new CChrMenuObj;
    if (pObj == 0) {
        return 0;
    }

    pObj->m_pMdl = static_cast<CChrMdlObj *>(pMdl);
    pObj->m_nType = 3;
    pObj->m_nChr = nChr;
    pObj->m_nSub = nSub;
    pObj->m_Source = static_cast<RFLDataSource>(6);
    pObj->m_Unk1C = 0;
    pObj->m_nId = m_nCount++;
    nw4r::ut::List_Append(&m_List, pObj);
    return pObj;
}

inline msw::CChrMenuObj *msw::CChrMenuCtrl::Find(unsigned long handle)
{
    CChrMenuObj *pObj = static_cast<CChrMenuObj *>(nw4r::ut::List_GetNext(&m_List, 0));
    while (pObj != 0) {
        // NOTE: infinite loop in the original: an object without a model is
        // never stepped past. A port should advance to the next object here.
        if (pObj->m_pMdl == 0) {
            continue;
        }
        if (handle == pObj->m_pMdl->GetHandle()) {
            return pObj;
        }
        pObj = static_cast<CChrMenuObj *>(nw4r::ut::List_GetNext(&m_List, pObj));
    }
    return 0;
}

msw::CChrMenuObj *msw::CChrMenuCtrl::GetChrMenuObj(unsigned long handle)
{
    return Find(handle);
}

void msw::CChrMenuCtrl::DeleteChrMenuObj(unsigned long handle)
{
    CChrMenuObj *pObj = Find(handle);
    if (pObj != 0) {
        nw4r::ut::List_Remove(&m_List, pObj);
        if (pObj != 0) {
            delete pObj;
        }
    }
}

namespace {

    // (name guessed) The three calls a resource file needs before use.
    inline void SetupResFile(nw4r::g3d::ResFile &res)
    {
        res.CheckRevision();
        res.Init();
        res.Bind(res);
    }
}

msw::CChrMdlObj *msw::CChrMenuCtrl::CreateMdlObj(long nChr, long nSub, RFLDataSource source, long nUnk, long nLayer)
{
    nw4r::g3d::ResFile mdlRes = CLoadData::GetChrMdlRes(0x27, nChr, nSub);
    SetupResFile(mdlRes);
    nw4r::g3d::ResFile motRes = CLoadData::GetChrMotRes(0x28, nChr);
    SetupResFile(motRes);

    CChrMdlObj *pMdl = new CChrMdlObj;
    pMdl->CreateMenuObj(mdlRes, nChr, source, nUnk, 0);
    pMdl->ResetLightSelector();
    pMdl->SetExBlendMode(1);
    pMdl->OnFlag(0x2000);

    bcs::CAnmMotion *pMot = new bcs::CAnmMotion;
    const bcs::CAnmObjMotBase::NODE_EX_INFO *pInfo = CC_DATA::GetMdl_NodeExInfo(nChr, 0);
    pMot->Create(pMdl, motRes, pInfo, 3, 0);
    pMot->OnFlag(8);
    pMot->SetTrgCallback(CChrMdlObj::CallbackMotTrg, pMdl);
    pMdl->AttachAnmObj(pMot, bcs::CMdlObj::ANMOBJTYPE_CHR);
    pMdl->SetDrawLayer(nLayer);
    return pMdl;
}
