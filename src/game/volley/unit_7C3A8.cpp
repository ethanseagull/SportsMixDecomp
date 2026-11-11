// msw::CComDrvVolley's serve and receive methods. With the move assist on
// the driver does nothing of its own (ReturnAutoRunMode); otherwise it asks
// the character to serve or receive.
#include "game/msw/CComDrvVolley.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComManVolley.hpp"

namespace msw {

int CComDrvVolley::MethdServe()
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }
    return m_pChr->MethdServe(0);
}

int CComDrvVolley::CanbeReceive()
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }
    // A dive receive is not a receive.
    if (CanbeDiveReceive()) {
        return 0;
    }
    return m_pChr->CanbeReceive(0);
}

int CComDrvVolley::MethdReceive()
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }
    int ret = 0;
    if (CanbeReceive()) {
        ret = m_pChr->MethdReceive(0, 0);
    }
    return ret;
}

int CComDrvVolley::CanbeDiveReceive()
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }
    if (m_pChr->IsVolleyEnableDiveReceive()) {
        return m_pChr->CanbeDiveReceive();
    }
    // Otherwise only when the ball comes down more than 0.7 away.
    bcs::FVEC3 pos;
    CComManVolley::GetTouchDownPos(m_pBall, pos);
    return GetDisXZ(pos, m_MyPos) > 0.7f;
}

int CComDrvVolley::MethdDiveReceive()
{
    if (m_pChr->IsMoveAssist()) {
        return ReturnAutoRunMode();
    }
    int ret = 0;
    if (CanbeDiveReceive()) {
        ret = m_pChr->MethdReceive(0, 0);
    }
    return ret;
}

} // namespace msw
