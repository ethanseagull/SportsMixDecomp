#ifndef CTOURNAMENT_HPP_
#define CTOURNAMENT_HPP_

#include "revolution/rfl.h"

namespace msw {

    // Only the members used so far. Both are static: their bodies in the DOL
    // work on globals and take no object.
    class CTournament {
        public:
            static void Initialize(long);
            // (added for the DOL's CGame::SendMessage_) Passes a game message on.
            static void SendMessage(long nMsg, long, long, long);
            // (added for the DOL's CGame::CtorStateGameResult; return type
            // guessed) 1 when team 0 won the tournament match, 2 when team 1
            // did, anything else for neither.
            static long GetBattleResult();
            // (added for BasketF game/basket/CGameBasketHungryBossPackun.cpp;
            // return type guessed) Not zero while a tournament game runs.
            static int IsTournamentGame();
            static int IsExtraMatchGame(); // returns 0 or 1; type guessed
            // (added for BossF game/boss/GameBoss.cpp; return types and
            // meanings guessed) Sets the tournament's demo type (the boss
            // game passes 0x5A); whether the mission has been cleared.
            static void SetDemoType(long nType);
            static int IsMissionClear();
            static int IsMixTournamentGame(); // type guessed
            // The byte at 0x1054 of the tournament's work area.
            static long GetGameMode();
            // (added for TourF CTmScript; description guessed) The sport of
            // the tournament, a fixed one for a mixed tournament.
            static long GetGameModeNoMix();
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
            // Added for MenuF game/menu/CHowTo.cpp. Starts tutorial
            // nTutorial (0 or 1) of game nGameKind (CSelectMenu::m_nGameKind)
            // with pad style nPadStyle. Argument names guessed.
            static void Tutorial(long nGameKind, int nPadStyle, int nTutorial);
            // Added for MenuF game/menu/CScoreTop.cpp. Whether cup nCup has
            // been won; the third argument passes through to the save data
            // and nPlayer goes to CSave::GetPlayerData. Names and return
            // type guessed.
            static int IsCupClear(long nUnk, long nCup, long nUnk2, long nPlayer);
            // Added for game/msw/CGameSnd.cpp (names and return types
            // guessed). fn_8017AC10: with GameWork byte 7 at 1 to 3, whether
            // the tournament's stage number (the short at 0x1060 of its
            // work) is none of 50, 60 and 70; otherwise whether it is 1000
            // or more, in one game mode only.
            static int IsSndStage();
            // fn_8017AE5C: the stage number, with 0x51 read as 0x50 and
            // 0x5B, 0x64 and 0x65 as 0x5A.
            static long GetSndStage();
            // Added for MenuF game/menu/CCourtChr.cpp (names and return type
            // guessed). Whether the top-menu character nChr is unlocked;
            // CCourtChr passes the ball index and -1.
            static int IsUnlockTopMenuChr(long nChr, long nUnk);
            // Added for BasketF game/basket/unit_7E8FC.cpp (return types
            // guessed). Whether the mission forbids the computer of team
            // nTeam the named kind of shot.
            static int IsMissionComThrowingShootDisable(long nTeam);
            static int IsMissionComDunkShootDisable(long nTeam);
            static int IsMissionComLayupShootDisable(long nTeam);
            // (added for game/basket/unit_B09B4.cpp) ... and the special shot.
            static int IsMissionComSpecialShootDisable(long nTeam);
            // Added for MenuF game/menu/CTournamentLoad.cpp (names of the
            // arguments and return types guessed; the callers pass
            // CSelectMenu::m_nGameKind and -1). The tournament saved for
            // later in game nGameKind: whether there is one, whether its
            // players have no Mii data, its number of players, its cup
            // (1 to 3) and how far it had got (0, 1 or 3); Continue
            // resumes it.
            static int IsContinue(long nGameKind, long nUnk);
            static int IsContinueNotMiiData(long nGameKind, long nUnk);
            static long GetContinuePlayer(long nGameKind, long nUnk);
            static long GetContinueCup(long nGameKind, long nUnk);
            static long GetContinueRank(long nGameKind, long nUnk);
            static void Continue(long nGameKind, long nUnk);
            // Added for MenuF game/menu/CSelectMenu.cpp (static, from the
            // symbols; argument names and return types guessed). Setting a
            // tournament up from the select menu: its sport, level, cup,
            // starting state, players (character, colour), their names and
            // Miis, the play type, and Setup to finish (the menu passes
            // -1). GetPlayerType gives player nPlayer's character.
            static void SetGameMode(unsigned char nGameMode);
            static void SetLevel(char nLevel);
            static void SetCup(unsigned char nCup);
            static void SetState(char nState);
            static void SetPlayer(long nPlayer, long nChara, long nColor, int nUnk);
            static void SetPlayerName(long nPlayer, const wchar_t *pName, long nLen);
            static void SetPlayerMiiStoreData(long nPlayer, const RFLStoreData &data);
            static void SetPlayType(long nPlayType);            // (added for TourF game/tour/CTournamentStep.cpp; static,
            // return types and parameter names guessed)
            static void Save(long nGameMode, int b, long n);
            static void SetExPauseFlag(int bPause);
            static void NextCup(long nCup, long nUnk);
            static void SetCupClear(long nUnk, long nCup, long nUnk2, long nPlayer);
            static void ExitMode();
            static void SetGameMode();

            static void Setup(long nUnk);
            static long GetPlayerType(long nPlayer);
    };
}

#endif /* !CTOURNAMENT_HPP_ */
