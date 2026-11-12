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
    };
}

#endif /* !BCS_CWORLDENV_HPP_ */
