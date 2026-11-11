// Open: the original .data has the vtables in the order CCharaLevelStar,
// CCharaLevel, CCharaOk (reverse definition order, as in the module's other
// multi-class files); this source gives CCharaOk, CCharaLevelStar,
// CCharaLevel, and neither the include order nor the declaration order of
// CCharaOk's virtuals changes that.
#include "game/msw/SelectMenu/CCharaOk.hpp"
#include "game/msw/SelectMenu/CCharaLevel.hpp"
#include "game/msw/SelectMenu/CCharaLevelStar.hpp"

#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuOption.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CSave.hpp"
#include "wchar.h"

// The names in this file's unnamed namespaces are guesses; the objects have
// no symbols.
namespace {

    // [0] is the speed of the level windows' opening and closing
    // animations; [1] is not read here. A table, not a scalar, as in
    // game/menu/CTeam.cpp: the original converts the value to float at run
    // time and keeps both words, first in .rodata.
    const long s_WinAnimSpeed[2] = { 2, 0 };

    // What the menu task's object 1 is sent.
    enum {
        EVENT_PUSH = 0x21 // plus the pad number
    };

    // The state after an arrow was pushed: waits for the arrows' animation
    // before greying them out again.
    enum {
        STATE_ARROW = 6
    };
}

// ---------------------------------------------------------------------------
// CCharaOk
// ---------------------------------------------------------------------------

namespace {

    enum {
        ANIM_BTN_00,
        ANIM_BTN_01,
        ANIM_BTN_02,
        ANIM_BTN_03,
        ANIM_BTN_04,
        ANIM_INOUT_00,
        ANIM_INOUT_01,
        ANIM_INOUT_02,
        ANIM_INOUT_03,
        ANIM_SCROLL,
        ANIM_OK_NUM
    };

    enum {
        PANE_WINDOW_S,
        PANE_NO,
        PANE_YES,
        PANE_B_NO_BTN_00,
        PANE_B_YES_BTN_00,
        PANE_T_WINDOW_00,
        PANE_T_NO_BTN,
        PANE_T_YES_BTN_00,
        PANE_OK_NUM
    };

    enum {
        OK_BUTTON_NUM = 2
    };

    // The kind ids the yes and no buttons lead to.
    enum {
        BTN_KIND_NO = 0x4E,
        BTN_KIND_YES = 0x4F
    };

    const char *const s_OkLayoutName = "me_cha_OK.brlyt";

    const char *s_OkAnimNames[ANIM_OK_NUM] = {
        "me_cha_OK_btn_00.brlan",
        "me_cha_OK_btn_01.brlan",
        "me_cha_OK_btn_02.brlan",
        "me_cha_OK_btn_03.brlan",
        "me_cha_OK_btn_04.brlan",
        "me_cha_OK_inout_00.brlan",
        "me_cha_OK_inout_01.brlan",
        "me_cha_OK_inout_02.brlan",
        "me_cha_OK_inout_03.brlan",
        "me_cha_OK_scroll.brlan"
    };

    const char *s_OkPaneNames[PANE_OK_NUM] = {
        "Window_s",
        "NO",
        "YES",
        "B_no_btn_00",
        "B_yes_btn_00",
        "t_Window_00",
        "t_no_btn",
        "t_yes_btn_00"
    };

    // Size 0x14. The constant part of a button's description.
    struct OkButtonParam {
        unsigned char m_nBtnKindId;         // 0x00
        unsigned char m_nHitPane;           // 0x01
        unsigned short m_nNextBtnKindId[3]; // 0x02, to [0], [3] and [4]
        long m_nSeCode[2];                  // 0x08
        long m_Unk10;                       // 0x10, not read
    };

    const OkButtonParam s_OkButtonParam[OK_BUTTON_NUM] = {
        { 2, PANE_B_YES_BTN_00, { BTN_KIND_YES, BTN_KIND_YES, BTN_KIND_NO }, { 0xBC0, 0xBBE }, 0 },
        { 1, PANE_B_NO_BTN_00, { BTN_KIND_NO, BTN_KIND_YES, BTN_KIND_NO }, { 0xBC0, 0xBBD }, 0 }
    };

