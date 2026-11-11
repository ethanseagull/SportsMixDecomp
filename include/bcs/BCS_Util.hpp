#ifndef BCS_BCS_UTIL_HPP_
#define BCS_BCS_UTIL_HPP_

#include <string.h>

// Utility functions of the bcs namespace (BCS_CpuClear32 sits with
// BCS_LotProbability and BCS_Util_IsPointSide in the DOL). Which header
// declared them is not known: the file name is a guess.

namespace bcs {

    struct FVEC3;

    // In the DOL (added for BasketF game/basket/CNpcGhostBasketBase.cpp;
    // parameter names guessed). Draws an index below nNum, each with the
    // weight pTbl gives it.
    long BCS_LotProbability(const long *pTbl, long nNum);

    // In the DOL (added for BasketF game/basket/CChrManBasket.cpp;
    // parameter meanings guessed). Not zero when rPos lies in the face
    // spanned from rFrom to rTo with the two widths, in the XZ plane.
    int BCS_Util_IsPointInFaceXZ(const FVEC3 &rPos, const FVEC3 &rFrom, const FVEC3 &rTo, float, float);

    // In the DOL. Clears size bytes at p, 32 bytes at a time.
    void BCS_CpuClear32(void *p, unsigned long size);

    // (name guessed) Inline in msw::CStgGmk_WaterPillar's calls of the
    // actor's state queue: short blocks go to memset.
    inline void BCS_CpuClear(void *p, unsigned long size)
    {
        if (size >= 0x200) {
            BCS_CpuClear32(p, size);
        } else {
            memset(p, 0, size);
        }
    }
}

#endif /* !BCS_BCS_UTIL_HPP_ */
