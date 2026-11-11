#include "game/msw/SelectMenu/CBackground.hpp"

#include <string.h>
#include <wchar.h>

#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CLytManager.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CScissorDL.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "nw4r/lyt/layout.h"
#include "nw4r/ut/RuntimeTypeInfo.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    enum {
        ANIM_BG_TAG_IN,
        ANIM_BG_TAG_LOOP,
        ANIM_BG_TAG_OFF,
        ANIM_BG_TAG_OUT,
        ANIM_TAG_COL_00,
        ANIM_TAG_COL_BA,
        ANIM_TAG_COL_DO,
        ANIM_TAG_COL_HO,
        ANIM_TAG_COL_VO,
        ANIM_TEXT_BOX_00,
        ANIM_TEXT_BOX_01,
        ANIM_TEXT_BOX_02,
        ANIM_TEXT_BOX_03,
        ANIM_TEXT_BOX_04,
        ANIM_TEXT_LINE_00,
        ANIM_TEXT_LINE_01,
        ANIM_TEXT_LINE_02,
        ANIM_TEXT_SCROLL,
        ANIM_TEXT_SCROLL_01,
        ANIM_TEXT_SCROLL2,
        ANIM_TEXT_SCROLL2_01,
        ANIM_BG_TAG_FI,
        ANIM_BG_TAG_FO,
        ANIM_BG_TAG_HL,
        ANIM_TAG_COL_PARTY,
        ANIM_TAG_COL_SCOREBOOK,
        ANIM_TAG_COL_WIFI,
        ANIM_SUBTITLE_00,
        ANIM_SUBTITLE_01,
        ANIM_SUBTITLE_02,
        ANIM_SUBTITLE_03,
        ANIM_TEXT_LINE_02_B, // the same file as ANIM_TEXT_LINE_02
        ANIM_NUM,

        // Negative values stop the line or text animations.
        ANIM_LINE_OFF = -10,
        ANIM_TEXT_OFF = -20
    };

    enum {
        PANE_BG_TAG_INOUT,
        PANE_T_BG_TAG_00,
        PANE_LINE_CTL,
        PANE_TEXT_CTL_FIX,
        PANE_TEXT_CTL_SCROLL,
        PANE_T_BG_LINE_00,
        PANE_T_BG_LINE_01,
        PANE_TEXT_RANGE_NULL,
        PANE_S00,
        PANE_S01,
        PANE_S02,
        PANE_BG_TAG_POS,
        PANE_TV_L,
        PANE_SCROLL_ANIM,
        PANE_SCROLL_ANIM2,
        PANE_ANZEN_RANGE,
        PANE_BASE_BAR,
        PANE_BASKET_BAR,
        PANE_VOLLEY_BAR,
        PANE_DODGE_BAR,
        PANE_HOCKEY_BAR,
        PANE_PARTY_BAR,
        PANE_SCORE_BOOK_BAR,
        PANE_WIFI_BAR,
        PANE_MIX_BAR,
        PANE_BASKET_ICON,
        PANE_VOLLEY_ICON,
        PANE_DODGE_ICON,
        PANE_HOCKEY_ICON,
        PANE_PARTY_ICON,
        PANE_SCORE_BOOK_ICON,
        PANE_WIFI_ICON,
        PANE_MIX_ICON,
        PANE_NUM,
        PANE_NONE = -1
    };

    // What the screens send (SendEvent).
    enum {
        EVENT_TAG_TEXT,      // pParam: the tag's text, null hides the tag
        EVENT_LINE_TEXT,     // pParam: the scrolling line's text
        EVENT_TAG_BAR,       // pParam: the colour bar (TAG_*)
        EVENT_TAG_IN,
        EVENT_TAG_OUT,
        EVENT_TAG_LOOP,
        EVENT_TAG_OFF,
        EVENT_LINE_BOX_00,   // line box animations (the ones with text set it again)
        EVENT_LINE_BOX_02,
        EVENT_LINE_BOX_01,
        EVENT_LINE_OFF,
        EVENT_LINE_BOX_03,
        EVENT_LINE_BOX_04,
        EVENT_TEXT_LINE_00,
        EVENT_TEXT_LINE_02,
        EVENT_TEXT_LINE_01,
        EVENT_TEXT_OFF,
        EVENT_TEXT_CHANGE    // pParam: a new text for the line, null for none
    };

    // The colour bars, TAG_NUM of them.
    enum {
        TAG_BASE,
        TAG_NUM = 9
    };

    const char *const s_LayoutName = "me_bg.brlyt";

    const char *s_AnimNames[ANIM_NUM] = {
        "me_bg_bg_tag_in.brlan",
        "me_bg_bg_tag_loop.brlan",
        "me_bg_bg_tag_off.brlan",
        "me_bg_bg_tag_out.brlan",
        "me_bg_tag_col_00.brlan",
        "me_bg_tag_col_BA.brlan",
        "me_bg_tag_col_DO.brlan",
        "me_bg_tag_col_HO.brlan",
        "me_bg_tag_col_VO.brlan",
        "me_bg_text_box_00.brlan",
        "me_bg_text_box_01.brlan",
        "me_bg_text_box_02.brlan",
        "me_bg_text_box_03.brlan",
        "me_bg_text_box_04.brlan",
        "me_bg_text_line_00.brlan",
        "me_bg_text_line_01.brlan",
        "me_bg_text_line_02.brlan",
        "me_bg_text_scroll.brlan",
        "me_bg_text_scroll_01.brlan",
        "me_bg_text_scroll2.brlan",
        "me_bg_text_scroll2_01.brlan",
        "me_bg_bg_tag_FI.brlan",
        "me_bg_bg_tag_FO.brlan",
        "me_bg_bg_tag_HL.brlan",
        "me_bg_tag_col_party.brlan",
        "me_bg_tag_col_scorebook.brlan",
        "me_bg_teg_col_WiFi.brlan",
        "me_bg_subtitle_00.brlan",
        "me_bg_subtitle_01.brlan",
        "me_bg_subtitle_02.brlan",
        "me_bg_subtitle_03.brlan",
        "me_bg_text_line_02.brlan"
    };

    const char *s_PaneNames[PANE_NUM] = {
        "bg_tag_inout",
        "t_bg_tag_00",
        "line_ctl",
        "text_ctl_fix",
        "text_ctl_scroll",
        "t_bg_line_00",
        "t_bg_line_01",
        "text_range_null",
        "s00",
        "s01",
        "s02",
        "bg_tag_pos",
        "tv_L",
        "scroll_anim",
        "scroll_anim2",
        "anzen_range",
        "Base_bar",
        "Basket_bar",
        "Volley_bar",
        "Dodge_bar",
        "Hockey_bar",
        "Party_bar",
        "Score_book_bar",
        "WiFi_bar",
        "Mix_bar",
        "basket_icon",
        "volley_icon",
        "dodge_icon",
        "hockey_icon",
        "party_icon",
        "score_book_icon",
        "WiFi_icon",
        "Mix_icon"
    };

    // The bar pane and the icon pane of each colour bar.
    const long s_TagPanes[TAG_NUM][2] = {
        { PANE_BASE_BAR, PANE_NONE },
        { PANE_BASKET_BAR, PANE_BASKET_ICON },
        { PANE_VOLLEY_BAR, PANE_VOLLEY_ICON },
        { PANE_DODGE_BAR, PANE_DODGE_ICON },
        { PANE_HOCKEY_BAR, PANE_HOCKEY_ICON },
        { PANE_PARTY_BAR, PANE_PARTY_ICON },
        { PANE_SCORE_BOOK_BAR, PANE_SCORE_BOOK_ICON },
        { PANE_WIFI_BAR, PANE_WIFI_ICON },
        { PANE_MIX_BAR, PANE_MIX_ICON }
    };

    // Put in front of the line's text.
    const wchar_t *s_LineHead = L"\x25CF ";

    // (no symbol; name guessed) A new scissor pane called pName, not marked
    // as allocated by the user; null when the allocation fails.
    inline bcs::G2D::CScissorDL *CreateScissorPane(const char *pName)
    {
        bcs::G2D::CScissorDL *pPane = nw4r::lyt::Layout::NewObj<bcs::G2D::CScissorDL>();
        if (pPane != 0) {
            pPane->SetName(pName);
            pPane->SetUserAllocated(false);
        }
        return pPane;
    }
}

