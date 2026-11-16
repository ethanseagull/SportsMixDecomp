// msw::CComDrvHockeyGame's special defence: what a computer character does
// against a special shot, per character (name of the file not known).
// SpDefenseInit_CHR_TYPE_<chr> draws the parameters from the computer data
// when the action starts, SpDefenseMain_CHR_TYPE_<chr> runs it every frame
// in one of two formations. Most characters share SpDefenseMain_FormationA
// and B; Luigi, Wario, Koopa, Ninja and Siroma have their own.
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManHockey.hpp"
#include "bcs/BCS_Math.hpp"
#include "nw4r/math/triangular.h"

namespace {

    // The distance between two points in the XZ plane (name guessed;
    // both are copied).
    inline float DisXZ(bcs::FVEC3 pos0, bcs::FVEC3 pos1)
    {
        return msw::CComMan::GetDisXZ(pos0, pos1);
    }

}

namespace msw {

// A point near the driver's goal in front of the puck holder (in the
// sector the parameters give); 0 when nobody holds the puck.
inline int CComDrvHockeyGame::GetHoldBallSectorPosition(float fMin, float fMax, float fDeg, bcs::FVEC3 &rPos) {
    const CChrCore *pHold = GetHoldBallCore();
    if (pHold == 0) {
        return 0;
    }
    GetRandomSectorPositionToGoal(pHold, 1, fMin, fMax, fDeg, rPos);
    return 1;
}

// Turns to face the shooter; 1 when facing it.
inline int CComDrvHockeyGame::SpDefenseTurnToShooter(SP_DEFENSE_WORK *pWork) {
    const bcs::FVEC3 &rPos = pWork->m_pShooter->GetPos();
    bcs::FVEC3 dir;
    // (y first: the original stores it first)
    dir.y = rPos.y;
    dir.x = rPos.x - m_MyPos.x;
    dir.z = rPos.z - m_MyPos.z;
    return MethdRotY(bcs::ROUND_PI(nw4r::math::Atan2Rad(dir.x, dir.z)));
}

// Tries a diving steal once each time one becomes possible, as often as
// the computer data says.
inline void CComDrvHockeyGame::SpDefenseDivingSteal(SP_DEFENSE_WORK *pWork) {
    if (m_pChr->CanbeDivingSteal(0)) {
        if (pWork->m_bDivingStealLot) {
            pWork->m_bDivingStealLot = 0;
            if (LotRate() < CComMan::GetDatI(0xA08, GetHockeyComLvF())) {
                m_pChr->MethdDivingSteal(-1.0f, 0, 0);
            }
        }
    } else {
        pWork->m_bDivingStealLot = 1;
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_MARIO() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xA18, fLv));
    if (LotRate() < CComMan::GetDatI(0xA10, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xA20, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xA28, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xA30, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xA38, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xA40, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xA48, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xA50, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_MARIO() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_LUIGI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xA60, fLv));
    if (LotRate() < CComMan::GetDatI(0xA58, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xA68, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xA70, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xA78, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xA80, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xA88, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xA90, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xA98, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_LUIGI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA_LUIGI();
    } else {
        SpDefenseMain_FormationB_LUIGI();
    }
}

// Formation A for Luigi: as SpDefenseMain_FormationA, keeping where he
// started braking to skate back to when there is no special puck.
void CComDrvHockeyGame::SpDefenseMain_FormationA_LUIGI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
    case 0:
        ActCallGoSpDefenseGoal(pWork->m_fWait, pWork->m_fGoal50, pWork->m_fGoal54, pWork->m_fGoal58, pWork->m_fSectorDeg);
        pWork->m_nStep++;
        break;
    case 1:
        pWork->m_SavePos = m_MyPos;
        pWork->m_nStep++;
        // fall through
    case 2:
        m_pChr->MethdBrake(-1.0f, 0);
        pWork->m_nStep++;
        break;
    case 3:
        if (SpDefenseTurnToShooter(pWork)) {
            pWork->m_nStep++;
        }
        break;
    case 4: {
        CBall *pBall = GetSpBallCore();
        if (pBall != 0) {
            bcs::FVEC3 pos = pBall->GetPos();
            MethdMove(pos, 2.0f);
        } else {
            MethdMove(pWork->m_SavePos, 2.0f);
        }
        break;
    }
    }
    SpDefenseDivingSteal(pWork);
}

