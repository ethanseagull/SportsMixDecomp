// msw::CCockpitHockey: the hockey game's cockpit task. It creates the
// displays (score board, coins, rink map, the party and extra-match boards,
// telops, result pages...), forwards the game's calls to them and runs the
// result screens. The class frame is BasketF's
// game/basket/CCockpitBasket.cpp.
#include "game/msw/CCockpitHockey.hpp"

#include <string.h>
#include "bcs/FVEC3.hpp"
#include "game/msw/CBallMarker2DHockey.hpp"
#include "game/msw/CCockpit3dScoreCount.hpp"
#include "game/msw/CCockpitChrIcon2DHockey.hpp"
#include "game/msw/CCockpitCombiIcon2D.hpp"
#include "game/msw/CCockpitHockeyClash.hpp"
#include "game/msw/CCockpitHockeyCoder.hpp"
#include "game/msw/CCockpitHockeyCoin.hpp"
#include "game/msw/CCockpitHockeyIconFightGauge.hpp"
#include "game/msw/CCockpitHockeyMap.hpp"
#include "game/msw/CCockpitHockeyModelTelop.hpp"
#include "game/msw/CCockpitHockeyPause.hpp"
#include "game/msw/CCockpitHockeyResult.hpp"
#include "game/msw/CCockpitHockeySmashVS.hpp"
#include "game/msw/CCockpitHockeyTelopOut.hpp"
#include "game/msw/CCockpitHockeyTelopReady.hpp"
#include "game/msw/CCockpitHockeyVSCount.hpp"
#include "game/msw/CCockpitMenuBase.hpp"
#include "game/msw/CCockpitPartMove.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CCockpitResult.hpp"
#include "game/msw/CCockpitResultChr.hpp"
#include "game/msw/CCockpitResultPartyVS.hpp"
#include "game/msw/CCockpitSignBoardScreen.hpp"
#include "game/msw/CCockpitTelopShake.hpp"
#include "game/msw/CCockpitTelopStageIntro.hpp"
#include "game/msw/CCockpitTelopSwitchColor.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CRadarIcon2DHockey.hpp"
#include "game/msw/CRadarIcon2DHockeyGoal.hpp"
#include "game/msw/CSignBoard.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/cockpit/ResultUtil.hpp"
#include "game/msw/cockpit/TelopTimeCount.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/menu/MenuUtil.hpp"

namespace {
    // Events the cockpit sends to its display objects.
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Display objects. Below 41 CreateObjImpl makes them itself (and
    // 0x1072); the others come from CCockpit::CreateObjCommonImpl.
    enum {
        OBJ_CODER = 0,
        OBJ_CLASH = 1,              // the party game's board
        OBJ_COIN = 2,
        OBJ_SMASH_VS = 3,           // the extra match's board
        OBJ_BALL_MARKER = 4,        // and 5 for the second view
        OBJ_BALL_MARKER_2 = 5,
        OBJ_RADAR_ICON = 6,
        OBJ_GOAL_ICON = 7,
        OBJ_SCORE_COUNT_3D = 8,
        OBJ_MAP = 9,
        OBJ_FIGHT_GAUGE = 10,
        OBJ_RESULT_SCORE = 11,      // the result's period scores (state 4)
        OBJ_RESULT_SCORE_2 = 12,    // the same in state 5
        OBJ_STAGE_INTRO = 17,       // 17 to 20, one telop type each
        OBJ_DISP_NUM = 41,          // SetDisp walks ids 0 to 40
        OBJ_TELOP_SHAKE = 0x1072,
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
// (both SetSetCount, GameResult_In, GameResult_Loop, GamePartyResult_In) the
// bytes come out in the original order but the two `@LOCAL@` names are
// exchanged (see notes/decisions.md). SetSetCount declares TEAM_1 first so
// that its values come out as 1, 0 like the original's.

// The objects PartMoveCheck moves: the score board, the coins, the party
// board and the extra match's board.
static const long objIdTbl[] = { OBJ_CODER, OBJ_COIN, OBJ_CLASH, OBJ_SMASH_VS };

// The constructor and InitState are ordinary out-of-line functions,
// inlined into Create; the original module has no copy of either (its link
// dropped them as unreferenced). See game/tour/CCockpitTournament.cpp.
msw::CCockpitHockey::CCockpitHockey(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk44 = 0;
    InitState();
}

msw::CCockpitHockey::~CCockpitHockey()
{
    Finalize();
}

long msw::CCockpitHockey::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitHockey>::ChangeState(nState);
}

void msw::CCockpitHockey::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_INITIALIZE, &CCockpitHockey::Initialize_In, &CCockpitHockey::Initialize_Loop,
             &CCockpitHockey::Initialize_Out);
    SetState(STATE_GAMERESTART, &CCockpitHockey::GameRestart_In, &CCockpitHockey::GameRestart_Loop,
             &CCockpitHockey::GameRestart_Out);
    SetState(STATE_GAMEPLAY, &CCockpitHockey::GamePlay_In, &CCockpitHockey::GamePlay_Loop,
             &CCockpitHockey::GamePlay_Out);
    SetState(STATE_GAMEPAUSE, &CCockpitHockey::GamePause_In, &CCockpitHockey::GamePause_Loop,
             &CCockpitHockey::GamePause_Out);
    SetState(STATE_GAMERESULT, &CCockpitHockey::GameResult_In, &CCockpitHockey::GameResult_Loop,
             &CCockpitHockey::GameResult_Out);
    SetState(STATE_GAMERESULT_2, &CCockpitHockey::GameResult_In, &CCockpitHockey::GameResult_Loop,
             &CCockpitHockey::GameResult_Out);
    SetState(STATE_GAMEPARTYRESULT, &CCockpitHockey::GamePartyResult_In, &CCockpitHockey::GamePartyResult_Loop,
             &CCockpitHockey::GamePartyResult_Out);
    ChangeState(STATE_INITIALIZE);
}

