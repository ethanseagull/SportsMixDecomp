// msw::CCockpitVolleyCoin: the volley cockpit's coin counters.
//
// Ported from game/hockey/CCockpitHockeyCoin.cpp; the parts that differ from
// hockey are marked "Volley:".
#include "game/msw/CCockpitVolleyCoin.hpp"

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
        ANIM_COUNTER_IN_WAIT = 5,
        ANIM_COUNTER_OUT = 8,
        ANIM_COUNTER_OUT_WAIT = 9,
        ANIM_COUNTER_IN = 10
    };

    // The resource file of the cockpit that holds the coin layout.
    const long RES_FILE_COIN = 3; // Volley: 2 in hockey
    // Layer of the layout object.
    const long LAYER_COIN = 6;
    // Number of panes in CoinRes's pane name table.
    const long PANE_NUM = 18;
    // Pane passed to CoinRes::SetCoinPosition.
    const unsigned char POSITION_PANE = 13;

    // The text box of a counter pane object.
    inline bcs::G2D::CTextBoxDL *GetTextBox(bcs::G2D::CPaneObj *pObj)
    {
        return nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());
    }
}

msw::CCockpitVolleyCoin::CCockpitVolleyCoin(long nId)
    : CCockpitVolleyLayoutBase(nId)
{
    m_nStep = 0;
}

msw::CCockpitVolleyCoin::~CCockpitVolleyCoin()
{
    Remove();
}

void msw::CCockpitVolleyCoin::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        long nTeams = CoinRes::GetTeamNum();
        long i;

        for (i = 0; i < nTeams; i++) {
            SetCoinState(i, COIN_STATE_RESET);
        }
        SetCoinDisp(1);
        for (i = 0; i < nTeams; i++) {
            m_3DCoin[i].EnableDisp(1);
        }
        // Volley: the counters are placed only on an unsplit screen.
        if (CGame::g_GameWork.m_nViewSplit == 0) {
            CoinRes::SetCoinPosition(m_pPaneAccessor, POSITION_PANE);
        }
        PlayCoinAnim(ANIM_COIN_IDLE);
        m_nStep = 2;
    }
}

void msw::CCockpitVolleyCoin::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);

    long nTeams = CoinRes::GetTeamNum();

    for (long i = 0; i < nTeams; i++) {
        m_3DCoin[i].AttachPane(m_pPaneAccessor->GetPane(CoinRes::GetCoin3DPane(i)));
    }
}

int msw::CCockpitVolleyCoin::Set()
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
            // Volley: the layout depends on the screen split.
            CoinRes::E_COINLAYOUTID nLayout;

            switch (CGame::g_GameWork.m_nViewSplit) {
                case 1:
                    nLayout = CoinRes::COINLAYOUT_VOLLEY_02;
                    break;
                case 2:
                    nLayout = CoinRes::COINLAYOUT_VOLLEY_03;
                    break;
                default:
                    nLayout = CoinRes::COINLAYOUT_COCKPIT;
                    break;
            }

            pPaneCtrl->BuildLayout(0, CoinRes::GetLayoutResource(nLayout));
            pPaneCtrl->BuildAnim(0, CoinRes::GetLayoutAnimResource(nLayout),
                                 CoinRes::GetLayoutAnimResourceNum(nLayout));
            SetPaneAccessor(pPaneCtrl, CoinRes::GetLayoutPaneNameTable(nLayout), PANE_NUM);
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

void msw::CCockpitVolleyCoin::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;

    long nTeams = CoinRes::GetTeamNum();

    for (long i = 0; i < nTeams; i++) {
        m_3DCoin[i].Finalize();
    }
}

long msw::CCockpitVolleyCoin::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

inline void msw::CCockpitVolleyCoin::PlayCoinAnim(long nAnim)
{
    long nTeams = CoinRes::GetTeamNum();

    for (long i = 0; i < nTeams; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(CoinRes::GetCoinPane(i));

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
}

// As in hockey: the counters move with the score part.
int msw::CCockpitVolleyCoin::ScoreOut()
{
    PlayCoinAnim(ANIM_COUNTER_OUT);
    return 0;
}

int msw::CCockpitVolleyCoin::ScoreIn()
{
    PlayCoinAnim(ANIM_COUNTER_IN);
    return 0;
}

int msw::CCockpitVolleyCoin::ScoreOutWait()
{
    PlayCoinAnim(ANIM_COUNTER_OUT_WAIT);
    return 0;
}

int msw::CCockpitVolleyCoin::ScoreInWait()
{
    PlayCoinAnim(ANIM_COUNTER_IN_WAIT);
    return 0;
}

// As in hockey: nothing to do for the coin part.
int msw::CCockpitVolleyCoin::CoinOut()
{
    return 0;
}

int msw::CCockpitVolleyCoin::CoinIn()
{
    return 0;
}

int msw::CCockpitVolleyCoin::CoinOutWait()
{
    return 0;
}

int msw::CCockpitVolleyCoin::CoinInWait()
{
    return 0;
}

// Volley: the counters slide in with the serve-score animation of kind 0;
// the other kinds (1-3) leave them as they are. -1 for an unknown kind.
long msw::CCockpitVolleyCoin::SetServeScoreAnim(long nKind)
{
    if (nKind < 0 || nKind > 3) {
        return -1;
    }
    if (nKind == 0) {
        PlayCoinAnim(ANIM_COUNTER_IN);
    }
    return 0;
}

// Defined here and not in the class: the original's .rodata has the empty
// string after Set's constant, where a definition at this point puts it.
inline const wchar_t *msw::CCockpitVolleyCoin::GetCountText(long nValue)
{
    static const wchar_t *noneStr = L"";

    if (nValue > 0) {
        return menu::GetNumString(nValue, -1);
    }
    return noneStr;
}

void msw::CCockpitVolleyCoin::SetCoinValue(long nTeam, long nValue)
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
    SetCoinState(nTeam, COIN_STATE_RESET);
}

void msw::CCockpitVolleyCoin::AddCoinValue(long nTeam, long nOld, long nNew)
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
        if (nOld > nNew) {
            SetCoinState(nTeam, COIN_STATE_DROP);
        }
        if (nNew == 0) {
            SetCoinState(nTeam, COIN_STATE_RESET);
        }
    } else {
        // The old count moves to the first text box, the new one to the
        // second.
        const wchar_t *pText = GetCountText(nNew);

        GetTextBox(pObj)->SetString(GetTextBox(pShadowObj)->GetStringBuffer(), 0);
        GetTextBox(pShadowObj)->SetString(pText, 0);
        SetCoinState(nTeam, COIN_STATE_POPUP);
    }
}

void msw::CCockpitVolleyCoin::SetCoinDisp(int bDisp)
{
    m_pPaneAccessor->SetDisp(CoinRes::GetCoinDispPane(0), bDisp);
    m_pPaneAccessor->SetDisp(CoinRes::GetCoinDispPane(1), bDisp);
}

void msw::CCockpitVolleyCoin::SetCoinState(long nTeam, long nState)
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
    }
}

// No telop to move.
void msw::CCockpitVolleyCoin::TelopMove(long, long)
{
}

long msw::CCockpitVolleyCoin::GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    if (nTeam >= CoinRes::GetTeamNum()) {
        return -1;
    }

    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(CoinRes::GetCoin3DPane(nTeam));

    if (pPane == 0) {
        return -2;
    }

    rSize = pPane->GetSize();

    nw4r::math::MTX34 mtx = pPane->GetGlobalMtx();

    rPos.x = mtx._03;
    rPos.y = mtx._13;
    rPos.z = mtx._23;
    return 0;
}
