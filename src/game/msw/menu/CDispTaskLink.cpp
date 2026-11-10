#include "game/msw/menu/CDispTaskList.hpp"

namespace msw {
    namespace menu {

        unsigned long CDispTaskLink::s_nNextId = 1;

        CDispTaskLink::CDispTaskLink()
        {
            m_nKind = 0;
            unsigned long id = s_nNextId++;
            if (s_nNextId == 0) {
                s_nNextId = 1;
            }
            m_nId = id;
            m_bEnd = 1;
            m_Unk1C = 0;
            m_Unk18 = 0;
            m_Pos = bcs::FVEC3(0.0f, 0.0f, 0.0f);
        }

        CDispTaskLink::~CDispTaskLink()
        {
        }

        void CDispTaskLink::StatusCheck(CMenuPaneAccessor *pAccessor)
        {
        }

        void CDispTaskLink::Disp(CMenuPaneAccessor *pAccessor, long)
        {
        }
    }
}