void msw::CCockpitHockey::Finalize()
{
    RemoveObj(OBJ_CODER);
    RemoveObj(OBJ_SHOT_CLOCK);
    RemoveObj(OBJ_RADAR_ICON);
    RemoveObj(OBJ_GOAL_ICON);
    RemoveObj(0x1001);
    RemoveObj(0x1004);
    RemoveObj(OBJ_COIN);
    RemoveObj(OBJ_MAP);
    RemoveObj(0x1006);
    RemoveObj(0x1007);
    RemoveObj(0x100A);
    RemoveObj(0x100B);
    RemoveObj(0x100C);
    RemoveObj(0x23);
    RemoveObj(0x24);
    RemoveObj(0x25);
    RemoveObj(0x26);
    RemoveObj(0x27);
    RemoveObj(0x28);
    RemoveObj(OBJ_FIGHT_GAUGE);
    RemoveObj(0x1015);
    RemoveObj(0x100F);
    RemoveObj(OBJ_CLASH);
    RemoveObj(OBJ_SMASH_VS);
    RemoveDemoTelop();
}

void msw::CCockpitHockey::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
    PartMoveCheck();
}

void msw::CCockpitHockey::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitHockey::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitHockey(pParent);
    }
}

void msw::CCockpitHockey::SetDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        for (long i = 0; i < OBJ_DISP_NUM; i++) {
            pTask->CMenuTask::SendObjEventTrigger(i, EVENT_SET_DISP, (void *)bDisp);
        }
        pTask->CCockpit::SetDisp(bDisp);
    }
}

void msw::CCockpitHockey::SetCoinDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoin *pCoin = (CCockpitHockeyCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->SetCoinDisp(bEnable);
        }
    }
}

void msw::CCockpitHockey::SetScoreDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetScoreDisp(bEnable);
        }
    }
}

void msw::CCockpitHockey::SetTimeDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTimeDisp(bEnable);
        }
    }
}

void msw::CCockpitHockey::SetPeriodDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetPeriodDisp(bEnable);
        }
    }
}

void msw::CCockpitHockey::SetBallMarkerDispEnable(int bEnable)
{
    // The markers take the event number itself: 1 shows, 0 hides.
    long nEvent = bEnable == 1;

    SendObjEventTrigger(OBJ_BALL_MARKER, nEvent, 0);
    SendObjEventTrigger(OBJ_BALL_MARKER_2, nEvent, 0);
}

void msw::CCockpitHockey::SetSPGaugeDispEnableImpl(int bEnable)
{
    CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetSPGaugeDisp(bEnable);
    }
}

