#include "game/msw/CSelectMenu.hpp"

#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

// The steps of the Wi-Fi top screen. A file of their own: the CWiFiGame
// unit before them ends with its `@28@` destructor thunk, and their
// constants (1.0f, 0.0f, "TitleWiFiTop") start a new 8-aligned block of
// .rodata. The names in this namespace are guesses.
namespace {

    // Ids of the CSelectMenu menu objects used here.
    enum {
        OBJ_TOP = 0,
        OBJ_WIFI_TOP = 0x24
    };

    // The button kind ids of the screen.
    enum {
        BTN_KIND_NONE = 0,
        BTN_KIND_BACK = 1,
        BTN_KIND_UNK_97 = 0x97,
        BTN_KIND_UNK_98 = 0x98
    };

    // The modes the two buttons lead to.
    enum {
        MODE_UNK_1E = 0x1E,
        MODE_UNK_24 = 0x24
    };

    // The pad bits read here (CMenuPad::GetInput).
    enum {
        PAD_A = 0x10,
        PAD_B = 0x20
    };

    enum {
        ANIM_IN = 6
    };
}

void msw::CSelectMenu::WifiTop_In()
{
    Build(OBJ_WIFI_TOP);

    SendObjEventTrigger(OBJ_TOP, 0, (void *)GetMessageString("TitleWiFiTop", 0));
    SendObjEventTrigger(OBJ_TOP, 1, (void *)GetMessageString("TitleWiFiTop", 1));
    SendObjEventTrigger(OBJ_TOP, 2, 0);

    m_Unk27B = 0;
    ChangeLeaderPad(CMenuPadInfo::s_Instance.SelectPadLeader(0));

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_WIFI_TOP);
    if (pObj != 0) {
        unsigned short &rBtnKindId = m_nCurrentBtnKindId[0][m_nMode];
        long nBtnKindId = pObj->SetDefaultMoveCursor(0, rBtnKindId);
        rBtnKindId = nBtnKindId;
        pObj->m_nPushButtonId[0] = nBtnKindId;
        pObj->SetCursorMarkerPosition(0, nBtnKindId);

        bcs::G2D::CPaneCtrl *pPaneCtrl = pObj->GetLytObj();
        if (pPaneCtrl != 0) {
            pPaneCtrl->PlayAnim(ANIM_IN, 1.0f, 0.0f, 1, 1);
        }
    }
}

void msw::CSelectMenu::WifiTop_Loop()
{
    long nMode = m_nMode;
    long nPad = CMenuPadInfo::s_Instance.GetPadLeader();

    CSelectMenuLayoutBase *pObj = (CSelectMenuLayoutBase *)FindObj(OBJ_WIFI_TOP);
    if (pObj == 0 || pObj->m_Unk30 != CSelectMenuLayoutBase::STATE_ACTIVE) {
        return;
    }

    unsigned short nBtnKindId = m_nCurrentBtnKindId[0][nMode];
    if (CMenuPad::s_Instance.GetInput(nPad) & PAD_A) {
        float fWait = _GetModeChangeWaitTime();
        switch (nBtnKindId) {
            case BTN_KIND_UNK_97:
                pObj->PushButton(0, nBtnKindId);
                SetNextMode(MODE_UNK_1E, fWait);
                break;
            case BTN_KIND_UNK_98:
                pObj->PushButton(0, nBtnKindId);
                SetNextMode(MODE_UNK_24, fWait);
                break;
            default:
                SetNextMode(nMode, fWait);
                break;
            case BTN_KIND_BACK:
                GoBackToMode(0, GetPrevMode(), pObj, 1);
                break;
            case BTN_KIND_NONE:
                break;
        }
    } else if (CMenuPad::s_Instance.GetInput(nPad) & PAD_B) {
        GoBackToMode(0, GetPrevMode(), pObj, 1);
    } else {
        InputDataToMenuObj(pObj, 0, nPad, nMode);
    }
}

void msw::CSelectMenu::WifiTop_Out()
{
    Scratch(OBJ_WIFI_TOP);
}
