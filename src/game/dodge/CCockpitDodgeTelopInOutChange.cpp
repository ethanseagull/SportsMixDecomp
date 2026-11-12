// The dodge game's telops and signs: msw::CSignBoardDodge (the game's
// boards), msw::COutSignDodge (an unused "out" sign with a match point
// mark), msw::CCockpitDodgeTelopInOutChange (a player going in or out of
// the court), msw::CCockpitDodgeTelopOut (the "out" telop with the
// character's name) and msw::CCockpitDodgeTelopCourtIn (the names of the
// players entering the court).
#include "game/msw/CSignBoardDodge.hpp"
#include "game/msw/COutSignDodge.hpp"
#include "game/msw/CCockpitDodgeTelopInOutChange.hpp"
#include "game/msw/CCockpitDodgeTelopOut.hpp"
#include "game/msw/CCockpitDodgeTelopCourtIn.hpp"

#include <wchar.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CLayoutObj.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "nw4r/lyt/Pane.hpp"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // Stand-in for an unknown debug-only call (notes/decisions.md).
    static inline void Nothing() {}

    // Events of the cockpit telops' SendEvent.
    enum {
        EVENT_OUT = 10002,     // play the out animation
        EVENT_WAIT_OUT = 10003 // pParam != 0: go out by itself after the loop
    };

    // Sub-steps of the telops' loop step (m_nState).
    enum {
        STATE_START = 1,
        STATE_IN,
        STATE_LOOP = 4,
        STATE_OUT
    };
}

// ---- CSignBoardDodge ----------------------------------------------------

msw::CSignBoardDodge::CSignBoardDodge(long nId, long nType, float fSortZ)
    : CSignBoard(nId, nType, fSortZ)
{
}

msw::CSignBoardDodge::~CSignBoardDodge()
{
    // The original stores the vtables here; see notes/decisions.md, "Empty
    // inline call in a destructor".
    Nothing();
}

long msw::CSignBoardDodge::SendEvent(long nEvent, void *pParam)
{
    CSignBoard::SendEvent(nEvent, pParam);
    return 0;
}

// ---- COutSignDodge ------------------------------------------------------

namespace {

    // The archive and the layout of the sign, and its animation. Only the
    // static initialiser below reads any of them; nothing in DodgeF makes
    // the sign.
    const char *s_OutSignLayout[2] = { "arc", "ck_out.brlyt" };
    const char *s_OutSignAnimName[1] = { "ck_out.brlan" };
    const float s_OutSignAnimSpeed[2] = { 1.0f, 1.0f };
    const char *s_MatchPointPane[1] = { "matchpoint" };
    // One picture per player, shown by event 3.
    const char *s_PlayerPicPane[4] = { "player1_pic", "player2_pic", "player3_pic", "player4_pic" };

    // (guessed) The original fills a word of .bss with the animation's name
    // in its static initialiser; nothing reads it.
    static const char *s_pOutSignAnimName = s_OutSignAnimName[0];

    // Events of COutSignDodge::SendEvent.
    enum {
        OUTSIGN_EVENT_PLAY = 1,       // play the sign's animation
        OUTSIGN_EVENT_MATCH_POINT,    // show the match point mark
        OUTSIGN_EVENT_PLAYER,         // pParam: show that player's picture (0 to 3)
        OUTSIGN_EVENT_SET_STEP        // pParam: the new m_nStep
    };

    // (guessed) Stand-in for the code that used the layout table; see
    // notes/decisions.md, "Unreferenced data kept by dead references".
    inline void UnusedReferences()
    {
        (void)s_OutSignLayout;
    }
}

// (body guessed) Nothing calls it; the original module link dropped it.
msw::COutSignDodge::COutSignDodge(long nId)
    : CMenuLinkBase(nId)
{
    m_nStep = 0;
}

msw::COutSignDodge::~COutSignDodge()
{
    // As in ~CSignBoardDodge.
    Nothing();
}