void msw::CCockpitHockey::SetTime(float fTime, int nArg)
{
    static const int force = 1;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTimeValue(fTime, nArg, force);
            return;
        }
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0) {
            pClash->SetTimeValue(fTime, nArg, force);
            return;
        }
        CCockpitHockeySmashVS *pSmashVS = (CCockpitHockeySmashVS *)pTask->FindObj(OBJ_SMASH_VS);
        if (pSmashVS != 0) {
            pSmashVS->SetTimeValue(fTime, nArg, force);
        }
    }
}

void msw::CCockpitHockey::SetRadarIconDisp(int bDisp)
{
    SendObjEventTrigger(OBJ_RADAR_ICON, 0, (void *)bDisp);
    SendObjEventTrigger(OBJ_GOAL_ICON, 0, (void *)bDisp);
}

void msw::CCockpitHockey::SetGoalIconDisp(long nGoal, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CRadarIcon2DHockeyGoal *pIcon = (CRadarIcon2DHockeyGoal *)pTask->FindObj(OBJ_GOAL_ICON);
        if (pIcon != 0) {
            pIcon->SetDispGoalIcon(nGoal, bDisp);
        }
    }
}

void msw::CCockpitHockey::SetTeamScore()
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetScoreValue(0, CGame::GetTeamScore(CGame::GetTeamColorIdx(0)));
            pCoder->SetScoreValue(1, CGame::GetTeamScore(CGame::GetTeamColorIdx(1)));
        }
    }
}

void msw::CCockpitHockey::SetPeriod(long nPeriod)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetPeriodValue(nPeriod);
        }
    }
}

// Not in the module: nothing in HockeyF calls it, so the original link
// dropped it and kept only its two constants (BasketF's and DodgeF's
// cockpits have the same pair). The body is a stand-in that uses them; what
// the original did is not known.
void msw::CCockpitHockey::SetSetCount()
{
    static const long TEAM_1 = 1;
    static const long TEAM_0 = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitHockeySmashVS *pSmashVS = (CCockpitHockeySmashVS *)pTask->FindObj(OBJ_SMASH_VS);
        if (pSmashVS != 0) {
            pSmashVS->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_0), 0);
            pSmashVS->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_1), 0);
        }
    }
}

void msw::CCockpitHockey::SetSetCount(long nPoint0, long nPoint1)
{
    static const long TEAM_1 = 1;
    static const long TEAM_0 = 0;

    CCockpit *pTask = GetTask();
    if (pTask != 0) {
        CCockpitHockeySmashVS *pSmashVS = (CCockpitHockeySmashVS *)pTask->FindObj(OBJ_SMASH_VS);
        if (pSmashVS != 0) {
            pSmashVS->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_0), nPoint0);
            pSmashVS->SetSetCountPoint(CGame::GetTeamColorIdx(TEAM_1), nPoint1);
        }
    }
}

void msw::CCockpitHockey::SetTeamScoreCoder(long nTeam, long nScore)
{
    CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreValue(CGame::GetTeamColorIdx(nTeam), nScore);
    }
}

void msw::CCockpitHockey::AddTeamScoreCoder(long nTeam, long nScore)
{
    CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreValue(CGame::GetTeamColorIdx(nTeam), nScore);
    }
}

void msw::CCockpitHockey::SetTeamScoreColorCoder(long nTeam, int nFont)
{
    CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetScoreFont(CGame::GetTeamColorIdx(nTeam), nFont);
    }
}

void msw::CCockpitHockey::SetTeamCoinCoder(long nTeam, long nCoin)
{
    CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitHockeyCoin *pCoin = (CCockpitHockeyCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
}

void msw::CCockpitHockey::AddTeamCoinCoder(long nTeam, long nCoin, long nAdd)
{
    CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitHockeyCoin *pCoin = (CCockpitHockeyCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->AddCoinValue(CGame::GetTeamColorIdx(nTeam), nAdd, nCoin);
    }
}

void msw::CCockpitHockey::SetCoinSpecialAnim(long nTeam, int bSpecial)
{
    long nState;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            nState = 5;
            if (bSpecial == 1) {
                nState = 6;
            }
            pCoder->SetCoinMaxState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitHockey::SetSPGaugeRatio(long nTeam, float fRatio, float fTelop)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPGauge(CGame::GetTeamColorIdx(nTeam), fRatio);
            if (fTelop > 0.0f) {
                pCoder->SetSPGaugeTelop(CGame::GetTeamColorIdx(nTeam), fTelop);
            }
        }
    }
}

