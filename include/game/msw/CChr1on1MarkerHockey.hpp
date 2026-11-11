#ifndef MSW_CCHR1ON1MARKERHOCKEY_HPP_
#define MSW_CCHR1ON1MARKERHOCKEY_HPP_

#include "game/msw/CChr1on1MarkerBase.hpp"

namespace msw {

    // The marker of a one-on-one duel, size 0x9C (what
    // CChrManHockey::CreateMarker_ allocates). Its functions are in
    // HockeyF's game/hockey/CShootMarker.cpp; a header of its own, so that
    // the file can declare it after CShootMarker (the order of the vtables
    // in .data follows the order of the declarations).
    class CChr1on1MarkerHockey : public CChr1on1MarkerBase {
        public:
            CChr1on1MarkerHockey(bcs::CTask *pParent, long nChr);
            // Inline: the original has it among the weak functions at the
            // end of game/hockey/CShootMarker.cpp.
            virtual ~CChr1on1MarkerHockey() {}

            virtual int CheckAppear();
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx);
    };
}

#endif /* !MSW_CCHR1ON1MARKERHOCKEY_HPP_ */
