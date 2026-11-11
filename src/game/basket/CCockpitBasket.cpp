// msw::CCockpitBasket: the basket game's cockpit task. It creates the
// displays (score board, coins, result pages...), forwards the game's
// calls to them and runs the result screens. Written with
// game/tour/CCockpitTournament.cpp as the model for the class frame.
#include "game/msw/CCockpitBasket.hpp"

#include <string.h>
#include "bcs/BCS_Math.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CCockpit3dScoreCount.hpp"
#include "game/msw/CCockpitBasketBonusTime.hpp"
#include "game/msw/CCockpitBasketCoder.hpp"
#include "game/msw/CCockpitBasketCoin.hpp"
#include "game/msw/CCockpitBasketHarapeko.hpp"
#include "game/msw/CCockpitBasketModelTelop.hpp"
#include "game/msw/CCockpitBasketPause.hpp"
#include "game/msw/CCockpitBasketResult.hpp"
#include "game/msw/CCockpitChrIcon2DBasket.hpp"
#include "game/msw/CCockpitCombiIcon2D.hpp"
#include "game/msw/CCockpitMenuBase.hpp"
#include "game/msw/CCockpitPartMove.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CCockpitResult.hpp"
#include "game/msw/CCockpitResultChr.hpp"
#include "game/msw/CCockpitResultPartyVS.hpp"
#include "game/msw/CCockpitShotClock.hpp"
#include "game/msw/CCockpitSignBoardScreen.hpp"
#include "game/msw/CCockpitTelopStageIntro.hpp"
#include "game/msw/CCockpitTelopSwitchColor.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CRadarIcon2DBasket.hpp"
#include "game/msw/CSignBoard.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/cockpit/TelopTimeCount.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/MenuUtil.hpp"

namespace {
    // Events the cockpit sends to its display objects.
    enum {
        EVENT_SET_DISP = 10001,       // pParam: nonzero to show
        EVENT_SCORE_ENABLE = 10008,   // the next five: pParam nonzero to enable
        EVENT_TIME_ENABLE = 10009,
        EVENT_PERIOD_ENABLE = 10010,
        EVENT_COIN_ENABLE = 10011,
        EVENT_SPGAUGE_ENABLE = 10012
    };

    // Display objects. Below 30 CreateObjImpl makes them itself; the
    // others come from CCockpit::CreateObjCommonImpl.
    enum {
        OBJ_CODER = 0,
        OBJ_HARAPEKO = 1,
        OBJ_COIN = 2,
        OBJ_RADAR_ICON = 6,
        OBJ_SCORE_COUNT_3D = 7,
        OBJ_RESULT_SCORE = 8,       // the result's period scores (state 4)
        OBJ_RESULT_SCORE_2 = 9,     // the same in state 5
        OBJ_STAGE_INTRO = 15,       // 15 to 18 all make this telop
        OBJ_DISP_NUM = 30,          // SetDisp walks ids 0 to 29
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
}

// static-order swap: in each function with two local static constants
// (SetSetCount, GameResult_In, GameResult_Loop, GamePartyResult_In) the
// bytes come out in the original order but the two `@LOCAL@` names are
// exchanged (see notes/decisions.md). SetSetCount declares TEAM_1 first so
// that its values come out as 1, 0 like the original's.

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// The objects PartMoveCheck moves: the score board, the hungry-ball board
// and the coins.
static const long objIdTbl[] = { OBJ_CODER, OBJ_HARAPEKO, OBJ_COIN };

// The constructor and InitState are ordinary out-of-line functions,
// inlined into Create; the original module has no copy of either (its link
// dropped them as unreferenced). See game/tour/CCockpitTournament.cpp.
msw::CCockpitBasket::CCockpitBasket(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk44 = 0;
    InitState();
}

msw::CCockpitBasket::~CCockpitBasket()
{
    Finalize();
}

long msw::CCockpitBasket::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitBasket>::ChangeState(nState);
}

