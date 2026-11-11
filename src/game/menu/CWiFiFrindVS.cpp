#include "game/msw/SelectMenu/CWiFiFrindVS.hpp"

#include "bcs/G2D/CPaneObj.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CMenuPartButton.hpp"
#include "game/msw/CPlayerInfoRender.hpp"
#include "game/msw/SelectMenu/CTrofy.hpp"
#include "game/msw/dwc/CFriend.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/MenuUtil.hpp"
#include "nw4r/lyt/Pane.hpp"
#include "revolution/gx/GXTexture.h"
#include "string.h"
#include "wchar.h"

// The names in this namespace are guesses; the objects have no symbols.
namespace {

    // The speed of the window's opening and closing animations. A table,
    // not a scalar, as in game/menu/CHowTo.cpp: the original converts the
    // value to float at run time and keeps the word, first in .rodata.
    const long s_WinAnimSpeed[1] = { 2 };

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_DWC_WINDOW = 0x3F, // what the Wi-Fi message window is sent through
        OBJ_TROFY = 0x4D
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_ACCEPT = 0x8010,
        BTN_KIND_REFUSE = 0x8011
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10
    };

    // Values of CSelectMenu::m_nStep while the screen is shown.
    enum {
        STEP_SELECT = 0,     // waiting for the player's answer
        STEP_BACK = 10,      // a message is shown; go back once it is closed
        STEP_WAIT = 20,      // accepted: waiting for the match to start
        STEP_ERROR = 22,     // the connection failed
        STEP_ERROR_MSG = 23,
        STEP_CLOSE = 24,
        STEP_START = 99      // the match is starting
    };

    // Values of bcs::dwc::CDwc::GetStat() tested here (names guessed).
    enum {
        DWC_STAT_READY = 4,
        DWC_STAT_ERROR = 10
    };

    // What CCockpit::GetResult() gives once the message window is closed.
    enum {
        WINDOW_CLOSED = 1000
    };

    // CCockpit object waited for before the error message.
    enum {
        COCKPIT_OBJ_ERROR = 0x1019
    };

    // The mode the friend match goes on to.
    enum {
        MODE_FRIEND_VS_WAIT = 0x28
    };

    // What the message window is sent (meaning not known).
    enum {
        EVENT_WINDOW_CLOSE = 0x32
    };

    // Sound effect played when the screen opens.
    enum {
        SE_FRIEND_VS = 0xBC4
    };
}

