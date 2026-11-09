#ifndef BCS_FVEC3_HPP_
#define BCS_FVEC3_HPP_

namespace bcs {

    // Three floats. Only the layout is known (msw::CTableSetMdlObj::
    // GetNodePosition in MenuF writes them at 0x0, 0x4 and 0x8).
    struct FVEC3 {
        float x; // 0x00
        float y; // 0x04
        float z; // 0x08

        // (name guessed) Inline in the modules.
        void Set(float fx, float fy, float fz)
        {
            x = fx;
            y = fy;
            z = fz;
        }
    };
}

#endif /* !BCS_FVEC3_HPP_ */
