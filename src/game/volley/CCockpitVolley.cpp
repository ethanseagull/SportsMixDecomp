// msw::CCockpitVolley: the volley game's cockpit task. It creates the
// displays (score board, SP gauges, coins, the boss and rhythm boards,
// telops, result pages...), forwards the game's calls to them and runs the
// result screens. The class frame is BasketF's
// game/basket/CCockpitBasket.cpp; this file was first ported from HockeyF's
// game/hockey/CCockpitHockey.cpp.
#include "game/msw/CCockpitVolley.hpp"

#include <string.h>
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit3dScoreCount.hpp"
#include "game/msw/CCockpit3dSetCount.hpp"
#include "game/msw/CCockpitChrIcon2DVolley.hpp"
#include "game/msw/CCockpitCombiIcon2D.hpp"
#include "game/msw/CCockpitMarkerIcon2D.hpp"
#include "game/msw/CCockpitMenuBase.hpp"
#include "game/msw/CCockpitPartMove.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CCockpitResult.hpp"
#include "game/msw/CCockpitResultChr.hpp"
#include "game/msw/CCockpitResultPartyTeam.hpp"
#include "game/msw/CCockpitShotClock.hpp"
#include "game/msw/CCockpitSignBoardScreen.hpp"
#include "game/msw/CCockpitTelopMiniPos.hpp"
#include "game/msw/CCockpitTelopStageIntro.hpp"
#include "game/msw/CCockpitTelopSwitchColor.hpp"
#include "game/msw/CCockpitVolleyBoss.hpp"
#include "game/msw/CCockpitVolleyCoder.hpp"
#include "game/msw/CCockpitVolleyCoin.hpp"
#include "game/msw/CCockpitVolleyLifeGauge.hpp"
#include "game/msw/CCockpitVolleyMarkerIcon2D.hpp"
#include "game/msw/CCockpitVolleyModelTelop.hpp"
#include "game/msw/CCockpitVolleyPause.hpp"
#include "game/msw/CCockpitVolleyResult.hpp"
#include "game/msw/CCockpitVolleyRhythmA.hpp"
#include "game/msw/CCockpitVolleyRhythmB.hpp"
#include "game/msw/CCockpitVolleySPGauge.hpp"
#include "game/msw/CCockpitVolleyTelopServe.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CRadarIcon2DVolley.hpp"
#include "game/msw/CSignBoard.hpp"
#include "game/msw/CSignBoardVolley.hpp"
#include "game/msw/cockpit/CockpitUtil.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/cockpit/TelopTimeCount.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/CMessage.hpp"
#include "game/msw/menu/MenuUtil.hpp"

namespace {
    // Events the cockpit sends to its display objects.
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Display objects. Below 54 CreateObjImpl makes them itself; the others
    // come from CCockpit::CreateObjCommonImpl.
    enum {
        OBJ_CODER = 0,
        OBJ_SP_GAUGE = 1,           // and 2 for the second view
        OBJ_SP_GAUGE_2 = 2,
        OBJ_COIN = 3,
        OBJ_RADAR_ICON = 6,
        OBJ_SCORE_COUNT_3D = 7,
        OBJ_SET_COUNT_3D = 8,
        OBJ_RESULT_SCORE = 9,       // the result's set scores (state 4)
        OBJ_RESULT_SCORE_2 = 10,    // the same in state 5
        OBJ_CHR_ICON = 0x24,        // 0x24 to 0x2D: character icons; with a
                                    // split screen the second view has its own
        OBJ_BOSS = 50,              // the boss match's board
        OBJ_LIFE_GAUGE = 51,        // and its life gauge
        OBJ_RHYTHM_A = 52,          // the rhythm game's score board
        OBJ_RHYTHM_B = 53,          // and its receive count
        OBJ_DISP_NUM = 54,          // SetDisp walks ids 0 to 53
        OBJ_SHOT_CLOCK = 0x1002,
        OBJ_RESULT = 0x101B,        // the result's team page (state 4)
        OBJ_RESULT_CHR = 0x101C,    // the result's players (state 4)
        OBJ_RESULT_2 = 0x101D,      // the same two in state 5
        OBJ_RESULT_CHR_2 = 0x101E,
        OBJ_RESULT_END = 0x101F,    // played out when the result closes
        OBJ_TIME_COUNT = 0x1041,    // the result's countdown telop
        // A menu made elsewhere (not by CCockpit::CreateObjCommonImpl);
        // the result screens wait for it to go out.
        OBJ_RESULT_MENU = 0x10B6
    };

    // The parts of a layout CCockpitPartMove entries move.
    enum {
        PART_ALL = 0,
        PART_COIN = 1,
        PART_SCORE = 3,
        PART_COMBO = 4,
        PART_BALL_SCORE = 5 // the ball is the entry's third value
    };

    // What CCockpitPartMove entries ask of a part.
    enum {
        PART_MOVE_IN = 0,
        PART_MOVE_IN_WAIT = 1,
        PART_MOVE_OUT = 2,
        PART_MOVE_OUT_WAIT = 3
    };
}

// static-order swap: in each function with two local static constants
// (SetSetCount, SetBossLifeCoder, AddBossLifeCoder, GameResult_In,
// GamePartyResult_In) the bytes come out in the original order but the two
// `@LOCAL@` names are exchanged (see notes/decisions.md).

// The objects PartMoveCheck moves: the score board, the two SP gauges, the
// coins, the boss boards and the rhythm boards.
static const long objIdTbl[] = { OBJ_CODER, OBJ_SP_GAUGE, OBJ_SP_GAUGE_2, OBJ_COIN,
                                 OBJ_BOSS,  OBJ_LIFE_GAUGE, OBJ_RHYTHM_A, OBJ_RHYTHM_B };

// The constructor and InitState are ordinary out-of-line functions,
// inlined into Create; the original module has no copy of either (its link
// dropped them as unreferenced). See game/tour/CCockpitTournament.cpp.
msw::CCockpitVolley::CCockpitVolley(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk44 = 0;
    InitState();
}

