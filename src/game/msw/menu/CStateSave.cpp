#include "game/msw/menu/CStateSave.hpp"

namespace msw {
    namespace menu {

        CStateSave *CStateSave::GetInstance()
        {
            return &s_Instance;
        }

        // Defined above the (empty) destructor, so the static initialiser
        // registers it with __register_global_object, as in the original.
        CStateSave CStateSave::s_Instance;

        CStateSave::~CStateSave()
        {
        }

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
