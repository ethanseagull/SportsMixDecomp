// msw::CCockpitMarkerIcon2D: a cockpit layout of one marker icon (goal and
// radar icons, the charge guide, the boss arrow), picked by m_nType.
//
// Only a slice of the original file: the same file holds CRadarIcon2D
// before and the icon task classes after this class (see the thunks at
// 0x801A27E0..0x801A27F8 and the float pool shared with
// CRadarIcon2D::InitPicPane).
#include "game/msw/CCockpitMarkerIcon2D.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace {
    // (names guessed) The layout of one marker type: resource file, number
    // of animations, layout, animations and the two panes the accessor
    // gets. The animation played on Set is 0; 4, when there is one, plays
    // on the first pane.
    struct MarkerIconData {
        unsigned char nResFile;     // 0x00: for CCockpit::GetResourceFileName
        unsigned char nAnimNum;     // 0x01
        const char *pLayoutName;    // 0x04
        const char *pAnimNames[5];  // 0x08
        const char *pPaneNames[2];  // 0x1C
    };

    const MarkerIconData s_MarkerIconData[] = {
        {
            1, 4, "CO_goal_ico.brlyt",
            { "CO_goal_ico_G_ng_00.brlan", "CO_goal_ico_G_ng_01.brlan", "CO_goal_ico_G_ng_02.brlan",
              "CO_goal_ico_G_ng_03.brlan", 0 },
            { "g_ng_Null", "g_ng_Null" },
        },
        {
            1, 5, "CO_goal_ico.brlyt",
            { "CO_goal_ico_G_ok_00.brlan", "CO_goal_ico_G_ok_01.brlan", "CO_goal_ico_G_ok_02.brlan",
              "CO_goal_ico_G_ok_03.brlan", "CO_goal_ico_C_00.brlan" },
            { "cont_g", "g_ok_Null" },
        },
        {
            1, 5, "CO_goal_ico.brlyt",
            { "CO_goal_ico_G_ok_00.brlan", "CO_goal_ico_G_ok_01.brlan", "CO_goal_ico_G_ok_02.brlan",
              "CO_goal_ico_G_ok_03.brlan", "CO_goal_ico_C_01.brlan" },
            { "cont_g", "g_ok_Null" },
        },
        {
            1, 4, "CO_radar_ico.brlyt",
            { "CO_radar_ico_G_ok_00.brlan", "CO_radar_ico_G_ok_01.brlan", "CO_radar_ico_G_ok_02.brlan",
              "CO_radar_ico_G_ok_03.brlan", 0 },
            { "cont_g", "g_ok_Null" },
        },
        {
            1, 4, "charge_guide.brlyt",
            { "charge_guide_all_ctl_03.brlan", "charge_guide_all_ctl_00.brlan", "charge_guide_all_ctl_01.brlan",
              "charge_guide_all_ctl_02.brlan", 0 },
            { "all_ctl", "all_ctl" },
        },
        {
            2, 4, "BOSS_cockpit_yaji.brlyt",
            { "BOSS_cockpit_yaji_Ya_01.brlan", "BOSS_cockpit_yaji_Ya_00.brlan", "BOSS_cockpit_yaji_Ya_00.brlan",
              "BOSS_cockpit_yaji_Ya_01.brlan", 0 },
            { "y_Null", "y_Null" },
        },
    };

    // The charge guide shows button text, so it takes the two fonts.
    const long MARKER_TYPE_CHARGE_GUIDE = 4;
    const long ANIM_START = 0;
    const long ANIM_EXTRA = 4;
}

msw::CCockpitMarkerIcon2D::CCockpitMarkerIcon2D(long nId, float fSortZ, long nType, long nLayer)
    : CCockpitLayoutBase(nId, fSortZ)
{
    m_nLayer = nLayer;
    m_Unk68 = 0;
    m_Unk6C = 0;
    m_nType = nType;
}

msw::CCockpitMarkerIcon2D::~CCockpitMarkerIcon2D()
{
    Remove();
}

void msw::CCockpitMarkerIcon2D::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);
}

void msw::CCockpitMarkerIcon2D::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, m_nLayer);
}

int msw::CCockpitMarkerIcon2D::Set()
{
    long nType;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    nType = m_nType;
    const MarkerIconData &rData = s_MarkerIconData[nType];
    const char *pFileName = CCockpit::GetResourceFileName(rData.nResFile);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        long nLayer;

        // default first: the original lays its block out right after the
        // compare chain, which this order reproduces (notes/toolchain.md).
        switch (m_nLayer) {
            default:
                nLayer = 1;
                break;
            case 1:
                nLayer = 2;
                break;
            case 2:
                nLayer = 3;
                break;
            case 3:
                nLayer = 4;
                break;
        }

        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, nLayer, 0);

        if (pPaneCtrl != 0) {
            if (nType == MARKER_TYPE_CHARGE_GUIDE) {
                pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
                pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            }
            pPaneCtrl->BuildLayout(0, rData.pLayoutName);
            pPaneCtrl->BuildAnim(0, const_cast<const char **>(rData.pAnimNames), rData.nAnimNum);
            SetPaneAccessor(pPaneCtrl, const_cast<const char **>(rData.pPaneNames), 2);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->PlayAnim(ANIM_START, 1.0f, 0.0f, 1, 1);

            bcs::G2D::CPaneObj *pPaneObj = m_pPaneAccessor->GetPaneObj(0L);
            if (pPaneObj != 0) {
                pPaneObj->GetPane()->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
                if (rData.nAnimNum > ANIM_EXTRA) {
                    pPaneObj->PlayAnimation(ANIM_EXTRA, 1.0f, 0.0f, 1, 0, 1);
                }
            }
            m_nStep = 1;
            m_nState = 1;
        }
    }
    return 0;
}

void msw::CCockpitMarkerIcon2D::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

long msw::CCockpitMarkerIcon2D::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == 10001) {
        int bVisible = (int)pParam;
        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return -1;
}