void msw::COutSignDodge::Main()
{
    if (m_nStep == 1) {
        bcs::G2D::CLayoutObj *pObj = GetLytObj();
        int bStop = (pObj == 0) ? 1 : pObj->IsAnimStop();

        if (bStop == 1) {
            m_Unk08 = 2;
        }
    }
}

long msw::COutSignDodge::SendEvent(long nEvent, void *pParam)
{
    long nResult = 0;

    UnusedReferences();

    switch (nEvent) {
        case OUTSIGN_EVENT_PLAY: {
            bcs::G2D::CLayoutObj *pObj = GetLytObj();

            if (pObj == 0) {
                nResult = -2;
                break;
            }
            pObj->PlayAnim(1, s_OutSignAnimSpeed[1], 0.0f);
            nResult = 0;
            break;
        }

        case OUTSIGN_EVENT_MATCH_POINT: {
            bcs::G2D::CLayoutObj *pObj = GetLytObj();

            if (pObj != 0) {
                nw4r::lyt::Pane *pRoot = pObj->GetLayout()->GetRootPane();

                if (pRoot != 0) {
                    nw4r::lyt::Pane *pPane = pRoot->FindPaneByName(s_MatchPointPane[0], true);

                    if (pPane != 0) {
                        pPane->SetVisible(true);
                    }
                }
            }
            break;
        }

        case OUTSIGN_EVENT_PLAYER: {
            bcs::G2D::CLayoutObj *pObj = GetLytObj();
            long nPlayer = (long)pParam;

            if (pObj != 0 && nPlayer < 4) {
                nw4r::lyt::Pane *pRoot = pObj->GetLayout()->GetRootPane();

                if (pRoot != 0) {
                    nw4r::lyt::Pane *pPane = pRoot->FindPaneByName(s_PlayerPicPane[nPlayer], true);

                    if (pPane != 0) {
                        pPane->SetVisible(true);
                    }
                }
            }
            break;
        }

        case OUTSIGN_EVENT_SET_STEP:
            m_nStep = (long)pParam;
            break;
    }
    return nResult;
}

// ---- CCockpitDodgeTelopInOutChange --------------------------------------

namespace {

    enum {
        INOUT_ANIM_COLOR_0, // the two team colours
        INOUT_ANIM_COLOR_1,
        INOUT_ANIM_IN,
        INOUT_ANIM_LOOP,
        INOUT_ANIM_OUT,
        INOUT_ANIM_NUM
    };

    enum {
        INOUT_PANE_BAR_OUT,
        INOUT_PANE_NAME_OUT,
        INOUT_PANE_BAR_IN,
        INOUT_PANE_NAME_IN,
        INOUT_PANE_COLOR_OUT,
        INOUT_PANE_COLOR_IN,
        INOUT_PANE_NUM
    };

    const char *s_InOutLayoutName = "te_inout_change.brlyt";
    const char *s_InOutAnimName[INOUT_ANIM_NUM] = {
        "te_inout_change_Co_00.brlan", "te_inout_change_Co_01.brlan", "te_inout_change_Te_00.brlan",
        "te_inout_change_Te_01.brlan", "te_inout_change_Te_02.brlan",
    };
    const char *s_InOutPaneName[INOUT_PANE_NUM] = {
        "BAR_out", "NAME_out", "BAR_in", "NAME_in", "t_c_out", "t_c_in",
    };

    // The bar, name and text panes of each mode (0: in, 1: out).
    const unsigned char s_InOutModePane[2][3] = {
        { INOUT_PANE_BAR_IN, INOUT_PANE_NAME_IN, INOUT_PANE_COLOR_IN },
        { INOUT_PANE_BAR_OUT, INOUT_PANE_NAME_OUT, INOUT_PANE_COLOR_OUT },
    };
    // The colour animation of each team colour.
    const unsigned char s_InOutColorAnim[2] = { INOUT_ANIM_COLOR_0, INOUT_ANIM_COLOR_1 };

