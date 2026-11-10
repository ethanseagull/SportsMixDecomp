#ifndef MSW_TOURNAMENTBASKET_HPP_
#define MSW_TOURNAMENTBASKET_HPP_

#include "game/msw/CTmGameHandler.hpp"

namespace msw {

    // The tournament handler of the basket game (BasketF), size 0x60. The
    // constructor is inline (TournamentBasket::CreateInstance only calls the
    // base one and stores the vtable); the destructor is the compiler's.
    //
    // Mission types (TournamentWork::MISSION_INFO::nType) and shot types
    // (the third argument of message 0x36) are written as numbers: their
    // names are not known.
    class CTmGameHandlerBasket : public CTmGameHandler {
        public:
            CTmGameHandlerBasket(bcs::CTask *pParent) : CTmGameHandler(pParent) {}

            virtual void _SendMessage(long nMsg, long, long, long);
            virtual int _CheckWin();
            virtual int _CheckClear();
            virtual int _CheckAppeal(int bPrevActive);
            virtual int _CheckBallHold(long nPlayer, long);
            virtual int _CheckShotClockTimeOver();
            virtual int _IsClear(long nMission);
            virtual int _IsFaild(long nMission);
            virtual int _IsMissionShootFalseRetry(long nPlayer, long nShotType);
            virtual void _SetCountDisp(int bDisp);
            virtual void _SetCountUp(long nMission);

            // Reads the rules from the missions of the match; called when
            // the game starts (message 0xF).
            void _InitRule();

        private:
            // Copies CGame::GetTeamScore of the four teams to m_aTeamScore.
            // Inline, defined in the .cpp.
            void _UpdateTeamScore();

            long m_aTeamScore[4];     // 0x44, CGame::GetTeamScore after each basket
            // One bit per group of shot types: 0x01 types 2, 4, 7 and any
            // other, 0x02 type 0, 0x04 type 6, 0x08 type 1, 0x10 types 3
            // and 5, 0x20 type 8, 0x40 type 9, 0x80 types 10 and 11.
            unsigned long m_nRetryShots; // 0x54, shots that do not count for the mission team
            unsigned long m_nShotsMade;  // 0x58, shots the mission team has scored with
            short m_Unk5C;            // 0x5C, set by the shot type missions; not read here
            short m_nConceded;        // 0x5E, set once the other team scores
    };

    // A namespace by the same guess as msw::Tournament: the name has no "C"
    // prefix.
    namespace TournamentBasket {
        // Makes sure there is a tournament handler: the one
        // CTmGameHandler::CreateInstance returns, or else a new
        // CTmGameHandlerBasket. Returns CTmGameHandler::m_pInstance.
        CTmGameHandler *CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !MSW_TOURNAMENTBASKET_HPP_ */
