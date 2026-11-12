// msw::CRadarIcon2DHockey: the radar icons of the hockey game (the same
// code as msw::CRadarIcon2DBasket, plus the goalkeeper check and the puck's
// icon in its holder's colour); msw::CRadarIcon2DHockeyGoal: the radar icons
// of the goals; msw::CBallMarker2DHockey: the marker over the puck.
#include "game/msw/CRadarIcon2DHockey.hpp"
#include "game/msw/CRadarIcon2DHockeyGoal.hpp"
#include "game/msw/CBallMarker2DHockey.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CCameraMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CLytResource.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "nw4r/lyt/layout.h"
#include "nw4r/math/triangular.h"

namespace {
    const float PI = 3.14159265f;

    // Indices into s_PaneName.
    enum {
        PANE_CENTER = 0,     // parent of every icon
        PANE_BALL_BASE = 1,  // frame of a ball icon
        PANE_CHARA_BASE = 2, // four frames of a character icon, by team colour
        PANE_BALL = 6        // four ball pictures (not used by this file)
    };
}

// Names of the radar layout's panes.
static const char *s_PaneName[] = {
    "Center00",
    "BaseIcon_Ball",
    "BaseIcon_2P", "BaseIcon_1P", "BaseIcon_4P", "BaseIcon_3P",
    "HO_Ball00", "HO_Ball00", "HO_Ball00", "HO_Ball00",
};

msw::CRadarIcon2DHockey::CRadarIcon2DHockey(long nId)
    : CRadarIcon2D(nId)
{
    Initialize();
}

msw::CRadarIcon2DHockey::~CRadarIcon2DHockey()
{
    Finalize();
}

void msw::CRadarIcon2DHockey::Initialize()
{
    // Not used; the original has both objects. (static-order swap: the
    // original puts the "@0" suffix on viewMaxNum.)
    static const long viewMaxNum = 4;
    static const long chrMaxNum = 8;

    InitCharaIcons(m_pIconParam);
    InitBallIcons(m_pIconParam + VIEW_MAX * ICON_MAX);
    m_Unk70 = 1;
    GetLytObj()->SetSortZ(-2.0f);
}

void msw::CRadarIcon2DHockey::CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                            const char *pBaseName, const char *pPictureName)
{
    CChrCore *pChrCore = CChrMan::GetChrCore(nChr);

    if (pChrCore == 0) {
        return;
    }
    // The goalkeepers have no radar icon.
    if (CGame::IsCharEx_HockeyKeeper(nChr)) {
        return;
    }

    // Never used: the original stores the three zeros to the stack all the
    // same (three separate slots), and every CRadarIcon2D<Game> has it.
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    bcs::G2D::CPaneObj *pCenterObj = pPaneCtrl->GetChildPaneObj(s_PaneName[PANE_CENTER]);

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
    bcs::G2D::CPaneObj *pBaseObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_PaneName[PANE_CHARA_BASE + nColor],
                                                              pBaseName, GetLayoutBinaryFileName(), 0);

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

void msw::CRadarIcon2DHockey::CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
                                           const char *pBaseName, const char *pPictureName)
{
    CBall *pBall = CBallMan::GetBallPtr(nBall);

    if (pBall == 0) {
        return;
    }

    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f); // never used, as in CreateCharaIcon

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    bcs::G2D::CPaneObj *pCenterObj = pPaneCtrl->GetChildPaneObj(s_PaneName[PANE_CENTER]);

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
        bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_PaneName[PANE_BALL_BASE], pBaseName, GetLayoutBinaryFileName(), 0);

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

void msw::CRadarIcon2DHockey::Finalize()
{
}

long msw::CRadarIcon2DHockey::SendEvent(long nEvent, void *pParam)
{
    return CRadarIcon2D::SendEvent(nEvent, pParam);
}

