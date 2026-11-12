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
            // (DOL fn_800D2E68, name guessed after its neighbour; nMode is
            // not read) The node settings for the motions of that model.
            static const bcs::CAnmObjMotBase::NODE_EX_INFO *GetArtsOptionMdl_NodeExInfo(long nChr, long nMode);

            // (fields added for BasketF game/basket/CcWarioSmokeMgr.cpp;
            // names guessed) The names of the model, of its shadow and of
            // its reflection model in the option's resource file (null for
            // none), the node settings for its motions, and the kind of
            // shadow. Size 0x20 (DOL game/msw/CC_DATA.cpp: 0x40 for the kind
            // of shadow in every entry; 0x10, 0x18 and 0x1C are 0 or -1 and
            // small numbers, meaning not known).
            struct ARTS_OPT_CREATE_INFO {
                const char *m_pMdlName;      // 0x00
                const char *m_pShadowName;   // 0x04
                const char *m_pRefName;      // 0x08
                const bcs::CAnmObjMotBase::NODE_EX_INFO *m_pNodeExInfo; // 0x0C
                long m_Unk10;                // 0x10
                long m_nShadowType;          // 0x14
                long m_Unk18;                // 0x18
                long m_Unk1C;                // 0x1C
            };

            // (field names guessed) The characters' movement parameters.
            struct CHR_PARAM {
                // A move to a point over a number of frames.
                struct MOVE_PARAM {
                    long m_nType;            // 0x00
                    float m_fFrame;          // 0x04
                    float m_fFrameMax;       // 0x08
                    float m_fDist;           // 0x0C
                };
                // (added for BasketF game/basket/unit_2E514.cpp; contents not
                // mapped) The parameters of a jump, as
                // CChrCore::Util_SetJumpParam takes them.
                struct JUMP_PARAM;
            };
    };
}

#endif /* !MSW_CC_DATA_HPP_ */
