// msw::TournamentBoss: the tournament rules of the boss game.
#include "game/msw/TournamentBoss.hpp"

#include "game/msw/TournamentWork.hpp"
#include "game/msw/pending/CGame.hpp"

msw::CTmGameHandler *msw::TournamentBoss::CreateInstance(bcs::CTask *pParent)
{
    if (CTmGameHandler::CreateInstance(pParent) == 0) {
        CTmGameHandler::m_pInstance = new CTmGameHandlerBoss(pParent);
        CTmGameHandler::m_pInstance->_Init();
    }
    return CTmGameHandler::m_pInstance;
}

// The calls of TournamentWork::GetTournamentInfo below are in the original
// with their results unused.

int msw::CTmGameHandlerBoss::_IsFaild(long)
{
    TournamentWork::GetTournamentInfo();
    return 0;
}

int msw::CTmGameHandlerBoss::_IsClear(long)
{
    TournamentWork::GetTournamentInfo();
    return 1;
}

int msw::CTmGameHandlerBoss::_CheckClear()
{
    return _CheckWin();
}

int msw::CTmGameHandlerBoss::_CheckWin()
{
    TournamentWork::GetTournamentInfo();
    _SetBattleWin(CGame::GetTeamResult(0));
    return _IsBattleWin();
}

void msw::CTmGameHandlerBoss::_SetCountDisp(int)
{
}

void msw::CTmGameHandlerBoss::_SetCountUp(long)
{
}

void msw::CTmGameHandlerBoss::_SendMessage(long nMsg, long, long, long)
{
    switch (nMsg) {
        case 0x22:
            m_Unk39 = 1;
            break;
        case 0xF:
            _GameInit(0);
            m_Unk44 = 0;
            m_Unk48 = 0;
            m_Unk4C = 0;
            m_Unk50 = 0;
            TournamentWork::GetTournamentInfo();
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
    }
}