void msw::CRadarIcon2DHockey::DispBallIcon(long nView, const bcs::FVEC3 &rCamAt, float fX, float fY)
{
    // The height the puck is shown at, and the number of icons.
    static const float CHR_Y = 0.75f;
    static const long ballMaxNum = 8;

    ICON_PARAMETER *pParam = m_pBallIcon[nView];

    for (long nBall = 0; nBall < ballMaxNum; nBall++, pParam++) {
        if (pParam == 0) {
            return;
        }

        bcs::G2D::CPaneObj *pNullObj = pParam->m_pPaneObj[ICON_PARAMETER::PANE_NULL];
        bcs::G2D::CPaneObj *pBaseObj = pParam->m_pPaneObj[ICON_PARAMETER::PANE_BASE];

        if (pNullObj == 0) {
            continue;
        }

        CBall *pBall = CBallMan::GetBallPtr(nBall);

        if (pBall == 0 || !pBall->IsActive()) {
            continue;
        }

        bcs::FVEC3 dir;
        bcs::FVEC3 radarPos;
        bcs::FVEC3 ballPos(pBall->GetPos());
        int bDisp = 0;
        ballPos.y = CHR_Y;

        if (bcs::BCS_GetRadarPos(radarPos, dir, rCamAt, ballPos, nView)) {
            float fRot = nw4r::math::Atan2Rad(dir.x, dir.y) + PI;

            while (fRot > 2.0f * PI) {
                fRot -= 2.0f * PI;
            }
            while (fRot < 0.0f) {
                fRot += 2.0f * PI;
            }

            radarPos.z = -2.0f;
            radarPos.x = radarPos.x + fX;
            radarPos.y = -(radarPos.y + fY);
            AdjustIconPosition(radarPos, fX, fY);

            pNullObj->GetPane()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(radarPos));
            pNullObj->GetPane()->SetVisible(true);

            if (pBaseObj != 0) {
                pBaseObj->GetPane()->SetRotate(nw4r::math::VEC3(0.0f, 0.0f, fRot * (180.0f / PI)));
            }

            // The frame in the colour of the team holding the puck.
            CChrCore *pChrCore = CChrMan::Util_SearchHoldBallChr(nBall);
            unsigned char nPane;

            if (pChrCore != 0) {
                nPane = CGame::GetTeamColorIdx(pChrCore->GetTeamNo()) + PANE_CHARA_BASE;
            } else {
                nPane = PANE_BALL_BASE;
            }

            const char *pBaseName = s_PaneName[nPane];
            bcs::G2D::CPaneObj *pSrcObj = GetLytObj()->GetChildPaneObj(pBaseName);

            if (pBaseObj != 0 && pSrcObj != 0) {
                bcs::G2D::ReplacePanePicture(pBaseObj->GetPane(), pSrcObj->GetPane());
            }

            CreateBallIconPicture(pParam, nBall, pParam->m_pPaneName[ICON_PARAMETER::PANE_PICTURE]);
            bDisp = 1;
        }

        SetAlphaAndVisible(pNullObj->GetPane(), bDisp);
    }
}

namespace {
    // Indices into s_GoalPaneName.
    enum {
        GOAL_PANE_CENTER = 0,  // parent of every icon
        GOAL_PANE_ICON = 1,    // the layout's own icon, hidden
        GOAL_PANE_BASE_2P = 2, // frames of a goal icon, by goal
        GOAL_PANE_BASE_1P = 3,
        GOAL_PANE_ARROW = 4    // pointer of a goal icon
    };
}

// The goal icons' archive folder and layout.
static const char *s_GoalRootName = "arc";
static const char *s_GoalLayoutName = "HO_RadarIcon.brlyt";

// Names of the goal layout's panes.
static const char *s_GoalPaneName[] = {
    "Center00", "ricon", "BaseIcon_2P_00", "BaseIcon_1P_00", "ricon_goal_r", 0,
};

