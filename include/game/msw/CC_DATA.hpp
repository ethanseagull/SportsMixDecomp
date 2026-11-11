#ifndef MSW_CC_DATA_HPP_
#define MSW_CC_DATA_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmObjMotBase.hpp"

namespace msw {

    // The characters' data tables. Only what the modules use so far.
    class CC_DATA {
        public:
            // The node settings for the motions of non-player model `nNpc`.
            static const bcs::CAnmObjMotBase::NODE_EX_INFO *GetNpcMdl_NodeExInfo(long nNpc);
            // The same for character nChr's model. (meaning of the second
            // argument not known; MenuF's CChrMenuCtrl passes 0)
            static const bcs::CAnmObjMotBase::NODE_EX_INFO *GetMdl_NodeExInfo(long nChr, long);
            // The colour of Toad's (Kinopio's) extra model for colour
            // variant nColor (BossF game/boss/CChrCoreBossKinopio.cpp).
            static bcs::BCS_Color GetChrOptColor_Kinopio(long nColor);
    };
}

#endif /* !MSW_CC_DATA_HPP_ */
