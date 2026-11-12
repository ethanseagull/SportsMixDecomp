#ifndef BCS_G2D_CMENUSCNGROUP_HPP_
#define BCS_G2D_CMENUSCNGROUP_HPP_

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Added for MenuF game/menu/CBgDisplay.cpp with
        // only the static orthographic projection of the menu screens; the
        // class itself is not written yet.
        class CMenuScnGroup {
            public:
                // The box C_MTXOrtho gets (names guessed from that use: the
                // first two words are its near and far planes, then left,
                // right, bottom and top).
                struct OrthoProjInfo {
                    float m_fNear;   // 0x00
                    float m_fFar;    // 0x04
                    float m_fLeft;   // 0x08
                    float m_fRight;  // 0x0C
                    float m_fBottom; // 0x10
                    float m_fTop;    // 0x14
                    float m_Unk18;   // 0x18 (size 0x20 in the DOL)
                    float m_Unk1C;   // 0x1C
                };

                static OrthoProjInfo s_OrthoProjInfo;
        };
    }
}

#endif /* !BCS_G2D_CMENUSCNGROUP_HPP_ */
