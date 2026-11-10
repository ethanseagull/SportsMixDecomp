// msw::TournamentHockey and msw::CTmGameHandlerHockey: the tournament rules of
// the hockey game. One file, as in BossF (TournamentBoss.cpp): CreateInstance
// comes first and _Init, which it calls, follows it in the original.
#include "game/msw/TournamentHockey.hpp"

#include "game/msw/TournamentWork.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CCockpitHockey.hpp"

msw::CTmGameHandler *msw::TournamentHockey::CreateInstance(bcs::CTask *pParent)
{
    if (CTmGameHandler::CreateInstance(pParent) == 0) {
        CTmGameHandler::m_pInstance = new CTmGameHandlerHockey(pParent);
        CTmGameHandler::m_pInstance->_Init();
    }
    return CTmGameHandler::m_pInstance;
}

// Most functions below call TournamentWork::GetTournamentInfo and, in the
// original, some of them never use the result.

typedef msw::TournamentWork::MISSION_INFO MISSION;

void msw::CTmGameHandlerHockey::_InitRule()
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();

    m_nShotFlags = 0xF;
    const MISSION *pMission = info.aMission;
    for (long i = 0; i < 16; i++, pMission++) {
        switch (pMission->nType) {
            case 0x39:
                _SetupPanel();
                break;
            case 0x38:
                _SetupItemParams();
                break;
            case 0x2A:
                CGame::SendMessage(0x27, 1, 0, 0);
                break;
            case 0x30:
                CGame::SendMessage(0x27, 9, 0, 0);
                break;
            case 0x35:
                CGame::SendMessage(0x27, 7, 0, 0);
                break;
            case 0x36:
                CGame::SendMessage(0x27, 4, 0, 0);
                CGame::SendMessage(0x27, 4, 1, 0);
                break;
            case 0x3C:
                m_Unk34 |= 8;
                break;
            case 0x3D:
                m_Unk34 |= 0x10;
                break;
            case 0x11:
                // The mission start mode (CTournament::GetMissionStartMode).
                m_Unk34 &= ~7;
                m_Unk34 |= pMission->nTarget;
                break;
            case 0x18:
            case 0x27:
                m_nShotFlags &= ~8;
                break;
            case 0x20:
            case 0x26:
                m_nShotFlags &= ~2;
                break;
            case 0x21:
            case 0x25:
                m_nShotFlags &= ~4;
                break;
        }
    }
}

int msw::CTmGameHandlerHockey::_IsFaild(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];

    switch (mission.nType) {
        case 0x28:
            return mission.nCount < 0;
        case 0x19:
            return _GetScore(1) != 0;
        case 0x17:
        case 0x1A:
            if (_GetScore(1) >= mission.nTarget) {
                return 1;
            }
            return 0;
        case 0x3F:
            if (CTournament::GetMissionStartMode() == 2) {
                return m_Unk5C;
            }
            return 0;
        case 0x40:
            return m_Unk5C;
    }
    return 0;
}

int msw::CTmGameHandlerHockey::_IsClear(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];

    switch (mission.nType) {
        case 0x16:
        case 0x19:
        case 0x28:
        case 0x3E:
        case 0x40:
            return mission.nCount > 0;
        case 0x13:
        case 0x1A:
            return _GetScore(0) >= mission.nTarget;
        case 0x18:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x3F:
            return mission.nCount >= mission.nTarget;
    }
    return 1;
}

int msw::CTmGameHandlerHockey::_CheckClear()
{
    TournamentWork::GetTournamentInfo();
    long nWon = m_aScore[0];
    long nLost = m_aScore[1];

    if (nLost <= 0 && nWon > 0) {
        _SetCount(0x19, nWon, 0, 24.0f);
    }
    _CheckPoint(0x1C, nWon - nLost, 1, 24.0f);
    _SetCount(0x40, m_Unk5C == 0, 0, 24.0f);
    return _CheckWin();
}

int msw::CTmGameHandlerHockey::_CheckWin()
{
    TournamentWork::GetTournamentInfo();
    long nResult = CGame::GetTeamResult(0);

    _SetBattleWin(nResult);
    if (nResult == 1) {
        _CountUp(0x16, 1, 1, 24.0f);
    }
    return _IsBattleWin();
}

void msw::CTmGameHandlerHockey::_SetCountDisp(int bShow)
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    int nColor = 0;
    int bDisp = 0;
    int nCoinMissions = 0;
    long nTarget = 0;
    long nCount = 0;

    if (bShow != 0 && CTournament::IsExtraMatchGame()) {
        for (long i = 0; i < 16; i++) {
            const MISSION &mission = info.aMission[i];
            if (mission.nFlags & 0x40) {
                bDisp = 1;
                nCount = mission.nCount;
                nTarget = mission.nTarget;
                switch (mission.nType) {
                    case 0x3E:
                        nColor = _GetCoin() >= mission.nTarget;
                        nTarget = 1;
                        nCoinMissions++;
                        nCount = mission.nCount > 0;
                        break;
                    case 0x1D:
                        nTarget = 1;
                        nCount = nCount > 0;
                        break;
                }
            }
        }
    }

    if (nCoinMissions > 0) {
        CCockpitHockey::SetMissionCountActiveColor(nColor);
    } else {
        CCockpitHockey::SetMissionCountActiveColor(1);
    }
    CCockpitHockey::SetMissionCountNum(0, nCount, nTarget);
    CCockpitHockey::SetMissionCountDisp(0, bDisp);
    m_Unk3A = nColor;
}