void msw::CSelectMenu::FriendVS_In()
{
    // `objId` is stored before `padNo0`; the game numbers them the other
    // way round, so only the two symbol names are exchanged: static-order
    // swap (see notes/decisions.md).
    static const long objId = 0x2B;
    static const long padNo0 = 0;

    if (CGamePadCheck::GetInstance() != 0) {
        CGamePadCheck::GetInstance()->m_UnkAC = 1;
    }
    m_pVsFriendSaved = m_pVsFriend;
    Build(objId);
    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[padNo0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(padNo0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[padNo0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(padNo0, nBtnKindId);
        CMenuPad::s_Instance.ChangeInputStickModeForce(0);
    }
    menu::CSndEff::s_Instance.Play(SE_FRIEND_VS, 1);
    m_nStep = STEP_SELECT;
}

void msw::CSelectMenu::FriendVS_Loop()
{
    // static-order swap, as in FriendVS_In().
    static const long objId = 0x2B;
    static const long padNo0 = 0;

    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(objId);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    long nStat = m_pGameDwc->m_Dwc.GetStat();

    switch (m_nStep) {
        case STEP_SELECT:
            if (nStat == DWC_STAT_READY) {
                // The friend may have withdrawn the request.
                if (m_pVsFriend != 0 && !m_pVsFriend->IsFriendMatchChallenger()) {
                    CancelVsFriend();
                }
                dwc::CFriend *pFriend = m_pVsFriend;
                if (pFriend == 0 || !pFriend->IsFriendMatchChallenger()) {
                    m_pGameDwc->WindowA_Button("WifiConnectMessage", "FENT_02_00", 0, 0, 1, 0);
                    m_nStep = STEP_BACK;
                } else {
                    long nBtnKindId = m_nCurrentBtnKindId[padNo0][nMode];
                    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
                        _GetModeChangeWaitTime();
                        pObj->PushButton(padNo0, nBtnKindId);
                        switch (nBtnKindId) {
                            case BTN_KIND_ACCEPT:
                                if (m_pGameDwc->m_Dwc.EntryFriendMatch(pFriend->m_nIndex, m_nPlayerNum)) {
                                    m_pGameDwc->Window_Message("WifiConnectMessage", "WVS_WAIT_03", 1, 0);
                                    m_bStarMii = 2;
                                    if (m_Unk23E == 1) {
                                        m_UnkA78 = 1;
                                    } else {
                                        m_UnkA78 = 0;
                                    }
                                    m_nStep = STEP_WAIT;
                                }
                                break;
                            case BTN_KIND_REFUSE:
                                if (m_pGameDwc->m_Dwc.RefuseFriendMatch(pFriend->m_nIndex)) {
                                    m_pGameDwc->WindowA_Button("WifiConnectMessage", "FENT_03_00", 0, 0, 1, 0);
                                    m_nStep = STEP_BACK;
                                }
                                break;
                        }
                    } else {
                        InputDataToMenuObj(pObj, padNo0, nPad, nMode);
                    }
                }
            } else {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            }
            break;
        case STEP_BACK:
            if (CCockpit::GetResult() == WINDOW_CLOSED) {
                m_bStarMii = 0;
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                m_pGameDwc->ClearWindowResult();
                CancelVsFriend();
                GoBackToMode(padNo0, GetPrevMode(), pObj, 1);
            }
            break;
        case STEP_WAIT:
            if (m_pGameDwc->m_Dwc.IsMatchPlay()) {
                SendObjEventTrigger(OBJ_DWC_WINDOW, EVENT_WINDOW_CLOSE, 0);
                m_pGameDwc->Window_Close();
                SetNextMode(MODE_FRIEND_VS_WAIT, pObj);
                memset(m_FriendVSWaitWork, 0, sizeof(m_FriendVSWaitWork));
                FriendVSWait_SendSetStep(0x10);
                m_nStep = STEP_START;
            } else if (m_pGameDwc->m_Dwc.GetStat() == DWC_STAT_ERROR) {
                m_pGameDwc->Window_Close();
                m_nStep = STEP_ERROR;
            }
            break;
        case STEP_ERROR:
            if (CCockpit::IsEndObj(COCKPIT_OBJ_ERROR)) {
                m_pGameDwc->WindowA_Button("WifiConnectMessage", "FENT_02_00", 0, 0, 1, 0);
                m_nStep = STEP_ERROR_MSG;
            }
            break;
        case STEP_ERROR_MSG:
            if (CCockpit::GetResult() == WINDOW_CLOSED) {
                m_pGameDwc->ClearWindowResult();
                m_nStep = STEP_CLOSE;
            }
            break;
        case STEP_CLOSE:
            m_pGameDwc->m_Dwc.CloseMatching();
            if (nStat == DWC_STAT_ERROR) {
                m_pGameDwc->m_Dwc.SetReadyFromMatchEnd();
            } else if (nStat == DWC_STAT_READY) {
                m_nStep = STEP_BACK;
            }
            break;
    }
}

void msw::CSelectMenu::FriendVS_Out()
{
    static const long objId = 0x2B;

    if (CGamePadCheck::GetInstance() != 0) {
        CGamePadCheck::GetInstance()->m_UnkAC = 0;
    }
    Scratch(objId);
}

// The names in this namespace are guesses. Defined here, after the
// CSelectMenu steps, because the strings follow those steps' constants in
// .rodata.
namespace {

    enum {
        ANIM_WIN_IN = 5,
        ANIM_WIN_OUT = 7,
        ANIM_BUTTONS = 8  // played on the window once it is open
    };

    enum {
        PANE_WIN = 0,
        PANE_TEXT_MESSAGE = 1,
        PANE_TEXT_NAME = 2,
        PANE_TEXT_NUM = 3,
        PANE_UNK_4 = 4,
        PANE_ACCEPT = 5,
        PANE_UNK_6 = 6,
        PANE_REFUSE = 7,
        PANE_MII = 8
    };

    const char *const s_LayoutName = "me_fvs_ent.brlyt";
    const char *const s_ButtonFileName = "me_fvs_ent.btn";

    // The panes of the four trophies.
    long s_TrofyPanes[4] = { 0xC, 0xA, 9, 0xB };

    enum {
        TROFY_NUM = 4
    };

    // Characters of the number texts.
    enum {
        NUM_TEXT_LEN = 0x10,
        MESSAGE_TEXT_LEN = 0x80
    };
}

msw::SelectMenu::CWiFiFrindVS::CWiFiFrindVS(long nId)
    : CSelectMenuLayoutBase(nId)
{
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
    m_pMiiRender = 0;
}

msw::SelectMenu::CWiFiFrindVS::~CWiFiFrindVS()
{
    Remove();
}

void msw::SelectMenu::CWiFiFrindVS::Main()
{
    switch (m_Unk30) {
        case STATE_SET: {
            // static-order swap, as in game/menu/CTeam.cpp (see
            // notes/decisions.md).
            static const int reset = 1;
            static const int recursive = 0;
            m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_IN, s_WinAnimSpeed[0], 0.0f, recursive, reset);
            m_Unk30 = STATE_IN;
            break;
        }
        case STATE_IN:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_IN)) {
                m_pPaneAccessor->PlayAnimation(PANE_UNK_6, 0, 1.0f, 0.0f, 1, 0);
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_BUTTONS, 1.0f, 0.0f, 1, 0);

                m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_ACCEPT)->m_nFlags40 &= ~1;
                m_pPaneAccessor->SetPaneDisp(PANE_ACCEPT, 1);
                m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_REFUSE)->m_nFlags40 &= ~1;
                m_pPaneAccessor->SetPaneDisp(PANE_REFUSE, 1);

                CSelectMenu *pSelectMenu = GetSelectMenu();
                long nMode = pSelectMenu->m_nMode;
                if (CMenuPad::s_Instance.IsInputPointerMode(0)) {
                    pSelectMenu->m_nCurrentBtnKindId[0][nMode] = BTN_KIND_ACCEPT;
                } else {
                    pSelectMenu->m_nCurrentBtnKindId[0][nMode] = BTN_KIND_ACCEPT;
                    m_nPushButtonId[0] = BTN_KIND_ACCEPT;
                    SetCursorMarkerPosition(0, BTN_KIND_ACCEPT);
                }
                m_Unk30 = STATE_ACTIVE;
            }
            break;
        case STATE_ACTIVE:
            m_pMenuPartList->CheckButtonAnim(m_nPushButtonId, 0);
            break;
        case 6:
            break;
        case STATE_CLOSE:
            if (m_pMenuPartList->IsPushButtonAnimEnd()) {
                // static-order swap, as above.
                static const int reset = 1;
                static const int recursive = 0;
                m_pPaneAccessor->PlayAnimation(PANE_WIN, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_UNK_4, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_pPaneAccessor->PlayAnimation(PANE_UNK_6, ANIM_WIN_OUT, s_WinAnimSpeed[0], 0.0f, recursive, reset);
                m_Unk30 = STATE_OUT;
            }
            break;
        case STATE_OUT:
            if (m_pPaneAccessor->IsAnimEnd(PANE_WIN, ANIM_WIN_OUT)) {
                m_Unk30 = STATE_NONE;
            }
            break;
        default:
            return;
    }

    if (m_pMenuPartList != 0) {
        m_pMenuPartList->StateCheck();
    }
}

