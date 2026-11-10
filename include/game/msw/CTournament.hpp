#ifndef CTOURNAMENT_HPP_
#define CTOURNAMENT_HPP_

namespace msw {

    // Only the members used so far. Both are static: their bodies in the DOL
    // work on globals and take no object.
    class CTournament {
        public:
            static void Initialize(long);
            static int IsExtraMatchGame(); // returns 0 or 1; type guessed
            // The low three bits of the tournament handler's word at 0x34
            // (set from a mission of type 0x11 in BasketF), 0 without a
            // handler. (return type guessed)
            static long GetMissionStartMode();
    };
}

#endif /* !CTOURNAMENT_HPP_ */
