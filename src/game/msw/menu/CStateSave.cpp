#include "game/msw/menu/CStateSave.hpp"

namespace msw {
    namespace menu {

        CStateSave *CStateSave::GetInstance()
        {
            return &s_Instance;
        }

        // Defined after GetInstance: its destructor is emitted at this point.
        CStateSave CStateSave::s_Instance;

        void CStateSave::Initialize()
        {
            m_bActive = 1;
            m_nStartMenuMode = 0;
        }

        void CStateSave::Finalize()
        {
            m_bActive = 0;
        }
    }
}
