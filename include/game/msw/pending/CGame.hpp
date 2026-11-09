#ifndef PENDING_CGAME_HPP_
#define PENDING_CGAME_HPP_

// Both headers define msw::CGame; taking the real header's guard makes a
// later include of it a no-op and an earlier one an error.
#ifdef CGAME_HPP_
#error "include either game/msw/CGame.hpp or game/msw/pending/CGame.hpp, not both"
#endif
#define CGAME_HPP_

// PENDING MERGE into game/msw/CGame.hpp, which this session may not edit.
// This is msw::CGame as game/boss/TournamentBoss.cpp needs it: that header's
// members plus GetTeamResult. Never include both files. Once the addition
// is in CGame.hpp, delete this file and include that one.

#include "bcs/CTask.hpp"

namespace msw {

    // Base of the per-sport game tasks. Only what callers outside the class
    // need so far; the layout past bcs::CTask is not mapped.
    class CGame : public bcs::CTask {
        public:
            // As in CGame.hpp.
            enum GAME_MODE {
                GAME_MODE_BASKET,
                GAME_MODE_VOLLEY,
                GAME_MODE_DODGE,
                GAME_MODE_HOCKEY,
                GAME_MODE_BOSS
            };

            // As in CGame.hpp. Settings of the game being set up or played,
            // 0x638 bytes.
            struct GameWork {
                long GetGameType() const { return m_nGameType; }

                unsigned char m_Unk00[4];
                unsigned char m_nGameMode; // 0x04
                unsigned char m_nGameType; // 0x05
                unsigned char m_Unk06[0x632];
            };

            CGame(bcs::CTask *pParent, GAME_MODE mode);
            virtual ~CGame();
            virtual void Main();

            static void SetDefaultParam();

            // Addition. The result of team nTeam: the byte at
            // g_GameWork + 0xFC + nTeam, read unsigned. What the values mean
            // is not known; CTmGameHandler::_SetBattleWin takes it and
            // _IsBattleWin tests for 1. (return type guessed)
            static long GetTeamResult(long nTeam);

            static GameWork g_GameWork;
            static CGame *m_pInstance;

        private:
            unsigned char m_Unk34[0x150 - 0x34];
    };
}

#endif /* !PENDING_CGAME_HPP_ */
