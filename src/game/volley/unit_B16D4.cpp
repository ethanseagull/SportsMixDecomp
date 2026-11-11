// msw::CComDrvVolleyRR's frame and message handlers. A file of its own
// after game/volley/CComDrvVolleyRR.cpp (its own float constants); the file
// name is not known.
#include "game/msw/CComDrvVolleyRR.hpp"

namespace msw {

int CComDrvVolleyRR::IsActiveBallNothingThk() const
{
    return 1;
}

// Every frame: runs the action in progress, or thinks of the next one.
long CComDrvVolleyRR::OnMain(_CHR_MSG *pMsg)
{
    CComDrv::OnMain(pMsg);
    ActInterrupt();
    long nAct = GetCurrentActType();
    if (nAct >= 0) {
        if (ActMain()) {
            ActFunc func = g_ActFuncMap[nAct];
            (this->*func)();
        }
    } else {
        ThkMain();
    }
    return 0;
}

// The home positions: four in a row across the court.
long CComDrvVolleyRR::OnInit(_CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnInit(pMsg);
    for (int i = 0; i < 4; i++) {
        m_aHomePos[i].x = -3.75f + 2.5f * i;
        m_aHomePos[i].y = 0.0f;
        m_aHomePos[i].z = 6.0f;
    }
    return nRet;
}

long CComDrvVolleyRR::OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg)
{
    long nRet = CComDrv::OnGameMsgProc(id, pMsg);
    if (id == 0x8C) {
        nRet = 1;
    }
    return nRet;
}

void CComDrvVolleyRR::OnAlarm1(long nAlarm)
{
    m_nUnk1A0 = 1;
    SetAlarm(nAlarm, 3600.0f);
}

// Breaks off for a ball to receive, or a coin to pick up.
void CComDrvVolleyRR::ActInterrupt()
{
    if (GetRootActType() != ACT_RECEIVE_GO) {
        bcs::FVEC3 pos;
        if (GetMyBallPos(pos)) {
            Reset();
            ActCallReceiveGo();
            return;
        }
    }
    if (GetRootActType() != ACT_RECEIVE_GO && GetRootActType() != ACT_GET_DROP_COIN && GetShoudGetDropCoin()) {
        Reset();
        ActCallGetDropCoin();
    }
}

void CComDrvVolleyRR::ThkMain()
{
    bcs::FVEC3 pos;
    if (GetMyBallPos(pos)) {
        ActCallReceiveGo();
    } else if (GetShoudGetDropCoin()) {
        ActCallGetDropCoin();
    } else {
        ActCallGoHome();
    }
}
}