void msw::CTmGameHandlerHockey::_SetCountUp(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];
    long nNew = mission.nPrevCount;
    long nCount = mission.nCount;

    switch (mission.nType) {
        case 0x3E:
        case 0x1D:
            nNew = mission.nCount > 0;
            nCount = 1;
            break;
    }
    CCockpitHockey::SetMissionCountUpNum(0, nNew, nCount);
    _SeCountUpDisp();
}

int msw::CTmGameHandlerHockey::_CheckAppeal(int nColor)
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    int nNewColor = 0;
    int nCoinMissions = 0;

    if (CTournament::IsExtraMatchGame()) {
        const MISSION *pMission = info.aMission;
        for (long i = 0; i < 16; i++, pMission++) {
            if ((pMission->nFlags & 0x40) && pMission->nType == 0x3E) {
                nNewColor = _GetCoin() >= pMission->nTarget;
                nCoinMissions++;
            }
        }
    }

    if (nCoinMissions > 0 && nColor != nNewColor) {
        CCockpitHockey::SetMissionCountActiveColor(nNewColor);
    }
    return nNewColor;
}

int msw::CTmGameHandlerHockey::_IsMissionShootFalseRetry(long nTeam, long)
{
    if (CTournament::GetMissionStartMode() == 1) {
        if (_IsMissionTeam(nTeam)) {
            switch (m_nLastShot) {
                case 4:
                    return _GetShotRetry(8);
                case 6:
                    return _GetShotRetry(4);
                case 2:
                    return _GetShotRetry(2);
                default:
                    return _GetShotRetry(1);
            }
        }
        return 4;
    }
    return 0;
}

int msw::CTmGameHandlerHockey::_CheckBallHold(long nTeam, long nPlayer)
{
    if (CTournament::GetMissionStartMode() == 1) {
        TournamentWork::GetTournamentInfo();
        if (nTeam == 1) {
            CGame::SendMessage(0x23, 4, 0, 0);
        }
    }
    if (m_Unk60 == 0 && _IsMissionTeam(nTeam)) {
        if (CTournament::GetMissionStartMode() == 2 && CGame::IsCharEx_HockeyKeeper(nPlayer)) {
            CGame::SendMessage(0x23, 4, 0, 0);
        } else {
            _CountUp(0x3F, 1, 1, 24.0f);
            _CheckEndMessage(0);
        }
    }
    return 0;
}

void msw::CTmGameHandlerHockey::_SendMessage(long nMsg, long nTeam, long nKind, long nFlag)
{
    switch (nMsg) {
        case 0x22:
            m_Unk39 = 1;
            break;
        case 0xF:
            _GameInit(0);
            m_nShotFlags = 0;
            m_Unk5C = 0;
            m_Unk60 = 0;
            m_aScore[0] = 0;
            m_aScore[1] = 0;
            m_aScore[2] = 0;
            m_aScore[3] = 0;
            _InitRule();
            break;
        case 0x10:
            _SetCountDisp(1);
            m_Unk60 = 0;
            m_nLastShot = -1;
            break;
        case 0x11:
            _CheckEndMessage(1);
            break;
        case 0x12:
            _GamePeriodInit(0);
            m_nLastShot = -1;
            break;
        case 0x13:
            m_nLastShot = -1;
            break;
        case 0x23:
            m_Unk60 = 1;
            break;
        case 0x62:
            // A shot.
            m_nLastShot = -1;
            if (_IsMissionTeam(nTeam)) {
                m_nLastShot = nKind;
                switch (nKind) {
                    case 0:
                        _CountUp(0x1E, 1, 1, 24.0f);
                        break;
                    case 1:
                        _CountUp(0x1F, 1, 1, 24.0f);
                        break;
                    case 3:
                    case 5:
                        _CountUp(0x22, 1, 1, 24.0f);
                        _CountUp(0x23, 1, 1, 24.0f);
                        break;
                }
            }
            _CheckEndMessage(0);
            m_Unk39 = 1;
            break;
        case 0x61: {
            // A goal; nKind is the points it gave.
            if (nFlag == 0 && _IsMissionTeam(nTeam)) {
                _CountUp(0x1B, 1, 1, 24.0f);
                _CheckPoint(0x1D, nKind, 1, 24.0f);
                _CheckPoint(0x3E, CGame::g_GameWork.m_Unk160, 1, 24.0f);
                switch (m_nLastShot) {
                    case 2:
                        _CountUp(0x20, 1, 1, 24.0f);
                        _CountUp(0x26, nKind, 1, 24.0f);
                        break;
                    case 4:
                        _CountUp(0x18, 1, 1, 24.0f);
                        _CountUp(0x27, nKind, 1, 24.0f);
                        break;
                    case 6:
                        _CountUp(0x25, 1, 1, 24.0f);
                        _CountUp(0x21, nKind, 1, 24.0f);
                        break;
                }
            } else {
                if (CTournament::IsExtraMatchGame() && _GetCount(0x1B) > 0) {
                    _SeCountDown();
                }
                _SetCount(0x1B, 0, 0, 24.0f);
            }
            if (!_IsMissionTeam(nTeam)) {
                m_Unk5C = 1;
            } else {
                m_Unk5C = 0;
            }
            for (long i = 0; i < 4; i++) {
                m_aScore[i] = CGame::GetTeamScore(i);
            }
            _CheckEndMessage(0);
            m_Unk39 = 1;
            break;
        }
    }
}
