// msw::CCockpitDodgeCoin: the dodge cockpit's coin counters.
//
// Written from game/hockey/CCockpitHockeyCoin.cpp; the parts that differ
// from hockey are marked "Dodge:".
#include "game/msw/CCockpitDodgeCoin.hpp"

#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "bcs/G2D/G2DUtil.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/CoinRes.hpp"
#include "game/msw/menu/MenuUtil.hpp"

namespace {
    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Counter animations (CoinRes's animation files, by index; names
    // guessed from where they are played).
    enum {
        ANIM_COIN_RESET = 0,
        ANIM_COIN_POPUP = 1,
        ANIM_COIN_IDLE = 2,
        ANIM_COIN_DROP = 3,
        ANIM_COUNTER_IN_WAIT = 5,   // coin_Set_01.brlan
        ANIM_COUNTER_IN_WAIT_2 = 6, // coin_Set_02.brlan
        ANIM_COUNTER_OUT = 8,
        ANIM_COUNTER_OUT_WAIT = 9,
        ANIM_COUNTER_IN = 10
    };

    // The resource file of the cockpit that holds the coin layout.
    const long RES_FILE_COIN = 2;
    // Layer of the layout object.
    const long LAYER_COIN = 6;
    // Number of panes in CoinRes's pane name table.
    const long PANE_NUM = 18;
    // Pane passed to CoinRes::SetCoinPosition (Dodge: 12, hockey 13).
    const unsigned char POSITION_PANE = 12;

    // The text box of a counter pane object.
    inline bcs::G2D::CTextBoxDL *GetTextBox(bcs::G2D::CPaneObj *pObj)
    {
        return nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());
    }
}

msw::CCockpitDodgeCoin::CCockpitDodgeCoin(long nId)
    : CCockpitDodgeLayoutBase(nId)
{
    m_nStep = 0;
}

msw::CCockpitDodgeCoin::~CCockpitDodgeCoin()
{
    Remove();
}

void msw::CCockpitDodgeCoin::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        long nTeams = CoinRes::GetTeamNum();
        long i;

        for (i = 0; i < nTeams; i++) {
            SetCoinState(i, COIN_STATE_RESET, 0);
        }
        // Dodge: the counters start hidden (hockey shows them here);
        // CCockpitDodge shows them through SetCoinDisp.
        SetCoinDisp(0);
        for (i = 0; i < nTeams; i++) {
            m_3DCoin[i].EnableDisp(1);
        }
        CoinRes::SetCoinPosition(m_pPaneAccessor, POSITION_PANE);
        PlayCoinAnim(ANIM_COIN_IDLE);
        m_nStep = 2;
    }
}

void msw::CCockpitDodgeCoin::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);

    long nTeams = CoinRes::GetTeamNum();

    for (long i = 0; i < nTeams; i++) {
        m_3DCoin[i].AttachPane(m_pPaneAccessor->GetPane(CoinRes::GetCoin3DPane(i)));
    }
}

