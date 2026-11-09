#ifndef TOURNAMENT_HPP_
#define TOURNAMENT_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    // Entry point of module TourF. Whether "Tournament" is a namespace or a
    // class with a static member cannot be told from the symbol; a namespace
    // is a guess.
    namespace Tournament {
        // Creates the tournament's root task (a msw::CTournamentStep) and
        // returns it.
        bcs::CTask *CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !TOURNAMENT_HPP_ */
