#ifndef MSW_TOURNAMENTWORK_HPP_
#define MSW_TOURNAMENTWORK_HPP_

namespace msw {

    // The state of the running tournament. Lives in the DOL. Whether
    // TournamentWork is a namespace or a class with static members cannot be
    // told from the symbols (it has nested types, such as
    // msw::TournamentWork::TOURNAMENT_INFO); a namespace is a guess.
    namespace TournamentWork {

        // The tournament's data, one static object in the DOL. Only the
        // member used so far is mapped; msw::CTmGameHandler also reads and
        // writes bytes up to 0xFBB, and the object goes on past 0x1060.
        struct TOURNAMENT_INFO {
            // One mission (rule) of the match, 0x14 bytes. Read by the
            // modules' tournament handlers (VolleyF CTmGameHandlerVolley);
            // the meaning of the members is inferred from those uses.
            struct MISSION {
                signed char nType;    // 0x00, what the mission counts (0x13 to 0x3F seen)
                unsigned char nFlags; // 0x01, 0x40 seen tested: the mission is shown in an extra match
                short nTarget;        // 0x02, the count to reach
                short nCount;         // 0x04, the count so far
                short nUnk06;         // 0x06, passed with nCount to the count-up display
                unsigned char m_Unk08[0x14 - 0x08];
            };

            unsigned char m_Unk0000[0xA20];
            MISSION aMission[16]; // 0xA20
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
