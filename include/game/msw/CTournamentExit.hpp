#ifndef CTOURNAMENTEXIT_HPP_
#define CTOURNAMENTEXIT_HPP_

namespace msw {

    // The "leave the tournament" menu of the map (module TourF). Only the
    // member used so far.
    class CTournamentExit {
        public:
            // (meaning guessed) Whether the menu may be opened; TourF's
            // Stage33 clears it while the map fades out for an airship
            // flight and sets it again afterwards. A byte.
            static bool m_bEnable;
    };
}

#endif /* !CTOURNAMENTEXIT_HPP_ */
