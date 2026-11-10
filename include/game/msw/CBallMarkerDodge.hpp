#ifndef MSW_CBALLMARKERDODGE_HPP_
#define MSW_CBALLMARKERDODGE_HPP_

#include "game/msw/CBallMarker.hpp"

namespace msw {

    // The marker of the dodge game's balls (DodgeF), size 0x90: CBallMarker
    // with no members of its own. The constructor is inline (CBallManDodge
    // has it after the CBallMarker constructor call); AppearCheck and the
    // destructor are in game/dodge/CBallMarkerDodge.cpp.
    class CBallMarkerDodge : public CBallMarker {
        public:
            CBallMarkerDodge(bcs::CTask *pParent, unsigned long nBallHandle, long nArg)
                : CBallMarker(pParent, nBallHandle, nArg)
            {
            }
            virtual ~CBallMarkerDodge();

            virtual int AppearCheck();
    };
}

#endif /* !MSW_CBALLMARKERDODGE_HPP_ */