void msw::CCockpitBasket::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_INITIALIZE, &CCockpitBasket::Initialize_In, &CCockpitBasket::Initialize_Loop,
             &CCockpitBasket::Initialize_Out);
    SetState(STATE_GAMERESTART, &CCockpitBasket::GameRestart_In, &CCockpitBasket::GameRestart_Loop,
             &CCockpitBasket::GameRestart_Out);
    SetState(STATE_GAMEPLAY, &CCockpitBasket::GamePlay_In, &CCockpitBasket::GamePlay_Loop,
             &CCockpitBasket::GamePlay_Out);
    SetState(STATE_GAMEPAUSE, &CCockpitBasket::GamePause_In, &CCockpitBasket::GamePause_Loop,
             &CCockpitBasket::GamePause_Out);
    SetState(STATE_GAMERESULT, &CCockpitBasket::GameResult_In, &CCockpitBasket::GameResult_Loop,
             &CCockpitBasket::GameResult_Out);
    SetState(STATE_GAMERESULT_2, &CCockpitBasket::GameResult_In, &CCockpitBasket::GameResult_Loop,
             &CCockpitBasket::GameResult_Out);
    SetState(STATE_GAMEPARTYRESULT, &CCockpitBasket::GamePartyResult_In, &CCockpitBasket::GamePartyResult_Loop,
             &CCockpitBasket::GamePartyResult_Out);
    ChangeState(STATE_INITIALIZE);
}

void msw::CCockpitBasket::Finalize()
{
    RemoveObj(OBJ_CODER);
    RemoveObj(OBJ_SHOT_CLOCK);
    RemoveObj(0x1003);
    RemoveObj(OBJ_RADAR_ICON);
    RemoveObj(0x1001);
    RemoveObj(0x1004);
    RemoveObj(OBJ_COIN);
    RemoveObj(0x1006);
    RemoveObj(0x1007);
    RemoveObj(0x100A);
    RemoveObj(0x100B);
    RemoveObj(0x100C);
    RemoveObj(0x1A);
    RemoveObj(0x1B);
    RemoveObj(0x1C);
    RemoveObj(0x1D);
    RemoveObj(0x1015);
    RemoveObj(0x100F);
    RemoveObj(OBJ_HARAPEKO);
    RemoveDemoTelop();
}

void msw::CCockpitBasket::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
    PartMoveCheck();
}

void msw::CCockpitBasket::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitBasket::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitBasket(pParent);
    }
}

void msw::CCockpitBasket::SetDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        for (long i = 0; i < OBJ_DISP_NUM; i++) {
            pTask->CMenuTask::SendObjEventTrigger(i, EVENT_SET_DISP, (void *)bDisp);
        }
        pTask->CCockpit::SetDisp(bDisp);
    }
}

void msw::CCockpitBasket::SetCoinDispEnable(int bEnable)
{
    SendObjEventTrigger(OBJ_CODER, EVENT_COIN_ENABLE, (void *)bEnable);
    SendObjEventTrigger(OBJ_COIN, EVENT_SET_DISP, (void *)bEnable);
}

void msw::CCockpitBasket::SetScoreDispEnable(int bEnable)
{
    SendObjEventTrigger(OBJ_CODER, EVENT_SCORE_ENABLE, (void *)bEnable);
}

void msw::CCockpitBasket::SetTimeDispEnable(int bEnable)
{
    SendObjEventTrigger(OBJ_CODER, EVENT_TIME_ENABLE, (void *)bEnable);
}

void msw::CCockpitBasket::SetPeriodDispEnable(int bEnable)
{
    SendObjEventTrigger(OBJ_CODER, EVENT_PERIOD_ENABLE, (void *)bEnable);
}

void msw::CCockpitBasket::SetSPGaugeDispEnableImpl(int bEnable)
{
    SendObjEventTrigger(OBJ_CODER, EVENT_SPGAUGE_ENABLE, (void *)bEnable);
}

