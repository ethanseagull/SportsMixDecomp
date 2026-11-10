#ifndef NW4R_LYT_PANE_HPP_
#define NW4R_LYT_PANE_HPP_

#include "nw4r/math/types.h"

namespace nw4r {
    namespace lyt {

        // Width and height of a pane.
        struct Size {
            float width;  // 0x00
            float height; // 0x04
        };

        // One node of a layout. Lives in the DOL. Only the members used so
        // far; the rest is opaque, including the vptr at 0, so the class must
        // not be instantiated or derived from yet.
        //
        // Offsets are what the game's own code shows (msw::CRadarIcon2D in
        // the DOL and its classes in the modules): translation at 0x2C,
        // scale at 0x44, size at 0x4C, alpha at 0xB8, flags at 0xBB, name at
        // 0xBC. Member and function names follow the public ogws
        // decompilation's description of nw4r::lyt::Pane (from memory, no
        // code copied).
        class Pane {
            public:
                void SetSize(const Size &rSize) { mSize = rSize; }
                const char *GetName() const { return mName; }
                // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp.
                void SetTranslate(const math::VEC3 &rTranslate) { mTranslate = rTranslate; }
                // Bit 0 of the flags.
                void SetVisible(bool bVisible)
                {
                    mFlag = (unsigned char)((mFlag & 0xFE) | (bVisible ? 1 : 0));
                }

            private:
                unsigned char m_Unk00[0x2C];
                math::VEC3 mTranslate;       // 0x2C
                unsigned char m_Unk38[0x14];
                Size mSize;                  // 0x4C
                unsigned char m_Unk54[0x67];
                unsigned char mFlag;         // 0xBB, bit 0: visible
                char mName[17];              // 0xBC
        };
    }
}

#endif /* !NW4R_LYT_PANE_HPP_ */
