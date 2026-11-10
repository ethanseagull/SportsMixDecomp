#ifndef CGAME_HPP_
#define CGAME_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // Base of the per-sport game tasks. Only what callers outside the class
    // need so far; the layout past bcs::CTask is not mapped.
    class CGame : public bcs::CTask {
        public:
            enum GAME_MODE {
                GAME_MODE_BASKET, // enumerator names guessed; the values are
                GAME_MODE_VOLLEY, // what each sport's constructor passes
                GAME_MODE_DODGE,
                GAME_MODE_HOCKEY,
                GAME_MODE_BOSS
            };

            // Settings of the game being set up or played, 0x638 bytes.
            // (type and member names guessed)
            struct GameWork {
                // Callers compare the byte as a signed int, hence a getter
                // that widens it.
                long GetGameType() const { return m_nGameType; }

                unsigned char m_Unk00[4];
                unsigned char m_nGameMode; // 0x04, the GAME_MODE of the running game
                unsigned char m_nGameType; // 0x05, 2 selects the sport's party game
                unsigned char m_Unk06[0xA];
                long m_nGameLevel;         // 0x10, 0-3 (EASY, NORMAL, HARD, EXPERT; names guessed)
                unsigned char m_Unk14[0x2C];
                // 0x40, flag words whose bit 0 the boss game's "?" panels test
                // (game/boss/CQPanelManBoss.cpp): with bit 0 of 0x40 set a coin
                // panel gives no coin; with bit 0 of 0x1C8 set the panels pop up
                // again without waiting. (meanings and names guessed)
                unsigned long m_nUnk40;    // 0x40
                unsigned char m_Unk44[0x184];
                unsigned long m_nUnk1C8;   // 0x1C8
                unsigned char m_Unk1CC[0x46C];
            };

            CGame(bcs::CTask *pParent, GAME_MODE mode);
            virtual ~CGame();
            virtual void Main();

            static void SetDefaultParam();
            // (meaning guessed) The task new objects of the running game are
            // created under (BossF's bomb explosions).
            static bcs::CTask *GetTempParent();

            // The three below are static in the DOL: their bodies index
            // g_GameWork with the argument. (return types guessed)
            // Which character player nPlayer has: the byte at
            // g_GameWork + 0x48 + nPlayer, read signed. The radar treats 0x13
            // and above differently (most likely the Mii characters; not
            // checked).
            static long GetCharType(long nPlayer);
            // The colour of team nTeam: the byte at g_GameWork + 0x80 +
            // nTeam, read unsigned. The radar uses it as an index of four.
            static long GetTeamColorIdx(long nTeam);
            // The result of team nTeam: the byte at g_GameWork + 0xFC + nTeam,
            // read unsigned. What the values mean is not known;
            // CTmGameHandler::_SetBattleWin takes it and _IsBattleWin tests
            // for 1.
            static long GetTeamResult(long nTeam);
            // The difficulty of the game (see GameWork::m_nGameLevel).
            // Inline; the parameter files emit it.
            static long GetGameLevel() { return g_GameWork.m_nGameLevel; }

            // (DOL; return types guessed) Team nTeam's score.
            static long GetTeamScore(long nTeam);
            // Not zero when team nTeam may still take an item.
            static int CanbeTeamItemPush(long nTeam);
            // Adds nCoin coins to team nTeam.
            static void AddTeamCoin(long nTeam, long nCoin);

            static GameWork g_GameWork;
            static CGame *m_pInstance;

        private:
            // The constructor writes up to 0x14C, so the class is at least
            // 0x150 bytes; its real size is not known.
            unsigned char m_Unk34[0x150 - 0x34];
    };
}

#endif /* !CGAME_HPP_ */