    // Null when there is no such button.
    inline const OkButtonParam *GetOkButtonParam(long nButton)
    {
        return nButton < OK_BUTTON_NUM ? &s_OkButtonParam[nButton] : 0;
    }
}

msw::SelectMenu::CCharaOk::CCharaOk(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CCharaOk::~CCharaOk()
{
    Remove();
}

void msw::SelectMenu::CCharaOk::Main()
{
    switch (m_Unk30) {
        case STATE_SET:
            m_Unk30 = STATE_IN;
            return;
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WINDOW_S, ANIM_INOUT_00)) {
                m_pPaneAccessor->PlayAnimation(PANE_WINDOW_S, ANIM_INOUT_01, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(PANE_WINDOW_S, ANIM_SCROLL, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            return;
        case STATE_ACTIVE:
            m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                m_pPaneAccessor->PlayAnimation(PANE_WINDOW_S, ANIM_INOUT_02, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(PANE_NO, ANIM_INOUT_02, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(PANE_YES, ANIM_INOUT_02, 1.0f, 0.0f, 1, 1);
                m_Unk30 = STATE_OUT;
            }
            return;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WINDOW_S, ANIM_INOUT_02)) {
                m_pPaneAccessor->PlayAnimation(PANE_WINDOW_S, ANIM_INOUT_03, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(PANE_NO, ANIM_INOUT_03, 1.0f, 0.0f, 1, 1);
                m_pPaneAccessor->PlayAnimation(PANE_YES, ANIM_INOUT_03, 1.0f, 0.0f, 1, 1);
                m_Unk30 = STATE_NONE;
            }
            return;
        default:
            return;
    }

    // Only while the window is active.
    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CCharaOk::Disp()
{
}

int msw::SelectMenu::CCharaOk::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(1);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_OkLayoutName);
            pPaneCtrl->BuildAnim(0, s_OkAnimNames, ANIM_OK_NUM);
            SetPaneAccessor(pPaneCtrl, s_OkPaneNames, PANE_OK_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(140.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            static const int btnNum = OK_BUTTON_NUM;

            for (long i = 0; i < btnNum; i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);

                const OkButtonParam *pParam = GetOkButtonParam(i);

                pPart->m_nBtnKindId = pParam->m_nBtnKindId;
                pPart->m_Unk16 = pParam->m_nHitPane;
                pPart->m_nCurrentBtnKindId = pParam->m_nBtnKindId;
                pPart->m_nAnim[0] = ANIM_BTN_01;
                pPart->m_nAnim[1] = ANIM_BTN_02;
                pPart->m_nAnim[2] = ANIM_BTN_03;
                pPart->m_nAnim[3] = ANIM_BTN_00;
                pPart->m_nAnim[4] = ANIM_BTN_04;
                pPart->m_nAnim[5] = ANIM_INOUT_02;
                pPart->m_nNextBtnKindId[0] = pParam->m_nNextBtnKindId[0];
                pPart->m_nNextBtnKindId[3] = pParam->m_nNextBtnKindId[1];
                pPart->m_nNextBtnKindId[4] = pParam->m_nNextBtnKindId[2];
                pPart->m_nSeCode[0] = pParam->m_nSeCode[0];
                pPart->m_nSeCode[1] = pParam->m_nSeCode[1];
                pPart->m_nSeCode[2] = 1;
                pPart->m_bUnk40_0004 = 0;
                pPart->m_bUnk40_0020 = 1;
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            m_pPaneAccessor->PlayAnimation(PANE_WINDOW_S, ANIM_INOUT_00, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(PANE_NO, ANIM_INOUT_00, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(PANE_YES, ANIM_INOUT_00, 1.0f, 0.0f, 1, 1);
            m_pPaneAccessor->PlayAnimation(PANE_NO, ANIM_BTN_00, 1.0f, 0.0f, 1, 1);
        }

        const wchar_t *pMessage = 0;
        const wchar_t *pYesText;
        const wchar_t *pNoText;
        if (m_bCharaMode) {
            pMessage = GetMessageLabelString("ExhibitionMenuMessage", "CHA_OK_00");
            pNoText = GetMessageLabelString("ExhibitionMenuMessage", "CHA_OK_01");
            pYesText = GetMessageLabelString("ExhibitionMenuMessage", "CHA_OK_02");
        } else {
            pNoText = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_01");
            pYesText = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_02");
            switch (pSelectMenu->m_Unk9B) {
                case 0:
                    pMessage = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_00_00");
                    break;
                case 1:
                    pMessage = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_00_01");
                    break;
                case 2:
                    pMessage = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_00_02");
                    break;
                case 4:
                    if (pSelectMenu->m_bStarMii) {
                        pMessage = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_00_04");
                    } else {
                        pMessage = GetMessageLabelString("ExhibitionMenuMessage", "BG_OK_00_03");
                    }
                    break;
            }
        }
        m_pPaneAccessor->SetTextBoxString(PANE_T_WINDOW_00, pMessage, wcslen(pMessage));
        m_pPaneAccessor->SetTextBoxString(PANE_T_YES_BTN_00, pYesText, wcslen(pYesText));
        m_pPaneAccessor->SetTextBoxString(PANE_T_NO_BTN, pNoText, wcslen(pNoText));

        m_Unk2C = 1;
        m_Unk30 = STATE_SET;
        ClearPushButtonId();
    }
    return 0;
}

void msw::SelectMenu::CCharaOk::Remove()
{
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCharaOk::SendEvent(long nEvent, void *pParam)
{
    return -1;
}

long msw::SelectMenu::CCharaOk::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CCharaOk::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaOk::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaOk::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaOk::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCharaOk::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CCharaOk::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_NO;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CCharaOk::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CCharaOk::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        if (nBtnKindId == BTN_KIND_YES) {
            m_pPaneAccessor->PlayAnimation(PANE_YES, ANIM_BTN_00, 1.0f, 0.0f, 1, 1);
        } else {
            m_pPaneAccessor->PlayAnimation(PANE_NO, ANIM_BTN_00, 1.0f, 0.0f, 1, 1);
        }
        return 0;
    }
    return 0;
}

long msw::SelectMenu::CCharaOk::PushNext()
{
    return 0;
}

long msw::SelectMenu::CCharaOk::PushCancel()
{
    return 0;
}

// ---------------------------------------------------------------------------
// CCharaLevel and CCharaLevelStar
// ---------------------------------------------------------------------------

namespace {

    // The kind ids of the two windows' buttons.
    enum {
        BTN_KIND_LEFT = 0x8040,   // the arrows
        BTN_KIND_RIGHT = 0x8041,
        BTN_KIND_OK = 0x8045,
        BTN_KIND_SELECT = 0x8047  // the setting between the arrows
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_RIGHT = 0x02,
        PAD_LEFT = 0x08
    };

    // The arrow animations (names not in the module).
    enum {
        ANIM_ARROW_ON = 0,
        ANIM_ARROW_PUSH_1 = 1,
        ANIM_ARROW_PUSH_2 = 2,
        ANIM_ARROW_PUSH_3 = 3,
        ANIM_ARROW_WAIT = 4,
        ANIM_ARROW_NONE = 0xFF
    };

    const char *const s_LevelLayoutName = "me_cha_level.brlyt";
    const char *const s_LevelButtonFileName = "me_cha_level.btn";

    enum {
        // Window animations, by index into the names the button file lists.
        ANIM_LEVEL_IN = 0x0A,
        ANIM_LEVEL_LOOP = 0x0B,
        ANIM_LEVEL_OUT = 0x0C,
        ANIM_LEVEL_RULE = 0x0E,
        ANIM_STAR_ON = 0x12
    };

    enum {
        PANE_LEVEL_WIN = 0,
        PANE_LEVEL_RULE = 7,
        PANE_LEVEL_LEFT = 8,
        PANE_LEVEL_RIGHT = 10,
        PANE_LEVEL_TEXT_0 = 0x0C,
        PANE_LEVEL_TEXT_1 = 0x0D,
        PANE_LEVEL_TEXT_2 = 0x0E
    };
}

msw::SelectMenu::CCharaLevel::CCharaLevel(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CCharaLevel::~CCharaLevel()
{
    Remove();
}

void msw::SelectMenu::CCharaLevel::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_LEVEL_WIN, ANIM_LEVEL_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_LEVEL_WIN, ANIM_LEVEL_IN)) {
                m_pPaneAccessor->PlayAnimation(PANE_LEVEL_WIN, ANIM_LEVEL_LOOP, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_LEVEL_WIN, ANIM_LEVEL_RULE, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_ARROW:
            if (m_pPaneAccessor->IsAnimEnd(PANE_LEVEL_LEFT, ANIM_ARROW_WAIT) &&
                m_pPaneAccessor->IsAnimEnd(PANE_LEVEL_RIGHT, ANIM_ARROW_WAIT)) {
                SetArrowDisable(BTN_KIND_LEFT, m_bLeftDisable);
                SetArrowDisable(BTN_KIND_RIGHT, m_bRightDisable);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                m_pPaneAccessor->PlayAnimation(PANE_LEVEL_WIN, ANIM_LEVEL_OUT, s_WinAnimSpeed[0], 0.0f, 1, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_LEVEL_WIN, ANIM_LEVEL_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

// Inlined into Main(). Defined here because its local static follows
// Main()'s constants in .rodata.
inline void msw::SelectMenu::CCharaLevel::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CCharaLevel::Disp()
{
}

int msw::SelectMenu::CCharaLevel::Set()
{
    const wchar_t *pText;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_LevelButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->BuildLayout(0, s_LevelLayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(140.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                SetUpButton(pPart, i, 1);
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            pText = GetMessageLabelString("ExhibitionMenuMessage", "CHA_LV_00");
            m_pPaneAccessor->SetTextBoxString(PANE_LEVEL_TEXT_0, pText, wcslen(pText));
            pText = GetMessageLabelString("ExhibitionMenuMessage", "CHA_LV_02");
            m_pPaneAccessor->SetTextBoxString(PANE_LEVEL_TEXT_2, pText, wcslen(pText));
            pText = GetMessageLabelString("ExhibitionMenuMessage", "CHA_LV_01");
            m_pPaneAccessor->SetTextBoxString(PANE_LEVEL_TEXT_1, pText, wcslen(pText));

            SetRuleText(0);
            SetArrowDisable(BTN_KIND_LEFT, m_bLeftDisable);
            SetArrowDisable(BTN_KIND_RIGHT, m_bRightDisable);

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CCharaLevel::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCharaLevel::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CCharaLevel::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaLevel::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaLevel::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaLevel::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCharaLevel::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The stick moves from the arrows as from the setting between them.
    switch (nBtnKindId) {
        case BTN_KIND_LEFT:
        case BTN_KIND_RIGHT:
            nBtnKindId = BTN_KIND_SELECT;
            break;
    }
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, nInput);
    // Left and right on the setting push the arrows.
    if (nNext == BTN_KIND_SELECT) {
        if (nInput & PAD_RIGHT) {
            PushButton(nPad, BTN_KIND_RIGHT);
            SetRuleText(1);
        } else if (nInput & PAD_LEFT) {
            PushButton(nPad, BTN_KIND_LEFT);
            SetRuleText(-1);
        }
    }
    return nNext;
}

long msw::SelectMenu::CCharaLevel::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_OK;
}

int msw::SelectMenu::CCharaLevel::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CCharaLevel::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

long msw::SelectMenu::CCharaLevel::GetOptionKindId()
{
    CSelectMenu *pSelectMenu = GetSelectMenu();
    long nMode = pSelectMenu->m_Unk9B;
    // Left unset when nMode is 1: the caller then gets whatever is in the
    // return register (`this`).
    long nKind;
    if (nMode == 2) {
        switch (pSelectMenu->m_Unk9C) {
            default:
                nKind = 4;
                break;
            case 3:
                nKind = 6;
                break;
            case 5:
                nKind = 8;
                break;
            case 7:
                nKind = 10;
                break;
        }
    } else if (nMode != 1) {
        switch (pSelectMenu->m_nGameKind) {
            default:
                nKind = 0x11;
                break;
            case 1:
                nKind = 0x12;
                break;
            case 2:
                nKind = 0x13;
                break;
            case 3:
                nKind = 0x14;
                break;
        }
    }
    return nKind;
}

void msw::SelectMenu::CCharaLevel::SetRuleText(long nAdd)
{
    const wchar_t *pText;
    CMenuOption &rOption = CMenuOption::s_Instance;
    long nKind = GetOptionKindId();
    long nNum = rOption.GetMenuParameterNum(nKind, 0);
    long nId = rOption.GetMenuParameterId(nKind, 0) + nAdd;

    // Looked up and not used.
    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_LEFT);
    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_RIGHT);

    if (nId >= nNum - 1) {
        m_bLeftDisable = 0;
        m_bRightDisable = 1;
        nId = nNum - 1;
    } else if (nId <= 0) {
        m_bLeftDisable = 1;
        m_bRightDisable = 0;
        nId = 0;
    } else {
        m_bLeftDisable = 0;
        m_bRightDisable = 0;
    }
    m_fUnk5A8 = 10.0f;
    m_Unk30 = STATE_ARROW;

    rOption.SetMenuParameterId(nKind, 0, nId);
    pText = rOption.GetMenuParameterText(nKind, 0, nId);
    m_pPaneAccessor->SetTextBoxString(PANE_LEVEL_RULE, pText, wcslen(pText));
}

void msw::SelectMenu::CCharaLevel::SetArrowDisable(long nBtnKindId, int bDisable)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart == 0) {
        return;
    }

    if (bDisable) {
        pPart->m_nAnim[0] = ANIM_ARROW_ON;
        pPart->m_nAnim[1] = ANIM_ARROW_ON;
        pPart->m_nAnim[2] = ANIM_ARROW_ON;
        pPart->m_nAnim[3] = ANIM_ARROW_ON;
        pPart->m_nAnim[4] = ANIM_ARROW_ON;
        pPart->m_nAnim[5] = ANIM_ARROW_ON;
        pPart->m_nSeCode[0] = -1;
        pPart->m_nSeCode[1] = 0xBBE;
    } else {
        pPart->m_nAnim[0] = ANIM_ARROW_PUSH_1;
        pPart->m_nAnim[1] = ANIM_ARROW_PUSH_2;
        pPart->m_nAnim[2] = ANIM_ARROW_PUSH_3;
        pPart->m_nAnim[3] = ANIM_ARROW_ON;
        pPart->m_nAnim[4] = ANIM_ARROW_WAIT;
        pPart->m_nAnim[5] = ANIM_ARROW_NONE;
        pPart->m_nSeCode[0] = 0xBC0;
        pPart->m_nSeCode[1] = 0xBC1;
    }
}

namespace {

    const char *const s_StarLayoutName = "me_cha_level_star.brlyt";
    const char *const s_StarButtonFileName = "me_cha_level_star.btn";

    enum {
        PANE_STAR_WIN = 0,
        PANE_STAR_TEXT_LV_00 = 2,
        PANE_STAR_LEFT = 3,
        PANE_STAR_RIGHT = 5,
        PANE_STAR_TEXT_LV_01 = 9,
        PANE_STAR_TEXT_LV_02 = 0x0B,
        PANE_STAR_PICKUP = 0x16,
        PANE_STAR_TEXT_PICKUP = 0x17
    };

    enum {
        STAR_LEVEL_MAX = 3,
        STAR_LEVEL_NUM = STAR_LEVEL_MAX + 1,
        STAR_ROW_NUM = 5
    };

    // One row per level (the last one is not used): its stars, the pane of
    // its name, and the name's label.
    long s_StarPane[STAR_ROW_NUM] = { 0x0C, 0x0E, 0x10, 0x12, 0x14 };
    long s_StarTextPane[STAR_ROW_NUM] = { 0x0D, 0x0F, 0x11, 0x13, 0x15 };
    const char *s_StarTextLabel[STAR_ROW_NUM] = {
        "COM_LV_SEL_00",
        "COM_LV_SEL_01",
        "COM_LV_SEL_02",
        "COM_LV_SEL_03",
        0
    };
}

msw::SelectMenu::CCharaLevelStar::CCharaLevelStar(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
}

msw::SelectMenu::CCharaLevelStar::~CCharaLevelStar()
{
    Remove();
}

void msw::SelectMenu::CCharaLevelStar::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_STAR_WIN, ANIM_LEVEL_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_STAR_WIN, ANIM_LEVEL_IN)) {
                m_pPaneAccessor->PlayAnimation(PANE_STAR_WIN, ANIM_LEVEL_LOOP, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_STAR_WIN, ANIM_STAR_ON, 1.0f, 0.0f, 1, 0);
                m_Unk30 = STATE_ARROW;
            }
            break;
        case STATE_ACTIVE:
            CheckButtonAnim();
            break;
        case STATE_ARROW:
            if (m_pPaneAccessor->IsAnimEnd(PANE_STAR_LEFT, ANIM_ARROW_WAIT) &&
                m_pPaneAccessor->IsAnimEnd(PANE_STAR_RIGHT, ANIM_ARROW_WAIT)) {
                SetArrowDisable(BTN_KIND_LEFT, m_bLeftDisable);
                SetArrowDisable(BTN_KIND_RIGHT, m_bRightDisable);
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                m_pPaneAccessor->PlayAnimation(PANE_STAR_WIN, ANIM_LEVEL_OUT, s_WinAnimSpeed[0], 0.0f, 1, 0);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_STAR_WIN, ANIM_LEVEL_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

// Inlined into Main(), as in CCharaLevel.
inline void msw::SelectMenu::CCharaLevelStar::CheckButtonAnim()
{
    static const int backEvent = 0x20;

    m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
    GetSelectMenu()->SendObjEventTrigger(1, backEvent, m_nPushButtonId);
}

void msw::SelectMenu::CCharaLevelStar::Disp()
{
}

int msw::SelectMenu::CCharaLevelStar::Set()
{
    const wchar_t *pText;
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CSelectMenu *pSelectMenu = GetSelectMenu();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();
    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = pSelectMenu->GetArchiveFileName(pSelectMenu->GetLoadArchiveFileId(-1));
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pSelectMenu->GetArchiveRootFolderName(1));
    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, 0, 1);
        if (pPaneCtrl != 0) {
            ArchiveBuildLoader(s_StarButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_StarLayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(140.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                SetUpButton(pPart, i, 1);
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            for (long i = 0; i < STAR_ROW_NUM; i++) {
                m_pPaneAccessor->SetPaneDisp(s_StarPane[i], 0);
                if (s_StarTextLabel[i] != 0) {
                    pText = GetMessageLabelString("ExhibitionMenuMessage", s_StarTextLabel[i]);
                    m_pPaneAccessor->SetTextBoxString(s_StarTextPane[i], pText, wcslen(pText));
                }
            }

            m_Unk5B0 = 0;
            m_pPaneAccessor->SetPaneDisp(PANE_STAR_PICKUP, 0);

            pText = GetMessageLabelString("ExhibitionMenuMessage", "COM_LV_PICKUP");
            m_pPaneAccessor->SetTextBoxString(PANE_STAR_TEXT_PICKUP, pText, wcslen(pText));
            pText = GetMessageLabelString("ExhibitionMenuMessage", "COM_LV_00");
            m_pPaneAccessor->SetTextBoxString(PANE_STAR_TEXT_LV_00, pText, wcslen(pText));
            pText = GetMessageLabelString("ExhibitionMenuMessage", "COM_LV_01");
            m_pPaneAccessor->SetTextBoxString(PANE_STAR_TEXT_LV_01, pText, wcslen(pText));
            pText = GetMessageLabelString("ExhibitionMenuMessage", "COM_LV_02");
            m_pPaneAccessor->SetTextBoxString(PANE_STAR_TEXT_LV_02, pText, wcslen(pText));

            m_nLevel = CSave::GetPlayerData(-1)->IsDifficult(pSelectMenu->m_nGameKind);
            SetLevelText(0);
            SetArrowDisable(BTN_KIND_LEFT, m_bLeftDisable);
            SetArrowDisable(BTN_KIND_RIGHT, m_bRightDisable);

            m_Unk2C = 1;
            m_Unk30 = STATE_SET;
        }
    }
    return 0;
}