// Centre of each view on the screen, by bcs::CWorldEnv::m_vstViewSplitType.
static const float s_ViewCenter[4][msw::CRadarIcon2DHockeyGoal::VIEW_MAX][2] = {
    { { -320.0f, -228.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { -320.0f, -228.0f }, { 0.0f, -228.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { -320.0f, -228.0f }, { -320.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { -320.0f, -228.0f }, { 0.0f, -228.0f }, { -320.0f, 0.0f }, { 0.0f, 0.0f } },
};

// Names of the panes each goal icon creates, by view and goal.
static const char *s_GoalRootPaneName[][msw::CRadarIcon2DHockeyGoal::GOAL_MAX] = {
    "Ro00", "Ro01", "Ro02", "Ro03", "Ro04", "Ro05", "Ro06", "Ro07",
    "Ro10", "Ro11", "Ro12", "Ro13", "Ro14", "Ro15", "Ro16", "Ro17",
    "Ro20", "Ro21", "Ro22", "Ro23", "Ro24", "Ro25", "Ro26", "Ro27",
    "Ro30", "Ro31", "Ro32", "Ro33", "Ro34", "Ro35", "Ro36", "Ro37",
};
static const char *s_GoalNullPaneName[][msw::CRadarIcon2DHockeyGoal::GOAL_MAX] = {
    "Go00", "Go01", "Go02", "Go03", "Go04", "Go05", "Go06", "Go07",
    "Go10", "Go11", "Go12", "Go13", "Go14", "Go15", "Go16", "Go17",
    "Go20", "Go21", "Go22", "Go23", "Go24", "Go25", "Go26", "Go27",
    "Go30", "Go31", "Go32", "Go33", "Go34", "Go35", "Go36", "Go37",
};
static const char *s_GoalBasePaneName[] = {
    "Go00_Frm", "Go01_Frm", "Go02_Frm", "Go03_Frm", "Go04_Frm", "Go05_Frm", "Go06_Frm", "Go07_Frm",
    "Go10_Frm", "Go11_Frm", "Go12_Frm", "Go13_Frm", "Go14_Frm", "Go15_Frm", "Go16_Frm", "Go17_Frm",
    "Go20_Frm", "Go21_Frm", "Go22_Frm", "Go23_Frm", "Go24_Frm", "Go25_Frm", "Go26_Frm", "Go27_Frm",
    "Go30_Frm", "Go31_Frm", "Go32_Frm", "Go33_Frm", "Go34_Frm", "Go35_Frm", "Go36_Frm", "Go37_Frm",
};
static const char *s_GoalArrowPaneName[] = {
    "Go00_Pic", "Go01_Pic", "Go02_Pic", "Go03_Pic", "Go04_Pic", "Go05_Pic", "Go06_Pic", "Go07_Pic",
    "Go10_Pic", "Go11_Pic", "Go12_Pic", "Go13_Pic", "Go14_Pic", "Go15_Pic", "Go16_Pic", "Go17_Pic",
    "Go20_Pic", "Go21_Pic", "Go22_Pic", "Go23_Pic", "Go24_Pic", "Go25_Pic", "Go26_Pic", "Go27_Pic",
    "Go30_Pic", "Go31_Pic", "Go32_Pic", "Go33_Pic", "Go34_Pic", "Go35_Pic", "Go36_Pic", "Go37_Pic",
};

// The frame of each goal's icon (an index into s_GoalPaneName).
static const unsigned char s_GoalBasePane[msw::CRadarIcon2DHockeyGoal::GOAL_MAX] = {
    GOAL_PANE_BASE_2P, GOAL_PANE_BASE_1P,
};

namespace {
    // (no symbol; name guessed) As CCockpitLayoutBase::NewPaneCtrl, from
    // the archive's names: a CPaneCtrl on folder pRootName of archive
    // pFileName, handed to the layout manager. Null when a name is missing
    // or anything fails.
    inline bcs::G2D::CPaneCtrl *NewPaneCtrl(const char *pFileName, const char *pRootName, long nLayer)
    {
        if (pFileName == 0 || pRootName == 0) {
            return 0;
        }

        nw4r::lyt::ArcResourceAccessor *pAccessor =
            bcs::G2D::CLytResource::GetInstance()->NewArcResourceAccessor(pFileName, pRootName);

        if (pAccessor == 0) {
            return 0;
        }

        bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
        bcs::G2D::CPaneCtrl *pPaneCtrl = new ((long)0) bcs::G2D::CPaneCtrl;

        if (pPaneCtrl == 0) {
            return 0;
        }
        pManager->AddObject(pPaneCtrl, nLayer);
        pPaneCtrl->CreateLayoutObject(nLayer);
        pPaneCtrl->AttachArchiveResource(pAccessor);
        return pPaneCtrl;
    }

    // (no symbol; name guessed) As CCockpitLayoutBase::CreateLytObj, from
    // the archive's names: makes the layout object of rLytObj.
    inline bcs::G2D::CPaneCtrl *CreateLytObj(msw::CMenuLytObj<bcs::G2D::CPaneCtrl, 1> &rLytObj,
                                             const char *pFileName, const char *pRootName, long nLayer)
    {
        if (msw::CLytCtrl::GetInstance() == 0) {
            return 0;
        }

        bcs::G2D::CPaneCtrl *pPaneCtrl = NewPaneCtrl(pFileName, pRootName, nLayer);

        if (pPaneCtrl != 0) {
            rLytObj.m_nHandle[rLytObj.m_nNum] = pPaneCtrl->m_nId;
            pPaneCtrl->GetLayout()->SetGDLMemArea(0);
            rLytObj.m_nNum++;
        }
        return pPaneCtrl;
    }
}

msw::CRadarIcon2DHockeyGoal::CRadarIcon2DHockeyGoal(long nId)
    : CMenuLinkBase(nId)
    , m_pIconParam(0)
{
    Initialize();
}

msw::CRadarIcon2DHockeyGoal::~CRadarIcon2DHockeyGoal()
{
    if (m_pIconParam != 0) {
        delete[] m_pIconParam;
    }
    m_pIconParam = 0;
}

void msw::CRadarIcon2DHockeyGoal::Initialize()
{
    // Not used. (static-order swap: the original puts the "@0" suffix on
    // viewMaxNum.)
    static const long viewMaxNum = 4;
    static const long goalMaxNum = 4;

    const GXRenderModeObj &rRmode = bcs::CMainApp::g_Rmode;
    float fWidth = (long)rRmode.fbWidth;
    float fViWidth = (long)rRmode.viWidth;
    float fViHeight = (long)rRmode.viHeight; // never used
    float fAspect = bcs::CWorldEnv::m_vatViewAspectType == 1 ? 1.368421f : 1.0f;
    float fScale = fWidth / fViWidth;
    const char *pLayoutName = s_GoalLayoutName;
    const char *pRootName = s_GoalRootName;
    long nResult;

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(*this, CCockpit::GetResourceFileName(1), pRootName, 0);

    if (pPaneCtrl != 0) {
        pPaneCtrl->BuildLayout(0, pLayoutName);
        bcs::G2D::SetVisiblePane(pPaneCtrl->GetLayout()->GetRootPane(), 0);
        pPaneCtrl->SetVisible(1);

        bcs::G2D::CPaneObj *pIconObj = pPaneCtrl->GetChildPaneObj(s_GoalPaneName[GOAL_PANE_ICON]);

        if (pIconObj != 0) {
            pIconObj->GetPane()->SetVisible(false);
        }
        nResult = 0;
    } else {
        nResult = -1;
    }

    if (nResult < 0) {
        m_Unk08 = 2;
        return;
    }

    pPaneCtrl = GetLytObj();
    bcs::G2D::CPaneObj *pCenterObj = pPaneCtrl->GetChildPaneObj(s_GoalPaneName[GOAL_PANE_CENTER]);
    nw4r::lyt::Pane *pCenter = pCenterObj->GetPane();

    pCenter->SetVisible(true);
    pCenterObj->GetPane()->SetScale(nw4r::math::VEC2(fAspect * fScale, 1.0f));
    pCenterObj->GetPane()->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));

    m_pIconParam = new (1, 4) ICON_PARAMETER[VIEW_MAX * GOAL_MAX];
    m_pGoalIcon[0] = 0;
    m_pGoalIcon[1] = 0;
    m_pGoalIcon[2] = 0;
    m_pGoalIcon[3] = 0;
    m_nGoalNum = 0;
    InitGoalIcons(m_pIconParam);
    pPaneCtrl->SetSortZ(-2.0f);
}

void msw::CRadarIcon2DHockeyGoal::InitGoalIcons(ICON_PARAMETER *pParam)
{
    // (static-order swap: the original puts the "@0" suffix on viewMaxNum,
    // here and in Initialize and CheckGoalIcon)
    static const long viewMaxNum = 4;
    static const long goalMaxNum = 4;
    long nGoalNum;
    ICON_PARAMETER *pIcon;

    GetLytObj()->GetChildPaneObj(s_GoalPaneName[GOAL_PANE_CENTER]);

    nGoalNum = CGame::g_GameWork.m_nGoalNum;
    long nViewNum = bcs::CWorldEnv::GetViewSplitNum();
    m_nGoalNum = nGoalNum;

    long nView;

    for (nView = 0; nView < nViewNum; nView++, pParam += goalMaxNum) {
        m_pGoalIcon[nView] = pParam;

        pIcon = pParam;
        long nGoal;

        for (nGoal = 0; nGoal < nGoalNum; nGoal++, pIcon++) {
            pIcon->Clear();
            CreateGoalIcon(pIcon, nGoal, s_GoalRootPaneName[nView][nGoal], s_GoalNullPaneName[nView][nGoal],
                           s_GoalBasePaneName[nView * nGoalNum + nGoal],
                           s_GoalArrowPaneName[nView * nGoalNum + nGoal]);
        }
        for (; nGoal < goalMaxNum; nGoal++, pIcon++) {
            pIcon->Clear();
        }
    }
    for (; nView < viewMaxNum; nView++) {
        m_pGoalIcon[nView] = 0;
    }
}

void msw::CRadarIcon2DHockeyGoal::Main()
{
}

void msw::CRadarIcon2DHockeyGoal::Disp()
{
    GetLytObj();

    long nViewNum = bcs::CWorldEnv::GetViewSplitNum();
    const float(*pCenter)[2] = s_ViewCenter[bcs::CWorldEnv::m_vstViewSplitType];

    for (long nView = 0; nView < nViewNum; pCenter++, nView++) {
        bcs::CCamera *pCamera = bcs::CCameraMan::GetActiveCamera(nView);

        if (pCamera != 0) {
            DispGoalIcon(nView, pCamera->GetViewData().at, (*pCenter)[0], (*pCenter)[1]);
        }
    }
}

void msw::CRadarIcon2DHockeyGoal::DispGoalIcon(long nView, const bcs::FVEC3 &rCamAt, float fX, float fY)
{
    // The height the goals are shown at.
    static const float GOAL_Y = 0.75f;

    long nTeamNum = CGame::g_GameWork.m_nTeamNum;
    ICON_PARAMETER *pParam = m_pGoalIcon[nView];

    for (long nTeam = 0; nTeam < nTeamNum; nTeam++, pParam++) {
        if (pParam == 0) {
            return;
        }

        bcs::G2D::CPaneObj *pNullObj = pParam->m_pPaneObj[ICON_PARAMETER::PANE_NULL];
        bcs::G2D::CPaneObj *pBaseObj = pParam->m_pPaneObj[ICON_PARAMETER::PANE_BASE];

        if (pNullObj == 0) {
            continue;
        }

        bcs::FVEC3 dir;
        bcs::FVEC3 radarPos;
        int bDisp = 0;
        bcs::FVEC3 goalPos(CGame::GetGoalPos(CGame::GetGuardGoalIdx(nTeam)));
        goalPos.y = GOAL_Y;

        if (bcs::BCS_GetRadarPos(radarPos, dir, rCamAt, goalPos, nView)) {
            float fRot = nw4r::math::Atan2Rad(dir.x, dir.y) + PI;

            while (fRot > 2.0f * PI) {
                fRot -= 2.0f * PI;
            }
            while (fRot < 0.0f) {
                fRot += 2.0f * PI;
            }

            radarPos.x = radarPos.x + fX;
            radarPos.y = -(radarPos.y + fY);

            pNullObj->GetPane()->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(radarPos));
            pNullObj->GetPane()->SetVisible(true);

            if (pBaseObj != 0) {
                pBaseObj->GetPane()->SetRotate(nw4r::math::VEC3(0.0f, 0.0f, fRot * (180.0f / PI)));
            }
            bDisp = 1;
        }

        CRadarIcon2D::SetAlphaAndVisible(pNullObj->GetPane(), bDisp);
    }
}

