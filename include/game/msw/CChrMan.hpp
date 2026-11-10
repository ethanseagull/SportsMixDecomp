#ifndef MSW_CCHRMAN_HPP_
#define MSW_CCHRMAN_HPP_

#include "bcs/CUnivTask.hpp"

namespace msw {

    class CChrCore;

    // Owner of the characters on the court. A DOL class and a task: its
    // constructor calls bcs::CTask's with priority 0x14 and its vtable has
    // bcs::CUnivTask's GetFrameRateF.
    //
    // INCOMPLETE: only what game/boss/CChrMarkerBoss.cpp needs is here. No
    // data members and none of the class's own virtual functions are
    // declared, so it must not be instantiated or derived from yet.
    class CChrMan : public bcs::CUnivTask {
        public:
            // The character with index `nChr` (0 to GetChrCoreNum() - 1), or
            // null when there is no manager or the index is out of range.
            // Static: it finds the instance itself.
            static CChrCore *GetChrCore(long nChr);
            // The number of characters: the word at 0x50 of the instance,
            // zero when there is none. Static, like GetChrCore.
            static long GetChrCoreNum();
            // Shows or hides the markers over the characters. Static.
            static void SetChrMarkerDisp(int bDisp);
            // The one instance. Static. (BasketF casts it to its
            // CChrManBasket)
            static CChrMan *GetInstance();
            // (name guessed; fn_800EC2E4, unnamed in the DOL) Not zero when
            // a character that is not disabled has bit 0x20000 of the word
            // at 0xE8 set (taken to be the star state). Static.
            static int IsExistStarChr();
    };
}

#endif /* !MSW_CCHRMAN_HPP_ */