namespace {

    // (no symbol; name guessed) The text box of a pane object.
    inline bcs::G2D::CTextBoxDL *GetTextBox(bcs::G2D::CPaneObj *pObj)
    {
        return nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());
    }

    // (no symbol; name guessed) Copies pSrc (an empty string when null)
    // into pDst; true, without copying, when it is already there.
    bool CopyText(wchar_t *pDst, const wchar_t *pSrc, unsigned long nLen)
    {
        if (pSrc != 0) {
            if (wcsncmp(pDst, pSrc, nLen) == 0) {
                return true;
            }
            wcsncpy(pDst, pSrc, nLen);
        } else {
            wcsncpy(pDst, L"", nLen);
        }
        return false;
    }
}

msw::SelectMenu::CBackground::CBackground(long nId)
    : CSelectMenuLayoutBase(nId)
    , m_RangeSize(0.0f, 0.0f)
{
    m_Unk2C = 0;
    memset(m_wLineText, 0, sizeof(m_wLineText));
    m_RangeSize = nw4r::lyt::Size(0.0f, 0.0f);
    m_nLineAnim = -1;
}

msw::SelectMenu::CBackground::~CBackground()
{
    Remove();
}

void msw::SelectMenu::CBackground::Main()
{
    // While the line scrolls, jump the scroll back once the text has gone
    // past its left end.
    bcs::G2D::CPaneObj *pScroll = m_pPaneAccessor->GetPaneObj(CLytCtrl::IsLocationAdjust() ? PANE_SCROLL_ANIM : PANE_SCROLL_ANIM2);
    if (pScroll != 0) {
        long nAnim = CLytCtrl::IsLocationAdjust() ? ANIM_TEXT_SCROLL : ANIM_TEXT_SCROLL2;
        if (pScroll->IsAnimPlay(nAnim)) {
            nw4r::math::VEC3 pos = pScroll->GetPane()->GetTranslate();
            if (-pos.x > m_fTextWidth) {
                pScroll->SetAnimCurrentFrame(nAnim, CLytCtrl::IsLocationAdjust() ? 777.0f : 739.0f);
            }
        }
    }

    bcs::G2D::CPaneObj *pText = m_pPaneAccessor->GetPaneObj(CLytCtrl::IsLocationAdjust() ? PANE_TEXT_CTL_SCROLL : PANE_TEXT_CTL_FIX);
    if (pText != 0 && pText->IsAnimPlay(ANIM_TEXT_LINE_02_B) && pText->IsAnimEnd(ANIM_TEXT_LINE_02_B)) {
        if (m_wLineText[0] == 0) {
            pText->PlayAnimation(ANIM_TEXT_OFF, 1.0f, 0.0f, 0, 1, 1);
        } else {
            SetLineTextString(m_wLineText);
            pText->PlayAnimation(ANIM_TEXT_LINE_00, 1.0f, 0.0f, 0, 1, 1);
            pText->PushNextAnimation(ANIM_TEXT_LINE_01, 1.0f, 0.0f, 0, 1);
        }
    }
}

