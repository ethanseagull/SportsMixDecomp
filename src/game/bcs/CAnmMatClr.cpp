#include "bcs/CAnmMatClr.hpp"

namespace bcs {

    CAnmMatClr::CAnmMatClr() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmMatClr::~CAnmMatClr()
    {
        Destroy();
    }
}
