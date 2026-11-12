// msw::CGameVolleyVsBoss, the volleyball game against a boss (VolleyF).
// The boss takes twelve hits; every point a character played by a pad
// scores costs it one. The game is set when it has none left or the game
// time runs out. Written after BossF's game/boss/GameBoss.cpp, which has
// the same state flow.
#include "game/msw/CGameVolleyVsBoss.hpp"

#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManVolley.hpp"
#include "game/msw/CCameraVolley.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManVolley.hpp"
#include "game/msw/CCockpitVolley.hpp"
#include "game/msw/CCollManVolley.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCtrlVolley.hpp"
#include "game/msw/CEventDemoVolley.hpp"
#include "game/msw/CFootmarkMan.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CGimmickManVolley.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CShadowTexMan.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {
    namespace {

        // The states of CGame's state queue this class changes to (as in
        // BossF's GameBoss.cpp).
        enum {
            STATE_WAIT_START = 4,
            STATE_TURN_RESULT = 7,
            STATE_TIME_UP = 8,
            STATE_GAME_SET = 12,
            STATE_GAME_RESULT = 13,
            STATE_END_MENU = 15
        };

        // Telops (CGame::TelopCreate_ numbers; names guessed).
        enum {
            TELOP_TIME_UP = 0x16,
            TELOP_READY = 0x17,
            TELOP_START = 0x1060,
            TELOP_GAME_SET = 0x1061
        };

        // Hits the boss takes.
        enum {
            BOSS_LIFE_MAX = 12
        };

        // Where the characters start, one place each in the order they
        // come: x, the yaw in radians, z. For the normal start and for the
        // result.
        bcs::FVEC3 s_aChrPosNormal[3] = {
            bcs::FVEC3(0.0f, 3.1415927f, 11.5f),
            bcs::FVEC3(-2.0f, 3.1415927f, 1.5f),
            bcs::FVEC3(2.0f, 3.1415927f, 1.5f),
        };
        bcs::FVEC3 s_aChrPosResult[3] = {
            bcs::FVEC3(0.0f, 0.0f, 3.0f),
            bcs::FVEC3(2.0f, 0.0f, 2.0f),
            bcs::FVEC3(-2.0f, 0.0f, 2.0f),
        };

        // The leader of the attacking team takes its serve state, every
        // other character in play its defence state.
        inline void SetAllLeaderFirstState()
        {
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && pChr->IsEnable()) {
                    if (pChr->GetStatusFlag() & 0x100) {
                        if (pChr->IsOffenseTeam()) {
                            pChr->Util_SetOffenseLeaderFirstState(0);
                        } else {
                            pChr->Util_SetDefenseLeaderFirstState();
                        }
                    } else {
                        pChr->Util_SetDefenseLeaderFirstState();
                    }
                }
            }
        }

        inline float Clamp(float fValue, float fMin, float fMax)
        {
            return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
        }
    }

    CGameVolleyVsBoss::CGameVolleyVsBoss(bcs::CTask *pParent) : CGameVolley(pParent)
    {
        m_nBossLife = 0;
        m_fBossLifeRatio = 0.0f;
    }

    CGameVolleyVsBoss::~CGameVolleyVsBoss()
    {
    }

    void CGameVolleyVsBoss::Initialize_()
    {
        InitWork_();
    }

    void CGameVolleyVsBoss::MainFunc()
    {
        UpdateCockpitCount_();
    }

    // The boss's life gauge and hit count, and the game time (flashing in
    // the last ten seconds).
    void CGameVolleyVsBoss::UpdateCockpitCount_()
    {
        CCockpitVolley::SetBossLifeRatio(m_fBossLifeRatio);
        if (g_GameWork.m_fUnk20 <= 0.0f) {
            CCockpitVolley::SetBossTime(0.0f, 0);
        } else {
            // flashing below 600 frames (ten seconds)
            float fTime = g_GameWork.m_fGameTime;
            int bWarn = 0;
            if (fTime < 600.0f) {
                bWarn = 1;
            }
            CCockpitVolley::SetBossTime(fTime, bWarn);
        }
        CCockpitVolley::SetBossLifeCoder(m_nBossLife);
        CCockpitVolley::SetBossCoinState(0);
    }

    void CGameVolleyVsBoss::DispFunc()
    {
    }

    void CGameVolleyVsBoss::FinishFirstLoadData()
    {
    }

    void CGameVolleyVsBoss::LoopStateEntranceDemo()
    {
        if (m_nStateStep == 1) {
            CCockpitVolley::SetPartMove(0, 3, 0);
            m_bPartMove = 1;
        }
        CGame::LoopStateEntranceDemo();
    }

    void CGameVolleyVsBoss::SendMessage_(long nMsg, long nArg1, long nArg2, long nArg3)
    {
        switch (nMsg) {
            case 0x63:
                switch (nArg1) {
                    case 1: {
                        // A point against the boss: one hit, if a pad's
                        // character scored it (or no character is named).
                        int bHit = 1;
                        if (nArg2 >= 0 && nArg2 < CChrMan::GetChrCoreNum()) {
                            CChrCore *pChr = CChrMan::GetChrCore(nArg2);
                            bHit = 0;
                            // (as in the original: the member function itself
                            // is tested, which is always true)
                            if (&CChrCore::IsEnable && pChr->IsPadCtrl()) {
                                bHit = 1;
                            }
                        }
                        if (bHit) {
                            if (m_nBossLife > 0) {
                                m_nBossLife--;
                            }
                            if (m_nBossLife <= 0) {
                                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                                CChrMan::AllSetState(5, -1, 0);
                                CChrMan::AllSetState(1, -1, 0);
                                m_nGameFlag |= 3;
                            }
                        }
                        break;
                    }
                    case 2: {
                        // The boss's life gauge: nArg2 of nArg3.
                        float fLife = nArg2;
                        float fMax = nArg3;
                        if (fMax > 0.0f) {
                            m_fBossLifeRatio = fLife / fMax;
                            m_fBossLifeRatio = Clamp(m_fBossLifeRatio, 0.0f, 1.0f);
                        }
                        break;
                    }
                    case 3:
                        switch (nArg2) {
                            case 0:
                                m_fBossLifeRatio = 0.0f;
                                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                                break;
                            case 1:
                                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                                m_nGameFlag |= 3;
                                break;
                        }
                        break;
                }
                break;
        }
        CGameVolley::SendMessage_(nMsg, nArg1, nArg2, nArg3);
    }

    void CGameVolleyVsBoss::GenerateInstance()
    {
        CShadowTexMan::CreateInstance(this);
        CFootmarkMan::CreateInstance(this);
        CCollManVolley::CreateInstance(this);
        CGimmickManVolley::CreateInstance(this);
        CChrManVolley::CreateInstance(this);
        CBallManVolley::CreateInstance(this);
        CStageManVolley::CreateInstance(this, g_GameWork.m_nStage);
        CEffectCtrlVolley::CreateInstance(this);
        CLytCtrl::CreateInstance(this);
        EvDemoVolley::CreateInstance(this);
        CCockpitVolley::Create(this);
        CCameraVolley::Initialize();
        for (long i = 0; i < bcs::CWorldEnv::GetViewSplitNum(); i++) {
            m_apCamera[i] = CGameCamera::GetInstance(i);
        }
    }

    void CGameVolleyVsBoss::StartArtsEvent_(long)
    {
    }

    void CGameVolleyVsBoss::EndArtsEvent_(long)
    {
    }

    int CGameVolleyVsBoss::IsSetOver_()
    {
        return 1;
    }

    int CGameVolleyVsBoss::IsGameSet_()
    {
        return IsSetOver_() != 0;
    }

    void CGameVolleyVsBoss::ChangeGoalIdx_()
    {
        SetGoalIdx(0, 0);
        SetGoalIdx(1, 1);
    }

    void CGameVolleyVsBoss::CtorStateGameSetup()
    {
        CGameVolley::CtorStateGameSetup();
        m_nStateStep = 0;
    }

    void CGameVolleyVsBoss::LoopStateGameSetup()
    {
        CCockpitVolley::SetPartMove(0, 3, 0);
        m_bPartMove = 1;
        FsmqChangeState(STATE_WAIT_START);
    }

    // The ready telop and the count-down of the boss's life gauge filling,
    // then the start.
    void CGameVolleyVsBoss::LoopStateWaitStart()
    {
        switch (m_nStateStep) {
            case 0:
                if (IsScrnFadeEnd_(3)) {
                    CCockpitVolley::SetPartMove(0, 0, 0);
                    m_bPartMove = 0;
                    TelopCreate_(TELOP_READY, 0, 0);
                    m_fStateCount = 180.0f;
                    m_fBossLifeRatio = 1.0f;
                    m_nBossLife = 0;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (m_fStateCount < 24.0f) {
                    TelopEnd_(TELOP_READY);
                    m_nStateStep++;
                }
                // fall through
            default:
                if (StateWorkDecStateCount_()) {
                    m_fBossLifeRatio = 1.0f;
                    m_nBossLife = BOSS_LIFE_MAX;
                    TelopCreate_(TELOP_START, 1, 0);
                    CGameSnd::Play(0xBF4, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    FsmqChangeState(m_nNextState);
                    SendMessage_(0x10, 0, 0, 0);
                } else {
                    m_nBossLife = (180.0f - m_fStateCount) / 15.0f;
                    m_nBossLife = m_nBossLife > BOSS_LIFE_MAX ? BOSS_LIFE_MAX : (m_nBossLife < 0 ? 0 : m_nBossLife);
                }
                break;
        }
    }

    void CGameVolleyVsBoss::CtorStateMainLoop()
    {
        SetReadyDemoMode(0);
        SetPreparationMode(0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(0), static_cast<CChrCore::STOP_MODE>(0));
        CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(4), 0, 0, 0, 0);
        m_nGameFlag &= ~1;
    }

    void CGameVolleyVsBoss::LoopStateMainLoop()
    {
        CheckTimeUp_();
        if (m_nGameFlag & 1) {
            if (CChrMan::CanbeTurnOver()) {
                m_nGameFlag |= 8;
                if (m_nGameFlag & 4) {
                    FsmqChangeState(STATE_TIME_UP);
                } else {
                    FsmqChangeState(STATE_TURN_RESULT);
                }
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            }
        } else {
            CheckScoreDispOut_();
            CheckDispRemoconShake_();
        }
    }

    void CGameVolleyVsBoss::CheckScoreDispOut_()
    {
    }

    void CGameVolleyVsBoss::DtorStateMainLoop()
    {
        DisableDispRemoconShake_();
    }

    void CGameVolleyVsBoss::CheckTimeUp_()
    {
        if (m_nGameFlag & 2) {
            return;
        }
        if (IsReqArtsEvent_()) {
            return;
        }
        if (g_GameWork.m_fUnk20 > 0.0f) {
            float fTime = g_GameWork.m_fGameTime;
            if (fTime <= 0.0f) {
                m_nGameFlag |= 5;
            } else {
                SetTime(fTime - GetFrameRateF());
            }
        }
    }

    void CGameVolleyVsBoss::CtorStateTurnResult()
    {
        SetReplayMode(0);
        SetResultMode(1);
        m_fStateCount = 300.0f;
        CCockpitVolley::SetPartMove(0, 2, 0);
        m_bPartMove = 1;
        m_nStateStep = 0;
    }

    // With the boss beaten the game is set; otherwise (time up) a fade and
    // the result.
    void CGameVolleyVsBoss::LoopStateTurnResult()
    {
        if (m_nStateStep == 0) {
            if (StateWorkDecStateCount_()) {
                if (m_nBossLife <= 0) {
                    FsmqChangeState(STATE_GAME_SET);
                } else {
                    ScrnFadeStart_(2, 0, 0.0625f);
                    m_nStateStep++;
                }
            }
        } else if (IsScrnFadeEnd_(3)) {
            SetPreparationMode(1);
            FsmqChangeState(STATE_GAME_RESULT);
        }
    }

    void CGameVolleyVsBoss::DtorStateTurnResult()
    {
    }

    void CGameVolleyVsBoss::CtorStateTimeUp()
    {
        CGameSnd::Play(0xBF5, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        SetResultMode(1);
        TelopCreate_(TELOP_TIME_UP, 0, 0);
        m_fStateCount = 30.0f;
        m_nStateStep = 0;
    }

    void CGameVolleyVsBoss::LoopStateTimeUp()
    {
        switch (m_nStateStep) {
            case 0:
                if (StateWorkDecStateCount_()) {
                    CChrMan::AllSetState(1, -1, 0);
                    m_fStateCount = 90.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_TIME_UP);
                    m_nStateStep++;
                }
                break;
            default:
                if (TelopIsEnd_(TELOP_TIME_UP)) {
                    FsmqChangeState(STATE_GAME_SET);
                }
                break;
        }
    }

    void CGameVolleyVsBoss::DtorStateTimeUp()
    {
        TelopRemove_(TELOP_TIME_UP);
    }

    void CGameVolleyVsBoss::CtorStateGameSet()
    {
        if (!m_bPartMove) {
            CCockpitVolley::SetPartMove(0, 2, 0);
            m_bPartMove = 1;
        }
        TelopCreate_(TELOP_GAME_SET, 0, 0);
        m_fStateCount = 330.0f;
        m_nStateStep = 1;
    }

    void CGameVolleyVsBoss::LoopStateGameSet()
    {
        switch (m_nStateStep) {
            case 0:
                if (TelopIsWait_(TELOP_GAME_SET)) {
                    m_fStateCount = 210.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_GAME_SET);
                    m_nStateStep++;
                }
                break;
            default:
                if (TelopIsEnd_(TELOP_GAME_SET)) {
                    FsmqChangeState(STATE_END_MENU);
                }
                break;
        }
    }

    void CGameVolleyVsBoss::DtorStateGameSet()
    {
        TelopRemove_(TELOP_GAME_SET);
    }

    void CGameVolleyVsBoss::DtorStateRestart()
    {
        CGame::DtorStateRestart();
        TelopRemove_(TELOP_GAME_SET);
    }

    // Team 1 wins when the boss has no hits left or the time has run out.
    void CGameVolleyVsBoss::SetGameResult_()
    {
        ClearGameResult_();
        long nWinTeam;
        if (m_nBossLife <= 0 || (g_GameWork.m_fUnk20 > 0.0f && g_GameWork.m_fGameTime <= 0.0f)) {
            nWinTeam = 1;
        } else {
            nWinTeam = 0;
        }
        if (nWinTeam < 0) {
            SetGameResultAll_(3);
        } else {
            SetGameResultAll_(2);
            SetTeamResult(nWinTeam, 1);
        }
    }

    // A point: bit 0 of nFlag keeps play going (no stop, no fade), bit 1
    // leaves the characters alone, bit 2 records no score.
    void CGameVolleyVsBoss::TrgVolleyGoalIn_(long nChr, long nTeam, long nShootType, long nScore, long nCoin, long,
                                             const bcs::FVEC3 &, unsigned long nFlag)
    {
        long nType = nShootType;
        if (nType < 0) {
            nType = 0;
        }
        if (m_nNextState == GetState()) {
            if (!(m_nGameFlag & 8)) {
                if (!(nFlag & 1)) {
                    m_nGameFlag |= 1;
                }
                if (!(nFlag & 2)) {
                    if (!(m_nGameFlag & 2)) {
                        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                        CChrMan::AllSetState(0, nChr, 0);
                    }
                    m_nGameFlag |= 2;
                }
                if (!(nFlag & 4)) {
                    SetGoalFinishMode(1);
                    SetGoalEventMode(1);
                    SetGoalShootType(nType, 0.0f);
                    SetGoalChar(nChr);
                    SetGoalTeam(nTeam);
                    SetGoalBaseScore(nScore);
                    SetGoalScore(nScore + nCoin);
                    SetGoalCoin(nCoin);
                    AddTeamPeriodScore(nTeam, g_GameWork.m_nPeriod, nScore);
                    CCockpitVolley::SetTeamScore(nTeam, g_GameWork.m_nPeriod, 1);
                    CChrMan::SendMsg(nChr, static_cast<CChrDrv::MSG_ID>(8), nType, 0, 0, 0);
                    m_nScoredTeam = nTeam;
                    m_bScored = 1;
                }
                if (!(nFlag & 1)) {
                    BgSetFadeIn(70.0f, 30.0f);
                }
            }
        } else if (g_GameWork.m_nFlag18 & 0x100) {
            SetGoalEventMode(1);
            SetGoalShootType(nType, 0.0f);
            SetGoalChar(nChr);
            SetGoalTeam(nTeam);
            BgSetFadeIn(70.0f, 30.0f);
        }
        SetReboundMode(0);
    }

    int CGameVolleyVsBoss::IsVolleyScored_() const
    {
        return m_bScored;
    }

    void CGameVolleyVsBoss::GameInit_()
    {
        m_nGameFlag = 0;
        GameWorkReset_();
        CChrMan::InitLeader(1);
        SendMessage_(0xF, 0, 0, 0);
        SendMessage_(0x12, 0, 0, 0);
        m_nUnk104 = 0;
        m_nUnk108 = 0;
        m_nUnk12C = 0;
        m_Unk130 = 0;
        m_nUnk21C = 0;
    }

    void CGameVolleyVsBoss::GameSetUp_()
    {
        m_nGameFlag = 0;
        m_bScored = 0;
        m_nUnk1E8 = 0;
        m_nUnk1F4 = -1;
        KillTempChild_();
        BgResetFade_();
        CChrMan::AllReset();
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        CBallMan::AllReset();
        CGimmickMan::AllReset();
        CEffectCtrl::EraseEffectAll();
        CGimmickMan::SetUp();
        ClearLastGoalInfo();
        g_GameWork.m_fUnk38 = 0.0f;
        ClearCoinHistory();
        SetVolleyServeMode(1);
        m_nBossLife = 0;
        m_fBossLifeRatio = 0.0f;
        SetOffenseTeam(0);

        long nTeam = g_GameWork.GetUserTeam();
        m_nScoredTeam = nTeam;
        m_nServeTeam = nTeam;
        m_anServeOrder[0] = 0;
        m_anServeOrder[1] = 1;
        m_anServeOrder[2] = 2;
        m_anServeOrder[3] = 3;
        ChrPosSetNormal_(0);

        SetAllLeaderFirstState();

        SetLooseBallModeFlag(0);
        SetVolleyServeMode(1);
        CameraReset_();
        CChrMan::AllSendReset();
        SendMessage_(0x13, 0, 0, 0);
    }

    // The characters in play at the start places, in their order.
    void CGameVolleyVsBoss::ChrPosSetNormal_(int)
    {
        long nNum = CChrMan::GetChrCoreNum();
        long nPos = 0;
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            const bcs::FVEC3 &rPos = s_aChrPosNormal[nPos++];
            float x = rPos.x;
            float fRot = rPos.y;
            float z = rPos.z;
            bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(0);
            x += ofs.x;
            z += ofs.z;
            pChr->SetRotY(fRot);
            pChr->SetPos(bcs::FVEC3(x, 0.0f, z));
            pChr->RecalcAllMatrix();
        }
    }

    void CGameVolleyVsBoss::ChrPosSetResult_()
    {
        long nNum = CChrMan::GetChrCoreNum();
        long nPos = 0;
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            const bcs::FVEC3 &rPos = s_aChrPosResult[nPos++];
            float x = rPos.x;
            float fRot = rPos.y;
            float z = rPos.z;
            bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(0);
            x += ofs.x;
            z += ofs.z;
            pChr->SetRotY(fRot);
            pChr->SetPos(bcs::FVEC3(x, 0.0f, z));
            pChr->RecalcAllMatrix();
        }
    }

    long CGameVolleyVsBoss::GetFirstGoalIdxMode_()
    {
        return 0;
    }

    int CGameVolleyVsBoss::MenuCallGameResult_()
    {
        return 0;
    }

    int CGameVolleyVsBoss::IsVolleyServeClockOver_() const
    {
        return 0;
    }
}