void msw::SelectMenu::CBackground::Disp()
{
}

int msw::SelectMenu::CBackground::Set()
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
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            SetPaneAccessor(pPaneCtrl, s_PaneNames, PANE_NUM);
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(0.0f);
            pPaneCtrl->BuildAnim(0, s_AnimNames, ANIM_NUM);

            static const int recursive = 1;
            static const int reset = 1;
            m_pPaneAccessor->PlayAnimation(PANE_BG_TAG_INOUT, ANIM_TAG_COL_00, 1.0f, 0.0f, recursive, reset);

            // Three scissor panes: one the size of "text_range_null", the
            // other two the size of the screen.
            bcs::G2D::CScissorDL *pScissorText = CreateScissorPane(s_PaneNames[PANE_S00]);
            if (pScissorText != 0) {
                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_TEXT_RANGE_NULL);
                if (pParent != 0) {
                    pParent->AppendChild(pScissorText);
                    bcs::G2D::SetPaneSizeFromParent(pScissorText);
                    pScissorText->SetVisible(true);
                    pParent->SetVisible(true);
                }
            }

            GXRenderModeObj *pRenderMode = bcs::CMainApp::GetRenderModeObj();
            nw4r::lyt::Size screenSize(pRenderMode->fbWidth, pRenderMode->efbHeight);

            bcs::G2D::CScissorDL *pScissorTv = CreateScissorPane(s_PaneNames[PANE_S01]);
            if (pScissorTv != 0) {
                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(PANE_TV_L);
                if (pParent != 0) {
                    pParent->AppendChild(pScissorTv);
                    pScissorTv->SetSize(screenSize);
                    pScissorTv->SetVisible(true);
                    pParent->SetVisible(true);
                }
            }

            bcs::G2D::CScissorDL *pScissorRoot = CreateScissorPane(s_PaneNames[PANE_S02]);
            if (pScissorRoot != 0) {
                nw4r::lyt::Pane *pRoot = pPaneCtrl->GetLayout()->GetRootPane();
                if (pRoot != 0) {
                    pRoot->AppendChild(pScissorRoot);
                    pScissorRoot->SetSize(screenSize);
                    pScissorRoot->SetVisible(true);
                }
            }

            m_pPaneAccessor->PlayAnimation(PANE_BASE_BAR, ANIM_SUBTITLE_02, 1.0f, 0.0f, 1, 0);
            for (long i = TAG_BASE + 1; i < TAG_NUM; i++) {
                m_pPaneAccessor->PlayAnimation(s_TagPanes[i][0], ANIM_SUBTITLE_00, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(s_TagPanes[i][1], ANIM_SUBTITLE_00, 1.0f, 0.0f, 1, 0);
            }

            m_nTagNo = -1;
            m_fTextWidth = 0.0f;

            nw4r::lyt::Pane *pRange = m_pPaneAccessor->GetPane(PANE_ANZEN_RANGE);
            bcs::G2D::CLytManager *pLytManager = bcs::G2D::CLytManager::GetInstance();
            if (pRange != 0 && pLytManager != 0) {
                nw4r::lyt::Size size = pRange->GetSize();
                if (CLytCtrl::IsLocationAdjust()) {
                    size.width /= pLytManager->GetLocationAdjustScaleH(1);
                }
                m_RangeSize = size;
            }

            m_pPaneAccessor->PlayAnimation(PANE_BG_TAG_INOUT, ANIM_BG_TAG_OFF, 1.0f, 0.0f, 1, 1);
            SetLineAnim(ANIM_LINE_OFF);

            m_Unk2C = 1;
            m_Unk30 = STATE_NONE;
        }
    }
    return 0;
}

