// msw::CCockpitHockeyMap: the small map of the rink in the hockey game's
// cockpit. Every character and the puck get a marker on it, built from
// panes made at run time; SetPlayerPosition and SetCameraPosition move them.
#include "game/msw/CCockpitHockeyMap.hpp"

#include <string.h>
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTexMap.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CDispTaskList.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "nw4r/lyt/layout.h"

namespace {

    const long ANIM_NUM = 5;

    // Animations, by index into the animation names.
    enum {
        ANIM_BASE = 0,
        ANIM_MAP_00 = 1,
        ANIM_MAP_01 = 2,
        ANIM_MAP_02 = 3,
        ANIM_MAP_03 = 4
    };

    // Panes, by index into s_PaneName.
    enum {
        PANE_minimap_base,    // 0x00
        PANE_pic_base,        // 0x01
        PANE_goal,            // 0x02
        PANE_pic_goal_b_waku, // 0x03
        PANE_pic_goal_r_waku, // 0x04
        PANE_BaseIcon_ball,   // 0x05
        PANE_BaseIcon_ball_r, // 0x06
        PANE_BaseIcon_ball_b, // 0x07
        PANE_BaseIcon_r,      // 0x08
        PANE_BaseIcon_b,      // 0x09
        PANE_player_1P,       // 0x0A
        PANE_player_2P,       // 0x0B
        PANE_player_3P,       // 0x0C
        PANE_player_4P,       // 0x0D
        PANE_com,             // 0x0E
        PANE_n0,              // 0x0F
        PANE_r0 = PANE_n0 + 8,  // 0x17
        PANE_m0 = PANE_r0 + 8,  // 0x1F
        PANE_f0 = PANE_m0 + 8,  // 0x27
        PANE_1p = PANE_f0 + 8,  // 0x2F
        PANE_n9 = PANE_1p + 4,  // 0x33
        PANE_r9,              // 0x34
        PANE_b9,              // 0x35
        PANE_c1,              // 0x36
        PANE_c2,              // 0x37
        PANE_c3,              // 0x38
        PANE_c4,              // 0x39
        PANE_c5,              // 0x3A
        PANE_NUM
    };

    // The layout, its animations and its panes.
    const char *s_LayoutName = "HO_minimap.brlyt";

    const char *s_AnimName[ANIM_NUM] = {
        "HO_minimap_bese_00.brlan",
        "HO_minimap_map_00.brlan",
        "HO_minimap_map_01.brlan",
        "HO_minimap_map_02.brlan",
        "HO_minimap_map_03.brlan",
    };

    const char *s_PaneName[PANE_NUM] = {
        "minimap_base",
        "pic_base",
        "goal",
        "pic_goal_b_waku",
        "pic_goal_r_waku",
        "BaseIcon_ball",
        "BaseIcon_ball_r",
        "BaseIcon_ball_b",
        "BaseIcon_r",
        "BaseIcon_b",
        "player_1P",
        "player_2P",
        "player_3P",
        "player_4P",
        "com",
        "n0", "n1", "n2", "n3", "n4", "n5", "n6", "n7",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "m0", "m1", "m2", "m3", "m4", "m5", "m6", "m7",
        "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
        "1p", "2p", "3p", "4p",
        "n9",
        "r9",
        "b9",
        "c1", "c2", "c3", "c4", "c5",
    };

    // The panes made for one character's marker: a null pane placed on the
    // map, a null pane under it that turns, the base icon in the team's
    // colour (a copy) and the face picture.
    enum {
        MARKER_NULL,
        MARKER_ROT,
        MARKER_BASE,
        MARKER_FACE,
        MARKER_PANE_NUM
    };

