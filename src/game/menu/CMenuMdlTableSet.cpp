// msw::CTableSetMdlObj and msw::CMenuMdlTableSet.
#include "game/msw/CMenuMdlTableSet.hpp"

#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmObj.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/ut/List.hpp"

msw::CTableSetMdlObj::CTableSetMdlObj(bcs::CMdlObj *pMdl)
{
    m_pMdl = pMdl;
    m_bOwnMdl = 1;
}

msw::CTableSetMdlObj::~CTableSetMdlObj()
{
    Finalize();
}

void msw::CTableSetMdlObj::Finalize()
{
    if (m_bOwnMdl != 0) {
        bcs::CMdlObj *pMdl = m_pMdl;
        if (pMdl != 0) {
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
            pMdl->DetachFromParent();
            if (pMdl != 0) {
                bcs::CScnObj::AppendDelayDeleteObject(pMdl);
            }
            m_pMdl = 0;
        }
    }
}

int msw::CTableSetMdlObj::IsAnmEnd()
{
    bcs::CAnmObj *pAnmObj = m_pMdl->GetAnmObjChr();
    if (pAnmObj == 0) {
        return 1;
    }
    return pAnmObj->IsEnd();
}

int msw::CTableSetMdlObj::IsAnmLooped()
{
    bcs::CAnmObj *pAnmObj = m_pMdl->GetAnmObjChr();
    if (pAnmObj == 0) {
        return 1;
    }
    return pAnmObj->IsLooped();
}

void msw::CTableSetMdlObj::SetVisible(int bVisible)
{
    bcs::CMdlObj *pMdl = m_pMdl;

    if (bVisible != 1) {
        pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    } else {
        pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    }
}

void msw::CTableSetMdlObj::GetNodePosition(const char *pName, bcs::FVEC3 &rPos)
{
    bcs::FMTX34 mtx;

    m_pMdl->GetNodeWorldMtx(m_pMdl->GetNodeID(pName), mtx);
    rPos.Set(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
}

void msw::CTableSetMdlObj::Play(nw4r::g3d::ResAnmClr res, float rate)
{
    // (guessed) The original has a float comparison here that leaves no
    // instruction: the file's 1.0f constant is created before the 0.0f that
    // CMenuMdlTableSet::MainLoop is the first to load, and no function
    // before MainLoop loads a float. A comparison whose result is unused
    // does exactly that. Which comparison, and in which of the functions
    // above MainLoop, is not known; this parameter is otherwise unused.
    if (rate == 1.0f) {
    }

    if (res.IsValid()) {
        bcs::CAnmObj *pOld = m_pMdl->GetAnmObjMatClr();
        if (pOld != 0) {
            m_pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_MATCLR);
            if (pOld != 0) {
                delete pOld;
            }
        }

        bcs::CAnmMatClr *pAnm = new bcs::CAnmMatClr;
        if (pAnm->Create(m_pMdl, res, 0, 0) == 1) {
            m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
        } else if (pAnm != 0) {
            delete pAnm;
        }
    }
}

void msw::CTableSetMdlObj::Play(nw4r::g3d::ResAnmTexSrt res, float)
{
    if (res.IsValid()) {
        bcs::CAnmObj *pOld = m_pMdl->GetAnmObjTexSrt();
        if (pOld != 0) {
            m_pMdl->DetachAnmObj(bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
            if (pOld != 0) {
                delete pOld;
            }
        }

        bcs::CAnmTexSrt *pAnm = new bcs::CAnmTexSrt;
        if (pAnm->Create(m_pMdl, res, 0, 0) == 1) {
            m_pMdl->AttachAnmObj(pAnm, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
        } else if (pAnm != 0) {
            delete pAnm;
        }
    }
}

msw::CMenuMdlTableSet::CMenuMdlTableSet()
    : m_pObj(0)
    , m_ResFile(0)
{
    m_pAnimTable = 0;
    m_nAnimNum = 0;
    nw4r::ut::List_Init(&m_List, 0);
    ClearList();
    m_bOwnObj = 0;
}

msw::CMenuMdlTableSet::~CMenuMdlTableSet()
{
    ClearList();
    if (m_pObj != 0 && m_bOwnObj != 0) {
        delete m_pObj;
        m_pObj = 0;
    }
}

void msw::CMenuMdlTableSet::MainLoop()
{
    CTableSetMdlObj *pObj = m_pObj;
    if (pObj == 0) {
        return;
    }

    // The original keeps the old value in f31 and the new one in f1, and
    // writes the unchanged value back when no time is left.
    float fWait = m_fWait;
    if (fWait > 0.0f) {
        float fNext = fWait - bcs::CMainApp::GetAppFrameRateF();
        if (fNext <= 0.0f) {
            pObj->StopAnm();
            fNext = 0.0f;
        }
        m_fWait = fNext;
        return;
    }
    m_fWait = fWait;

    if (pObj->IsAnmVFunc3C() || pObj->IsAnmEnd()) {
        CChrMenuAnimInfoLink *pLink = static_cast<CChrMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, 0));
        if (pLink != 0) {
            nw4r::ut::List_Remove(&m_List, pLink);
        }
        if (pLink != 0) {
            PlayQueued(pLink->m_nAnim, pLink->m_fRate, pLink->m_fFrame);
            delete pLink;
        }
    }
}