void msw::CCockpitBasket::SetTime(float fTime, int nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTimeValue(fTime, nArg, 1);
        }
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetTime(fTime, nArg, 1);
        }
    }
}

void msw::CCockpitBasket::SetRadarIconDisp(int bDisp)
{
    SendObjEventTrigger(OBJ_RADAR_ICON, 0, (void *)bDisp);
}

void msw::CCockpitBasket::SetShotClockCount(long nCount, long nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitShotClock *pShotClock = (CCockpitShotClock *)pTask->FindObj(OBJ_SHOT_CLOCK);
        if (pShotClock != 0) {
            pShotClock->SetTimeCount(nCount, nArg);
        }
    }
}

void msw::CCockpitBasket::SetTeamScore()
{
    CCockpitBasketCoder *pCoder;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        long nColor0 = CGame::GetTeamColorIdx(0);
        long nColor1 = CGame::GetTeamColorIdx(1);
        long nScore0 = CGame::GetTeamScore(nColor0);
        long nScore1 = CGame::GetTeamScore(nColor1);

        pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetScoreValue(0, nScore0);
            pCoder->SetScoreValue(1, nScore1);
        }
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetTeamScore(0, nScore0);
            pHarapeko->SetTeamScore(1, nScore1);
        }
    }
}

// Not in the module: nothing in BasketF calls it, so the original link
// dropped it and kept only its two constants (the dodge game's cockpit has
// the same pair). The body is a stand-in that uses them; what the original
// did is not known.
void msw::CCockpitBasket::SetSetCount()
{
    static const long TEAM_1 = 1;
    static const long TEAM_0 = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetScoreValue(CGame::GetTeamColorIdx(TEAM_0), 0);
            pCoder->SetScoreValue(CGame::GetTeamColorIdx(TEAM_1), 0);
        }
    }
}

void msw::CCockpitBasket::SetPeriod(long nPeriod)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetPeriodValue(nPeriod);
        }
    }
}

void msw::CCockpitBasket::SetTeamScoreCoder(long nTeam, long nScore)
{
    CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreValue(CGame::GetTeamColorIdx(nTeam), nScore);
    }
    CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)FindObj(OBJ_HARAPEKO);
    if (pHarapeko != 0) {
        pHarapeko->SetTeamScore(CGame::GetTeamColorIdx(nTeam), nScore);
    }
}

void msw::CCockpitBasket::AddTeamScoreCoder(long nTeam, long nScore)
{
    CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreValue(CGame::GetTeamColorIdx(nTeam), nScore);
    }
    CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)FindObj(OBJ_HARAPEKO);
    if (pHarapeko != 0) {
        pHarapeko->SetTeamScore(CGame::GetTeamColorIdx(nTeam), nScore);
    }
}

void msw::CCockpitBasket::SetTeamScoreColorCoder(long nTeam, int nFont)
{
    CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreFont(CGame::GetTeamColorIdx(nTeam), nFont);
    }
}

void msw::CCockpitBasket::SetTeamCoinCoder(long nTeam, long nCoin)
{
    CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitBasketCoin *pCoin = (CCockpitBasketCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
}

void msw::CCockpitBasket::AddTeamCoinCoder(long nTeam, long nCoin, long nAdd)
{
    CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitBasketCoin *pCoin = (CCockpitBasketCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->AddCoinValue(CGame::GetTeamColorIdx(nTeam), nAdd, nCoin);
    }
}

void msw::CCockpitBasket::SetCoinSpecialAnim(long nTeam, int bSpecial)
{
    long nState;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            nState = 5;
            if (bSpecial == 1) {
                nState = 6;
            }
            pCoder->SetCoinMaxState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitBasket::SetSPGaugeRatio(long nTeam, float fRatio, float fTelop)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPGauge(CGame::GetTeamColorIdx(nTeam), fRatio);
            if (fTelop > 0.0f) {
                pCoder->SetSPGaugeTelop(CGame::GetTeamColorIdx(nTeam), fTelop);
            }
        }
    }
}

