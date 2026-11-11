// msw::CRadarIcon2DDodge: the radar icons of the dodge game (the same code
// as msw::CRadarIcon2DBasket, plus the outfielder check), and
// msw::CLockCursorDodge: the lock-on cursors.
#include "game/msw/CRadarIcon2DDodge.hpp"
#include "game/msw/CLockCursorDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitDodge.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"

// Names of the radar layout's panes. (the grouping into members is a guess:
// the original is one unnamed object of nine pointers)
static struct {
    const char *pCenter;       // parent of every icon
    const char *pBallBase;     // frame of a ball icon
    const char *pCharaBase[4]; // frame of a character icon, by team colour
    const char *pBall[3];      // not used by this class
} s_PaneName = {
    "Center00",
    "BaseIcon_Ball",
    { "BaseIcon_2P", "BaseIcon_1P", "BaseIcon_4P", "BaseIcon_3P" },
    { "DO_Ball00", "DO_Ball01", "DO_Ball02" },
};

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

msw::CRadarIcon2DDodge::CRadarIcon2DDodge(long nId)
    : CRadarIcon2D(nId)
{
    Initialize();
}

msw::CRadarIcon2DDodge::~CRadarIcon2DDodge()
{
    Finalize();
}

void msw::CRadarIcon2DDodge::Initialize()
{
    // Not used; the original has both objects. (static-order swap: the
    // original puts the "@0" suffix on viewMaxNum.)
    static const long viewMaxNum = 4;
    static const long chrMaxNum = 8;

    InitCharaIcons(m_pIconParam);
    InitBallIcons(m_pIconParam + VIEW_MAX * ICON_MAX);
}

