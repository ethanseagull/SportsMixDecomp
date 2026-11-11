#ifndef MSW_CCHRMARKERBOSS_HPP_
#define MSW_CCHRMARKERBOSS_HPP_

#include "game/msw/CChrMarker.hpp"

namespace msw {

    // The character marker of the boss game (BossF). It adds no members to
    // CChrMarker (size 0xA0); it only replaces the two checks that decide
    // when the marker is shown.
    class CChrMarkerBoss : public CChrMarker {
        public:
            CChrMarkerBoss(bcs::CTask *pParent, long nChr, long nType, long nArg);
            // Not inline: the original emits it right after the constructor.
            virtual ~CChrMarkerBoss();
            virtual void MainFunc();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };
}

#endif /* !MSW_CCHRMARKERBOSS_HPP_ */
