#include "game/msw/CEffectTable.hpp"

// (guess) The original destructor stores the vtable pointer, which the
// compiler leaves out of an empty body and keeps when the body calls an
// inline function, even one that compiles to nothing. What the original
// called there is not known (see notes/decisions.md).
static inline void Nothing() {}

namespace msw {

    CEffectTableList::CEffectTableList()
    {
        m_apTable[0] = 0;
        m_apTable[1] = 0;
        m_apTable[2] = 0;
        m_apTable[3] = 0;
        m_nNum = 0;
    }

    CEffectTableList::~CEffectTableList()
    {
        Nothing();
    }
}
