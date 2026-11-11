#ifndef MSW_CCHRMARKERHOCKEY_HPP_
#define MSW_CCHRMARKERHOCKEY_HPP_

#include "game/msw/CChrMarker.hpp"

namespace msw {

    // The character markers of the hockey game (HockeyF
    // game/hockey/CShootMarker.cpp). Both derive from CChrMarker and are
    // allocated with its 0xA0 bytes (game/hockey/CChrManHockey.cpp). The
    // arrow's constructor has no type argument (it is type 4). The
    // destructors are declared without bodies so that no other unit emits
    // the vtables.
    class CChrMarkerHockey : public CChrMarker {
        public:
            CChrMarkerHockey(bcs::CTask *pParent, long nChr, long nType, long nArg);
            virtual ~CChrMarkerHockey();

            virtual void MainFunc();
            virtual void ArrowSignalCheck();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };

    class CChrMarkerHockeyArrow : public CChrMarker {
        public:
            CChrMarkerHockeyArrow(bcs::CTask *pParent, long nChr, long nArg);
            virtual ~CChrMarkerHockeyArrow();

            virtual void MainFunc();
            virtual int AppearCheck();
            virtual int DisappearCheck();
    };
}

#endif /* !MSW_CCHRMARKERHOCKEY_HPP_ */
