#ifndef BCS_FMTX34_HPP_
#define BCS_FMTX34_HPP_

#include "revolution/mtx.h"

namespace bcs {

    // A 3x4 matrix, rows first: the translation is m[0][3], m[1][3], m[2][3]
    // (read that way by msw::CTableSetMdlObj::GetNodePosition in MenuF).
    // The member name is guessed.
    struct FMTX34 {
        float m[3][4]; // 0x00
    };

    // (name guessed) *pOut = *p, skipped when both are the same matrix.
    // Inline in the original: msw::CGameExDemoMaskObj::Draw (DOL) compares
    // the two pointers before calling PSMTXCopy. Most likely NW4R's
    // MTX34Copy.
    inline FMTX34 *FMTX34Copy(FMTX34 *pOut, const FMTX34 *p)
    {
        if (pOut != p) {
            PSMTXCopy(p->m, pOut->m);
        }
        return pOut;
    }
}

#endif /* !BCS_FMTX34_HPP_ */