void msw::SelectMenu::CCharaLevelStar::Remove()
{
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CCharaLevelStar::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CCharaLevelStar::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaLevelStar::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaLevelStar::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CCharaLevelStar::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CCharaLevelStar::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The stick moves from the arrows as from the level between them.
    switch (nBtnKindId) {
        case BTN_KIND_LEFT:
        case BTN_KIND_RIGHT:
            nBtnKindId = BTN_KIND_SELECT;
            break;
    }
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    unsigned long nInput = CMenuPad::s_Instance.GetInput(nPad);
    long nNext = (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, nInput);
    // Left and right on the level push the arrows.
    if (nNext == BTN_KIND_SELECT) {
        if (nInput & PAD_RIGHT) {
            PushButton(nPad, BTN_KIND_RIGHT);
            SetLevelText(1);
        } else if (nInput & PAD_LEFT) {
            PushButton(nPad, BTN_KIND_LEFT);
            SetLevelText(-1);
        }
    }
    return nNext;
}

long msw::SelectMenu::CCharaLevelStar::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    return IsButtonKindOfStickMode(nBtnKindId) ? nBtnKindId : BTN_KIND_OK;
}

int msw::SelectMenu::CCharaLevelStar::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CCharaLevelStar::PushButton(long nPad, long nBtnKindId)
{
    if (m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad) >= 0) {
        return 0;
    }
    return GetSelectMenu()->SendObjEventTrigger(1, nPad + EVENT_PUSH, (void *)nBtnKindId);
}