msw::CCockpitVolley::~CCockpitVolley()
{
    Finalize();
}

long msw::CCockpitVolley::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitVolley>::ChangeState(nState);
}

void msw::CCockpitVolley::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_INITIALIZE, &CCockpitVolley::Initialize_In, &CCockpitVolley::Initialize_Loop,
             &CCockpitVolley::Initialize_Out);
    SetState(STATE_GAMERESTART, &CCockpitVolley::GameRestart_In, &CCockpitVolley::GameRestart_Loop,
             &CCockpitVolley::GameRestart_Out);
    SetState(STATE_GAMEPLAY, &CCockpitVolley::GamePlay_In, &CCockpitVolley::GamePlay_Loop,
             &CCockpitVolley::GamePlay_Out);
    SetState(STATE_GAMEPAUSE, &CCockpitVolley::GamePause_In, &CCockpitVolley::GamePause_Loop,
             &CCockpitVolley::GamePause_Out);
    SetState(STATE_GAMERESULT, &CCockpitVolley::GameResult_In, &CCockpitVolley::GameResult_Loop,
             &CCockpitVolley::GameResult_Out);
    SetState(STATE_GAMERESULT_2, &CCockpitVolley::GameResult_In, &CCockpitVolley::GameResult_Loop,
             &CCockpitVolley::GameResult_Out);
    SetState(STATE_GAMEPARTYRESULT, &CCockpitVolley::GamePartyResult_In, &CCockpitVolley::GamePartyResult_Loop,
             &CCockpitVolley::GamePartyResult_Out);
    ChangeState(STATE_INITIALIZE);
}

void msw::CCockpitVolley::Finalize()
{
    RemoveObj(OBJ_CODER);
    RemoveObj(OBJ_SHOT_CLOCK);
    RemoveObj(OBJ_RADAR_ICON);
    RemoveObj(0x1001);
    RemoveObj(OBJ_SP_GAUGE);
    RemoveObj(OBJ_SP_GAUGE_2);
    RemoveObj(OBJ_COIN);
    RemoveObj(0x1006);
    RemoveObj(0x1007);
    RemoveObj(0x1008);
    RemoveObj(0x1009);
    RemoveObj(0x24);
    RemoveObj(0x25);
    RemoveObj(0x26);
    RemoveObj(0x27);
    RemoveObj(0x28);
    RemoveObj(0x29);
    RemoveObj(0x2A);
    RemoveObj(0x2B);
    RemoveObj(0x2C);
    RemoveObj(0x2D);
    RemoveObj(0x2E);
    RemoveObj(0x2F);
    RemoveObj(0x30);
    RemoveObj(0x31);
    RemoveObj(0x1015);
    RemoveObj(0x100F);
    RemoveObj(0x1010);
    RemoveObj(0x1004);
    RemoveObj(0x1005);
    RemoveObj(OBJ_LIFE_GAUGE);
    RemoveObj(OBJ_BOSS);
    RemoveObj(OBJ_RHYTHM_A);
    RemoveObj(OBJ_RHYTHM_B);
    RemoveDemoTelop();
}

void msw::CCockpitVolley::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
    PartMoveCheck();
}

void msw::CCockpitVolley::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitVolley::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitVolley(pParent);
    }
}

void msw::CCockpitVolley::SetDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        for (long i = 0; i < OBJ_DISP_NUM; i++) {
            pTask->CMenuTask::SendObjEventTrigger(i, EVENT_SET_DISP, (void *)bDisp);
        }
        pTask->CCockpit::SetDisp(bDisp);
    }
}

void msw::CCockpitVolley::SetCoinDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->SetCoinDisp(bEnable);
        }
    }
}

void msw::CCockpitVolley::SetScoreDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetScoreDisp(bEnable);
        }
    }
}

void msw::CCockpitVolley::SetSetCountDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSetCountDisp(bEnable);
        }
    }
}

void msw::CCockpitVolley::SetSPGaugeDispEnableImpl(int bEnable)
{
    CCockpitVolleySPGauge *pGauge = (CCockpitVolleySPGauge *)FindObj(OBJ_SP_GAUGE);
    if (pGauge != 0) {
        pGauge->SetSPGaugeDisp(bEnable);
    }
    pGauge = (CCockpitVolleySPGauge *)FindObj(OBJ_SP_GAUGE_2);
    if (pGauge != 0) {
        pGauge->SetSPGaugeDisp(bEnable);
    }
}

void msw::CCockpitVolley::SetRadarIconDisp(int bDisp)
{
    SendObjEventTrigger(OBJ_RADAR_ICON, 0, (void *)bDisp);
}

void msw::CCockpitVolley::SetShotClockCount(long nCount, long nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitShotClock *pShotClock = (CCockpitShotClock *)pTask->FindObj(OBJ_SHOT_CLOCK);
        if (pShotClock != 0) {
            pShotClock->SetTimeCount(nCount, nArg);
        }
    }
}

void msw::CCockpitVolley::SetTeamScore(long nTeam, long nScore, int)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetScoreValue(CGame::GetTeamColorIdx(nTeam), nScore);
        }
    }
}

void msw::CCockpitVolley::SetTeamScoreCoder(long nTeam, long nScore)
{
    CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreValue(CGame::GetTeamColorIdx(nTeam), nScore);
    }
}

void msw::CCockpitVolley::AddTeamScoreCoder(long nTeam, long nScore)
{
    SetTeamScoreCoder(nTeam, nScore);
}

void msw::CCockpitVolley::SetTeamScoreColorCoder(long nTeam, int nFont)
{
    CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreFont(CGame::GetTeamColorIdx(nTeam), nFont);
    }
}

void msw::CCockpitVolley::SetTeamScore()
{
    SetTeamScore(0, CGame::GetTeamPeriodScore(0, CGame::g_GameWork.GetPeriod()), 0);
    SetTeamScore(1, CGame::GetTeamPeriodScore(1, CGame::g_GameWork.GetPeriod()), 0);
}

