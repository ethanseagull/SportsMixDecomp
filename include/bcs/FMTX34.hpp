#ifndef BCS_FMTX34_HPP_
#define BCS_FMTX34_HPP_

namespace bcs {

    // A 3x4 matrix, rows first: the translation is m[0][3], m[1][3], m[2][3]
    // (read that way by msw::CTableSetMdlObj::GetNodePosition in MenuF).
    // The member name is guessed.
    struct FMTX34 {
        float m[3][4]; // 0x00
    };
}

#endif /* !BCS_FMTX34_HPP_ */
