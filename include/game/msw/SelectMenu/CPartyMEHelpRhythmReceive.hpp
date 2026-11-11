#ifndef SELECTMENU_CPARTYMEHELPRHYTHMRECEIVE_HPP_
#define SELECTMENU_CPARTYMEHELPRHYTHMRECEIVE_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace msw {
    namespace SelectMenu {

        // Size 0x5A4. The help screen of a party game ("me_ptg_operation_VO").
        // Its buttons are read from a ".btn" file by the CButtonEntry base
        // at 0x5C (a second base, as in CTeam). Defined in
        // game/menu/CPartyMEHelpBossPackun.cpp with the other party game
        // help screens.
        class CPartyMEHelpRhythmReceive : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                CPartyMEHelpRhythmReceive(long nId);
                virtual ~CPartyMEHelpRhythmReceive();
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

#endif /* !SELECTMENU_CPARTYMEHELPRHYTHMRECEIVE_HPP_ */