void msw::CRadarIcon2DDodge::CreateCharaIcon(ICON_PARAMETER *pParam, long nChr, const char *pNullName,
                                           const char *pBaseName, const char *pPictureName)
{
    CChrCore *pChrCore = CChrMan::GetChrCore(nChr);

    if (pChrCore == 0) {
        return;
    }

    // The extra outfielders get no icon.
    if (CGame::IsCharEx_DodgeOutfielder(nChr)) {
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

void msw::CRadarIcon2DDodge::CreateBallIcon(ICON_PARAMETER *pParam, long nBall, const char *pNullName,
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

void msw::CRadarIcon2DDodge::Finalize()
{
}

long msw::CRadarIcon2DDodge::SendEvent(long nEvent, void *pParam)
{
    return CRadarIcon2D::SendEvent(nEvent, pParam);
}

// The lock-on cursor layout.
static const char *s_LayoutName[] = { "md_lcursor01_01.brlyt" };

// Its animations. Per player (1P to 4P, then PS): the cursor ends, ends
// and loops, loops, starts.
static const char *s_AnimName[] = {
    "md_lcursor01_01_end_1Pcsrl.brlan",
    "md_lcursor01_01_end_2Pcsrl.brlan",
    "md_lcursor01_01_end_3Pcsrl.brlan",
    "md_lcursor01_01_end_4Pcsrl.brlan",
    "md_lcursor01_01_end_start_PScsrl.brlan",
    "md_lcursor01_01_end_loop_1Pcsrl.brlan",
    "md_lcursor01_01_end_loop_2Pcsrl.brlan",
    "md_lcursor01_01_end_loop_3Pcsrl.brlan",
    "md_lcursor01_01_end_loop_4Pcsrl.brlan",
    "md_lcursor01_01_end_loop_PScsrl.brlan",
    "md_lcursor01_01_loop_1Pcsrl.brlan",
    "md_lcursor01_01_loop_2Pcsrl.brlan",
    "md_lcursor01_01_loop_3Pcsrl.brlan",
    "md_lcursor01_01_loop_4Pcsrl.brlan",
    "md_lcursor01_01_loop_PScsrl.brlan",
    "md_lcursor01_01_start_1Pcsrl.brlan",
    "md_lcursor01_01_start_2Pcsrl.brlan",
    "md_lcursor01_01_start_3Pcsrl.brlan",
    "md_lcursor01_01_start_4Pcsrl.brlan",
    "md_lcursor01_01_start_PScsrl.brlan",
};

// The layout's panes: the root, then the four parts every cursor is copied
// from (a frame and three children).
static const char *s_PaneName2[] = {
    "lockcursor", "lockcursor01_00", "lcsr01_01", "lcsr01_02", "lcsr01_03", 0,
};

// Indices into s_PaneName2: the root, then the source panes of each
// player's cursors (all players use the same ones).
static const unsigned char s_RootPaneIdx[] = { 0 };
static const unsigned char s_CursorPaneIdx[msw::CLockCursorDodge::PLAYER_MAX][4] = {
    { 1, 2, 3, 4 },
    { 1, 2, 3, 4 },
    { 1, 2, 3, 4 },
    { 1, 2, 3, 4 },
};

// Names of the copies: lc<player><cursor><part>.
static const char *s_CursorPaneName[msw::CLockCursorDodge::PLAYER_MAX][msw::CLockCursorDodge::CURSOR_MAX][4] = {
    {
        { "lc000", "lc001", "lc002", "lc003" },
        { "lc010", "lc011", "lc012", "lc013" },
        { "lc020", "lc021", "lc022", "lc023" },
    },
    {
        { "lc100", "lc101", "lc102", "lc103" },
        { "lc110", "lc111", "lc112", "lc113" },
        { "lc120", "lc121", "lc122", "lc123" },
    },
    {
        { "lc200", "lc201", "lc202", "lc203" },
        { "lc210", "lc211", "lc212", "lc213" },
        { "lc220", "lc221", "lc222", "lc223" },
    },
    {
        { "lc300", "lc301", "lc302", "lc303" },
        { "lc310", "lc311", "lc312", "lc313" },
        { "lc320", "lc321", "lc322", "lc323" },
    },
};

namespace {
    // Columns of s_AnimIdx.
    enum {
        ANIM_START,
        ANIM_LOOP,
        ANIM_END,
        ANIM_END_LOOP,
        ANIM_NUM
    };
}

// The animations (indices into s_AnimName) of each player's cursors. The
// fourth player uses the PS set.
static const unsigned char s_AnimIdx[msw::CLockCursorDodge::PLAYER_MAX][ANIM_NUM] = {
    { 15, 10, 0, 5 },
    { 16, 11, 1, 6 },
    { 17, 12, 2, 7 },
    { 19, 14, 4, 9 },
};

// How far above a character's position its cursor sits, by player number.
static const float s_CursorHeight[] = {
    0.75f, 0.8f,  0.8f,  0.8f,  0.8f,  0.75f, 0.75f, 0.75f, 0.75f,
    0.75f, 0.9f,  0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f,
    0.75f, 0.8f,  0.8f,  0.7f,  0.7f,  0.9f,  0.9f,
};

msw::CLockCursorDodge::CLockCursorDodge(long nId)
    : CCockpitLayoutBase(nId)
{
    Set();

    if (GetLytObj() == 0) {
        m_Unk08 = 2;
    } else {
        CursorInit();
    }
}

msw::CLockCursorDodge::~CLockCursorDodge()
{
    Remove();
}

void msw::CLockCursorDodge::CursorInit()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl == 0) {
        return;
    }

    // Counts the pad-controlled characters; the count is not used.
    long nChrNum = CChrMan::GetChrCoreNum();
    long nPadNum = 0;

    for (long nChr = 0; nChr < nChrNum; nChr++) {
        CChrCore *pChrCore = CChrMan::GetChrCore(nChr);

        if (pChrCore != 0 && pChrCore->IsPadCtrl()) {
            nPadNum++;
        }
    }

    m_nPlayerNum = PLAYER_MAX;

    for (long nPlayer = 0; nPlayer < PLAYER_MAX; nPlayer++) {
        SCursorParameter *pCursor = m_Cursor[nPlayer];

        for (long nCursor = 0; nCursor < CURSOR_MAX; nCursor++, pCursor++) {
            pCursor->Init(nPlayer);

            bcs::G2D::CPaneObj *pBaseObj =
                bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_PaneName2[s_CursorPaneIdx[nPlayer][0]],
                                           s_CursorPaneName[nPlayer][nCursor][0], s_LayoutName[0], 0);
            pPaneCtrl->AppendPaneObj(pBaseObj, (const char *)0);
            pBaseObj->GetPane()->SetVisible(true);

            pCursor->m_pAnimPaneName = 0;
            for (long nPart = 1; nPart < 4; nPart++) {
                bcs::G2D::CPaneObj *pPartObj =
                    bcs::G2D::DuplicatePaneObj(pPaneCtrl, s_PaneName2[s_CursorPaneIdx[nPlayer][nPart]],
                                               s_CursorPaneName[nPlayer][nCursor][nPart], s_LayoutName[0], 1);

                if (pPartObj != 0) {
                    pPaneCtrl->AppendPaneObj(pPartObj, *pBaseObj);
                    pPartObj->GetPane()->SetVisible(true);

                    if (pCursor->m_pAnimPaneName == 0) {
                        pCursor->m_pAnimPaneName = pPartObj->GetPane()->GetName();
                    }
                }
            }
            pCursor->m_pPaneName = pBaseObj->GetPane()->GetName();
            pCursor->PlayAnimation(s_AnimIdx[pCursor->m_nPlayer][ANIM_END_LOOP], -1, pPaneCtrl);
        }
    }

    for (long i = 0; i < (long)(sizeof(s_RootPaneIdx) / sizeof(s_RootPaneIdx[0])); i++) {
        m_pPaneAccessor->SetPaneDisp(s_RootPaneIdx[i], 0);
    }
}

