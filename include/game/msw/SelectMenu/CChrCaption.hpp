#ifndef SELECTMENU_CCHRCAPTION_HPP_
#define SELECTMENU_CCHRCAPTION_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "game/msw/menu/CDispTaskList.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {
    namespace SelectMenu {

        // Size 0x6C. The captions over the characters on the team and play
        // mode screens ("me_p"): each is a pane of the layout kept over a
        // scene object by a task in the task list at 0x5C. CPlayMode and
        // CTeam call SetCaption. The game's file is SelectMenuChrCap.cpp
        // (from the names of its local statics); it started as a copy of
        // SelectMenuChrSign.cpp (CChrSign), whose SetSign and sign task it
        // still has.
        class CChrCaption : public CSelectMenuLayoutBase {
            public:
                CChrCaption(long nId);
                virtual ~CChrCaption();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                // Shows the next pane of caption kind nCaption over scene
                // object hScnObj (the model of character nChr), moved by
                // *pPos (when not null) in the world and by nChr's screen
                // offset. nCamera is not used (CTeam passes a camera number,
                // CPlayMode 0). Returns 0. (argument names guessed)
                long SetCaption(long nChr, long nCaption, unsigned long hScnObj, long nCamera, const bcs::FVEC3 *pPos);

                // Not in the game's module: nothing calls them and the link
                // dropped them; only their local statics and float constant
                // are left (see the .cpp).
                long SetSign(long nPlayer, long nType, const bcs::FVEC3 *pPos, long);
                long ClearSign(long nPlayer);

            private:
                menu::CDispTaskList m_DispTaskList; // 0x5C
        };
    }
}

#endif /* !SELECTMENU_CCHRCAPTION_HPP_ */
