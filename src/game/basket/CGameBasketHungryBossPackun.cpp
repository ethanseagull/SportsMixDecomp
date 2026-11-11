// msw::CGameBasketHungryBossPackun: the game task of the basket party game
// against the hungry boss Piranha Plant (BasketF).
#include "game/msw/CGameBasketHungryBossPackun.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBallManBasketHungryBossPackun.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitBasket.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/Unlock.hpp"

// Set by the file's static initialiser and used by nothing in the file (see
// game/basket/CBallManBasket.cpp: most likely from a shared header).
namespace {
    float s_fAngle70 = bcs::DEG_TO_RAD(70.0f);
}

namespace {
    // The states of the game's state queue that this class changes to
    // (numbers from the code; names guessed).
    enum {
        STATE_WAIT_START = 4,
        STATE_GAME_SET = 12,
        STATE_GAME_RESULT = 13
    };

    // Telops (CGameBasket::CreateTelop_ numbers; names guessed).
    enum {
        TELOP_READY = 0x17,
        TELOP_START = 0x1060,
        TELOP_TIME_UP = 0x1062
    };

    // The crown icon actions of CCockpitBasket::SetPlayerCrownIcon (names
    // guessed).
    enum {
        CROWN_SHOW = 0,
        CROWN_HIDE = 2,
        CROWN_CLEAR = 3
    };
}

namespace msw {