void msw::SelectMenu::CBackground::Remove()
{
    if (m_pPaneAccessor != 0) {
        DeletePaneAccessor();
        DeleteLytObj();
        DeleteArcFont();
        m_Unk2C = 0;
        m_Unk30 = STATE_NONE;
    }
}

long msw::SelectMenu::CBackground::SendEvent(long nEvent, void *pParam)
{
    switch (nEvent) {
        case EVENT_TAG_TEXT: {
            const wchar_t *pText = (const wchar_t *)pParam;
            if (pText == 0) {
                m_pPaneAccessor->SetPaneDisp(PANE_BG_TAG_POS, 0);
            } else {
                m_pPaneAccessor->SetPaneDisp(PANE_BG_TAG_POS, 1);
                if (pText[0] == 0) {
                    m_pPaneAccessor->SetPaneDisp(PANE_T_BG_TAG_00, 0);
                } else {
                    m_pPaneAccessor->SetTextBoxString(PANE_T_BG_TAG_00, pText, wcslen(pText));
                    m_pPaneAccessor->SetPaneDisp(PANE_T_BG_TAG_00, 1);
                }
            }
            break;
        }
        case EVENT_LINE_TEXT: {
            const wchar_t *pText = (const wchar_t *)pParam;
            if (pText != 0) {
                memset(m_wLineText, 0, sizeof(m_wLineText));
                wcscpy(m_wLineText, pText);
            }
            SetLineTextString(pText);
            break;
        }
        case EVENT_TAG_BAR: {
            long nTagNo = (long)pParam;
            if (nTagNo != m_nTagNo) {
                if (m_nTagNo >= 0) {
                    const long *pOld = s_TagPanes[m_nTagNo];
                    m_pPaneAccessor->PlayAnimation(pOld[0], ANIM_SUBTITLE_03, 1.0f, 0.0f, 1, 0);
                    if (pOld[1] >= 0) {
                        m_pPaneAccessor->PlayAnimation(pOld[1], ANIM_SUBTITLE_03, 1.0f, 0.0f, 1, 0);
                    }
                }
                const long *pNew = s_TagPanes[nTagNo];
                m_pPaneAccessor->PlayAnimation(pNew[0], ANIM_SUBTITLE_01, 1.0f, 0.0f, 1, 0);
                if (pNew[1] >= 0) {
                    m_pPaneAccessor->PlayAnimation(pNew[1], ANIM_SUBTITLE_01, 1.0f, 0.0f, 1, 0);
                }
                m_nTagNo = nTagNo;
            }
            break;
        }
        case EVENT_TAG_IN:
            SetTagAnim(ANIM_BG_TAG_IN);
            break;
        case EVENT_TAG_LOOP:
            SetTagAnim(ANIM_BG_TAG_LOOP);
            break;
        case EVENT_TAG_OUT:
            SetTagAnim(ANIM_BG_TAG_OUT);
            break;
        case EVENT_TAG_OFF:
            SetTagAnim(ANIM_BG_TAG_OFF);
            break;
        case EVENT_LINE_BOX_00:
            SetLineAnim(ANIM_TEXT_BOX_00);
            SetLineTextString(m_wLineText);
            break;
        case EVENT_LINE_BOX_01:
            SetLineAnim(ANIM_TEXT_BOX_01);
            break;
        case EVENT_LINE_BOX_02:
            SetLineAnim(ANIM_TEXT_BOX_02);
            break;
        case EVENT_LINE_OFF:
            SetLineAnim(ANIM_LINE_OFF);
            break;
        case EVENT_LINE_BOX_03:
            SetLineAnim(ANIM_TEXT_BOX_03);
            SetLineTextString(m_wLineText);
            break;
        case EVENT_LINE_BOX_04:
            SetLineAnim(ANIM_TEXT_BOX_04);
            break;
        case EVENT_TEXT_LINE_00:
            SetTextAnim(ANIM_TEXT_LINE_00);
            break;
        case EVENT_TEXT_LINE_02:
            SetTextAnim(ANIM_TEXT_LINE_02);
            break;
        case EVENT_TEXT_LINE_01:
            SetTextAnim(ANIM_TEXT_LINE_01);
            break;
        case EVENT_TEXT_OFF:
            SetTextAnim(ANIM_TEXT_OFF);
            break;
        case EVENT_TEXT_CHANGE: {
            // The line's text for the next time it comes round; the line
            // is changed only when the text is new.
            if (!CopyText(m_wLineText, (const wchar_t *)pParam, 0x80)) {
                SetTextAnim(ANIM_TEXT_LINE_02_B);
            }
            break;
        }
    }
    return 1;
}

