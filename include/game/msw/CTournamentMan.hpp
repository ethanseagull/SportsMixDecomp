#ifndef CTOURNAMENTMAN_HPP_
#define CTOURNAMENTMAN_HPP_

namespace msw {

    // The tournament map's manager (module TourF). Only the members used so
    // far.
    class CTournamentMan {
        public:
            static CTournamentMan *GetInstance();

            // Deletes the two objects the manager keeps for character
            // `nIndex`; with bDestroy set it also has msw::CEvDemoRes
            // destroy the character's model. (parameter names guessed)
            void CleanChrModel(long nIndex, int bDestroy);
    };
}

#endif /* !CTOURNAMENTMAN_HPP_ */