void msw::CCockpitBasket::SetSPContrllerDisp(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPControllerState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitBasket::SetTelop3DScoreCount(long nArg0, long nArg1, long nArg2, long nScore0, long nScore1,
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
            param.nUnk10 = 1;
            param.nUnk14 = 0;
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

void msw::CCockpitBasket::SetArrowState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
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
                    nAnim = 4;
                    break;
            }
            pCoder->SetArrowStateAnim(nAnim);
        }
    }
}

void msw::CCockpitBasket::SetScoreArrowState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            long nAnim;
            switch (nState) {
                case 0:
                    nAnim = 0;
                    break;
                case 1:
                    nAnim = 1;
                    break;
                default:
                    return;
            }
            pCoder->SetScoreLineAnim(nAnim);
        }
    }
}

void msw::CCockpitBasket::SetPlayerScore(long nPlayer, long nScore, int bAnim, int bBig)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetScore(nPlayer, nScore, bAnim, bBig);
        }
    }
}

void msw::CCockpitBasket::SetPlayerPointUp(long nPlayer, const bcs::FVEC3 *pPos, long nPoint)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0 && pPos != 0) {
            pHarapeko->SetPointUpText(nPlayer, *pPos, nPoint);
        }
    }
}

void msw::CCockpitBasket::ClearPlayerPointUp(long nPlayer)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->ClearPointUpText(nPlayer);
        }
    }
}

void msw::CCockpitBasket::SetPlayerCrownIcon(long nPlayer, long nArg, const bcs::FVEC3 *pPos, long nAction)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0 && pPos != 0) {
            pHarapeko->SetCrownIcon(nPlayer, *pPos, nArg, nAction);
        }
    }
}

void msw::CCockpitBasket::SetHarapekoBallScore(long nBall, long nScore)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetBallScore(nBall, nScore);
        }
    }
}

void msw::CCockpitBasket::SetHarapekoDispBallFruit4(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetDispBallFruit4(bDisp);
        }
    }
}

void msw::CCockpitBasket::SetHarapekoPlayerScoreDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetDispPlayerScore(bDisp);
        }
    }
}

void msw::CCockpitBasket::SetHarapekoTeamScoreDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketHarapeko *pHarapeko = (CCockpitBasketHarapeko *)pTask->FindObj(OBJ_HARAPEKO);
        if (pHarapeko != 0) {
            pHarapeko->SetDispTeamScore(bDisp);
        }
    }
}

void msw::CCockpitBasket::SetMissionCountDisp(long nIdx, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountDisp(bDisp);
        }
    }
}

void msw::CCockpitBasket::SetMissionCountNum(long nIdx, long nNum, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountNum(nNum, nMax);
        }
    }
}

void msw::CCockpitBasket::SetMissionCountUpNum(long nIdx, long nNum, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountUpNum(nNum, nMax);
        }
    }
}

void msw::CCockpitBasket::SetMissionCountActiveColor(int bActive)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetMissionCountActiveColor(bActive);
        }
    }
}

void msw::CCockpitBasket::SetIconFaceImpl()
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoder *pCoder = (CCockpitBasketCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetFaceIcon();
        }
    }
}

void msw::CCockpitBasket::SetChrIcon(long nObjId, long nChr, long nUnk, const bcs::FVEC3 *pPos)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0 && pPos != 0) {
        CCockpitChrIcon2D *pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
        if (pIcon != 0) {
            pIcon->ClearChrIcon(nChr, 1, nUnk);
            pIcon->SetChrIcon(nChr, *pPos, nUnk);
        }
    }
}

