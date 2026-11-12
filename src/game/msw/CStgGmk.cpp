#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmkMan.hpp"
#include "game/msw/CStgGmkClusterImpl.hpp"

// msw::CStgGmk: the constructor and the destructor (and with it the
// vtables of CStgGmk and CStgGmk::CNode).

namespace msw {

    // Registers the gimmick with the gimmick manager.
    CStgGmk::CStgGmk(bcs::CTask *pParent, unsigned long arg)
        : CStgObj(pParent, arg)
    {
        m_Node.m_pGmk = this;
        m_pCluster = 0;
        m_nClusterIdx = -1;
        CStgGmkMan::GetInstance()->Regist(&m_Node);
    }

    // A gimmick of a cluster hands itself back to it.
    CStgGmk::~CStgGmk()
    {
        if (m_pCluster && m_nClusterIdx >= 0) {
            m_pCluster->ReleaseGmk(this);
        }
    }
}