void msw::CCockpitVolley::PartMoveCheck()
{
    static const long objIdTblLength = 8;
    CCockpitPartMove::PartMoveInfo info;
    const long *pObjIdTbl = objIdTbl;
    CCockpitPartMove &rPartMove = CCockpitPartMove::s_Instance;

    while (rPartMove.Pop(info) >= 0) {
        long nPart = info.m_nPart;
        long nMove = info.m_nArg0;
        long nBall = info.m_nArg1;
        for (long i = 0; i < objIdTblLength; i++) {
            CCockpitVolleyLayoutBase *pObj = (CCockpitVolleyLayoutBase *)FindObj(pObjIdTbl[i]);
            if (pObj == 0) {
                continue;
            }
            switch (nPart) {
                case PART_ALL:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->AllIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->AllOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->AllInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->AllOutWait();
                            break;
                    }
                    break;
                case PART_SCORE:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->ScoreIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->ScoreOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->ScoreInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->ScoreOutWait();
                            break;
                    }
                    break;
                case PART_COIN:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->CoinIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->CoinOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->CoinInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->CoinOutWait();
                            break;
                    }
                    break;
                case PART_COMBO:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->ComboIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->ComboOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->ComboInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->ComboOutWait();
                            break;
                    }
                    break;
                case PART_BALL_SCORE:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->BallScoreIn(nBall);
                            break;
                        case PART_MOVE_OUT:
                            pObj->BallScoreOut(nBall);
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->BallScoreInWait(nBall);
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->BallScoreOutWait(nBall);
                            break;
                    }
                    break;
            }
        }
    }
}

void msw::CCockpitVolley::SetPartMove(long nPart, long nMove, long nArg)
{
    CCockpitPartMove::s_Instance.Push(nPart, nMove, nArg);
}

void msw::CCockpitVolley::SetServeAnim(long nAnim)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetServeScoreAnim(nAnim);
        }
        CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->SetServeScoreAnim(nAnim);
        }
    }
}

// Not in the module: nothing in VolleyF calls it, so the original link
// dropped it and kept only its two constants (BasketF's and DodgeF's
// cockpits have the same pair). The body is a stand-in that uses them; what
// the original did is not known.
void msw::CCockpitVolley::SetSetCount()
{
    static const long TEAM_1 = 1;
    static const long TEAM_0 = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_0), 0);
            pCoder->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_1), 0);
        }
    }
}

void msw::CCockpitVolley::SetSetCount(long nPoint0, long nPoint1)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            long nColor0 = CGame::GetTeamColorIdx(0);
            long nColor1 = CGame::GetTeamColorIdx(1);
            pCoder->SetSetCountPoint(nColor0, nPoint0);
            pCoder->SetSetCountPoint(nColor1, nPoint1);
        }
    }
}

void msw::CCockpitVolley::SetTeamCoinCoder(long nTeam, long nCoin)
{
    CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
}

void msw::CCockpitVolley::AddTeamCoinCoder(long nTeam, long nCoin, long nAdd)
{
    CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->AddCoinValue(CGame::GetTeamColorIdx(nTeam), nAdd, nCoin);
    }
}

void msw::CCockpitVolley::SetCoinSpecialAnim(long nTeam, int bSpecial)
{
    CCockpitVolleySPGauge *pGauge;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        long nState = 5;
        if (bSpecial == 1) {
            nState = 6;
        }
        pGauge = (CCockpitVolleySPGauge *)pTask->FindObj(OBJ_SP_GAUGE);
        if (pGauge != 0) {
            pGauge->SetCoinMaxState(CGame::GetTeamColorIdx(nTeam), nState);
        }
        pGauge = (CCockpitVolleySPGauge *)pTask->FindObj(OBJ_SP_GAUGE_2);
        if (pGauge != 0) {
            pGauge->SetCoinMaxState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitVolley::SetSPGaugeRatio(long nTeam, float fRatio, float fTelop)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleySPGauge *pGauge = (CCockpitVolleySPGauge *)pTask->FindObj(OBJ_SP_GAUGE);
        if (pGauge != 0) {
            pGauge->SetSPGauge(CGame::GetTeamColorIdx(nTeam), fRatio);
            if (fTelop > 0.0f) {
                pGauge->SetSPGaugeTelop(CGame::GetTeamColorIdx(nTeam), fTelop);
            }
        }
        pGauge = (CCockpitVolleySPGauge *)pTask->FindObj(OBJ_SP_GAUGE_2);
        if (pGauge != 0) {
            pGauge->SetSPGauge(CGame::GetTeamColorIdx(nTeam), fRatio);
            if (fTelop > 0.0f) {
                pGauge->SetSPGaugeTelop(CGame::GetTeamColorIdx(nTeam), fTelop);
            }
        }
    }
}

void msw::CCockpitVolley::SetSPContrllerDisp(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleySPGauge *pGauge = (CCockpitVolleySPGauge *)pTask->FindObj(OBJ_SP_GAUGE);
        if (pGauge != 0) {
            pGauge->SetSPControllerState(CGame::GetTeamColorIdx(nTeam), nState);
        }
        pGauge = (CCockpitVolleySPGauge *)pTask->FindObj(OBJ_SP_GAUGE_2);
        if (pGauge != 0) {
            pGauge->SetSPControllerState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitVolley::SetTelop3DScoreCount(long nArg0, long nArg1, long nArg2, long nScore0, long nScore1,
                                               long nArg5, long nArg6, long nArg7, int bArg8, long nArg9)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpit3dScoreCount *pCount = (CCockpit3dScoreCount *)pTask->FindObj(OBJ_SCORE_COUNT_3D);
        if (pCount != 0) {
            CCockpit3dScoreCount::SET_PARAM param;
            param.nArg0 = nArg0;
            param.nArg1 = nArg1;
            param.nUnk08 = 0;
            param.nUnk0C = 0;
            param.nUnk10 = 4;
            param.nUnk14 = 1;
            param.nArg2 = nArg2;
            param.nScore0 = nScore0 > 0 ? nScore0 : 0;
            param.nScore1 = nScore1 > 0 ? nScore1 : 0;
            param.nArg5 = nArg5;
            param.nArg6 = nArg6;
            param.nArg7 = nArg7;
            param.bArg8 = bArg8;
            param.nArg9 = nArg9;
            pCount->StartScoreCount(param);
        }
    }
}

