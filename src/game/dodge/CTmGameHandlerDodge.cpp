// msw::CTmGameHandlerDodge: the tournament missions of the dodge game.
// msw::TournamentDodge::CreateInstance, which creates the handler, is in
// TournamentDodge.cpp.
#include "game/msw/TournamentDodge.hpp"

#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpitDodge.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/TournamentWork.hpp"

typedef msw::TournamentWork::TOURNAMENT_INFO::MISSION MISSION;

// (guessed) The original tests !IsDisable() as a computed value (cntlzw,
// srwi.), which is what an inline function returning it gives; a plain
// `!pChr->IsDisable()` in the condition compiles to cmpwi/bne. Name and
// place of the real helper are not known.
static inline int IsChrEnable(const msw::CChrCore *pChr)
{
    return !pChr->IsDisable();
}

void msw::CTmGameHandlerDodge::_InitRule()
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    for (int i = 0; i < 16; i++) {
        switch (info.aMission[i].nType) {
            case 0x39:
                _SetupPanel();
                break;
            case 0x38:
                _SetupItemParams();
                break;
            case 0x29:
                CGame::SendMessage(0x27, 2, 0, 0);
                break;
            case 0x31:
                CGame::SendMessage(0x27, 3, 0, 0);
                break;
            case 0x30:
                CGame::SendMessage(0x27, 8, 0, 0);
                break;
            case 0x2B:
                CGame::SendMessage(0x27, 5, 1, 0);
                break;
            case 0x36:
                CGame::SendMessage(0x27, 0xB, 1, 0);
                break;
            case 0x33:
                CGame::SendMessage(0x27, 0xA, 0, 0);
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

int msw::CTmGameHandlerDodge::_IsFaild(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];
    switch (mission.nType) {
        case 0x19:
            return m_nLostScore[0] + m_nLostScoreSum[0] > 0;
        case 0x18:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            return 0;
    }
    return 0;
}

int msw::CTmGameHandlerDodge::_IsClear(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];
    switch (mission.nType) {
        case 0x16:
        case 0x19:
            if (mission.nCount > 0) {
                return 1;
            }
            return 0;
        case 0x18:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x20:
        case 0x21:
            if (mission.nCount >= mission.nTarget) {
                return 1;
            }
            return 0;
    }
    return 1;
}

// The calls of TournamentWork::GetTournamentInfo below are in the original
// with their results unused.

int msw::CTmGameHandlerDodge::_CheckClear()
{
    TournamentWork::GetTournamentInfo();
    long nResult = CGame::GetTeamResult(0);
    _SetBattleWin(nResult);
    if (nResult == 1) {
        if (m_nLostScoreSum[0] == 0) {
            _SetCount(0x19, 1, 0, 24.0f);
        }
        _CountUp(0x16, 1, 1, 24.0f);
    }
    return _IsBattleWin();
}

int msw::CTmGameHandlerDodge::_CheckWin()
{
    TournamentWork::GetTournamentInfo();
    long nResult = CGame::GetTeamResult(0);
    _SetBattleWin(nResult);
    if (nResult == 1) {
        _CountUp(0x16, 1, 1, 24.0f);
    }
    return _IsBattleWin();
}

void msw::CTmGameHandlerDodge::_SetCountDisp(int bDisp)
{
    const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
    int bShow = 0;
    long nTarget = 0;
    long nCount = 0;
    if (bDisp != 0 && CTournament::IsExtraMatchGame()) {
        for (int i = 0; i < 16; i++) {
            const MISSION &mission = info.aMission[i];
            if (mission.nFlags & 0x40) {
                nTarget = mission.nTarget;
                bShow = 1;
                nCount = mission.nCount;
            }
        }
    }
    CCockpitDodge::SetMissionCountActiveColor(1);
    CCockpitDodge::SetMissionCountNum(0, nCount, nTarget);
    CCockpitDodge::SetMissionCountDisp(0, bShow);
    m_Unk3A = 0;
}