void msw::SelectMenu::CCharaLevelStar::SetLevelText(long nAdd)
{
    // Looked up and not used.
    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_LEFT);
    m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_RIGHT);
    CSave::GetPlayerData(-1);

    long nOldLevel = m_nLevel;
    m_bLeftDisable = 0;
    m_bRightDisable = 0;
    m_nLevel = nOldLevel + nAdd;
    if (m_nLevel <= 0) {
        m_bLeftDisable = 1;
        m_bRightDisable = 0;
        m_nLevel = 0;
    }
    if (m_nLevel >= STAR_LEVEL_MAX) {
        m_bLeftDisable = 0;
        m_bRightDisable = 1;
        m_nLevel = STAR_LEVEL_MAX;
    }

    for (long i = 0; i < STAR_LEVEL_NUM; i++) {
        if (m_nLevel == i) {
            m_pPaneAccessor->SetPaneDisp(s_StarPane[i], 1);
        } else {
            m_pPaneAccessor->SetPaneDisp(s_StarPane[i], 0);
        }
    }

    if (nOldLevel != m_nLevel) {
        m_Unk30 = STATE_ARROW;
    }
}

void msw::SelectMenu::CCharaLevelStar::SetArrowDisable(long nBtnKindId, int bDisable)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    if (pPart == 0) {
        return;
    }

    if (bDisable) {
        pPart->m_nAnim[0] = ANIM_ARROW_ON;
        pPart->m_nAnim[1] = ANIM_ARROW_ON;
        pPart->m_nAnim[2] = ANIM_ARROW_ON;
        pPart->m_nAnim[3] = ANIM_ARROW_ON;
        pPart->m_nAnim[4] = ANIM_ARROW_ON;
        pPart->m_nAnim[5] = ANIM_ARROW_ON;
        pPart->m_nSeCode[0] = -1;
        pPart->m_nSeCode[1] = 0xBBE;
    } else {
        pPart->m_nAnim[0] = ANIM_ARROW_PUSH_1;
        pPart->m_nAnim[1] = ANIM_ARROW_PUSH_2;
        pPart->m_nAnim[2] = ANIM_ARROW_PUSH_3;
        pPart->m_nAnim[3] = ANIM_ARROW_ON;
        pPart->m_nAnim[4] = ANIM_ARROW_WAIT;
        pPart->m_nAnim[5] = ANIM_ARROW_NONE;
        pPart->m_nSeCode[0] = 0xBC0;
        pPart->m_nSeCode[1] = 0xBC1;
    }
}
