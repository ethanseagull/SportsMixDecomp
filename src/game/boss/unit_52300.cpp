// msw::CComDrvBoss's methods (MethdServe to MethdRelayRun), the ones its
// actions call. A file of its own (own float constants; CComManBoss's
// GetTouchDownPos, in the file before, is called, not inlined); the file
// name is not known.
#include "game/msw/CComDrvBoss.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComManBoss.hpp"

namespace msw {

int CComDrvBoss::MethdServe()
{
    return m_pChr->MethdServe(0);
}

int CComDrvBoss::CanbeReceive()
{
    if (CanbeDiveReceive()) {
        return 0;
    }
    return m_pChr->CanbeReceive(0);
}

int CComDrvBoss::MethdReceive()
{
    int bRet = 0;
    if (CanbeReceive()) {
        bRet = m_pChr->MethdReceive(0, 0);
    }
    return bRet;
}

// A dive when the character can dive or the ball comes down more than 0.7
// away.
int CComDrvBoss::CanbeDiveReceive()
{
    if (m_pBall == 0) {
        return 0;
    }
    if (m_pChr->IsVolleyEnableDiveReceive()) {
        return m_pChr->CanbeDiveReceive();
    }
    bcs::FVEC3 pos;
    CComManBoss::GetTouchDownPos(m_pBall, pos);
    return GetDisXZ(pos, m_MyPos) > 0.7f;
}

int CComDrvBoss::MethdDiveReceive()
{
    int bRet = 0;
    if (CanbeDiveReceive()) {
        bRet = m_pChr->MethdReceive(0, 0);
    }
    return bRet;
}

int CComDrvBoss::CanbeShootStance()
{
    return m_pChr->CanbeShootStance();
}

int CComDrvBoss::MethdShootStance()
{
    return m_pChr->MethdShootStance(-1.0f, 0);
}

int CComDrvBoss::CanbeDunk()
{
    return m_pChr->CanbeDunk();
}

int CComDrvBoss::MethdDunk()
{
    return m_pChr->MethdDunk();
}

void CComDrvBoss::ResetRelayRun()
{
    m_nRelayStep = -1;
}

// Runs to rPos, round the Behemoth by way of a relay point when it is in
// between (with a wider margin there). Not zero once at rPos itself.
int CComDrvBoss::MethdRelayRun(const bcs::FVEC3 &rPos, float fRange)
{
    bcs::FVEC3 relay;
    int bRelay = Util_GetRelayPosForBoss(relay, m_MyPos, rPos);
    if (bRelay) {
        if (m_nRelayStep < 0) {
            m_nRelayStep = 1;
            m_RelayPos = relay;
        }
    } else {
        m_nRelayStep = 0;
        m_RelayPos = rPos;
    }
    if (m_nRelayStep >= 1) {
        fRange = 2.0f;
    }
    if (MethdRun(m_RelayPos, fRange, 0)) {
        if (!bRelay && m_nRelayStep <= 0) {
            return 1;
        }
        return 0;
    }
    return 0;
}

} // namespace msw
