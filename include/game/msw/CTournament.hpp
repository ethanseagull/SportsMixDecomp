#ifndef CTOURNAMENT_HPP_
#define CTOURNAMENT_HPP_

namespace msw {

    // Only the members used so far. Both are static: their bodies in the DOL
    // work on globals and take no object.
    class CTournament {
        public:
            static void Initialize(long);
            static int IsExtraMatchGame(); // returns 0 or 1; type guessed
            static int IsMixTournamentGame(); // type guessed
            // The byte at 0x1054 of the tournament's work area.
            static long GetGameMode();
            // The cup being played (0 to 3). (description guessed)
            static long GetCup();
            // The game mode of the stage: GetGameMode, mapped when it is 5.
            static long GetStageGameMode();
            // The low three bits of the tournament handler's word at 0x34
            // (CTmGameHandler::m_Unk34, set from a mission of type 0x11 in
            // BasketF), 0 without a handler. Which mission rule the match
            // starts with; 1 and 2 are tested. (return type guessed)
            static long GetMissionStartMode();
            // The tournament's level (TOURNAMENT_INFO::nLevel).
            static long GetLevel();
            // Members per team for the tournament's play type. (return type
            // guessed)
            static long GetTeamMax();
            // The CGame::GAME_PLAY_TYPE for sport nGameMode with nPlayerMax
            // players (TOURNAMENT_INFO::nPlayerMax), for a normal game and
            // for a party game. (return types and parameter names guessed)
            static long GetPlayerType(long nGameMode, long nPlayerMax);
            static long GetPlayerTypeParty(long nGameMode, long nPlayerMax);
    };
}

#endif /* !CTOURNAMENT_HPP_ */
