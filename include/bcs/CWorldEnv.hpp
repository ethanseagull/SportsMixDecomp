#ifndef BCS_CWORLDENV_HPP_
#define BCS_CWORLDENV_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/math/types.h"

namespace nw4r {
    namespace g3d {
        struct CameraData;
    }
}

namespace bcs {

    // Only the members used so far. Both functions are static: their bodies
    // in the DOL take the view number in r3.
    class CWorldEnv {
        public:
            // How the screen is split into views. Named in the mangled names
            // of bcs::CRenderMan::ReqPushViewSplitType and
            // ReqPopViewSplitType; only 0 (taken to be one full-screen view)
            // is passed so far. (enumerator names guessed)
            enum ViewSplitType {
                VIEW_SPLIT_1
            };

            // (name guessed) The camera data of a view; the offsets match
            // nw4r::g3d::CameraData. msw::CStgGmk_WaterPillar_Act::OnUpdate
            // in the DOL fades the pillar's effects by the distance to the
            // eye; bcs::EF::CManager reads the matrices and the projection.
            struct CameraData {
                nw4r::math::MTX34 m_ViewMtx;   // 0x00 (name guessed)
                float m_ProjMtx[4][4];         // 0x30, passed to GXSetProjection (name guessed)
                unsigned char m_Unk70[0x74 - 0x70];
                FVEC3 m_Eye; // 0x74 (name guessed)
                unsigned char m_Unk80[0xAC - 0x80];
                float m_fFovy;                 // 0xAC (name guessed)
                float m_fAspect;               // 0xB0 (name guessed)
            };
            static CameraData *GetViewCameraData(long view);

            static void SetViewFovy(long view, float fovy);
            // (added for VolleyF CStageVolley42) Sets the projection camera
            // of a view.
            static void SetProjCameraData(long view, const nw4r::g3d::CameraData &rData);
            static float GetViewFovy(long view);
            // How many views the screen is split into. Return type guessed.
            static long GetViewSplitNum();

            // (fn_8007DFF8, name guessed) Sets m_vstViewSplitType and lays
            // the views out again; called by bcs::CRenderMan.
            static void SetViewSplitType(ViewSplitType type);
            // (fn_8007E000, name guessed) Sets the aspect ratio, viewport
            // and scissor box of every view from m_vatViewAspectType,
            // m_vstViewSplitType and g3d's render mode.
            static void UpdateViewSplit();
            // (fn_8007E690, name guessed) The view matrix of a view: copied
            // when the camera data carries one (flag 8), else computed by
            // nw4r::g3d::Camera.
            static void GetViewCameraMtx(long view, nw4r::math::MTX34 *pMtx);

            // (name guessed) What a view keeps of its projection and screen
            // area, 0x30 bytes; copied into the view's camera data by
            // SetViewCameraData and UpdateViewSplit. Field names follow the
            // CameraData members they are copied to.
            struct ViewProjInfo {
                float projFovy;              // 0x00
                float projAspect;            // 0x04
                float projNear;              // 0x08
                float projFar;               // 0x0C
                float viewportX;             // 0x10
                float viewportY;             // 0x14
                float viewportWidth;         // 0x18
                float viewportHeight;        // 0x1C
                unsigned long scissorX;      // 0x20
                unsigned long scissorY;      // 0x24
                unsigned long scissorWidth;  // 0x28
                unsigned long scissorHeight; // 0x2C
            };
            // (fn_8007E710, name guessed) Read for the aspect ratio by the
            // DOL's fn_80024A88 region.
            static ViewProjInfo *GetViewProjInfo(long view);
            // (fn_8007E7CC, name guessed) Sets the near and far planes of a
            // view (made positive), as SetViewFovy does the fovy.
            static void SetViewNearFar(long view, float fNear, float fFar);
            // (fn_8007E888, name guessed) Copies rData into the view's
            // camera data, keeping the view's own projection and screen
            // area, and clears flags 0x80 and 0x08.
            static void SetViewCameraData(long view, const nw4r::g3d::CameraData &rData);
            // (added for MenuF game/menu/CTopMenu.cpp) Sets the 3D
            // background colour of every view.
            static void Set3DBackColorAll(BCS_Color color);

            // 1 selects the second set of camera data in the tournament
            // (msw::CTournamentCamera::GetDataMode). Type guessed.
            static long m_vatViewAspectType;
            // (added for HockeyF CChrManHockey::SetCamIdAssign_) How the
            // screen is split: 1 and 2 two views, 3 four views (guessed from
            // the camera tables chosen by it). Type guessed.
            static long m_vstViewSplitType;

            // (names guessed; the DOL's lbl_804D0210, lbl_804D1130,
            // lbl_804D1140) The camera data and projection of each view:
            // four views, then the full screen (index 4, used for view -1
            // and any other out-of-range number). Defined with the rest of
            // this file, which reaches past the unit's current .text range.
            static nw4r::g3d::CameraData m_ViewCameraData[5];
            static BCS_Color m_3DBackColor[4];
            static ViewProjInfo m_ViewProjInfo[5];
    };
}

#endif /* !BCS_CWORLDENV_HPP_ */
