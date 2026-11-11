#ifndef SELECTMENU_CBGMDL_HPP_
#define SELECTMENU_CBGMDL_HPP_

#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/SelectMenu/CSelectMenuBgMdlObj.hpp"

namespace msw {
    namespace SelectMenu {

        // The background model of the SelectMenu screens; menu object 0x1D
        // of CSelectMenu. Only the members used so far: the direct base is
        // not known (CMenuLinkBase is at 0, since CSelectMenu's object list
        // holds it), its virtuals are not declared, so never create one
        // through this header.
        class CBgMdl : public CMenuLinkBase {
            public:
                // Shows the ball of game nBall (0 to 3).
                void ChangeBall(long nBall);
                // (meaning guessed) Moves ball model nId to the front
                // position, swapping it with the one there.
                void SetSelectPosMdlObjId(long nId);

            private:
                // From game/menu/CBgMdl.cpp (MenuF). The members from 0x1C to
                // 0x70 are not mapped, nor how many models there are.
                unsigned char m_Unk1C[0x54];          // 0x1C
                CSelectMenuBgMdlObj *m_pBallMdl[0xC]; // 0x70, indexed by the ids below
                unsigned char m_nPosMdlObjId[4];      // 0xA0, the model at each position; 0 is the front
                float m_fMoveTime;                    // 0xA4, frames of a move
        };
    }
}

#endif /* !SELECTMENU_CBGMDL_HPP_ */
