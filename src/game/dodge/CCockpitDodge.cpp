// msw::CCockpitDodge: the dodgeball game's cockpit task. It creates the
// displays (score board, life gauges, coins, telops, result pages...),
// forwards the game's calls to them and runs the result screens. The class
// frame is the same as BasketF's game/basket/CCockpitBasket.cpp.
#include "game/msw/CCockpitDodge.hpp"

#include <string.h>
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit3dSetCount.hpp"
#include "game/msw/CCockpitChrIcon2DDodge.hpp"
#include "game/msw/CCockpitCombiIcon2D.hpp"
#include "game/msw/CCockpitDodgeBallPower.hpp"
#include "game/msw/CCockpitDodgeCoder.hpp"
#include "game/msw/CCockpitDodgeCoin.hpp"
#include "game/msw/CCockpitDodgeLifeGauge.hpp"
#include "game/msw/CCockpitDodgeModelTelop.hpp"
#include "game/msw/CCockpitDodgeOrDie.hpp"
#include "game/msw/CCockpitDodgePause.hpp"
#include "game/msw/CCockpitDodgeResult.hpp"
#include "game/msw/CCockpitDodgeTelopCourtIn.hpp"
#include "game/msw/CCockpitDodgeTelopInOutChange.hpp"
#include "game/msw/CCockpitDodgeTelopOut.hpp"
#include "game/msw/CCockpitMenuBase.hpp"
#include "game/msw/CCockpitPartMove.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CCockpitResult.hpp"
#include "game/msw/CCockpitResultChr.hpp"
#include "game/msw/CCockpitResultPartyVS.hpp"
#include "game/msw/CCockpitScoreCount.hpp"
#include "game/msw/CCockpitShotClock.hpp"
#include "game/msw/CCockpitSignBoardScreen.hpp"
#include "game/msw/CCockpitTelopMiniPos.hpp"
#include "game/msw/CCockpitTelopStageIntro.hpp"
#include "game/msw/CCockpitTelopSwitchColor.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameDodge.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CLockCursorDodge.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CRadarIcon2DDodge.hpp"
#include "game/msw/CSignBoard.hpp"
#include "game/msw/CSignBoardDodge.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/cockpit/TelopTimeCount.hpp"
#include "game/msw/dwc/CGameDwc.hpp"

namespace {
    // Events the cockpit sends to its display objects.
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Display objects. Below 55 CreateObjImpl makes them itself; the
    // others come from CCockpit::CreateObjCommonImpl.
    enum {
        OBJ_RADAR_ICON = 3,
        OBJ_LOCK_CURSOR = 4,        // and 5 for the second view
        OBJ_LOCK_CURSOR_2 = 5,
        OBJ_CODER = 6,
        OBJ_LIFE_GAUGE = 7,
        OBJ_SET_COUNT_3D = 9,
        OBJ_COIN = 10,
        OBJ_BALL_POWER = 11,
        OBJ_RESULT_SCORE = 12,      // the result's set scores (state 4)
        OBJ_RESULT_SCORE_2 = 13,    // the same in state 5
        OBJ_OUT_TELOP = 29,         // 29 and 30
        OBJ_MINI_TELOP = 43,        // 43 and 44
        OBJ_OR_DIE = 45,
        OBJ_CHR_ICON_0 = 46,
        OBJ_CHR_ICON_1 = 47,
        // Not an object: SetChrIcon and ClearChrIcon take it for the pair
        // OBJ_CHR_ICON_PAD and OBJ_CHR_ICON_PAD_2.
        OBJ_CHR_ICON_PAIR = 48,
        OBJ_CHR_ICON_PAD = 49,      // the icon for one pad style
        OBJ_CHR_ICON_PAD_2 = 50,    // and for the other
        OBJ_CHR_ICON_4 = 51,
        OBJ_CHR_ICON_5 = 52,
        OBJ_COMBI_ICON_0 = 53,
        OBJ_COMBI_ICON_1 = 54,
        OBJ_DISP_NUM = 55,          // SetDisp walks ids 0 to 54
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

    // What CCockpitPartMove entries ask of a part.
    enum {
        PART_MOVE_IN = 0,
        PART_MOVE_IN_WAIT = 1,
        PART_MOVE_OUT = 2,
        PART_MOVE_OUT_WAIT = 3
    };

    // The game type with the "dodge or die" board (CGame::GetGameType).
    enum {
        GAME_TYPE_OR_DIE = 2
    };
}

// static-order swap: in each function with two local static constants
// (SetSetCount, GameResult_In, GameResult_Loop, GamePartyResult_In) the
// bytes come out in the original order but the two `@LOCAL@` names are
// exchanged (see notes/decisions.md). SetSetCount declares TEAM_1 first so
// that its values come out as 1, 0 like the original's.

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

// The objects PartMoveCheck moves: the score board, the life gauges, the
// coins, the ball power and the "dodge or die" board.
static const long objIdTbl[] = { OBJ_CODER, OBJ_LIFE_GAUGE, OBJ_COIN, OBJ_BALL_POWER, OBJ_OR_DIE };

// The constructor and InitState are ordinary out-of-line functions,
// inlined into Create; the original module has no copy of either (its link
// dropped them as unreferenced). See game/tour/CCockpitTournament.cpp.
msw::CCockpitDodge::CCockpitDodge(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk44 = 0;
    InitState();
}

msw::CCockpitDodge::~CCockpitDodge()
{
    Finalize();
}

long msw::CCockpitDodge::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitDodge>::ChangeState(nState);
}