int msw::CLockCursorDodge::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    CCockpit *pCockpit = CCockpitDodge::GetTask();

    if (pLytCtrl == 0) {
        return -1;
    }

    long nLayer = 2;

    if (m_nId == 4) {
        nLayer = 1;
    }

    const char *pFileName = pCockpit->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, nLayer, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->BuildLayout(0, s_LayoutName[0]);
            pPaneCtrl->BuildAnim(0, s_AnimName, sizeof(s_AnimName) / sizeof(s_AnimName[0]));
            SetPaneAccessor(pPaneCtrl, s_PaneName2, 5);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->SetVisible(1);
        }
    }
    return 0;
}

void msw::CLockCursorDodge::Remove()
{
    m_nStep = 0;
}

void msw::CLockCursorDodge::Main()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl == 0) {
        return;
    }

    for (long nPlayer = 0; nPlayer < PLAYER_MAX; nPlayer++) {
        SCursorParameter *pCursor = m_Cursor[nPlayer];

        for (long nCursor = 0; nCursor < CURSOR_MAX; nCursor++, pCursor++) {
            switch (pCursor->m_nState) {
                case 1: {
                    // Ending: play the end animation, then loop the ended
                    // cursor and free it.
                    unsigned char nEnd = s_AnimIdx[pCursor->m_nPlayer][ANIM_END];
                    unsigned char nAnim = pCursor->m_nAnim;

                    if (nAnim != nEnd) {
                        pCursor->PlayAnimation(nEnd, -1, pPaneCtrl);
                    } else if (nAnim == nEnd) {
                        if (pCursor->IsAnimEnd(nAnim, pPaneCtrl)) {
                            pCursor->m_nState = 0;
                            pCursor->m_nTarget = 0xFF;
                            pCursor->PlayAnimation(s_AnimIdx[pCursor->m_nPlayer][ANIM_END_LOOP], -1, pPaneCtrl);
                        }
                    }
                    break;
                }
                case 3:
                    // Starting: play the start animation, then the loop.
                    pCursor->m_nState = 2;
                    pCursor->PlayAnimation(s_AnimIdx[pCursor->m_nPlayer][ANIM_START],
                                           s_AnimIdx[pCursor->m_nPlayer][ANIM_LOOP], pPaneCtrl);
                    break;
            }
        }
    }
}

void msw::CLockCursorDodge::Disp()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl != 0 && pPaneCtrl->IsVisible()) {
        GetTargetPosition();

        long nCamera = m_nId - 4;

        for (long nPlayer = 0; nPlayer < PLAYER_MAX; nPlayer++) {
            SCursorParameter *pCursor = m_Cursor[nPlayer];

            for (long nCursor = 0; nCursor < CURSOR_MAX; nCursor++, pCursor++) {
                bcs::G2D::CPaneObj *pPaneObj = pPaneCtrl->GetChildPaneObj(pCursor->m_pPaneName);

                if (pPaneObj == 0) {
                    continue;
                }

                int bVisible = 0;

                // Follow the target on screen.
                if (pCursor->IsTarget() == 1 && pCursor->IsPosValid() == 1) {
                    bcs::FVEC3 screen;

                    if (bcs::G2D::GetWorldScreenPosition(pCursor->m_Pos, nCamera, screen) == 0) {
                        pPaneObj->GetPane()->SetTranslate((const nw4r::math::VEC3 &)screen);
                    }
                }

                unsigned char nState = pCursor->m_nState;

                if (nState != 3 && nState != 0) {
                    bVisible = 1;
                }

                if (bVisible == 1) {
                    pCursor->SetRotZ(pCursor->m_fRotZ, pPaneCtrl);
                } else if (nState == 2) {
                    pCursor->m_nState = 1;
                }

                pPaneObj->GetPane()->SetVisible(bVisible);
            }
        }
    }
}