int msw::CCockpitDodgeCoin::Set()
{
    if (m_pPaneAccessor != 0) {
        return 0;
    }

    CCockpit *pCockpit = CCockpit::GetTask();
    CLytCtrl *pLytCtrl = CLytCtrl::GetInstance();

    if (pLytCtrl == 0) {
        return -1;
    }

    const char *pFileName = CCockpit::GetResourceFileName(RES_FILE_COIN);
    nw4r::lyt::ArcResourceAccessor *pAccessor =
        pLytCtrl->NewArcResourceAccessor(pFileName, pCockpit->GetArchiveRootFolderName(0));

    if (pAccessor != 0) {
        bcs::G2D::CPaneCtrl *pPaneCtrl = CreateLytObj(pAccessor, LAYER_COIN, 0);

        if (pPaneCtrl != 0) {
            pPaneCtrl->BuildLayout(0, CoinRes::GetLayoutResource(CoinRes::COINLAYOUT_COCKPIT));
            pPaneCtrl->BuildAnim(0, CoinRes::GetLayoutAnimResource(CoinRes::COINLAYOUT_COCKPIT),
                                 CoinRes::GetLayoutAnimResourceNum(CoinRes::COINLAYOUT_COCKPIT));
            SetPaneAccessor(pPaneCtrl, CoinRes::GetLayoutPaneNameTable(CoinRes::COINLAYOUT_COCKPIT), PANE_NUM);
            pPaneCtrl->SetSortZ(-990.0f);
            pPaneCtrl->SetVisible(0);

            long nTeams = CoinRes::GetTeamNum();
            long i;

            for (i = 0; i < nTeams; i++) {
                m_3DCoin[i].Initialize(i, pCockpit);
                m_3DCoin[i].EnableDisp(0);
            }

            // Hang each team's telop pane under the pane of this game mode,
            // at its origin. (`const` lets the compiler keep the zero in a
            // register across the loop's calls, as the original does.)
            const nw4r::math::VEC3 origin(0.0f, 0.0f, 0.0f);
            long nMode = CGame::g_GameWork.m_nGameMode;

            for (i = 0; i < nTeams; i++) {
                nw4r::lyt::Pane *pTelop = m_pPaneAccessor->GetPane(CoinRes::GetCoinTelopPane(i));
                nw4r::lyt::Pane *pParent = m_pPaneAccessor->GetPane(CoinRes::GetTelopAttachPane(nMode));

                if (pTelop != 0 && pParent != 0) {
                    bcs::G2D::SetPaneParent(pTelop, pParent, 1, 0);
                    pTelop->SetTranslate(origin);
                }
            }

            for (i = 0; i < nTeams; i++) {
                SetCoinValue(i, CoinRes::GetGameCoinNum(i));
            }
        }
    }
    m_nStep = 1;
    return 0;
}

void msw::CCockpitDodgeCoin::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;

    long nTeams = CoinRes::GetTeamNum();

    for (long i = 0; i < nTeams; i++) {
        m_3DCoin[i].Finalize();
    }
}

long msw::CCockpitDodgeCoin::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