void msw::CCockpitDodge::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_INITIALIZE, &CCockpitDodge::Initialize_In, &CCockpitDodge::Initialize_Loop,
             &CCockpitDodge::Initialize_Out);
    SetState(STATE_GAMERESTART, &CCockpitDodge::GameRestart_In, &CCockpitDodge::GameRestart_Loop,
             &CCockpitDodge::GameRestart_Out);
    SetState(STATE_GAMEPLAY, &CCockpitDodge::GamePlay_In, &CCockpitDodge::GamePlay_Loop,
             &CCockpitDodge::GamePlay_Out);
    SetState(STATE_GAMEPAUSE, &CCockpitDodge::GamePause_In, &CCockpitDodge::GamePause_Loop,
             &CCockpitDodge::GamePause_Out);
    SetState(STATE_GAMERESULT, &CCockpitDodge::GameResult_In, &CCockpitDodge::GameResult_Loop,
             &CCockpitDodge::GameResult_Out);
    SetState(STATE_GAMERESULT_2, &CCockpitDodge::GameResult_In, &CCockpitDodge::GameResult_Loop,
             &CCockpitDodge::GameResult_Out);
    SetState(STATE_GAMEPARTYRESULT, &CCockpitDodge::GamePartyResult_In, &CCockpitDodge::GamePartyResult_Loop,
             &CCockpitDodge::GamePartyResult_Out);
    ChangeState(STATE_INITIALIZE);
}

void msw::CCockpitDodge::Finalize()
{
    RemoveObj(OBJ_CODER);
    RemoveObj(OBJ_LIFE_GAUGE);
    RemoveObj(OBJ_COIN);
    RemoveObj(OBJ_BALL_POWER);
    RemoveObj(OBJ_SHOT_CLOCK);
    RemoveObj(0x1003);
    RemoveObj(OBJ_RADAR_ICON);
    RemoveObj(0x1001);
    RemoveObj(0x1006);
    RemoveObj(0x1007);
    RemoveObj(OBJ_CHR_ICON_4);
    RemoveObj(OBJ_CHR_ICON_5);
    RemoveObj(OBJ_COMBI_ICON_0);
    RemoveObj(OBJ_COMBI_ICON_1);
    RemoveObj(0x1015);
    RemoveObj(0x100F);
    RemoveObj(0x1004);
    RemoveObj(OBJ_OR_DIE);
    RemoveObj(OBJ_CHR_ICON_0);
    RemoveObj(OBJ_CHR_ICON_1);
    RemoveObj(OBJ_CHR_ICON_PAD);
    RemoveObj(OBJ_CHR_ICON_PAD_2);
    RemoveDemoTelop();
}

void msw::CCockpitDodge::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
    PartMoveCheck();
}

void msw::CCockpitDodge::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitDodge::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitDodge(pParent);
    }
}

msw::CCockpit *msw::CCockpitDodge::GetTask()
{
    return CCockpit::GetTask();
}

void msw::CCockpitDodge::SetDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        for (long i = 0; i < OBJ_DISP_NUM; i++) {
            pTask->CMenuTask::SendObjEventTrigger(i, EVENT_SET_DISP, (void *)bDisp);
        }
        pTask->CCockpit::SetDisp(bDisp);
    }
}

void msw::CCockpitDodge::SetCoinDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetCoinDisp(bEnable);
        }
        CCockpitDodgeCoin *pCoin = (CCockpitDodgeCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->SetCoinDisp(bEnable);
        }
    }
}

void msw::CCockpitDodge::SetScoreDispEnable(int bEnable)
{
}

void msw::CCockpitDodge::SetSetCountDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSetCountDisp(bEnable);
        }
    }
}

void msw::CCockpitDodge::SetTimeDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTimeDisp(bEnable);
        }
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetTimeDisp(bEnable);
        }
    }
}

void msw::CCockpitDodge::SetLifeDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetLifeDisp(bEnable);
        }
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetLifeDisp(bEnable);
        }
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetLifeDisp(bEnable);
        }
    }
}

void msw::CCockpitDodge::SetPassBallDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetPassBallDisp(bEnable);
        }
    }
}

void msw::CCockpitDodge::SetSPGaugeDispEnableImpl(int bEnable)
{
    CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetSPGaugeDisp(bEnable);
    }
}

void msw::CCockpitDodge::SetRadarIconDisp(int bDisp)
{
    SendObjEventTrigger(OBJ_RADAR_ICON, 0, (void *)bDisp);
}

