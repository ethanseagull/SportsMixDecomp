// msw::CComDrvHockeyGame's per-character attack parameters and its
// questions about the puck in flight. A file of its own between
// game/hockey/CBallMarkerHockey.cpp and game/hockey/unit_EEA14.cpp (name not
// known).
#include "game/msw/CComDrvHockeyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

// The computer data holds nine values per character (from id 0x5FE on):
// three for each of the three attacks. Unknown characters get the first
// character's.
void CComDrvHockeyGame::GetAttack1Param(long nChr, float &rParam0, float &rParam1, float &rParam2) {
    switch (nChr) {
        default:
        case 0:
            rParam0 = CComMan::GetDatF(0x5FE, 0L);
            rParam1 = CComMan::GetDatF(0x5FF, 0L);
            rParam2 = CComMan::GetDatF(0x600, 0L);
            break;
        case 1:
            rParam0 = CComMan::GetDatF(0x607, 0L);
            rParam1 = CComMan::GetDatF(0x608, 0L);
            rParam2 = CComMan::GetDatF(0x609, 0L);
            break;
        case 2:
            rParam0 = CComMan::GetDatF(0x610, 0L);
            rParam1 = CComMan::GetDatF(0x611, 0L);
            rParam2 = CComMan::GetDatF(0x612, 0L);
            break;
        case 3:
            rParam0 = CComMan::GetDatF(0x619, 0L);
            rParam1 = CComMan::GetDatF(0x61A, 0L);
            rParam2 = CComMan::GetDatF(0x61B, 0L);
            break;
        case 4:
            rParam0 = CComMan::GetDatF(0x622, 0L);
            rParam1 = CComMan::GetDatF(0x623, 0L);
            rParam2 = CComMan::GetDatF(0x624, 0L);
            break;
        case 5:
            rParam0 = CComMan::GetDatF(0x62B, 0L);
            rParam1 = CComMan::GetDatF(0x62C, 0L);
            rParam2 = CComMan::GetDatF(0x62D, 0L);
            break;
        case 6:
            rParam0 = CComMan::GetDatF(0x634, 0L);
            rParam1 = CComMan::GetDatF(0x635, 0L);
            rParam2 = CComMan::GetDatF(0x636, 0L);
            break;
        case 7:
            rParam0 = CComMan::GetDatF(0x63D, 0L);
            rParam1 = CComMan::GetDatF(0x63E, 0L);
            rParam2 = CComMan::GetDatF(0x63F, 0L);
            break;
        case 8:
            rParam0 = CComMan::GetDatF(0x646, 0L);
            rParam1 = CComMan::GetDatF(0x647, 0L);
            rParam2 = CComMan::GetDatF(0x648, 0L);
            break;
        case 9:
            rParam0 = CComMan::GetDatF(0x64F, 0L);
            rParam1 = CComMan::GetDatF(0x650, 0L);
            rParam2 = CComMan::GetDatF(0x651, 0L);
            break;
        case 10:
            rParam0 = CComMan::GetDatF(0x658, 0L);
            rParam1 = CComMan::GetDatF(0x659, 0L);
            rParam2 = CComMan::GetDatF(0x65A, 0L);
            break;
        case 11:
            rParam0 = CComMan::GetDatF(0x661, 0L);
            rParam1 = CComMan::GetDatF(0x662, 0L);
            rParam2 = CComMan::GetDatF(0x663, 0L);
            break;
        case 12:
            rParam0 = CComMan::GetDatF(0x66A, 0L);
            rParam1 = CComMan::GetDatF(0x66B, 0L);
            rParam2 = CComMan::GetDatF(0x66C, 0L);
            break;
        case 13:
            rParam0 = CComMan::GetDatF(0x673, 0L);
            rParam1 = CComMan::GetDatF(0x674, 0L);
            rParam2 = CComMan::GetDatF(0x675, 0L);
            break;
        case 14:
            rParam0 = CComMan::GetDatF(0x67C, 0L);
            rParam1 = CComMan::GetDatF(0x67D, 0L);
            rParam2 = CComMan::GetDatF(0x67E, 0L);
            break;
        case 15:
            rParam0 = CComMan::GetDatF(0x685, 0L);
            rParam1 = CComMan::GetDatF(0x686, 0L);
            rParam2 = CComMan::GetDatF(0x687, 0L);
            break;
        case 16:
            rParam0 = CComMan::GetDatF(0x68E, 0L);
            rParam1 = CComMan::GetDatF(0x68F, 0L);
            rParam2 = CComMan::GetDatF(0x690, 0L);
            break;
        case 17:
            rParam0 = CComMan::GetDatF(0x697, 0L);
            rParam1 = CComMan::GetDatF(0x698, 0L);
            rParam2 = CComMan::GetDatF(0x699, 0L);
            break;
    }
}

