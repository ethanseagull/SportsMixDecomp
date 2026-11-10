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
                // Compared as a signed int too (HockeyF
                // CEventDemoHockey::CameraEvent).
                long GetStage() const { return m_nStage; }

                unsigned char m_Unk00[4];
                unsigned char m_nGameMode; // 0x04, the GAME_MODE of the running game
                unsigned char m_nGameType; // 0x05, 2 selects the sport's party game
                unsigned char m_Unk06[4];
                // 0x0A, the stage being played (name guessed; the DOL's
                // CEventDemoBase::_Init copies it, and HockeyF tests 0x11
                // before calling CStageHockey17)
                unsigned char m_nStage;
                unsigned char m_Unk0B;
                unsigned char m_nPeriod;   // 0x0C, the period being played; 0 is the first (guess)
                unsigned char m_Unk0D[0x18 - 0x0D];
                // 0x18: flag bits (name guessed). BasketF's
                // CStageManBasket::Main_ skips the change-of-offense sound
                // while any of 0x4306 is set.
                unsigned long m_nFlag18;
                unsigned char m_Unk1C[0x160 - 0x1C];
                // 0x160: counted by tournament mission 0x3E (HockeyF) and
                // passed to the coin mission (0x4E) check after a basket
                // (BasketF); meaning not known.
                long m_Unk160;
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
            // The score of team nTeam in period nPeriod. (DOL, static; return type guessed)
            static long GetTeamPeriodScore(long nTeam, long nPeriod);
            // The coins of team nTeam: the word at g_GameWork + 0x9C +
            // 4 * nTeam, 0 for a negative team.
            static long GetTeamCoin(long nTeam);
            // The score of team nTeam, from a word array in the DOL's data.
            static long GetTeamScore(long nTeam);
            // Passes a message to the running game's virtual at vtable
            // offset 0x3C, if there is a game.
            static void SendMessage(long nMsg, long, long, long);
            // The goal team nTeam attacks: the byte at g_GameWork + 0x88 +
            // nTeam, read unsigned. (return type guessed)
            static long GetTargetGoalIdx(long nTeam);
            // Whether player nPlayer's character is the hockey keeper (bit 1
            // of the byte at g_GameWork + 0x58 + nPlayer). (return type guessed)
            static int IsCharEx_HockeyKeeper(long nPlayer);

            static GameWork g_GameWork;
            static CGame *m_pInstance;

        private:
            // The constructor writes up to 0x14C, so the class is at least
            // 0x150 bytes; its real size is not known.
            unsigned char m_Unk34[0x150 - 0x34];
    };
}

#endif /* !CGAME_HPP_ */