void msw::CCockpitDodge::SetShotClockCount(long nCount, long nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitShotClock *pShotClock = (CCockpitShotClock *)pTask->FindObj(OBJ_SHOT_CLOCK);
        if (pShotClock != 0) {
            pShotClock->SetTimeCount(nCount, nArg);
        }
    }
}

void msw::CCockpitDodge::SetTime(float fTime, int nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTimeValue(fTime, nArg, 1);
        }
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetTimeValue(fTime, nArg, 1);
        }
    }
}

void msw::CCockpitDodge::PartMoveCheck()
{
    static const long objIdTblLength = 5;
    CCockpitPartMove::PartMoveInfo info;
    const long *pObjIdTbl = objIdTbl;
    CCockpitPartMove &rPartMove = CCockpitPartMove::s_Instance;

    while (rPartMove.Pop(info) >= 0) {
        long nPart = info.m_nPart;
        long nMove = info.m_nArg0;
        for (long i = 0; i < objIdTblLength; i++) {
            CCockpitDodgeLayoutBase *pObj = (CCockpitDodgeLayoutBase *)FindObj(pObjIdTbl[i]);
            if (pObj == 0) {
                continue;
            }
            switch (nPart) {
                case 0:
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
                case 1:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->GaugeIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->GaugeOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->GaugeInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->GaugeOutWait();
                            break;
                    }
                    break;
                case 2:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->SetCountIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->SetCountOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->SetCountInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->SetCountOutWait();
                            break;
                    }
                    break;
                case 3:
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
            }
        }
    }
}

void msw::CCockpitDodge::SetPartMove(long nPart, long nMove)
{
    CCockpitPartMove::s_Instance.Push(nPart, nMove, 0);
}

// Not in the module: nothing in DodgeF calls it, so the original link
// dropped it and kept only its two constants. The body is a stand-in that
// uses them; what the original did is not known.
void msw::CCockpitDodge::SetSetCount()
{
    static const long TEAM_1 = 1;
    static const long TEAM_0 = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_0), 0);
            pCoder->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_1), 0);
        }
    }
}

void msw::CCockpitDodge::SetSetCount(long nCount0, long nCount1)
{
    static const long TEAM_1 = 1;
    static const long TEAM_0 = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_0), nCount0);
            pCoder->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_1), nCount1);
        }
    }
}

void msw::CCockpitDodge::SetTeamCoinCoder(long nTeam, long nCoin)
{
    CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitDodgeCoin *pCoin = (CCockpitDodgeCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
}

void msw::CCockpitDodge::AddTeamCoinCoder(long nTeam, long nCoin, long nAdd)
{
    CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitDodgeCoin *pCoin = (CCockpitDodgeCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->AddCoinValue(CGame::GetTeamColorIdx(nTeam), nAdd, nCoin);
    }
}

void msw::CCockpitDodge::SetCoinSpecialAnim(long nTeam, int bSpecial)
{
    long nState;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            nState = 5;
            if (bSpecial == 1) {
                nState = 6;
            }
            pCoder->SetCoinMaxState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitDodge::SetCoinSpendAnim(long nTeam, long nNum)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoin *pCoin = (CCockpitDodgeCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->SetCoinState(CGame::GetTeamColorIdx(nTeam), CCockpitDodgeCoin::COIN_STATE_USE, nNum);
        }
    }
}

void msw::CCockpitDodge::SetSPGaugeRatio(long nTeam, float fRatio, float fTelop)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPGauge(CGame::GetTeamColorIdx(nTeam), fRatio);
            if (fTelop > 0.0f) {
                pCoder->SetSPGaugeTelop(CGame::GetTeamColorIdx(nTeam), fTelop);
            }
        }
    }
}

void msw::CCockpitDodge::SetLifeCoder(long nTeam, long nNo, long nLife, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetLifeValue(CGame::GetTeamColorIdx(nTeam), nNo, nLife, nMax);
        }
    }
}

void msw::CCockpitDodge::SetLifePercent(long nTeam, long nNo, float fRate, float fRate2)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetLifeValue(CGame::GetTeamColorIdx(nTeam), nNo, fRate, fRate2);
        }
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetLifeValue(nTeam >= 0 ? CGame::GetTeamColorIdx(nTeam) : -1, nNo, fRate, fRate2);
        }
    }
}

void msw::CCockpitDodge::SetLifeValue(long nTeam, long nNo, unsigned short nLife)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetLifeValueText(CGame::GetTeamColorIdx(nTeam), nNo, nLife);
        }
    }
}

void msw::CCockpitDodge::SetPassCoder(long nPass)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetPassBallValue(nPass);
        }
    }
}

void msw::CCockpitDodge::AddPassCoder(long nPass, long nAdd)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->AddPassBallValue(nPass, nAdd);
        }
    }
}

void msw::CCockpitDodge::AddLifeCoder(long nTeam, long nNo, long nPrevLife, long nLife, long nArg4, long nArg5)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->AddLifeValue(CGame::GetTeamColorIdx(nTeam), nNo, nPrevLife, nLife, nArg4, nArg5);
        }
    }
}

