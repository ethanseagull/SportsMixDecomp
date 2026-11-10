#ifndef MSW_TOURNAMENTHOCKEY_HPP_
#define MSW_TOURNAMENTHOCKEY_HPP_

#include "game/msw/CTmGameHandler.hpp"
#include "game/msw/TournamentWork.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    // The tournament handler of the hockey game (HockeyF), size 0x64. The
    // destructor is the compiler's.
    //
    // Mission types it handles (TournamentWork::TOURNAMENT_INFO::MISSION::
    // nType), from what each case does; the names of the missions are not
    // known. 0x16 is counted when the match is won, 0x19 fails as soon as
    // the other team scores, 0x1B counts goals in a row, 0x1C is the score
    // difference, 0x1D and 0x3E (coins) are shown as done / not done, 0x3F
    // counts ball holds.
    class CTmGameHandlerHockey : public CTmGameHandler {
        public:
            CTmGameHandlerHockey(bcs::CTask *pParent) : CTmGameHandler(pParent) {}

            virtual void _SendMessage(long nMsg, long, long, long);
            virtual int _CheckWin();
            virtual int _CheckClear();
            virtual int _CheckAppeal(int nColor);
            virtual int _CheckBallHold(long nTeam, long nPlayer);
            virtual int _IsClear(long nMission);
            virtual int _IsFaild(long nMission);
            virtual int _IsMissionShootFalseRetry(long nTeam, long);
            virtual void _SetCountDisp(int bShow);
            virtual void _SetCountUp(long nMission);

            // Applies the missions that change the game's rules (message 0xF).
            void _InitRule();

        private:
            // The original calls TournamentWork::GetTournamentInfo in front
            // of these reads and drops the result; written as inline
            // helpers.
            long _GetScore(long nTeam)
            {
                TournamentWork::GetTournamentInfo();
                return m_aScore[nTeam];
            }
            // 4 when the kind of shot nBit stands for may be retried, else
            // 0. Guess: the original sets 0 and then 4 in each case of
            // _IsMissionShootFalseRetry; one inline helper reproduces that.
            int _GetShotRetry(unsigned long nBit) const
            {
                int nRetry = 0;
                if (m_nShotFlags & nBit) {
                    nRetry = 4;
                }
                return nRetry;
            }
            // Compared unsigned.
            unsigned long _GetCoin()
            {
                TournamentWork::GetTournamentInfo();
                return CGame::GetTeamCoin(0);
            }

            // Each team's score (message 0x61 reads them with
            // CGame::GetTeamScore); 0 is the player's team.
            long m_aScore[4]; // 0x44
            // One bit per kind of shot (1, 2, 4, 8) that may be retried when
            // the mission start mode is 1; 0xF to begin with, bits cleared by
            // the rule missions 0x18/0x27, 0x20/0x26, 0x21/0x25.
            unsigned long m_nShotFlags; // 0x54
            // The kind of the last shot (third argument of message 0x62), -1
            // when there is none or it was not the mission team's.
            long m_nLastShot; // 0x58
            // Set when the last goal (message 0x61) was not the mission
            // team's.
            long m_Unk5C; // 0x5C
            // Set by message 0x23, cleared by 0xF and 0x10; ball holds are
            // not counted while it is set.
            long m_Unk60; // 0x60
    };

    // A namespace by the same guess as msw::Tournament: the name has no "C"
    // prefix. Defined in game/hockey/TournamentHockey.cpp.
    namespace TournamentHockey {
        // Makes sure there is a tournament handler: the one
        // CTmGameHandler::CreateInstance returns, or else a new
        // CTmGameHandlerHockey. Returns CTmGameHandler::m_pInstance.
        CTmGameHandler *CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !MSW_TOURNAMENTHOCKEY_HPP_ */
