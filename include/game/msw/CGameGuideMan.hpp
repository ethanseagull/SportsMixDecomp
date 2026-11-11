#ifndef MSW_CGAMEGUIDEMAN_HPP_
#define MSW_CGAMEGUIDEMAN_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CChrCore;

    // INCOMPLETE: the in-game guide (help telops and line messages over the
    // characters). A DOL class; only the static functions that
    // game/volley/CChrManVolley.cpp calls are declared. Parameter meanings
    // and return types are guesses.
    class CGameGuideMan {
        public:
            // Not zero while guides may be shown.
            static int IsDispEnable();
            // Shows guide telop nType for pChr for fTime frames, offset by
            // *pOfs. The last three numbers are not understood (-1 when not
            // set).
            static void StartGuideTelop(long nType, CChrCore *pChr, float fTime, const bcs::FVEC3 *pOfs, float, float, float);
            static void EndGuideTelop(long nType);
            // Shows line message nType over pChr.
            static void StartLineMsg(long nType, CChrCore *pChr, int, float);
            static void EndLineMsg(int);
    };
}

#endif /* !MSW_CGAMEGUIDEMAN_HPP_ */