void msw::CCockpitHockey::SetSPContrllerDisp(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPControllerState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitHockey::SetTelop3DScoreCount(long nArg0, long nArg1, long nArg2, long nScore0, long nScore1,
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

void msw::CCockpitHockey::SetArrowState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
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

void msw::CCockpitHockey::SetScoreArrowState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
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

void msw::CCockpitHockey::SetPlayerScore(long nPlayer, long nScore, int bAnim, int bBig)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0) {
            pClash->SetScore(nPlayer, nScore, bAnim, bBig);
        }
    }
}

void msw::CCockpitHockey::SetPlayerPointUp(long nPlayer, const bcs::FVEC3 *pPos, long nPoint)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0 && pPos != 0) {
            pClash->SetPointUpText(nPlayer, *pPos, nPoint);
        }
    }
}

void msw::CCockpitHockey::ClearPlayerPointUp(long nPlayer)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0) {
            pClash->ClearPointUpText(nPlayer);
        }
    }
}

void msw::CCockpitHockey::SetPlayerLeaveCount(long nPlayer, long nCount, const bcs::FVEC3 *pPos, int bArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0 && pPos != 0) {
            pClash->SetLeaveCountText(nPlayer, *pPos, nCount, bArg);
        }
    }
}

void msw::CCockpitHockey::SetPlayerShakeIcon(long nPlayer, long nArg, const bcs::FVEC3 *pPos, int bArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0 && pPos != 0) {
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0) {
            pClash->SetRimoconShakeIcon(nPlayer, *pPos, nArg, bArg);
        }
        CCockpitHockeySmashVS *pSmashVS = (CCockpitHockeySmashVS *)pTask->FindObj(OBJ_SMASH_VS);
        if (pSmashVS != 0) {
            pSmashVS->SetRimoconShakeIcon(nPlayer, *pPos, nArg, bArg);
        }
    }
}

void msw::CCockpitHockey::SetPlayerCrownIcon(long nPlayer, long nArg, const bcs::FVEC3 *pPos, long nAction)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyClash *pClash = (CCockpitHockeyClash *)pTask->FindObj(OBJ_CLASH);
        if (pClash != 0 && pPos != 0) {
            pClash->SetCrownIcon(nPlayer, *pPos, nArg, nAction);
        }
    }
}

void msw::CCockpitHockey::SetMissionCountDisp(long nIdx, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountDisp(bDisp);
        }
    }
}

void msw::CCockpitHockey::SetMissionCountNum(long nIdx, long nCount, long nTarget)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountNum(nCount, nTarget);
        }
    }
}

void msw::CCockpitHockey::SetMissionCountUpNum(long nIdx, long nNum, long nMax)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0 && nIdx == 0) {
            pCoder->SetMissionCountUpNum(nNum, nMax);
        }
    }
}

void msw::CCockpitHockey::SetMissionCountActiveColor(int bActive)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetMissionCountActiveColor(bActive);
        }
    }
}

void msw::CCockpitHockey::SetMapDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetDispEnable(bEnable);
        }
    }
}

void msw::CCockpitHockey::SetMapCamera(float fPos)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetCameraPosition(fPos);
        }
    }
}

void msw::CCockpitHockey::SetMapPlayerPosition(long nPlayer, float fX, float fY, float fZ, long nArg4, long nArg5)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetPlayerPosition(nPlayer, fX, fY, fZ, nArg4, nArg5);
        }
    }
}

void msw::CCockpitHockey::SetMapPlayerDisp(long nPlayer, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetPlayerDisp(nPlayer, bDisp);
        }
    }
}

void msw::CCockpitHockey::SetMapBallDisp(long nBall, int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetBallDisp(nBall, bDisp);
        }
    }
}

void msw::CCockpitHockey::SetMapDirection(int nDirection)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetDirection(nDirection);
        }
    }
}

void msw::CCockpitHockey::SetMapGoalTarget(long nTarget)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyMap *pMap = (CCockpitHockeyMap *)pTask->FindObj(OBJ_MAP);
        if (pMap != 0) {
            pMap->SetGoalTarget(nTarget);
        }
    }
}

void msw::CCockpitHockey::SetChrIcon(long nObjId, long nChr, long nUnk, const bcs::FVEC3 *pPos)
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

