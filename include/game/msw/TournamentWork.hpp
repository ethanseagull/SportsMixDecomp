#ifndef MSW_TOURNAMENTWORK_HPP_
#define MSW_TOURNAMENTWORK_HPP_

namespace msw {

    // The state of the running tournament. Lives in the DOL. Whether
    // TournamentWork is a namespace or a class with static members cannot be
    // told from the symbols (it has nested types, such as
    // msw::TournamentWork::TOURNAMENT_INFO); a namespace is a guess.
    namespace TournamentWork {

        // The tournament's data, one static object in the DOL. Only the
        // members used so far are mapped; msw::CTmGameHandler also reads and
        // writes bytes up to 0xFBB, and the object goes on past 0x1060.
        // One mission of a tournament match, 0x14 bytes, read and written by
        // msw::CTmGameHandler and the game modules' handlers. (type and
        // member names guessed)
        struct MISSION_INFO {
            signed char nType;     // 0x00, what the mission asks for; values are per sport
            unsigned char nFlags;  // 0x01, 0x40: shown on the cockpit's mission counter
            short nTarget;         // 0x02, the count that clears the mission
            short nCount;          // 0x04, the current count
            short nPrevCount;      // 0x06, nCount before the last change
            unsigned char m_Unk08[4];
            float fCountTime;      // 0x0C, the float the count functions store
            unsigned char m_Unk10[4];
        };

        struct TOURNAMENT_INFO {
            unsigned char m_Unk0000[0xA20];
            MISSION_INFO aMission[16]; // 0xA20
            unsigned char m_Unk0B60[0x1060 - 0xB60];
            // The word msw::CTournament::SetDemoType stores (DOL): the demo
            // the tournament map is showing.
            short nDemoType; // 0x1060
        };

        // Both return the address of the same object. (reference or pointer
        // is a guess; so is the const of the first)
        const TOURNAMENT_INFO &GetTournamentInfo();
        TOURNAMENT_INFO &TournamentInfo();
    }
}

#endif /* !MSW_TOURNAMENTWORK_HPP_ */