// Uncalled stand-in: the original has a CheckGoalIcon whose code the module
// link removed (nothing calls it); only its two constants remain in
// .rodata, after DispGoalIcon's. Its body is not known.
void msw::CRadarIcon2DHockeyGoal::CheckGoalIcon()
{
    static const long viewMaxNum = 4;
    static const long goalMaxNum = 4;
}

long msw::CRadarIcon2DHockeyGoal::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

        if (pPaneCtrl != 0) {
            pPaneCtrl->SetVisible((int)pParam);
        }
    }
    return 0;
}

void msw::CRadarIcon2DHockeyGoal::CreateGoalIcon(ICON_PARAMETER *pParam, long nGoal, const char *pRootName,
                                                 const char *pNullName, const char *pBaseName,
                                                 const char *pArrowName)
{
    // Never used, as in CRadarIcon2DHockey::CreateCharaIcon.
    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    bcs::G2D::CPaneObj *pCenterObj = pPaneCtrl->GetChildPaneObj(s_GoalPaneName[GOAL_PANE_CENTER]);

    nw4r::lyt::Pane *pRootPane = bcs::G2D::CreateNullPane(pRootName);

    if (pRootPane == 0) {
        return;
    }

    bcs::G2D::CPaneObj *pRootObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pRootPane);

    if (pRootObj == 0) {
        nw4r::lyt::Layout::DeleteObj(pRootPane);
        return;
    }
    pPaneCtrl->AppendPaneObj(pRootObj, *pCenterObj);
    pRootPane->SetVisible(true);

    nw4r::lyt::Pane *pNullPane = bcs::G2D::CreateNullPane(pNullName);

    if (pNullPane == 0) {
        return;
    }

    bcs::G2D::CPaneObj *pNullObj = bcs::G2D::CreatePaneObj(pPaneCtrl, pNullPane);

    if (pNullObj == 0) {
        nw4r::lyt::Layout::DeleteObj(pNullPane);
        return;
    }
    pPaneCtrl->AppendPaneObj(pNullObj, *pRootObj);
    CRadarIcon2D::InitNullPane(pNullPane);

    bcs::G2D::CPaneObj *pBaseObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_GoalPaneName[s_GoalBasePane[nGoal]],
                                                              pBaseName, s_GoalLayoutName, 0);

    if (pBaseObj == 0) {
        return;
    }
    pPaneCtrl->AppendPaneObj(pBaseObj, *pNullObj);
    CRadarIcon2D::InitPicPane(pBaseObj->GetPane());

    bcs::G2D::CPaneObj *pArrowObj = bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_GoalPaneName[GOAL_PANE_ARROW],
                                                               pArrowName, s_GoalLayoutName, 0);

    if (pArrowObj == 0) {
        return;
    }
    pPaneCtrl->AppendPaneObj(pArrowObj, *pNullObj);
    CRadarIcon2D::InitPicPane(pArrowObj->GetPane());

    pParam->SetPaneObj(ICON_PARAMETER::PANE_ROOT, pRootObj);
    pParam->SetPaneObj(ICON_PARAMETER::PANE_NULL, pNullObj);
    pParam->SetPaneObj(ICON_PARAMETER::PANE_BASE, pBaseObj);
    pParam->SetPaneObj(ICON_PARAMETER::PANE_ARROW, pArrowObj);
}

