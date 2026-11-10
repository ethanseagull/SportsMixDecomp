// msw::TournamentBasket and msw::CTmGameHandlerBasket: the tournament rules
// of the basket game. One file, as TournamentBoss.cpp is in BossF: the
// original has CTmGameHandler::_Init (emitted here) right after CreateInstance.
#include "game/msw/TournamentBasket.hpp"

#include "game/msw/TournamentWork.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CCockpitBasket.hpp"

msw::CTmGameHandler *msw::TournamentBasket::CreateInstance(bcs::CTask *pParent)
{
    if (CTmGameHandler::CreateInstance(pParent) == 0) {
        CTmGameHandler::m_pInstance = new CTmGameHandlerBasket(pParent);
        CTmGameHandler::m_pInstance->_Init();
    }
    return CTmGameHandler::m_pInstance;
}

// Several functions call TournamentWork::GetTournamentInfo a second time
// without using the result; the original does the same.

void msw::CTmGameHandlerBasket::_InitRule()
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();

    m_nRetryShots = 0xFF;
    m_Unk5C = 0;
    for (int i = 0; i < 16; i++) {
        const TournamentWork::MISSION_INFO &mission = info.aMission[i];
        switch (mission.nType) {
            case 0x39:
                _SetupPanel();
                break;
            case 0x38:
                _SetupItemParams();
                break;
            case 0x2D:
                CGame::SendMessage(0x27, 1, 0, 0);
                break;
            case 0x2E:
                CGame::SendMessage(0x27, 6, 0, 0);
                break;
            case 0x2F:
                CGame::SendMessage(0x27, 3, 1, 0);
                break;
            case 0x32:
                CGame::SendMessage(0x27, 7, 0, 0);
                break;
            case 0x33:
                CGame::SendMessage(0x27, 4, 0, 0);
                CGame::SendMessage(0x27, 4, 1, 0);
                break;
            case 0x3C:
                m_Unk34 |= 0xF8;
                break;
            case 0x3D:
                m_Unk34 |= 0x10;
                break;
            case 0x3E:
                m_Unk34 |= 0x20;
                break;
            case 0x3F:
                m_Unk34 |= 0x40;
                break;
            case 0x40:
                m_Unk34 |= 0x80;
                break;
            case 0x4A:
                m_Unk34 |= 0x100;
                break;
            case 0x4B:
                m_Unk34 |= 0x200;
                break;
            case 0x4C:
                m_Unk34 |= 0x400;
                break;
            case 0x4D:
                m_Unk34 |= 0x800;
                break;
            case 0x11:
                // The mission start mode (CTournament::GetMissionStartMode).
                m_Unk34 &= ~7;
                m_Unk34 |= mission.nTarget;
                break;
            case 0x1E:
            case 0x1F:
            case 0x47:
            case 0x48:
            case 0x49:
                m_nRetryShots = 0;
                m_Unk5C = 1;
                break;
            case 0x18:
            case 0x41:
                m_nRetryShots &= ~0xC0;
                break;
            case 0x20:
            case 0x42:
                m_nRetryShots &= ~0x20;
                break;
            case 0x21:
            case 0x43:
                m_nRetryShots &= ~0x04;
                break;
            case 0x22:
            case 0x44:
                m_nRetryShots &= ~0x08;
                break;
            case 0x25:
            case 0x45:
                m_nRetryShots &= ~0x30;
                break;
            case 0x26:
            case 0x46:
                m_nRetryShots &= ~0x0A;
                break;
        }
    }
}

