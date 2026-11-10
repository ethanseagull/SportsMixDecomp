#ifndef BCS_BCS_RAND_HPP_
#define BCS_BCS_RAND_HPP_

// Random numbers of the bcs namespace. Which header declared them is not
// known: the file name is a guess.

namespace bcs {

    // In the DOL. A random float in [0, max) (range not checked). The two
    // draw from different generators; which is which is not known.
    float BCS_RandMax(float max);
    float BCS_OrgRandMax(float max);
}

#endif /* !BCS_BCS_RAND_HPP_ */