long msw::SelectMenu::CBackground::SetLineTextString(const wchar_t *pText)
{
    static const unsigned long STR_BUF_LENGTH = 0x200;
    static wchar_t strBuf[STR_BUF_LENGTH];

    if (pText == 0) {
        m_pPaneAccessor->SetPaneDisp(PANE_T_BG_LINE_00, 0);
        m_pPaneAccessor->SetPaneDisp(PANE_T_BG_LINE_01, 0);
        return -1;
    }

    bcs::G2D::CPaneObj *pLine0 = m_pPaneAccessor->GetPaneObj(PANE_T_BG_LINE_00);
    bcs::G2D::CPaneObj *pLine1 = m_pPaneAccessor->GetPaneObj(PANE_T_BG_LINE_01);

    unsigned long nLen = wcslen(pText);
    if (nLen >= STR_BUF_LENGTH - wcslen(s_LineHead)) {
        return -2;
    }
    swprintf(strBuf, STR_BUF_LENGTH, L"%ls%ls", s_LineHead, pText);

    if (pLine0 != 0) {
        GetTextBox(pLine0)->SetString(strBuf, 0);
        m_fTextWidth = GetTextBox(pLine0)->GetTextDrawRectNoLineFeed().GetWidth();

        float fScrollTime;
        if (bcs::CMainApp::GetRegionLanguage() == 0) {
            fScrollTime = 120.0f;
        } else {
            fScrollTime = 60.0f;
        }

        static const int recursive = 0;
        static const int reset = 1;

        // The scroll: the first animation, then, when the text is longer
        // than the range, the one that follows it.
        if (CLytCtrl::IsLocationAdjust()) {
            static const long scrollPaneId = PANE_SCROLL_ANIM;
            const long anim[2] = { ANIM_TEXT_SCROLL_01, ANIM_TEXT_SCROLL };
            bcs::G2D::CPaneObj *pScroll = m_pPaneAccessor->GetPaneObj(scrollPaneId);
            if (pScroll != 0) {
                if (m_fTextWidth < m_RangeSize.width) {
                    pScroll->PlayAnimation(anim[0], 1.0f, 0.0f, recursive, reset, 1);
                } else {
                    pScroll->PlayAnimation(anim[0], 1.0f / fScrollTime, 0.0f, recursive, reset, 1);
                    pScroll->PushNextAnimation(anim[1], 1.0f, 0.0f, recursive, reset);
                }
            }
            if (pLine1 != 0) {
                GetTextBox(pLine1)->SetString(strBuf, 0);
                pLine1->GetPane()->SetVisible(true);
            }
            pLine0->GetPane()->SetVisible(false);
        } else {
            static const long scrollPaneId = PANE_SCROLL_ANIM2;
            const long anim[2] = { ANIM_TEXT_SCROLL2_01, ANIM_TEXT_SCROLL2 };
            bcs::G2D::CPaneObj *pScroll = m_pPaneAccessor->GetPaneObj(scrollPaneId);
            if (pScroll != 0) {
                if (m_fTextWidth < m_RangeSize.width) {
                    pScroll->PlayAnimation(anim[0], 1.0f, 0.0f, recursive, reset, 1);
                } else {
                    pScroll->PlayAnimation(anim[0], 1.0f / fScrollTime, 0.0f, recursive, reset, 1);
                    pScroll->PushNextAnimation(anim[1], 1.0f, 0.0f, recursive, reset);
                }
            }
            if (pLine1 != 0) {
                pLine1->GetPane()->SetVisible(false);
            }
            GetTextBox(pLine0)->SetString(strBuf, 0);
            pLine0->GetPane()->SetVisible(true);
        }
    }
    return 0;
}

