// msw::CCockpitBossCoin: the boss cockpit's coin counter.
//
// The boss game's copy of game/hockey/CCockpitHockeyCoin.cpp, from before
// the layout tables moved into CoinRes: one team, its own layout, animation
// and pane tables, and the team count as a local constant in several
// functions.
#include "game/msw/CCockpitBossCoin.hpp"

#include "bcs/FVEC3.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "bcs/G2D/CTextBoxDL.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/menu/MenuUtil.hpp"

namespace {
    // Events the cockpit sends (see game/tour/CCockpitTournament.cpp).
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Counter animations (indexes into sAnimNames).
    enum {
        ANIM_COIN_RESET = 0,
        ANIM_COIN_POPUP = 1,
        ANIM_COIN_IDLE = 2,
        ANIM_COIN_DROP = 3,
        ANIM_COUNTER_IN = 4,
        ANIM_COUNTER_IN_WAIT = 5,
        ANIM_COUNTER_OUT = 6,
        ANIM_COUNTER_OUT_WAIT = 7
    };

    // The resource file of the cockpit that holds the coin layout.
    const long RES_FILE_COIN = 2;
    // Layer of the layout object.
    const long LAYER_COIN = 6;
    // Number of teams with a counter.
    const long TEAM_NUM = 1;

    const char *sLayoutNames[] = {
        "BOSS_coin.brlyt"
    };

    const char *sAnimNames[] = {
        "BOSS_coin_Coin_ani_00.brlan",
        "BOSS_coin_Coin_ani_01.brlan",
        "BOSS_coin_Coin_ani_02.brlan",
        "BOSS_coin_Coin_ani_03.brlan",
        "BOSS_coin_Set_00.brlan",
        "BOSS_coin_Set_01.brlan",
        "BOSS_coin_Set_02.brlan",
        "BOSS_coin_Set_03.brlan",
        "BOSS_coin_Set_coin_00.brlan",
        "BOSS_coin_Set_coin_01.brlan",
        "BOSS_coin_Set_coin_03.brlan"
    };

    const char *sPaneNames[] = {
        "coin_L",
        "coin_R",
        "tn_t_coin_L",
        "tn_t_coin_L02",
        "tn_t_coin_R",
        "tn_t_coin_R02",
        "coin_L_ani",
        "coin_R_ani",
        "L_null",
        "R_null",
        "coin_L_vis",
        "coin_R_vis",
        "VO_DO_coin",
        "BA_HO_coin"
    };

    // Indexes into sPaneNames, per team.
    const unsigned char sCoinPane[TEAM_NUM] = { 0 };
    const unsigned char sCoinDispPane[TEAM_NUM] = { 10 };
    const unsigned char sCoinAnimPane[TEAM_NUM] = { 6 };
    const unsigned char sCoin3DPane[TEAM_NUM] = { 8 };

    // The text box of a counter pane object.
    inline bcs::G2D::CTextBoxDL *GetTextBox(bcs::G2D::CPaneObj *pObj)
    {
        return nw4r::ut::DynamicCast<bcs::G2D::CTextBoxDL *>(pObj->GetPane());
    }
}

msw::CCockpitBossCoin::CCockpitBossCoin(long nId)
    : CCockpitBossLayoutBase(nId)
{
    m_nStep = 0;
}

msw::CCockpitBossCoin::~CCockpitBossCoin()
{
    Remove();
}

void msw::CCockpitBossCoin::Main()
{
    m_DispTaskList.StatusCheck(m_pPaneAccessor);

    if (m_nStep == 1) {
        SetupGame();
        PlayAnimCoin(ANIM_COIN_IDLE);
        m_nStep = 2;
    }
}

void msw::CCockpitBossCoin::Disp()
{
    m_DispTaskList.DispLoop(m_pPaneAccessor, 0);

    for (long i = 0; i < TEAM_NUM; i++) {
        m_3DCoin[i].AttachPane(m_pPaneAccessor->GetPane(sCoin3DPane[i]));
    }
}

int msw::CCockpitBossCoin::Set()
{
    static const long teamNum = 1;

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
            pPaneCtrl->BuildLayout(0, sLayoutNames[0]);
            pPaneCtrl->BuildAnim(0, sAnimNames, sizeof(sAnimNames) / sizeof(sAnimNames[0]));
            SetPaneAccessor(pPaneCtrl, sPaneNames, sizeof(sPaneNames) / sizeof(sPaneNames[0]));
            pPaneCtrl->SetSortZ(-990.0f);

            long i;

            for (i = 0; i < teamNum; i++) {
                m_3DCoin[i].Initialize(i, pCockpit);
                m_3DCoin[i].EnableDisp(0);
            }
            for (i = 0; i < teamNum; i++) {
                SetCoinValue(i, CGame::GetTeamCoin(CGame::GetTeamColorIdx(i)));
            }
        }
    }
    m_nStep = 1;
    return 0;
}

inline void msw::CCockpitBossCoin::SetupGame()
{
    static const long teamNum = 1;
    long i;

    for (i = 0; i < teamNum; i++) {
        SetCoinState(i, COIN_STATE_RESET);
    }
    SetCoinDisp(1);
    for (i = 0; i < teamNum; i++) {
        m_3DCoin[i].EnableDisp(1);
    }
}

