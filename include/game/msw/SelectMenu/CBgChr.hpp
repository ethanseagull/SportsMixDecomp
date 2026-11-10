#ifndef SELECTMENU_CBGCHR_HPP_
#define SELECTMENU_CBGCHR_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // The characters standing in the menu background; menu object 0x3A
        // of CSelectMenu. Only the members used so far: its virtuals are
        // not declared, so never create one through this header. Return
        // and argument types are guesses unless the mangled name gives
        // them.
        class CBgChr : public CSelectMenuLayoutBase {
            public:
                void SetDispTagName(int bDisp);
                void SetSelectGameChara();
                // The camera position index of the character of pad nPad.
                long GetPosChara(long nPad);
                void SetCharaSpeed(float fSpeed);
        };
    }
}

#endif /* !SELECTMENU_CBGCHR_HPP_ */