void CComDrvHockeyGame::GetAttack2Param(long nChr, float &rParam0, float &rParam1, float &rParam2) {
    switch (nChr) {
        default:
        case 0:
            rParam0 = CComMan::GetDatF(0x601, 0L);
            rParam1 = CComMan::GetDatF(0x602, 0L);
            rParam2 = CComMan::GetDatF(0x603, 0L);
            break;
        case 1:
            rParam0 = CComMan::GetDatF(0x60A, 0L);
            rParam1 = CComMan::GetDatF(0x60B, 0L);
            rParam2 = CComMan::GetDatF(0x60C, 0L);
            break;
        case 2:
            rParam0 = CComMan::GetDatF(0x613, 0L);
            rParam1 = CComMan::GetDatF(0x614, 0L);
            rParam2 = CComMan::GetDatF(0x615, 0L);
            break;
        case 3:
            rParam0 = CComMan::GetDatF(0x61C, 0L);
            rParam1 = CComMan::GetDatF(0x61D, 0L);
            rParam2 = CComMan::GetDatF(0x61E, 0L);
            break;
        case 4:
            rParam0 = CComMan::GetDatF(0x625, 0L);
            rParam1 = CComMan::GetDatF(0x626, 0L);
            rParam2 = CComMan::GetDatF(0x627, 0L);
            break;
        case 5:
            rParam0 = CComMan::GetDatF(0x62E, 0L);
            rParam1 = CComMan::GetDatF(0x62F, 0L);
            rParam2 = CComMan::GetDatF(0x630, 0L);
            break;
        case 6:
            rParam0 = CComMan::GetDatF(0x637, 0L);
            rParam1 = CComMan::GetDatF(0x638, 0L);
            rParam2 = CComMan::GetDatF(0x639, 0L);
            break;
        case 7:
            rParam0 = CComMan::GetDatF(0x640, 0L);
            rParam1 = CComMan::GetDatF(0x641, 0L);
            rParam2 = CComMan::GetDatF(0x642, 0L);
            break;
        case 8:
            rParam0 = CComMan::GetDatF(0x649, 0L);
            rParam1 = CComMan::GetDatF(0x64A, 0L);
            rParam2 = CComMan::GetDatF(0x64B, 0L);
            break;
        case 9:
            rParam0 = CComMan::GetDatF(0x652, 0L);
            rParam1 = CComMan::GetDatF(0x653, 0L);
            rParam2 = CComMan::GetDatF(0x654, 0L);
            break;
        case 10:
            rParam0 = CComMan::GetDatF(0x65B, 0L);
            rParam1 = CComMan::GetDatF(0x65C, 0L);
            rParam2 = CComMan::GetDatF(0x65D, 0L);
            break;
        case 11:
            rParam0 = CComMan::GetDatF(0x664, 0L);
            rParam1 = CComMan::GetDatF(0x665, 0L);
            rParam2 = CComMan::GetDatF(0x666, 0L);
            break;
        case 12:
            rParam0 = CComMan::GetDatF(0x66D, 0L);
            rParam1 = CComMan::GetDatF(0x66E, 0L);
            rParam2 = CComMan::GetDatF(0x66F, 0L);
            break;
        case 13:
            rParam0 = CComMan::GetDatF(0x676, 0L);
            rParam1 = CComMan::GetDatF(0x677, 0L);
            rParam2 = CComMan::GetDatF(0x678, 0L);
            break;
        case 14:
            rParam0 = CComMan::GetDatF(0x67F, 0L);
            rParam1 = CComMan::GetDatF(0x680, 0L);
            rParam2 = CComMan::GetDatF(0x681, 0L);
            break;
        case 15:
            rParam0 = CComMan::GetDatF(0x688, 0L);
            rParam1 = CComMan::GetDatF(0x689, 0L);
            rParam2 = CComMan::GetDatF(0x68A, 0L);
            break;
        case 16:
            rParam0 = CComMan::GetDatF(0x691, 0L);
            rParam1 = CComMan::GetDatF(0x692, 0L);
            rParam2 = CComMan::GetDatF(0x693, 0L);
            break;
        case 17:
            rParam0 = CComMan::GetDatF(0x69A, 0L);
            rParam1 = CComMan::GetDatF(0x69B, 0L);
            rParam2 = CComMan::GetDatF(0x69C, 0L);
            break;
    }
}

