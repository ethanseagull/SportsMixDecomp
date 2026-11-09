#ifndef CTOURNAMENT_HPP_
#define CTOURNAMENT_HPP_

namespace msw {

    // Only the members used so far. Both are static: their bodies in the DOL
    // work on globals and take no object.
    class CTournament {
        public:
            static void Initialize(long);
            static int IsExtraMatchGame(); // returns 0 or 1; type guessed
    };
}

#endif /* !CTOURNAMENT_HPP_ */