void msw::CCockpitVolley::SetTelopSetCount(long nArg0, long nArg1, long nArg2, long nArg3, long nArg4)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpit3dSetCount *pCount = (CCockpit3dSetCount *)pTask->FindObj(OBJ_SET_COUNT_3D);
        if (pCount != 0) {
            pCount->SetSetCountData(nArg0, nArg1, nArg2, nArg3, nArg4);
        }
    }
}

void msw::CCockpitVolley::SetBossCoinState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyBoss *pBoss = (CCockpitVolleyBoss *)pTask->FindObj(OBJ_BOSS);
        if (pBoss != 0) {
            pBoss->SetCoinState(nState);
        }
    }
}

void msw::CCockpitVolley::SetBossLifeRatio(float fRatio)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyBoss *pBoss = (CCockpitVolleyBoss *)pTask->FindObj(OBJ_BOSS);
        if (pBoss != 0) {
            pBoss->SetBossLife(fRatio);
        }
    }
}

void msw::CCockpitVolley::SetBossTime(float fTime, int nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyBoss *pBoss = (CCockpitVolleyBoss *)pTask->FindObj(OBJ_BOSS);
        if (pBoss != 0) {
            pBoss->SetTimeValue(fTime, nArg, 1);
        }
    }
}

void msw::CCockpitVolley::SetBossLifeCoder(long nLife)
{
    static const long chrNo = 0;
    static const long teamNo = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitVolleyLifeGauge *pGauge = (CCockpitVolleyLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetLifeValue(CGame::GetTeamColorIdx(teamNo), chrNo, nLife);
        }
    }
}

// Not in the module: nothing in VolleyF calls it, so the original link
// dropped it and kept only its two constants. The body is a stand-in that
// uses them (the gauge's own add function is gone as well); what the
// original did is not known.
void msw::CCockpitVolley::AddBossLifeCoder(long nLife, long nAdd)
{
    static const long chrNo = 0;
    static const long teamNo = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitVolleyLifeGauge *pGauge = (CCockpitVolleyLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetLifeValue(CGame::GetTeamColorIdx(teamNo), chrNo, nLife + nAdd);
        }
    }
}

void msw::CCockpitVolley::SetRhythmScore(long nScore)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetScore(nScore);
        }
    }
}

void msw::CCockpitVolley::SetRhythmTargetScore(long nScore, int bArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetTargetScore(nScore, bArg);
        }
    }
}

void msw::CCockpitVolley::SetRhythmHiScore(long nScore, int bArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetHiScore(nScore, bArg);
        }
    }
}

void msw::CCockpitVolley::SetRhythmCombo(long nCombo)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetCombo(nCombo);
        }
    }
}

void msw::CCockpitVolley::SetRhythmReceiveCount(long nCount)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmB *pRhythm = (CCockpitVolleyRhythmB *)pTask->FindObj(OBJ_RHYTHM_B);
        if (pRhythm != 0) {
            pRhythm->SetReceiveCount(nCount);
        }
    }
}

void msw::CCockpitVolley::SetRhythmBonus(long nBonus, float fArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetBonus(nBonus);
            pRhythm->SetBonusDisp(fArg);
        }
    }
}

void msw::CCockpitVolley::SetRhythmBallScore(long nBall, long nScore)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetBallScore(nBall, nScore);
        }
    }
}

void msw::CCockpitVolley::SetRhythmBallIcon(long nBall, long nIcon)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetBallIcon(nBall, nIcon, nIcon != 6);
        }
    }
}

void msw::CCockpitVolley::SetRhythmScorePosition(long nPos)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetScorePosition(nPos);
        }
    }
}

void msw::CCockpitVolley::SetRhythmComboTelop(long nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmA *pRhythm = (CCockpitVolleyRhythmA *)pTask->FindObj(OBJ_RHYTHM_A);
        if (pRhythm != 0) {
            pRhythm->SetComboTelop(nArg);
        }
    }
}

void msw::CCockpitVolley::SetPlayerPointUp(long nPlayer, const bcs::FVEC3 *pPos, long nPoint)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmB *pRhythm = (CCockpitVolleyRhythmB *)pTask->FindObj(OBJ_RHYTHM_B);
        if (pRhythm != 0 && pPos != 0) {
            pRhythm->SetPointUpText(nPlayer, *pPos, nPoint);
        }
    }
}

void msw::CCockpitVolley::ClearPlayerPointUp(long nPlayer)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyRhythmB *pRhythm = (CCockpitVolleyRhythmB *)pTask->FindObj(OBJ_RHYTHM_B);
        if (pRhythm != 0) {
            pRhythm->ClearPointUpText(nPlayer);
        }
    }
}

void msw::CCockpitVolley::SetMissionCountDisp(long nIdx, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountDisp(bDisp);
        }
    }
}

void msw::CCockpitVolley::SetMissionCountNum(long nIdx, long nCount, long nTarget)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountNum(nCount, nTarget);
        }
    }
}

void msw::CCockpitVolley::SetMissionCountUpNum(long nIdx, long nNum, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountUpNum(nNum, nMax);
        }
    }
}

void msw::CCockpitVolley::SetMissionCountActiveColor(int bActive)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetMissionCountActiveColor(bActive);
        }
    }
}

void msw::CCockpitVolley::SetIconFaceImpl()
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoder *pCoder = (CCockpitVolleyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetFaceIcon();
        }
    }
}

