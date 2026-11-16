// msw::CGameBasket: the game task of the basketball match (BasketF), with
// the helper task of the file that starts the hit-stop effect after a
// delay (msw::CStopEffectBasket).
//
// The state flow follows CGame's state queue as VolleyF's
// game/volley/CGameVolley.cpp and BossF's game/boss/GameBoss.cpp use it.
#include "game/msw/CGameTask.hpp"
#include "game/msw/CGameBasket.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CPad.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"
#include "bcs/dwc/CDwc.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManBasket.hpp"
#include "game/msw/CCameraBasket.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitBasket.hpp"
#include "game/msw/CCollManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCtrlBasket.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CEventDemoBasket.hpp"
#include "game/msw/CFootmarkMan.hpp"
#include "game/msw/CGameBGM.hpp"
#include "game/msw/CGameBasketHungryBossPackun.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameGuideManBasket.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CGimmickManBasket.hpp"
#include "game/msw/CItemBall.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CNameMesg.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CShadowTexMan.hpp"
#include "game/msw/CStageBasket06.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageManBasket.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/TournamentBasket.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "nw4r/math/triangular.h"

// Set by the file's static initialiser and used by nothing in the file (as
// in game/basket/CGameBasketHungryBossPackun.cpp: most likely from a shared
// header).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace msw {
    namespace {

        // The states of CGame's state queue (the places of the
        // Ctor/Loop/Dtor triples in CGame's vtable plus two; names after
        // those functions).
        enum {
            STATE_GAME_SETUP = 3,
            STATE_WAIT_START = 4,
            STATE_MAIN_LOOP = 5,
            STATE_GOAL_EVENT = 6,
            STATE_TURN_RESULT = 7,
            STATE_TIME_UP = 8,
            STATE_TURN_OVER = 10,
            STATE_CHG_OFFENSE = 11,
            STATE_GAME_SET = 12,
            STATE_GAME_RESULT = 13,
            STATE_EVENT_DEMO = 19,
            STATE_ONLINE_END = 26 // (name guessed) queued in an online game
        };

        // The places of the telop table of GetTelopID_ (names guessed from
        // where they are shown).
        enum {
            TELOP_READY = 6,          // before the jump ball
            TELOP_TIME_UP = 8,
            TELOP_SHOT_CLOCK = 9,     // the shot clock ran out
            TELOP_GAME_SET = 11,
            TELOP_SCORE_COUNT = 12,
            TELOP_GOAL_0 = 14,        // a basket without coins
            TELOP_GOAL_1 = 15,        // with coins
            TELOP_GOAL_2 = 16,        // with ten coins or more
            TELOP_SHAKE_REMOCON = 17, // the "shake the remote" prompt of the endure
            TELOP_RETRY = 20          // a tournament mission's retry message
        };

        // The sounds of the table of PlaySnd_ (names guessed).
        enum {
            SND_GOAL = 0, // the stage's goal sound
            SND_START = 1,
            SND_BUZZER = 2,
            SND_GOAL_0 = 3,
            SND_GOAL_1 = 4,
            SND_GOAL_2 = 5,
            SND_SHOT_CLOCK = 7,
            SND_TIME_COUNT = 8
        };

        // The sound of place nSnd of the table of PlaySnd_; place 0 is the
        // stage's goal sound.
        inline long GetSndID(long nSnd)
        {
            long anSnd[9] = {0, 0xBF4, 0xBF5, 0xC00, 0xC02, 0xC01, 0xBFE, 0xBFB, 0xBFC};
            anSnd[SND_GOAL] = CStageMan::GetInstance()->GetGoalSE(static_cast<CStageMan::GOAL_SE_TYPE>(0));
            return anSnd[nSnd];
        }

        // The panel types of a stage's floor panels that colour the bonus
        // of a basket (names guessed).
        inline int IsPanelDouble(long nType)
        {
            int b = 1;
            if (nType != 2 && nType != 3) {
                b = 0;
            }
            return b;
        }

        inline int IsPanelBonus(long nType)
        {
            int b = 1;
            if (!IsPanelDouble(nType) && nType != 4) {
                b = 0;
            }
            return b;
        }

        // The cockpit's goal marker icons.
        enum {
            MARKER_ICON_0 = 0x100A,
            MARKER_ICON_1 = 0x100B,
            MARKER_ICON_2 = 0x100C
        };

        // The work memory of CGame's event demo state, as the game setup
        // fills it in (as in VolleyF's CGameVolley.cpp; only the words
        // written here are known).
        struct EVENT_DEMO_WORK {
            unsigned long nFlag; // 0x00
            long nUnk04;
            long nUnk08;         // 0x08
            long nUnk0C;         // 0x0C
            long nUnk10;
            long nUnk14;
            long nUnk18;         // 0x18
            float fUnk1C;        // 0x1C
        };

        // Starts the combination play of pChr (it tests the character again).
        static inline void StartCombinationPlay(CChrCoreBasket *pChr)
        {
            if (pChr != 0) {
                pChr->SetStateCombinationPlay(0);
            }
        }

        // Not zero while the shot clock runs: bit 0 of CChrManBasket's
        // flags, unless the game has no shot clock (bit 4).
        inline unsigned long IsShotClockRun()
        {
            CChrManBasket *pMan = CChrManBasket::GetInstance();
            if (pMan->IsNoShotClock()) {
                return 0;
            }
            return pMan->GetFlagAC() & 1;
        }

        // Not zero while the shot clock is shown: always in a game without
        // a time limit, else while the game time left is longer than it.
        inline int IsShotClockDisp()
        {
            int bDisp;
            if (0.0f == CGame::g_GameWork.m_fUnk20) {
                bDisp = 1;
            } else {
                bDisp = CGame::g_GameWork.m_fGameTime >= CChrManBasket::GetInstance()->GetClockTime_();
            }
            return bDisp;
        }

        // The yaw brought back into [0, 2pi) after half a turn.
        inline float TurnRotY(float fRot)
        {
            fRot += 3.1415927f;
            while (fRot > 6.2831855f) {
                fRot -= 6.2831855f;
            }
            while (fRot < 0.0f) {
                fRot += 6.2831855f;
            }
            return fRot;
        }

        inline long Clamp(long n, long nMin, long nMax)
        {
            return n > nMax ? nMax : (n < nMin ? nMin : n);
        }

        // The result positions of the characters (x, the yaw, z), by place
        // from the basket out; mirrored for the team on the other side.
        bcs::FVEC3 s_aResultPos[3] = {
            bcs::FVEC3(0.0f, 0.0f, 3.0f),
            bcs::FVEC3(2.0f, 0.0f, 2.0f),
            bcs::FVEC3(-2.0f, 0.0f, 2.0f),
        };
    }

    // Starts the hit-stop effect (CGame::StartStopEffect) after a delay.
    // Size 0x40.
    class CStopEffectBasket : public CGameTask {
        public:
            CStopEffectBasket(bcs::CTask *pParent, float fDelay, float fFrame, float fPower) : CGameTask(pParent, -1)
            {
                m_fDelay = fDelay;
                m_fFrame = fFrame;
                m_fPower = fPower;
            }
            virtual ~CStopEffectBasket() {}
            virtual void Main();

        private:
            float m_fDelay; // 0x34, frames left before the effect
            float m_fFrame; // 0x38
            float m_fPower; // 0x3C
    };

    CGameBasket::CGameBasket(bcs::CTask *pParent) : CGame(pParent, GAME_MODE_BASKET)
    {
        m_nArrowState = -1;
        m_nScoreArrowState = -1;
        m_nShotClockColor = -1;
        m_nShotClockCount = -1;
        m_nLastArrowState = 4;
        m_nExCameraType = 0;
        m_nGoalMarker = -1;
        m_nRiverFallTeam = -1;
        m_nGoalCoin = 0;
        m_nBasketFlag = 0;
        m_pInstance = this;
        Initialize_();
    }

    CGameBasket::~CGameBasket()
    {
        Finalize_();
    }

    void CGameBasket::Initialize_()
    {
        bcs::CPad::SetNunchukStickClamp(0xF, 0x5A, 0);
    }

    void CGameBasket::Finalize_()
    {
        CLytCtrl::Dispose();
        CLytCtrl::DetachResource();
        CLytCtrl::Close();
    }

    void CGameBasket::MainFunc()
    {
    }

    void CGameBasket::DispFunc()
    {
    }

    void CGameBasket::FinishFirstLoadData()
    {
    }

    void CGameBasket::GenerateInstance()
    {
        CShadowTexMan::CreateInstance(this);
        CFootmarkMan::CreateInstance(this);
        CCollManBasket::CreateInstance(this);
        CGimmickManBasket::CreateInstance(this);
        CChrManBasket::CreateInstance(this);
        CBallManBasket::CreateInstance(this);
        CEffectCtrlBasket::CreateInstance(this);
        CStageManBasket::CreateInstance(this, g_GameWork.m_nStage);
        CLytCtrl::CreateInstance(this);
        EvDemoBasket::CreateInstance(this);
        TournamentBasket::CreateInstance(this);
        CCockpitBasket::Create(this);
        CComManBasketGame::CreateInstance();
        menu::CSndEff::s_Instance.Initialize();
        menu::CVibEff::s_Instance.Initialize();
        CCameraBasket::Initialize();
        for (long i = 0; i < bcs::CWorldEnv::GetViewSplitNum(); i++) {
            m_apCamera[i] = CGameCamera::GetInstance(i);
        }
    }

    void CGameBasket::CtorStateGameInit()
    {
        m_nStateStep = 0;
        SetTurnStartTelopType_(1, 110.0f);
        g_GameWork.m_nFlag18 = 0;
        SetFirstGoalIdx_();
        GameInit_();
        SetPreparationMode(1);
        SetUpJumpBallerInfo();
    }

    void CGameBasket::CtorStateGameSetup()
    {
        g_GameWork.m_nFlag18 = 0;
        GameSetUp_();
        SetPreparationMode(1);
        SetCockpitDisp_(1);
        CCockpitBasket::SetPartMove(0, 3);
        CCockpitBasket::SetPartMove(1, 1);
        SetCockpitItemIconDisp_(0, 0);
        CGame::SetDispChrHeadIcon_(0);
        SetDispRadarIcon_(0);
        SetShotClockCount_(-1, 0);
        CChrMan::SetChrMarkerDisp(1);
        CCockpitBasket::SetTeamScore();
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            CCockpit::SetTeamCoin(i, GetTeamCoin(i));
            CCockpit::SetTeamScoreColor(i, 0);
            ClearTeamDisableDropCoin(i);
        }
        m_nGoalCoin = 0;
        StopReboundEffectCamera_(0);
        m_nLastArrowState = 4;
        m_nUnk148 = 1;
    }

    void CGameBasket::LoopStateGameSetup()
    {
        if (g_GameWork.m_nUnk180 == 0 && CEventDemo::IsEnable() && !(m_nBasketFlag & 4) && m_nUnk104 > 0) {
            EVENT_DEMO_WORK *pWork = static_cast<EVENT_DEMO_WORK *>(FsmqPushWork(STATE_EVENT_DEMO));
            pWork->nFlag |= 1;
            pWork->nUnk08 = 4;
            pWork->nUnk0C = m_nUnk108;
            pWork->nUnk18 = 3;
            pWork->fUnk1C = 30.0f;
            m_nUnk104 = 0;
            if (g_GameWork.GetUnk150() == 0) {
                SetTurnStartTelopType_(1, 0.0f);
            } else {
                SetTurnStartTelopType_(0, 0.0f);
            }
        }
        FsmqPush(STATE_WAIT_START);
        FsmqNext();
    }

    void CGameBasket::DtorStateGameSetup()
    {
    }

    void CGameBasket::CtorStateWaitStart()
    {
        ScrnFadeStart_(2, 4, 0.0625f);
        ScrnFadeStart_(1, 4, 0.0625f);
        SetReadyDemoMode(1);
        SetPreparationMode(0);
        m_nGameFlag = 0;
        m_fStateCount = 0.0f;
        m_nStateStep = 0;
    }

    // The ready telop or the jump ball, then the start.
    void CGameBasket::LoopStateWaitStart()
    {
        static const float fWaitFrame = 200.0f;

        switch (m_nStateStep) {
            case 0:
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(1), static_cast<CChrCore::STOP_MODE>(0));
                m_nStateStep++;
                break;
            case 1:
                if (IsScrnFadeEnd_(3)) {
                    m_fStateCount = 0.0f;
                    if (m_nTurnStartTelopType == 1) {
                        if (g_GameWork.GetUnk150() == 0) {
                            // The jump ball.
                            m_fStateCount = fWaitFrame - 10.0f;
                            CChrMan::NpcSendMessage(1, 2, 200, 0, 0);
                            m_nStateStep = 3;
                        } else {
                            CreateTelop_(GetTelopID_(TELOP_READY), 0, 0);
                            m_fStateCount = m_fTurnStartTelopFrame;
                            m_nStateStep = 2;
                        }
                    } else if (m_nBasketFlag & 4) {
                        CreateTelop_(GetTelopID_(TELOP_READY), 0, 0);
                        m_fStateCount = 110.0f;
                        m_nStateStep = 2;
                    } else {
                        CChrMan::NpcSendMessage(1, 4, 0, 0, 0);
                        m_nStateStep = 99;
                    }
                    SetTurnStartTelopType_(1, 110.0f);
                }
                break;
            case 2:
                if (StateWorkDecStateCount_()) {
                    EndTelop_(GetTelopID_(TELOP_READY));
                    m_fStateCount = 10.0f;
                    m_nStateStep++;
                }
                break;
            case 3:
                if (g_GameWork.GetUnk150() == 0) {
                    if (CGameGuideManBasket::IsJumpBallGuide() && m_fStateCount <= fWaitFrame * 0.5f) {
                        CGameGuideManBasket::StartJumpBall();
                    }
                }
                if (StateWorkDecStateCount_()) {
                    m_fStateCount = 10.0f;
                    m_nStateStep = 99;
                }
                break;
            default:
                if (StateWorkDecStateCount_()) {
                    if (g_GameWork.GetUnk150() != 0) {
                        CGame::SendMessage(0x2A, 0, 0, 0);
                    }
                    PlaySnd_(SND_START, 0);
                    FsmqChangeState(m_nNextState);
                    CChrManBasket::GetInstance()->ResetClockOver_();
                    if (g_GameWork.GetUnk150() == 1) {
                        CChrManBasket::GetInstance()->SetFlagAC(1);
                    }
                }
                break;
        }
    }

    void CGameBasket::DtorStateWaitStart()
    {
    }

    void CGameBasket::CtorStateMainLoop()
    {
        SetReadyDemoMode(0);
        SetPreparationMode(0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(0), static_cast<CChrCore::STOP_MODE>(0));
        CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(4), 0, 0, 0, 0);
        CGame::SendMessage(0x10, 0, 0, 0);
        m_nGameFlag &= ~1;
        CGame::SendMessage(0x16, 0, 0, 0);
        m_fUnk1C0 = 0.0f;
        SetCockpitDisp_(1);
        if (g_GameWork.m_nUnk40 & 1) {
            SetCockpitCoinDisp_(0);
        } else {
            SetCockpitCoinDisp_(1);
        }
        if (!(g_GameWork.m_nUnk40 & 8)) {
            SetCockpitArtsGaugeDisp_(1);
            CCockpitBasket::SetPartMove(2, 0);
        } else {
            SetCockpitArtsGaugeDisp_(0);
        }
        if (g_GameWork.m_fUnk20 > 0.0f) {
            SetCockpitTimeDisp_(1);
            CCockpitBasket::SetTime(g_GameWork.m_fGameTime, IsTimeColorChange());
            CCockpitBasket::SetPartMove(3, 0);
        } else {
            SetCockpitTimeDisp_(0);
        }
        CCockpitBasket::SetPeriod(g_GameWork.m_nPeriod);
        CCockpitBasket::SetPartMove(4, 0);
        SetDispRadarIcon_(1);
        SetCockpitItemIconDisp_(1, 1);
        CGame::SetDispChrHeadIcon_(1);
        CChrManBasket::GetInstance()->AllDispPlayerIcon_();
        m_nBasketFlag &= ~0x10;
    }

    void CGameBasket::LoopStateMainLoop()
    {
        CheckTimeUp__();
        UpDateShotClock_();
        if (m_nGameFlag & 1) {
            // Play has stopped: once the ball is dead, go on to what
            // stopped it.
            if (CChrMan::CanbeTurnOver()) {
                m_nGameFlag |= 8;
                if (m_nGameFlag & 0x20000) {
                    signed char nChr = -1;
                    if (m_nForceExitTeam >= 0) {
                        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                            CChrCore *pChr = CChrMan::GetChrCore(i);
                            if (pChr && !pChr->IsDisable() && m_nForceExitTeam == pChr->GetTeamNo()) {
                                nChr = pChr->GetPlayerNo();
                                break;
                            }
                        }
                    }
                    CChrMan::AllSetState(0, nChr, 0);
                    FsmqChangeState(STATE_TURN_OVER);
                } else if (m_nGameFlag & 2) {
                    FsmqChangeState(STATE_GOAL_EVENT);
                } else if (m_nGameFlag & 0x400) {
                    FsmqChangeState(STATE_TURN_OVER);
                } else if ((m_nGameFlag & 0x800) && !(m_nGameFlag & 0x1004)) {
                    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                        CChrCore *pChr = CChrMan::GetChrCore(i);
                        if (pChr && !pChr->IsDisable() && m_nRiverFallTeam != pChr->GetTeamNo()) {
                            CChrMan::AllSetState(0, i, 0);
                            break;
                        }
                    }
                    FsmqChangeState(STATE_TURN_OVER);
                } else {
                    m_nGameFlag &= ~0x1000;
                    FsmqChangeState(STATE_TIME_UP);
                }
                CChrManBasket::GetInstance()->AllClearPlayerIcon_(0);
                DispOffGoalMarker(1);
                SetShotClockCount_(-2, -1);
                CCockpitBasket::SetPartMove(2, 2);
                CCockpitBasket::SetPartMove(3, 2);
                CCockpitBasket::SetPartMove(4, 2);
                SetDispRadarIcon_(0);
                SetCockpitItemIconDisp_(0, 0);
                CGameBasket::GetInstance()->SetCheckSpEndure_(0);
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            }
        } else {
            m_fUnk1C0 += GetFrameRateF();
            if (m_fUnk1C0 >= 9999.9f) {
                m_fUnk1C0 = 9999.9f;
            }
            IncBallModeCounter(GetFrameRateF());
            IncCoinHistCount(GetFrameRateF());
            CheckSpEndure();
            CheckExceptionalCamera_();
            if (GetBasketFlag_() & 8) {
                // The rebound camera ends a while after it started, once the
                // ball is no longer rebounding.
                int bNoShoot = 1;
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall && pBall->IsNowRebound()) {
                    bNoShoot = 0;
                }
                if (bNoShoot) {
                    m_fReboundNoShoot += GetFrameRateF();
                }
                m_fReboundTime += GetFrameRateF();
                if (bNoShoot && m_fReboundTime >= 30.0f && m_fReboundNoShoot >= 50.0f) {
                    m_nBasketFlag |= 0x10;
                    StopReboundEffectCamera_(1);
                }
            } else if (m_nBasketFlag & 0x10) {
                m_nBasketFlag &= ~0x10;
                SetDispRadarIcon_(1);
                CGame::SetDispChrHeadIcon_(1);
            }
            if (m_nGameFlag & 0x800) {
                if (CheckChangeStateToRiverFall()) {
                    m_nGameFlag |= 1;
                }
            }
        }
    }

    void CGameBasket::DtorStateMainLoop()
    {
    }

    int CGameBasket::CheckTimeUp_()
    {
        CheckTimeUp__();
        return 0;
    }

    // Counts the game time down; at zero the game stops once no shot is in
    // the air.
    void CGameBasket::CheckTimeUp__()
    {
        static const long nID = 8;

        unsigned long nFlag = GetGameFlag_();
        if (!(nFlag & 2) && !(nFlag & 0x2000) && !IsReqArtsEvent_() && g_GameWork.m_fUnk20 > 0.0f) {
            float fTime = g_GameWork.m_fGameTime;
            if (fTime <= 0.0f) {
                if (g_GameWork.GetGameType() != GAME_TYPE_PARTY) {
                    CBall *pBall = CBallMan::GetBallPtr(0);
                    if (pBall && pBall->IsShootMode()) {
                        m_nGameFlag |= 0x1000;
                        return;
                    }
                }
                if (CChrManBasket::GetInstance()->GetFlagAC() & 2) {
                    m_nGameFlag |= 0x1000;
                    return;
                }
                m_nGameFlag |= 5;
                return;
            }
            SetTime(fTime - GetFrameRateF());
            long nNow = g_GameWork.m_fGameTime / 60.0f;
            long nPrev = fTime / 60.0f;
            if (nNow < 9 && nNow != nPrev) {
                PlaySnd_(nID, 0);
            }
            if (nPrev == 13 && nNow != nPrev) {
                CGameBGM::HurryUp();
            }
            CCockpitBasket::SetTime(g_GameWork.m_fGameTime, IsTimeColorChange());
            if (fTime <= 900.0f && !(GetGameFlag_() & 0x200)) {
                m_nGameFlag |= 0x200;
            }
        }
    }

    void CGameBasket::UpDateShotClock_()
    {
        if (!(m_nGameFlag & 2) && !(m_nGameFlag & 0x2000) && !IsReqArtsEvent_()) {
            float fRate = GetFrameRateF();
            CChrManBasket::GetInstance()->UpDateShotClockOver_(fRate);
            DispShotClock_();
        }
    }

    void CGameBasket::CtorStateGoalEvent()
    {
        CGame::CtorStateGoalEvent();
        m_fStateCount = 120.0f;
    }

    void CGameBasket::LoopStateGoalEvent()
    {
        if (m_nStateStep == 0) {
            if (IsEndGoalDemo_()) {
                EndGoalDemo_();
                StartGoalTelop_();
                SetCockpitMapDisp_(0);
                m_nStateStep++;
            }
        } else if (StateWorkDecStateCount_()) {
            // Wait for the characters still in a goal state.
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable()) {
                    long nState = pChr->GetState();
                    if ((nState >= 0x26 && nState <= 0x28) || nState == 0x24) {
                        return;
                    }
                }
            }
            CChrMan::AllSetState(0, g_GameWork.m_nGoalChar, g_GameWork.m_nGoalOwnGoal ? -1 : 0);
            FsmqChangeState(STATE_TURN_RESULT);
        }
    }

    void CGameBasket::DtorStateGoalEvent()
    {
        CGame::DtorStateGoalEvent();
    }

    void CGameBasket::CtorStateTurnResult()
    {
        CGame::CtorStateTurnResult();
    }

    void CGameBasket::LoopStateTurnResult()
    {
        switch (m_nStateStep) {
            case 0:
                if (CanbeScoredView_()) {
                    StartScoredView_(-1);
                    if (g_GameWork.m_fUnk20 > 0.0f && g_GameWork.m_fGameTime <= 0.0f) {
                        m_nGameFlag |= 4;
                    }
                    if (m_Unk130 > 0) {
                        CTournament::SendMessage(0x22, 0, 0, 0);
                    }
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    IsGameSet_();
                    m_fStateCount = 60.0f;
                    m_nStateStep++;
                }
                break;
            default:
                if (StateWorkDecStateCount_()) {
                    if (IsGameSet_() && !(m_nGameFlag & 2)) {
                        FsmqChangeState(STATE_GAME_SET);
                    } else if (g_GameWork.m_fUnk20 > 0.0f) {
                        if (g_GameWork.m_fGameTime <= 0.0f && !(m_nGameFlag & 2)) {
                            FsmqChangeState(STATE_TIME_UP);
                        } else {
                            FsmqChangeState(STATE_TURN_OVER);
                        }
                    } else {
                        FsmqChangeState(STATE_TURN_OVER);
                    }
                }
                break;
        }
    }

    void CGameBasket::DtorStateTurnResult()
    {
        CGame::DtorStateTurnResult();
    }

    // The end of a period.
    void CGameBasket::CtorStateTimeUp()
    {
        if (m_Unk130 > 0) {
            CChrMan::AllSetState(1, -1, 0);
            m_fStateCount = 90.0f;
            m_nStateStep = 99;
            return;
        }
        CChrMan::AllSetState(1, -1, 0);
        if (!IsGameSet_()) {
            PlaySnd_(SND_BUZZER, 0);
        }
        if (m_nGameFlag & 0x1000) {
            m_nStateStep = 3;
            return;
        }
        m_fStateCount = 30.0f;
        m_nStateStep = 0;
    }

    void CGameBasket::LoopStateTimeUp()
    {
        switch (m_nStateStep) {
            case 0:
                if (StateWorkDecStateCount_()) {
                    RemoveTelop_(GetTelopID_(TELOP_SCORE_COUNT));
                    CreateTelop_(GetTelopID_(TELOP_TIME_UP), 0, 0);
                    m_fStateCount = 60.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    EndTelop_(GetTelopID_(TELOP_TIME_UP));
                    m_nStateStep++;
                }
                break;
            case 2:
                if (!IsTelopEnd_(GetTelopID_(TELOP_TIME_UP))) {
                    break;
                }
                m_nStateStep++;
                // fall through
            case 3:
                if (IsGameSet_()) {
                    FsmqChangeState(STATE_GAME_SET);
                } else {
                    m_fStateCount = 60.0f;
                    m_nStateStep++;
                }
                break;
            case 4:
                ScrnFadeStart_(2, 0, 0.0625f);
                m_nStateStep++;
                break;
            case 99:
                if (StateWorkDecStateCount_()) {
                    FsmqChangeState(STATE_GAME_SET);
                    m_nStateStep++;
                }
                break;
            case 100:
                break;
            default:
                if (IsScrnFadeEnd_(3)) {
                    RemoveTelop_(GetTelopID_(TELOP_SCORE_COUNT));
                    CGame::SendMessage(0x17, 0, 0, 0);
                    EndScoredView_();
                    if (m_Unk130 > 0) {
                        FsmqChangeState(STATE_GAME_SET);
                    } else {
                        ChgCourtDemoSetup_();
                    }
                }
                break;
        }
    }

    void CGameBasket::DtorStateTimeUp()
    {
        RemoveTelop_(GetTelopID_(TELOP_SCORE_COUNT));
        RemoveTelop_(GetTelopID_(TELOP_TIME_UP));
    }

    void CGameBasket::CtorStateChgCourt()
    {
        CGame::CtorStateChgCourt();
        for (long i = 0; i < 4; i++) {
            if (GetPeriodStartCoin(i) >= 0) {
                SetTeamCoin(i, GetPeriodStartCoin(i));
            }
        }
        CCockpitBasket::SetTime(g_GameWork.m_fUnk20, IsTimeColorChange());
        CCockpitBasket::SetPeriod(g_GameWork.m_nPeriod);
        GetInstance()->SetOffenseTeam_(-1);
        SetTurnStartMode(GAME_TURN_START_MODE_0);
        SetTurnStartTelopType_(1, 110.0f);
        ResetSPGauge_();
    }

    void CGameBasket::LoopStateChgCourt()
    {
        CGame::LoopStateChgCourt();
    }

    void CGameBasket::DtorStateChgCourt()
    {
        CGame::DtorStateChgCourt();
        SendMessage_(0x12, 0, 0, 0);
    }

    void CGameBasket::CtorStateTurnOver()
    {
        m_fStateCount = 120.0f;
        if (g_GameWork.m_fUnk20 > 0.0f && g_GameWork.m_fGameTime <= 0.0f) {
            m_nGameFlag |= 4;
        }
        m_nStateStep = 0;
    }

    // After a basket: the telop of the basket, the score count and the
    // change of offense. After the shot clock or a fall into the river:
    // their own steps first. After a tournament mission's failed shot: its
    // retry message.
    void CGameBasket::LoopStateTurnOver()
    {
        if (m_nGameFlag & 0x60000) {
            if (m_nStateStep == 0) {
                if (!IsEndGoalDemo_()) {
                    return;
                }
                EndGoalDemo_();
                if (m_Unk130 > 0) {
                    m_nStateStep = 0x5A;
                } else if (m_nGameFlag & 0x80000) {
                    m_nStateStep = 0x63;
                    if (m_nGameFlag & 0x20000) {
                        m_fStateCount = 60.0f;
                    } else {
                        m_fStateCount = 90.0f;
                    }
                } else {
                    m_nStateStep = 0x65;
                    if (m_nGameFlag & 0x20000) {
                        m_fStateCount = 120.0f;
                    } else {
                        m_fStateCount = 90.0f;
                    }
                }
            }
        } else if (m_nGameFlag & 0x400) {
            if (!LoopStateTurnOverToShotClock()) {
                return;
            }
            m_nGameFlag &= ~0x400;
            CTournament::ShotClockTimeOver();
            CGame::SendMessage(0x17, 0, 0, 0);
            m_nStateStep = 3;
        } else if (m_nGameFlag & 0x800) {
            if (!LoopStateTurnOverToRiverFall()) {
                return;
            }
            GetInstance()->SetOffenseTeam_(m_nRiverFallTeam);
            m_nGameFlag &= ~0x800;
            CGame::SendMessage(0x17, 0, 0, 0);
            m_nStateStep = 3;
        }

        switch (m_nStateStep) {
            case 0:
                if (IsTelopEnd_(GetTelopID_(m_nGoalTelop))) {
                    EndTelop_(GetTelopID_(m_nGoalTelop));
                    SetDispScoreCountResult_();
                    m_fStateCount = 2.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    if (DemoSkipInputCheck(-1) >= 0) {
                        long nTeam = g_GameWork.m_nGoalTeam;
                        CCockpit::AddTeamScore(nTeam, GetTeamScore(nTeam));
                        RemoveTelop_(GetTelopID_(TELOP_SCORE_COUNT));
                        if (m_nGameFlag & 4) {
                            FsmqChangeState(STATE_TIME_UP);
                        } else {
                            m_nStateStep += 2;
                        }
                    } else if (IsTelopEndStartAnim_(GetTelopID_(TELOP_SCORE_COUNT))) {
                        long nTeam = g_GameWork.m_nGoalTeam;
                        CCockpit::AddTeamScore(nTeam, GetTeamScore(nTeam));
                        m_fStateCount = 60.0f;
                        m_nStateStep++;
                    }
                }
                break;
            case 2:
                if (StateWorkDecStateCount_() || DemoSkipInputCheck(-1) >= 0) {
                    if (m_nGameFlag & 4) {
                        EndTelop_(GetTelopID_(TELOP_SCORE_COUNT));
                        FsmqChangeState(STATE_TIME_UP);
                    } else {
                        m_nStateStep++;
                    }
                }
                break;
            case 0x5A:
                EndTelop_(GetTelopID_(TELOP_SCORE_COUNT));
                FsmqChangeState(STATE_GAME_SET);
                m_nStateStep++;
                break;
            case 0x63:
                if (StateWorkDecStateCount_()) {
                    CreateTelop_(GetTelopID_(TELOP_RETRY), 0, 0);
                    CNameMesg::SetRetryMessage(GetTelopID_(TELOP_RETRY), (long)g_GameWork.m_fGameTime);
                    m_fStateCount = 2.0f;
                    m_nStateStep++;
                }
                break;
            case 0x64:
                if (StateWorkDecStateCount_()) {
                    if (IsTelopEndStartAnim_(GetTelopID_(TELOP_RETRY))) {
                        m_fStateCount = 60.0f;
                        m_nStateStep++;
                    }
                }
                break;
            case 0x65:
                if (StateWorkDecStateCount_()) {
                    EndTelop_(GetTelopID_(TELOP_RETRY));
                    m_fStateCount = 2.0f;
                    m_nStateStep++;
                }
                break;
            case 0x66:
                if (StateWorkDecStateCount_() && IsTelopEndStartAnim_(GetTelopID_(TELOP_RETRY))) {
                    if (m_nGameFlag & 4) {
                        EndTelop_(GetTelopID_(TELOP_SCORE_COUNT));
                        FsmqChangeState(STATE_TIME_UP);
                    } else {
                        m_nStateStep++;
                    }
                }
                break;
            default:
                bcs::CRenderMan::StartCrossFade(CLoadData::GetWipeMdlFile(), "cutchange01", 2.0f, 1);
                EndScoredView_();
                FsmqChangeState(STATE_CHG_OFFENSE);
                break;
        }
    }

    // The shot clock telop; not zero once it has gone.
    int CGameBasket::LoopStateTurnOverToShotClock()
    {
        switch (m_nStateStep) {
            case 0:
                CreateTelop_(GetTelopID_(TELOP_SHOT_CLOCK), 1, 0);
                m_nStateStep++;
                // fall through
            case 1:
                if (IsTelopEnd_(GetTelopID_(TELOP_SHOT_CLOCK))) {
                    RemoveTelop_(GetTelopID_(TELOP_SHOT_CLOCK));
                    m_nStateStep++;
                } else {
                    return 0;
                }
                // fall through
            default:
                return 1;
        }
    }

    // A wait after a fall into the river; not zero once it is over.
    int CGameBasket::LoopStateTurnOverToRiverFall()
    {
        switch (m_nStateStep) {
            case 0:
                m_fStateCount = 120.0f;
                m_nStateStep++;
                // fall through
            case 1:
                if (StateWorkDecStateCount_()) {
                    m_nStateStep++;
                } else {
                    return 0;
                }
                // fall through
            default:
                return 1;
        }
    }

    void CGameBasket::DtorStateTurnOver()
    {
        RemoveTelop_(GetTelopID_(TELOP_RETRY));
    }

    void CGameBasket::CtorStateChgOffense()
    {
        SetTurnStartMode(static_cast<GAME_TURN_START_MODE>(1));
        SetReplayMode(0);
        SetResultMode(0);
        SetPreparationMode(1);
        m_nStateStep = 0;
        m_fStateCount = 2.0f;
        bcs::CRenderMan::StartCrossFade(CLoadData::GetWipeMdlFile(), "cutchange01", 4.0f, 1);
        RemoveTelop_(GetTelopID_(TELOP_SCORE_COUNT));
        EndScoredView_();
        CChrMan::AllMotStop();
        long nCoin = g_GameWork.m_Unk160;
        long nTeam = g_GameWork.m_nGoalTeam;
        AddTeamGainCoin(nTeam, nCoin);
        unsigned long nTeamCoin = GetTeamCoin(nTeam) - nCoin;
        SetTeamCoin(nTeam, nTeamCoin > 100 ? 100 : nTeamCoin);
        GetInstance()->SetOffenseTeam_(1 - g_GameWork.GetUserTeam());
    }

    void CGameBasket::LoopStateChgOffense()
    {
        if (m_Unk130 > 0) {
            FsmqChangeState(STATE_GAME_SET);
        } else {
            FsmqChangeState(STATE_GAME_SETUP);
        }
    }

    void CGameBasket::DtorStateChgOffense()
    {
    }

    void CGameBasket::CtorStateGameSet()
    {
        if (m_Unk130 > 0) {
            m_fStateCount = 90.0f;
            m_nStateStep = 99;
            return;
        }
        m_fStateCount = 30.0f;
        m_nStateStep = 0;
    }

    void CGameBasket::LoopStateGameSet()
    {
        switch (m_nStateStep) {
            case 0:
                if (StateWorkDecStateCount_()) {
                    CreateTelop_(GetTelopID_(TELOP_GAME_SET), 0, 0);
                    PlaySnd_(SND_BUZZER, 0);
                    m_fStateCount = 60.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    EndTelop_(GetTelopID_(TELOP_GAME_SET));
                    m_nStateStep++;
                }
                break;
            case 2:
                if (IsTelopEnd_(GetTelopID_(TELOP_GAME_SET))) {
                    ScrnFadeStart_(2, 0, 0.0625f);
                    m_nStateStep++;
                }
                break;
            case 99:
                if (StateWorkDecStateCount_()) {
                    ScrnFadeStart_(2, 0, 0.0625f);
                    m_nStateStep++;
                }
                break;
            default:
                if (IsScrnFadeEnd_(3)) {
                    CGame::SendMessage(0x17, 0, 0, 0);
                    StopReboundEffectCamera_(0);
                    EndScoredView_();
                    FsmqCutNext();
                    if (bcs::dwc::CDwc::GetInstance()) {
                        FsmqPush(STATE_ONLINE_END);
                    }
                    FsmqPush(STATE_GAME_RESULT);
                    FsmqNext();
                }
                break;
        }
    }

    void CGameBasket::DtorStateGameSet()
    {
        RemoveTelop_(GetTelopID_(TELOP_GAME_SET));
        RemoveTelop_(GetTelopID_(TELOP_SCORE_COUNT));
    }

    void CGameBasket::CtorStateGameResult()
    {
        SetShotClockCount_(-1, 0);
        CGame::CtorStateGameResult();
    }

    void CGameBasket::LoopStateGameResult()
    {
        CGame::LoopStateGameResult();
    }

    void CGameBasket::DtorStateGameResult()
    {
        CGame::DtorStateGameResult();
    }

    int CGameBasket::SetGameResultNext_()
    {
        return CGame::SetGameResultNext_();
    }

    void CGameBasket::PrepareRestartGame_()
    {
        CCockpitBasket::SetMenuState(1);
        ScrnSetBlackOut_();
        g_GameWork.m_nFlag18 = 0;
    }

    int CGameBasket::IsGameSet_()
    {
        if (m_Unk130 > 0) {
            return 1;
        }
        if ((m_nGameFlag & 4) && g_GameWork.m_nPeriod >= g_GameWork.m_Unk0B - 1) {
            return 1;
        }
        return 0;
    }

    void CGameBasket::SetGameResult_()
    {
        SetPeriodResult_();
        ClearGameResult_();
        long nWin;
        if (GetTeamScore(0) == GetTeamScore(1)) {
            nWin = -1;
        } else {
            nWin = GetTeamScore(0) <= GetTeamScore(1);
        }
        if (nWin < 0) {
            SetGameResultAll_(3);
            return;
        }
        SetGameResultAll_(2);
        SetTeamResult(nWin, 1);
    }

    // The skill points: 50 for a win, 10 for a draw, -50 for a loss, plus
    // up to 50 by the score difference (full at a hundred points).
    void CGameBasket::CalcAddSkillPoint_()
    {
        if (g_GameWork.m_Unk151) {
            // (static-order swap: the original has these in reverse order)
            static const long nWinSkillPt = 50;
            static const long nWinMargin = 100;
            static const long nWinMarginPt = 50;
            for (long nTeam = 0; nTeam < 2; nTeam++) {
                long nDiff = GetTeamScore(nTeam) - GetTeamScore(1 - nTeam);
                long nPoint;
                if (nDiff >= 1) {
                    nPoint = nWinSkillPt;
                } else if (nDiff == 0) {
                    nPoint = 10;
                } else {
                    nPoint = -nWinSkillPt;
                }
                long nMargin = Clamp(nDiff, -nWinMargin, nWinMargin);
                SetTeamSkillPoint(nTeam, nPoint + (long)((float)(nMargin * nWinMarginPt) / nWinMargin));
            }
        }
    }

    // The points of a basket of shot type nShootType.
    long CGameBasket::GetShootPointType_(long nChr, long nShootType) const
    {
        switch (nShootType) {
            case 1:
                return 3;
            case 0:
            case 2:
            case 6:
            case 7:
                return 2;
            case 3:
            case 5:
            case 8:
                return 2;
            case 9:
            case 10:
            case 11: {
                CChrCore *pChr = CChrMan::GetChrCore(nChr);
                if (pChr && pChr->GetChrId() == 6) {
                    return 1;
                }
                return 3;
            }
            default:
                return 0;
        }
    }

    // The hit stop of a basket.
    void CGameBasket::StopEffectGoalIn_(long nShootType)
    {
        long nDelay;
        long nFrame;
        long nPower;
        switch (nShootType) {
            case 6:
                nDelay = 8;
                nFrame = 4;
                nPower = 35;
                break;
            case 3:
                nDelay = 10;
                nFrame = 5;
                nPower = 60;
                break;
            default:
                nDelay = 8;
                nFrame = 4;
                nPower = 30;
                break;
        }
        // (one statement per conversion: the original calls this function,
        // -inline auto goes by statement count)
        float fDelay = nDelay;
        float fFrame = nFrame;
        float fPower = nPower;
        GetInstance()->StopEffect_(fDelay, fFrame, fPower);
    }

    long CGameBasket::GetFirstGoalIdxMode_()
    {
        return -1;
    }

    void CGameBasket::SetDispRadarIcon_(int bDisp)
    {
        CCockpitBasket::SetRadarIconDisp(bDisp);
    }

    void CGameBasket::SetCockpitCoinGetTelopMove_(long nTeam, long nState)
    {
        CCockpitBasket::SetCoinTelopState(nTeam, nState);
    }

    void CGameBasket::SetCockpitDisp_(int bDisp)
    {
        CCockpitBasket::SetDisp(bDisp);
    }

    void CGameBasket::SetCockpitCoinDisp_(int bDisp)
    {
        CCockpitBasket::SetCoinDispEnable(bDisp);
    }

    void CGameBasket::SetCockpitTimeDisp_(int bDisp)
    {
        CCockpitBasket::SetTimeDispEnable(bDisp);
    }

    void CGameBasket::SetCockpitScoreDisp_(int bDisp)
    {
        CCockpitBasket::SetScoreDispEnable(bDisp);
    }

    void CGameBasket::SetCockpitPeriodDisp_(int bDisp)
    {
        CCockpitBasket::SetPeriodDispEnable(bDisp);
    }

    // The score count after a basket: the score before and the points of
    // the basket. Then each team's score up to now for the next one.
    void CGameBasket::SetDispScoreCountResult_()
    {
        CreateTelop_(GetTelopID_(TELOP_SCORE_COUNT), 1, 1);
        CCockpit::SetTeamCoin(g_GameWork.m_nGoalTeam, g_GameWork.m_Unk160);
        CCockpitBasket::SetTelop3DScoreCount(m_anTeamScore[0], m_anTeamScore[1], g_GameWork.m_nGoalTeam,
                                             g_GameWork.m_nGoalBaseScore, g_GameWork.m_Unk160,
                                             g_GameWork.m_nGoalBonusType, g_GameWork.m_nGoalBonusPoint,
                                             g_GameWork.m_nGoalBonusColor, 0, -1);
        CGameSnd::Play(0xBF6, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        for (long nTeam = 0; nTeam < 4; nTeam++) {
            m_anTeamScore[nTeam] = 0;
            for (long i = 0; i <= g_GameWork.m_nPeriod; i++) {
                m_anTeamScore[nTeam] += GetTeamPeriodScore(nTeam, i);
            }
        }
    }

    void CGameBasket::SetCockpitArtsGaugeRatio_(long nTeam, float fRatio, float fTelop)
    {
        CCockpitBasket::SetSPGaugeRatio(nTeam, fRatio, fTelop);
    }

    void CGameBasket::SetCockpitArtsGaugeSpAnim_(long nTeam, int bSpecial)
    {
        CCockpitBasket::SetCoinSpecialAnim(nTeam, bSpecial);
        CCockpitBasket::SetSPContrllerDisp(nTeam, bSpecial);
    }

    // The result screen: each team's score per period and who won it.
    int CGameBasket::MenuCallGameResult_()
    {
        CCockpitBasket::SetMenuState(4);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        nPeriod = Clamp(nPeriod, 0, 4);
        nPeriodNum = Clamp(nPeriodNum, 1, 5);
        long anScore0[5];
        long anScore1[5];
        long anWin0[5];
        long anWin1[5];
        for (long i = 0; i < 5; i++) {
            anScore0[i] = 0;
            anScore1[i] = 0;
            anWin0[i] = 0;
            anWin1[i] = 0;
        }
        for (long i = 0; i < nPeriodNum; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriod + 1, nPeriodNum, anScore0, anScore1, anWin0, anWin1, 0, 0, -1);
        return 1;
    }

    // The same between periods.
    int CGameBasket::MenuCallInterval_()
    {
        CCockpitBasket::SetMenuState(5);
        CCockpit::ClearResult(-1);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        nPeriod = Clamp(nPeriod, 0, 4);
        nPeriodNum = Clamp(nPeriodNum, 1, 5);
        long anScore0[5];
        long anScore1[5];
        long anWin0[5];
        long anWin1[5];
        for (long i = 0; i < 5; i++) {
            anScore0[i] = 0;
            anScore1[i] = 0;
            anWin0[i] = 0;
            anWin1[i] = 0;
        }
        for (long i = 0; i < nPeriodNum; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriod, nPeriodNum, anScore0, anScore1, anWin0, anWin1, 0, 0, -1);
        return 1;
    }

    // The same in the pause menu.
    int CGameBasket::MenuCallPause_()
    {
        CCockpitBasket::SetMenuState(3);
        CCockpit::ClearResult(-1);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        nPeriod = Clamp(nPeriod, 0, 4);
        nPeriodNum = Clamp(nPeriodNum, 1, 5);
        long anScore0[5];
        long anScore1[5];
        long anWin0[5];
        long anWin1[5];
        for (long i = 0; i < 5; i++) {
            anScore0[i] = 0;
            anScore1[i] = 0;
            anWin0[i] = 0;
            anWin1[i] = 0;
        }
        for (long i = 0; i < nPeriodNum; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriod, nPeriodNum, anScore0, anScore1, anWin0, anWin1, 0, 0, -1);
        return 1;
    }

    // The arrow towards the goal the player's team attacks, the goal marker
    // and the score board's arrow.
    void CGameBasket::CockpitCheckArrowState_()
    {
        long nState = 4;
        if (!(g_GameWork.m_nFlag18 & 1) && !(g_GameWork.m_nFlag18 & 8)) {
            switch (g_GameWork.m_nUserTeam) {
                case 0:
                    if (GetViewTargetTeam(0) == 0) {
                        nState = 0;
                    } else {
                        nState = 1;
                    }
                    break;
                case 1:
                    nState = GetViewTargetTeam(0) == 1 ? 2 : 3;
                    break;
            }
        }
        if (nState != m_nArrowState && !(g_GameWork.m_nFlag18 & 0x4000) && !(m_nBasketFlag & 8)) {
            if (m_nLastArrowState != nState && nState != 4) {
                if (GetState() == STATE_MAIN_LOOP) {
                    GetInstance()->PlayExceptionalCameraSlow_(2);
                    DispOffGoalMarker(1);
                    DispOnGoalMarker();
                }
            }
            CCockpitBasket::SetArrowState(nState);
            m_nArrowState = nState;
            if (nState != 4) {
                m_nLastArrowState = nState;
            }
            unsigned long bNoMarker = g_GameWork.m_nFlag18 & 1;
            if (bNoMarker && (m_nBasketFlag & 2)) {
                DispOffGoalMarker(1);
            } else if (!bNoMarker && !(m_nBasketFlag & 2)) {
                DispOnGoalMarker();
            }
        }
        DispUpDateGoalMaker();
        long nScoreState = 0;
        if (GetViewTargetTeam(0) == 1) {
            nScoreState = 1;
        }
        if (nScoreState != m_nScoreArrowState) {
            CCockpitBasket::SetScoreArrowState(nScoreState);
            m_nScoreArrowState = nScoreState;
        }
    }

    void CGameBasket::ScoreBookInitCountWork_()
    {
        CGame::ScoreBookInitCountWork_();
        for (long i = 0; i < 10; i++) {
            m_anScoreBook[i] = 0;
        }
    }

    // Counts the shots of the main loop: dunks, layups, throws (with the
    // three-pointers) and blocks, each made and missed.
    void CGameBasket::ScoreBookAddCountWork_(long nKind, long, long)
    {
        if (IsNowMainLoopEnable_()) {
            switch (nKind) {
                case 3:
                    m_anScoreBook[0]++;
                    break;
                case 4:
                    m_anScoreBook[1]++;
                    break;
                case 5:
                    m_anScoreBook[2]++;
                    break;
                case 6:
                    m_anScoreBook[3]++;
                    break;
                case 7:
                    m_anScoreBook[4]++;
                    break;
                case 8:
                    m_anScoreBook[5]++;
                    break;
                case 9:
                    m_anScoreBook[6]++;
                    m_anScoreBook[4]++;
                    break;
                case 10:
                    m_anScoreBook[7]++;
                    m_anScoreBook[5]++;
                    break;
                case 11:
                    m_anScoreBook[8]++;
                    break;
                case 12:
                    m_anScoreBook[9]++;
                    break;
            }
        }
    }

    void CGameBasket::ScoreBookApplyCountWork_()
    {
        CGame::ScoreBookApplyCountWork_();
        SAVE_DATA_PLAYER *pPlayer = CSave::GetPlayerData(-1);
        if (pPlayer) {
            pPlayer->SetBasketDunkScore(m_anScoreBook[0], m_anScoreBook[1]);
            pPlayer->SetBasketLayupScore(m_anScoreBook[2], m_anScoreBook[3]);
            pPlayer->SetBasketThrowingScore(m_anScoreBook[4], m_anScoreBook[5]);
            pPlayer->SetBasketThreePointScore(m_anScoreBook[6], m_anScoreBook[7]);
            pPlayer->SetBasketBlockScore(m_anScoreBook[8], m_anScoreBook[9]);
        }
    }

    void CGameBasket::AddTeamCoin_(long nTeam, long nCoin)
    {
        CGame::AddTeamCoin_(nTeam, nCoin);
    }

    void CGameBasket::TrgBasketGoalIn_(long nChr, long nTeam, long nShootType, long nBall, float fRate,
                                       const bcs::FVEC3 &rPos, long nBallIdx)
    {
        TrgBasketGoalIn__(nChr, nTeam, nShootType, nBall, fRate, rPos, nBallIdx, 1, 1);
        SetReboundMode(0);
    }

    // A basket. With bDemo the goal demo or the quake follows; with bCoin
    // the team's coins are scored with it.
    void CGameBasket::TrgBasketGoalIn__(long nChr, long nTeam, long nShootType, long, float fRate,
                                        const bcs::FVEC3 &rPos, long nBallIdx, int bDemo, int bCoin)
    {
        if (!IsNowMainLoopEnable_()) {
            return;
        }
        if (m_nGameFlag & 0x1000) {
            if (nShootType == 3 || nShootType == 5 || nShootType == 8) {
                return;
            }
        }
        SetGoalShootType(nShootType, fRate);
        SetGoalChar(nChr);
        SetGoalTeam(nTeam);
        long nRetry = CTournament::IsMissionShootFalseRetry(nTeam, nShootType);
        if (nRetry > 0) {
            // A tournament mission does not count this kind of shot.
            DispOffGoalMarker(1);
            SetGoalFinishMode(0);
            SetGoalEventMode(0);
            SetGoalOwnGoal(1);
            m_nGameFlag |= 0x40000 | 3;
            if (nRetry == 4) {
                m_nGameFlag |= 0x80000;
            }
            return;
        }
        if (nShootType >= 9 && nShootType <= 11) {
            CBall *pBall = CBallMan::GetBallPtr(nBallIdx);
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr->GetChrId() == 6 && pBall && static_cast<CBallBasket *>(pBall)->IsDummy()) {
                // A dummy ball of character 6's special shot: its coins and
                // points wait for the real ball.
                long nCoin = GetTeamCoin(nTeam);
                SetGoalCoinEffect(nCoin, nChr, nTeam, nShootType, rPos);
                SetTeamDisableDropCoin(nTeam, nCoin);
                m_nShootPoint += GetShootPointType_(nChr, nShootType);
                m_nGameFlag |= 0x8000;
                return;
            }
        }
        CBall *pBall = CBallMan::GetBallPtr(nBallIdx);
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        m_nGameFlag &= ~0x8000;
        CGame::SendMessage(0x36, nChr, nShootType, AddTeamScore_(nChr, nTeam, nShootType, rPos, bCoin));
        CGame::SendMessage(0x37, nBallIdx, 0, 0);
        m_nGameFlag |= 1;
        if (!(m_nGameFlag & 2)) {
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            if (g_GameWork.m_nGameMode == GAME_MODE_BASKET) {
                CChrMan::ReserveAllSetState(0, nChr, g_GameWork.m_nGoalOwnGoal ? -1 : 0);
            }
        }
        DispOffGoalMarker(1);
        m_nGameFlag |= 2;
        SetGoalFinishMode(1);
        SetGoalEventMode(1);
        m_nGameFlag &= ~0x800;
        if (bDemo) {
            m_nGoalBallType = 0;
            if (pBall) {
                if (static_cast<CBallBasket *>(pBall)->GetBasketFlag() & 0x2000) {
                    m_nGoalBallType = 2;
                } else if (pBall->IsItemEffect()) {
                    m_nGoalBallType = 1;
                }
            }
            long nDemo = GetGoalDemoType_(nTeam, nShootType);
            long nTelop = GetGoalDemoTelop_(g_GameWork.m_nGoalScore, nShootType, g_GameWork.m_Unk160, 0);
            if (nDemo >= 0) {
                StartGoalDemo_(0, nDemo, rPos, GetGoalIdx(nTeam), nTelop, 0.35f);
                m_nUnk11C = 0;
            } else {
                PlayTrGoalInQuake(pChr, pBall);
                StopEffectGoalIn_(nShootType);
                m_nUnk11C++;
            }
        }
        CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(9), nChr, nShootType, 0, 0);
    }

    // (inline in the original, emitted here after its first caller)
    int CBallBasket::IsDummy() const
    {
        return 0;
    }

    // The telop and sound of a basket, by the coins scored with it.
    void CGameBasket::StartGoalTelop_()
    {
        if (!(m_nGameFlag & 0x40000)) {
            long nSnd;
            if (g_GameWork.m_Unk160 == 0) {
                m_nGoalTelop = TELOP_GOAL_0;
                nSnd = SND_GOAL_0;
            } else if (g_GameWork.m_Unk160 >= 10) {
                m_nGoalTelop = TELOP_GOAL_2;
                nSnd = SND_GOAL_2;
            } else {
                m_nGoalTelop = TELOP_GOAL_1;
                nSnd = SND_GOAL_1;
            }
            CreateTelop_(GetTelopID_(m_nGoalTelop), 1, 0);
            PlaySnd_(nSnd, 0);
        }
    }

    // The coins flying out of the goal.
    // (written one step per statement: the original does not inline it into
    // its callers, -inline auto goes by statement count)
    long CGameBasket::DoGoalEffect_(long nChr, long nTeam, long nShootType, unsigned long nCoin,
                                    const bcs::FVEC3 &rPos)
    {
        CChrManBasket *pMan = CChrManBasket::GetInstance();
        if (pMan->IsCreateBossPackun_()) {
            return 0;
        }
        CChrCore *pChr = 0;
        if (nShootType >= 9 && nShootType <= 11) {
            // (the character is looked up and not used)
            pChr = CChrMan::GetChrCore(nChr);
        }
        if (nCoin) {
            bcs::FVEC3 dir;
            bcs::FVEC3 pos = rPos;
            long nTeamGoal = nTeam;
            long nGoal = GetGoalIdx(nTeamGoal);
            float fRot = GetGoalYRot(nGoal);
            bcs::BCS_VecOneZ_RotY(dir, fRot);
            long nNum = nCoin;
            CGimmickMan::EffectCoinScoreCreate(pos, dir, nNum);
        }
        return 0;
    }

    // The goal demo of the basket of team nTeam, -1 for none.
    long CGameBasket::GetGoalDemoType_(long, long nShootType) const
    {
        if (!CEventDemo::IsEnable()) {
            return -1;
        }
        if (CheckGoalDemoRnd_(100) && g_GameWork.m_nPeriod == 0) {
            // The first basket of the game.
            int bFirst = 1;
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                long nScore = GetTeamPeriodScore(i, g_GameWork.m_nPeriod);
                if (i == g_GameWork.m_nGoalTeam) {
                    nScore -= g_GameWork.m_nGoalScore;
                }
                if (nScore > 0) {
                    bFirst = 0;
                    break;
                }
            }
            if (bFirst && g_GameWork.m_nGoalScore > 0) {
                return 0x10;
            }
        }
        if (CheckGoalDemoRnd_(100)) {
            // A basket that takes the lead.
            long nScore0 = GetTeamPeriodScore(0, g_GameWork.m_nPeriod);
            long nScore1 = GetTeamPeriodScore(1, g_GameWork.m_nPeriod);
            if (g_GameWork.m_nGoalTeam == 0) {
                nScore0 -= g_GameWork.m_nGoalScore;
            } else if (g_GameWork.m_nGoalTeam == 1) {
                nScore1 -= g_GameWork.m_nGoalScore;
            }
            if (nScore0 < nScore1) {
                if (nScore0 + g_GameWork.m_nGoalScore > nScore1) {
                    return 0x11;
                }
            } else if (nScore0 > nScore1 && nScore1 + g_GameWork.m_nGoalScore > nScore0) {
                return 0x11;
            }
        }
        if (nShootType >= 9 && nShootType <= 11 && CheckGoalDemoRnd_(100)) {
            return 0x12;
        }
        if (m_nGoalBallType == 1 && CheckGoalDemoRnd_(20)) {
            return 0x14;
        }
        if (m_nGoalBallType == 2 && CheckGoalDemoRnd_(20)) {
            return 0x13;
        }
        if (CheckGoalDemoRnd_(20) && g_GameWork.m_nGoalBonusType != 0) {
            return 0x17;
        }
        if (CheckGoalDemoRnd_(100) && g_GameWork.m_fUnk20 > 0.0f && g_GameWork.m_fGameTime <= 900.0f) {
            return 0x16;
        }
        long nDemo = -1;
        if (m_nUnk11C >= 2) {
            nDemo = 1;
        }
        return nDemo;
    }

    // The telop of the goal demo, by the shot and the coins; -1 for none.
    long CGameBasket::GetGoalDemoTelop_(long, long nShootType, long nCoin, int) const
    {
        long nTelop = -1;
        if (nShootType >= 9 && nShootType <= 11 && nCoin >= 0) {
            nTelop = 1;
        }
        if (nTelop <= 0 && m_nGoalBallType == 1 && nCoin >= 5) {
            nTelop = 3;
        }
        if (nTelop <= 0 && m_nGoalBallType == 2 && nCoin >= 1) {
            nTelop = 2;
        }
        switch (nShootType) {
            case 0:
            case 1:
            case 2:
                if (nTelop <= 0 && nCoin >= 5) {
                    nTelop = 3;
                }
                break;
            case 5:
                if (nTelop <= 0 && nCoin >= 5) {
                    nTelop = 3;
                }
                break;
            case 3:
            case 4:
                if (nTelop <= 0 && nCoin >= 5) {
                    nTelop = 3;
                }
                break;
            case 8:
                if (nTelop <= 0 && nCoin >= 1) {
                    nTelop = 2;
                }
                break;
        }
        if (nTelop > 0) {
            return nTelop - 1;
        }
        return -1;
    }

    // Ends the game early (a tournament mission decided it); nTeam is the
    // team whose character starts the next state.
    void CGameBasket::ForceGameExit_(long nTeam)
    {
        if (!(GetGameFlag_() & 0x20000)) {
            switch (m_nUnk12C) {
                case 3:
                    m_nGameFlag |= 0x20000 | 1;
                    break;
                case 4:
                    m_nGameFlag |= 0xA0000 | 1;
                    break;
            }
            m_nForceExitTeam = nTeam;
            if (m_Unk130 > 0) {
                m_nGameFlag |= 0x30000 | 5;
            }
        }
    }

    void CGameBasket::GameInit_()
    {
        m_nGameFlag = 0;
        GameWorkReset_();
        CChrMan::InitLeader(1);
        ResetSPGauge_();
        for (long i = 0; i < 4; i++) {
            if (GetPeriodStartCoin(i) >= 0) {
                SetTeamCoin(i, GetPeriodStartCoin(i));
            } else {
                ClearTeamCoin(i);
            }
        }
        for (long i = 0; i < 4; i++) {
            SetTeamPeriodScore(i, 0, GetPeriodStartScore(i));
            m_anTeamScore[i] = GetPeriodStartScore(i);
        }
        ClearJumpBallerInfo();
        CGame::SendMessage(0xF, 0, 0, 0);
        SendMessage_(0x12, 0, 0, 0);
        m_nUnk104 = 1;
        m_nUnk108 = 0;
        m_nUnk12C = 0;
        m_Unk130 = 0;
        StopReboundEffectCamera_(1);
        SetTime(g_GameWork.m_fUnk20);
        if (CTournament::GetMissionStartMode() && g_GameWork.GetUnk150() == 0) {
            m_nBasketFlag |= 4;
            SetTurnStartMode(static_cast<GAME_TURN_START_MODE>(1));
        }
        CChrManBasket::GetInstance()->AllClearPlayerIcon_(0);
    }

    void CGameBasket::GameSetUp_()
    {
        m_nGameFlag = 0;
        KillTempChild_();
        BgResetFade_();
        CChrMan::AllReset();
        CBallMan::AllReset();
        CGimmickMan::AllReset();
        CStageMan::GetInstance()->GoalRepairAll();
        CGimmickMan::SetUp();
        CGimmickMan::QPanelSetViewBaseRot(GetTargetGoalIdx(GetViewTargetTeam(0)) ? 3.1415927f : 0.0f);
        CEffectCtrl::EraseEffectAll();
        DispOffGoalMarker(0);
        CGame::SendMessage(0x13, 0, 0, 0);
        m_nExCameraType = 0;
        ClearLastGoalInfo();
        g_GameWork.m_fUnk38 = 0.0f;
        ClearCoinHistory();
        SetJumpBallMode(0);
        m_nArrowState = -1;
        m_nScoreArrowState = -1;
        m_nGoalTelop = -1;
        m_nRiverFallTeam = -1;
        SetGoalOwnGoal(0);
        ChrPosSetNormal_();
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && pChr->IsEnable()) {
                pChr->Util_SetDefenseLeaderFirstState();
            }
        }
        if (g_GameWork.GetUnk150() == 0) {
            // A jump ball starts the period.
            CChrMan::GetTeamLeader(0)->Util_SetDefenseLeaderFirstState();
            CChrMan::GetTeamLeader(1)->Util_SetDefenseLeaderFirstState();
            CBallMan::SetJumpBallMode(0);
            SetLooseBallModeFlag(1);
            SetJumpBallMode(1);
            GetInstance()->SetOffenseTeam_(-1);
            CChrMan::NpcSendMessage(1, 1, 0, 0, 0);
        } else {
            long nTeam = g_GameWork.GetUserTeam();
            CChrMan::GetTeamLeader(nTeam)->Util_SetOffenseLeaderFirstState(0);
            CChrMan::GetTeamLeader(1 - g_GameWork.GetUserTeam())->Util_SetDefenseLeaderFirstState();
        }
        m_nShootPoint = 0;
        GetInstance()->SetCheckSpEndure_(0);
        m_nForceExitTeam = -1;
        CameraReset_();
        CChrMan::AllSendReset();
    }

    void CGameBasket::CameraReset_()
    {
        for (long i = 0; i < 4; i++) {
            if (m_apCamera[i]) {
                m_apCamera[i]->Reset();
            }
        }
    }

    // Puts the characters in their places for the start of a period: the
    // jump ballers at the centre circle, or the party game's line-ups.
    void CGameBasket::ChrPosSetNormal_()
    {
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            m_anJumpBaller[i] = -1;
        }

        if (g_GameWork.GetGameType() == GAME_TYPE_PARTY) {
            if (CTournament::IsTournamentGame()) {
                if (g_GameWork.GetUnk06() == 4 || g_GameWork.GetUnk06() == 6) {
                    // All in one row.
                    long n = 0;
                    for (long nTeam = 0; nTeam < 2; nTeam++) {
                        for (long i = nTeam; i < CChrMan::GetChrCoreNum(); i += 2) {
                            CChrCore *pChr = CChrMan::GetChrCore(i);
                            if (pChr && !pChr->IsDisable() && !pChr->IsDisable()) {
                                pChr->SetPos(bcs::FVEC3(2.5f * n + -3.75f, 0.0f, 0.0f));
                                pChr->SetRotY(3.1415927f);
                                n++;
                            }
                        }
                    }
                    return;
                }
                // Each team on its side.
                bcs::FVEC3 aPos[3] = {
                    bcs::FVEC3(-4.0f, 0.0f, 2.0f),
                    bcs::FVEC3(-1.5f, 0.0f, 2.0f),
                    bcs::FVEC3(-2.75f, 0.0f, -2.0f),
                };
                for (long nTeam = 0; nTeam < 2; nTeam++) {
                    const bcs::FVEC3 *pPos = aPos;
                    for (long i = nTeam; i < CChrMan::GetChrCoreNum(); i += 2) {
                        CChrCore *pChr = CChrMan::GetChrCore(i);
                        if (pChr && !pChr->IsDisable()) {
                            bcs::FVEC3 pos = *pPos;
                            if (nTeam == 1) {
                                pos.x = -pos.x;
                            }
                            pChr->SetPos(pos);
                            pChr->SetRotY(3.1415927f);
                            pPos++;
                        }
                    }
                }
                return;
            }
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable()) {
                    pChr->SetPos(bcs::FVEC3(2.5f * i + -3.75f, 0.0f, 0.0f));
                    pChr->SetRotY(3.1415927f);
                }
            }
            return;
        }

        // Which line-up each team takes: 0 the jump ball, 1 attacking, 2
        // defending.
        long anLineUp[2] = {0, 0};
        long nFirstTeam = -1;
        switch (CTournament::GetMissionStartMode()) {
            case 1:
                nFirstTeam = 0;
                break;
            case 2:
                nFirstTeam = 1;
                break;
        }
        if (nFirstTeam != -1) {
            GetInstance()->SetOffenseTeam_(nFirstTeam);
        }
        int bJumpBall = nFirstTeam == -1;
        if (g_GameWork.GetUnk150() == 1) {
            anLineUp[g_GameWork.GetUserTeam()] = 1;
            anLineUp[1 - g_GameWork.GetUserTeam()] = 2;
            if (bJumpBall) {
                PositionChange_(g_GameWork.GetUserTeam());
            }
        } else if (g_GameWork.GetPeriod() >= 1 && bJumpBall) {
            PositionChange_(-1);
        }

        // The characters by place: team 0 at the even places, team 1 at the
        // odd ones, the first of each the jump baller. A star player goes
        // first.
        long anChr[6];
        for (long i = 0; i < 6; i++) {
            anChr[i] = i;
        }
        if (CChrMan::GetChrCore(4)->GetStatusFlag() & 0x100) {
            anChr[0] = 4;
            anChr[2] = 2;
            anChr[4] = 0;
        }
        if (CChrMan::GetChrCore(2)->GetStatusFlag() & 0x100) {
            anChr[0] = 2;
            anChr[2] = 0;
            anChr[4] = 4;
        }
        if (CChrMan::GetChrCore(0)->GetStatusFlag() & 0x100) {
            anChr[0] = 0;
            anChr[2] = 2;
            anChr[4] = 4;
        }
        if (CChrMan::GetChrCore(5)->GetStatusFlag() & 0x100) {
            anChr[1] = 5;
            anChr[3] = 3;
            anChr[5] = 1;
        }
        if (CChrMan::GetChrCore(3)->GetStatusFlag() & 0x100) {
            anChr[1] = 3;
            anChr[3] = 1;
            anChr[5] = 5;
        }
        if (CChrMan::GetChrCore(1)->GetStatusFlag() & 0x100) {
            anChr[1] = 1;
            anChr[3] = 3;
            anChr[5] = 5;
        }

        // The jump baller of a team chosen before (by the computer, or in
        // turn among the players' pads) and the others after him; the
        // jump baller stands fJumpBallDist from the centre.
        const long aanOrder[6][3] = {
            {0, 2, 4}, {1, 3, 5}, {2, 0, 4}, {3, 1, 5}, {4, 2, 0}, {5, 3, 1},
        };
        // (static-order swap: the original names the two the other way round)
        static const float fJumpBallDist = 1.6f;
        static const long MAX = 3;
        for (long nTeam = 0; nTeam < 2; nTeam++) {
            if (bJumpBall) {
                switch (m_anJumpBallType[nTeam]) {
                    case 2: {
                        CChrCore *pChr = CChrMan::GetChrCore(m_aanJumpBallPad[nTeam][0]);
                        if (pChr) {
                            const long *pOrder = aanOrder[pChr->GetPlayerNo()];
                            anChr[nTeam] = pOrder[0];
                            anChr[nTeam + 2] = pOrder[1];
                            anChr[nTeam + 4] = pOrder[2];
                        }
                        break;
                    }
                    case 1: {
                        // The places of the players' pads; the computer
                        // character takes the place left.
                        long nRest = nTeam == 0 ? 0 + 2 + 4 : 1 + 3 + 5;
                        for (long k = 0; k < MAX; k++) {
                            if (m_aanJumpBallPad[nTeam][k] == -1) {
                                anChr[nTeam + k * 2] = -1;
                            } else {
                                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                                    CChrCore *pChr = CChrMan::GetChrCore(i);
                                    if (pChr && !pChr->IsDisable() && nTeam == pChr->GetTeamNo() &&
                                        pChr->DriverGetType() == 1 &&
                                        m_aanJumpBallPad[nTeam][k] == pChr->GetPadNo()) {
                                        anChr[nTeam + k * 2] = pChr->GetPlayerNo();
                                        nRest -= pChr->GetPlayerNo();
                                    }
                                }
                            }
                        }
                        if (anChr[nTeam] == -1) {
                            anChr[nTeam] = nRest;
                        } else if (anChr[nTeam + 2] == -1) {
                            anChr[nTeam + 2] = nRest;
                        } else if (anChr[nTeam + 4] == -1) {
                            anChr[nTeam + 4] = nRest;
                        }
                        break;
                    }
                }
            }
        }

        // The places (x, the yaw, z) of each line-up, the jump baller first;
        // mirrored for the team at the other goal.
        float fJumpRot = 2.7925268f;
        const bcs::FVEC3 aaPos[3][MAX] = {
            {
                bcs::FVEC3(-fJumpBallDist * nw4r::math::SinRad(fJumpRot), fJumpRot,
                           -fJumpBallDist * nw4r::math::CosRad(fJumpRot)),
                bcs::FVEC3(2.5f, 3.1415927f, 2.0f),
                bcs::FVEC3(-2.5f, 3.1415927f, 2.0f),
            },
            {
                bcs::FVEC3(0.0f, 3.1415927f, 0.0f),
                bcs::FVEC3(4.0f, 3.1415927f, 4.0f),
                bcs::FVEC3(-4.0f, 3.1415927f, 4.0f),
            },
            {
                bcs::FVEC3(0.0f, 3.1415927f, 7.0f),
                bcs::FVEC3(4.0f, 3.1415927f, 4.0f),
                bcs::FVEC3(-4.0f, 3.1415927f, 4.0f),
            },
        };
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(anChr[i]);
            if (pChr && !pChr->IsDisable()) {
                long nTeam = pChr->GetTeamNo();
                const bcs::FVEC3 &rPos = aaPos[anLineUp[nTeam]][i >> 1];
                float fX = rPos.x;
                float fRot = rPos.y;
                float fZ = rPos.z;
                if (GetGoalIdx(nTeam)) {
                    fX *= -1.0f;
                    fZ *= -1.0f;
                    fRot = TurnRotY(fRot);
                }
                pChr->SetRotY(fRot);
                pChr->SetPos(bcs::FVEC3(fX, 0.0f, fZ));
                pChr->RecalcAllMatrix();
            }
        }
        CChrMan::GetInstance()->SetTeamLeader(0, anChr[0], 0);
        CChrMan::GetInstance()->SetTeamLeader(1, anChr[1], 0);
        m_anJumpBaller[0] = anChr[0];
        m_anJumpBaller[1] = anChr[1];
    }

    // Puts the characters in their places for the result, a star player
    // in front.
    void CGameBasket::ChrPosSetResult_()
    {
        long anChr[6];
        for (long i = 0; i < 6; i++) {
            anChr[i] = i;
        }
        if (CChrMan::GetChrCore(2)->GetStatusFlag() & 0x100) {
            anChr[0] = 2;
            anChr[2] = 0;
        }
        if (CChrMan::GetChrCore(4)->GetStatusFlag() & 0x100) {
            anChr[0] = 4;
            anChr[4] = 0;
        }
        if (CChrMan::GetChrCore(3)->GetStatusFlag() & 0x100) {
            anChr[1] = 3;
            anChr[3] = 1;
        }
        if (CChrMan::GetChrCore(5)->GetStatusFlag() & 0x100) {
            anChr[1] = 5;
            anChr[5] = 1;
        }
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(anChr[i]);
            if (!pChr->IsDisable()) {
                const bcs::FVEC3 &rPos = s_aResultPos[i >> 1];
                float fX = rPos.x;
                float fRot = rPos.y;
                float fZ = rPos.z;
                if (GetGoalIdx(pChr->GetTeamNo())) {
                    fX *= -1.0f;
                    fZ *= -1.0f;
                    fRot = TurnRotY(fRot);
                }
                pChr->SetRotY(fRot);
                pChr->SetPos(bcs::FVEC3(fX, 0.0f, fZ));
                pChr->RecalcAllMatrix();
            }
        }
    }

    unsigned long CGameBasket::IsNowShotClock_() const
    {
        return m_nGameFlag & 0x400;
    }

    // The shot clock has run out: the play stops unless a shot is in the
    // air. Not zero when it stopped the play.
    int CGameBasket::SetClockOver()
    {
        if (GetState() == m_nNextState) {
            if (m_nGameFlag & 0x400) {
                return 0;
            }
            if (!(m_nGameFlag & 8)) {
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall && pBall->IsShootMode()) {
                    m_nGameFlag |= 0x1000;
                    return 0;
                }
                m_nGameFlag |= 1;
                if (!(m_nGameFlag & 2)) {
                    CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                    if (g_GameWork.m_nGameMode == GAME_MODE_BASKET) {
                        signed char nChr = -1;
                        CChrCore *pChr = CChrMan::GetTeamLeader(g_GameWork.GetUserTeam() == 0);
                        if (pChr) {
                            nChr = pChr->GetPlayerNo();
                        }
                        CChrMan::AllSetState(0, nChr, 0);
                    }
                }
                m_nStateStep = 0;
                m_nGameFlag |= 0x400;
                return 1;
            }
        }
        return 0;
    }

    void CGameBasket::CreateTelop_(long nTelop, int bClose, long nParam)
    {
        if (nTelop >= 0) {
            CCockpitBasket::CreateObj(nTelop, nParam);
            CCockpit::SetAnimEndClose(nTelop, bClose);
        }
    }

    // Moves the players' pads of a team (or of both, -1) one place on, so
    // that the jump ball goes round the players.
    void CGameBasket::PositionChange_(long nTeam)
    {
        long nNum = 0;
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable() && pChr->GetTeamNo() == 0) {
                nNum++;
            }
        }
        for (long i = 0; i < 2; i++) {
            if (nTeam == -1 || i == nTeam) {
                for (;;) {
                    if (nNum == 2) {
                        long n = m_aanJumpBallPad[i][0];
                        m_aanJumpBallPad[i][0] = m_aanJumpBallPad[i][1];
                        m_aanJumpBallPad[i][1] = n;
                    } else if (nNum == 3) {
                        long n = m_aanJumpBallPad[i][2];
                        m_aanJumpBallPad[i][2] = m_aanJumpBallPad[i][0];
                        m_aanJumpBallPad[i][0] = m_aanJumpBallPad[i][1];
                        m_aanJumpBallPad[i][1] = n;
                    } else {
                        break;
                    }
                    if (m_anJumpBallType[i] != 1) {
                        break;
                    }
                    long nPad = GetNowJumpBallerPadNo_(i);
                    if (nTeam == -1 && nPad != -1) {
                        if (nPad == m_aanJumpBallPad[i][0]) {
                            break;
                        }
                    } else if (m_aanJumpBallPad[i][0] != -1) {
                        break;
                    }
                }
            }
        }
    }

    int CGameBasket::IsNowMainLoopEnable_() const
    {
        if (GetState() != STATE_MAIN_LOOP) {
            return 0;
        }
        if (m_nGameFlag & 1) {
            return 0;
        }
        return !(m_nGameFlag & 0x20000);
    }

    long CGameBasket::GetTeamGhostType(long nTeam)
    {
        switch (GetTeamColorIdx(nTeam)) {
            case 0:
                return 4;
            case 1:
                return 3;
            default:
                return 5;
        }
    }

    // The hit stop: at once, or after fDelay frames (not in the party game).
    void CGameBasket::StopEffect_(float fDelay, float fFrame, float fPower)
    {
        if (g_GameWork.GetGameType() != GAME_TYPE_PARTY && !(fFrame <= 0.0f)) {
            if (fDelay > 0.0f) {
                new CStopEffectBasket(this, fDelay, fFrame, fPower);
            } else {
                StartStopEffect(fFrame, fPower, 0);
            }
        }
    }

    void CGameBasket::PlaySndGoalShake_(long nGoal)
    {
        if (g_GameWork.GetGameType() != GAME_TYPE_PARTY) {
            PlaySnd_(SND_GOAL, &GetGoalPos(nGoal));
        }
    }

    // The slow motion of an exceptional camera: 1 a close shot, 2 the
    // change of offense, 5 a trick move.
    void CGameBasket::PlayExceptionalCameraSlow_(long nType)
    {
        if (g_GameWork.GetGameType() == GAME_TYPE_NORMAL && (nType != m_nExCameraType || m_nExCameraType != 5)) {
            m_nExCameraType = nType;
            float fRate;
            float fFrame;
            float fOut;
            switch (nType) {
                case 1:
                    m_nExCameraType = 0;
                    fFrame = 0.0f;
                    fRate = 1.0f;
                    fOut = fFrame;
                    break;
                case 2:
                    m_nExCameraType = 0;
                    fRate = 0.5f;
                    fFrame = 15.0f;
                    fOut = 5.0f;
                    break;
                case 5: {
                    CChrCore *pTrick = 0;
                    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                        CChrCore *pChr = CChrMan::GetChrCore(i);
                        if (pChr && !pChr->IsDisable() && pChr->GetState() == 0x1E) {
                            pTrick = pChr;
                        }
                    }
                    fFrame = static_cast<CChrCoreBasket *>(pTrick)->GetStateWorkTrickMoveRemainderFrame();
                    if (fFrame <= 0.0f) {
                        return;
                    }
                    fRate = 0.3f;
                    fOut = 10.0f;
                    break;
                }
                default:
                    return;
            }
            StartSlowEffect(fRate, fFrame, 0.0f, fOut);
        }
    }

    // Ends the exceptional camera once what it shows is over.
    void CGameBasket::CheckExceptionalCamera_()
    {
        switch (GetExceptionalCameraType_()) {
            case 1: {
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall) {
                    if (pBall->IsShootMode()) {
                        return;
                    }
                    if (g_GameWork.m_nFlag18 & 0x20) {
                        return;
                    }
                }
                break;
            }
            case 2:
                if (IsNowSlowEffect()) {
                    return;
                }
                break;
            case 3:
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && (pChr->GetStatusFlag() & 0x2000)) {
                        return;
                    }
                }
                break;
            case 5:
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && pChr->GetState() == 0x1E) {
                        return;
                    }
                }
                break;
            default:
                return;
        }
        m_nExCameraType = 0;
    }

    long CGameBasket::GetExceptionalCameraType_()
    {
        return m_nExCameraType;
    }

    void CGameBasket::ClearJumpBallerInfo()
    {
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            m_aanJumpBallerPad[i][0] = -1;
            m_aanJumpBallerPad[i][1] = -1;
            m_aanJumpBallerPad[i][2] = -1;
        }
    }

    // Who takes the jump balls: the players' pads of a team in order (the
    // last two in random order), or the computer's characters.
    void CGameBasket::SetUpJumpBallerInfo()
    {
        ClearJumpBallerInfo();
        for (long nTeam = 0; nTeam < g_GameWork.m_nTeamNum; nTeam++) {
            m_anJumpBallType[nTeam] = 0;
            long nNum = 0;
            m_aanJumpBallPad[nTeam][0] = -1;
            m_aanJumpBallPad[nTeam][1] = -1;
            m_aanJumpBallPad[nTeam][2] = -1;
            long *pPad = m_aanJumpBallerPad[nTeam];
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && nTeam == pChr->GetTeamNo()) {
                    if (pChr->IsPadCtrl()) {
                        nNum++;
                        *pPad++ = pChr->GetPadNo();
                    }
                }
            }
            long *pSort = m_aanJumpBallerPad[nTeam];
            for (long i = 0; i < nNum; i++) {
                for (long j = i + 1; j < nNum; j++) {
                    if (pSort[i] >= pSort[j]) {
                        long n = pSort[i];
                        pSort[i] = pSort[j];
                        pSort[j] = n;
                    }
                }
            }
            if (nNum == 3 && bcs::BCS_RandMax(1L)) {
                long n = pSort[1];
                pSort[1] = pSort[2];
                pSort[2] = n;
            }
            m_anJumpBallerNum[nTeam] = nNum;
        }

        for (long nTeam = 0; nTeam < g_GameWork.m_nTeamNum; nTeam++) {
            long nPadNum = 0;
            CChrCore *apChr[3] = {0, 0, 0};
            CChrCore **ppChr = apChr;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && nTeam == pChr->GetTeamNo()) {
                    if (pChr->DriverGetType() == 1) {
                        nPadNum++;
                    }
                    *ppChr++ = pChr;
                }
            }
            switch (nPadNum) {
                case 1:
                    break;
                case 0:
                    if (apChr[0]) {
                        m_aanJumpBallPad[nTeam][0] = apChr[0]->GetPlayerNo();
                    }
                    if (apChr[1]) {
                        m_aanJumpBallPad[nTeam][1] = apChr[1]->GetPlayerNo();
                    }
                    if (apChr[2]) {
                        m_aanJumpBallPad[nTeam][2] = apChr[2]->GetPlayerNo();
                    }
                    m_anJumpBallType[nTeam] = 2;
                    break;
                default:
                    for (long k = 0; k < 3; k++) {
                        if (apChr[k] && apChr[k]->DriverGetType() == 1) {
                            m_aanJumpBallPad[nTeam][k] = apChr[k]->GetPadNo();
                        }
                    }
                    m_anJumpBallType[nTeam] = 1;
                    break;
            }
        }
    }

    long CGameBasket::GetNowJumpBallerChrIdx_(long nTeam) const
    {
        return m_anJumpBaller[nTeam];
    }

    // The pad whose turn it is to take the jump ball of the period; -1 for
    // none.
    long CGameBasket::GetNowJumpBallerPadNo_(long nTeam) const
    {
        long nNum = m_anJumpBallerNum[nTeam];
        if (nNum <= 0) {
            return -1;
        }
        return m_aanJumpBallerPad[nTeam][g_GameWork.m_nPeriod % nNum];
    }

    // The marker over the goal the player's team attacks.
    void CGameBasket::DispOnGoalMarker()
    {
        if (!(g_GameWork.m_nFlag18 & 0x4000)) {
            long nState = GetState();
            if (nState == STATE_WAIT_START || nState == STATE_MAIN_LOOP) {
                long nTeam = g_GameWork.GetUserTeam();
                if (!CChrManBasket::GetInstance()->IsAllComTeam_(nTeam)) {
                    long nGoal = GetTargetGoalIdx(nTeam);
                    long nIcon = MARKER_ICON_2;
                    if (GetTeamColorIdx(g_GameWork.GetUserTeam()) == 0) {
                        nIcon = MARKER_ICON_1;
                    }
                    if (m_nGoalMarker != -1 && m_nGoalMarker != nIcon) {
                        DispOffGoalMarker(0);
                    }
                    m_nGoalMarker = nIcon;
                    bcs::FVEC3 pos = GetGoalPos(nGoal);
                    CCockpit::SetMarkerIcon(m_nGoalMarker, &pos);
                    m_nBasketFlag |= 2;
                }
            }
        }
    }

    void CGameBasket::DispUpDateGoalMaker()
    {
        if (m_nBasketFlag & 2) {
            if (g_GameWork.m_nFlag18 & 0x4000) {
                DispOffGoalMarker(1);
                return;
            }
            long nGoal = GetTargetGoalIdx(g_GameWork.GetUserTeam());
            if (m_nGoalMarker != -1) {
                bcs::FVEC3 pos = GetGoalPos(nGoal);
                CCockpit::SetMarkerIconPosition(m_nGoalMarker, &pos);
            }
        }
    }

    void CGameBasket::DispOffGoalMarker(int bAnim)
    {
        unsigned long nFlag = m_nBasketFlag;
        m_nGoalMarker = -1;
        m_nBasketFlag = nFlag & ~2;
        CCockpit::ClearMarkerIcon(MARKER_ICON_0, bAnim);
        CCockpit::ClearMarkerIcon(MARKER_ICON_1, bAnim);
        CCockpit::ClearMarkerIcon(MARKER_ICON_2, bAnim);
    }

    unsigned long CGameBasket::IsPlayReboundEffectCamera_() const
    {
        return (m_nBasketFlag >> 3) & 1;
    }

    // The camera of a rebound at goal nGoal, unless someone can dunk or is
    // inside the three-point line. Not zero when it started.
    int CGameBasket::PlayReboundEffectCamera_(long nGoal)
    {
        if (g_GameWork.GetGameType() == GAME_TYPE_PARTY) {
            return 0;
        }
        if (!IsNowMainLoopEnable_()) {
            return 0;
        }
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                if (static_cast<const CChrCoreBasket *>(pChr)->CanbeDunkJump()) {
                    return 0;
                }
                const bcs::FVEC3 &rPos = pChr->GetPos();
                if (CChrManBasket::GetInstance()->In3PoinArea_(nGoal, rPos, 0.0f)) {
                    return 0;
                }
            }
        }
        CEventDemo::SetupDemoCamera("Rebound", nGoal);
        bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
        CEventDemo::SetDemoCameraInfo(GetGoalPos(nGoal), target, nGoal);
        CEventDemo::PlayDemoCamera(0);
        m_nBasketFlag |= 8;
        m_fReboundTime = 0.0f;
        m_fReboundNoShoot = 0.0f;
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        DispOffGoalMarker(1);
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCoreBasket *pChr = static_cast<CChrCoreBasket *>(CChrMan::GetChrCore(i));
            if (pChr) {
                StartCombinationPlay(pChr);
            }
        }
        m_nBasketFlag &= ~0x10;
        SetDispRadarIcon_(0);
        CGame::SetDispChrHeadIcon_(0);
        return 1;
    }

    void CGameBasket::StopReboundEffectCamera_(int bStop)
    {
        if (m_nBasketFlag & 8) {
            CEventDemo::SetEndDemoCamera();
            if (bStop) {
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(0), static_cast<CChrCore::STOP_MODE>(0));
            }
        }
        m_nBasketFlag &= ~8;
        m_fReboundTime = 0.0f;
        m_fReboundNoShoot = 0.0f;
    }

    int CGameBasket::IsRingMove_(long nGoal) const
    {
        CGoalObj *pGoal = CStageMan::GetInstance()->GetGoal(nGoal);
        if (pGoal) {
            return pGoal->IsNowDunkMot();
        }
        return 0;
    }

    // Stage 06: the lights over goal 0.
    int CGameBasket::IsChandelierOn_() const
    {
        return static_cast<CStageBasket06 *>(CStageMan::GetInstance())->Illumi_IsOn(0);
    }

    // Camera quake nQuake for character nChr; most of them only for a
    // player's character.
    void CGameBasket::CameraQuake_(long nQuake, long nChr)
    {
        static unsigned short nTb[22] = {
            3, 4, 5, 6, 3, 0x33, 0x33, 0x33, 0x33, 3, 0xF, 0x2F, 0x2F, 3, 0x11, 0x26, 3, 0xF, 0x2F, 0x2F, 5, 0x36,
        };

        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        switch (nQuake) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 11:
            case 12:
            case 15:
            case 16:
            case 18:
            case 19:
            case 20:
                if (!CChrManBasket::GetInstance()->IsPadTypePlayer_(pChr)) {
                    break;
                }
                // fall through
            default:
                StartCameraQuake(nTb[nQuake], nChr, -1);
                break;
        }
    }

    // The quake of a basket, by the shot and the character.
    void CGameBasket::PlayTrGoalInQuake(const CChrCore *pChr, const CBall *pBall)
    {
        long nShootType = pBall->GetShootType();
        long nChr = pChr->GetPlayerNo();
        switch (nShootType) {
            case 9:
            case 10:
            case 11:
                // The special shots, by the character.
                switch (pChr->GetChrTypeID()) {
                    case 0:
                        CameraQuake_(10, nChr);
                        break;
                    case 7:
                        CameraQuake_(13, nChr);
                        break;
                    case 10:
                        CameraQuake_(14, nChr);
                        break;
                    case 13:
                        CameraQuake_(16, nChr);
                        break;
                    case 0x13:
                    case 0x14:
                    case 0x15:
                    case 0x16:
                    case 0x17:
                    case 0x18:
                        CameraQuake_(17, nChr);
                        break;
                }
                break;
            case 3:
                CameraQuake_(0, nChr);
                break;
            case 8:
                CameraQuake_(4, nChr);
                break;
            default: {
                CItemBall *pItem = pBall->GetItemEffect_();
                if (pItem && pItem->GetType() == 4) {
                    CameraQuake_(9, nChr);
                }
                break;
            }
        }
    }

    unsigned long CGameBasket::IsRiverFallRestart_() const
    {
        return m_nGameFlag & 0x800;
    }

    void CGameBasket::SetRiverFallRestart_(long nTeam)
    {
        unsigned long nFlag = GetGameFlag_();
        if (!(nFlag & 1) && !(nFlag & 0x800)) {
            m_nRiverFallTeam = nTeam;
            m_nGameFlag |= 0x800;
        }
    }

    long CGameBasket::GetPartyLv()
    {
        return GetPartyGameParam(0);
    }

    // The "shake the remote" prompt of the special move's endure.
    void CGameBasket::DispShakeRemoconTelop_(int bDisp)
    {
        long nTelop = GetTelopID_(TELOP_SHAKE_REMOCON);
        if (bDisp) {
            if (TelopIsEnd_(nTelop)) {
                CCockpit::CreateObj(nTelop, 0);
                GetInstance()->PlayExceptionalCameraSlow_(3);
            }
        } else {
            if (GetExceptionalCameraType_() == 3) {
                m_nExCameraType = 0;
            }
            TelopEnd_(nTelop);
        }
    }

    void CGameBasket::SetCheckSpEndure_(int bCheck)
    {
        if (bCheck) {
            m_nBasketFlag |= 1;
            return;
        }
        DispShakeRemoconTelop_(0);
        m_nBasketFlag &= ~1;
    }

    // Shows the prompt while a player's character endures a special move.
    void CGameBasket::CheckSpEndure()
    {
        if (m_nBasketFlag & 1) {
            int bEndure = 0;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable() && pChr->IsPadCtrl() && (pChr->GetStatusFlag() & 0x2000) &&
                    (!g_GameWork.m_Unk151 || g_GameWork.m_nUnk00 == pChr->GetTeamNo())) {
                    bEndure = 1;
                    break;
                }
            }
            DispShakeRemoconTelop_(bEndure);
        }
    }

    void CGameBasket::EndTelop_(long nTelop)
    {
        CCockpit::PlayEndAnim(nTelop);
    }

    void CGameBasket::RemoveTelop_(long nTelop)
    {
        CCockpit::RemoveObj(nTelop);
    }

    int CGameBasket::IsTelopEndStartAnim_(long nTelop)
    {
        return CCockpit::IsEndOfStartAnim(nTelop);
    }

    int CGameBasket::IsTelopEnd_(long nTelop) const
    {
        return CCockpit::IsEndObj(nTelop);
    }

    // The cockpit object of telop nIdx.
    long CGameBasket::GetTelopID_(long nIdx) const
    {
        const long anTelop[21] = {
            0,    0x1002, 3,    4,    5,    6,      0xA,  0xB,    0xC,    0xD,    0x1060,
            0xE,  7,      0xF,  0x14, 0x13, 0x15,   0x1072, 0x1073, 0x1074, 0x1051,
        };
        return anTelop[nIdx];
    }

    // The shot clock display: nColor -2 and -1 hide it (-2 keeps the last
    // seconds), 0 and 1 are its colours.
    void CGameBasket::SetShotClockCount_(long nColor, long nCount)
    {
        if (nColor == -2 || nColor == -1) {
            long nPrev = m_nShotClockCount;
            unsigned long bOn = (GetGameFlag_() >> 14) & 1;
            m_nGameFlag &= ~0x4000;
            m_nShotClockCount = -1;
            if (nColor == -2 && !bOn) {
                return;
            }
            if (nColor == -2 && nPrev <= 5) {
                return;
            }
        } else {
            if (m_nShotClockCount == nCount) {
                return;
            }
            if (nCount < 6) {
                PlaySnd_(SND_SHOT_CLOCK, 0);
            }
            m_nShotClockCount = nCount;
            m_nGameFlag |= 0x4000;
        }
        long nPrevColor = m_nShotClockColor;
        m_nShotClockColor = nColor;
        if ((unsigned long)nPrevColor <= 1 && (unsigned long)nColor <= 1 && nPrevColor != nColor) {
            CCockpitBasket::SetShotClockCount(-2, m_nShotClockCount);
        }
        CCockpitBasket::SetShotClockCount(nColor, nCount);
    }

    void CGameBasket::PlaySnd_(long nSnd, const bcs::FVEC3 *pPos)
    {
        CGameSnd::Play(GetSndID(nSnd), pPos, 1, -1, 0.0f, 1.0f, 0.0f);
    }

    // The coins of a basket: the team's coins not yet dropped.
    void CGameBasket::SetGoalCoinEffect(long nCoin, long nChr, long nTeam, long nShootType, const bcs::FVEC3 &rPos)
    {
        long nGoalCoin = nCoin - GetTeamDisableDropCoin(nTeam);
        if (nGoalCoin < 0) {
            nGoalCoin = 0;
        }
        SetGoalCoin(nGoalCoin);
        DoGoalEffect_(nChr, nTeam, nShootType, nGoalCoin, rPos);
        m_nGoalCoin += nGoalCoin;
    }

    void CGameBasket::ResetSPGauge_()
    {
        for (long i = 0; i < 4; i++) {
            m_anArtsCount[i] = 0;
        }
        for (long i = 0; i < 2; i++) {
            ClearTeamArtsGauge(i);
        }
    }

    int CGameBasket::IsTimeColorChange()
    {
        return g_GameWork.m_fGameTime < 960.0f;
    }

    // After a fall into the river: not zero once nobody holds the ball in
    // a shot and no shot is in the air.
    int CGameBasket::CheckChangeStateToRiverFall()
    {
        CChrCore *pChr = CChrMan::Util_SearchHoldBallChr(0);
        if (pChr) {
            long nState = pChr->GetState();
            if ((nState >= 0x24 && nState <= 0x28) || nState == 0x22) {
                return 0;
            }
        } else {
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall && pBall->IsShootMode()) {
                return 0;
            }
        }
        return 1;
    }

    // Scores a basket: the points of the shot, the coins and the stage's
    // panel bonus. Returns the points added.
    long CGameBasket::AddTeamScore_(long nChr, long nTeam, long nShootType, const bcs::FVEC3 &rPos, int bCoin)
    {
        if ((unsigned long)nTeam > 3) {
            return 0;
        }
        if (!(g_GameWork.m_nFlag18 & 0x100)) {
            long nCoin = !bCoin ? 0 : GetTeamCoin(nTeam);
            if (bCoin) {
                SetGoalCoinEffect(nCoin, nChr, nTeam, nShootType, rPos);
            }
            SetGoalCoin(m_nGoalCoin);
            long nGoalCoin = m_nGoalCoin;
            long nBase = m_nShootPoint + GetShootPointType_(nChr, nShootType);
            m_nShootPoint = 0;
            long nScore = nBase + nGoalCoin;
            SetGoalBaseScore(nBase);
            SetGoalBonusType(0);
            SetGoalScore(nScore);
            CStageManBasket *pStage = static_cast<CStageManBasket *>(CStageMan::GetInstance());
            long nPanel = pStage->GetCurrentPanelType();
            long nTotal = pStage->GetPanelScore(nScore, nPanel);
            SetGoalBonusColor(0);
            unsigned short nType = nPanel;
            if (IsPanelBonus(nType)) {
                SetGoalBonusColor(3);
            }
            if (nType == 5) {
                SetGoalBonusColor(2);
            }
            if (nType == 6) {
                SetGoalBonusColor(3);
            }
            if (nType == 7) {
                SetGoalBonusColor(3);
            }
            if (nType == 8) {
                SetGoalBonusColor(3);
            }
            if (nType == 9) {
                SetGoalBonusColor(4);
            }
            switch (pStage->GetPanelBonusType()) {
                case 1: {
                    long nRate = pStage->GetPanelBonusRate();
                    if (nRate < 0) {
                        SetGoalBonusType(3);
                        SetGoalBonusRate(-nRate);
                        SetGoalBonusColor(1);
                        CChrCore *pChr = CChrMan::GetChrCore(nChr);
                        if (pChr) {
                            pChr->PlayVoice(0x27, 0);
                        }
                    } else {
                        SetGoalBonusType(1);
                        SetGoalBonusRate(nRate);
                    }
                    break;
                }
                case 2:
                    SetGoalBonusType(2);
                    SetGoalBonusRate(pStage->GetPanelBonusRate());
                    break;
            }
            if (nTotal < 0 || (g_GameWork.m_nStage == 10 && g_GameWork.m_nGoalBonusType == 3)) {
                SetGoalOwnGoal(1);
            }
            AddTeamPeriodScore(nTeam, g_GameWork.m_nPeriod, nTotal);
            return nTotal;
        }
        return 0;
    }

    // The shot clock: shown while it is shorter than the game time left;
    // at zero the shot clock violation.
    void CGameBasket::DispShotClock_()
    {
        if (IsShotClockDisp() && IsShotClockRun()) {
            long nCount = CChrManBasket::GetInstance()->GetClockTimeSeconds_();
            SetShotClockCount_(GetTeamColorIdx(g_GameWork.GetUserTeam()) != 0, nCount);
            if (nCount <= 0) {
                if (!(m_nGameFlag & 0x2000)) {
                    PlaySnd_(SND_BUZZER, 0);
                    m_nGameFlag |= 0x2000;
                }
                SetClockOver();
            }
        } else if (m_nGameFlag & 0x4000) {
            SetShotClockCount_(-2, 0);
        }
    }

    void CGameBasket::SendMessage_(long nMsg, long nArg1, long nArg2, long nArg3)
    {
        switch (nMsg) {
            case 3:
                // A new period.
                SetTime(g_GameWork.m_fUnk20);
                SetNowPeriod(0);
                for (long i = 0; i < 4; i++) {
                    SetTeamPeriodScore(i, 0, GetPeriodStartScore(i));
                    m_anTeamScore[i] = GetPeriodStartScore(i);
                }
                break;
            case 0x1C:
                if (GetExceptionalCameraType_() == 4) {
                    m_nExCameraType = 0;
                }
                break;
            case 0xE:
                // The real ball of character 6's special shot: score the
                // basket the dummy ball kept back.
                if (m_nGameFlag & 0x8000) {
                    m_nShootPoint -= GetShootPointType_(g_GameWork.m_nGoalChar, g_GameWork.m_nGoalShootType);
                    long nTeam = g_GameWork.m_nGoalTeam;
                    long nShootType = g_GameWork.m_nGoalShootType;
                    long nChr = g_GameWork.m_nGoalChar;
                    TrgBasketGoalIn__(nChr, nTeam, nShootType, 0, 0.0f, GetGoalPos(GetGoalIdx(nTeam)), 0, 0, 0);
                }
                break;
            case 0x29:
                CGame::SendMessage(0x2A, 0, 0, 0);
                CGameGuideManBasket::EndJumpBall();
                break;
            case 0xB:
                CGame::SetDispChrHeadIcon_(0);
                DispOffGoalMarker(0);
                break;
            case 0xC:
                CGame::SetDispChrHeadIcon_(1);
                DispOnGoalMarker();
                break;
            case 0x20:
                if (CTournament::IsTournamentGame()) {
                    m_nUnk12C = 1;
                    m_Unk130 = nArg1;
                    ForceGameExit_(0);
                }
                break;
            case 0x21:
                if (CTournament::IsTournamentGame()) {
                    m_nUnk12C = 2;
                    m_Unk130 = nArg1;
                    ForceGameExit_(1);
                }
                break;
            case 0x23:
                if (CTournament::IsTournamentGame()) {
                    m_nUnk12C = nArg1;
                    m_Unk130 = 0;
                    if (nArg1 != 4) {
                        ForceGameExit_(0);
                    } else {
                        ForceGameExit_(1);
                    }
                }
                break;
            case 0x34: {
                int bOther = 0;
                if ((unsigned long)nArg2 > 1) {
                    bOther = 1;
                }
                if (!bOther) {
                    break;
                }
            }
                // fall through
            case 0x35: {
                // A shot: the slow motion of a close shot, and the special
                // move gauge of the shooter's team (five shots fill it, a
                // special shot empties it).
                static const long nMax = 5;
                if ((unsigned long)nArg2 <= 1) {
                    CBall *pBall = CBallMan::GetBallPtr(nArg3);
                    if (pBall && !pBall->GetItemEffect_()) {
                        GetInstance()->PlayExceptionalCameraSlow_(1);
                    }
                }
                if (nArg2 != 6) {
                    CChrCore *pChr = CChrMan::GetChrCore(nArg1);
                    if (pChr) {
                        long nTeam = pChr->GetTeamNo();
                        if (nArg2 >= 9 && nArg2 <= 11) {
                            m_anArtsCount[nTeam] = 0;
                            ClearTeamArtsGauge(nTeam);
                        } else {
                            if (++m_anArtsCount[nTeam] > nMax) {
                                m_anArtsCount[nTeam] = nMax;
                            }
                            float fRate = (float)m_anArtsCount[nTeam] / nMax;
                            if (fRate > 1.0f) {
                                fRate = 1.0f;
                            } else if (fRate < 0.0f) {
                                fRate = 0.0f;
                            }
                            SetTeamArtsGauge(nTeam, fRate, -1.0f);
                        }
                    }
                }
                break;
            }
        }
        CGame::SendMessage_(nMsg, nArg1, nArg2, nArg3);
    }

    float CGameBasket::GetCourtWidth()
    {
        return 15.0f;
    }

    float CGameBasket::GetCourtDepth()
    {
        return 11.5f;
    }

    float CGameBasket::GetCourt3PointSize()
    {
        if (g_GameWork.GetGameType() == GAME_TYPE_PARTY) {
            return BASKET_PRM_BOSS_PACKUN_COURT_3POINT_SIZE;
        }
        return 5.5f;
    }

    // The position of goal nGoal moved 2 out of the court.
    bcs::FVEC3 CGameBasket::GetGoalPos_OffsetZ(long nGoal)
    {
        static const long NUM = 2;
        bcs::FVEC3 aPos[NUM];
        aPos[0] = CStageMan::GetInstance()->GetGoalPos(0);
        aPos[1] = CStageMan::GetInstance()->GetGoalPos(1);
        if (aPos[0].z < 0.0f) {
            aPos[0].z -= 2.0f;
            aPos[1].z += 2.0f;
        } else {
            aPos[0].z += 2.0f;
            aPos[1].z -= 2.0f;
        }
        return aPos[nGoal];
    }

    void CGameBasket::SetOffenseTeam_(long nTeam)
    {
        SetOffenseTeam(nTeam);
    }

    CGame *CGameBasket::CreateInstance()
    {
        if (g_GameWork.GetGameType() == GAME_TYPE_PARTY) {
            return BASKET::CreateGameBasketHungryBossPackun();
        }
        return new CGameBasket(0);
    }

    void CStopEffectBasket::Main()
    {
        m_fDelay -= GetFrameRateF();
        if (m_fDelay <= 0.0f) {
            CGame::StartStopEffect(m_fFrame, m_fPower, 0);
            Delete();
        }
    }
}