inline void msw::SelectMenu::CBackground::SetTagAnim(long nAnim)
{
    // The game numbers these statics recursive, reset@0 ... reset@4 and
    // stores them in the reverse order; the bytes differ here (the unit's
    // .rodata cannot match for SetLineTextString anyway, see notes).
    if (nAnim == ANIM_BG_TAG_IN) {
        static const int recursive = 1;
        static const int reset = 1;
        m_pPaneAccessor->PlayAnimation(PANE_BG_TAG_INOUT, nAnim, 1.0f, 0.0f, recursive, reset);
        // The highlight, without a reset.
        static const int recursiveHL = 1;
        static const int resetHL = 0;
        m_pPaneAccessor->PlayAnimation(PANE_BG_TAG_INOUT, ANIM_BG_TAG_HL, 1.0f, 0.0f, recursiveHL, resetHL);
    } else {
        static const int recursive = 1;
        static const int reset = 1;
        m_pPaneAccessor->PlayAnimation(PANE_BG_TAG_INOUT, nAnim, 1.0f, 0.0f, recursive, reset);
    }
}

void msw::SelectMenu::CBackground::SetLineAnim(long nAnim)
{
    if (nAnim == m_nLineAnim) {
        return;
    }
    m_nLineAnim = nAnim;

    if (nAnim >= ANIM_TEXT_BOX_00 && nAnim <= ANIM_TEXT_BOX_04) {
        static const int recursive = 1;
        static const int reset = 0;
        // Faster on screen 4.
        CSelectMenu *pSelectMenu = GetSelectMenu();
        if (pSelectMenu->m_nMode == 4) {
            m_pPaneAccessor->PlayAnimation(PANE_LINE_CTL, nAnim, 4.0f, 0.0f, recursive, reset);
        } else {
            m_pPaneAccessor->PlayAnimation(PANE_LINE_CTL, nAnim, 1.0f, 0.0f, recursive, reset);
        }
    } else {
        bcs::G2D::CPaneObj *pLine = m_pPaneAccessor->GetPaneObj(PANE_LINE_CTL);
        if (pLine != 0) {
            pLine->StopAllAnimation(1);
            nw4r::lyt::Pane *pPane = pLine->GetPane();
            pPane->SetVisible(false);
        }
    }
}