    const unsigned char s_PlayerPane[8][MARKER_PANE_NUM] = {
        { PANE_n0 + 0, PANE_r0 + 0, PANE_m0 + 0, PANE_f0 + 0 },
        { PANE_n0 + 1, PANE_r0 + 1, PANE_m0 + 1, PANE_f0 + 1 },
        { PANE_n0 + 2, PANE_r0 + 2, PANE_m0 + 2, PANE_f0 + 2 },
        { PANE_n0 + 3, PANE_r0 + 3, PANE_m0 + 3, PANE_f0 + 3 },
        { PANE_n0 + 4, PANE_r0 + 4, PANE_m0 + 4, PANE_f0 + 4 },
        { PANE_n0 + 5, PANE_r0 + 5, PANE_m0 + 5, PANE_f0 + 5 },
        { PANE_n0 + 6, PANE_r0 + 6, PANE_m0 + 6, PANE_f0 + 6 },
        { PANE_n0 + 7, PANE_r0 + 7, PANE_m0 + 7, PANE_f0 + 7 },
    };

    // The "1P" to "4P" labels of the human players.
    const unsigned char s_PadPane[4] = {
        PANE_player_1P, PANE_player_2P, PANE_player_3P, PANE_player_4P,
    };

    // The "COM" labels, one per character (copies of "com" after the first).
    const unsigned char s_ComPane[6] = {
        PANE_com, PANE_c1, PANE_c2, PANE_c3, PANE_c4, PANE_c5,
    };

    // The base icon of each team colour.
    const unsigned char s_TeamPane[4] = {
        PANE_BaseIcon_b, PANE_BaseIcon_r, PANE_BaseIcon_ball_b, PANE_BaseIcon_ball_r,
    };

    // The base icon of each puck.
    const unsigned char s_BallPane[1] = {
        PANE_BaseIcon_ball,
    };

    // SendEvent: pParam nonzero shows the layout.
    const long EVENT_SET_VISIBLE = 10001;

    // The resource file of the cockpit that holds the layout.
    const long RES_FILE = 2;

    // Values of m_nStep.
    enum {
        STEP_NONE = 0,
        STEP_SET = 1,
        STEP_RUN = 2
    };

    // Destroys a pane made at run time, if any.
    inline void DeletePane(nw4r::lyt::Pane *pPane)
    {
        if (pPane != 0) {
            nw4r::lyt::Layout::DeleteObj(pPane);
        }
    }
}

msw::CCockpitHockeyMap::CCockpitHockeyMap(long nId)
    : CCockpitHockeyLayoutBase(nId)
{
    m_nStep = STEP_NONE;
    m_fMapWidth = 1.0f;
    m_fMapHeight = 1.0f;
}

msw::CCockpitHockeyMap::~CCockpitHockeyMap()
{
    Remove();
}

void msw::CCockpitHockeyMap::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == STEP_SET) {
        m_nStep = STEP_RUN;
    }
}

void msw::CCockpitHockeyMap::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);
}

int msw::CCockpitHockeyMap::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor == 0) {
        return -2;
    }

    bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

    if (pPaneCtrl == 0) {
        return -3;
    }

    pPaneCtrl->BuildLayout(0, s_LayoutName);
    pPaneCtrl->BuildAnim(0, s_AnimName, ANIM_NUM);
    SetPaneAccessor(pPaneCtrl, s_PaneName, PANE_NUM);
    pPaneCtrl->SetSortZ(0.0f);

    nw4r::lyt::Pane *pBase = m_pPaneAccessor->GetPane(PANE_minimap_base);

    if (pBase != 0) {
        nw4r::lyt::Size size = pBase->GetSize();

        m_fMapWidth = size.width;
        m_fMapHeight = size.height;

        long nChrNum = CChrMan::GetChrCoreNum();

        for (long i = 0; i < nChrNum; i++) {
            CreatePlayerMarker(i, pBase, pPaneCtrl);
        }
        CreateBallMarker(pBase, pPaneCtrl);
        m_pPaneAccessor->SetPaneDisp(PANE_pic_goal_b_waku, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_pic_goal_r_waku, 0);
    }
    m_nStep = STEP_SET;
    return 0;
}