// Formation B for Luigi: as SpDefenseMain_FormationB, skating back to the
// point near the goal when there is no special puck.
void CComDrvHockeyGame::SpDefenseMain_FormationB_LUIGI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1:
        if (GetHoldBallSectorPosition(pWork->m_fSectorMin, pWork->m_fSectorMax, pWork->m_fSectorDeg, pWork->m_TargetPos)) {
            pWork->m_SavePos = pWork->m_TargetPos;
            pWork->m_nStep++;
        } else {
            ReturnAct(0);
            return;
        }
        break;
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            pWork->m_nStep++;
        }
        break;
    case 3:
        if (SpDefenseTurnToShooter(pWork)) {
            pWork->m_nStep++;
        }
        break;
    case 4: {
        CBall *pBall = GetSpBallCore();
        if (pBall != 0) {
            pWork->m_TargetPos = pBall->GetPos();
            MethdMove(pWork->m_TargetPos, 2.0f);
        } else {
            MethdMove(pWork->m_SavePos, 2.0f);
        }
        break;
    }
    }
    SpDefenseDivingSteal(pWork);
}


void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_PEACH() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xAA8, fLv));
    if (LotRate() < CComMan::GetDatI(0xAA0, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xAB0, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xAB8, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xAC0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xAC8, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xAD0, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xAD8, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xAE0, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_PEACH() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_DAISY() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xAF0, fLv));
    if (LotRate() < CComMan::GetDatI(0xAE8, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xAF8, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xB00, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xB08, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xB10, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xB18, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xB20, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xB28, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_DAISY() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_YOSHI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xB38, fLv));
    if (LotRate() < CComMan::GetDatI(0xB30, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xB40, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xB48, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xB50, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xB58, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xB60, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xB68, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xB70, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_YOSHI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_WARIO() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xB80, fLv));
    if (LotRate() < CComMan::GetDatI(0xB78, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xB88, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xB90, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xB98, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xBA0, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xBA8, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xBB0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xBB8, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_WARIO() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    long bSmoke = isEnterSmokeScreen();

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA_WARIO(bSmoke);
    } else {
        SpDefenseMain_FormationB_WARIO(bSmoke);
    }
}

// Formation A for Wario: in a smoke screen he neither chases the special
// puck nor tries a diving steal.
void CComDrvHockeyGame::SpDefenseMain_FormationA_WARIO(long bSmoke) {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
    case 0:
        ActCallGoSpDefenseGoal(pWork->m_fWait, pWork->m_fGoal50, pWork->m_fGoal54, pWork->m_fGoal58, pWork->m_fSectorDeg);
        pWork->m_nStep++;
        break;
    case 1:
        m_pChr->MethdBrake(-1.0f, 0);
        pWork->m_nStep++;
        break;
    case 2:
        if (SpDefenseTurnToShooter(pWork)) {
            pWork->m_nStep++;
        }
        break;
    case 3: {
        CBall *pBall = GetSpBallCore();
        if (pBall != 0) {
            bcs::FVEC3 pos = pBall->GetPos();
            if (bSmoke != 1) {
                MethdMove(pos, 2.0f);
            }
        }
        break;
    }
    }
    if (bSmoke != 1) {
        SpDefenseDivingSteal(pWork);
    }
}

