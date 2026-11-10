#include "game/msw/CStgObj.hpp"

namespace msw {

    CStgDrv::CStgDrv(CStgObj *pParent)
        : m_pParent(pParent)
        , m_pAct(0)
    {
    }

    CStgDrv::~CStgDrv()
    {
    }
}