void msw::CTmGameHandlerDodge::_SetCountUp(long nMission)
{
    const MISSION &mission = TournamentWork::GetTournamentInfo().aMission[nMission];
    CCockpitDodge::SetMissionCountUpNum(0, mission.nUnk06, mission.nCount);
    _SeCountUpDisp();
}

int msw::CTmGameHandlerDodge::_CheckAppeal(int)
{
    // Both results are unused in the original.
    TournamentWork::GetTournamentInfo();
    CTournament::IsExtraMatchGame();
    return 0;
}

inline void msw::CTmGameHandlerDodge::_CountLostScore()
{
    for (int i = 0; i < 4; i++) {
        m_nLostScore[i] = 0;
    }
    for (long nChr = 0; nChr < CChrMan::GetChrCoreNum(); nChr++) {
        if (CGame::IsCharEx_DodgeOutfielder(nChr)) {
            continue;
        }
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr != 0 && IsChrEnable(pChr)) {
            CGameDodge *pGame = static_cast<CGameDodge *>(CGame::m_pInstance);
            long nLost = pGame->GetChrLostScore_(nChr);
            m_nLostScore[pChr->GetTeamNo()] += nLost;
        }
    }
}

inline void msw::CTmGameHandlerDodge::_AddLostScore()
{
    TournamentWork::GetTournamentInfo();
    m_nLostScoreSum[0] += m_nLostScore[0];
    m_nLostScoreSum[1] += m_nLostScore[1];
}

void msw::CTmGameHandlerDodge::_SendMessage(long nMsg, long nArg1, long nArg2, long nArg3)
{
    switch (nMsg) {
        case 0x22:
            m_Unk39 = 1;
            break;
        case 0xF:
            _GameInit(0);
            for (int i = 0; i < 4; i++) {
                m_nLostScoreSum[i] = 0;
                m_nLostScore[i] = 0;
            }
            _InitRule();
            break;
        case 0x10:
            _SetCountDisp(1);
            break;
        case 0x11:
            _CountLostScore();
            _AddLostScore();
            _CheckEndMessage(1);
            break;
        case 0x12:
            _GamePeriodInit(0);
            _CountLostScore();
            _AddLostScore();
            break;
        case 0x3F:
            if (_IsMissionTeam(nArg1)) {
                CChrCore *pChr = CChrMan::GetChrCore(nArg1);
                if (pChr != 0 && IsChrEnable(pChr) && !pChr->IsDodgeInfield()) {
                    _CountUp(0x1B, 1, 1, 24.0f);
                }
                switch (nArg2) {
                    case 0:
                        _CountUp(0x1E, 1, 1, 24.0f);
                        break;
                    case 0xB:
                    case 2:
                        _CountUp(0x1D, 1, 1, 24.0f);
                        break;
                    case 6:
                    case 7:
                    case 8:
                        _CountUp(0x1C, 1, 1, 24.0f);
                        break;
                    case 9:
                        _CountUp(0x18, 1, 1, 24.0f);
                        break;
                }
            }
            _CountLostScore();
            _CheckEndMessage(0);
            m_Unk39 = 1;
            break;
        case 0x42:
            if (nArg1 != -1 && _IsMissionTeam(nArg1)) {
                switch (nArg2) {
                    case 5:
                        _CountUp(0x21, 1, 1, 24.0f);
                        break;
                    case 4:
                        if (nArg3 != -1 && !_IsMissionTeam(nArg3)) {
                            _CountUp(0x1F, 1, 1, 24.0f);
                        }
                        break;
                    case 3:
                        break;
                    default:
                        if (nArg3 != -1 && !_IsMissionTeam(nArg3)) {
                            _CountUp(0x20, 1, 1, 24.0f);
                        }
                        break;
                }
            }
            _CountLostScore();
            _CheckEndMessage(0);
            m_Unk39 = 1;
            break;
    }
}