void msw::CRadarIcon2DHockeyGoal::SetDispGoalIcon(long nGoal, int bDisp)
{
    GetLytObj();

    long nViewNum = bcs::CWorldEnv::GetViewSplitNum();

    for (long nView = 0; nView < nViewNum; nView++) {
        bcs::G2D::CPaneObj *pRootObj = m_pGoalIcon[nView][nGoal].m_pPaneObj[ICON_PARAMETER::PANE_ROOT];

        if (pRootObj != 0) {
            pRootObj->GetPane()->SetVisible(bDisp == 1);
        }
    }
}

// The puck marker's archive folder, layout and animation.
static const char *s_MarkerRootName = "arc";
static const char *s_MarkerLayoutName = "mh_packicon01_01.brlyt";
static const char *s_MarkerAnimName[] = { "mh_packicon01_01.brlan", 0 };

// The marker's pictures, by SBallParameter::m_nKind.
static const char s_MarkerPaneName[msw::CBallMarker2DHockey::SBallParameter::KIND_NUM][16] = {
    "Picture_00", "Picture_01", "Picture_02",
};

// How far above a character's position the marker goes, by character
// number (CChrCore::m_nChrId), before the character's scale.
static const float s_MarkerHeight[] = {
    1.5f, 1.6f, 1.6f, 1.6f, 1.6f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.8f, 1.5f, 1.5f,
    1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.6f, 1.6f, 1.4f, 1.4f, 1.8f, 1.8f,
};

