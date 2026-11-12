#ifndef BCS_FVEC2_HPP_
#define BCS_FVEC2_HPP_

namespace bcs {

    // Two floats. The DOL takes it by reference (BCS_GetLineToEllipseCross,
    // CPadWI::GetScreenPosDPD_F); BasketF, DodgeF and HockeyF emit the two
    // constructors below out of line (`__ct__Q23bcs5FVEC2Fff`,
    // `__ct__Q23bcs5FVEC2FRCQ23bcs5FVEC2`). Member names follow FVEC3.
    struct FVEC2 {
        float x; // 0x00
        float y; // 0x04

        FVEC2()
        {
        }

        FVEC2(float fx, float fy)
        {
            x = fx;
            y = fy;
        }

        FVEC2(const FVEC2 &v)
        {
            x = v.x;
            y = v.y;
        }

        // Added for MenuF game/menu/CBgDisplay.cpp (emitted out of line
        // there as Set__Q23bcs5FVEC2Fff).
        void Set(float fx, float fy)
        {
            x = fx;
            y = fy;
        }
    };
}

#endif /* !BCS_FVEC2_HPP_ */
