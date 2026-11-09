// msw::CTableSetMdlObj and msw::CMenuMdlTableSet.
#include "game/msw/CTableSetMdlObj.hpp"

#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmObj.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/FVEC3.hpp"

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

void msw::CTableSetMdlObj::Play(nw4r::g3d::ResAnmClr res, float)
{
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
