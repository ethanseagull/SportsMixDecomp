// msw::CRadarIcon2DVolley: the radar icons of the volley game (written from
// game/boss/CRadarIcon2DBoss.cpp; character icons only), and
// msw::CCockpitVolleyMarkerIcon2D: the charge guide shown by a player.
#include "game/msw/CRadarIcon2DVolley.hpp"
#include "game/msw/CCockpitVolleyMarkerIcon2D.hpp"

#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CMessage.hpp"
#include <wchar.h>

// Names of the layout's panes. (the grouping into members is a guess: the
// original is one unnamed object of six pointers)
static struct {
    const char *pCenter;       // parent of every icon
    const char *pBallBase;     // frame of a ball icon (not used in volley)
    const char *pCharaBase[4]; // frame of a character icon, by team colour
} s_PaneName = {
    "Center00",
    "BaseIcon_Ball",
    { "BaseIcon_2P", "BaseIcon_1P", "BaseIcon_4P", "BaseIcon_3P" },
};

msw::CRadarIcon2DVolley::CRadarIcon2DVolley(long nId)
    : CRadarIcon2D(nId)
{
    Initialize();
}

msw::CRadarIcon2DVolley::~CRadarIcon2DVolley()
{
    Finalize();
}

void msw::CRadarIcon2DVolley::Initialize()
{
    // Not used; the original has both objects. (static-order swap: the
    // original puts the "@0" suffix on viewMaxNum.)
    static const long viewMaxNum = 4;
    static const long chrMaxNum = 8;

    InitCharaIcons(m_pIconParam);
}

void msw::CRadarIcon2DVolley::CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                            const char *pBaseName, const char *pPictureName)
{
    CChrCore *pChrCore = CChrMan::GetChrCore(nChr);

    if (pChrCore == 0) {
        return;
    }

    // Never used: the original stores the three zeros to the stack all the
    // same (three separate slots), and every CRadarIcon2D<Game> has it.
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    bcs::G2D::CPaneObj *pCenterObj = pPaneCtrl->GetChildPaneObj(s_PaneName.pCenter);

    nw4r::lyt::Pane *pNullPane = bcs::G2D::CreateNullPane(pNullName);

    if (pNullPane == 0) {
        return;
    }

    bcs::G2D::CPaneObj *pNullObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pNullPane);

    if (pNullObj == 0) {
        return;
    }
    pPaneCtrl->AppendPaneObj(pNullObj, *pCenterObj);
    InitNullPane(pNullPane);

    long nColor = CGame::GetTeamColorIdx(pChrCore->GetTeamNo());
    bcs::G2D::CPaneObj *pBaseObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_PaneName.pCharaBase[nColor], pBaseName,
                                                              GetLayoutBinaryFileName(), 0);

    if (pBaseObj == 0) {
        return;
    }
    pPaneCtrl->AppendPaneObj(pBaseObj, *pNullObj);
    InitPicPane(pBaseObj->GetPane());

    bcs::G2D::CPaneObj *pPictureObj = 0;
    const _GXTexObj *pTexObj = pChrCore->GetRadarIconTexObj();

    if (pTexObj != 0) {
        bcs::G2D::CTexMap texMap(*pTexObj);
        nw4r::lyt::Pane *pPicturePane = bcs::G2D::CreatePicturePane(pPictureName, texMap);

        if (pPicturePane != 0) {
            if (CGame::GetCharType(pChrCore->GetPlayerNo()) >= 0x13) {
                pPicturePane->SetSize(m_MiiIconSize);
            }
            InitPicPane(pPicturePane);
            pPictureObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pPicturePane);
        }
    }

    if (pPictureObj != 0) {
        pPaneCtrl->AppendPaneObj(pPictureObj, *pNullObj);
        pParam->m_pTexObj = pTexObj;
    }

    pParam->SetPaneObj(ICON_PARAMETER::PANE_NULL, pNullObj);
    pParam->SetPaneObj(ICON_PARAMETER::PANE_BASE, pBaseObj);
    pParam->SetPaneObj(ICON_PARAMETER::PANE_PICTURE, pPictureObj);
}

// The volley radar has no ball icons.
void msw::CRadarIcon2DVolley::CreateBallIcon(ICON_PARAMETER *, long, const char *, const char *, const char *)
{
}

void msw::CRadarIcon2DVolley::Finalize()
{
}

long msw::CRadarIcon2DVolley::SendEvent(long nEvent, void *pParam)
{
    return CRadarIcon2D::SendEvent(nEvent, pParam);
}