// Not used. (the first is the animation's name, copied by the static
// initialiser; the second the marker's picture size: one view, and split
// screens)
static const char *s_pMarkerAnimName = s_MarkerAnimName[0];
static const nw4r::lyt::Size s_MarkerSize[2] = {
    nw4r::lyt::Size(24.0f, 24.0f),
    nw4r::lyt::Size(16.0f, 16.0f),
};

msw::CBallMarker2DHockey::CBallMarker2DHockey(long nId)
    : CMenuLinkBase(nId)
{
    long nLayer = 2;

    if (nId == 4) {
        nLayer = 1;
    }

    const char *pRootName = s_MarkerRootName;
    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(*this, CCockpit::GetResourceFileName(1), pRootName, nLayer);

    if (pPaneCtrl != 0) {
        pPaneCtrl->BuildLayout(0, s_MarkerLayoutName);
        bcs::G2D::SetVisiblePane(pPaneCtrl->GetLayout()->GetRootPane(), 1);
        pPaneCtrl->SetVisible(1);
    }

    if (GetLytObj() == 0) {
        m_Unk08 = 2;
    } else {
        MarkerInit();
    }
}

msw::CBallMarker2DHockey::~CBallMarker2DHockey()
{
    if (m_pBallParam != 0) {
        delete[] m_pBallParam;
    }
    m_pBallParam = 0;
}

