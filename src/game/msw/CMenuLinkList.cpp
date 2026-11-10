#include "game/msw/CMenuLinkList.hpp"

#include "game/msw/CMenuLinkBase.hpp"

namespace msw {

    void CMenuLinkList::RemoveObject(CMenuLinkBase *pObj)
    {
        CMenuLinkBase *pLink = static_cast<CMenuLinkBase *>(nw4r::ut::List_GetNext(&m_List, 0));
        while (pLink != 0) {
            if (pLink == pObj) {
                nw4r::ut::List_Remove(&m_List, pLink);
                delete pLink;
                break;
            }
            pLink = static_cast<CMenuLinkBase *>(nw4r::ut::List_GetNext(&m_List, pLink));
        }
    }
}