void msw::CCockpitBasket::ClearChrIcon(long nObjId, long nChr, long nUnk, int bUnk)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitChrIcon2D *pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
        if (pIcon != 0) {
            pIcon->ClearChrIcon(nChr, bUnk, nUnk);
        }
    }
}

void msw::CCockpitBasket::SetCoinTelopState(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBasketCoin *pCoin = (CCockpitBasketCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->TelopMove(nTeam, nState);
        }
    }
}

long msw::CCockpitBasket::GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    CCockpitBasketCoin *pCoin = (CCockpitBasketCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return -1;
    }
    return pCoin->GetTargetInfo(nTeam, rPos, rSize);
}

bcs::CTask *msw::CCockpitBasket::Get3DCoinTaskImpl(long nTeam)
{
    CCockpitBasketCoin *pCoin = (CCockpitBasketCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return 0;
    }
    return pCoin->Get3DCoin(CGame::GetTeamColorIdx(nTeam)).Get3DCoinTask();
}

void msw::CCockpitBasket::PartMoveCheck()
{
    static const long objIdTblLength = 3;
    CCockpitPartMove::PartMoveInfo info;

    while (CCockpitPartMove::s_Instance.Pop(info) >= 0) {
        long nPart = info.m_nPart;
        long nMove = info.m_nArg0;
        for (long i = 0; i < objIdTblLength; i++) {
            CCockpitBasketLayoutBase *pObj = (CCockpitBasketLayoutBase *)FindObj(objIdTbl[i]);
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
                case 2:
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
                case 3:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->TimeIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->TimeOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->TimeInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->TimeOutWait();
                            break;
                    }
                    break;
                case 4:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->PeriodIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->PeriodOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->PeriodInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->PeriodOutWait();
                            break;
                    }
                    break;
            }
        }
    }
}

void msw::CCockpitBasket::SetPartMove(long nPart, long nMove)
{
    CCockpitPartMove::s_Instance.Push(nPart, nMove, 0);
}

long msw::CCockpitBasket::CreateObj(long nObjId, long nParam)
{
    CCockpit *pCockpit = GetTask();

    if (pCockpit == 0) {
        return -2;
    }
    return pCockpit->CreateObjImpl(nObjId, nParam);
}

