// msw::CComDrvVolleyRR's own actions (g_ActFuncMap from 10 on), started by
// ActCall* and run every frame by ActLoop*. A file of its own before
// game/volley/CComDrvVolleyRR.cpp (its own float constants, padded at the
// end); the file name is not known.
#include "game/msw/CComDrvVolleyRR.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComManVolleyRR.hpp"
#include "game/msw/CDropCoin.hpp"

namespace {

    // (names guessed) The work of the actions that run to a place: where
    // to, and the ball or the coin they go for.
    struct RECEIVE_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Target;   // 0x1C
        long m_nBall;          // 0x28, the ball's number; -1 for none yet
    };

    struct COIN_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Target;   // 0x1C
        unsigned long m_nCoin; // 0x28, the coin's task id
    };
}

namespace msw {

void CComDrvVolleyRR::ActCallGoHome()
{
    EntryAct(ACT_GO_HOME);
}

// Runs to the home position and faces the front.
void CComDrvVolleyRR::ActLoopGoHome()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2:
            GetHomePos(pWork->m_Target);
            if (MethdRunEx(pWork->m_Target, 0.6f, 1.0f, 600.0f, 7, 10.0f)) {
                pWork->m_nStep++;
            }
            break;
        case 3:
            ActCallLookFront();
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvVolleyRR::ActLoopReceiveReady()
{
    GetCurrentActWork();
}

void CComDrvVolleyRR::ActCallReceiveGo()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(EntryAct(ACT_RECEIVE_GO));
    pWork->m_nBall = -1;
}

// Runs to where the ball comes down and receives it, diving when it is a
// little too far.
void CComDrvVolleyRR::ActLoopReceiveGo()
{
    RECEIVE_WORK *pWork = static_cast<RECEIVE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 pos;
    CBall *pMyBall = GetMyBallPos(pos);
    if (pMyBall == 0) {
        ReturnAct(0);
        return;
    }

    // Keep to the ball chosen first unless it is gone or still far off.
    long nMyBall = pMyBall->GetBallNo();
    if (CComManVolleyRR::GetValidPtr(pWork->m_nBall) == 0) {
        pWork->m_nBall = nMyBall;
    }
    CBall *pBall = CComManVolleyRR::GetValidPtr(pWork->m_nBall);
    if (pBall != 0 && pBall->GetTouchDownFrame(0.3f) >= 60.0f) {
        pWork->m_nBall = nMyBall;
        pBall = CComManVolleyRR::GetValidPtr(nMyBall);
    }
    if (pBall == 0) {
        ReturnAct(0);
        return;
    }

    CComManVolley::GetBallTargetPos(pBall, pWork->m_Target);
    float fDist = CComMan::GetDisXZ(m_MyPos, pWork->m_Target);
    float fFrame = pBall->GetTouchDownFrame(0.3f);
    if (fFrame <= 8.0f && fDist < 4.0f && fDist > 2.5f) {
        if (m_pChr->IsVolleyEnableDiveReceive() && MethdDiveReceive()) {
            ReturnAct(1);
            return;
        }
    } else if (fFrame <= 8.0f && fDist < 2.0f) {
        if (MethdReceive()) {
            ReturnAct(1);
            return;
        }
    }

    m_nFlag &= ~4;
    m_fUnk54 = 0.0f;
    MethdRunEx(pWork->m_Target, 0.6f, 1.0f, 600.0f, 7, 10.0f);
}

void CComDrvVolleyRR::ActCallGetDropCoin()
{
    EntryAct(ACT_GET_DROP_COIN);
}

// Runs to the coin GetShoudGetDropCoin chose while it is there.
void CComDrvVolleyRR::ActLoopGetDropCoin()
{
    COIN_WORK *pWork = static_cast<COIN_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            m_nFlag &= ~4;
            m_fUnk54 = 0.0f;
            pWork->m_nCoin = GetShoudGetDropCoin();
            if (pWork->m_nCoin == 0) {
                ReturnAct(0);
                break;
            }
            pWork->m_nStep++;
            break;
        case 1: {
            CDropCoin *pCoin = CComMan::DropCoin_GetPtr(pWork->m_nCoin);
            if (pCoin == 0) {
                ReturnAct(1);
                break;
            }
            if (!pCoin->CanbePickup()) {
                ReturnAct(1);
                break;
            }
            pWork->m_Target = CComMan::DropCoin_GetPos(pCoin);
            if (MethdRunEx(pWork->m_Target, 0.3f, 1.0f, 600.0f, 7, 10.0f)) {
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}
}