inline void msw::CCockpitBossCoin::PlayAnimCoin(long nAnim)
{
    static const long teamNum = 1;

    for (long i = 0; i < teamNum; i++) {
        bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinPane[i]);

        if (pObj != 0) {
            pObj->PlayAnimation(nAnim, 1.0f, 0.0f, 0, 1, 1);
        }
    }
}

void msw::CCockpitBossCoin::Remove()
{
    DeletePaneAccessor();
    DeleteLytObj();
    m_nStep = 0;

    for (long i = 0; i < TEAM_NUM; i++) {
        m_3DCoin[i].Finalize();
    }
}

long msw::CCockpitBossCoin::SendEvent(long nEvent, void *pParam)
{
    if (nEvent == EVENT_SET_DISP) {
        int bVisible = reinterpret_cast<int>(pParam);

        if (GetLytObj() != 0) {
            GetLytObj()->SetVisible(bVisible);
        }
    }
    return 0;
}

// The life gauge part is not this layout's.
int msw::CCockpitBossCoin::LifeGaugeOut()
{
    return 0;
}

int msw::CCockpitBossCoin::LifeGaugeIn()
{
    return 0;
}

int msw::CCockpitBossCoin::LifeGaugeOutWait()
{
    return 0;
}

int msw::CCockpitBossCoin::LifeGaugeInWait()
{
    return 0;
}

int msw::CCockpitBossCoin::CoinOut()
{
    PlayAnimCoin(ANIM_COUNTER_OUT);
    return 0;
}

int msw::CCockpitBossCoin::CoinIn()
{
    PlayAnimCoin(ANIM_COUNTER_IN);
    return 0;
}

int msw::CCockpitBossCoin::CoinOutWait()
{
    PlayAnimCoin(ANIM_COUNTER_OUT_WAIT);
    return 0;
}

int msw::CCockpitBossCoin::CoinInWait()
{
    PlayAnimCoin(ANIM_COUNTER_IN_WAIT);
    return 0;
}

void msw::CCockpitBossCoin::SetCoinValue(long nTeam, long nValue)
{
    // Unused here; the arguments SetCoinState passes to PlayAnimation.
    // static-order swap: the original names these @reset@0 and @recursive;
    // declared in the other order the names come out right but the two
    // objects swap places (notes/decisions.md).
    static const int reset = 1;
    static const int recursive = 0;
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

void msw::CCockpitBossCoin::AddCoinValue(long nTeam, long nOld, long nNew)
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

// Reads sCoinDispPane[1] although the table has one entry, as the original
// does: it gets the padding byte after the table, 0 ("coin_L"). A port
// should give the table a second entry 0 (with it the compiler folds the
// read to a constant, which the original did not).
void msw::CCockpitBossCoin::SetCoinDisp(int bDisp)
{
    m_pPaneAccessor->SetPaneDisp(sCoinDispPane[0], bDisp);
    m_pPaneAccessor->SetPaneDisp(sCoinDispPane[1], bDisp);
}

void msw::CCockpitBossCoin::SetCoinState(long nTeam, long nState)
{
    // static-order swap, as in SetCoinValue.
    static const int reset = 1;
    static const int recursive = 1;

    if (nTeam >= TEAM_NUM) {
        return;
    }

    bcs::G2D::CPaneObj *pObj = m_pPaneAccessor->GetPaneObj(sCoinAnimPane[nTeam]);

    if (pObj == 0) {
        return;
    }

    switch (nState) {
        case COIN_STATE_RESET:
            pObj->PlayAnimation(ANIM_COIN_RESET, 1.0f, 0.0f, reset, recursive, 1);
            m_3DCoin[nTeam].ActReset();
            break;
        case COIN_STATE_POPUP:
            pObj->PlayAnimation(ANIM_COIN_POPUP, 1.0f, 0.0f, reset, recursive, 1);
            pObj->PushNextAnimation(ANIM_COIN_IDLE, 1.0f, 0.0f, reset, recursive);
            m_3DCoin[nTeam].ActPopup();
            break;
        case COIN_STATE_DROP:
            pObj->PlayAnimation(ANIM_COIN_DROP, 1.0f, 0.0f, reset, recursive, 1);
            pObj->PushNextAnimation(ANIM_COIN_RESET, 1.0f, 0.0f, reset, recursive);
            m_3DCoin[nTeam].ActDrop();
            break;
    }
}

// Defined here and not in the class: the original's .rodata has the empty
// string after SetCoinState's constants, where a definition at this point
// puts it.
inline const wchar_t *msw::CCockpitBossCoin::GetCountText(long nValue)
{
    static const wchar_t *noneStr = L"";

    if (nValue > 0) {
        return menu::GetNumString(nValue, -1);
    }
    return noneStr;
}

// Nothing in BossF calls this (CCockpitBoss asks the ball power layout), so
// the original link dead-stripped it; its constant is still in .rodata.
long msw::CCockpitBossCoin::GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    static const long teamNum = 1;

    if (nTeam >= teamNum) {
        return -1;
    }

    nw4r::lyt::Pane *pPane = m_pPaneAccessor->GetPane(sCoin3DPane[nTeam]);

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