void msw::CCockpitHockeyMap::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = STEP_NONE;
}

long msw::CCockpitHockeyMap::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_VISIBLE) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

int msw::CCockpitHockeyMap::ScoreOut()
{
    return 0;
}

int msw::CCockpitHockeyMap::ScoreIn()
{
    return 0;
}

int msw::CCockpitHockeyMap::ScoreOutWait()
{
    return 0;
}

int msw::CCockpitHockeyMap::ScoreInWait()
{
    return 0;
}

long msw::CCockpitHockeyMap::CreatePlayerMarker(long nPlayer, nw4r::lyt::Pane *pParent,
                                                bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    if (pParent == 0 || pPaneCtrl == 0) {
        return -1;
    }

    CChrCore *pChr = CChrMan::GetChrCore(nPlayer);

    if (pChr == 0 || pChr->IsDisable() || pChr->IsKeeper()) {
        return -2;
    }

    const unsigned char *pPane = s_PlayerPane[nPlayer];
    const char *pNullName = s_PaneName[pPane[MARKER_NULL]];
    const char *pRotName = s_PaneName[pPane[MARKER_ROT]];
    const char *pBaseName = s_PaneName[pPane[MARKER_BASE]];
    const char *pFaceName = s_PaneName[pPane[MARKER_FACE]];

    nw4r::lyt::Pane *pNull = bcs::G2D::CreateNullPane(pNullName);
    nw4r::lyt::Pane *pRot = bcs::G2D::CreateNullPane(pRotName);
    nw4r::lyt::Pane *pBase = bcs::G2D::DuplicatePane(
        pPaneCtrl, m_pPaneAccessor->GetPane(s_TeamPane[CGame::GetTeamColorIdx(pChr->GetTeamNo())]), pBaseName,
        s_LayoutName, 0);

    nw4r::lyt::Pane *pFace;
    const _GXTexObj *pTexObj = pChr->GetMinimapIconTexObj();

    if (pTexObj == 0) {
        pFace = 0;
    } else {
        bcs::G2D::CTexMap texMap(*pTexObj);

        pFace = bcs::G2D::CreatePicturePane(pFaceName, texMap);
        if (pFace != 0) {
            // The result is not used (CRadarIcon2DHockey resizes Mii
            // pictures at this point).
            CGame::GetCharType(pChr->GetPlayerNo());
        }
    }

    // The "1P" to "4P" label of a human player.
    nw4r::lyt::Pane *pPadLabel = 0;

    if (pChr->IsPadCtrl()) {
        pPadLabel = m_pPaneAccessor->GetPane(s_PadPane[pChr->GetCtrlPlayerNo()]);
    }

    // The "COM" label, made from "com" the first time.
    unsigned char nComPane = s_ComPane[nPlayer];
    nw4r::lyt::Pane *pComLabel = m_pPaneAccessor->GetPane(nComPane);

    if (pComLabel == 0) {
        pComLabel = bcs::G2D::DuplicatePane(pPaneCtrl, m_pPaneAccessor->GetPane(PANE_com), s_PaneName[nComPane],
                                            s_LayoutName, 0);
    }

    if (pNull == 0 || pRot == 0 || pBase == 0 || pFace == 0) {
        DeletePane(pNull);
        DeletePane(pRot);
        DeletePane(pBase);
        DeletePane(pFace);
        DeletePane(pPadLabel);
        DeletePane(pComLabel);
        return -10;
    }

    pNull->AppendChild(pRot);
    pNull->AppendChild(pFace);
    pRot->AppendChild(pBase);
    pParent->AppendChild(pNull);
    pNull->SetVisible(true);
    pRot->SetVisible(true);
    pBase->SetVisible(true);
    pFace->SetVisible(true);

    if (pPadLabel != 0) {
        bcs::G2D::SetPaneParent(pPadLabel, pNull, 1, 0);
        pPadLabel->SetVisible(true);
    }
    if (pComLabel != 0) {
        bcs::G2D::SetPaneParent(pComLabel, pNull, 1, 0);
        pComLabel->SetVisible(pPadLabel == 0);
    }
    return 0;
}

