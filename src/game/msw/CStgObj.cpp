#include "game/msw/CStgObj.hpp"
#include "game/msw/CStgAct.hpp"
#include "game/msw/CStgMdl.hpp"
#include "bcs/BCS_Util.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgSrt.hpp"

// msw::CStgObj: the constructor, destructor (and with it the vtable) and
// the non-inline virtual functions.

namespace msw {

    CStgObj::CStgObj(bcs::CTask *pParent, unsigned long mdlNum)
        : bcs::CUnivTaskEx(pParent, -1)
        , m_nMdlNum(mdlNum)
        , m_ppMdl(0)
        , m_pAct(0)
        , m_pDrv(0)
        , m_nIdx(-1)
    {
        OnUnk08(1);
        m_ppMdl = new CStgMdl *[mdlNum];
        bcs::BCS_CpuClear32(m_ppMdl, mdlNum * sizeof(CStgMdl *));
    }

    // Deletes the driver, the actor and the models.
    CStgObj::~CStgObj()
    {
        if (m_pDrv) {
            delete m_pDrv;
        }
        m_pDrv = 0;
        if (m_pAct) {
            delete m_pAct;
        }
        m_pAct = 0;
        for (unsigned long i = 0; i < m_nMdlNum; i++) {
            if (m_ppMdl[i]) {
                delete m_ppMdl[i];
            }
            m_ppMdl[i] = 0;
        }
        if (m_ppMdl) {
            delete m_ppMdl;
        }
        m_ppMdl = 0;
    }

    // Makes the models, the actor and the driver, then creates the models
    // and links the parts together.
    void CStgObj::Create()
    {
        for (unsigned long i = 0; i < GetMdlNum(); i++) {
            m_ppMdl[i] = NewMdl(0, i);
            if (m_ppMdl[i]) {
                m_ppMdl[i]->SetIndex(i);
            }
        }
        m_pAct = NewAct(0);
        m_pDrv = NewDrv(0);
        OnPreCreate();
        for (unsigned long i = 0; i < GetMdlNum(); i++) {
            if (m_ppMdl[i]) {
                m_ppMdl[i]->Create();
            }
        }
        if (m_pAct) {
            m_pAct->SetMdl(m_ppMdl[0]);
        }
        if (m_pDrv) {
            m_pDrv->SetAct(m_pAct);
        }
        if (m_pAct) {
            m_pAct->Init();
        }
        OnPostCreate();
    }

    CStgMdl *CStgObj::NewMdl(long heap, unsigned long idx)
    {
        return new (heap) CStgMdl(this);
    }

    CStgAct *CStgObj::NewAct(long heap)
    {
        return new (heap) CStgAct(this);
    }

    CStgDrv *CStgObj::NewDrv(long heap)
    {
        return new (heap) CStgDrv(this);
    }

    void CStgObj::OnPreCreate()
    {
    }

    void CStgObj::OnPostCreate()
    {
    }

    // One frame: the driver, then the actor, then every model.
    void CStgObj::Main()
    {
        float rate = GetFrameRateF();
        if (m_pDrv) {
            m_pDrv->Main(rate);
        }
        if (m_pAct) {
            m_pAct->Update(rate);
        }
        if (m_ppMdl) {
            for (unsigned long i = 0; i < GetMdlNum(); i++) {
                if (m_ppMdl[i]) {
                    m_ppMdl[i]->Main(rate);
                }
            }
        }
    }

    void CStgObj::EventNotify(CStgAct *pAct, long event, void *pArg)
    {
        if (m_pDrv) {
            m_pDrv->EventNotify(pAct, event, pArg);
        }
    }

    void CStgObj::SceneEntry()
    {
        if (m_pAct) {
            m_pAct->UpdateMdlMtx();
        }
        for (unsigned long i = 0; i < GetMdlNum(); i++) {
            if (m_ppMdl[i]) {
                m_ppMdl[i]->SceneEntry();
            }
        }
        OnSceneEntry();
    }

    void CStgObj::OnSceneEntry()
    {
    }

    void CStgObj::SceneLeave()
    {
        OnSceneLeave();
        for (unsigned long i = 0; i < GetMdlNum(); i++) {
            if (m_ppMdl[i]) {
                m_ppMdl[i]->SceneLeave();
            }
        }
    }

    void CStgObj::OnSceneLeave()
    {
    }

    // A checksum of the object's state: the bits of the position and of
    // the front axis of the model matrix, then the object's and the
    // actor's own parts.
    unsigned long CStgObj::CalcSum() const
    {
        unsigned long sum = 0;
        // GetMtx brings the cached matrix up to date first, so the
        // const object's CStgSrt is used as a mutable one.
        CStgSrtMove *pSrt = const_cast<CStgSrtMove *>(GetSrt());
        if (pSrt) {
            bcs::FMTX34 mtx;
            bcs::FMTX34Copy(&mtx, &pSrt->GetMtx());
            const unsigned long *pBits = reinterpret_cast<const unsigned long *>(mtx.m);
            sum += pBits[3];
            sum += pBits[7];
            sum += pBits[11];
            sum += pBits[2];
            sum += pBits[6];
            sum += pBits[10];
        }
        sum += OnCalcSum();
        const CStgAct *pAct = GetAct();
        if (pAct) {
            sum += pAct->CalcSum();
        }
        return sum;
    }
}
