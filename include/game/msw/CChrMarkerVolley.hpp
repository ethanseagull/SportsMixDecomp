#ifndef MSW_CCHRMARKERVOLLEY_HPP_
#define MSW_CCHRMARKERVOLLEY_HPP_

#include "game/msw/CChrMarker.hpp"

namespace msw {

    // The character marker of the volley game (VolleyF). It adds no members
    // to CChrMarker (size 0x9C); it only replaces the two checks that decide
    // when the marker is shown.
    class CChrMarkerVolley : public CChrMarker {
        public:
            CChrMarkerVolley(bcs::CTask *pParent, long nChr, long nType, long nArg);
            // Not inline: the original emits it right after the constructor.
            virtual ~CChrMarkerVolley();
            virtual void MainFunc();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };

    // The second marker of the volley game, shown instead of the normal one
    // while the character's spike jump marker is up. It turns about the Y
    // axis and is drawn larger. Derived from CChrMarker directly, with no
    // members of its own.
    class CChrMarkerVolleyJump : public CChrMarker {
        public:
            CChrMarkerVolleyJump(bcs::CTask *pParent, long nChr, long nType, long nArg);
            virtual ~CChrMarkerVolleyJump();
            virtual void MainFunc();
            virtual int AppearCheck();
            virtual int DisappearCheck();
            virtual float GetDispScale() const;
    };
}

#endif /* !MSW_CCHRMARKERVOLLEY_HPP_ */