void msw::SelectMenu::CBackground::SetTextAnim(long nAnim)
{
    switch (nAnim) {
        case ANIM_TEXT_LINE_00: {
            bcs::G2D::CPaneObj *pLine = m_pPaneAccessor->GetPaneObj(PANE_LINE_CTL);
            if (pLine != 0) {
                static const int recursive = 1;
                static const int reset = 1;
                pLine->PlayAnimation(nAnim, 1.0f, 0.0f, recursive, reset, 1);
                pLine->PushNextAnimation(ANIM_TEXT_LINE_01, 1.0f, 0.0f, recursive, reset);
            }
            break;
        }
        case ANIM_TEXT_LINE_01: {
            static const int recursive = 1;
            static const int reset = 1;
            m_pPaneAccessor->PlayAnimation(PANE_LINE_CTL, nAnim, 1.0f, 0.0f, recursive, reset);
            break;
        }
        case ANIM_TEXT_LINE_02: {
            bcs::G2D::CPaneObj *pText = m_pPaneAccessor->GetPaneObj(CLytCtrl::IsLocationAdjust() ? PANE_TEXT_CTL_SCROLL : PANE_TEXT_CTL_FIX);
            if (pText != 0 && !pText->IsAnimPlay(nAnim)) {
                static const int recursive = 1;
                static const int reset = 1;
                m_pPaneAccessor->PlayAnimation(PANE_LINE_CTL, nAnim, 1.0f, 0.0f, recursive, reset);
            }
            break;
        }
        case ANIM_TEXT_LINE_02_B: {
            bcs::G2D::CPaneObj *pText = m_pPaneAccessor->GetPaneObj(CLytCtrl::IsLocationAdjust() ? PANE_TEXT_CTL_SCROLL : PANE_TEXT_CTL_FIX);
            if (pText != 0 && !pText->IsAnimPlay(nAnim)) {
                static const int recursive = 1;
                static const int reset = 1;
                m_pPaneAccessor->PlayAnimation(PANE_LINE_CTL, nAnim, 1.0f, 0.0f, recursive, reset);
            }
            break;
        }
        default: {
            bcs::G2D::CPaneObj *pLine = m_pPaneAccessor->GetPaneObj(PANE_LINE_CTL);
            if (pLine != 0) {
                pLine->StopAllAnimation(1);
            }
            m_pPaneAccessor->SetPaneDisp(PANE_TEXT_CTL_FIX, 0);
            m_pPaneAccessor->SetPaneDisp(PANE_TEXT_CTL_SCROLL, 0);
            break;
        }
    }
}