int msw::CMenuMdlTableSet::SetAnimTable(const AnimInfo *pTable, long num)
{
    m_pAnimTable = pTable;
    m_nAnimNum = num;
    return 0;
}

long msw::CMenuMdlTableSet::Play(long nAnim, float rate, float frame, unsigned long flag, float blend)
{
    ClearList();
    return PlayImpl(nAnim, rate, frame, flag, blend);
}

long msw::CMenuMdlTableSet::PlayImpl(long nAnim, float rate, float frame, unsigned long flag, float blend)
{
    CTableSetMdlObj *pObj = m_pObj;
    if (pObj == 0) {
        return -1;
    }
    if (nAnim < 0 || nAnim >= m_nAnimNum) {
        return -2;
    }

    const AnimInfo *pInfo = &m_pAnimTable[nAnim];
    unsigned long playFlag = flag;
    float playRate;

    if (pInfo->bLoop) {
        playFlag |= 2;
    }
    if (pInfo->bReverse) {
        playRate = -rate;
        frame = static_cast<float>(GetResAnimMot(pInfo->pName, m_ResFile).GetNumFrame()) - 1.0f - frame;
        if (frame < 0.0f) {
            frame = 0.0f;
        }
    } else {
        playRate = rate;
    }

    if (blend <= 0.0f) {
        pObj->Play(GetResAnimMot(pInfo->pName, m_ResFile), playFlag, playRate, frame);
    } else {
        pObj->Play(GetResAnimMot(pInfo->pName, m_ResFile), playFlag, playRate, blend, frame);
    }
    return 0;
}

void msw::CMenuMdlTableSet::PushBack(long nAnim, float rate, float frame)
{
    nw4r::ut::List_Append(&m_List, new CChrMenuAnimInfoLink(nAnim, rate, frame));
}

void msw::CMenuMdlTableSet::ClearList()
{
    CChrMenuAnimInfoLink *pNext;
    CChrMenuAnimInfoLink *pLink = static_cast<CChrMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, 0));
    while (pLink != 0) {
        pNext = static_cast<CChrMenuAnimInfoLink *>(nw4r::ut::List_GetNext(&m_List, pLink));
        nw4r::ut::List_Remove(&m_List, pLink);
        delete pLink;
        pLink = pNext;
    }
}

nw4r::g3d::ResAnmChr msw::CMenuMdlTableSet::GetResAnimMot(const char *pName, nw4r::g3d::ResFile file)
{
    if (!file.IsValid()) {
        return nw4r::g3d::ResAnmChr(0);
    }
    return file.GetResAnmChr(pName);
}

nw4r::g3d::ResAnmClr msw::CMenuMdlTableSet::GetResAnimClr(const char *pName, nw4r::g3d::ResFile file)
{
    if (!file.IsValid()) {
        return nw4r::g3d::ResAnmClr(0);
    }
    return file.GetResAnmClr(pName);
}

nw4r::g3d::ResAnmTexSrt msw::CMenuMdlTableSet::GetResAnimTsa(const char *pName, nw4r::g3d::ResFile file)
{
    if (!file.IsValid()) {
        return nw4r::g3d::ResAnmTexSrt(0);
    }
    return file.GetResAnmTexSrt(pName);
}
