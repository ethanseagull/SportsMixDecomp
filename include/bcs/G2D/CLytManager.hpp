#ifndef BCS_G2D_CLYTMANAGER_HPP_
#define BCS_G2D_CLYTMANAGER_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    namespace G2D {
        class CBase;

        // Lives in the DOL. Only the members used so far.
        class CLytManager {
            public:
                static CLytManager *GetInstance();
                // (name guessed; the DOL's fn_8003F418) Creates the instance
                // kept in the DOL's sbss when there is none and returns it.
                static CLytManager *CreateInstance();
                // Null when no object has that id. The return type is a
                // guess (the function reads a pointer out of a list node).
                CBase *GetObject(unsigned long nId);

                // Added for TourF game/tour/CCockpitTournamentMenuWinTrophy.cpp.
                // Returns 1 when the object was added; nBit is a bit number.
                int AddObject(CBase *pObj, long nBit);

                // Added for game/msw/cockpit/CMissionCount.cpp (C3DCoin): the
                // scale that puts a location-adjusted pane in place, for the
                // aspect mode bAdjust. The vector (the DOL's fn_8003FE7C,
                // name guessed; written to pScale, which the function takes
                // after `this`) has the horizontal scale in x and 1 in y and
                // z; GetLocationAdjustScaleH is the vertical one.
                void GetLocationAdjustScale(FVEC3 *pScale, int bAdjust);
                float GetLocationAdjustScaleH(int bAdjust);
                // (name guessed) Bit 0 of the word at 0x44, what C3DCoin
                // passes to both.
                int GetAdjustMode() const { return m_nFlag & 1; }

            private:
                unsigned char m_Unk00[0x44];
                unsigned long m_nFlag; // 0x44 (name guessed)
        };
    }
}

#endif /* !BCS_G2D_CLYTMANAGER_HPP_ */
