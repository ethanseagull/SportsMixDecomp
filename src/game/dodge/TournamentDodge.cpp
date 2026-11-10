// msw::TournamentDodge: the tournament rules of the dodge game.
// Written from src/game/boss/TournamentBoss.cpp (BossF). The handler's own
// functions are in CTmGameHandlerDodge.cpp.
#include "game/msw/TournamentDodge.hpp"

msw::CTmGameHandler *msw::TournamentDodge::CreateInstance(bcs::CTask *pParent)
{
    if (CTmGameHandler::CreateInstance(pParent) == 0) {
        CTmGameHandler::m_pInstance = new CTmGameHandlerDodge(pParent);
        CTmGameHandler::m_pInstance->_Init();
    }
    return CTmGameHandler::m_pInstance;
}