void msw::CBallMarker2DHockey::MarkerInit()
{
    if (!CBallMan::IsActive()) {
        return;
    }
    CBallMan::GetInstance();

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl == 0) {
        return;
    }

    nw4r::lyt::Size size(s_MarkerSize[bcs::CWorldEnv::GetViewSplitNum() != 1]);
    long nBallNum = CBallMan::GetActiveBallNum();
    SBallParameter *pParamTop = 0;

    if (nBallNum > 0) {
        pParamTop = new (1) SBallParameter[nBallNum];

        SBallParameter *pParam = pParamTop;

        for (long nBall = 0; nBall < nBallNum; pParam++, nBall++) {
            pParam->m_Pos.x = 0.0f;
            pParam->m_Pos.y = 0.0f;
            pParam->m_Pos.z = 0.0f;
            pParam->m_pPaneName[0] = 0;
            pParam->m_pPaneName[1] = 0;
            pParam->m_pPaneName[2] = 0;
            pParam->m_nFlag = 0;
            pParam->m_nKind = SBallParameter::KIND_NONE;

            for (long nKind = 0; nKind < SBallParameter::KIND_NUM; nKind++) {
                pParam->m_pPaneName[nKind] = s_MarkerPaneName[nKind];

                bcs::G2D::CPaneObj *pObj = pPaneCtrl->GetChildPaneObj(s_MarkerPaneName[nKind]);

                if (pObj != 0) {
                    pObj->GetPane()->SetSize(size);
                }
            }
            SetBallKind(pParam, 0);
        }
    }

    m_nBallNum = nBallNum;
    m_pBallParam = pParamTop;
}

namespace {
    // (no symbol; name guessed) The character holding ball hBall, or null.
    inline msw::CChrCore *SearchBallHolder(unsigned long hBall)
    {
        long nChrNum = msw::CChrMan::GetInstance()->GetChrCoreNum();

        for (long nChr = 0; nChr < nChrNum; nChr++) {
            msw::CChrCore *pChrCore = msw::CChrMan::GetChrCore(nChr);

            if (hBall == pChrCore->GetBallHandle()) {
                return pChrCore;
            }
        }
        return 0;
    }
}