void msw::CCockpitDodge::SetLifeGaugeState(long nTeam, long nNo, long nState, long nArg3, long nArg4)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            long nAnim;
            switch (nState) {
                case 0:
                    nAnim = 0;
                    break;
                case 1:
                    nAnim = 1;
                    break;
                case 2:
                    nAnim = 2;
                    break;
                case 3:
                    nAnim = 3;
                    break;
                default:
                    return;
            }
            pGauge->SetLifeAnim(CGame::GetTeamColorIdx(nTeam), nNo, nAnim, nArg3, nArg4);
        }
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetLifeGaugeAnim(nTeam >= 0 ? CGame::GetTeamColorIdx(nTeam) : -1, nNo, nState);
        }
    }
}

void msw::CCockpitDodge::SetLockCursor(int nType, long nIdx, long nArg)
{
    long nBase = 6;
    if (nType == 1) {
        nBase = 2;
    }
    long nEvent = nBase + nIdx;
    SendObjEventTrigger(OBJ_LOCK_CURSOR, nEvent, (void *)nArg);
    if (bcs::CWorldEnv::m_vstViewSplitType != 0) {
        SendObjEventTrigger(OBJ_LOCK_CURSOR_2, nEvent, (void *)nArg);
    }
}

void msw::CCockpitDodge::SetTelopSetCount(long nArg0, long nArg1, long nArg2, long nArg3, long nArg4)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpit3dSetCount *pCount = (CCockpit3dSetCount *)pTask->FindObj(OBJ_SET_COUNT_3D);
        if (pCount != 0) {
            pCount->SetSetCountData(nArg0, nArg1, nArg2, nArg3, nArg4);
        }
    }
}

void msw::CCockpitDodge::SetOutSign_MuchPoint(long nObjId)
{
    SendObjEventTrigger(nObjId, 2, 0);
}

void msw::CCockpitDodge::SetOutSign_ChrNo(long nObjId, long nChr, long nTeam)
{
    if (nObjId < OBJ_OUT_TELOP || nObjId > OBJ_OUT_TELOP + 1) {
        return;
    }

    CCockpit *pTask = GetTask();
    if (pTask != 0 && pTask->FindObj(OBJ_CODER) != 0) {
        CCockpitDodgeTelopOut *pTelop = (CCockpitDodgeTelopOut *)pTask->FindObj(nObjId);
        if (pTelop != 0) {
            pTelop->SetAppendTableParam(0, nChr);
            pTelop->SetAppendTableParam(1, CGame::GetTeamColorIdx(nTeam));
        }
    }
}

long msw::CCockpitDodge::CreateOutEffect(long, long, bcs::FVEC3 pos)
{
    return 0;
}

long msw::CCockpitDodge::CreateMiniTelop(long nObjId, long nChr, bcs::FVEC3 *pOffset)
{
    static const long camNo = 0;

    if (nObjId < OBJ_MINI_TELOP || nObjId > OBJ_MINI_TELOP + 1) {
        return -13;
    }
    long nChrNum = CChrMan::GetChrCoreNum();
    if (nChr < 0 || nChr >= nChrNum) {
        return -11;
    }
    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0) {
        return -12;
    }
    bcs::FVEC3 pos = pChr->GetPos();
    if (pOffset != 0) {
        bcs::FVEC3Add(&pos, &pos, pOffset);
    }
    return CreateTelopMiniPos(nObjId, camNo, pos);
}

void msw::CCockpitDodge::SetSPContrllerDisp(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPControllerState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitDodge::SetTelopInOut(long nObjId, long nMode, long nTeam, const wchar_t *pText0,
                                       const wchar_t *pText1)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeTelopInOutChange *pTelop = (CCockpitDodgeTelopInOutChange *)pTask->FindObj(nObjId);
        if (pTelop != 0) {
            long nTelopMode;
            switch (nMode) {
                case 0:
                    nTelopMode = 0;
                    break;
                case 1:
                    nTelopMode = 1;
                    break;
                default:
                    return;
            }
            pTelop->SetParameter(CGame::GetTeamColorIdx(nTeam), nTelopMode, pText0, pText1);
        }
    }
}

void msw::CCockpitDodge::SetChargeGauge(long nTeam, long nNo, float fRate, float fRate2)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->ChangeChargeGauge(CGame::GetTeamColorIdx(nTeam), nNo, fRate, fRate2);
        }
    }
}

void msw::CCockpitDodge::SetIconFaceImpl()
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeLifeGauge *pGauge = (CCockpitDodgeLifeGauge *)pTask->FindObj(OBJ_LIFE_GAUGE);
        if (pGauge != 0) {
            pGauge->SetFaceIcon();
        }
    }
}

void msw::CCockpitDodge::SetFaceIcon(long nTeam, long nNo, CChrCore *pChr)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetFaceIcon(nTeam >= 0 ? CGame::GetTeamColorIdx(nTeam) : -1, nNo, pChr);
        }
    }
}