namespace {
    // Panes of the charge guide layout (names guessed from s_Layout).
    enum {
        PANE_ALL = 0,
        PANE_GUIDE = 2,
        PANE_SIDE_ON = 3,
        PANE_SIDE_OFF = 4,
        PANE_PARTS_ON = 5,
        PANE_PARTS_OFF = 6,
        PANE_TEXT_0 = 7,
        PANE_TEXT_1 = 8,
        PANE_NUM = 9
    };

    const long ANIM_NUM = 4;

    // One marker type: its resource file, layout, animations and panes.
    struct MARKER_LAYOUT {
        long nResFile;
        const char *pLayoutName;
        const char *pAnimName[ANIM_NUM];
        const char *pPaneName[PANE_NUM];
    };
}

// By m_nType: the guide on one side of the player, and the mirrored one.
static const MARKER_LAYOUT s_Layout[] = {
    {
        1,
        "charge_guide.brlyt",
        { "charge_guide_all_ctl_03.brlan", "charge_guide_all_ctl_00.brlan", "charge_guide_all_ctl_01.brlan",
          "charge_guide_all_ctl_02.brlan" },
        { "all_ctl", "all_ctl", "center", "left", "right", "right_parts", "left_parts", "t_sspke_01",
          "t_sspk_00" },
    },
    {
        1,
        "charge_guide.brlyt",
        { "charge_guide_all_ctl_03.brlan", "charge_guide_all_ctl_00.brlan", "charge_guide_all_ctl_01.brlan",
          "charge_guide_all_ctl_02.brlan" },
        { "all_ctl", "all_ctl", "center", "right", "left", "left_parts", "right_parts", "t_sspke_01",
          "t_sspk_00" },
    },
};

// Message file of the guide texts by m_nType, and the two labels.
static const char *s_pGuideFile[] = { "Text_Operation_Guide", "Text_Operation_Guide" };
static const char *s_pGuideLabel[] = { "GUIDE_021_01", "GUIDE_021_02" };

msw::CCockpitVolleyMarkerIcon2D::CCockpitVolleyMarkerIcon2D(long nId, float fSortZ, long nType, long nLayer)
    : CCockpitMarkerIcon2D(nId, fSortZ, nType, nLayer)
{
}

msw::CCockpitVolleyMarkerIcon2D::~CCockpitVolleyMarkerIcon2D()
{
    Remove();
}

int msw::CCockpitVolleyMarkerIcon2D::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    unsigned long nType = m_nType;
    const MARKER_LAYOUT *pLayout = &s_Layout[nType];
    const char *pFileName = CCockpit::GetResourceFileName(pLayout->nResFile);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        long nLayer;

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
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, pLayout->pLayoutName);
            // (the table is const; BuildAnim and SetPaneAccessor take plain pointers)
            pPaneCtrl->BuildAnim(0, const_cast<const char **>(pLayout->pAnimName), ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, const_cast<const char **>(pLayout->pPaneName), PANE_NUM);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->PlayAnim(0, 1.0f, 0.0f, 1, 1);

            nw4r::lyt::Pane *pAll = m_pPaneAccessor->GetPane(PANE_ALL);

            if (pAll != 0) {
                pAll->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
            }

            m_pPaneAccessor->SetPaneDisp(PANE_SIDE_ON, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_SIDE_OFF, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_PARTS_ON, 1);
            m_pPaneAccessor->SetPaneDisp(PANE_PARTS_OFF, 0);

            nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_SIDE_ON);
            nw4r::lyt::Pane *pGuide = m_pPaneAccessor->GetPane(PANE_GUIDE);

            if (pGuide != 0 && pParent != 0) {
                bcs::G2D::SetPaneParent(pGuide, pParent, 1, 0);
            }

            if (nType <= 1) {
                menu::CMessage &rMessage = menu::CMessage::s_Instance;
                const wchar_t *pText;

                pText = rMessage.GetTextByLabel(rMessage.GetFileIndex(s_pGuideFile[nType]), s_pGuideLabel[1]);
                m_pPaneAccessor->SetTextBoxString(PANE_TEXT_0, pText, wcslen(pText));
                pText = rMessage.GetTextByLabel(rMessage.GetFileIndex(s_pGuideFile[nType]), s_pGuideLabel[0]);
                m_pPaneAccessor->SetTextBoxString(PANE_TEXT_1, pText, wcslen(pText));
            }

            m_nStep = 1;
            m_nState = 1;
        }
    }
    return 0;
}
