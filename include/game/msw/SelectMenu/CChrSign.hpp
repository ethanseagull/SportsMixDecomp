#ifndef SELECTMENU_CCHRSIGN_HPP_
#define SELECTMENU_CCHRSIGN_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "game/msw/menu/CDispTaskList.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {
    namespace SelectMenu {

        // Size 0x6C. The signs over the characters on the character select
        // screen ("me_cha_p"): each is a pane of the layout played and moved
        // to a world position's place on screen by a task in the task list
        // at 0x5C. CCharSel calls SetSign and ClearSign. The game's file is
        // SelectMenuChrSign.cpp (from the names of its local statics).
        class CChrSign : public CSelectMenuLayoutBase {
            public:
                CChrSign(long nId);
                virtual ~CChrSign();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                // Shows sign nType for player nPlayer over the world point
                // *pPos. The last argument is not used. -1 before Set, else
                // 0. (argument names guessed)
                long SetSign(long nPlayer, long nType, const bcs::FVEC3 *pPos, long);
                // Plays the closing animation of every sign of player
                // nPlayer; the tasks end with it. Returns 0.
                long ClearSign(long nPlayer);

            private:
                menu::CDispTaskList m_DispTaskList; // 0x5C
        };
    }
}

#endif /* !SELECTMENU_CCHRSIGN_HPP_ */
