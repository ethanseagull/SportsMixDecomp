#include "bcs/CAnmTexSrt.hpp"

namespace bcs {

    CAnmTexSrt::CAnmTexSrt() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmTexSrt::~CAnmTexSrt()
    {
        Destroy();
    }
}
