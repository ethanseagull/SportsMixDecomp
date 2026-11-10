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
                // Compared as a signed int as well (VolleyF
                // CTmGameHandlerVolley::_SendMessage).
                long GetPeriod() const { return m_nPeriod; }
                // (name guessed) m_nUserTeam, with no team read as team 0;
                // the modules inline it (extsb, then srawi/andc).
                long GetUserTeam() const { return m_nUserTeam < 0 ? 0 : m_nUserTeam; }

                unsigned char m_Unk00[4];
                unsigned char m_nGameMode; // 0x04, the GAME_MODE of the running game
                unsigned char m_nGameType; // 0x05, 2 selects the sport's party game
                unsigned char m_Unk06[6];
                unsigned char m_nPeriod;   // 0x0C, the period being played; 0 is the first (guess)
                unsigned char m_Unk0D[2];
                // 0x0F (name guessed): a team number, -1 for none; read
                // through GetUserTeam (CComManVolley::CanbeTouch, and the
                // same read in BasketF's CComManBasketGame).
                signed char m_nUserTeam;
                unsigned char m_Unk10[0x160 - 0x10];
                long m_Unk160;             // 0x160, counted by tournament mission 0x3E (coins?)
                unsigned char m_Unk164[0x638 - 0x164];
            };

            CGame(bcs::CTask *pParent, GAME_MODE mode);
            virtual ~CGame();
            virtual void Main();

            static void SetDefaultParam();

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
            // The score of team nTeam in period nPeriod; the coins of team
            // nTeam. (DOL, static; return types guessed)
            static long GetTeamPeriodScore(long nTeam, long nPeriod);
            static long GetTeamCoin(long nTeam);
            // Sends a message to the running game (static in the DOL: the
            // first argument goes in r3). Ends in the game's tournament
            // handler, CTmGameHandler::_SendMessage.
            static void SendMessage(long, long, long, long);

            static GameWork g_GameWork;
            static CGame *m_pInstance;

        private:
            // The constructor writes up to 0x14C, so the class is at least
            // 0x150 bytes; its real size is not known.
            unsigned char m_Unk34[0x150 - 0x34];
    };
}

#endif /* !CGAME_HPP_ */