int msw::CTmGameHandlerBasket::_IsFaild(long nMission)
{
    const TournamentWork::MISSION_INFO &mission = TournamentWork::GetTournamentInfo().aMission[nMission];

    switch (mission.nType) {
        case 0x19:
            TournamentWork::GetTournamentInfo();
            return m_aTeamScore[1] != 0;
        case 0x28:
            return mission.nCount < 0;
        case 0x17:
        case 0x1A:
            TournamentWork::GetTournamentInfo();
            return m_aTeamScore[1] >= mission.nTarget;
        case 0x1E:
            return (m_nShotsMade & ~0xCA) != 0;
        case 0x1F:
            return (m_nShotsMade & ~0xF0) != 0;
        case 0x47:
            return (m_nShotsMade & ~0xE0) != 0;
        case 0x48:
            return (m_nShotsMade & ~0xC4) != 0;
        case 0x49:
            return (m_nShotsMade & ~0xC8) != 0;
        case 0x4F:
            if (CTournament::GetMissionStartMode() == 2) {
                return m_nConceded;
            }
            return 0;
        case 0x50:
            return m_nConceded;
    }
    return 0;
}

int msw::CTmGameHandlerBasket::_IsClear(long nMission)
{
    const TournamentWork::MISSION_INFO &mission = TournamentWork::GetTournamentInfo().aMission[nMission];

    switch (mission.nType) {
        case 0x16:
        case 0x19:
        case 0x1E:
        case 0x1F:
        case 0x28:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4E:
        case 0x50:
            return mission.nCount > 0;
        case 0x13:
        case 0x1A:
            TournamentWork::GetTournamentInfo();
            return m_aTeamScore[0] >= mission.nTarget;
        case 0x18:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x4F:
            return mission.nCount >= mission.nTarget;
    }
    return 1;
}

int msw::CTmGameHandlerBasket::_CheckClear()
{
    TournamentWork::GetTournamentInfo();

    long nWon = m_aTeamScore[0];
    long nLost = m_aTeamScore[1];
    if (nLost <= 0 && nWon > 0) {
        _SetCount(0x19, nWon, 0, 24.0f);
    }
    _CheckPoint(0x1C, nWon - nLost, 1, 24.0f);
    _SetCount(0x50, m_nConceded == 0, 0, 24.0f);
    _SetCount(0x47, (m_nShotsMade & ~0xE0) == 0, 0, 24.0f);
    _SetCount(0x1F, (m_nShotsMade & ~0xF0) == 0, 0, 24.0f);
    _SetCount(0x1E, (m_nShotsMade & ~0xCA) == 0, 0, 24.0f);
    // Both with the mask _IsFaild uses for 0x49 (0x48 has ~0xC4 there).
    _SetCount(0x48, (m_nShotsMade & ~0xC8) == 0, 0, 24.0f);
    _SetCount(0x49, (m_nShotsMade & ~0xC8) == 0, 0, 24.0f);
    return _CheckWin();
}

int msw::CTmGameHandlerBasket::_CheckWin()
{
    TournamentWork::GetTournamentInfo();

    long nResult = CGame::GetTeamResult(0);
    _SetBattleWin(nResult);
    if (nResult == 1) {
        _CountUp(0x16, 1, 1, 24.0f);
    }
    return _IsBattleWin();
}

void msw::CTmGameHandlerBasket::_SetCountDisp(int bDisp)
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    int bActive = 0;
    int bShow = 0;
    long nCoinMissions = 0;
    long nMax = 0;
    long nNum = 0;

    if (bDisp != 0 && CTournament::IsExtraMatchGame()) {
        for (int i = 0; i < 16; i++) {
            const TournamentWork::MISSION_INFO &mission = info.aMission[i];
            if (mission.nFlags & 0x40) {
                bShow = 1;
                nNum = mission.nCount;
                nMax = mission.nTarget;
                switch (mission.nType) {
                    case 0x4E: {
                        // The coins are compared unsigned (as in
                        // _CheckAppeal).
                        TournamentWork::GetTournamentInfo();
                        unsigned long nCoin = CGame::GetTeamCoin(0);
                        nMax = 1;
                        nCoinMissions++;
                        bActive = nCoin >= mission.nTarget;
                        nNum = mission.nCount > 0;
                        break;
                    }
                    case 0x1D:
                        nMax = 1;
                        nNum = nNum > 0;
                        break;
                }
            }
        }
    }

    if (nCoinMissions > 0) {
        CCockpitBasket::SetMissionCountActiveColor(bActive);
    } else {
        CCockpitBasket::SetMissionCountActiveColor(1);
    }
    CCockpitBasket::SetMissionCountNum(0, nNum, nMax);
    CCockpitBasket::SetMissionCountDisp(0, bShow);
    m_Unk3A = bActive;
}