void msw::CCockpitDodge::SetLifeGaugeLayout(long nLayout)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetLifeGaugeLayout(nLayout);
        }
    }
}

void msw::CCockpitDodge::SetMissionCountDisp(long nIdx, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountDisp(bDisp);
        }
    }
}

void msw::CCockpitDodge::SetMissionCountNum(long nIdx, long nNum, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountNum(nNum, nMax);
        }
    }
}

void msw::CCockpitDodge::SetMissionCountUpNum(long nIdx, long nNum, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountUpNum(nNum, nMax);
        }
    }
}

void msw::CCockpitDodge::SetMissionCountActiveColor(int bActive)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetMissionCountActiveColor(bActive);
        }
    }
}

void msw::CCockpitDodge::SetChrIcon(long nObjId, long nIcon, long nChr, const bcs::FVEC3 *pPos)
{
    CCockpitChrIcon2D *pIcon;
    CCockpit *pTask = GetTask();

    if (pTask != 0 && pPos != 0) {
        if (nObjId == OBJ_CHR_ICON_PAIR) {
            // The icon goes on the object for the style of the character's
            // pad (pad 0 for a character the computer plays).
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr == 0 || pChr->IsDisable()) {
                return;
            }
            long nShowId;
            if (pChr->IsPadCtrl()) {
                nShowId = OBJ_CHR_ICON_PAD;
                if (CMenuPad::s_Instance.GetPadStyle(pChr->GetPadNo()) != 0) {
                    nShowId = OBJ_CHR_ICON_PAD_2;
                }
            } else {
                nShowId = OBJ_CHR_ICON_PAD;
                if (CMenuPad::s_Instance.GetPadStyle(0) != 0) {
                    nShowId = OBJ_CHR_ICON_PAD_2;
                }
            }
            long aObjId[] = { OBJ_CHR_ICON_PAD, OBJ_CHR_ICON_PAD_2 };
            // The original's constants, in the order its .rodata has them:
            // padNo0 is the pad read above for a computer character (written
            // as 0 there, since the array has to come first).
            static const long len = 2;
            static const long padNo0 = 0;
            static const int isFadeout = 0;
            for (long i = 0; i < len; i++) {
                pIcon = (CCockpitChrIcon2D *)pTask->FindObj(aObjId[i]);
                if (pIcon != 0) {
                    pIcon->ClearChrIcon(nIcon, isFadeout, nChr);
                    if (nShowId == aObjId[i]) {
                        pIcon->SetChrIcon(nIcon, *pPos, nChr);
                    }
                }
            }
        } else {
            pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
            if (pIcon != 0) {
                pIcon->SetChrIcon(nIcon, *pPos, nChr);
            }
        }
    }
}

void msw::CCockpitDodge::ClearChrIcon(long nObjId, long nIcon, long nChr, int bFade)
{
    CCockpit *pTask = GetTask();
    if (pTask == 0) {
        return;
    }

    if (nObjId == OBJ_CHR_ICON_PAIR) {
        long aObjId[] = { OBJ_CHR_ICON_PAD, OBJ_CHR_ICON_PAD_2 };
        static const long len = 2;
        for (long i = 0; i < len; i++) {
            CCockpitChrIcon2D *pIcon = (CCockpitChrIcon2D *)pTask->FindObj(aObjId[i]);
            if (pIcon != 0) {
                pIcon->ClearChrIcon(nIcon, bFade, nChr);
            }
        }
    } else {
        CCockpitChrIcon2D *pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
        if (pIcon != 0) {
            pIcon->ClearChrIcon(nIcon, bFade, nChr);
        }
    }
}

void msw::CCockpitDodge::SetBallPowerValue(long nTeam, short nValue)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeBallPower *pPower = (CCockpitDodgeBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerValue(CGame::GetTeamColorIdx(nTeam), nValue);
        }
    }
}

void msw::CCockpitDodge::SetBallPowerValueColor(long nTeam, int nColor)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeBallPower *pPower = (CCockpitDodgeBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerValueColor(CGame::GetTeamColorIdx(nTeam), nColor);
        }
    }
}

void msw::CCockpitDodge::SetBallPowerState(long nTeam, long nState)
{
    CCockpitDodgeCoder *pCoder;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        long nColor = CGame::GetTeamColorIdx(nTeam);
        pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetBallPowerState(nColor, nState);
            if (nState != 0) {
                pCoder->SetCoinState(nColor, 1);
            }
        }
        CCockpitDodgeBallPower *pPower = (CCockpitDodgeBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerState(nColor, nState);
        }
    }
}

void msw::CCockpitDodge::SetPowerUpTelopState(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTelopPowerUp(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitDodge::SetBallPowerShift(long nTeam, long nShift)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        long nColor = CGame::GetTeamColorIdx(nTeam);
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetBallPowerShift(nColor, nShift);
        }
        CCockpitDodgeBallPower *pPower = (CCockpitDodgeBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerShift(nColor, nShift);
        }
    }
}

