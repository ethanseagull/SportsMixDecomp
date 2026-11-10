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
            unsigned char m_Unk0000[0x1060];
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
