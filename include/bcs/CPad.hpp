#ifndef BCS_CPAD_HPP_
#define BCS_CPAD_HPP_

namespace bcs {

    // The engine's pad input. Lives in the DOL. Only what callers use so
    // far; parameter meanings are guesses.
    class CPad {
        public:
            // (added for game/msw/CGame.cpp, which resets it to 0x47, 0xF, 0
            // when a game ends) The range the nunchuk stick is clamped to.
            static void SetNunchukStickClamp(long nMax, long nMin, int bUnk);
    };
}

#endif /* !BCS_CPAD_HPP_ */
