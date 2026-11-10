#include "game/msw/CCockpitPartMove.hpp"

namespace msw {

    long CCockpitPartMove::Push(long nPart, long nArg0, long nArg1)
    {
        PartMoveInfo *pInfo = new PartMoveInfo;
        if (pInfo == 0) {
            return -1;
        }
        pInfo->m_nPart = nPart;
        pInfo->m_nArg0 = nArg0;
        pInfo->m_nArg1 = nArg1;
        nw4r::ut::List_Append(&m_List, pInfo);
        return 0;
    }
}
