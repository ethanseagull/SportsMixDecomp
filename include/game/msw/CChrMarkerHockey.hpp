#ifndef MSW_CCHRMARKERHOCKEY_HPP_
#define MSW_CCHRMARKERHOCKEY_HPP_

#include "game/msw/CChrMarker.hpp"

namespace msw {

    // INCOMPLETE: the character markers of the hockey game (HockeyF). Both
    // derive from CChrMarker and are allocated with its 0xA0 bytes
    // (game/hockey/CChrManHockey.cpp); only the constructors are declared.
    // The arrow's constructor has no type argument. The destructors are
    // declared without bodies so that no unit emits the vtables from here.
    class CChrMarkerHockey : public CChrMarker {
        public:
            CChrMarkerHockey(bcs::CTask *pParent, long nChr, long nType, long nArg);
            virtual ~CChrMarkerHockey();
    };

    class CChrMarkerHockeyArrow : public CChrMarker {
        public:
            CChrMarkerHockeyArrow(bcs::CTask *pParent, long nChr, long nArg);
            virtual ~CChrMarkerHockeyArrow();
    };

    // INCOMPLETE: the marker of a one-on-one duel, size 0x9C (what
    // CChrManHockey::CreateMarker_ allocates). Its base in the original is
    // the DOL's CChr1on1MarkerBase (constructor (bcs::CTask *, long)), which
    // is not declared yet; bcs::CTask stands in for it.
    class CChr1on1MarkerHockey : public bcs::CTask {
        public:
            CChr1on1MarkerHockey(bcs::CTask *pParent, long nChr);
            virtual ~CChr1on1MarkerHockey();
            virtual void Main();

        protected:
            unsigned char m_Unk34[0x9C - 0x34];
    };
}

#endif /* !MSW_CCHRMARKERHOCKEY_HPP_ */