void msw::CCockpitVolley::SetChrIcon(long nObjId, long nIcon, long nChr, const bcs::FVEC3 *pPos, const void *pArg)
{
    CCockpitChrIcon2D *pIcon;
    CCockpit *pTask = GetTask();

    if (pTask == 0) {
        return;
    }
    if (pPos == 0) {
        return;
    }
    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return;
    }
    long nColor = CGame::GetTeamColorIdx(pChr->GetTeamNo());

    // Objects 0x24 and 0x25 show only the characters of one team colour
    // each.
    pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
    if (pIcon != 0) {
        pIcon->ClearChrIcon(nIcon, 1, nChr);
        switch (nObjId) {
            case 0x24:
                if (nColor == 0) {
                    pIcon->SetChrIcon(nIcon, *pPos, nChr, pArg);
                }
                break;
            case 0x25:
                if (nColor == 1) {
                    pIcon->SetChrIcon(nIcon, *pPos, nChr, pArg);
                }
                break;
            default:
                pIcon->SetChrIcon(nIcon, *pPos, nChr);
                break;
        }
    }

    // The same icon on the second view's object.
    if (cockpit::GetViewSplitNum() > 1) {
        long nSubId;
        switch (nObjId) {
            case 0x24:
                nSubId = 0x25;
                break;
            case 0x26:
                nSubId = 0x28;
                break;
            case 0x27:
                nSubId = 0x29;
                break;
            case 0x2A:
                nSubId = 0x2C;
                break;
            case 0x2B:
                nSubId = 0x2D;
                break;
            default:
                return;
        }
        pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nSubId);
        if (pIcon != 0) {
            pIcon->ClearChrIcon(nIcon, 1, nChr);
            if (nObjId == 0x24) {
                if (nColor == 1) {
                    pIcon->SetChrIcon(nIcon, *pPos, nChr, pArg);
                }
            } else {
                pIcon->SetChrIcon(nIcon, *pPos, nChr);
            }
        }
    }
}

void msw::CCockpitVolley::ClearChrIcon(long nObjId, long nIcon, long nChr, int bFade)
{
    CCockpitChrIcon2D *pIcon;
    CCockpit *pTask = GetTask();

    if (pTask == 0) {
        return;
    }
    pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
    if (pIcon != 0) {
        pIcon->ClearChrIcon(nIcon, bFade, nChr);
    }

    // The same icon on the second view's object.
    if (cockpit::GetViewSplitNum() > 1) {
        long nSubId;
        switch (nObjId) {
            case 0x24:
                nSubId = 0x25;
                break;
            case 0x26:
                nSubId = 0x28;
                break;
            case 0x27:
                nSubId = 0x29;
                break;
            case 0x2A:
                nSubId = 0x2C;
                break;
            case 0x2B:
                nSubId = 0x2D;
                break;
            default:
                return;
        }
        pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nSubId);
        if (pIcon != 0) {
            pIcon->ClearChrIcon(nIcon, bFade, nChr);
        }
    }
}

void msw::CCockpitVolley::SetCoinTelopState(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->TelopMove(nTeam, nState);
        }
    }
}

long msw::CCockpitVolley::GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return -1;
    }
    return pCoin->GetTargetInfo(nTeam, rPos, rSize);
}

bcs::CTask *msw::CCockpitVolley::Get3DCoinTaskImpl(long nTeam)
{
    CCockpitVolleyCoin *pCoin = (CCockpitVolleyCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return 0;
    }
    return pCoin->Get3DCoin(CGame::GetTeamColorIdx(nTeam)).Get3DCoinTask();
}

long msw::CCockpitVolley::CreateObj(long nObjId, long nParam)
{
    CCockpit *pCockpit = GetTask();

    if (pCockpit == 0) {
        return -2;
    }
    return pCockpit->CreateObjImpl(nObjId, nParam);
}

