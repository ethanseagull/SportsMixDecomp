#ifndef BCS_BCS_RAND_HPP_
#define BCS_BCS_RAND_HPP_

// Random numbers of the bcs namespace. Which header declared them is not
// known: the file name is a guess.

namespace bcs {

    // In the DOL. A random float in [0, max) (range not checked). The two
    // draw from different generators; which is which is not known.
    float BCS_RandMax(float max);
    float BCS_OrgRandMax(float max);
    // In the DOL. A random integer up to max (whether max itself can come
    // out is not checked; DodgeF calls it with 99 for a percentage).
    long BCS_RandMax(long max);
    // In the DOL. A random integer over the generator's whole range
    // (return type guessed; TournamentRule passes it on as a long).
    long BCS_Rand();
    // In the DOL. A random float, taken to be in [0, 1) (BasketF
    // CStageBasket13 scales a period by it to get a start time).
    float BCS_RandF();
    // In the DOL. A random number between min and max (whether max itself
    // can come out is not checked). BasketF's CBallBasketFruit sorts the
    // two longs before the call.
    float BCS_RandRange(float min, float max);
    long BCS_RandRange(long min, long max);
    // In the DOL. As BCS_RandRange(float, float), from the generator of
    // BCS_OrgRandMax.
    float BCS_OrgRandRange(float min, float max);
    // In the DOL (added for MenuF game/menu/CWiFiGame.cpp). Seeds the
    // generator.
    void BCS_SRand(unsigned long nSeed);

    // (inline: TourF game/tour/CTournamentStep.cpp has a weak copy, a
    // tail call) BCS_RandMax for an integer.
    inline long BCS_RandMaxI(long max) { return BCS_RandMax(max); }
}

#endif /* !BCS_BCS_RAND_HPP_ */