    // The message ("IN") shown after the names when a player comes in.
    const char *s_InOutMessageFile = "te_inout_change";
    const char *s_InOutMessageLabel = "t_IN";
}

msw::CCockpitDodgeTelopInOutChange::CCockpitDodgeTelopInOutChange(long nId, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
{
    m_nStep = 1;
    m_nState = STATE_START;
    Set();
}

msw::CCockpitDodgeTelopInOutChange::~CCockpitDodgeTelopInOutChange()
{
    Remove();
}

int msw::CCockpitDodgeTelopInOutChange::Set()
{
    static const int isDisp = 0;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = CCockpit::GetResourceFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_InOutLayoutName);
            pPaneCtrl->BuildAnim(0, s_InOutAnimName, INOUT_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_InOutPaneName, INOUT_PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(m_fSortZ);

            // Hide both bars and names until SetParameter picks one.
            m_pPaneAccessor->SetPaneDisp(INOUT_PANE_BAR_OUT, isDisp);
            m_pPaneAccessor->SetPaneDisp(INOUT_PANE_NAME_OUT, isDisp);
            m_pPaneAccessor->SetPaneDisp(INOUT_PANE_BAR_IN, isDisp);
            m_pPaneAccessor->SetPaneDisp(INOUT_PANE_NAME_IN, isDisp);
            pPaneCtrl->PlayAnim(INOUT_ANIM_IN, 1.0f, 0.0f, 1, 1);
        }
    }
    return 0;
}

void msw::CCockpitDodgeTelopInOutChange::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitDodgeTelopInOutChange::Main()
{
    switch (m_nStep) {
        case 1:
            if (GetLytObj() == 0) {
                m_Unk08 = 2;
                break;
            }
            m_nStep = 2;
            // fall through
        case 2: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            switch (m_nState) {
                case STATE_START:
                    m_nState = STATE_IN;
                    break;
                case STATE_IN:
                    if (pPaneCtrl->IsAnimEnd(INOUT_ANIM_IN)) {
                        pPaneCtrl->PlayAnim(INOUT_ANIM_LOOP, 1.0f, 0.0f, 1, 1);
                        m_nState = STATE_LOOP;
                    }
                    break;
                case STATE_LOOP:
                    if (IsWaitFrameOut() == 1) {
                        pPaneCtrl->PlayAnim(INOUT_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                        m_nState = STATE_OUT;
                    }
                    break;
                case STATE_OUT:
                    if (pPaneCtrl->IsAnimEnd(INOUT_ANIM_OUT)) {
                        m_Unk08 = 2;
                    }
                    break;
                default:
                    m_Unk08 = 2;
                    break;
            }
            break;
        }
        case 3:
            m_Unk08 = 2;
            break;
    }
}

void msw::CCockpitDodgeTelopInOutChange::Disp()
{
}

long msw::CCockpitDodgeTelopInOutChange::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(INOUT_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nState = STATE_OUT;
            }
            break;
        }
        case EVENT_WAIT_OUT:
            SetWaitFrameOut(pParam != 0);
            break;
    }
    return 0;
}