void msw::CTmGameHandlerBasket::_SetCountUp(long nMission)
{
    const TournamentWork::MISSION_INFO &mission = TournamentWork::GetTournamentInfo().aMission[nMission];
    long nNum = mission.nPrevCount;
    long nMax = mission.nCount;

    switch (mission.nType) {
        case 0x4E:
        case 0x1D:
            nMax = 1;
            nNum = mission.nCount > 0;
            break;
    }
    CCockpitBasket::SetMissionCountUpNum(0, nNum, nMax);
    _SeCountUpDisp();
}

int msw::CTmGameHandlerBasket::_CheckAppeal(int bPrevActive)
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    int bActive = 0;
    long nCoinMissions = 0;

    if (CTournament::IsExtraMatchGame()) {
        for (int i = 0; i < 16; i++) {
            const TournamentWork::MISSION_INFO &mission = info.aMission[i];
            if ((mission.nFlags & 0x40) && mission.nType == 0x4E) {
                TournamentWork::GetTournamentInfo();
                unsigned long nCoin = CGame::GetTeamCoin(0);
                nCoinMissions++;
                bActive = nCoin >= mission.nTarget;
            }
        }
    }

    if (nCoinMissions > 0 && bPrevActive != bActive) {
        CCockpitBasket::SetMissionCountActiveColor(bActive);
    }
    return bActive;
}

// 4 when a shot by nPlayer does not count for the mission (the shot type is
// one of m_nRetryShots, or the player is not on the mission's team), else 0.
int msw::CTmGameHandlerBasket::_IsMissionShootFalseRetry(long nPlayer, long nShotType)
{
    int nResult;

    if (CTournament::GetMissionStartMode() == 1) {
        if (_IsMissionTeam(nPlayer)) {
            switch (nShotType) {
                case 8:
                    nResult = 0;
                    if (m_nRetryShots & 0x20) {
                        nResult = 4;
                    }
                    break;
                case 6:
                    nResult = 0;
                    if (m_nRetryShots & 0x04) {
                        nResult = 4;
                    }
                    break;
                case 1:
                    nResult = 0;
                    if (m_nRetryShots & 0x08) {
                        nResult = 4;
                    }
                    break;
                case 3:
                case 5:
                    nResult = 0;
                    if (m_nRetryShots & 0x10) {
                        nResult = 4;
                    }
                    break;
                case 9:
                    nResult = 0;
                    if (m_nRetryShots & 0x40) {
                        nResult = 4;
                    }
                    break;
                case 10:
                case 11:
                    nResult = 0;
                    if (m_nRetryShots & 0x80) {
                        nResult = 4;
                    }
                    break;
                case 0:
                    nResult = 0;
                    if (m_nRetryShots & 0x02) {
                        nResult = 4;
                    }
                    break;
                default:
                    nResult = 0;
                    if (m_nRetryShots & 0x01) {
                        nResult = 4;
                    }
                    break;
            }
        } else {
            nResult = 4;
        }
    } else {
        nResult = 0;
    }
    return nResult;
}

int msw::CTmGameHandlerBasket::_CheckShotClockTimeOver()
{
    if (CTournament::GetMissionStartMode() == 2) {
        _CountUp(0x4F, 1, 1, 24.0f);
    }
    return 0;
}

int msw::CTmGameHandlerBasket::_CheckBallHold(long nPlayer, long)
{
    if (CTournament::GetMissionStartMode() == 1) {
        TournamentWork::GetTournamentInfo();
        if (nPlayer == 1) {
            CGame::SendMessage(0x23, 4, 0, 0);
        }
    }
    if (_IsMissionTeam(nPlayer)) {
        _CountUp(0x4F, 1, 1, 24.0f);
        _CheckEndMessage(0);
        m_Unk39 = 1;
    }
    return 0;
}

