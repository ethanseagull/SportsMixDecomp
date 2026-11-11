#ifndef BCS_BCS_UTIL_HPP_
#define BCS_BCS_UTIL_HPP_

#include <string.h>

// Utility functions of the bcs namespace (BCS_CpuClear32 sits with
// BCS_LotProbability and BCS_Util_IsPointSide in the DOL). Which header
// declared them is not known: the file name is a guess.

namespace bcs {

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