void msw::CCockpitDodge::SetOutTelopState(long nTeam, long nState, int bArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeOrDie *pOrDie = (CCockpitDodgeOrDie *)pTask->FindObj(OBJ_OR_DIE);
        if (pOrDie != 0) {
            pOrDie->SetOutTelop(CGame::GetTeamColorIdx(nTeam), nState, bArg);
        }
    }
}

long msw::CCockpitDodge::GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    CCockpitDodgeBallPower *pPower = (CCockpitDodgeBallPower *)FindObj(OBJ_BALL_POWER);
    if (pPower == 0) {
        return -1;
    }
    return pPower->GetTargetInfo(nTeam, rPos, rSize);
}

void msw::CCockpitDodge::SetPeriod(long nPeriod)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitDodgeCoder *pCoder = (CCockpitDodgeCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetPeriodValue(nPeriod);
        }
    }
}

long msw::CCockpitDodge::CreateObj(long nObjId, long nParam)
{
    CCockpit *pCockpit = GetTask();

    if (pCockpit == 0) {
        return -2;
    }
    return pCockpit->CreateObjImpl(nObjId, nParam);
}

long msw::CCockpitDodge::CreateObjImpl(long nObjId, long nParam)
{
    CMenuLinkBase *pObj = 0;

    // The cases are in the order of the original's code.
    switch (nObjId) {
        case OBJ_CODER:
            pObj = new CCockpitDodgeCoder(nObjId);
            break;
        case OBJ_LIFE_GAUGE:
            pObj = new CCockpitDodgeLifeGauge(nObjId);
            break;
        case OBJ_COIN:
            pObj = new CCockpitDodgeCoin(nObjId);
            break;
        case OBJ_BALL_POWER:
            pObj = new CCockpitDodgeBallPower(nObjId);
            break;
        case 0:
            pObj = new CSignBoardDodge(nObjId, 2, 10.0f);
            break;
        case 1:
            pObj = new CSignBoardDodge(nObjId, 3, 10.0f);
            break;
        case 2:
            pObj = new CSignBoardDodge(nObjId, 4, 10.0f);
            break;
        case 14:
            pObj = new CSignBoardDodge(nObjId, 0, 20.0f);
            break;
        case OBJ_RADAR_ICON:
            pObj = new CRadarIcon2DDodge(nObjId);
            break;
        case OBJ_LOCK_CURSOR:
        case OBJ_LOCK_CURSOR_2:
            pObj = new CLockCursorDodge(nObjId);
            break;
        case OBJ_RESULT_SCORE:
            pObj = new CCockpitDodgeResult(nObjId, 0.0f);
            break;
        case OBJ_RESULT_SCORE_2:
            pObj = new CCockpitDodgeResult(nObjId, 0.0f);
            break;
        case OBJ_CHR_ICON_0:
            pObj = new CCockpitChrIcon2DDodge(nObjId, 0.0f, 0, 0);
            break;
        case OBJ_CHR_ICON_1:
            pObj = new CCockpitChrIcon2DDodge(nObjId, 0.0f, 0, 1);
            break;
        case OBJ_CHR_ICON_PAD:
            pObj = new CCockpitChrIcon2DDodge(nObjId, 0.0f, 0, 2);
            break;
        case OBJ_CHR_ICON_PAD_2:
            pObj = new CCockpitChrIcon2DDodge(nObjId, 0.0f, 0, 3);
            break;
        case OBJ_CHR_ICON_4:
            pObj = new CCockpitChrIcon2DDodge(nObjId, 0.0f, 0, 4);
            break;
        case OBJ_CHR_ICON_5:
            pObj = new CCockpitChrIcon2DDodge(nObjId, 0.0f, 0, 5);
            break;
        case OBJ_COMBI_ICON_0:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 0);
            break;
        case OBJ_COMBI_ICON_1:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 1);
            break;
        case 15: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 9, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 16: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 6, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 17: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 18: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 2, 0.0f);
            if (pTelop != 0) {
                pTelop->SetAppendTableParam(0, CGame::g_GameWork.GetPeriod());
                pObj = pTelop;
            }
            break;
        }
        case 19: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 3, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 20: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 4, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 21: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 5, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 22:
        case 23:
        case 24:
        case 25: {
            long nType;
            switch (nObjId) {
                case 22:
                    nType = 1;
                    break;
                case 23:
                    nType = 2;
                    break;
                case 24:
                    nType = 3;
                    break;
                case 25:
                    nType = 4;
                    break;
            }
            CCockpitTelopStageIntro *pTelop = new CCockpitTelopStageIntro(nObjId, 0.0f, nType);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 26: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 12, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 27: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 13, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case 28: {
            CSignBoard *pBoard = new CSignBoard(nObjId, 14, 0.0f);
            if (pBoard != 0) {
                pBoard->m_nWaitFrameOut = 1;
                pObj = pBoard;
            }
            break;
        }
        case OBJ_OUT_TELOP: {
            CCockpitDodgeTelopOut *pTelop = new CCockpitDodgeTelopOut(nObjId, 0, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case OBJ_OUT_TELOP + 1: {
            CCockpitDodgeTelopOut *pTelop = new CCockpitDodgeTelopOut(nObjId, 1, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 8: {
            CCockpitScoreCount *pCount = new CCockpitScoreCount(nObjId, 0.0f);
            if (pCount != 0) {
                pObj = pCount;
            }
            break;
        }
        case OBJ_SET_COUNT_3D: {
            CCockpit3dSetCount *pCount = new CCockpit3dSetCount(nObjId, 0.0f);
            if (pCount != 0) {
                pObj = pCount;
            }
            break;
        }
        case 38:
        case 39:
        case 40: {
            CCockpitDodgeTelopInOutChange *pTelop = new CCockpitDodgeTelopInOutChange(nObjId, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case OBJ_MINI_TELOP:
            pObj = new CCockpitTelopMiniPos(nObjId, 0.0f, 1);
            break;
        case OBJ_MINI_TELOP + 1:
            pObj = new CCockpitTelopMiniPos(nObjId, 0.0f, 2);
            break;
        case 31: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xB, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 32: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xA, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 33: {
            CCockpitSignBoardScreen *pScreen = new CCockpitSignBoardScreen(nObjId, 0x1E, 0.0f);
            if (pScreen != 0) {
                pObj = pScreen;
            }
            break;
        }
        case OBJ_OR_DIE:
            pObj = new CCockpitDodgeOrDie(nObjId);
            break;
        case 34: {
            CCockpitDodgeModelTelop *pTelop = new CCockpitDodgeModelTelop(nObjId, 0, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 35: {
            CCockpitDodgeModelTelop *pTelop = new CCockpitDodgeModelTelop(nObjId, 1, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 36: {
            CCockpitDodgeModelTelop *pTelop = new CCockpitDodgeModelTelop(nObjId, 2, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 41: {
            CCockpitDodgeTelopCourtIn *pTelop = new CCockpitDodgeTelopCourtIn(nObjId, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 37:
            pObj = new CSignBoard(nObjId, 0x39, 0.0f);
            break;
        default:
            pObj = CreateObjCommonImpl(nObjId, nParam);
            break;
    }
    return AddObj(pObj);
}

void msw::CCockpitDodge::SetMenuState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        pTask->ChangeState(nState);
    }
}

void msw::CCockpitDodge::Initialize_In()
{
    long nNextState;

    if (CGame::g_GameWork.GetGameType() == GAME_TYPE_OR_DIE) {
        // The "dodge or die" board and its icons only.
        nNextState = STATE_GAMEPLAY;
        CreateObj(OBJ_OR_DIE, -1);
        CreateObj(OBJ_CHR_ICON_0, -1);
        CreateObj(OBJ_CHR_ICON_1, -1);
        CreateObj(OBJ_CHR_ICON_PAD, -1);
        CreateObj(OBJ_CHR_ICON_PAD_2, -1);
        CreateObj(0x1015, -1);
        Build(OBJ_OR_DIE);
        Build(OBJ_CHR_ICON_0);
        Build(OBJ_CHR_ICON_1);
        Build(OBJ_CHR_ICON_PAD);
        Build(OBJ_CHR_ICON_PAD_2);
        Build(0x1015);
    } else {
        nNextState = STATE_GAMEPLAY;
        CreateObj(OBJ_CODER, -1);
        CreateObj(OBJ_LIFE_GAUGE, -1);
        CreateObj(OBJ_SHOT_CLOCK, -1);
        CreateObj(0x1003, -1);
        CreateObj(OBJ_RADAR_ICON, -1);
        CreateObj(0x1001, -1);
        CreateObj(0x1006, -1);
        CreateObj(0x1007, -1);
        CreateObj(OBJ_COIN, -1);
        CreateObj(OBJ_BALL_POWER, -1);
        CreateObj(OBJ_CHR_ICON_4, -1);
        CreateObj(OBJ_CHR_ICON_5, -1);
        CreateObj(OBJ_COMBI_ICON_0, -1);
        CreateObj(OBJ_COMBI_ICON_1, -1);
        CreateObj(0x1015, -1);
        CreateObj(0x100F, -1);
        CreateObj(0x1004, -1);
        Build(OBJ_CODER);
        Build(OBJ_LIFE_GAUGE);
        Build(OBJ_SHOT_CLOCK);
        Build(0x1003);
        Build(OBJ_RADAR_ICON);
        Build(0x1001);
        Build(0x1006);
        Build(0x1007);
        Build(OBJ_COIN);
        Build(OBJ_BALL_POWER);
        Build(OBJ_CHR_ICON_4);
        Build(OBJ_CHR_ICON_5);
        Build(OBJ_COMBI_ICON_0);
        Build(OBJ_COMBI_ICON_1);
        Build(0x1015);
        Build(0x100F);
        Build(0x1004);
    }
    if (nNextState >= 0) {
        ChangeState(nNextState);
    }
    CreateDemoTelop();
}

void msw::CCockpitDodge::Initialize_Loop()
{
}

void msw::CCockpitDodge::Initialize_Out()
{
}

void msw::CCockpitDodge::GameRestart_In()
{
    ChangeState(STATE_GAMEPLAY);
}

void msw::CCockpitDodge::GameRestart_Loop()
{
}

void msw::CCockpitDodge::GameRestart_Out()
{
}

void msw::CCockpitDodge::GamePlay_In()
{
}

void msw::CCockpitDodge::GamePlay_Loop()
{
}

void msw::CCockpitDodge::GamePlay_Out()
{
}

void msw::CCockpitDodge::GamePause_In()
{
    CCockpitDodgePause::Create(GetTask());
}

void msw::CCockpitDodge::GamePause_Loop()
{
}

void msw::CCockpitDodge::GamePause_Out()
{
    CCockpitPause::Remove();
}

void msw::CCockpitDodge::GameResult_In()
{
    long nPeriod;
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
    CreateObj(nObjId, -1);
    CCockpitResult *pResult = (CCockpitResult *)FindObj(nObjId);
    if (pResult != 0) {
        Build(nObjId);
        GetResultData(pResult, m_nState == STATE_GAMERESULT, aTotal);
        pResult->SetTitleText(cockpit::GetResultTitleString(aTitle, titleStrLen));
        pResult->SetDelayResultButton();
    }

    // The scores and points of each set.
    nObjId = OBJ_RESULT_SCORE_2;
    if (m_nState == STATE_GAMERESULT) {
        nObjId = OBJ_RESULT_SCORE;
    }
    CreateObj(nObjId, -1);
    CCockpitDodgeResult *pScore = (CCockpitDodgeResult *)FindObj(nObjId);
    if (pScore != 0) {
        Build(nObjId);
        long nPeriodNum = m_nPeriodNum;
        nPeriod = m_nPeriod;
        pScore->SetCountScore(m_aPeriodScore[0], m_aPeriodScore[1], nPeriod, nPeriodNum);
        pScore->SetCountPoint(m_aPeriodPoint[0], m_aPeriodPoint[1], nPeriod);
        pScore->SetTextNext(nPeriodNum, nPeriod);
        pScore->SetTotalPoint(aTotal[0], aTotal[1]);
    }

    // The players.
    nObjId = OBJ_RESULT_CHR_2;
    if (m_nState == STATE_GAMERESULT) {
        nObjId = OBJ_RESULT_CHR;
    }
    CreateObj(nObjId, -1);
    CCockpitResultChr *pChr = (CCockpitResultChr *)FindObj(nObjId);
    if (pChr != 0) {
        Build(nObjId);
        GetResultMemberData(&memberInfo);
        long nMemberNum = CGameDodge::GetVariableChrNum(0);
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

void msw::CCockpitDodge::GameResult_Loop()
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
                cockpit::TelopTimeCount::SetParameter(OBJ_TIME_COUNT, (long)(fTime / 60.0f), 0);
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
            // Done once every result object has gone.
            CMenuLinkBase *pResult = FindObj(nResultId);
            CMenuLinkBase *pChr = FindObj(nChrId);
            CMenuLinkBase *pScore = FindObj(nScoreId);
            CMenuLinkBase *pEnd = FindObj(OBJ_RESULT_END);
            if (pResult == 0 && pChr == 0 && pScore == 0 && pEnd == 0) {
                if (m_nState != STATE_GAMERESULT && CGame::g_GameWork.m_Unk151 != 0 &&
                    CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 0;
                }
                m_Unk48 = 2;
            }
            break;
        }
    }
}

void msw::CCockpitDodge::GameResult_Out()
{
    RemoveObj(OBJ_RESULT);
    RemoveObj(OBJ_RESULT_SCORE);
    RemoveObj(OBJ_RESULT_CHR);
    RemoveObj(OBJ_RESULT_END);
    RemoveObj(OBJ_RESULT_2);
    RemoveObj(OBJ_RESULT_SCORE_2);
    RemoveObj(OBJ_RESULT_CHR_2);
}

void msw::CCockpitDodge::GamePartyResult_In()
{
    static const long titleStrLen = 128;
    static const long resultObjId = 0x1021;
    wchar_t aTitle[titleStrLen];

    CreateObj(resultObjId, -1);
    CCockpitResultPartyVS *pResult = (CCockpitResultPartyVS *)FindObj(resultObjId);
    if (pResult != 0) {
        Build(resultObjId);
        GetResultPartyVSParameter(pResult);
        pResult->SetSubTitleText(cockpit::GetScoreBoardTitleString(aTitle, titleStrLen));
        pResult->SetDelayResultButton();
    }
    m_Unk44 = 0;
}

void msw::CCockpitDodge::GamePartyResult_Loop()
{
    static const long resultObjId = 0x1021;

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

void msw::CCockpitDodge::GamePartyResult_Out()
{
    static const long resultObjId = 0x1021;

    RemoveObj(resultObjId);
}