void CComDrvHockeyGame::GetAttack3Param(long nChr, float &rParam0, float &rParam1, float &rParam2) {
    switch (nChr) {
        default:
        case 0:
            rParam0 = CComMan::GetDatF(0x604, 0L);
            rParam1 = CComMan::GetDatF(0x605, 0L);
            rParam2 = CComMan::GetDatF(0x606, 0L);
            break;
        case 1:
            rParam0 = CComMan::GetDatF(0x60D, 0L);
            rParam1 = CComMan::GetDatF(0x60E, 0L);
            rParam2 = CComMan::GetDatF(0x60F, 0L);
            break;
        case 2:
            rParam0 = CComMan::GetDatF(0x616, 0L);
            rParam1 = CComMan::GetDatF(0x617, 0L);
            rParam2 = CComMan::GetDatF(0x618, 0L);
            break;
        case 3:
            rParam0 = CComMan::GetDatF(0x61F, 0L);
            rParam1 = CComMan::GetDatF(0x620, 0L);
            rParam2 = CComMan::GetDatF(0x621, 0L);
            break;
        case 4:
            rParam0 = CComMan::GetDatF(0x628, 0L);
            rParam1 = CComMan::GetDatF(0x629, 0L);
            rParam2 = CComMan::GetDatF(0x62A, 0L);
            break;
        case 5:
            rParam0 = CComMan::GetDatF(0x631, 0L);
            rParam1 = CComMan::GetDatF(0x632, 0L);
            rParam2 = CComMan::GetDatF(0x633, 0L);
            break;
        case 6:
            rParam0 = CComMan::GetDatF(0x63A, 0L);
            rParam1 = CComMan::GetDatF(0x63B, 0L);
            rParam2 = CComMan::GetDatF(0x63C, 0L);
            break;
        case 7:
            rParam0 = CComMan::GetDatF(0x643, 0L);
            rParam1 = CComMan::GetDatF(0x644, 0L);
            rParam2 = CComMan::GetDatF(0x645, 0L);
            break;
        case 8:
            rParam0 = CComMan::GetDatF(0x64C, 0L);
            rParam1 = CComMan::GetDatF(0x64D, 0L);
            rParam2 = CComMan::GetDatF(0x64E, 0L);
            break;
        case 9:
            rParam0 = CComMan::GetDatF(0x655, 0L);
            rParam1 = CComMan::GetDatF(0x656, 0L);
            rParam2 = CComMan::GetDatF(0x657, 0L);
            break;
        case 10:
            rParam0 = CComMan::GetDatF(0x65E, 0L);
            rParam1 = CComMan::GetDatF(0x65F, 0L);
            rParam2 = CComMan::GetDatF(0x660, 0L);
            break;
        case 11:
            rParam0 = CComMan::GetDatF(0x667, 0L);
            rParam1 = CComMan::GetDatF(0x668, 0L);
            rParam2 = CComMan::GetDatF(0x669, 0L);
            break;
        case 12:
            rParam0 = CComMan::GetDatF(0x670, 0L);
            rParam1 = CComMan::GetDatF(0x671, 0L);
            rParam2 = CComMan::GetDatF(0x672, 0L);
            break;
        case 13:
            rParam0 = CComMan::GetDatF(0x679, 0L);
            rParam1 = CComMan::GetDatF(0x67A, 0L);
            rParam2 = CComMan::GetDatF(0x67B, 0L);
            break;
        case 14:
            rParam0 = CComMan::GetDatF(0x682, 0L);
            rParam1 = CComMan::GetDatF(0x683, 0L);
            rParam2 = CComMan::GetDatF(0x684, 0L);
            break;
        case 15:
            rParam0 = CComMan::GetDatF(0x68B, 0L);
            rParam1 = CComMan::GetDatF(0x68C, 0L);
            rParam2 = CComMan::GetDatF(0x68D, 0L);
            break;
        case 16:
            rParam0 = CComMan::GetDatF(0x694, 0L);
            rParam1 = CComMan::GetDatF(0x695, 0L);
            rParam2 = CComMan::GetDatF(0x696, 0L);
            break;
        case 17:
            rParam0 = CComMan::GetDatF(0x69D, 0L);
            rParam1 = CComMan::GetDatF(0x69E, 0L);
            rParam2 = CComMan::GetDatF(0x69F, 0L);
            break;
    }
}

int CComDrvHockeyGame::AreYouShot(long nSide, long &rChr) {
    CBall *pBall = m_pBall;
    if (pBall == 0) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 0;
    }
    if (pBall->IsShootMode()) {
        long nShooter = pBall->GetShotChrIdx();
        long nShooterSide = FriendOrEnemy(nShooter);
        if ((nSide == 0 && nShooterSide == 0) || (nSide == 1 && nShooterSide == 1)) {
            rChr = nShooter;
            return 1;
        }
        rChr = -1;
        return 0;
    }
    return 0;
}

int CComDrvHockeyGame::AreYouPass(long nSide, long &rChr) {
    CBall *pBall = m_pBall;
    if (pBall == 0) {
        return 0;
    }
    if (CGame::g_GameWork.m_nFlag18 & 1) {
        return 0;
    }
    if (pBall->IsPassMode()) {
        if (nSide == 0 && pBall->GetShotTeamIdx() == GetMyTeamNo()) {
            rChr = pBall->GetHockeyPassTgtChrIdx();
            return 1;
        }
        if (nSide == 1 && pBall->GetShotTeamIdx() != GetMyTeamNo()) {
            rChr = pBall->GetHockeyPassTgtChrIdx();
            return 1;
        }
    }
    return 0;
}

}
