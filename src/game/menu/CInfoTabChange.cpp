#include "game/msw/CInfoTabChange.hpp"

long msw::CInfoTabChange::IncTabNo(long nTabNo)
{
    nTabNo++;
    if (nTabNo >= m_nTabNum) {
        nTabNo = 0;
    }
    m_nTabNo = nTabNo;
    return nTabNo;
}

long msw::CInfoTabChange::DecTabNo(long nTabNo)
{
    nTabNo--;
    if (nTabNo < 0) {
        nTabNo = m_nTabNum - 1;
    }
    m_nTabNo = nTabNo;
    return nTabNo;
}
