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

    // (DOL; added for BossF game/boss/CComDrvBoss.cpp, parameter names and
    // meaning guessed) Which side of the line from (x0, z0) to (x1, z1) the
    // point (x, z) is on: negative, zero on the line, or positive.
    long BCS_Util_IsPointSide(float x, float z, float x0, float z0, float x1, float z1);

    // In the DOL (added for BasketF game/basket/CChrManBasket.cpp;
    // parameter meanings guessed). Not zero when rPos lies in the face
    // spanned from rFrom to rTo with the two widths, in the XZ plane.
    int BCS_Util_IsPointInFaceXZ(const FVEC3 &rPos, const FVEC3 &rFrom, const FVEC3 &rTo, float, float);

    // In the DOL. Clears size bytes at p, 32 bytes at a time.
    void BCS_CpuClear32(void *p, unsigned long size);

    // (added for MenuF game/menu/CSelectMenu.cpp, which emits it; name
    // from the symbol) Clears size bytes at p.
    inline void CPU_CLEAR(void *p, unsigned long size) { memset(p, 0, size); }

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