void msw::CCockpitHockey::ClearChrIcon(long nObjId, long nChr, long nUnk, int bUnk)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitChrIcon2D *pIcon = (CCockpitChrIcon2D *)pTask->FindObj(nObjId);
        if (pIcon != 0) {
            pIcon->ClearChrIcon(nChr, bUnk, nUnk);
        }
    }
}

void msw::CCockpitHockey::SetFightGaugeIcon(long nTeam, const bcs::FVEC3 *pPos)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyIconFightGauge *pGauge = (CCockpitHockeyIconFightGauge *)pTask->FindObj(OBJ_FIGHT_GAUGE);
        if (pGauge != 0 && pPos != 0) {
            pGauge->SetIcon(nTeam, *pPos);
        }
    }
}

void msw::CCockpitHockey::SetFightGaugeRatio(long nTeam, float fRatio)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyIconFightGauge *pGauge = (CCockpitHockeyIconFightGauge *)pTask->FindObj(OBJ_FIGHT_GAUGE);
        if (pGauge != 0) {
            pGauge->SetGaugeBar(nTeam, fRatio);
        }
    }
}

void msw::CCockpitHockey::ClearFightGaugeIcon(long nTeam, int bArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyIconFightGauge *pGauge = (CCockpitHockeyIconFightGauge *)pTask->FindObj(OBJ_FIGHT_GAUGE);
        if (pGauge != 0) {
            pGauge->ClearIcon(nTeam, bArg);
        }
    }
}

void msw::CCockpitHockey::SetIconFaceImpl()
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoder *pCoder = (CCockpitHockeyCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetFaceIcon();
        }
    }
}

void msw::CCockpitHockey::SetIconFaceByTurnNo(long nTurn0, long nTurn1, long nArg)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeySmashVS *pSmashVS = (CCockpitHockeySmashVS *)pTask->FindObj(OBJ_SMASH_VS);
        if (pSmashVS != 0) {
            pSmashVS->SetFaceIcon(0, nTurn0, nArg);
            pSmashVS->SetFaceIcon(1, nTurn1, nArg);
        }
    }
}

void msw::CCockpitHockey::SetCoinTelopState(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitHockeyCoin *pCoin = (CCockpitHockeyCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->TelopMove(nTeam, nState);
        }
    }
}

long msw::CCockpitHockey::GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    CCockpitHockeyCoin *pCoin = (CCockpitHockeyCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return -1;
    }
    return pCoin->GetTargetInfo(nTeam, rPos, rSize);
}

bcs::CTask *msw::CCockpitHockey::Get3DCoinTaskImpl(long nTeam)
{
    CCockpitHockeyCoin *pCoin = (CCockpitHockeyCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return 0;
    }
    return pCoin->Get3DCoin(CGame::GetTeamColorIdx(nTeam)).Get3DCoinTask();
}