long msw::CCockpitBasket::CreateObjImpl(long nObjId, long nParam)
{
    CMenuLinkBase *pObj = 0;

    // The cases are in the order of the original's code.
    switch (nObjId) {
        case OBJ_CODER:
            pObj = new CCockpitBasketCoder(nObjId);
            break;
        case OBJ_HARAPEKO:
            pObj = new CCockpitBasketHarapeko(nObjId, 0.0f);
            break;
        case OBJ_COIN:
            pObj = new CCockpitBasketCoin(nObjId);
            break;
        case 3:
            pObj = new CSignBoard(nObjId, 2, 10.0f);
            break;
        case 4:
            pObj = new CSignBoard(nObjId, 3, 10.0f);
            break;
        case 5:
            pObj = new CSignBoard(nObjId, 4, 10.0f);
            break;
        case OBJ_RADAR_ICON:
            pObj = new CRadarIcon2DBasket(nObjId);
            break;
        case OBJ_RESULT_SCORE:
            pObj = new CCockpitBasketResult(nObjId, 0.0f);
            break;
        case OBJ_RESULT_SCORE_2:
            pObj = new CCockpitBasketResult(nObjId, 0.0f);
            break;
        case 26:
            pObj = new CCockpitChrIcon2DBasket(nObjId, 0.0f, 0, 0);
            break;
        case 27:
            pObj = new CCockpitChrIcon2DBasket(nObjId, 0.0f, 0, 1);
            break;
        case 28:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 0);
            break;
        case 29:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 1);
            break;
        case 10: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 8, 0.0f);
            pObj = pTelop;
            pTelop->SetAppendTableParam(0, CGame::g_GameWork.m_nPeriod);
            break;
        }
        case 11:
            pObj = new CCockpitTelopSwitchColor(nObjId, 0, 0.0f);
            break;
        case 12:
            pObj = new CCockpitTelopSwitchColor(nObjId, 3, 0.0f);
            break;
        case 13:
            pObj = new CCockpitTelopSwitchColor(nObjId, 4, 0.0f);
            break;
        case 14:
            pObj = new CCockpitTelopSwitchColor(nObjId, 5, 0.0f);
            break;
        case 15:
        case 16:
        case 17:
        case 18:
            pObj = CCockpitTelopStageIntro::CreateObj(OBJ_STAGE_INTRO, 0.0f);
            break;
        case OBJ_SCORE_COUNT_3D:
            if (FindObj(OBJ_SCORE_COUNT_3D) == 0) {
                pObj = new CCockpit3dScoreCount(nObjId, 0.0f);
            }
            break;
        case 19:
            pObj = new CCockpitBasketModelTelop(nObjId, 0, 0.0f);
            if (pObj != 0) {
                pObj->Set();
            }
            break;
        case 20:
            pObj = new CCockpitBasketModelTelop(nObjId, 1, 0.0f);
            if (pObj != 0) {
                pObj->Set();
            }
            break;
        case 21:
            pObj = new CCockpitBasketModelTelop(nObjId, 2, 0.0f);
            if (pObj != 0) {
                pObj->Set();
            }
            break;
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
        case 24: {
            CCockpitSignBoardScreen *pScreen = new CCockpitSignBoardScreen(nObjId, 0x1E, 0.0f);
            if (pScreen != 0) {
                pObj = pScreen;
            }
            break;
        }
        case 25:
            pObj = new CCockpitBasketBonusTime(nObjId, 0, 0.0f);
            if (pObj != 0) {
                pObj->Set();
            }
            break;
        default:
            pObj = CreateObjCommonImpl(nObjId, nParam);
            break;
    }
    return AddObj(pObj);
}

void msw::CCockpitBasket::SetMenuState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        pTask->ChangeState(nState);
    }
}

void msw::CCockpitBasket::Initialize_In()
{
    long nNextState;

    if (CGame::g_GameWork.GetGameType() == 2) {
        // The party game: only the hungry-ball board.
        nNextState = STATE_GAMEPLAY;
        CreateObj(OBJ_HARAPEKO, 0);
        CreateObj(0x1015, 0);
        Build(OBJ_HARAPEKO);
        Build(0x1015);
    } else {
        nNextState = STATE_GAMEPLAY;
        CreateObj(OBJ_CODER, 0);
        CreateObj(OBJ_SHOT_CLOCK, 0);
        CreateObj(0x1003, 0);
        CreateObj(OBJ_RADAR_ICON, 0);
        CreateObj(0x1001, 0);
        CreateObj(0x1004, 0);
        CreateObj(OBJ_COIN, 0);
        CreateObj(0x1006, 0);
        CreateObj(0x1007, 0);
        CreateObj(0x100A, 0);
        CreateObj(0x100B, 0);
        CreateObj(0x100C, 0);
        CreateObj(0x1A, 0);
        CreateObj(0x1B, 0);
        CreateObj(0x1C, 0);
        CreateObj(0x1D, 0);
        CreateObj(0x1015, 0);
        CreateObj(0x100F, 0);
        Build(OBJ_CODER);
        Build(OBJ_SHOT_CLOCK);
        Build(0x1003);
        Build(OBJ_RADAR_ICON);
        Build(0x1001);
        Build(0x1004);
        Build(OBJ_COIN);
        Build(0x1006);
        Build(0x1007);
        Build(0x100A);
        Build(0x100B);
        Build(0x100C);
        Build(0x1A);
        Build(0x1B);
        Build(0x1C);
        Build(0x1D);
        Build(0x1015);
        Build(0x100F);
    }
    if (nNextState >= 0) {
        ChangeState(nNextState);
    }
    CreateDemoTelop();
}

