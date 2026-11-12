#ifndef MSW_CC_DATA_HPP_
#define MSW_CC_DATA_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/FVEC2.hpp"

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

            // (DOL fn_800D2C38; name and contents guessed) Picked by
            // CHR_PARAM::m_nSubParam. All three entries are the same. Size
            // 0x38.
            struct CHR_SUB_PARAM {
                bcs::FVEC2 m_Unk00[5];       // 0x00
                float m_Unk28[4];            // 0x28
            };

            // (field names guessed) The characters' movement parameters.
            // Size 0x54.
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

                long m_Unk00;                // 0x00 (0 in every entry)
                long m_Unk04;                // 0x04 (0 to 4)
                long m_nName;                // 0x08 'c00'..'c18', 'm00', 'm01'
                float m_Unk0C;               // 0x0C (16.0)
                float m_Unk10;               // 0x10 (3.0)
                float m_Unk14;               // 0x14
                float m_Unk18;               // 0x18
                float m_Unk1C;               // 0x1C (0.6)
                float m_Unk20;               // 0x20
                float m_Unk24;               // 0x24 (2.0)
                long m_nSubParam;            // 0x28 index for GetChrSubParam
                long m_nMoveSpeed;           // 0x2C index for GetMoveSpeed
                float m_Unk30;               // 0x30 (1.0, 1.2 or 1.5)
                float m_Unk34[4];            // 0x34 angles in radians
                long m_Unk44[4];             // 0x44
            };

            // (DOL fn_800D2B54, name guessed) The parameters of character
            // nChr; the first entry for an unknown one.
            static const CHR_PARAM *GetChrParam(long nChr);
            // (DOL fn_800D2C24, name guessed; CChrCore::GetPrmMoveSpeed
            // passes CHR_PARAM::m_nMoveSpeed) A movement speed.
            static float GetMoveSpeed(long nIdx);
            // (DOL fn_800D2C38, name guessed) Entry nIdx of the table above.
            static const CHR_SUB_PARAM *GetChrSubParam(long nIdx);

            // (DOL fn_800D2F98, name guessed; the argument is not read) A
            // fixed colour, 0x28282800.
            static bcs::BCS_Color GetChrOptColor_Default(long);
            // (DOL fn_800D2FA8, name and meaning guessed: the values look
            // like sound ids) Entry nIdx of character nChr's table for
            // nKind 0, of a common table for nKind 1; -1 for none. nMode and
            // the last argument are not read.
            static long GetChrSeId(long nMode, long nChr, long nKind, long nIdx, long);
            // (DOL fn_800D3008, name guessed) Entry nIdx of a table for
            // nKind 0, of game mode nMode's table for nKind 1; -1 for none.
            // nChr and the last argument are not read.
            static long GetChrSeIndex(long nMode, long nChr, long nKind, long nIdx, long);
    };
}

#endif /* !MSW_CC_DATA_HPP_ */
