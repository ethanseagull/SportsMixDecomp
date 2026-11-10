#ifndef MSW_TOURNAMENTVOLLEY_HPP_
#define MSW_TOURNAMENTVOLLEY_HPP_

#include "game/msw/CTmGameHandler.hpp"
#include "game/msw/TournamentWork.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    // The tournament handler of the volley game (VolleyF), size 0x5C. The
    // destructor is the compiler's.
    //
    // Mission types it handles (TournamentWork::TOURNAMENT_INFO::MISSION::
    // nType), from what each case does; the names of the missions are not
    // known. 0x16 is counted when the match is won, 0x19 fails as soon as
    // the other team scores, 0x1B counts points in a row, 0x1C is the score
    // difference, 0x1D and 0x3E (coins) are shown as done / not done.
    class CTmGameHandlerVolley : public CTmGameHandler {
        public:
            CTmGameHandlerVolley(bcs::CTask *pParent) : CTmGameHandler(pParent) {}

            virtual void _SendMessage(long nMsg, long, long, long);
            virtual int _CheckWin();
            virtual int _CheckClear();
            virtual int _CheckAppeal(int nColor);
            virtual int _IsClear(long nMission);
            virtual int _IsFaild(long nMission);
            virtual void _SetCountDisp(int bShow);
            virtual void _SetCountUp(long nMission);

            // Applies the missions that change the game's rules (message 0xF).
            void _InitRule();

        private:
            // The original calls TournamentWork::GetTournamentInfo in front
            // of these reads and drops the result; written as inline
            // helpers. The value is compared unsigned.
            unsigned long _GetScore(long nTeam)
            {
                TournamentWork::GetTournamentInfo();
                return m_aScore[nTeam];
            }
            unsigned long _GetCoin()
            {
                TournamentWork::GetTournamentInfo();
                return CGame::GetTeamCoin(0);
            }

            // Each team's score in the running period (message 0x54 reads
            // them with CGame::GetTeamPeriodScore); 0 is the player's team.
            long m_aScore[4]; // 0x44
            // The kind of the last point and the points it gave (the second
            // and third arguments of message 0x54), -1 when it was not the
            // mission team's.
            long m_nLastKind;  // 0x54
            long m_nLastPoint; // 0x58
    };

    // A namespace by the same guess as msw::Tournament: the name has no "C"
    // prefix. Defined in game/volley/TournamentVolley.cpp.
    namespace TournamentVolley {
        // Makes sure there is a tournament handler: the one
        // CTmGameHandler::CreateInstance returns, or else a new
        // CTmGameHandlerVolley. Returns CTmGameHandler::m_pInstance.
        CTmGameHandler *CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !MSW_TOURNAMENTVOLLEY_HPP_ */
