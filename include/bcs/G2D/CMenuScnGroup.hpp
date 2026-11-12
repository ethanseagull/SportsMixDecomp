#ifndef BCS_G2D_CMENUSCNGROUP_HPP_
#define BCS_G2D_CMENUSCNGROUP_HPP_

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the static member used so far (added for
        // game/msw/CNowLoading.cpp).
        class CMenuScnGroup {
            public:
                // The 2D screen's orthographic projection, 0x20 bytes of
                // floats. Field meanings from how CNowLoading::DrawLayout
                // passes them to C_MTXOrtho; names guessed.
                struct OrthoProjInfo {
                    float fNear;        // 0x00
                    float fFar;         // 0x04
                    float fLeft;        // 0x08
                    float fRight;       // 0x0C
                    float fBottom;      // 0x10
                    float fTop;         // 0x14
                    float fUnk18[2];    // 0x18
                };

                static OrthoProjInfo s_OrthoProjInfo;
        };
    }
}

#endif /* !BCS_G2D_CMENUSCNGROUP_HPP_ */
