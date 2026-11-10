// msw::TournamentVolley and msw::CTmGameHandlerVolley: the tournament rules of
// the volley game.
#include "game/msw/TournamentVolley.hpp"

#include "game/msw/TournamentWork.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CCockpitVolley.hpp"

msw::CTmGameHandler *msw::TournamentVolley::CreateInstance(bcs::CTask *pParent)
{
    if (CTmGameHandler::CreateInstance(pParent) == 0) {
        CTmGameHandler::m_pInstance = new CTmGameHandlerVolley(pParent);
        CTmGameHandler::m_pInstance->_Init();
    }
    return CTmGameHandler::m_pInstance;
}

// Most functions below call TournamentWork::GetTournamentInfo and, in the
// original, some of them never use the result.

typedef msw::TournamentWork::MISSION_INFO MISSION;

void msw::CTmGameHandlerVolley::_InitRule()
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();

    for (long i = 0; i < 16; i++) {
        switch (info.aMission[i].nType) {
            case 0x39:
                _SetupPanel();
                break;
            case 0x38:
                _SetupItemParams();
                break;
            case 0x32:
                CGame::SendMessage(0x27, 8, 1, 0);
                break;
            case 0x33:
                CGame::SendMessage(0x27, 5, 1, 0);
                break;
            case 0x2A:
                CGame::SendMessage(0x27, 10, 0, 0);
                break;
            case 0x34:
                CGame::SendMessage(0x27, 4, 0, 0);
                CGame::SendMessage(0x27, 4, 1, 0);
                break;
            case 0x3C:
                m_Unk34 |= 8;
                break;
            case 0x3D:
                m_Unk34 |= 0x10;
                break;
        }
    }
}

int msw::CTmGameHandlerVolley::_IsFaild(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];

    switch (mission.nType) {
        case 0x19:
            return m_aScore[1] != 0;
        case 0x1A:
        case 0x17:
            if (_GetScore(1) >= mission.nTarget) {
                return 1;
            }
            return 0;
        case 0x28:
            return mission.nCount < 0;
        case 0x1B:
            return 0;
    }
    return 0;
}

int msw::CTmGameHandlerVolley::_IsClear(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];

    switch (mission.nType) {
        case 0x16:
        case 0x19:
        case 0x28:
        case 0x3E:
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
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x3F:
            return mission.nCount >= mission.nTarget;
    }
    return 1;
}

int msw::CTmGameHandlerVolley::_CheckClear()
{
    TournamentWork::GetTournamentInfo();
    long nWon = m_aScore[0];
    long nLost = m_aScore[1];

    if (nLost <= 0 && nWon > 0) {
        _SetCount(0x19, nWon, 0, 24.0f);
    }
    _CheckPoint(0x1C, nWon - nLost, 1, 24.0f);
    return _CheckWin();
}

int msw::CTmGameHandlerVolley::_CheckWin()
{
    TournamentWork::GetTournamentInfo();
    long nResult = CGame::GetTeamResult(0);

    _SetBattleWin(nResult);
    if (nResult == 1) {
        _CountUp(0x16, 1, 1, 24.0f);
    }
    return _IsBattleWin();
}

void msw::CTmGameHandlerVolley::_SetCountDisp(int bShow)
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
                        nCoinMissions++;
                        nCount = mission.nCount;
                        // fall through
                    case 0x1D:
                        nTarget = 1;
                        nCount = nCount > 0;
                        break;
                }
            }
        }
    }

    if (nCoinMissions > 0) {
        CCockpitVolley::SetMissionCountActiveColor(nColor);
    } else {
        CCockpitVolley::SetMissionCountActiveColor(1);
    }
    CCockpitVolley::SetMissionCountNum(0, nCount, nTarget);
    CCockpitVolley::SetMissionCountDisp(0, bDisp);
    m_Unk3A = nColor;
}

void msw::CTmGameHandlerVolley::_SetCountUp(long nMission)
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
    CCockpitVolley::SetMissionCountUpNum(0, nNew, nCount);
    _SeCountUpDisp();
}

