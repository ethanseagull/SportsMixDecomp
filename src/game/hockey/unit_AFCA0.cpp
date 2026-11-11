// msw::CComDrvHockeyGame's skating and shooting moves. A file of its own:
// its float constants (1, 0, -1) are pooled again after those of
// game/hockey/unit_AE7E8.cpp, which a single file would have shared (name
// not known).
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"

namespace msw {

// Skates in the direction fRotY: brakes to turn when the character can
// (always when it may brake to the full turn, otherwise as often as the
// character's computer data says), else skates or runs.
int CComDrvHockeyGame::MethdMoveMain(float fRotY) {
    float fRate = 1.0f;

    if (m_pChr->CanbeSwitchBrake(fRotY, 1) && m_pChr->MethdSwitchBrake(fRotY, fRate)) {
        return 0;
    }
    if (m_pChr->CanbeSwitchBrake(fRotY, 0)) {
        if (m_bSwitchBrakeLot == 0) {
            m_bSwitchBrakeLot = 1;
            // The character's rate, scaled by the computer level's.
            long nRate = (long)(CComMan::GetDatF(0x1055, GetChrTypeID()) *
                                CComMan::GetDatF(0x106A, GetHockeyComLvF()));
            if (nRate > 100) {
                nRate = 100;
            } else {
                nRate = nRate < 0 ? 0 : nRate;
            }
            if (LotRate() < nRate) {
                m_bSwitchBrake = 1;
            } else {
                m_bSwitchBrake = 0;
            }
        }
        if (m_bSwitchBrake == 1 && m_pChr->MethdSwitchBrake(fRotY, fRate)) {
            return 0;
        }
    } else {
        m_bSwitchBrake = 0;
        m_bSwitchBrakeLot = 0;
    }

    if (m_pChr->MethdSkate(fRotY, 1, 0.0f, 0)) {
        return 0;
    }
    m_pChr->MethdRun(fRotY);
    return 0;
}

int CComDrvHockeyGame::MethdMoveRevise(const bcs::FVEC3 &pos) {
    CChrCore *pChr = m_pChr;
    return pChr->MethdHockeySetMoveRevise(pChr->GetRotY_ToPosXZ(pos), 0.0f);
}

int CComDrvHockeyGame::MethdRotY(float fRotY) {
    m_bRotY = 1;
    return MethdRotateY(fRotY, 20);
}

int CComDrvHockeyGame::MethdFeintShoot() {
    if (m_pChr->MethdFeintShoot(-1.0f)) {
        m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
        return 1;
    }
    return 0;
}

int CComDrvHockeyGame::MethdShoot() {
    if (m_pChr->MethdShoot(-1.0f)) {
        m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
        return 1;
    }
    return 0;
}

int CComDrvHockeyGame::MethdItemBallShoot() {
    if (m_pChr->MethdItemBallShoot(-1.0f)) {
        m_fShootWait = CComMan::GetDatF(0x119, GetHockeyComLvF());
        return 1;
    }
    return 0;
}

}
