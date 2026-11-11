#include "bcs/CAnmTexPat.hpp"

namespace bcs {

    CAnmTexPat::CAnmTexPat() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmTexPat::CAnmTexPat(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexPat res, int arg2, long arg3)
        : m_pResult(0), m_nResultNum(0)
    {
        Create(pMdl, res, arg2, arg3);
    }

    CAnmTexPat::~CAnmTexPat()
    {
        Destroy();
    }
}