long msw::CLockCursorDodge::GetTargetPosition()
{
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();
    long nChrNum = CChrMan::GetChrCoreNum();
    // How many cursors are on each character.
    long *pLockNum = (long *)__alloca(nChrNum * sizeof(long));

    for (long nChr = 0; nChr < nChrNum; nChr++) {
        pLockNum[nChr] = 0;
    }

    for (long nPlayer = 0; nPlayer < PLAYER_MAX; nPlayer++) {
        SCursorParameter *pCursor = m_Cursor[nPlayer];

        for (long nCursor = 0; nCursor < CURSOR_MAX; nCursor++, pCursor++) {
            pCursor->m_Flag.bPosValid = false;

            if (pCursor->m_nState != 2 || pCursor->IsTarget() == 0) {
                continue;
            }

            CChrCore *pChrCore = CChrMan::GetChrCore(pCursor->m_nTarget);

            if (pChrCore == 0 || pChrCore->IsDisable()) {
                continue;
            }

            if (pLockNum[pCursor->m_nTarget] > 0) {
                if (pCursor->m_nLockFrame == 0) {
                    pCursor->m_fRotZ = 180.0f;
                }
                pCursor->m_Flag.bShared = true;
            } else {
                pCursor->m_Flag.bShared = false;
            }
            pLockNum[pCursor->m_nTarget]++;

            pCursor->m_Pos = pChrCore->GetPos();
            pCursor->m_Pos.y += s_CursorHeight[pChrCore->GetPlayerNo()];
            pCursor->m_Flag.bPosValid = true;
            pCursor->m_nLockFrame++;
        }
    }
    return 0;
}

// Defined after its callers: the original has its local static after
// GetTargetPosition's float constant.
inline void msw::CLockCursorDodge::SCursorParameter::PlayAnimation(long nAnim, long nNext,
                                                                 bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    static const float speed = 1.0f;

    if (pPaneCtrl == 0) {
        return;
    }

    bcs::G2D::CPaneObj *pPaneObj = pPaneCtrl->GetChildPaneObj(m_pPaneName);

    if (pPaneObj == 0) {
        return;
    }

    long nBlendAnim = m_nBlendAnim;

    if (nBlendAnim == 0xFF) {
        pPaneObj->PlayAnimation(nAnim, speed, 0.0f, 1, 1, 1);
    } else {
        pPaneObj->PlayAnimation(nBlendAnim, speed, 0.0f, 1, 1, 1);
        pPaneObj->PlayAnimation(nAnim, speed, 0.0f, 1, 0, 1);
    }

    if (nNext >= 0) {
        pPaneObj->PushNextAnimation(nNext, speed, 0.0f, 1, 1);
    }
    m_nAnim = nAnim;
}

long msw::CLockCursorDodge::SendEvent(long nEvent, void *pParam)
{
    long nRet = 0;
    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl == 0) {
        return -2;
    }

    switch (nEvent) {
        case 0:
        case 1:
            pPaneCtrl->SetVisible(nEvent == 1);
            break;
        case 2:
        case 3:
        case 4:
        case 5: {
            // Lock a free cursor of player nEvent - 2 on character pParam.
            nRet = -1;
            long nChrNum = CChrMan::GetChrCoreNum();
            unsigned char nTarget = (unsigned char)(long)pParam;
            SCursorParameter *pCursor = m_Cursor[nEvent - 2];

            for (long nChr = 0; nChr < nChrNum; nChr++) {
                CChrMan::GetChrCore(nChr);

                for (long nCursor = 0; nCursor < CURSOR_MAX; nCursor++, pCursor++) {
                    if (pCursor->m_nState == 0) {
                        pCursor->Reset();
                        pCursor->m_nState = 3;

                        CChrCore *pChrCore = CChrMan::GetChrCore(nTarget);

                        if (pChrCore != 0 && !pChrCore->IsDisable()) {
                            pCursor->m_nTarget = nTarget;
                        }
                        nRet = nCursor;
                        break;
                    }
                }
                break;
            }
            break;
        }
        case 6:
        case 7:
        case 8:
        case 9: {
            // Release the cursors of player nEvent - 6 on character pParam.
            SCursorParameter *pCursor = m_Cursor[nEvent - 6];
            unsigned char nTarget = (unsigned char)(long)pParam;

            nRet = -1;
            for (long nCursor = 0; nCursor < CURSOR_MAX; nCursor++, pCursor++) {
                if (pCursor->m_nState == 2 && pCursor->m_nTarget == nTarget) {
                    pCursor->m_nState = 1;
                    nRet = 0;
                }
            }
            break;
        }
        case 10001:
            pPaneCtrl->SetVisible((int)pParam);
            break;
    }
    return nRet;
}