void msw::CBallMarker2DHockey::Main()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl != 0) {
        if (CBallMan::IsActive()) {
            CBallMan *pBallMan = CBallMan::GetInstance();
            long nBallNum = m_nBallNum;
            SBallParameter *pParam = m_pBallParam;

            for (long nBall = 0; nBall < nBallNum; nBall++, pParam++) {
                if (pPaneCtrl->GetChildPaneObj(pParam->GetPaneName()) == 0) {
                    continue;
                }

                CBall *pBall = pBallMan->GetBallPtr(nBall);

                if ((int)((CGame::g_GameWork.m_nFlag18 >> 2) & 1) == 1 || pBall == 0) {
                    SetBallKind(&m_pBallParam[nBall], SBallParameter::KIND_NONE);
                } else if (pBall->IsAttach() == 1) {
                    // Over the character holding the puck, in its team's
                    // picture; hidden over the player's own character.
                    CChrCore *pHolder = SearchBallHolder(pBallMan->GetBallHandle(nBall));

                    if (pHolder != 0) {
                        pParam->m_Pos = pHolder->GetPos();
                        pParam->m_Pos.y += pHolder->GetScaleBase() * s_MarkerHeight[pHolder->GetChrIdS()];
                        SetBallKind(&m_pBallParam[nBall], pHolder->GetTeamNo() + 1);

                        int bOwn;

                        if (pHolder->IsPadCtrl() == 1 && pHolder->GetPadNo() == m_nId - 4) {
                            bOwn = 1;
                        } else {
                            bOwn = 0;
                        }
                        pParam->SetHide(bOwn);
                    }
                } else if ((int)(CGame::g_GameWork.m_nFlag18 & 1) == 1) {
                    // Over the loose puck.
                    SetBallKind(&m_pBallParam[nBall], 0);
                    pParam->m_Pos = pBall->GetPos();
                    pParam->m_Pos.y += 0.4f;
                    pParam->m_nFlag &= (unsigned char)~1;
                } else {
                    pParam->m_nFlag |= 1;
                }
            }
        }
    }
}

void msw::CBallMarker2DHockey::SetBallKind(SBallParameter *pParam, long nKind)
{
    if (pParam != 0 && pParam->m_nKind != nKind) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

        if (pPaneCtrl != 0) {
            // Hide the picture shown so far.
            unsigned char nOldKind = pParam->m_nKind;
            const char *pName = nOldKind < SBallParameter::KIND_NUM ? pParam->m_pPaneName[nOldKind] : 0;

            if (pName != 0) {
                bcs::G2D::CPaneObj *pObj = pPaneCtrl->GetChildPaneObj(pName);

                if (pObj != 0) {
                    nw4r::lyt::Pane *pPane = pObj->GetPane();
                    pPane->SetVisible(false);
                }
            }

            if (nKind < SBallParameter::KIND_NUM) {
                // (the extra local keeps SetBallKind a call, as in the
                // original: auto-inlining goes by statement count)
                unsigned char nNewKind = nKind;
                pParam->m_nKind = nNewKind;
            }
            pParam->m_bUnk = 0;
        }
    }
}

void msw::CBallMarker2DHockey::Disp()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl != 0 && pPaneCtrl->IsVisible()) {
        // (the rectangle is not used)
        reinterpret_cast<nw4r::lyt::Layout *>(pPaneCtrl->GetLayout())->GetLayoutRect();

        SBallParameter *pParam = m_pBallParam;
        bcs::FVEC3 *pScreenPos;
        long nBallNum = m_nBallNum;
        long nCamera = m_nId - 4;
        pScreenPos = (bcs::FVEC3 *)__alloca(nBallNum * sizeof(bcs::FVEC3));

        for (long nBall = 0; nBall < nBallNum; pParam++, pScreenPos++, nBall++) {
            bcs::G2D::CPaneObj *pObj = pPaneCtrl->GetChildPaneObj(pParam->GetPaneName());

            if (pObj == 0) {
                continue;
            }

            int bDisp = 0;
            CBallMan::GetInstance();

            if (!(pParam->m_nFlag & 1) &&
                bcs::G2D::GetWorldScreenPosition(pParam->m_Pos, nCamera, *pScreenPos) == 0) {
                nw4r::lyt::Pane *pPane = pObj->GetPane();
                bDisp = 1;
                pPane->SetTranslate(reinterpret_cast<const nw4r::math::VEC3 &>(*pScreenPos));
            }
            pObj->GetPane()->SetVisible(bDisp == 1);
        }
    }
}

long msw::CBallMarker2DHockey::SendEvent(long nEvent, void *pParam)
{
    if (GetLytObj() == 0) {
        return -2;
    }

    switch (nEvent) {
    case 0:
    case 1:
        bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

        if (pPaneCtrl != 0) {
            pPaneCtrl->SetVisible(nEvent == 1);
        }
        break;
    }
    return 0;
}