// A guessed inline helper: written in place, the loop's pointer and counter
// get each other's registers.
inline void msw::CTmGameHandlerBasket::_UpdateTeamScore()
{
    for (int i = 0; i < 4; i++) {
        m_aTeamScore[i] = CGame::GetTeamScore(i);
    }
}

void msw::CTmGameHandlerBasket::_SendMessage(long nMsg, long nArg1, long nArg2, long nArg3)
{
    switch (nMsg) {
        case 0x22:
            m_Unk39 = 1;
            break;
        case 0xF:
            _GameInit(0);
            m_nShotsMade = 0;
            m_nRetryShots = 0;
            m_Unk5C = 0;
            m_nConceded = 0;
            m_aTeamScore[0] = 0;
            m_aTeamScore[1] = 0;
            m_aTeamScore[2] = 0;
            m_aTeamScore[3] = 0;
            _InitRule();
            _SetCountDisp(1);
            break;
        case 0x10:
            m_Unk39 = 1;
            break;
        case 0x11:
            _CheckEndMessage(1);
            break;
        case 0x12:
            _GamePeriodInit(0);
            break;
        case 0x33:
            if (_IsMissionTeam(nArg1)) {
                _CountUp(0x23, 1, 1, 24.0f);
            }
            _CheckEndMessage(0);
            m_Unk39 = 1;
            break;
        case 0x2F:
            if (nArg1 >= 0 && _IsMissionTeam(nArg1) && nArg2 >= 0 && !_IsMissionTeam(nArg2)) {
                _CountUp(0x24, 1, 1, 24.0f);
                _CheckEndMessage(0);
                m_Unk39 = 1;
            }
            break;
        case 0x36: {
            // A basket: nArg1 is the shooter, nArg2 the shot type, nArg3
            // the points.
            if (_IsMissionTeam(nArg1)) {
                _CountUp(0x1B, 1, 1, 24.0f);
                _CheckPoint(0x4E, CGame::g_GameWork.m_Unk160, 1, 24.0f);
                switch (nArg2) {
                    case 9:
                        _CountUp(0x18, 1, 1, 24.0f);
                        _CountUp(0x41, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x40;
                        break;
                    case 10:
                    case 11:
                        _CountUp(0x18, 1, 1, 24.0f);
                        _CountUp(0x41, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x80;
                        break;
                    case 8:
                        _CountUp(0x25, 1, 1, 24.0f);
                        _CountUp(0x45, nArg3, 1, 24.0f);
                        _CountUp(0x20, 1, 1, 24.0f);
                        _CountUp(0x42, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x20;
                        break;
                    case 6:
                        _CountUp(0x21, 1, 1, 24.0f);
                        _CountUp(0x43, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x04;
                        break;
                    case 1:
                        _CountUp(0x22, 1, 1, 24.0f);
                        _CountUp(0x44, nArg3, 1, 24.0f);
                        _CountUp(0x26, 1, 1, 24.0f);
                        _CountUp(0x46, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x08;
                        break;
                    case 0:
                        _CountUp(0x26, 1, 1, 24.0f);
                        _CountUp(0x46, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x02;
                        break;
                    case 3:
                    case 5:
                        _CountUp(0x25, 1, 1, 24.0f);
                        _CountUp(0x45, nArg3, 1, 24.0f);
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x10;
                        break;
                    default:
                        _CheckPoint(0x1D, nArg3, 1, 24.0f);
                        m_nShotsMade |= 0x01;
                        break;
                }
            } else {
                m_nConceded = 1;
                if (CTournament::IsExtraMatchGame() && _GetCount(0x1B) > 0) {
                    _SeCountDown();
                }
                _SetCount(0x1B, 0, 0, 24.0f);
                _SeCountDown();
            }
            _UpdateTeamScore();
            _CheckEndMessage(0);
            m_Unk39 = 0;
            break;
        }
    }
}
