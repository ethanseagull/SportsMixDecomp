#ifndef BCS_G2D_CLYTMANAGER_HPP_
#define BCS_G2D_CLYTMANAGER_HPP_

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
        };
    }
}

#endif /* !BCS_G2D_CLYTMANAGER_HPP_ */
