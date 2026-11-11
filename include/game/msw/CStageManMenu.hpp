#ifndef MSW_CSTAGEMANMENU_HPP_
#define MSW_CSTAGEMANMENU_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // Lives in the DOL. The stage behind the menus. Size 0x1FC, the same as
    // CStageMan (what MenuF's CSelectMenu::TopMenu_Loop allocates). Only
    // the constructor is declared so far.
    class CStageManMenu : public CStageMan {
        public:
            CStageManMenu(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSTAGEMANMENU_HPP_ */
