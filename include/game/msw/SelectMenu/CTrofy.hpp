#ifndef SELECTMENU_CTROFY_HPP_
#define SELECTMENU_CTROFY_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace bcs {
    namespace G2D {
        class CPaneObj;
    }
}

namespace msw {
    namespace dwc {
        class CFriend;
    }

    namespace SelectMenu {

        // Size 0x60. The trophy display of the menus (menu object 0x4D of
        // CSelectMenu). Only the members used so far; names are guesses.
        class CTrofy : public CSelectMenuLayoutBase {
            public:
                virtual ~CTrofy();
                virtual void Main();
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                // Shows trophy nIndex of m_pFriend in pane pPaneObj.
                // Return type guessed.
                void SetWiFiTrofy(long nIndex, bcs::G2D::CPaneObj *pPaneObj);
                // Added for MenuF game/menu/CFriendInfo.cpp (return types
                // guessed): the sparkle on trophy nIndex, and taking all of
                // them off.
                void SetWiFiTrofyEffect(long nIndex, bcs::G2D::CPaneObj *pPaneObj);
                void ClearTrofyEffect();

                // 0x5C. The friend whose trophies are shown (set by
                // SelectMenu::CWiFiFrindVS::Set).
                dwc::CFriend *m_pFriend;
        };
    }
}

#endif /* !SELECTMENU_CTROFY_HPP_ */
