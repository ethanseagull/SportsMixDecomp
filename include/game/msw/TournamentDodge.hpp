#ifndef MSW_TOURNAMENTDODGE_HPP_
#define MSW_TOURNAMENTDODGE_HPP_

#include "game/msw/CTmGameHandler.hpp"

namespace msw {

    // The tournament handler of the dodge game (DodgeF), size 0x64. The
    // destructor is the compiler's.
    class CTmGameHandlerDodge : public CTmGameHandler {
        public:
            CTmGameHandlerDodge(bcs::CTask *pParent) : CTmGameHandler(pParent) {}

            virtual void _SendMessage(long nMsg, long, long, long);
            virtual int _CheckWin();
            virtual int _CheckClear();
            virtual int _CheckAppeal(int);
            virtual int _IsClear(long);
            virtual int _IsFaild(long);
            virtual void _SetCountDisp(int);
            virtual void _SetCountUp(long);

        private:
            // Prepares the match's missions (message 0xF): sends the game
            // the rule messages the mission types ask for.
            void _InitRule();
            // Recounts m_nLostScore from the characters on the court.
            void _CountLostScore();
            // Adds m_nLostScore of the two teams to m_nLostScoreSum.
            void _AddLostScore();

            // Per team (index CChrCore::GetTeamNo()). Meaning guessed from
            // the source of the numbers, CGameDodge::GetChrLostScore_:
            // m_nLostScore is recounted from the characters at each message,
            // m_nLostScoreSum adds it up at the end of each period.
            long m_nLostScoreSum[4]; // 0x44
            long m_nLostScore[4];    // 0x54
    };

    // A namespace by the same guess as msw::Tournament: the name has no "C"
    // prefix.
    namespace TournamentDodge {
        // Makes sure there is a tournament handler: the one
        // CTmGameHandler::CreateInstance returns, or else a new
        // CTmGameHandlerDodge. Returns CTmGameHandler::m_pInstance.
        CTmGameHandler *CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !MSW_TOURNAMENTDODGE_HPP_ */