// Formation B for Wario, with the same smoke screen exceptions.
void CComDrvHockeyGame::SpDefenseMain_FormationB_WARIO(long bSmoke) {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1:
        if (GetHoldBallSectorPosition(pWork->m_fSectorMin, pWork->m_fSectorMax, pWork->m_fSectorDeg, pWork->m_TargetPos)) {
            pWork->m_nStep++;
        } else {
            ReturnAct(0);
            return;
        }
        break;
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            pWork->m_nStep++;
        }
        break;
    case 3:
        if (SpDefenseTurnToShooter(pWork)) {
            pWork->m_nStep++;
        }
        break;
    case 4: {
        CBall *pBall = GetSpBallCore();
        if (pBall != 0) {
            pWork->m_TargetPos = pBall->GetPos();
            if (bSmoke != 1) {
                MethdMove(pWork->m_TargetPos, 2.0f);
            }
        }
        break;
    }
    }
    if (bSmoke != 1) {
        SpDefenseDivingSteal(pWork);
    }
}


void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_WALUIGI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xBC8, fLv));
    if (LotRate() < CComMan::GetDatI(0xBC0, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xBD0, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xBD8, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xBE0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xBE8, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xBF0, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xBF8, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xC00, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_WALUIGI() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_DONKEY() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xC10, fLv));
    if (LotRate() < CComMan::GetDatI(0xC08, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xC18, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xC20, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xC28, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xC30, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xC38, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xC40, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xC48, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_DONKEY() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_DIDDY() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xC58, fLv));
    if (LotRate() < CComMan::GetDatI(0xC50, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xC60, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xC68, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xC70, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xC78, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xC80, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xC88, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xC90, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_DIDDY() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_KINOPIO() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xCA0, fLv));
    if (LotRate() < CComMan::GetDatI(0xC98, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xCA8, fLv);
        // 0xC68 (Diddy's entry; 0xCB0 would be Kinopio's) as in the
        // original.
        pWork->m_fGoal54 = CComMan::GetDatF(0xC68, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xCB8, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xCC0, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xCC8, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xCD0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xCD8, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_KINOPIO() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_KOOPA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xCE8, fLv));
    if (LotRate() < CComMan::GetDatI(0xCE0, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (LotRate() < CComMan::GetDatI(0xCF0, fLv)) {
        pWork->m_bAvoidFire = 1;
    } else {
        pWork->m_bAvoidFire = 0;
    }
    if (LotRate() < CComMan::GetDatI(0xCF8, fLv)) {
        pWork->m_bReturn = 1;
    } else {
        pWork->m_bReturn = 0;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xD00, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xD08, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xD10, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xD18, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xD20, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xD28, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xD30, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_KOOPA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA_KOOPA();
    } else {
        SpDefenseMain_FormationB_KOOPA();
    }
}

// Formation A for Koopa: the driver's rank by distance to its own goal
// picks how far in front of the goal (by the puck holder's distance to it)
// Koopa takes position; he can keep out of the fire circle and, with no
// special puck, skate back to that position.
void CComDrvHockeyGame::SpDefenseMain_FormationA_KOOPA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 pos;

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1: {
        long nRank = GetMostNearMyGoal();
        if (nRank < 0) {
            ReturnAct(0);
            return;
        }
        float fRate;
        if (nRank == 0) {
            fRate = pWork->m_fGoal50;
        } else if (nRank == 1) {
            fRate = pWork->m_fGoal54;
        } else {
            fRate = pWork->m_fGoal58;
        }
        const CChrCore *pHold = GetHoldBallCore();
        if (pHold == 0) {
            ReturnAct(0);
            return;
        }
        bcs::FVEC3 holdPos = CComMan::GetChrWorldPos(pHold);
        float fDis = fRate * (DisXZ(holdPos, m_GuardGoalPos) / 10.0f);
        GetRandomSectorPositionToGoal(pHold, 1, fDis, fDis, pWork->m_fSectorDeg, pWork->m_SavePos);
        pWork->m_TargetPos = pWork->m_SavePos;
        if (pWork->m_bAvoidFire == 1) {
            if (isEnterFireCircle(pWork->m_TargetPos, pos) == 1) {
                pWork->m_TargetPos = pos;
            }
        }
        pWork->m_nStep++;
    }
        // fall through
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            pWork->m_nStep++;
        }
        break;
    case 3: {
        const bcs::FVEC3 &rPos = pWork->m_pShooter->GetPos();
        bcs::FVEC3 dir;
        dir.y = rPos.y;
        dir.x = rPos.x - m_MyPos.x;
        dir.z = rPos.z - m_MyPos.z;
        if (MethdRotY(bcs::ROUND_PI(nw4r::math::Atan2Rad(dir.x, dir.z)))) {
            pWork->m_nStep++;
        }
        break;
    }
    case 4:
        if (pWork->m_bAvoidFire == 0) {
            CBall *pBall = GetSpBallCore();
            if (pBall != 0) {
                pos = pBall->GetPos();
                MethdMove(pos, 2.0f);
            } else if (MethdMove(pWork->m_SavePos, 2.0f)) {
                m_pChr->MethdBrake(-1.0f, 0);
            }
        } else {
            long nFire = isEnterFireCircle(m_MyPos, pos);
            if (nFire == -1 || nFire == 0) {
                CBall *pBall = GetSpBallCore();
                if (pBall != 0) {
                    pos = pBall->GetPos();
                    MethdMove(pos, 2.0f);
                } else if (pWork->m_bReturn == 0) {
                    m_pChr->MethdBrake(-1.0f, 0);
                } else if (MethdMove(pWork->m_SavePos, 2.0f)) {
                    m_pChr->MethdBrake(-1.0f, 0);
                }
            } else {
                pWork->m_TargetPos = pos;
                if (pWork->m_bReturn == 1) {
                    if (isEnterFireCircle(pWork->m_SavePos, pos) != 1) {
                        pWork->m_TargetPos = pWork->m_SavePos;
                    }
                }
                pWork->m_nStep++;
            }
        }
        break;
    case 5:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            pWork->m_nStep = 4;
        }
        break;
    }
    SpDefenseDivingSteal(pWork);
}