void msw::CCockpitDodgeTelopInOutChange::SetParameter(unsigned long nIdx, long nMode, const wchar_t *pText0,
                                                      const wchar_t *pText1)
{
    // static-order swap (notes/decisions.md): the original declares
    // recursive, reset, nameBufLength and stores them in reverse.
    static const long nameBufLength = 64;
    static const int recursive = 1;
    static const int reset = 1;

    long nBarPane = s_InOutModePane[nMode][0];
    long nNamePane = s_InOutModePane[nMode][1];
    long nTextPane = s_InOutModePane[nMode][2];
    long nAnim = s_InOutColorAnim[nIdx];

    bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

    if (pPaneCtrl != 0) {
        pPaneCtrl->PlayAnim(nAnim, 1.0f, 0.0f, recursive, reset);
    }

    wchar_t buf[nameBufLength];
    int nLen;

    if (nMode == 0) {
        menu::CMessage *pMessage = menu::CMessage::GetInstance();
        const wchar_t *pIn =
            pMessage->GetTextByLabel(pMessage->GetFileIndex(s_InOutMessageFile), s_InOutMessageLabel);

        if (pText0 != 0 && pText1 != 0) {
            nLen = swprintf(buf, nameBufLength, L"%ls %ls %ls", pText0, pText1, pIn);
        } else if (pText0 != 0) {
            nLen = swprintf(buf, nameBufLength, L"%ls %ls", pText0, pIn);
        } else if (pText1 != 0) {
            nLen = swprintf(buf, nameBufLength, L"%ls %ls", pText1, pIn);
        } else {
            nLen = swprintf(buf, nameBufLength, L"Name is Empty. %ls", pIn);
        }
    } else {
        if (pText0 != 0 && pText1 != 0) {
            nLen = swprintf(buf, nameBufLength, L"%ls %ls", pText0, pText1);
        } else if (pText0 != 0) {
            nLen = swprintf(buf, nameBufLength, L"%ls", pText0);
        } else if (pText1 != 0) {
            nLen = swprintf(buf, nameBufLength, L"%ls", pText1);
        } else {
            nLen = swprintf(buf, nameBufLength, L"Name is Empty.");
        }
    }

    if (nLen > 0) {
        buf[nLen] = 0;
        m_pPaneAccessor->SetTextBoxString(nTextPane, buf, wcslen(buf));
        m_pPaneAccessor->SetPaneDisp(nBarPane, 1);
        m_pPaneAccessor->SetPaneDisp(nNamePane, 1);
    }
}

// ---- CCockpitDodgeTelopOut ----------------------------------------------

namespace {

    enum {
        OUT_ANIM_OUT_0, // the "out" animation of each type
        OUT_ANIM_OUT_1,
        OUT_ANIM_VIS_0,
        OUT_ANIM_VIS_1,
        OUT_ANIM_COLOR_0, // the two team colours
        OUT_ANIM_COLOR_1,
        OUT_ANIM_NUM
    };

    enum {
        OUT_PANE_PLAYER,
        OUT_PANE_NAME,
        OUT_PANE_NAME_SHADOW,
        OUT_PANE_NUM
    };

    // (null-terminated)
    const char *s_OutLayoutName[2] = { "TE_out.brlyt", 0 };
    const char *s_OutAnimName[OUT_ANIM_NUM] = {
        "TE_out_Uo_00.brlan", "TE_out_Uo_03.brlan",  "TE_out_Vis_00.brlan",
        "TE_out_Vis_01.brlan", "TE_out_Co_00.brlan", "TE_out_Co_01.brlan",
    };
    // The animation of each type.
    const unsigned char s_OutTypeAnim[2] = { OUT_ANIM_OUT_0, OUT_ANIM_OUT_1 };
    const char *s_OutPaneName[OUT_PANE_NUM] = { "player_NULL", "t_c00", "ts_c00_kage" };
    // The colour animation of each team colour.
    const unsigned char s_OutColorAnim[2] = { OUT_ANIM_COLOR_0, OUT_ANIM_COLOR_1 };

    // SetAppendTableParam's entries.
    enum {
        OUT_PARAM_CHR,
        OUT_PARAM_COLOR,
        OUT_PARAM_NUM
    };
}