void msw::CCockpitHockey::PartMoveCheck()
{
    static const long objIdTblLength = 4;
    CCockpitPartMove::PartMoveInfo info;
    const long *pObjIdTbl = objIdTbl;
    CCockpitPartMove &rPartMove = CCockpitPartMove::s_Instance;

    while (rPartMove.Pop(info) >= 0) {
        long nPart = info.m_nPart;
        long nMove = info.m_nArg0;
        for (long i = 0; i < objIdTblLength; i++) {
            CCockpitHockeyLayoutBase *pObj = (CCockpitHockeyLayoutBase *)FindObj(pObjIdTbl[i]);
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

void msw::CCockpitHockey::SetPartMove(long nPart, long nMove)
{
    CCockpitPartMove::s_Instance.Push(nPart, nMove, 0);
}

long msw::CCockpitHockey::CreateObj(long nObjId, long nParam)
{
    CCockpit *pCockpit = GetTask();

    if (pCockpit == 0) {
        return -2;
    }
    return pCockpit->CreateObjImpl(nObjId, nParam);
}


long msw::CCockpitHockey::CreateObjImpl(long nObjId, long nParam)
{
    CMenuLinkBase *pObj = 0;

    // The cases are in the order of the original's code.
    switch (nObjId) {
        case OBJ_CODER:
            pObj = new CCockpitHockeyCoder(nObjId);
            break;
        case OBJ_CLASH:
            pObj = new CCockpitHockeyClash(nObjId, 0.0f);
            break;
        case OBJ_COIN:
            pObj = new CCockpitHockeyCoin(nObjId);
            break;
        case OBJ_MAP:
            pObj = new CCockpitHockeyMap(nObjId);
            break;
        case OBJ_SMASH_VS:
            pObj = new CCockpitHockeySmashVS(nObjId);
            break;
        case OBJ_BALL_MARKER:
        case OBJ_BALL_MARKER_2:
            pObj = new CBallMarker2DHockey(nObjId);
            break;
        case OBJ_RADAR_ICON:
            pObj = new CRadarIcon2DHockey(nObjId);
            break;
        case OBJ_GOAL_ICON:
            pObj = new CRadarIcon2DHockeyGoal(nObjId);
            break;
        case OBJ_FIGHT_GAUGE:
            pObj = new CCockpitHockeyIconFightGauge(nObjId, 0.0f, 0);
            break;
        case OBJ_RESULT_SCORE:
            pObj = new CCockpitHockeyResult(nObjId, 0.0f);
            break;
        case OBJ_RESULT_SCORE_2:
            pObj = new CCockpitHockeyResult(nObjId, 0.0f);
            break;
        case 0x23:
            pObj = new CCockpitChrIcon2DHockey(nObjId, 0.0f, 0, 1);
            break;
        case 0x24:
            pObj = new CCockpitChrIcon2DHockey(nObjId, 0.0f, 0, 0);
            break;
        case 0x25:
            pObj = new CCockpitChrIcon2DHockey(nObjId, 0.0f, 0, 2);
            break;
        case 0x26:
            pObj = new CCockpitChrIcon2DHockey(nObjId, 0.0f, 0, 3);
            break;
        case 0x27:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 0);
            break;
        case 0x28:
            pObj = new CCockpitCombiIcon2D(nObjId, 0.0f, 0, 1);
            break;
        case 13: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 9, 0.0f);
            pTelop->SetAppendTableParam(0, CGame::g_GameWork.GetPeriod());
            if (pTelop != 0) {
                pObj = pTelop;
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
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 3, 0.0f);
            if (pTelop != 0) {
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
        case 21: {
            CCockpitHockeyModelTelop *pTelop = new CCockpitHockeyModelTelop(nObjId, 0, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 22:
            // Nothing is made for this id.
            break;
        case 23: {
            CCockpitHockeyModelTelop *pTelop = new CCockpitHockeyModelTelop(nObjId, 1, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case OBJ_TELOP_SHAKE:
            pObj = new CCockpitTelopShake(nObjId, 0, 3, 0.0f);
            break;
        case OBJ_SCORE_COUNT_3D:
            if (FindObj(OBJ_SCORE_COUNT_3D) == 0) {
                pObj = new CCockpit3dScoreCount(nObjId, 0.0f);
            }
            break;
        case 24: {
            CCockpitHockeyModelTelop *pTelop = new CCockpitHockeyModelTelop(nObjId, 2, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 25: {
            CCockpitHockeyModelTelop *pTelop = new CCockpitHockeyModelTelop(nObjId, 3, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 26: {
            CCockpitHockeyModelTelop *pTelop = new CCockpitHockeyModelTelop(nObjId, 4, 0.0f);
            if (pTelop != 0) {
                pTelop->Set();
            }
            pObj = pTelop;
            break;
        }
        case 27: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xB, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 28: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xA, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 29: {
            CCockpitSignBoardScreen *pScreen = new CCockpitSignBoardScreen(nObjId, 0x1E, 0.0f);
            if (pScreen != 0) {
                pObj = pScreen;
            }
            break;
        }
        case 30:
            pObj = new CSignBoard(nObjId, 0x2B, 0.0f);
            break;
        case 31:
            pObj = new CCockpitHockeyTelopOut(nObjId, 0, 0.0f);
            break;
        case 32:
            pObj = new CCockpitHockeyTelopReady(nObjId, 0, 0.0f);
            break;
        case 33:
            pObj = new CCockpitHockeyTelopReady(nObjId, 1, 0.0f);
            break;
        case 34:
            pObj = new CCockpitHockeyVSCount(nObjId, 0.0f);
            break;
        default:
            pObj = CreateObjCommonImpl(nObjId, nParam);
            break;
    }
    return AddObj(pObj);
}

void msw::CCockpitHockey::SetMenuState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        pTask->ChangeState(nState);
    }
}


void msw::CCockpitHockey::Initialize_In()
{
    long nNextState;

    if (CGame::g_GameWork.GetGameType() == 2) {
        // The party game: its board (the extra match has its own) only.
        nNextState = STATE_GAMEPLAY;
        if (CTournament::IsExtraMatchGame()) {
            CreateObj(OBJ_SMASH_VS, 0);
            Build(OBJ_SMASH_VS);
        } else {
            CreateObj(OBJ_CLASH, 0);
            Build(OBJ_CLASH);
        }
        CreateObj(0x1015, 0);
        Build(0x1015);
    } else {
        // (The shot clock, the goal icons, the map and object 0x24 are
        // built but never created here.)
        nNextState = STATE_GAMEPLAY;
        CreateObj(OBJ_CODER, 0);
        CreateObj(OBJ_RADAR_ICON, 0);
        CreateObj(0x1001, 0);
        CreateObj(0x1004, 0);
        CreateObj(OBJ_COIN, 0);
        CreateObj(0x1006, 0);
        CreateObj(0x1007, 0);
        CreateObj(0x100A, 0);
        CreateObj(0x100B, 0);
        CreateObj(0x100C, 0);
        CreateObj(0x23, 0);
        CreateObj(0x25, 0);
        CreateObj(0x26, 0);
        CreateObj(0x27, 0);
        CreateObj(0x28, 0);
        CreateObj(OBJ_FIGHT_GAUGE, 0);
        CreateObj(0x1015, 0);
        CreateObj(0x100F, 0);
        Build(OBJ_CODER);
        Build(OBJ_SHOT_CLOCK);
        Build(OBJ_RADAR_ICON);
        Build(OBJ_GOAL_ICON);
        Build(0x1001);
        Build(0x1004);
        Build(OBJ_COIN);
        Build(OBJ_MAP);
        Build(0x1006);
        Build(0x1007);
        Build(0x100A);
        Build(0x100B);
        Build(0x100C);
        Build(0x23);
        Build(0x24);
        Build(0x25);
        Build(0x26);
        Build(0x27);
        Build(0x28);
        Build(OBJ_FIGHT_GAUGE);
        Build(0x1015);
        Build(0x100F);
    }
    if (nNextState >= 0) {
        ChangeState(nNextState);
    }
    CreateDemoTelop();
}

void msw::CCockpitHockey::Initialize_Loop()
{
}

void msw::CCockpitHockey::Initialize_Out()
{
}

void msw::CCockpitHockey::GameRestart_In()
{
    long nNextState = -1;

    switch (CGame::g_GameWork.GetGameType()) {
        case 0:
        case 2:
            nNextState = STATE_GAMEPLAY;
            break;
    }
    if (nNextState >= 0) {
        ChangeState(nNextState);
    }
}

void msw::CCockpitHockey::GameRestart_Loop()
{
}

void msw::CCockpitHockey::GameRestart_Out()
{
}

void msw::CCockpitHockey::GamePlay_In()
{
}

void msw::CCockpitHockey::GamePlay_Loop()
{
}

void msw::CCockpitHockey::GamePlay_Out()
{
}

void msw::CCockpitHockey::GamePause_In()
{
    CCockpitHockeyPause::Create(GetTask());
}

void msw::CCockpitHockey::GamePause_Loop()
{
}

void msw::CCockpitHockey::GamePause_Out()
{
    CCockpitPause::Remove();
}

void msw::CCockpitHockey::GameResult_In()
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
    CCockpitHockeyResult *pScore = (CCockpitHockeyResult *)FindObj(nObjId);
    if (pScore != 0) {
        Build(nObjId);
        long nPeriodNum = m_nPeriodNum;
        nPeriod = m_nPeriod;
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

void msw::CCockpitHockey::GameResult_Loop()
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

void msw::CCockpitHockey::GameResult_Out()
{
    RemoveObj(OBJ_RESULT);
    RemoveObj(OBJ_RESULT_SCORE);
    RemoveObj(OBJ_RESULT_CHR);
    RemoveObj(OBJ_RESULT_END);
    RemoveObj(OBJ_RESULT_2);
    RemoveObj(OBJ_RESULT_SCORE_2);
    RemoveObj(OBJ_RESULT_CHR_2);
}

void msw::CCockpitHockey::GamePartyResult_In()
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

void msw::CCockpitHockey::GamePartyResult_Loop()
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

void msw::CCockpitHockey::GamePartyResult_Out()
{
    static const long resultObjId = 0x1021;

    RemoveObj(resultObjId);
}
