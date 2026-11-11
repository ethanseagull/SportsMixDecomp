#include "game/msw/CStgGmkClusterImpl.hpp"
#include "game/msw/CStgGmk.hpp"

// The base of every gimmick cluster: a table of GetTblSize() gimmicks that
// NewGmk makes, that are created, entered into the scene and summed
// together. Constructor, destructor and vtable (DOL lbl_8048A3F0) are part
// of this file; the inline virtual functions the vtable names are emitted
// first by game/msw/CStgGmk_WaterPillarCluster.cpp.

namespace msw {

    CStgGmkClusterImpl::CStgGmkClusterImpl(bcs::CTask *pParent)
    {
        m_bCreated = 0;
        m_pParent = pParent;
        m_ppGmk = 0;
        m_nTblSize = 0;
    }

    CStgGmkClusterImpl::~CStgGmkClusterImpl()
    {
        m_bCreated = 0;
        if (m_ppGmk != 0) {
            delete[] m_ppGmk;
        }
        m_ppGmk = 0;
    }

    void CStgGmkClusterImpl::CreateGmk()
    {
        DeleteGmk();

        long num = GetMaxNum();
        if (num <= 0) {
            return;
        }

        m_ppGmk = new (1) CStgGmk *[num];
        m_nTblSize = num;
        m_bCreated = 1;

        for (long i = 0; i < num; i++) {
            CStgGmk *pGmk = NewGmk(0);
            m_ppGmk[i] = pGmk;
            if (pGmk != 0) {
                pGmk->SetCluster(this, i);
                pGmk->Create();
                pGmk->SceneEntry();
            }
        }

        OnPostCreateGmk();
    }

    void CStgGmkClusterImpl::DeleteGmk()
    {
        m_bCreated = 0;
        if (m_ppGmk == 0) {
            return;
        }

        OnPreDeleteGmk();
        if (m_ppGmk == 0) {
            return;
        }

        long num = GetTblSize();
        for (long i = 0; i < num; i++) {
            if (m_ppGmk[i] != 0) {
                m_ppGmk[i]->Delete();
            }
            m_ppGmk[i] = 0;
        }

        if (m_ppGmk != 0) {
            delete[] m_ppGmk;
        }
        m_ppGmk = 0;
        m_nTblSize = 0;
    }

    void CStgGmkClusterImpl::ReleaseGmk(CStgGmk *pGmk)
    {
        if (pGmk == 0 || m_ppGmk == 0 || m_bCreated == 0) {
            return;
        }

        if (m_ppGmk != 0) {
            long num = GetTblSize();
            for (long i = 0; i < num; i++) {
                if (m_ppGmk[i] == pGmk) {
                    m_ppGmk[i] = 0;
                    break;
                }
            }
        }

        if (pGmk != 0) {
            pGmk->SetCluster(0, -1);
        }
    }

    void CStgGmkClusterImpl::ReleaseGmk(long idx)
    {
        if (m_ppGmk == 0 || m_bCreated == 0) {
            return;
        }

        if (m_ppGmk != 0) {
            CStgGmk *pGmk = At(idx);
            if (pGmk != 0) {
                pGmk->SetCluster(0, -1);
            }
            m_ppGmk[idx] = 0;
        }
    }

    CStgGmk *CStgGmkClusterImpl::GetGmk_(long idx)
    {
        return At(idx);
    }

    const CStgGmk *CStgGmkClusterImpl::GetGmk_(long idx) const
    {
        return At(idx);
    }

    void CStgGmkClusterImpl::Main(float rate)
    {
        OnUpdate(rate);
    }

    unsigned long CStgGmkClusterImpl::CalcSum() const
    {
        long num = GetTblSize();
        unsigned long sum = num;
        for (long i = 0; i < num; i++) {
            const CStgGmk *pGmk = GetGmk_(i);
            if (pGmk != 0) {
                sum += pGmk->CalcSum();
            }
        }
        return sum + OnCalcSum();
    }
}
