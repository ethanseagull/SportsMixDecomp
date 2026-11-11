#ifndef BCS_G2D_CMENUMDLSCNGROUP_HPP_
#define BCS_G2D_CMENUMDLSCNGROUP_HPP_

#include "bcs/CMainApp.hpp"
#include "math.h"
#include "nw4r/math/triangular.h"

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Added for MenuF game/menu/unit_6EE84.cpp with
        // only the static projection settings of the menu model scenes;
        // the class itself is not written yet.
        class CMenuMdlScnGroup {
            public:
                // Size 0x38 (the DOL's s_PersProjInfo). Names guessed from
                // what SetDefaultPersProjInfo stores.
                struct PersProjInfo {
                    float m_fNear;       // 0x00
                    float m_fFar;        // 0x04
                    float m_fWidth;      // 0x08
                    float m_fHeight;     // 0x0C
                    float m_fFovy;       // 0x10, degrees
                    // 0x14 to 0x1C and 0x20 to 0x28: two points, each on
                    // the axis at the distance where the height fills the
                    // view; 0x2C to 0x34 not known.
                    float m_fDistance;   // 0x14
                    float m_Unk18;       // 0x18
                    float m_Unk1C;       // 0x1C
                    float m_fDistance2;  // 0x20
                    float m_Unk24;       // 0x24
                    float m_Unk28;       // 0x28
                    float m_Unk2C;       // 0x2C
                    float m_Unk30;       // 0x30
                    float m_Unk34;       // 0x34
                };

                static PersProjInfo *GetPersProjInfo();

                // (name guessed; inline, no symbol) Puts the default
                // projection back: a 30 degree view of the whole frame
                // buffer. The same code is in CSelectMenu::CharSel_Out and
                // in game/menu/CGamePadCheck.cpp (MenuF).
                static void SetDefaultPersProjInfo()
                {
                    PersProjInfo &rInfo = s_PersProjInfo;
                    float fFovy = 30.0f;
                    rInfo.m_fNear = 1.0f;
                    rInfo.m_fWidth = (long)CMainApp::g_Rmode.fbWidth;
                    rInfo.m_fHeight = (long)CMainApp::g_Rmode.efbHeight;
                    rInfo.m_fFar = 10000.0f;
                    rInfo.m_fFovy = fFovy;
                    float fDistance = (0.5f * rInfo.m_fHeight) / (float)tan(NW4R_MATH_DEG_TO_RAD(0.5f * fFovy));
                    rInfo.m_Unk18 = 0.0f;
                    rInfo.m_Unk1C = 0.0f;
                    rInfo.m_Unk24 = 0.0f;
                    rInfo.m_Unk28 = 0.0f;
                    rInfo.m_Unk2C = 0.0f;
                    rInfo.m_Unk30 = 0.0f;
                    rInfo.m_fDistance = fDistance;
                    rInfo.m_fDistance2 = fDistance;
                }

                static PersProjInfo s_PersProjInfo;
        };
    }
}

#endif /* !BCS_G2D_CMENUMDLSCNGROUP_HPP_ */
