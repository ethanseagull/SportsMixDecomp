#ifndef SELECTMENU_CCHARAMDL_HPP_
#define SELECTMENU_CCHARAMDL_HPP_

#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/g3d/ResMat.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {
    namespace SelectMenu {

        // The character model of the character select screen (menu object
        // 0x1C). Its base and size are not known (no allocation site), so do
        // not create one through this header. Only the fields the module
        // reads are declared; names are guesses.
        class CCharaMdl {
            public:
                // The texture the name plate model (the word at 0x64) is
                // drawn with.
                nw4r::g3d::ResTexObj GetResTexObj();
                // Places the models for nPlayerNum players (1 to 3).
                void SetPlayerDeploy(long nPlayerNum);
                // Plays the motion for state nState (1, 2, otherwise the
                // default) on the menu character of player nPlayer.
                void SetStateChrMenuObj(long nPlayer, long nState);

            private:
                unsigned char m_Unk00[0x5C];          // 0x00
                nw4r::g3d::ResFile m_ResFile[1];      // 0x5C
                bcs::CMdlObj *m_pMdl;                 // 0x60, the stage model
                bcs::CMdlObj *m_pNameMdl;             // 0x64, the name plate
                unsigned char m_Unk68[0x2C];          // 0x68
                unsigned long m_hChrMenuObj[3];       // 0x94, msw::CChrMenuCtrl handles per player
        };
    }
}

#endif /* !SELECTMENU_CCHARAMDL_HPP_ */