inline void msw::CCockpitDodgeCoin::PlayCoinAnim(long nAnim)
{
    long nTeams = CoinRes::GetTeamNum();

    for (long i = 0; i < nTeams; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(CoinRes::GetCoinPane(i));

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
}

// Dodge: new virtuals here (the dodge cockpit has no score part); the same
// slides as the other sports' counters.
int msw::CCockpitDodgeCoin::ScoreOut()
{
    PlayCoinAnim(ANIM_COUNTER_OUT);
    return 0;
}

int msw::CCockpitDodgeCoin::ScoreIn()
{
    PlayCoinAnim(ANIM_COUNTER_IN);
    return 0;
}

int msw::CCockpitDodgeCoin::ScoreOutWait()
{
    PlayCoinAnim(ANIM_COUNTER_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoin::ScoreInWait()
{
    PlayCoinAnim(ANIM_COUNTER_IN_WAIT);
    return 0;
}

// Dodge: the counters move with the coin part (hockey's do nothing here).
int msw::CCockpitDodgeCoin::CoinOut()
{
    PlayCoinAnim(ANIM_COUNTER_OUT);
    return 0;
}

int msw::CCockpitDodgeCoin::CoinIn()
{
    PlayCoinAnim(ANIM_COUNTER_IN);
    return 0;
}

int msw::CCockpitDodgeCoin::CoinOutWait()
{
    PlayCoinAnim(ANIM_COUNTER_OUT_WAIT);
    return 0;
}

int msw::CCockpitDodgeCoin::CoinInWait()
{
    PlayCoinAnim(ANIM_COUNTER_IN_WAIT_2);
    return 0;
}

// Defined here and not in the class: the original's .rodata has the empty
// string after Set's constant, where a definition at this point puts it.
inline const wchar_t *msw::CCockpitDodgeCoin::GetCountText(long nValue)
{
    static const wchar_t *noneStr = L"";

    if (nValue > 0) {
        return menu::GetNumString(nValue, -1);
    }
    return noneStr;
}

void msw::CCockpitDodgeCoin::SetCoinValue(long nTeam, long nValue)
{
    long nPane;
    long nShadowPane;

    switch (nTeam) {
        case 0:
            nPane = 2;
            nShadowPane = 3;
            break;
        case 1:
            nPane = 4;
            nShadowPane = 5;
            break;
        default:
            return;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);
    bcs::G2D::CPaneObj *pShadowObj = m_pPaneAccessor->GetPaneObj(nShadowPane);

    if (pObj == 0 || pShadowObj == 0) {
        return;
    }

    const wchar_t *pText = GetCountText(nValue);

    GetTextBox(pObj)->SetString(pText, 0);
    GetTextBox(pShadowObj)->SetString(pText, 0);
    SetCoinState(nTeam, COIN_STATE_RESET, 0);
}

void msw::CCockpitDodgeCoin::AddCoinValue(long nTeam, long nOld, long nNew)
{
    long nPane;
    long nShadowPane;

    switch (nTeam) {
        case 0:
            nPane = 2;
            nShadowPane = 3;
            break;
        case 1:
            nPane = 4;
            nShadowPane = 5;
            break;
        default:
            return;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(nPane);
    bcs::G2D::CPaneObj *pShadowObj = m_pPaneAccessor->GetPaneObj(nShadowPane);

    if (pObj == 0 || pShadowObj == 0) {
        return;
    }

    if (nOld >= nNew) {
        const wchar_t *pText = GetCountText(nNew);

        GetTextBox(pObj)->SetString(pText, 0);
        GetTextBox(pShadowObj)->SetString(pText, 0);
        // Dodge: no drop animation when the count falls (coins are spent
        // through SetCoinState's COIN_STATE_USE instead).
        if (nNew == 0) {
            SetCoinState(nTeam, COIN_STATE_RESET, 0);
        }
    } else {
        // The old count moves to the first text box, the new one to the
        // second.
        const wchar_t *pText = GetCountText(nNew);

        GetTextBox(pObj)->SetString(GetTextBox(pShadowObj)->GetStringBuffer(), 0);
        GetTextBox(pShadowObj)->SetString(pText, 0);
        SetCoinState(nTeam, COIN_STATE_POPUP, 0);
    }
}

void msw::CCockpitDodgeCoin::SetCoinDisp(int bDisp)
{
    m_pPaneAccessor->SetDisp(CoinRes::GetCoinDispPane(0), bDisp);
    m_pPaneAccessor->SetDisp(CoinRes::GetCoinDispPane(1), bDisp);
}

// Dodge: a third argument, the number of coins for COIN_STATE_USE.
void msw::CCockpitDodgeCoin::SetCoinState(long nTeam, long nState, long nNum)
{
    if (nTeam >= CoinRes::GetTeamNum()) {
        return;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(CoinRes::GetCoinAnimPane(nTeam));

    if (pObj == 0) {
        return;
    }

    switch (nState) {
        case COIN_STATE_RESET:
            pObj->PlayAnimation(ANIM_COIN_RESET, 1.0f, 0.0f, 1, 1, 1);
            m_3DCoin[nTeam].ActReset();
            break;
        case COIN_STATE_POPUP:
            pObj->PlayAnimation(ANIM_COIN_POPUP, 1.0f, 0.0f, 1, 1, 1);
            pObj->PushNextAnimation(ANIM_COIN_IDLE, 1.0f, 0.0f, 1, 1);
            m_3DCoin[nTeam].ActPopup();
            break;
        case COIN_STATE_DROP:
            pObj->PlayAnimation(ANIM_COIN_DROP, 1.0f, 0.0f, 1, 1, 1);
            pObj->PushNextAnimation(ANIM_COIN_RESET, 1.0f, 0.0f, 1, 1);
            m_3DCoin[nTeam].ActDrop();
            break;
        case COIN_STATE_USE:
            pObj->PlayAnimation(ANIM_COIN_DROP, 1.0f, 0.0f, 1, 1, 1);
            pObj->PushNextAnimation(ANIM_COIN_RESET, 1.0f, 0.0f, 1, 1);
            m_3DCoin[nTeam].ActUse(nNum);
            break;
    }
}
