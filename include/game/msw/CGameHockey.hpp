#ifndef CGAMEHOCKEY_HPP_
#define CGAMEHOCKEY_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // All three live in module HockeyF.

    // Size 0x2D4.
    class CGameHockey : public CGame {
        public:
            CGameHockey(bcs::CTask *pParent);

            // The hockey game's own virtual functions, after CGame's
            // (vtable 0x2A8 to 0x2B4). Added for
            // game/hockey/CCockpitHockeyVSCount.cpp; return types guessed.
            // The party game classes override the last two (layout.py), so
            // the cockpit calls them through this class.
            virtual long GetCrashScore(long nTeam);          // 0x2A8
            virtual float GetForceMoveSpeed();               // 0x2AC
            // The character index team nTeam plays with in a party-game
            // mission (compared with CChrCore's player number).
            virtual long GetKSMissionPlayChrIdx(long nTeam); // 0x2B0
            virtual long GetKSMissionWinTeamNo();            // 0x2B4

        private:
            unsigned char m_Unk150[0x2D4 - sizeof(CGame)];
    };

    // The hockey party game. Size 0x3E4. Derives from CGame directly, not
    // from CGameHockey.
    class CGameHockeyCrashSkate : public CGame {
        public:
            CGameHockeyCrashSkate(bcs::CTask *pParent);

        private:
            unsigned char m_Unk150[0x3E4 - sizeof(CGame)];
    };

    // The party game as played in a tournament's extra match. Size 0x408.
    class CGameHockeyCSMission : public CGameHockeyCrashSkate {
        public:
            CGameHockeyCSMission(bcs::CTask *pParent);

        private:
            unsigned char m_Unk3E4[0x408 - 0x3E4];
    };
}

#endif /* !CGAMEHOCKEY_HPP_ */