// Formation B for Koopa: as SpDefenseMain_FormationB, with the fire circle
// and the way back of formation A.
void CComDrvHockeyGame::SpDefenseMain_FormationB_KOOPA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 dir;
    bcs::FVEC3 pos;

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1:
        if (GetHoldBallSectorPosition(pWork->m_fSectorMin, pWork->m_fSectorMax, pWork->m_fSectorDeg, pWork->m_SavePos)) {
            pWork->m_TargetPos = pWork->m_SavePos;
            if (pWork->m_bAvoidFire == 1) {
                if (isEnterFireCircle(pWork->m_TargetPos, pos) == 1) {
                    pWork->m_TargetPos = pos;
                }
            }
            pWork->m_nStep++;
        } else {
            ReturnAct(0);
            return;
        }
        break;
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            pWork->m_nStep++;
        }
        break;
    case 3: {
        const bcs::FVEC3 &rPos = pWork->m_pShooter->GetPos();
        dir.y = rPos.y;
        dir.x = rPos.x - m_MyPos.x;
        dir.z = rPos.z - m_MyPos.z;
        if (MethdRotY(bcs::ROUND_PI(nw4r::math::Atan2Rad(dir.x, dir.z)))) {
            pWork->m_nStep++;
        }
        break;
    }
    case 4:
        if (pWork->m_bAvoidFire == 0) {
            CBall *pBall = GetSpBallCore();
            if (pBall != 0) {
                pos = pBall->GetPos();
                MethdMove(pos, 2.0f);
            } else if (MethdMove(pWork->m_SavePos, 2.0f)) {
                m_pChr->MethdBrake(-1.0f, 0);
            }
        } else {
            long nFire = isEnterFireCircle(m_MyPos, pos);
            if (nFire == -1 || nFire == 0) {
                CBall *pBall = GetSpBallCore();
                if (pBall != 0) {
                    pos = pBall->GetPos();
                    MethdMove(pos, 2.0f);
                } else if (pWork->m_bReturn == 0) {
                    m_pChr->MethdBrake(-1.0f, 0);
                } else if (MethdMove(pWork->m_SavePos, 2.0f)) {
                    m_pChr->MethdBrake(-1.0f, 0);
                }
            } else {
                pWork->m_EscPos = pos;
                if (pWork->m_bReturn == 1) {
                    if (isEnterFireCircle(pWork->m_SavePos, pos) != 1) {
                        pWork->m_EscPos = pWork->m_SavePos;
                    }
                }
                pWork->m_nStep++;
            }
        }
        break;
    case 5:
        if (MethdMove(pWork->m_EscPos, 2.0f)) {
            pWork->m_nStep = 4;
        }
        break;
    }
    SpDefenseDivingSteal(pWork);
}