void msw::SelectMenu::CWiFiFrindVS::Disp()
{
}

int msw::SelectMenu::CWiFiFrindVS::Set()
{
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
            ArchiveBuildLoader(s_ButtonFileName);
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(0), pLytCtrl->GetFontResourceName(0));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(1), pLytCtrl->GetFontResourceName(1));
            pPaneCtrl->AddFontRefLink(pLytCtrl->GetArchiveFont(3), pLytCtrl->GetFontResourceName(3));
            pPaneCtrl->BuildLayout(0, s_LayoutName);
            SetUpAnim(pPaneCtrl);
            SetPaneAccessor(pPaneCtrl, GetNameTable(), GetNameNum());
            SetTextBoxFromMessage(pPaneCtrl);
            pPaneCtrl->SetSortZ(100.0f);

            CMenuPartList *pMenuPartList = new CMenuPartList;

            pMenuPartList->m_pPaneAccessor = m_pPaneAccessor;

            for (long i = 0; i < GetButtonNum(); i++) {
                CMenuPartButton *pPart = new CMenuPartButton(i);

                pPart->m_pList = pMenuPartList;
                pMenuPartList->Add(pPart);
                SetUpButton(pPart, i, 0);
                pPart->m_bUnk40_0004 = 0;
                pPart->SetCallBackAnimIn(this);
                pPart->InitButtonAnim();
            }
            m_pMenuPartList = pMenuPartList;

            // The friend may have withdrawn the request.
            if (pSelectMenu->m_pVsFriend != 0 && !pSelectMenu->m_pVsFriend->IsFriendMatchChallenger()) {
                pSelectMenu->CancelVsFriend();
            }

            dwc::CFriend *pFriend = pSelectMenu->m_pVsFriend;
            if (pFriend != 0) {
                wchar_t message[MESSAGE_TEXT_LEN];
                swprintf(message, MESSAGE_TEXT_LEN, GetMessageLabelString("WifiConnectMessage", "FVS_ENT_00"),
                         pFriend->m_Name);
                SetTextPaneString(PANE_TEXT_MESSAGE, message, -1);
                const wchar_t *pName = pFriend->m_Name;
                SetTextPaneString(PANE_TEXT_NAME, pName, -1);

                // The friend's Mii, rendered into a picture pane of the
                // size of PANE_MII.
                dwc::CFriend::StorageData *pData = pFriend->GetOwnStorageData();
                RFLStoreData *pStoreData = &pData->m_StoreData;
                nw4r::lyt::Size size = m_pPaneAccessor->GetPaneObj(PANE_MII)->GetPane()->GetSize();
                m_pMiiRender = new CPlayerInfoRender(0, *pStoreData, size.width, size.height);

                GXTexObj texObj;
                GXInitTexObj(&texObj, m_pMiiRender->GetTexImg(), (long)size.width, (long)size.height, GX_TF_RGB5A3,
                             GX_CLAMP, GX_CLAMP, 0);
                GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
                bcs::G2D::CPaneCtrl *pLytObj = GetLytObj();
                menu::CreateMiiPicturePane(&texObj, pLytObj, m_pPaneAccessor->GetPane(PANE_MII), "RENDER_MII");
                m_pMiiRender->SetDispState(1);

                SetTextPaneString(PANE_TEXT_NUM, 0, pData->m_nWinNum);

                pSelectMenu->Build(OBJ_TROFY);
                CTrofy *pTrofy = static_cast<CTrofy *>(pSelectMenu->FindObj(OBJ_TROFY));
                pTrofy->m_pFriend = pFriend;
                for (long i = 0; i < TROFY_NUM; i++) {
                    pTrofy->SetWiFiTrofy(i, m_pPaneAccessor->GetPaneObj(s_TrofyPanes[i]));
                }

                // The two buttons stay hidden until the window is open.
                m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_ACCEPT)->m_nFlags40 |= 1;
                m_pPaneAccessor->SetPaneDisp(PANE_ACCEPT, 0);
                m_pMenuPartList->FindObjectByCurrentBtnKindId(BTN_KIND_REFUSE)->m_nFlags40 |= 1;
                m_pPaneAccessor->SetPaneDisp(PANE_REFUSE, 0);

                m_Unk2C = 1;
                m_Unk30 = STATE_SET;
                ClearPushButtonId();
            } else {
                m_pPaneAccessor->SetPaneDisp(PANE_WIN, 0);
                m_Unk2C = 1;
                m_Unk30 = STATE_ACTIVE;
                ClearPushButtonId();
            }
        }
    }
    return 0;
}

