#ifndef SELECTMENU_CPARTYMEHELPBOSSPACKUN_HPP_
#define SELECTMENU_CPARTYMEHELPBOSSPACKUN_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A4. The help screen of the party game played against Petey Piranha ("me_ptg_operation_BA").
        // Its buttons are read from a ".btn" file by the CButtonEntry base
        // at 0x5C (a second base, as in CTeam). Defined in
        // game/menu/CPartyMEHelpBossPackun.cpp with the other party game
        // help screens.
        class CPartyMEHelpBossPackun : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CPartyMEHelpBossPackun(long nId);
                virtual ~CPartyMEHelpBossPackun();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

            private:
                // Shows the panes that match the controller style of pad 0
                // when it has changed. Inlined into Main() and Set().
                void UpdatePadStyle();

                // 0x5A0. (name guessed) The controller style the panes
                // show, -1 before the first.
                long m_nPadStyle;
        };
    }
}

#endif /* !SELECTMENU_CPARTYMEHELPBOSSPACKUN_HPP_ */