void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_KOOPAJR() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xD40, fLv));
    if (LotRate() < CComMan::GetDatI(0xD38, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xD50, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xD58, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xD60, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xD68, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xD70, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xD78, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xD80, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_KOOPAJR() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_MOOGLE() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xD90, fLv));
    if (LotRate() < CComMan::GetDatI(0xD88, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xD98, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xDA0, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xDA8, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xDB0, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xDB8, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xDC0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xDC8, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_MOOGLE() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_SABOTEN() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xDD8, fLv));
    if (LotRate() < CComMan::GetDatI(0xDD0, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xDE0, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xDE8, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xDF0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xDF8, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xE00, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xE08, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xE10, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_SABOTEN() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_NINJA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xE28, fLv));
    if (LotRate() < CComMan::GetDatI(0xE20, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (LotRate() < CComMan::GetDatI(0xE18, fLv)) {
        pWork->m_nNinjaMode = 2;
    } else if (LotRate() < 50) {
        pWork->m_nNinjaMode = 0;
    } else {
        pWork->m_nNinjaMode = 1;
    }
    float fUnused;
    GetAttack1Param(GetChrTypeID(), pWork->m_fAttackDist, pWork->m_fAttackWidth, fUnused);
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xE30, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xE38, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xE40, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xE48, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xE50, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xE58, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xE60, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_NINJA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA_NINJA();
    } else {
        SpDefenseMain_FormationB_NINJA();
    }
}

// Formation A for Ninja: once facing the shooter with clones out, she
// blocks at a clone (or at the shooter while it holds the puck) with an
// attack whenever it is in reach. No diving steal.
void CComDrvHockeyGame::SpDefenseMain_FormationA_NINJA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 pos;
    bcs::FVEC3 clonePos0;
    bcs::FVEC3 clonePos1;
    long nClone = GetCloneNinja(clonePos0, clonePos1);

    switch (pWork->m_nStep) {
    case 0:
        ActCallGoSpDefenseGoal(pWork->m_fWait, pWork->m_fGoal50, pWork->m_fGoal54, pWork->m_fGoal58, pWork->m_fSectorDeg);
        pWork->m_nStep++;
        break;
    case 1:
        m_pChr->MethdBrake(-1.0f, 0);
        pWork->m_nStep++;
        break;
    case 2:
        pos = pWork->m_pShooter->GetPos();
        pos.x -= m_MyPos.x;
        pos.z -= m_MyPos.z;
        if (MethdRotY(bcs::ROUND_PI(nw4r::math::Atan2Rad(pos.x, pos.z))) && nClone > 0) {
            if (pWork->m_nNinjaMode == 2) {
                pWork->m_nStep++;
            } else {
                pWork->m_nStep = 4;
            }
        }
        break;
    case 3: {
        const CChrCore *pHold = GetHoldBallCore();
        if (pHold != 0 && pHold == pWork->m_pShooter) {
            pos = pWork->m_pShooter->GetPos();
            MethdMove(pos, 2.0f);
            MethdMoveRevise(pos);
            if (CComManHockey::IsExistTargetCore(m_pChr, pWork->m_pShooter, pWork->m_fAttackDist, pWork->m_fAttackWidth)) {
                m_pChr->MethdBlockAttack(0);
            }
        }
        break;
    }
    case 4:
        if (nClone == 2) {
            if (pWork->m_nNinjaMode == 0) {
                pos = clonePos0;
            } else {
                pos = clonePos1;
            }
            MethdMove(pos, 2.0f);
            MethdMoveRevise(pos);
            if (CComManHockey::IsExistTargetPos(m_pChr, pos, pWork->m_fAttackDist, pWork->m_fAttackWidth)) {
                m_pChr->MethdBlockAttack(0);
            }
        } else if (nClone == 1) {
            pos = clonePos0;
            MethdMove(pos, 2.0f);
            MethdMoveRevise(pos);
            if (CComManHockey::IsExistTargetPos(m_pChr, pos, pWork->m_fAttackDist, pWork->m_fAttackWidth)) {
                m_pChr->MethdBlockAttack(0);
            }
        } else {
            m_pChr->MethdBrake(-1.0f, 0);
        }
        break;
    }
}

