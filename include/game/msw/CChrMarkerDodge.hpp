#ifndef MSW_CCHRMARKERDODGE_HPP_
#define MSW_CCHRMARKERDODGE_HPP_

#include "game/msw/CChrMarker.hpp"

namespace msw {

    // The character markers of the dodge game (DodgeF), all in
    // game/dodge/CChrMarkerDodge.cpp. None adds members to CChrMarker (size
    // 0x9C). The classes are declared in this order because the original
    // emits their vtables in the reverse order.

    // The plain marker. It only replaces the two checks that decide when
    // the marker is shown; MainFunc stays CChrMarker's (the vtable
    // __vt__Q23msw15CChrMarkerDodge names the base one).
    class CChrMarkerDodge : public CChrMarker {
        public:
            CChrMarkerDodge(bcs::CTask *pParent, long nChr, long nType, long nArg);
            // Not inline: the original emits it right after the constructor.
            virtual ~CChrMarkerDodge();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };

    // Marker type 4: an arrow at the character that turns towards the
    // position of its target marker while that is active.
    class CChrMarkerDodgeArrow : public CChrMarker {
        public:
            CChrMarkerDodgeArrow(bcs::CTask *pParent, long nChr, long nArg);
            virtual ~CChrMarkerDodgeArrow();
            virtual void Disp();
            virtual void MainFunc();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };

    // Marker type 5. Its AppearCheck never lets it come up.
    class CChrMarkerDodgeCloseRange : public CChrMarker {
        public:
            CChrMarkerDodgeCloseRange(bcs::CTask *pParent, long nChr, long nArg);
            virtual ~CChrMarkerDodgeCloseRange();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };

    // The marker of the party game: as CChrMarker's, but only for a
    // character with lives left.
    class CChrMarkerDodgeDodgeOrDie : public CChrMarker {
        public:
            CChrMarkerDodgeDodgeOrDie(bcs::CTask *pParent, long nChr, long nType, long nArg);
            virtual ~CChrMarkerDodgeDodgeOrDie();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };
}

#endif /* !MSW_CCHRMARKERDODGE_HPP_ */
