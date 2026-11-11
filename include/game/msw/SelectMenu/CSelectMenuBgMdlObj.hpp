#ifndef SELECTMENU_CSELECTMENUBGMDLOBJ_HPP_
#define SELECTMENU_CSELECTMENUBGMDLOBJ_HPP_

#include "bcs/CAnmMotion.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {
    namespace SelectMenu {

        // One ball of the SelectMenu background (MenuF), held by
        // msw::SelectMenu::CBgMdl. The class is named by the static local
        // Main()::rotYSpeedDiv in the .rodata of game/menu/CBgMdl.cpp; its
        // functions and vtable are not in the module (the original link
        // stripped them), so only the members CBgMdl uses are declared, with
        // guessed names. Never create one through this header.
        class CSelectMenuBgMdlObj : public bcs::CMdlObj {
            public:
                // (name guessed) Starts a move of fFrames frames to the
                // position nState names: plays the motion of the ball model
                // so that it lasts the move. Inline, defined in
                // game/menu/CBgMdl.cpp (it reads that file's node names).
                inline void StartMove(float fFrames, long nState, unsigned char nFlag);

            private:
                float m_fMoveTime;            // 0x1C8
                float m_fMoveTimer;           // 0x1CC
                bcs::FVEC3 m_vStartPos;       // 0x1D0
                unsigned char m_Unk1DC[0x0C]; // 0x1DC
                long m_nState;                // 0x1E8
                unsigned long m_nBallHandle;  // 0x1EC, handle of the ball model
                unsigned char m_Unk1F0[0x10]; // 0x1F0
                unsigned char m_nFlag200;     // 0x200
        };
    }
}

#endif /* !SELECTMENU_CSELECTMENUBGMDLOBJ_HPP_ */