    inline void CGameBasketHungryBossPackun::ClearCrownDisp()
    {
        m_nCrownFlag = 0;
        bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                CCockpitBasket::SetPlayerCrownIcon(pChr->GetPlayerNo(), pChr->GetPlayerNo(), &pos, CROWN_CLEAR);
            }
        }
    }

    inline bool CGameBasketHungryBossPackun::IsPointUpWait() const
    {
        for (long i = 0; i < PLAYER_MAX; i++) {
            if (m_bPointUp[i]) {
                return true;
            }
        }
        return false;
    }

    CGameBasketHungryBossPackun::~CGameBasketHungryBossPackun()
    {
    }

    void CGameBasketHungryBossPackun::TrgGoalIn_(long nChr, long nTeam, long nShootType, long nKind, float,
                                                 const bcs::FVEC3 &rPos)
    {
        if (m_nGameFlag & 1) {
            return;
        }

        long nScore = static_cast<CBallManBasket *>(CBallMan::GetInstance())->GetFruitScore_(nKind);
        CGame::SendMessage(0x36, nChr, nShootType, nScore);

        long nPrev = GetTeamPeriodScore(nTeam, g_GameWork.m_nPeriod);
        long nMax = CTournament::IsTournamentGame() ? 9999 : 999;
        long nTotal = nScore + nPrev;
        long nAdd = nTotal - nPrev;
        if (nTotal > nMax) {
            nAdd = nMax - nPrev;
        }
        AddTeamPeriodScore(nTeam, g_GameWork.m_nPeriod, nAdd);

        DoGoalEffect_(nChr, 0, nShootType, 0, rPos);
        UpDateCrownDisp();

        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr) {
            m_bPointUp[nChr] = 1;
            m_nPointUp[nChr] = nScore;
            if (pChr->IsPadCtrl() && nShootType == 3) {
                CGameBasket::GetInstance()->CameraQuake_(0x14, nChr);
            }
        }

        if (CTournament::IsTournamentGame()) {
            CCockpitBasket::SetTeamScore();
        }
    }

    void CGameBasketHungryBossPackun::CtorStateGameInit()
    {
        CGameBasket::CtorStateGameInit();
        CCockpitBasket::SetTimeDispEnable(0);
        CCockpitBasket::SetScoreDispEnable(0);
        ClearCrownDisp();
        CCockpitBasket::SetHarapekoPlayerScoreDisp(0);
        CCockpitBasket::SetHarapekoTeamScoreDisp(0);
    }

    void CGameBasketHungryBossPackun::CtorStateGameSetup()
    {
        g_GameWork.m_nFlag18 = 0;
        m_nGameFlag = 0;
        KillTempChild_();
        BgResetFade_();
        CChrMan::AllReset();
        CBallMan::AllReset();
        CGimmickMan::AllReset();
        CStageMan::GetInstance()->GoalRepairAll();
        CGimmickMan::SetUp();
        CGimmickMan::QPanelSetViewBaseRot(GetTargetGoalIdx(GetViewTargetTeam(0)) ? 3.14159265f : 0.0f);
        CEffectCtrl::EraseEffectAll();
        SetCockpitDisp_(1);
        CCockpitBasket::SetPartMove(0, 1);
        float fTime = g_GameWork.m_fGameTime;
        CCockpitBasket::SetTime(fTime, IsTimeColorChange());
        SetGoalCoin(0);
        SetGoalEffectCoin(0);
        SetGoalScore(0);
        SetGoalBonusPoint(0);
        SetGoalBonusType(0);
        g_GameWork.m_fUnk38 = 0.0f;
        ClearCoinHistory();
        SetJumpBallMode(0);
        ChrPosSetNormal_();

        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && pChr->IsEnable()) {
                pChr->Util_SetDefenseLeaderFirstState();
            }
        }

        CameraReset_();
        CChrMan::AllSendReset();
        SetPreparationMode(1);

        if (CTournament::IsTournamentGame()) {
            CCockpitBasket::SetHarapekoPlayerScoreDisp(0);
            CCockpitBasket::SetHarapekoTeamScoreDisp(1);
            CCockpitBasket::SetTeamScore();
        } else {
            CCockpitBasket::SetHarapekoPlayerScoreDisp(1);
            CCockpitBasket::SetHarapekoTeamScoreDisp(0);
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CCockpitBasket::SetPlayerScore(i, 0, 0, 0);
            }
        }

        long nLv = GetPartyLv() == 1;
        // Which fruit score display shows fruit i, for the two party
        // levels (-1: none).
        const long tbl[2][4] = {
            { 2, -1, 1, 0 },
            { 3, 2, 1, 0 },
        };
        CCockpitBasket::SetHarapekoDispBallFruit4(nLv);
        for (long i = 0; i < 6; i++) {
            long nDisp = -1;
            switch (i) {
                case 0:
                    nDisp = tbl[nLv][0];
                    break;
                case 1:
                    nDisp = tbl[nLv][1];
                    break;
                case 2:
                    nDisp = tbl[nLv][2];
                    break;
                case 3:
                    nDisp = tbl[nLv][3];
                    break;
            }
            if (nDisp != -1) {
                CCockpitBasket::SetHarapekoBallScore(
                    nDisp, static_cast<CBallManBasket *>(CBallMan::GetInstance())->GetFruitScore_(i));
            }
        }

        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CCockpitBasket::ClearPlayerPointUp(i);
        }

        for (long i = 0; i < PLAYER_MAX; i++) {
            m_bPointUp[i] = 0;
            m_nPointUp[i] = -1;
        }
    }

    void CGameBasketHungryBossPackun::LoopStateGameSetup()
    {
        FsmqPush(STATE_WAIT_START);
        FsmqNext();
    }

    void CGameBasketHungryBossPackun::CtorStateWaitStart()
    {
        CGameBasket::CtorStateWaitStart();
    }

    void CGameBasketHungryBossPackun::LoopStateWaitStart()
    {
        switch (m_nStateStep) {
            case 0:
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(1), static_cast<CChrCore::STOP_MODE>(0));
                m_nStateStep++;
                break;
            case 1:
                if (IsScrnFadeEnd_(3)) {
                    m_fStateCount = 100.0f;
                    CreateTelop_(TELOP_READY, 0, 0);
                    SetTurnStartTelopType_(0, 0.0f);
                    m_nStateStep = 2;
                }
                break;
            case 2:
                if (StateWorkDecStateCount_()) {
                    EndTelop_(TELOP_READY);
                    m_fStateCount = 20.0f;
                    m_nStateStep = 99;
                }
                break;
            default:
                if (StateWorkDecStateCount_()) {
                    CGame::SendMessage(0x2A, 0, 0, 0);
                    PlaySnd_(1, 0);
                    CreateTelop_(TELOP_START, 0, 0);
                    FsmqChangeState(m_nNextState);
                    CChrManBasket::GetInstance()->ResetClockOver_();
                    if (g_GameWork.GetUnk150() == 1) {
                        CChrManBasket::GetInstance()->SetFlagAC(1);
                    }
                }
                break;
        }
    }

    void CGameBasketHungryBossPackun::DtorStateWaitStart()
    {
        CGameBasket::DtorStateWaitStart();
    }

    void CGameBasketHungryBossPackun::LoopStateMainLoop()
    {
        CheckTimeUp_();
        if (m_nGameFlag & 1) {
            // Play has stopped: once no fruit is in play and every point-up
            // telop has shown, the game is set.
            if (CChrMan::CanbeTurnOver() && !IsPointUpWait()) {
                CGame::SendMessage(0x31, 0, 0, 0);
                FsmqChangeState(STATE_GAME_SET);
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            }
        } else {
            IncBallModeCounter(GetFrameRateF());
            IncCoinHistCount(GetFrameRateF());
        }
        UpDatePointUpTelopInfo();
    }

    void CGameBasketHungryBossPackun::CtorStateTimeUp()
    {
        CChrMan::AllSetState(1, -1, 0);
        CreateTelop_(TELOP_TIME_UP, 0, 0);
        m_fStateCount = 180.0f;
        PlaySnd_(2, 0);
        m_nStateStep = 0;
    }

    void CGameBasketHungryBossPackun::LoopStateTimeUp()
    {
        switch (m_nStateStep) {
            case 0:
                if (StateWorkDecStateCount_()) {
                    EndTelop_(TELOP_TIME_UP);
                    m_nStateStep++;
                }
                break;
            case 1:
                if (IsTelopEnd_(TELOP_TIME_UP)) {
                    FsmqChangeState(STATE_GAME_RESULT);
                }
                break;
        }
    }

    void CGameBasketHungryBossPackun::DtorStateTimeUp()
    {
        RemoveTelop_(TELOP_TIME_UP);
    }

    void CGameBasketHungryBossPackun::CtorStateGameSet()
    {
        CreateTelop_(TELOP_TIME_UP, 0, 0);
        PlaySnd_(2, 0);
        m_fStateCount = 60.0f;
        m_nStateStep = 0;
        Unlock::CheckParty(g_GameWork.m_nStage, 1);
        CChrMan::AllSetState(1, -1, 0);
        m_fStateCount = 60.0f;
    }

    void CGameBasketHungryBossPackun::LoopStateGameSet()
    {
        switch (m_nStateStep) {
            case 0:
                m_nStateStep++;
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    m_fStateCount = 180.0f;
                    m_nStateStep++;
                }
                break;
            case 2:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_TIME_UP);
                    m_nStateStep++;
                }
                break;
            case 3:
                if (TelopIsEnd_(TELOP_TIME_UP)) {
                    m_fStateCount = 30.0f;
                    m_nStateStep++;
                }
                break;
            default:
                if (StateWorkDecStateCount_()) {
                    CGame::SendMessage(0x17, 0, 0, 0);
                    FsmqCutNext();
                    FsmqPush(STATE_GAME_RESULT);
                    FsmqNext();
                }
                break;
        }
    }

    void CGameBasketHungryBossPackun::DtorStateGameSet()
    {
        RemoveTelop_(TELOP_TIME_UP);
    }

    void CGameBasketHungryBossPackun::CtorStateGameResult()
    {
        ClearCrownDisp();
        CGameBasket::CtorStateGameResult();
    }

    void CGameBasketHungryBossPackun::LoopStateGameResult()
    {
        CGameBasket::LoopStateGameResult();
    }

    void CGameBasketHungryBossPackun::DtorStateGameResult()
    {
        CGameBasket::DtorStateGameResult();
    }

    int CGameBasketHungryBossPackun::MenuCallGameResult_()
    {
        CCockpitBasket::SetMenuState(CCockpitBasket::STATE_GAMEPARTYRESULT);
        if (!CTournament::IsTournamentGame()) {
            static const long TBL_MAX = 4;
            long anScore[TBL_MAX];
            long anTeam[TBL_MAX];
            long anRank[TBL_MAX];
            long nNum = 0;
            long i;

            for (i = 0; i < TBL_MAX; i++) {
                anScore[i] = 0;
                anTeam[i] = 0;
                anRank[i] = 0;
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && !pChr->IsDisable()) {
                    anScore[nNum] = GetTeamScore(pChr->GetTeamNo());
                    anTeam[nNum] = pChr->GetTeamNo();
                    nNum++;
                }
            }

            // Rank: the number of players with a higher score.
            for (i = 0; i < nNum; i++) {
                for (long j = 0; j < nNum; j++) {
                    if (i != j && anScore[i] < anScore[j]) {
                        anRank[i]++;
                    }
                }
            }

            int bDiffRank = 0;
            for (i = 1; i < nNum; i++) {
                if (anRank[0] != anRank[i]) {
                    bDiffRank = 1;
                    break;
                }
            }

            CCockpit::SetResultPartyVSParameter(nNum, anScore, 0, anTeam, anRank, bDiffRank);
        }
        return 1;
    }

    void CGameBasketHungryBossPackun::SendMessage_(long nMsg, long n1, long n2, long n3)
    {
        CGameBasket::SendMessage_(nMsg, n1, n2, n3);
    }

    int CGameBasketHungryBossPackun::IsGameSet_()
    {
        if (m_Unk130 > 0) {
            return 1;
        }
        return (m_nGameFlag >> 2) & 1;
    }

    void CGameBasketHungryBossPackun::SetGameResult_()
    {
        ClearGameResult_();
        int bAllWin = 1;
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            int bWin = 1;
            for (long j = 0; j < g_GameWork.m_nTeamNum; j++) {
                if (j != i && GetTeamScore(i) < GetTeamScore(j)) {
                    bWin = 0;
                    bAllWin = 0;
                    break;
                }
            }
            if (bWin) {
                SetTeamResult(i, 1);
            } else {
                SetTeamResult(i, 2);
            }
        }
        if (bAllWin) {
            SetGameResultAll_(3);
        }
    }

    void CGameBasketHungryBossPackun::UpDateCrownDisp()
    {
        long i;

        // Every team on the same score: no crown.
        int bSame = 1;
        for (i = 1; i < g_GameWork.m_nTeamNum; i++) {
            if (GetTeamScore(0) != GetTeamScore(i)) {
                bSame = 0;
                break;
            }
        }

        // The leading teams, one bit each.
        unsigned long nTopTeam = 0;
        if (!bSame) {
            unsigned long nMax = 0;
            for (i = 0; i < g_GameWork.m_nTeamNum; i++) {
                unsigned long nScore = GetTeamScore(i);
                if (nScore != 0) {
                    if (nMax < nScore) {
                        nMax = nScore;
                        nTopTeam = 1 << i;
                    } else if (nMax == nScore) {
                        nMax = nScore;
                        nTopTeam |= 1 << i;
                    }
                }
            }
        }

        for (i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                unsigned long nBit = 1 << i;
                int bCrown = (m_nCrownFlag & nBit) != 0;
                if (nTopTeam & (1 << pChr->GetTeamNo())) {
                    if (!bCrown) {
                        bcs::FVEC3 pos(0.0f, BASKET_PRM_COMMON_CROWN_TELOP_DISP_Y, 0.0f);
                        m_nCrownFlag |= nBit;
                        CCockpitBasket::SetPlayerCrownIcon(pChr->GetPlayerNo(), pChr->GetPlayerNo(), &pos, CROWN_SHOW);
                    }
                } else if (bCrown) {
                    bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                    CCockpitBasket::SetPlayerCrownIcon(pChr->GetPlayerNo(), pChr->GetPlayerNo(), &pos, CROWN_HIDE);
                    m_nCrownFlag &= ~nBit;
                }
            }
        }
    }

    void CGameBasketHungryBossPackun::UpDatePointUpTelopInfo()
    {
        float fCheckY = BASKET_PRM_COMMON_POINT_UP_TELOP_CHECK_DISP_Y;
        for (long i = 0; i < PLAYER_MAX; i++) {
            if (!m_bPointUp[i]) {
                continue;
            }
            const CChrCore *pChr = CChrMan::GetChrCore(i);
            if (!pChr || pChr->IsDisable()) {
                continue;
            }
            if (pChr->GetPos().y >= fCheckY) {
                continue;
            }

            bcs::FVEC3 pos(pChr->GetPos().x, pChr->GetPos().y + BASKET_PRM_COMMON_POINT_UP_TELOP_DISP_Y,
                           pChr->GetPos().z);
            CCockpitBasket::SetPlayerPointUp(CTournament::IsTournamentGame() ? pChr->GetTeamNo() : i, &pos,
                                             m_nPointUp[i]);
            if (!CTournament::IsTournamentGame()) {
                CCockpitBasket::SetPlayerScore(pChr->GetPlayerNo(), GetTeamScore(pChr->GetTeamNo()), 1, 1);
            }
            m_bPointUp[i] = 0;
            m_nPointUp[i] = -1;
        }
    }

    CGame *BASKET::CreateGameBasketHungryBossPackun()
    {
        return new CGameBasketHungryBossPackun;
    }

    void CGameBasketHungryBossPackun::EntranceDemoSetUp()
    {
        SetCockpitDisp_(0);
        CChrMan::SetChrMarkerDisp(0);
        SetShotClockCount_(-1, 0);
        CCockpitBasket::SetTimeDispEnable(0);
        CCockpitBasket::SetScoreDispEnable(0);
        CCockpitBasket::SetHarapekoPlayerScoreDisp(0);
        CCockpitBasket::SetHarapekoTeamScoreDisp(0);
    }

    void CGameBasketHungryBossPackun::PrepareRestartGame_()
    {
        ClearCrownDisp();
        CGameBasket::PrepareRestartGame_();
    }

    int CGameBasketHungryBossPackun::IsNowBonusTime_() const
    {
        return !CBallMan::IsActive() ? 0 : static_cast<CBallManBasketHungryBossPackun *>(CBallMan::GetInstance())->IsNowBonusTime_();
    }
}