void msw::CCockpitBasket::Initialize_Loop()
{
}

void msw::CCockpitBasket::Initialize_Out()
{
}

void msw::CCockpitBasket::GameRestart_In()
{
    ChangeState(STATE_GAMEPLAY);
}

void msw::CCockpitBasket::GameRestart_Loop()
{
}

void msw::CCockpitBasket::GameRestart_Out()
{
}

void msw::CCockpitBasket::GamePlay_In()
{
}

void msw::CCockpitBasket::GamePlay_Loop()
{
}

void msw::CCockpitBasket::GamePlay_Out()
{
}

void msw::CCockpitBasket::GamePause_In()
{
    CCockpitBasketPause::Create(GetTask());
}

void msw::CCockpitBasket::GamePause_Loop()
{
}

void msw::CCockpitBasket::GamePause_Out()
{
    CCockpitPause::Remove();
}

void msw::CCockpitBasket::GameResult_In()
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

    // The period scores.
    nObjId = OBJ_RESULT_SCORE_2;
    if (m_nState == STATE_GAMERESULT) {
        nObjId = OBJ_RESULT_SCORE;
    }
    CreateObj(nObjId, 0);
    CCockpitBasketResult *pScore = (CCockpitBasketResult *)FindObj(nObjId);
    if (pScore != 0) {
        Build(nObjId);
        long nPeriodNum = m_nPeriodNum;
        long nPeriod = m_nPeriod;
        pScore->SetCountScore(m_aPeriodScore[0], m_aPeriodScore[1], nPeriod, nPeriodNum);
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

void msw::CCockpitBasket::GameResult_Loop()
{
    static const float timeCount = 959.0f;
    static const float FRAME_1SEC = 60.0f;
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
                cockpit::TelopTimeCount::SetParameter(OBJ_TIME_COUNT, (long)(fTime / FRAME_1SEC), 0);
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
            CMenuLinkBase *pScore = FindObj(nScoreId);
            CMenuLinkBase *pChr = FindObj(nChrId);
            CMenuLinkBase *pEnd = FindObj(OBJ_RESULT_END);
            if (pResult == 0 && pChr == 0 && pScore == 0 && pEnd == 0) {
                if (m_nState != STATE_GAMERESULT && CGame::g_GameWork.m_Unk151 != 0 &&
                    CGamePadCheck::GetInstance() != 0) {
                    CGamePadCheck::GetInstance()->m_UnkAC = 0;
                }
                ChangeState(STATE_GAMEPLAY);
            }
            break;
        }
    }
}

void msw::CCockpitBasket::GameResult_Out()
{
    RemoveObj(OBJ_RESULT);
    RemoveObj(OBJ_RESULT_SCORE);
    RemoveObj(OBJ_RESULT_CHR);
    RemoveObj(OBJ_RESULT_END);
    RemoveObj(OBJ_RESULT_2);
    RemoveObj(OBJ_RESULT_SCORE_2);
    RemoveObj(OBJ_RESULT_CHR_2);
}

void msw::CCockpitBasket::GamePartyResult_In()
{
    static const long titleStrLen = 128;
    static const long resultObjId = 0x1021;
    wchar_t aTitle[titleStrLen];

    CreateObj(resultObjId, 0);
    CCockpitResultPartyVS *pResult = (CCockpitResultPartyVS *)FindObj(resultObjId);
    if (pResult != 0) {
        Build(resultObjId);
        GetResultPartyVSParameter(pResult);
        pResult->SetSubTitleText(cockpit::GetScoreBoardTitleString(aTitle, titleStrLen));
        pResult->SetDelayResultButton();
    }
    m_Unk44 = 0;
}

void msw::CCockpitBasket::GamePartyResult_Loop()
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

void msw::CCockpitBasket::GamePartyResult_Out()
{
    static const long resultObjId = 0x1021;

    RemoveObj(resultObjId);
}