long msw::CCockpitHockeyMap::CreateBallMarker(nw4r::lyt::Pane *pParent, bcs::G2D::CPaneCtrl *pPaneCtrl)
{
    // static-order swap (notes/decisions.md): the game stores the local
    // constants of this function and of SetCameraPosition in the reverse
    // of their declaration order (`len` before `ballNo`); every use is
    // folded, so only the .rodata order differs.
    static const long ballNo = 0;

    if (pParent == 0 || pPaneCtrl == 0) {
        return -1;
    }

    CBall *pBall = CBallMan::GetBallPtr(ballNo);

    if (pBall == 0 || !pBall->IsActive()) {
        return -2;
    }

    const char *pNullName = s_PaneName[PANE_n9];
    const char *pRotName = s_PaneName[PANE_r9];
    const char *pFaceName = s_PaneName[PANE_b9];

    nw4r::lyt::Pane *pNull = bcs::G2D::CreateNullPane(pNullName);
    nw4r::lyt::Pane *pRot = bcs::G2D::CreateNullPane(pRotName);
    nw4r::lyt::Pane *pFace;
    const _GXTexObj *pTexObj = static_cast<const _GXTexObj *>(pBall->GetMinimapIconTexObj());

    if (pTexObj == 0) {
        pFace = 0;
    } else {
        bcs::G2D::CTexMap texMap(*pTexObj);

        pFace = bcs::G2D::CreatePicturePane(pFaceName, texMap);
    }

    if (pNull == 0 || pRot == 0 || pFace == 0) {
        DeletePane(pNull);
        DeletePane(pRot);
        DeletePane(pFace);
        return -10;
    }

    pNull->AppendChild(pRot);
    pNull->AppendChild(pFace);
    pParent->AppendChild(pNull);

    {
        static const long len = 1;

        for (long i = 0; i < len; i++) {
            nw4r::lyt::Pane *pBase = m_pPaneAccessor->GetPane(s_BallPane[i]);

            bcs::G2D::SetPaneParent(pBase, pRot, 1, 0);
            pBase->SetVisible(true);
        }
    }
    return 0;
}

void msw::CCockpitHockeyMap::SetCameraPosition(float fPos)
{
    static const long rootPaneId = PANE_minimap_base;

    if (m_pPaneAccessor == 0) {
        return;
    }

    bcs::G2D::CPaneObj *pRoot = m_pPaneAccessor->GetPaneObj(rootPaneId);

    if (pRoot->IsAnimPlay(ANIM_MAP_00) || pRoot->IsAnimPlay(ANIM_MAP_02) || pRoot->IsAnimPlay(ANIM_MAP_03)) {
        return;
    }

    {
        static const long paneId = PANE_pic_base;
        static const long animId = ANIM_BASE;
        static const int recursive = 0;
        static const int reset = 1;
        static const float speed = 0.0f;

        if (fPos > 1.0f) {
            fPos = 1.0f;
        } else if (fPos < -1.0f) {
            fPos = -1.0f;
        }
        // The animation is 10 frames long, from -1 to 1.
        m_pPaneAccessor->PlayAnimation(paneId, animId, speed, 10.0f * ((1.0f + fPos) * 0.5f), recursive, reset);
    }
}