// Inlined into Set(); defined after it because its local static follows
// Set()'s constants in .rodata.
inline void msw::SelectMenu::CWiFiFrindVS::SetTextPaneString(long nPane, const wchar_t *pText, long nValue)
{
    static const bool recursive = true;

    if (pText == 0) {
        wchar_t num[NUM_TEXT_LEN];
        swprintf(num, NUM_TEXT_LEN, L"%d", nValue);
        m_pPaneAccessor->SetTextBoxString(nPane, num, wcslen(num));
    } else {
        CMenuPaneAccessor *pAccessor = m_pPaneAccessor;
        nw4r::lyt::Pane *pPane =
            (unsigned long)nPane < pAccessor->m_nNum ? pAccessor->GetPane(pAccessor->GetPaneName(nPane), recursive, 0) : 0;
        if (pPane != 0) {
            m_pPaneAccessor->SetTextBoxString(nPane, pText, wcslen(pText));
        }
    }
}

void msw::SelectMenu::CWiFiFrindVS::Remove()
{
    GetSelectMenu()->Scratch(OBJ_TROFY);
    RemoveList();
    DeleteMenuPartList();
    DeletePaneAccessor();
    DeleteLytObj();
    DeleteArcFont();
    if (m_pMiiRender != 0) {
        m_pMiiRender->Delete();
        m_pMiiRender = 0;
    }
    m_Unk2C = 0;
    m_Unk30 = STATE_NONE;
}

