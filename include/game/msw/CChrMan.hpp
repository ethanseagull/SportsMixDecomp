#ifndef MSW_CCHRMAN_HPP_
#define MSW_CCHRMAN_HPP_

#include "bcs/CUnivTask.hpp"

namespace msw {

    class CChrCore;
    class CNpcBase;

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
            // DOL. The instance, or null.
            static CChrMan *GetInstance();
            // DOL, static. The next NPC of kind `nKind` after pPrev (the
            // first with a null pPrev), or null. (parameter meanings guessed
            // from the callers: BasketF's CChrManBasket::GetBossPackun_ walks
            // the NPCs of kind 2 this way)
            static CNpcBase *NpcSearch(long nKind, CNpcBase *pPrev);
    };
}

#endif /* !MSW_CCHRMAN_HPP_ */