void msw::CCockpitHockeyMap::SetPlayerPosition(long nPlayer, float fX, float fY, float fAngle, long nPad,
                                               long nTeamColor)
{
    const unsigned char *pPane = s_PlayerPane[nPlayer];

    float fWidth = m_fMapWidth;
    float fHeight = m_fMapHeight;

    // Positions are in -1 to 1 across the map.
    fX = (fX > 1.0f) ? 1.0f : ((fX < -1.0f) ? -1.0f : fX);
    fY = (fY > 1.0f) ? 1.0f : ((fY < -1.0f) ? -1.0f : fY);

    nw4r::lyt::Pane *pNull = m_pPaneAccessor->GetPane(pPane[MARKER_NULL]);
    nw4r::lyt::Pane *pRot = m_pPaneAccessor->GetPane(pPane[MARKER_ROT]);

    if (pNull == 0 || pRot == 0) {
        return;
    }

    nw4r::math::VEC3 trans(0.5f * fWidth * fX, 0.5f * fHeight * fY, 0.0f);
    nw4r::math::VEC3 rot(0.0f, 0.0f, (180.0f / 3.14159265f) * fAngle);

    pNull->SetTranslate(trans);
    pRot->SetRotate(rot);

    // A human player's label moves to the marker he now controls; the
    // "COM" label shows on the marker he left and hides on the new one.
    if ((unsigned long)nPad <= 3) {
        nw4r::lyt::Pane *pPadLabel = m_pPaneAccessor->GetPane(s_PadPane[nPad]);

        if (pPadLabel != 0 && pPadLabel->GetParent() != pNull) {
            SetVisibleChildCom(pPadLabel->GetParent()->GetChildList(), 1);
            SetVisibleChildCom(pNull->GetChildList(), 0);
            bcs::G2D::SetPaneParent(pPadLabel, pNull, 1, 0);
            pPadLabel->SetVisible(true);
        }
    }

    if (nTeamColor < 0) {
        return;
    }

    nw4r::lyt::Pane *pBase = m_pPaneAccessor->GetPane(pPane[MARKER_BASE]);

    if (pBase == 0) {
        return;
    }

    nw4r::lyt::Pane *pSrc = m_pPaneAccessor->GetPane(s_TeamPane[nTeamColor]);

    if (pSrc != 0) {
        bcs::G2D::ReplacePanePicture(pBase, pSrc);
    }
}

inline void msw::CCockpitHockeyMap::SetVisibleChildCom(nw4r::ut::LinkList<nw4r::lyt::Pane, 4> &rList, int bVisible)
{
    static const long tblLen = 6;

    for (nw4r::ut::LinkList<nw4r::lyt::Pane, 4>::Iterator it = rList.GetBeginIter(); it != rList.GetEndIter(); ++it) {
        const char *pName = (*it).GetName();

        for (long i = 0; i < tblLen; i++) {
            if (strcmp(pName, s_PaneName[s_ComPane[i]]) == 0) {
                (*it).SetVisible(bVisible);
            }
        }
    }
}

void msw::CCockpitHockeyMap::SetPlayerDisp(long nPlayer, int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(s_PlayerPane[nPlayer][MARKER_NULL], bDisp);
}

void msw::CCockpitHockeyMap::SetBallDisp(long, int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(PANE_n9, bDisp);
}

void msw::CCockpitHockeyMap::SetDispEnable(int bEnable)
{
    if (GetLytObj() != 0) {
        GetLytObj()->SetVisible(bEnable);
    }
}

void msw::CCockpitHockeyMap::SetDirection(int bReverse)
{
    nw4r::lyt::Pane *pGoal = m_pPaneAccessor->GetPane(PANE_goal);

    if (pGoal != 0) {
        nw4r::math::VEC3 rot(0.0f, 0.0f, 0.0f);

        if (bReverse) {
            rot.z = 180.0f;
        }
        pGoal->SetRotate(rot);
    }
}

void msw::CCockpitHockeyMap::SetGoalTarget(long nTarget)
{
    int bBlue;
    int bRed;

    switch (nTarget) {
        case 0:
            bBlue = 1;
            bRed = 0;
            break;

        case 1:
            bBlue = 0;
            bRed = 1;
            break;

        default:
            bBlue = 0;
            bRed = 0;
            break;
    }
    m_pPaneAccessor->SetPaneDisp(PANE_pic_goal_b_waku, bBlue);
    m_pPaneAccessor->SetPaneDisp(PANE_pic_goal_r_waku, bRed);
}