// Formation B for Ninja: skates to the point near the goal, then blocks as
// in formation A.
void CComDrvHockeyGame::SpDefenseMain_FormationB_NINJA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 clonePos0;
    bcs::FVEC3 clonePos1;
    long nClone = GetCloneNinja(clonePos0, clonePos1);
    bcs::FVEC3 pos;

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1:
        if (GetHoldBallSectorPosition(pWork->m_fSectorMin, pWork->m_fSectorMax, pWork->m_fSectorDeg, pWork->m_TargetPos)) {
            pWork->m_nStep++;
        } else {
            ReturnAct(0);
            return;
        }
        break;
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            if (nClone > 0) {
                if (pWork->m_nNinjaMode == 2) {
                    pWork->m_nStep++;
                } else {
                    pWork->m_nStep = 4;
                }
            }
        }
        break;
    case 3: {
        const CChrCore *pHold = GetHoldBallCore();
        if (pHold != 0 && pHold == pWork->m_pShooter) {
            pos = pWork->m_pShooter->GetPos();
            MethdMove(pos, 2.0f);
            MethdMoveRevise(pos);
            if (CComManHockey::IsExistTargetCore(m_pChr, pWork->m_pShooter, pWork->m_fAttackDist, pWork->m_fAttackWidth)) {
                m_pChr->MethdBlockAttack(0);
            }
        }
        break;
    }
    case 4:
        if (nClone == 2) {
            if (pWork->m_nNinjaMode == 0) {
                pos = clonePos0;
            } else {
                pos = clonePos1;
            }
            MethdMove(pos, 2.0f);
            MethdMoveRevise(pos);
            if (CComManHockey::IsExistTargetPos(m_pChr, pos, pWork->m_fAttackDist, pWork->m_fAttackWidth)) {
                m_pChr->MethdBlockAttack(0);
            }
        } else if (nClone == 1) {
            pos = clonePos0;
            MethdMove(pos, 2.0f);
            MethdMoveRevise(pos);
            if (CComManHockey::IsExistTargetPos(m_pChr, pos, pWork->m_fAttackDist, pWork->m_fAttackWidth)) {
                m_pChr->MethdBlockAttack(0);
            }
        } else {
            m_pChr->MethdBrake(-1.0f, 0);
        }
        break;
    }
}