msw::CCockpitDodgeTelopOut::CCockpitDodgeTelopOut(long nId, long nType, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
{
    m_nType = nType;
    m_nStep = 1;
    for (long i = 0; i < OUT_PARAM_NUM; i++) {
        SetAppendTableParam(i, 0);
    }
}

msw::CCockpitDodgeTelopOut::~CCockpitDodgeTelopOut()
{
    Remove();
}

int msw::CCockpitDodgeTelopOut::Set()
{
    // static-order swap (notes/decisions.md): the original declares these
    // in the opposite order (reset, recursive, ..., nameBufLength) and
    // stores them in reverse.
    static const long nameBufLength = 32;
    static const int recursiveColor = 1;
    static const int resetColor = 0;
    static const int recursiveName = 1;
    static const int resetName = 1;
    static const int recursive = 1;
    static const int reset = 1;

    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = GetCockpit();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = CCockpit::GetResourceFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            long nType = m_nType;

            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_OutLayoutName[0]);
            pPaneCtrl->BuildAnim(0, s_OutAnimName, OUT_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_OutPaneName, OUT_PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(m_fSortZ);
            pPaneCtrl->PlayAnim(s_OutTypeAnim[nType], 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(OUT_PANE_PLAYER, OUT_ANIM_VIS_0, 1.0f, 0.0f, recursive, reset);

            long nChr = m_nParam[OUT_PARAM_CHR];

            if (CGame::GetCharNameLen(nChr) > 0) {
                m_pPaneAccessor->PlayAnimation(OUT_PANE_NAME_SHADOW, OUT_ANIM_VIS_1, 1.0f, 0.0f, recursiveName,
                                               resetName);
                m_pPaneAccessor->PlayAnimation(OUT_PANE_NAME_SHADOW, s_OutColorAnim[m_nParam[OUT_PARAM_COLOR]], 1.0f,
                                               0.0f, recursiveColor, resetColor);

                wchar_t name[nameBufLength];

                CGame::GetCharName(nChr, name, nameBufLength);
                m_pPaneAccessor->SetTextBoxString(OUT_PANE_NAME, name, wcslen(name));
                m_pPaneAccessor->SetTextBoxString(OUT_PANE_NAME_SHADOW, name, wcslen(name));
            }
        }
    }
    return 0;
}

void msw::CCockpitDodgeTelopOut::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
}

void msw::CCockpitDodgeTelopOut::Main()
{
    switch (m_nStep) {
        case 1:
            Set();
            m_nStep = 2;
            break;
        case 2: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl == 0 || pPaneCtrl->IsAnimEnd(s_OutTypeAnim[m_nType])) {
                m_Unk08 = 2;
            }
            break;
        }
    }
}

void msw::CCockpitDodgeTelopOut::Disp()
{
}

long msw::CCockpitDodgeTelopOut::SendEvent(long, void *)
{
    return 0;
}

void msw::CCockpitDodgeTelopOut::SetAppendTableParam(unsigned long nIndex, short nValue)
{
    if (nIndex < OUT_PARAM_NUM) {
        m_nParam[nIndex] = nValue;
    }
}

// ---- CCockpitDodgeTelopCourtIn ------------------------------------------

namespace {

    enum {
        COURTIN_ANIM_IN,
        COURTIN_ANIM_OUT,
        COURTIN_ANIM_LOOP,
        COURTIN_ANIM_NUM
    };

    enum {
        COURTIN_PANE_SHADOW_1,
        COURTIN_PANE_SHADOW_2,
        COURTIN_PANE_NAME_1,
        COURTIN_PANE_NAME_2,
        COURTIN_PANE_NUM
    };

    const char *s_CourtInLayoutName = "TE_cha_in.brlyt";
    // (null-terminated)
    const char *s_CourtInAnimName[COURTIN_ANIM_NUM + 1] = {
        "TE_cha_in_Te_00.brlan", "TE_cha_in_Te_01.brlan", "TE_cha_in_Te_02.brlan", 0,
    };
    const char *s_CourtInPaneName[COURTIN_PANE_NUM] = {
        "ts_cha01_kage", "ts_cha02_kage", "t_cha01", "t_cha02",
    };

