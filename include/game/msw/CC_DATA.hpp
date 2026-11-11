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

            // (added for VolleyF game/volley/CChrCoreVolleyKinopio.cpp;
            // contents not known) How an extra model of a special move is
            // made, for character nChr in game mode nMode.
            struct ARTS_OPT_CREATE_INFO;
            static const ARTS_OPT_CREATE_INFO *GetArtsOptionMdl_CreateInfo(long nChr, long nMode);

            // (field names guessed) The characters' movement parameters.
            struct CHR_PARAM {
                // A move to a point over a number of frames.
                struct MOVE_PARAM {
                    long m_nType;            // 0x00
                    float m_fFrame;          // 0x04
                    float m_fFrameMax;       // 0x08
                    float m_fDist;           // 0x0C
                };
            };
    };
}

#endif /* !MSW_CC_DATA_HPP_ */