void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_SIROMA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xE70, fLv));
    if (LotRate() < CComMan::GetDatI(0xE68, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (LotRate() < CComMan::GetDatI(0xE78, fLv)) {
        pWork->m_bReturn = 1;
    } else {
        pWork->m_bReturn = 0;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xE80, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xE88, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xE90, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xE98, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xEA0, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xEA8, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xEB0, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_SIROMA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA_SIROMA();
    } else {
        SpDefenseMain_FormationB_SIROMA();
    }
}

// Formation A for Siroma: as SpDefenseMain_FormationA, keeping out of the
// magic circle and, with no special puck, holding or going back to where
// she braked.
void CComDrvHockeyGame::SpDefenseMain_FormationA_SIROMA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 pos;

    switch (pWork->m_nStep) {
    case 0:
        ActCallGoSpDefenseGoal(pWork->m_fWait, pWork->m_fGoal50, pWork->m_fGoal54, pWork->m_fGoal58, pWork->m_fSectorDeg);
        pWork->m_nStep++;
        break;
    case 1:
        pWork->m_SavePos = m_MyPos;
        pWork->m_nStep++;
        // fall through
    case 2:
        m_pChr->MethdBrake(-1.0f, 0);
        pWork->m_nStep++;
        break;
    case 3: {
        const bcs::FVEC3 &rPos = pWork->m_pShooter->GetPos();
        bcs::FVEC3 dir;
        dir.y = rPos.y;
        dir.x = rPos.x - m_MyPos.x;
        dir.z = rPos.z - m_MyPos.z;
        if (MethdRotY(bcs::ROUND_PI(nw4r::math::Atan2Rad(dir.x, dir.z)))) {
            pWork->m_nStep++;
        }
        break;
    }
    case 4: {
        long nMagic = isEnterMagicCircle(m_MyPos, pos);
        if (nMagic == -1 || nMagic == 0) {
            CBall *pBall = GetSpBallCore();
            if (pBall != 0) {
                pos = pBall->GetPos();
                MethdMove(pos, 2.0f);
            } else if (pWork->m_bReturn == 0) {
                if (nMagic == 0) {
                    m_pChr->MethdBrake(-1.0f, 0);
                } else {
                    MethdMove(pWork->m_SavePos, 2.0f);
                }
            } else if (MethdMove(pWork->m_SavePos, 2.0f)) {
                m_pChr->MethdBrake(-1.0f, 0);
            }
        } else {
            pWork->m_TargetPos = pos;
            if (pWork->m_bReturn == 1) {
                if (isEnterMagicCircle(pWork->m_SavePos, pos) != 1) {
                    pWork->m_TargetPos = pWork->m_SavePos;
                }
            }
            pWork->m_nStep++;
        }
        break;
    }
    case 5:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            pWork->m_nStep = 4;
        }
        break;
    }
    SpDefenseDivingSteal(pWork);
}

// Formation B for Siroma: as SpDefenseMain_FormationB, with the magic
// circle of formation A.
void CComDrvHockeyGame::SpDefenseMain_FormationB_SIROMA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    bcs::FVEC3 dir;
    bcs::FVEC3 pos;

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1:
        if (GetHoldBallSectorPosition(pWork->m_fSectorMin, pWork->m_fSectorMax, pWork->m_fSectorDeg, pWork->m_SavePos)) {
            pWork->m_TargetPos = pWork->m_SavePos;
            pWork->m_nStep++;
        } else {
            ReturnAct(0);
            return;
        }
        break;
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            pWork->m_nStep++;
        }
        break;
    case 3: {
        const bcs::FVEC3 &rPos = pWork->m_pShooter->GetPos();
        dir.y = rPos.y;
        dir.x = rPos.x - m_MyPos.x;
        dir.z = rPos.z - m_MyPos.z;
        if (MethdRotY(bcs::ROUND_PI(nw4r::math::Atan2Rad(dir.x, dir.z)))) {
            pWork->m_nStep++;
        }
        break;
    }
    case 4: {
        long nMagic = isEnterMagicCircle(m_MyPos, pos);
        if (nMagic == -1 || nMagic == 0) {
            CBall *pBall = GetSpBallCore();
            if (pBall != 0) {
                pos = pBall->GetPos();
                MethdMove(pos, 2.0f);
            } else if (pWork->m_bReturn == 0) {
                if (nMagic == 0) {
                    m_pChr->MethdBrake(-1.0f, 0);
                } else {
                    MethdMove(pWork->m_SavePos, 2.0f);
                }
            } else if (MethdMove(pWork->m_SavePos, 2.0f)) {
                m_pChr->MethdBrake(-1.0f, 0);
            }
        } else {
            pWork->m_EscPos = pos;
            if (pWork->m_bReturn == 1) {
                if (isEnterMagicCircle(pWork->m_SavePos, pos) != 1) {
                    pWork->m_EscPos = pWork->m_SavePos;
                }
            }
            pWork->m_nStep++;
        }
        break;
    }
    case 5:
        if (MethdMove(pWork->m_EscPos, 2.0f)) {
            pWork->m_nStep = 4;
        }
        break;
    }
    SpDefenseDivingSteal(pWork);
}