    // The panes of each team colour.
    const unsigned char s_CourtInShadowPane[2] = { COURTIN_PANE_SHADOW_1, COURTIN_PANE_SHADOW_2 };
    const unsigned char s_CourtInNamePane[2] = { COURTIN_PANE_NAME_1, COURTIN_PANE_NAME_2 };
}

msw::CCockpitDodgeTelopCourtIn::CCockpitDodgeTelopCourtIn(long nId, float fSortZ)
    : CCockpitLayoutBase(nId, fSortZ)
    , CCockpitFrameOutBase(1.0f)
{
    m_nStep = 1;
    m_nState = STATE_START;
    Set();
}

msw::CCockpitDodgeTelopCourtIn::~CCockpitDodgeTelopCourtIn()
{
    Remove();
}

int msw::CCockpitDodgeTelopCourtIn::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pArchiveName = CCockpit::GetResourceFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pArchiveName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_CourtInLayoutName);
            pPaneCtrl->BuildAnim(0, s_CourtInAnimName, COURTIN_ANIM_NUM);
            SetPaneAccessor(pPaneCtrl, s_CourtInPaneName, COURTIN_PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(m_fSortZ);
            pPaneCtrl->PlayAnim(COURTIN_ANIM_IN, 1.0f, 0.0f, 1, 1);
        }
    }
    return 0;
}

void msw::CCockpitDodgeTelopCourtIn::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;
    m_nState = 0;
}

void msw::CCockpitDodgeTelopCourtIn::Main()
{
    switch (m_nStep) {
        case 1:
            if (GetLytObj() == 0) {
                m_Unk08 = 2;
                break;
            }
            m_nStep = 2;
            // fall through
        case 2: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            switch (m_nState) {
                case STATE_START:
                    m_nState = STATE_IN;
                    break;
                case STATE_IN:
                    if (pPaneCtrl->IsAnimEnd(COURTIN_ANIM_IN)) {
                        pPaneCtrl->PlayAnim(COURTIN_ANIM_LOOP, 1.0f, 0.0f, 1, 1);
                        m_nState = STATE_LOOP;
                    }
                    break;
                case STATE_LOOP:
                    if (IsWaitFrameOut() == 1) {
                        pPaneCtrl->PlayAnim(COURTIN_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                        m_nState = STATE_OUT;
                    }
                    break;
                case STATE_OUT:
                    if (pPaneCtrl->IsAnimEnd(COURTIN_ANIM_OUT)) {
                        m_Unk08 = 2;
                    }
                    break;
                default:
                    m_Unk08 = 2;
                    break;
            }
            break;
        }
        case 3:
            m_Unk08 = 2;
            break;
    }
}

void msw::CCockpitDodgeTelopCourtIn::Disp()
{
}

long msw::CCockpitDodgeTelopCourtIn::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_OUT: {
            bcs::G2D::CPaneCtrl *pPaneCtrl = GetLytObj();

            if (pPaneCtrl != 0) {
                pPaneCtrl->PlayAnim(COURTIN_ANIM_OUT, 1.0f, 0.0f, 1, 1);
                m_nState = STATE_OUT;
            }
            break;
        }
        case EVENT_WAIT_OUT:
            SetWaitFrameOut(pParam != 0);
            break;
    }
    return 0;
}

void msw::CCockpitDodgeTelopCourtIn::SetParameter(CCockpit *pCockpit, long nObjId, unsigned long nTeam,
                                                  const wchar_t *pName)
{
    if (pName != 0) {
        CCockpitDodgeTelopCourtIn *pTelop = (CCockpitDodgeTelopCourtIn *)pCockpit->FindObj(nObjId);

        if (pTelop != 0) {
            long nColor = CGame::GetTeamColorIdx(nTeam);

            pTelop->m_pPaneAccessor->SetPaneDisp(s_CourtInShadowPane[nColor], 1);
            pTelop->m_pPaneAccessor->SetTextBoxString(s_CourtInNamePane[nColor], pName, wcslen(pName));
        }
    }
}
