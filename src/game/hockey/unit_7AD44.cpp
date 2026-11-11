// msw::CComDrvHockey's message handlers and its skating and turning
// methods. They follow game/hockey/CComDrvHockey.cpp's weak functions in
// the original and have their own float constants, so they were a file of
// their own (name not known).
#include "game/msw/CComDrvHockey.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

long CComDrvHockey::OnInit(_CHR_MSG *pMsg) {
    long nRet = CComDrv::OnInit(pMsg);
    // Outside the party game the keepers are looked up once.
    if (CGame::g_GameWork.GetGameType() == 0) {
        InitEmTeamKeeperCore();
        InitMyTeamKeeperCore();
    }
    return nRet;
}

long CComDrvHockey::OnMain(_CHR_MSG *pMsg) {
    CComDrv::OnMain(pMsg);
    return 0;
}

// Skates towards pos (relative to the driver's court). Returns 1 once the
// character is within fDist of it, else 0. bSkate lets it skate rather
// than run before it has braked or turned.
int CComDrvHockey::MethdSkate(const bcs::FVEC3 &pos, int bSkate, float fDist) {
    bcs::FVEC3 target = Local2World(pos, -1);
    float fRotY = m_pChr->GetRotY_ToPosXZ(target);
    float fRate = 1.0f;

    if (m_pChr->GetDist_ToPosXZ(target) <= fDist) {
        return 1;
    }
    if (m_pChr->MethdSwitchBrake(fRotY, fRate)) {
        return 0;
    }
    if (bSkate && m_pChr->MethdSkate(fRotY, 1, 0.0f, 0)) {
        return 0;
    }
    if (m_pChr->MethdRun(fRotY)) {
        return 0;
    }
    m_pChr->MethdSkate(fRotY, 1, 0.0f, 0);
    return 0;
}

// Turns to the yaw fRot while standing (state 1) or braking (0x43).
// Returns 1 once the yaw is reached.
int CComDrvHockey::MethdRotateY(float fRot, long nRange) {
    long nState = m_pChr->GetState();
    if (nState == 1 || nState == 0x43) {
        if (CComMan::IsRangeRot(m_pChr->GetRotY(), fRot, nRange)) {
            return 1;
        }
        m_pChr->RotateYRate_(fRot, 0.2f);
        if (m_pChr->GetState() == 0x43) {
            m_pChr->MethdBrake(-1.0f, 0);
        }
        return 0;
    }
    return 0;
}

}