void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_SLIME() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xEC0, fLv));
    if (LotRate() < CComMan::GetDatI(0xEB8, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xEC8, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xED0, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xED8, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xEE0, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xEE8, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xEF0, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xEF8, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_SLIME() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}

void CComDrvHockeyGame::SpDefenseInit_CHR_TYPE_KUROMA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());
    float fLv = GetHockeyComLvF();

    pWork->m_fWait = CComManHockey::GetRandValueF(0.0f, CComMan::GetDatF(0xF08, fLv));
    if (LotRate() < CComMan::GetDatI(0xF00, fLv)) {
        pWork->m_nFormation = 0;
    } else {
        pWork->m_nFormation = 1;
    }
    if (pWork->m_nFormation == 0) {
        pWork->m_fGoal50 = CComMan::GetDatF(0xF10, fLv);
        pWork->m_fGoal54 = CComMan::GetDatF(0xF18, fLv);
        pWork->m_fGoal58 = CComMan::GetDatF(0xF20, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xF28, fLv);
    } else {
        pWork->m_fSectorMin = CComMan::GetDatF(0xF30, fLv);
        pWork->m_fSectorMax = CComMan::GetDatF(0xF38, fLv);
        pWork->m_fSectorDeg = CComMan::GetDatF(0xF40, fLv);
        if (pWork->m_fWait == 0.0f) {
            pWork->m_nStep = 1;
        }
    }
}

void CComDrvHockeyGame::SpDefenseMain_CHR_TYPE_KUROMA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    if (pWork->m_nFormation == 0) {
        SpDefenseMain_FormationA();
    } else {
        SpDefenseMain_FormationB();
    }
}


// Formation A: skate to the goal, brake, face the shooter, then chase the
// special puck.
void CComDrvHockeyGame::SpDefenseMain_FormationA() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
    case 0:
        ActCallGoSpDefenseGoal(pWork->m_fWait, pWork->m_fGoal50, pWork->m_fGoal54, pWork->m_fGoal58, pWork->m_fSectorDeg);
        pWork->m_nStep++;
        break;
    case 1:
        m_pChr->MethdBrake(-1.0f, 0);
        pWork->m_nStep++;
        break;
    case 2:
        if (SpDefenseTurnToShooter(pWork)) {
            pWork->m_nStep++;
        }
        break;
    case 3: {
        CBall *pBall = GetSpBallCore();
        if (pBall != 0) {
            bcs::FVEC3 pos = pBall->GetPos();
            MethdMove(pos, 2.0f);
        }
        break;
    }
    }
    SpDefenseDivingSteal(pWork);
}

// Formation B: wait, skate to a point near the goal in front of the puck
// holder, brake, face the shooter, then chase the special puck.
void CComDrvHockeyGame::SpDefenseMain_FormationB() {
    SP_DEFENSE_WORK *pWork = static_cast<SP_DEFENSE_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
    case 0:
        pWork->m_fWaitTime += m_pChr->GetFrameStep();
        if (pWork->m_fWaitTime >= pWork->m_fWait) {
            pWork->m_nStep++;
        }
        break;
    case 1:
        if (GetHoldBallSectorPosition(pWork->m_fSectorMin, pWork->m_fSectorMax, pWork->m_fSectorDeg, pWork->m_TargetPos)) {
            pWork->m_nStep++;
        } else {
            ReturnAct(0);
            return;
        }
        break;
    case 2:
        if (MethdMove(pWork->m_TargetPos, 2.0f)) {
            m_pChr->MethdBrake(-1.0f, 0);
            pWork->m_nStep++;
        }
        break;
    case 3:
        if (SpDefenseTurnToShooter(pWork)) {
            pWork->m_nStep++;
        }
        break;
    case 4: {
        CBall *pBall = GetSpBallCore();
        if (pBall != 0) {
            pWork->m_TargetPos = pBall->GetPos();
            MethdMove(pWork->m_TargetPos, 2.0f);
        }
        break;
    }
    }
    SpDefenseDivingSteal(pWork);
}

}