int msw::CTmGameHandlerVolley::_CheckAppeal(int nColor)
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    int nNewColor = 0;
    int nCoinMissions = 0;

    if (CTournament::IsExtraMatchGame()) {
        for (long i = 0; i < 16; i++) {
            const MISSION &mission = info.aMission[i];
            if ((mission.nFlags & 0x40) && mission.nType == 0x3E) {
                nNewColor = _GetCoin() >= mission.nTarget;
                nCoinMissions++;
            }
        }
    }

    if (nCoinMissions > 0 && nColor != nNewColor) {
        CCockpitVolley::SetMissionCountActiveColor(nNewColor);
    }
    return nNewColor;
}

void msw::CTmGameHandlerVolley::_SendMessage(long nMsg, long nTeam, long nKind, long nPoint)
{
    switch (nMsg) {
        case 0x22:
            m_Unk39 = 1;
            break;
        case 0xF:
            _GameInit(0);
            m_aScore[0] = 0;
            m_aScore[1] = 0;
            m_aScore[2] = 0;
            m_aScore[3] = 0;
            _InitRule();
            break;
        case 0x10:
            m_Unk39 = 1;
            _SetCountDisp(1);
            break;
        case 0x11:
            _CheckEndMessage(1);
            break;
        case 0x12:
            _GamePeriodInit(0);
            _SetCount(0x1B, 0, 0, 24.0f);
            if (CGame::g_GameWork.GetPeriod() != 0) {
                unsigned long nWon = _GetScore(0);
                unsigned long nLost = _GetScore(1);
                _CheckPoint(0x1C, nWon - nLost, 1, 24.0f);
            }
            break;
        case 0x54: {
            long i;
            long nPeriod = CGame::g_GameWork.m_nPeriod;
            for (i = 0; i < 4; i++) {
                m_aScore[i] = CGame::GetTeamPeriodScore(i, nPeriod);
            }
            m_nLastKind = -1;
            m_nLastPoint = -1;
            if (_IsMissionTeam(nTeam)) {
                m_nLastPoint = nPoint;
                switch (nKind) {
                    case 11:
                    case 9:
                    case 3:
                        if (nPoint > 0) {
                            m_nLastKind = nKind;
                        }
                        break;
                    case 2:
                        if (nPoint > 0) {
                            m_nLastKind = nKind;
                            _CountUp(0x1F, 1, 1, 24.0f);
                        }
                        break;
                    case 7:
                        if (nPoint > 0) {
                            m_nLastKind = nKind;
                            _CountUp(0x20, 1, 1, 24.0f);
                        }
                        break;
                }
            }
            break;
        }
        case 0x55:
            TournamentWork::GetTournamentInfo();
            if (nTeam == 0) {
                _CheckPoint(0x1D, nPoint, 1, 24.0f);
                _CheckPoint(0x3E, CGame::g_GameWork.m_Unk160, 1, 24.0f);
                if (m_nLastPoint == 2) {
                    _CountUp(0x23, 1, 1, 24.0f);
                    _CountUp(0x24, nPoint, 1, 24.0f);
                }
                switch (m_nLastKind) {
                    case 11:
                        _CountUp(0x21, nPoint, 1, 24.0f);
                        break;
                    case 3:
                        _CountUp(0x25, 1, 1, 24.0f);
                        _CountUp(0x1E, nPoint, 1, 24.0f);
                        break;
                    case 9:
                        _CountUp(0x26, 1, 1, 24.0f);
                        _CountUp(0x18, nPoint, 1, 24.0f);
                        break;
                }
                _CountUp(0x1B, 1, 1, 24.0f);
            } else {
                if (CTournament::IsExtraMatchGame() && _GetCount(0x1B) > 0) {
                    _SeCountDown();
                }
                _SetCount(0x1B, 0, 0, 24.0f);
            }
            _CheckEndMessage(0);
            m_Unk39 = 0;
            break;
        case 0x56:
            TournamentWork::GetTournamentInfo();
            if (nTeam == 0) {
                _CountUp(0x3F, 1, 20, 24.0f);
                _CheckEndMessage(0);
                m_Unk39 = 1;
            }
            break;
        case 0x57:
            if (_IsMissionTeam(nTeam)) {
                _CountUp(0x22, 1, 20, 24.0f);
                _CheckEndMessage(0);
                m_Unk39 = 1;
            }
            break;
        case 0x58:
            if (_IsMissionTeam(nTeam)) {
                _CountUp(0x27, 1, 1, 24.0f);
                _CheckEndMessage(0);
                m_Unk39 = 1;
            }
            break;
    }
}
