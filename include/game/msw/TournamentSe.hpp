#ifndef TOURNAMENTSE_HPP_
#define TOURNAMENTSE_HPP_

namespace msw {

    // Sounds of the tournament map (module TourF). A namespace by the same
    // guess as msw::Tournament: the name has no "C" prefix.
    namespace TournamentSe {
        // Returns a table of 23 sound numbers. The element type is guessed;
        // the tables hold 4-byte values.
        const long *GetVoiceTable(long nIndex);
    }
}

#endif /* !TOURNAMENTSE_HPP_ */
