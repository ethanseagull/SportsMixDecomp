#ifndef SELECTMENU_CCHARSEL_HPP_
#define SELECTMENU_CCHARSEL_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x7BC. The character select screen (menu object 0x45), with
        // the characters standing on the court. A second base CButtonEntry
        // follows CSelectMenuLayoutBase (its destructor has a `@28@`
        // thunk). Only the members used so far are written; do not create
        // or destroy one through this header. Return types and argument
        // names are guesses.
        class CCharSel : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                // Added for MenuF game/menu/CCharaStatus.cpp. Shows or hides
                // the pad sign over character slot nIndex.
                void SetCharaObjPadSign(long nUnk, long nIndex);
                void ClearPadSign(long nIndex, long nUnk);
                // The court characters of slot nIndex (CCharaStatus passes
                // a motion and its variant).
                void SetCourtCharaObjImplEx(long nUnk, long nMotion, long nMotionSub, long nIndex);
                void SetCourtCharaObjMotionEx(long nIndex, long nMotion, long nMotionSub);
                void SetCourtCharaObj(long nUnk0, long nMotion, long nMotionSub, long nUnk1);
        };
    }
}

#endif /* !SELECTMENU_CCHARSEL_HPP_ */
