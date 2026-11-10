#ifndef BCS_BCS_PHYS_HPP_
#define BCS_BCS_PHYS_HPP_

// Simple physics helpers of the bcs namespace. Which header declared them is
// not known: the file name is a guess.

namespace bcs {

    // In the DOL. (meaning of the arguments guessed) Solves for the start
    // speed rVel and the gravity rGravity of a throw that takes fFrame
    // frames from height fStartY to fEndY and peaks at fTopY.
    void BCS_Phys_CalcYVG(float fFrame, float fStartY, float fEndY, float fTopY, float &rVel, float &rGravity);
}

#endif /* !BCS_BCS_PHYS_HPP_ */
