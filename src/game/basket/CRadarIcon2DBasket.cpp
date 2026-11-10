// msw::CRadarIcon2DBasket: the radar icons of the basket game. The same code
// as msw::CRadarIcon2DBoss (BossF).
#include "game/msw/CRadarIcon2DBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
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

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

msw::CRadarIcon2DBasket::CRadarIcon2DBasket(long nId)
    : CRadarIcon2D(nId)
{
    Initialize();
}

msw::CRadarIcon2DBasket::~CRadarIcon2DBasket()
{
    Finalize();
}

void msw::CRadarIcon2DBasket::Initialize()
{
    InitCharaIcons(m_pIconParam);
    InitBallIcons(m_pIconParam + VIEW_MAX * ICON_MAX);
}

void msw::CRadarIcon2DBasket::CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
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

void msw::CRadarIcon2DBasket::CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                           const char *pBaseName, const char *pPictureName)
{
    CBall *pBall = CBallMan::GetBallPtr(nBall);

    if (pBall == 0) {
        return;
    }

    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f); // never used, as in CreateCharaIcon

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

void msw::CRadarIcon2DBasket::Finalize()
{
}

long msw::CRadarIcon2DBasket::SendEvent(long nEvent, void *pParam)
{
    return CRadarIcon2D::SendEvent(nEvent, pParam);
}