long msw::CCockpitVolley::CreateObjImpl(long nObjId, long nParam)
{
    CMenuLinkBase *pObj = 0;

    // The cases are in the order of the original's code.
    switch (nObjId) {
        case OBJ_CODER:
            pObj = new CCockpitVolleyCoder(nObjId);
            break;
        case OBJ_SP_GAUGE:
            pObj = new CCockpitVolleySPGauge(nObjId, 0);
            break;
        case OBJ_SP_GAUGE_2:
            pObj = new CCockpitVolleySPGauge(nObjId, 1);
            break;
        case OBJ_COIN:
            pObj = new CCockpitVolleyCoin(nObjId);
            break;
        case 4:
            pObj = new CSignBoardVolley(nObjId, 3, 10.0f);
            break;
        case 5:
            pObj = new CSignBoardVolley(nObjId, 4, 10.0f);
            break;
        case OBJ_RADAR_ICON:
            pObj = new CRadarIcon2DVolley(nObjId);
            break;
        case 0x24:
            pObj = new CCockpitChrIcon2DVolley(nObjId, 0.0f, 0, 0);
            break;
        case 0x25:
            pObj = new CCockpitChrIcon2DVolley(nObjId, 0.0f, 1, 0);
            break;
        case 0x26:
            pObj = new CCockpitChrIcon2DVolley(nObjId, 0.0f, 0, 1);
            break;
        case 0x27:
            pObj = new CCockpitChrIcon2DVolley(nObjId, 0.0f, 0, 2);
            break;
        case 0x28:
            pObj = new CCockpitChrIcon2DVolley(nObjId, 0.0f, 1, 1);
            break;
        case 0x29:
            pObj = new CCockpitChrIcon2DVolley(nObjId, 0.0f, 1, 2);
            break;
        case 0x2A:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 0);
            break;
        case 0x2B:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 1);
            break;
        case 0x2C:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 1, 0);
            break;
        case 0x2D:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 1, 1);
            break;
        case 0x2E:
            pObj = new CCockpitMarkerIcon2D(nObjId, 0.0f, 3, 0);
            break;
        case 0x2F:
            pObj = new CCockpitMarkerIcon2D(nObjId, 0.0f, 3, 1);
            break;
        case 0x30:
            pObj = new CCockpitVolleyMarkerIcon2D(nObjId, 0.0f, 0, 0);
            break;
        case 0x31:
            pObj = new CCockpitVolleyMarkerIcon2D(nObjId, 0.0f, 1, 1);
            break;
        case OBJ_RESULT_SCORE:
        case OBJ_RESULT_SCORE_2:
            pObj = new CCockpitVolleyResult(nObjId, 0.0f);
            break;
        case 11: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 0, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 12: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 2, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 13: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 0xB, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 14: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 15: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 1, 0.0f);
            if (pTelop != 0) {
                pTelop->SetAppendTableParam(0, CGame::g_GameWork.GetPeriod());
                pObj = pTelop;
            }
            break;
        }
        case 16: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 5, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 17:
        case 18:
        case 19:
        case 20: {
            long nType;
            switch (nObjId) {
                case 17:
                    nType = 1;
                    break;
                case 18:
                    nType = 2;
                    break;
                case 19:
                    nType = 3;
                    break;
                case 20:
                    nType = 4;
                    break;
            }
            CCockpitTelopStageIntro *pTelop = new CCockpitTelopStageIntro(nObjId, 0.0f, nType);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case OBJ_SCORE_COUNT_3D:
            if (FindObj(OBJ_SCORE_COUNT_3D) == 0) {
                pObj = new CCockpit3dScoreCount(nObjId, 0.0f);
            }
            break;
        case OBJ_SET_COUNT_3D: {
            CCockpit3dSetCount *pCount = new CCockpit3dSetCount(nObjId, 0.0f);
            if (pCount != 0) {
                pObj = pCount;
            }
            break;
        }
        case 21: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 0x12, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 22: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xB, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 23: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xA, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 24:
            pObj = new CCockpitSignBoardScreen(nObjId, 0x1C, 0.0f);
            break;
        case 25: {
            CCockpitVolleyModelTelop *pTelop = new CCockpitVolleyModelTelop(nObjId, 0, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 26: {
            CCockpitVolleyModelTelop *pTelop = new CCockpitVolleyModelTelop(nObjId, 1, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 27: {
            CCockpitVolleyModelTelop *pTelop = new CCockpitVolleyModelTelop(nObjId, 2, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 28: {
            CCockpitVolleyModelTelop *pTelop = new CCockpitVolleyModelTelop(nObjId, 3, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 29: {
            CCockpitSignBoardScreen *pScreen = new CCockpitSignBoardScreen(nObjId, 0x1D, 0.0f);
            if (pScreen != 0) {
                pObj = pScreen;
            }
            break;
        }
        case OBJ_BOSS: {
            CCockpitVolleyBoss *pBoss = new CCockpitVolleyBoss(nObjId, 0.0f);
            if (pBoss != 0) {
                pObj = pBoss;
            }
            break;
        }
        case OBJ_LIFE_GAUGE: {
            CCockpitVolleyLifeGauge *pGauge = new CCockpitVolleyLifeGauge(nObjId, 1.0f);
            if (pGauge != 0) {
                pObj = pGauge;
            }
            break;
        }
        case OBJ_RHYTHM_A: {
            CCockpitVolleyRhythmA *pRhythm = new CCockpitVolleyRhythmA(nObjId, 0.0f);
            if (pRhythm != 0) {
                pObj = pRhythm;
            }
            break;
        }
        case OBJ_RHYTHM_B: {
            CCockpitVolleyRhythmB *pRhythm = new CCockpitVolleyRhythmB(nObjId, 0.0f);
            if (pRhythm != 0) {
                pObj = pRhythm;
            }
            break;
        }
        case 32:
        case 34:
            pObj = new CCockpitTelopMiniPos(nObjId, 0.0f, 1);
            break;
        case 33:
        case 35:
            pObj = new CCockpitTelopMiniPos(nObjId, 0.0f, 0);
            break;
        case 30:
            pObj = new CCockpitVolleyTelopServe(nObjId, 0.0f);
            break;
        case 31: {
            CCockpitSignBoardScreen *pScreen = new CCockpitSignBoardScreen(nObjId, 0x1E, 0.0f);
            if (pScreen != 0) {
                pObj = pScreen;
            }
            break;
        }
        default:
            pObj = CreateObjCommonImpl(nObjId, nParam);
            break;
    }
    return AddObj(pObj);
}

void msw::CCockpitVolley::SetMenuState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        pTask->ChangeState(nState);
    }
}

void msw::CCockpitVolley::Initialize_In()
{
    long nNextState;

    switch (CGame::g_GameWork.GetGameType()) {
        case 1:
            // The boss match: its board and life gauge only.
            nNextState = STATE_GAMEPLAY;
            CreateObj(OBJ_BOSS, 0);
            CreateObj(OBJ_LIFE_GAUGE, 0);
            Build(OBJ_BOSS);
            Build(OBJ_LIFE_GAUGE);
            break;
        case 2:
            // The rhythm game.
            nNextState = STATE_GAMEPLAY;
            CreateObj(OBJ_RHYTHM_A, 0);
            CreateObj(OBJ_RHYTHM_B, 0);
            CreateObj(0x1015, 0);
            Build(OBJ_RHYTHM_A);
            Build(OBJ_RHYTHM_B);
            Build(0x1015);
            break;
        default:
            nNextState = STATE_GAMEPLAY;
            CreateObj(OBJ_CODER, 0);
            CreateObj(OBJ_SP_GAUGE, 0);
            CreateObj(OBJ_COIN, 0);
            CreateObj(OBJ_SHOT_CLOCK, 0);
            CreateObj(OBJ_RADAR_ICON, 0);
            CreateObj(0x1001, 0);
            CreateObj(0x1006, 0);
            CreateObj(0x1007, 0);
            CreateObj(0x24, 0);
            CreateObj(0x26, 0);
            CreateObj(0x27, 0);
            CreateObj(0x2A, 0);
            CreateObj(0x2B, 0);
            CreateObj(0x2E, 0);
            CreateObj(0x30, 0);
            CreateObj(0x1015, 0);
            CreateObj(0x100F, 0);
            CreateObj(0x1004, 0);
            Build(OBJ_CODER);
            Build(OBJ_SP_GAUGE);
            Build(OBJ_COIN);
            Build(OBJ_SHOT_CLOCK);
            Build(OBJ_RADAR_ICON);
            Build(0x1001);
            Build(0x1006);
            Build(0x1007);
            Build(0x24);
            Build(0x26);
            Build(0x27);
            Build(0x2A);
            Build(0x2B);
            Build(0x2E);
            Build(0x30);
            Build(0x1015);
            Build(0x100F);
            Build(0x1004);
            if (cockpit::GetViewSplitNum() > 1) {
                // The second view's objects.
                CreateObj(OBJ_SP_GAUGE_2, 0);
                CreateObj(0x1008, 0);
                CreateObj(0x1009, 0);
                CreateObj(0x25, 0);
                CreateObj(0x28, 0);
                CreateObj(0x29, 0);
                CreateObj(0x2C, 0);
                CreateObj(0x2D, 0);
                CreateObj(0x2F, 0);
                CreateObj(0x31, 0);
                CreateObj(0x1010, 0);
                CreateObj(0x1005, 0);
                Build(OBJ_SP_GAUGE_2);
                Build(0x1008);
                Build(0x1009);
                Build(0x25);
                Build(0x28);
                Build(0x29);
                Build(0x2C);
                Build(0x2D);
                Build(0x2F);
                Build(0x31);
                Build(0x1010);
                Build(0x1005);
            }
            break;
    }
    if (nNextState >= 0) {
        ChangeState(nNextState);
    }
    CreateDemoTelop();
}

void msw::CCockpitVolley::Initialize_Loop()
{
}

void msw::CCockpitVolley::Initialize_Out()
{
}

void msw::CCockpitVolley::GameRestart_In()
{
    ChangeState(STATE_GAMEPLAY);
}

void msw::CCockpitVolley::GameRestart_Loop()
{
}

void msw::CCockpitVolley::GameRestart_Out()
{
}

void msw::CCockpitVolley::GamePlay_In()
{
}

void msw::CCockpitVolley::GamePlay_Loop()
{
}

void msw::CCockpitVolley::GamePlay_Out()
{
}

void msw::CCockpitVolley::GamePause_In()
{
    CCockpitVolleyPause::Create(GetTask());
}

void msw::CCockpitVolley::GamePause_Loop()
{
}

void msw::CCockpitVolley::GamePause_Out()
{
    CCockpitPause::Remove();
}

void msw::CCockpitVolley::GameResult_In()
{
    static const long TeamMax = 2;
    static const long titleStrLen = 128;
    long aTotal[2];
    ResultMemberInfo memberInfo;
    wchar_t aTitle[titleStrLen];
    unsigned char aSendData[0x200];

    // The team page.
    long nObjId = OBJ_RESULT_2;
    if (m_nState == STATE_GAMERESULT) {
        nObjId = OBJ_RESULT;
    }
    CreateObj(nObjId, 0);
    CCockpitResult *pResult = (CCockpitResult *)FindObj(nObjId);
    if (pResult != 0) {
        Build(nObjId);
        GetResultData(pResult, m_nState == STATE_GAMERESULT, aTotal);
        pResult->SetTitleText(cockpit::GetResultTitleString(aTitle, titleStrLen));
        pResult->SetDelayResultButton();
    }

    // The set scores.
    nObjId = OBJ_RESULT_SCORE_2;
    if (m_nState == STATE_GAMERESULT) {
        nObjId = OBJ_RESULT_SCORE;
    }
    CreateObj(nObjId, 0);
    Build(nObjId);
    CCockpitVolleyResult *pScore = (CCockpitVolleyResult *)FindObj(nObjId);
    if (pScore != 0) {
        long nPeriodNum = m_nPeriodNum;
        long nPeriod = m_nPeriod;
        pScore->SetCountScore(m_aPeriodScore[0], m_aPeriodScore[1], nPeriod, nPeriodNum);
        pScore->SetCountPoint(m_aPeriodPoint[0], m_aPeriodPoint[1], nPeriod);
        pScore->SetPerfectIcon(m_aPeriodPerfect[0], m_aPeriodPerfect[1], nPeriodNum);
        pScore->SetTextNext(nPeriodNum, nPeriod);
        pScore->SetTotalPoint(aTotal[0], aTotal[1]);
    }

    // The players.
    nObjId = OBJ_RESULT_CHR_2;
    if (m_nState == STATE_GAMERESULT) {
        nObjId = OBJ_RESULT_CHR;
    }
    CreateObj(nObjId, 0);
    CCockpitResultChr *pChr = (CCockpitResultChr *)FindObj(nObjId);
    if (pChr != 0) {
        Build(nObjId);
        GetResultMemberData(&memberInfo);
        long nMemberNum = menu::GetTeamPlayerNum(CGame::g_GameWork.m_nUnk06);
        for (long nTeam = 0; nTeam < TeamMax; nTeam++) {
            for (long nMember = 0; nMember < nMemberNum; nMember++) {
                pChr->SetIconFace(memberInfo.nUnk, nTeam, nMember, memberInfo.aChr[nTeam][nMember]);
            }
        }
    }

    if (CGame::g_GameWork.m_Unk151 != 0) {
        if (m_nState == STATE_GAMERESULT) {
            dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
            if (pDwc != 0 && pDwc->m_Dwc.IsConnect()) {
                pDwc->m_UnkDCC = 0;
                memset(aSendData, 0, sizeof(aSendData));
                pDwc->m_Dwc.SetMySingleDataA(aSendData, sizeof(aSendData));
            }
        } else if (CGamePadCheck::GetInstance() != 0) {
            CGamePadCheck::GetInstance()->m_UnkAC = 1;
        }
    }
    m_Unk44 = 0;
}

void msw::CCockpitVolley::GameResult_Loop()
{
    static const float timeCount = 959.0f;
    long nResultId;
    long nScoreId;
    long nChrId;

    if (m_nState == STATE_GAMERESULT) {
        nResultId = OBJ_RESULT;
        nScoreId = OBJ_RESULT_SCORE;
        nChrId = OBJ_RESULT_CHR;
    } else {
        nResultId = OBJ_RESULT_2;
        nScoreId = OBJ_RESULT_SCORE_2;
        nChrId = OBJ_RESULT_CHR_2;
    }

    switch (m_Unk44) {
        case 0:
            // Wait for the menu, then start the countdown.
            if (FindObj(OBJ_RESULT_MENU) != 0) {
                m_Unk44 = 1;
                if (CGame::g_GameWork.m_Unk151 != 0) {
                    CreateObj(OBJ_TIME_COUNT, 0);
                    bcs::FVEC3 pos(220.0f, 180.0f, 0.0f);
                    cockpit::TelopTimeCount::SetParameter(OBJ_TIME_COUNT, 15, &pos);
                    m_fTimeCount = timeCount;
                }
            }
            break;
        case 1: {
            // Close everything when the menu goes out or the time is up.
            int bTimeUp = 0;
            if (CGame::g_GameWork.m_Unk151 != 0) {
                float fTime = CalcTimeCount();
                if (fTime == 0.0f) {
                    bTimeUp = 1;
                }
                cockpit::TelopTimeCount::SetParameter(OBJ_TIME_COUNT, (long)(fTime / 60.0f), 0); // seconds
            }
            CCockpitMenuBase *pMenu = (CCockpitMenuBase *)FindObj(OBJ_RESULT_MENU);
            if (pMenu == 0 || pMenu->IsMenuOut() || bTimeUp) {
                CCockpitLayoutBase *pObj = (CCockpitLayoutBase *)FindObj(nResultId);
                if (pObj != 0) {
                    pObj->CancelMenu();
                }
                pObj = (CCockpitLayoutBase *)FindObj(nChrId);
                if (pObj != 0) {
                    pObj->CancelMenu();
                }
                pObj = (CCockpitLayoutBase *)FindObj(nScoreId);
                if (pObj != 0) {
                    pObj->CancelMenu();
                }
                PlayEndAnim(OBJ_RESULT_END);
                // (pMenu is not checked for null here.)
                if (!pMenu->IsMenuOut()) {
                    pMenu->MenuOut();
                }
                RemoveObj(OBJ_TIME_COUNT);
                m_Unk44 = 2;
            }
            break;
        }
        case 2: {
            // Leave once every result object has gone.
            CMenuLinkBase *pResult = FindObj(nResultId);
            CMenuLinkBase *pChr = FindObj(nChrId);
            CMenuLinkBase *pScore = FindObj(nScoreId);
            CMenuLinkBase *pEnd = FindObj(OBJ_RESULT_END);
            if (pResult == 0 && pChr == 0 && pScore == 0 && pEnd == 0) {
                if (m_nState != STATE_GAMERESULT && CGame::g_GameWork.m_Unk151 != 0 &&
                    CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 0;
                }
                // (Unlike the basket game's, it does not change state.)
                m_Unk48 = 2;
            }
            break;
        }
    }
}

void msw::CCockpitVolley::GameResult_Out()
{
    RemoveObj(OBJ_RESULT);
    RemoveObj(OBJ_RESULT_SCORE);
    RemoveObj(OBJ_RESULT_CHR);
    RemoveObj(OBJ_RESULT_END);
    RemoveObj(OBJ_RESULT_2);
    RemoveObj(OBJ_RESULT_SCORE_2);
    RemoveObj(OBJ_RESULT_CHR_2);
}

void msw::CCockpitVolley::GamePartyResult_In()
{
    static const long titleStrLen = 128;
    static const long resultObjId = 0x1020;
    wchar_t aTitle[titleStrLen];

    CreateObj(resultObjId, 0);
    CCockpitResultPartyTeam *pResult = (CCockpitResultPartyTeam *)FindObj(resultObjId);
    if (pResult != 0) {
        Build(resultObjId);
        GetResultPartyTeamParameter(pResult);
        pResult->SetSubTitleText(cockpit::GetScoreBoardTitleString(aTitle, titleStrLen));

        // The rank line: "HiScore" and the play style's name.
        long nStyle = pResult->m_nPlayStyle - 1;
        const char *aStyleLabel[] = { "PlayStyle_00", "PlayStyle_01", "PlayStyle_02", "PlayStyle_03" };
        if (nStyle >= 0 && nStyle < sizeof(aStyleLabel) / sizeof(aStyleLabel[0])) {
            const char *pFile = "CockpitResult";
            const char *pHiScoreLabel = "HiScore";
            menu::CMessage &rMessage = menu::CMessage::s_Instance;
            const wchar_t *pHiScore = rMessage.GetTextByLabel(rMessage.GetFileIndex(pFile), pHiScoreLabel);
            const wchar_t *pStyle = rMessage.GetTextByLabel(rMessage.GetFileIndex(pFile), aStyleLabel[nStyle]);
            pResult->SetRankText(cockpit::GetMergeString(aTitle, titleStrLen, pHiScore, pStyle, L"\x3000"));
        }
        pResult->SetDelayResultButton();
    }
    m_Unk44 = 0;
}

void msw::CCockpitVolley::GamePartyResult_Loop()
{
    static const long resultObjId = 0x1020;

    switch (m_Unk44) {
        case 0:
            if (FindObj(OBJ_RESULT_MENU) != 0) {
                m_Unk44 = 1;
            }
            break;
        case 1: {
            CCockpitMenuBase *pMenu = (CCockpitMenuBase *)FindObj(OBJ_RESULT_MENU);
            if (pMenu == 0 || pMenu->IsMenuOut()) {
                CCockpitLayoutBase *pResult = (CCockpitLayoutBase *)FindObj(resultObjId);
                if (pResult != 0) {
                    pResult->CancelMenu();
                }
                m_Unk44 = 2;
            }
            break;
        }
        case 2:
            if (FindObj(resultObjId) == 0) {
                m_Unk48 = 2;
            }
            break;
    }
}

void msw::CCockpitVolley::GamePartyResult_Out()
{
    static const long resultObjId = 0x1020;

    RemoveObj(resultObjId);
}