long msw::SelectMenu::CWiFiFrindVS::HitCheck(bcs::FVEC3 *pPos)
{
    if (pPos == 0) {
        return 0;
    }
    return m_pMenuPartList->CheckHitButton(pPos);
}

long msw::SelectMenu::CWiFiFrindVS::SetCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiFrindVS::PushCursorAnim(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiFrindVS::SetCursorMarkerPosition(long, long)
{
    return 0;
}

long msw::SelectMenu::CWiFiFrindVS::SetCursorMarkerVisible(long, int)
{
    return 0;
}

long msw::SelectMenu::CWiFiFrindVS::StickMoveCheck(long nPad, long nBtnKindId)
{
    // The result is not used.
    IsButtonKindOfStickMode(nBtnKindId);
    // The game truncates the result to 16 bits before returning it as a long.
    return (unsigned short)m_pMenuPartList->CheckStckMoveButton(nPad, nBtnKindId, CMenuPad::s_Instance.GetInput(nPad));
}

long msw::SelectMenu::CWiFiFrindVS::SetDefaultMoveCursor(long nPad, long nBtnKindId)
{
    if (!IsButtonKindOfStickMode(nBtnKindId)) {
        nBtnKindId = BTN_KIND_ACCEPT;
    }
    return nBtnKindId;
}

int msw::SelectMenu::CWiFiFrindVS::IsButtonKindOfStickMode(long nBtnKindId)
{
    CMenuPartBase *pPart = m_pMenuPartList->FindObjectByCurrentBtnKindId(nBtnKindId);
    int bStickMode = 0;

    if (pPart != 0 && pPart->IsStickMode() == 1) {
        bStickMode = 1;
    }
    return bStickMode;
}

long msw::SelectMenu::CWiFiFrindVS::PushButton(long nPad, long nBtnKindId)
{
    m_pMenuPartList->SetPushButtonAnim(nBtnKindId, nPad);
    return 0;
}
