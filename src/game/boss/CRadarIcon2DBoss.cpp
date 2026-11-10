// msw::CRadarIcon2DBoss: the radar icons of the boss game.
#include "game/msw/CRadarIcon2DBoss.hpp"

#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
// PENDING: these two stand in for game/msw/CChrCore.hpp and
// game/msw/CGame.hpp until what they add is merged (see the files).
#include "game/msw/CChrCore.hpp"
#include "game/msw/CGame.hpp"

// Names of the layout's panes. (the grouping into members is a guess: the
// original is one unnamed object of ten pointers)
static struct {
    const char *pCenter;       // parent of every icon
    const char *pBallBase;     // frame of a ball icon
    const char *pCharaBase[4]; // frame of a character icon, by team colour
    const char *pBall[4];      // not used by this class
} s_PaneName = {
    "Center00",
    "BaseIcon_Ball",
    { "BaseIcon_2P", "BaseIcon_1P", "BaseIcon_4P", "BaseIcon_3P" },
    { "BA_Ball00", "BA_Ball00", "BA_Ball00", "BA_Ball00" },
};

msw::CRadarIcon2DBoss::CRadarIcon2DBoss(long nId)
    : CRadarIcon2D(nId)
{
    Initialize();
}

msw::CRadarIcon2DBoss::~CRadarIcon2DBoss()
{
    Finalize();
}

void msw::CRadarIcon2DBoss::Initialize()
{
    InitCharaIcons(m_pIconParam);
    InitBallIcons(m_pIconParam + VIEW_MAX * ICON_MAX);
}

void msw::CRadarIcon2DBoss::CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                            const char *pBaseName, const char *pPictureName)
{
    CChrCore *pChrCore = CChrMan::GetChrCore(nChr);

    if (pChrCore == 0) {
        return;
    }

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

void msw::CRadarIcon2DBoss::CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                           const char *pBaseName, const char *pPictureName)
{
    CBall *pBall = CBallMan::GetBallPtr(nBall);

    if (pBall == 0) {
        return;
    }

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

    bcs::G2D::CPaneObj *pBaseObj =
        bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_PaneName.pBallBase, pBaseName, GetLayoutBinaryFileName(), 0);

    if (pBaseObj == 0) {
        return;
    }
    pPaneCtrl->AppendPaneObj(pBaseObj, *pNullObj);
    InitPicPane(pBaseObj->GetPane());

    bcs::G2D::CPaneObj *pPictureObj = 0;
    // CBall.hpp gives the texture as a void pointer.
    const _GXTexObj *pTexObj = static_cast<const _GXTexObj *>(pBall->GetRadarIconTexObj());

    if (pTexObj != 0) {
        bcs::G2D::CTexMap texMap(*pTexObj);
        nw4r::lyt::Pane *pPicturePane = bcs::G2D::CreatePicturePane(pPictureName, texMap);

        if (pPicturePane != 0) {
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

void msw::CRadarIcon2DBoss::Finalize()
{
}

long msw::CRadarIcon2DBoss::SendEvent(long nEvent, void *pParam)
{
    return CRadarIcon2D::SendEvent(nEvent, pParam);
}
