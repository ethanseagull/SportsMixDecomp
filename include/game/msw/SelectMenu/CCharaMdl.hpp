#ifndef SELECTMENU_CCHARAMDL_HPP_
#define SELECTMENU_CCHARAMDL_HPP_

#include "nw4r/g3d/ResMat.hpp"

namespace msw {
    namespace SelectMenu {

        // The character model of the character select screen (menu object
        // 0x1C). Added for MenuF game/menu/unit_6EE84.cpp with only the two
        // members called there; its base and size are not known, so do not
        // create one through this header. Argument names are guesses.
        class CCharaMdl {
            public:
                // The texture the model is drawn into (the word at 0x64).
                nw4r::g3d::ResTexObj GetResTexObj();
                // Places the models for nPlayerNum players.
                void SetPlayerDeploy(long nPlayerNum);
        };
    }
}

#endif /* !SELECTMENU_CCHARAMDL_HPP_ */
