// msw::CCockpitBoss: the boss game's cockpit task. It creates the displays
// (the coder with the life and boss gauges, coins, ball power, telops) and
// forwards the game's calls to them. The class frame is BasketF's
// game/basket/CCockpitBasket.cpp.
#include "game/msw/CCockpitBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpitBossBallPower.hpp"
#include "game/msw/CCockpitBossCoder.hpp"
#include "game/msw/CCockpitBossCoin.hpp"
#include "game/msw/CCockpitBossLayoutBase.hpp"
#include "game/msw/CCockpitBossPause.hpp"
#include "game/msw/CCockpitPartMove.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CCockpitTelopMiniPos.hpp"
#include "game/msw/CCockpitTelopSwitchColor.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CRadarIcon2DBoss.hpp"

namespace {
    // Events the cockpit sends to its display objects.
    enum {
        EVENT_SET_DISP = 10001 // pParam: nonzero to show
    };

    // Display objects. Below 10 CreateObjImpl makes them itself; the
    // others come from CCockpit::CreateObjCommonImpl.
    enum {
        OBJ_CODER = 0,
        OBJ_COIN = 1,
        OBJ_RADAR_ICON = 2,
        OBJ_BALL_POWER = 3,
        OBJ_MINI_TELOP = 9,
        OBJ_DISP_NUM = 10 // SetDisp walks ids 0 to 9
    };

    // What CCockpitPartMove entries ask of a part.
    enum {
        PART_MOVE_IN = 0,
        PART_MOVE_IN_WAIT = 1,
        PART_MOVE_OUT = 2,
        PART_MOVE_OUT_WAIT = 3
    };
}

// The objects PartMoveCheck moves: the coder and the coins. (Its length
// is not a static constant here: the module has no copy of one.)
static const long objIdTbl[] = { OBJ_CODER, OBJ_COIN };
enum { OBJ_ID_TBL_LENGTH = 2 };

// The constructor and InitState are ordinary out-of-line functions,
// inlined into Create; the original module has no copy of either (its link
// dropped them as unreferenced). See game/tour/CCockpitTournament.cpp.
msw::CCockpitBoss::CCockpitBoss(bcs::CTask *pParent)
    : CCockpit(pParent)
{
    m_Unk44 = 0;
    InitState();
}

msw::CCockpitBoss::~CCockpitBoss()
{
    RemoveObj(OBJ_CODER);
    RemoveObj(OBJ_RADAR_ICON);
    RemoveObj(0x1004);
    RemoveObj(OBJ_COIN);
    RemoveObj(OBJ_BALL_POWER);
    RemoveObj(0x1011);
    RemoveObj(0x1011);
    RemoveObj(0x1001);
    RemoveDemoTelop();
}

long msw::CCockpitBoss::ChangeState(long nState)
{
    return bcs::CFsm<CCockpitBoss>::ChangeState(nState);
}

void msw::CCockpitBoss::InitState()
{
    InitFsm(this, STATE_NUM);
    SetState(STATE_INITIALIZE, &CCockpitBoss::Initialize_In, &CCockpitBoss::Initialize_Loop,
             &CCockpitBoss::Initialize_Out);
    SetState(STATE_GAMERESTART, &CCockpitBoss::GameRestart_In, &CCockpitBoss::GameRestart_Loop,
             &CCockpitBoss::GameRestart_Out);
    SetState(STATE_GAMEPLAY, &CCockpitBoss::GamePlay_In, &CCockpitBoss::GamePlay_Loop,
             &CCockpitBoss::GamePlay_Out);
    SetState(STATE_GAMEPAUSE, &CCockpitBoss::GamePause_In, &CCockpitBoss::GamePause_Loop,
             &CCockpitBoss::GamePause_Out);
    SetState(STATE_GAMERESULT, &CCockpitBoss::GameResult_In, &CCockpitBoss::GameResult_Loop,
             &CCockpitBoss::GameResult_Out);
    SetState(STATE_GAMERESULT_2, &CCockpitBoss::GameResult_In, &CCockpitBoss::GameResult_Loop,
             &CCockpitBoss::GameResult_Out);
    ChangeState(STATE_INITIALIZE);
}

void msw::CCockpitBoss::Main()
{
    MainLoopCommon();
    ExecState();
    ModListMainLoop();
    PartMoveCheck();
}

void msw::CCockpitBoss::Disp()
{
    ModListDispLoop();
}

void msw::CCockpitBoss::Create(bcs::CTask *pParent)
{
    if (s_TaskHandle == 0) {
        new CCockpitBoss(pParent);
    }
}

void msw::CCockpitBoss::SetDisp(int bDisp)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        for (long i = 0; i < OBJ_DISP_NUM; i++) {
            pTask->CMenuTask::SendObjEventTrigger(i, EVENT_SET_DISP, (void *)bDisp);
        }
        pTask->CCockpit::SetDisp(bDisp);
    }
}

void msw::CCockpitBoss::SetCoinDispEnable(int bEnable)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoin *pCoin = (CCockpitBossCoin *)pTask->FindObj(OBJ_COIN);
        if (pCoin != 0) {
            pCoin->SetCoinDisp(bEnable);
        }
    }
}

void msw::CCockpitBoss::SetSPGaugeDispEnableImpl(int bEnable)
{
    CCockpitBossCoder *pCoder = (CCockpitBossCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetSPGaugeDisp(bEnable);
    }
}

void msw::CCockpitBoss::SetRadarIconDisp(int bDisp)
{
    SendObjEventTrigger(OBJ_RADAR_ICON, 0, (void *)bDisp);
}

void msw::CCockpitBoss::SetTeamCoinCoder(long nTeam, long nCoin)
{
    CCockpitBossCoder *pCoder = (CCockpitBossCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitBossCoin *pCoin = (CCockpitBossCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
}

void msw::CCockpitBoss::AddTeamCoinCoder(long nTeam, long nCoin, long nAdd)
{
    CCockpitBossCoder *pCoder = (CCockpitBossCoder *)FindObj(OBJ_CODER);
    if (pCoder != 0) {
        pCoder->SetCoinValue(CGame::GetTeamColorIdx(nTeam), nCoin);
    }
    CCockpitBossCoin *pCoin = (CCockpitBossCoin *)FindObj(OBJ_COIN);
    if (pCoin != 0) {
        pCoin->AddCoinValue(CGame::GetTeamColorIdx(nTeam), nAdd, nCoin);
    }
}

void msw::CCockpitBoss::SetCoinSpecialAnim(long nTeam, int bSpecial)
{
    long nState;
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            nState = 5;
            if (bSpecial == 1) {
                nState = 6;
            }
            pCoder->SetCoinMaxState(nState);
        }
    }
}

void msw::CCockpitBoss::SetSPGaugeRatio(long nTeam, float fRatio, float fTelop)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPGauge(fRatio);
        }
    }
}

void msw::CCockpitBoss::SetSPContrllerDisp(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetSPControllerState(CGame::GetTeamColorIdx(nTeam), nState);
        }
    }
}

void msw::CCockpitBoss::SetLifeGaugeLayout(long nLayout)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetLifeGaugeLayout(nLayout);
        }
    }
}

void msw::CCockpitBoss::SetLifeGaugeState(long nPlayer, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetLifeGaugeAnim(nPlayer, nState);
        }
    }
}

void msw::CCockpitBoss::SetLifePercent(long nPlayer, float fLife, float fLife2)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetLifeValue(nPlayer, fLife, fLife2);
        }
    }
}

void msw::CCockpitBoss::SetBossLifePercent(float fRate)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetBossLifePercent(fRate);
        }
    }
}

void msw::CCockpitBoss::SetBossIcon(long nIcon)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetBossIcon(nIcon);
        }
    }
}

void msw::CCockpitBoss::SetIconFaceImpl()
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetFaceIcon();
        }
    }
}

void msw::CCockpitBoss::SetBallPowerValue(long nTeam, short nValue)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossBallPower *pPower = (CCockpitBossBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerValue(nTeam, nValue);
        }
    }
}

void msw::CCockpitBoss::SetBallPowerValueColor(long nTeam, int bColor)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossBallPower *pPower = (CCockpitBossBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerValueColor(nTeam, bColor);
        }
    }
}

void msw::CCockpitBoss::SetBallPowerState(long nTeam, long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        if (nState != 0) {
            CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
            if (pCoder != 0) {
                pCoder->SetCoinState(nTeam, 1);
            }
        }
        CCockpitBossBallPower *pPower = (CCockpitBossBallPower *)pTask->FindObj(OBJ_BALL_POWER);
        if (pPower != 0) {
            pPower->SetBallPowerState(nTeam, nState);
        }
    }
}

void msw::CCockpitBoss::SetPowerUpTelopState(long nTeam, long nPlayer)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetTelopPowerUp(nTeam, nPlayer);
        }
    }
}

void msw::CCockpitBoss::SetOutTelopState(long nPlayer, int bOut)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        CCockpitBossCoder *pCoder = (CCockpitBossCoder *)pTask->FindObj(OBJ_CODER);
        if (pCoder != 0) {
            pCoder->SetOutTelop(nPlayer, bOut);
        }
    }
}

long msw::CCockpitBoss::GetCoinEffectTarget(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize)
{
    CCockpitBossBallPower *pPower = (CCockpitBossBallPower *)FindObj(OBJ_BALL_POWER);
    if (pPower == 0) {
        return -1;
    }
    return pPower->GetTargetInfo(nTeam, rPos, rSize);
}

bcs::CTask *msw::CCockpitBoss::Get3DCoinTaskImpl(long nTeam)
{
    CCockpitBossCoin *pCoin = (CCockpitBossCoin *)FindObj(OBJ_COIN);
    if (pCoin == 0) {
        return 0;
    }
    return pCoin->Get3DCoin(CGame::GetTeamColorIdx(nTeam)).Get3DCoinTask();
}

long msw::CCockpitBoss::CreateMiniTelop(long nObjId, long nChr, bcs::FVEC3 *pOffset)
{
    if (nObjId != OBJ_MINI_TELOP) {
        return -13;
    }
    long nChrNum = CChrMan::GetChrCoreNum();
    if (nChr < 0 || nChr >= nChrNum) {
        return -11;
    }
    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0 || pChr->IsDisable()) {
        return -12;
    }
    bcs::FVEC3 pos = pChr->GetPos();
    if (pOffset != 0) {
        bcs::FVEC3Add(&pos, &pos, pOffset);
    }
    return CreateTelopMiniPos(nObjId, 0, pos);
}

void msw::CCockpitBoss::SetItemRouletteStateImpl(long nArg0, long nArg1, long nArg2)
{
    if (nArg0 == 0) {
        CCockpit::SetItemRouletteStateImpl(nArg0, nArg1, nArg2);
    }
}

void msw::CCockpitBoss::PartMoveCheck()
{
    CCockpitPartMove::PartMoveInfo info;
    const long *pObjIdTbl = objIdTbl;
    CCockpitPartMove &rPartMove = CCockpitPartMove::s_Instance;

    while (rPartMove.Pop(info) >= 0) {
        long nPart = info.m_nPart;
        long nMove = info.m_nArg0;
        for (long i = 0; i < OBJ_ID_TBL_LENGTH; i++) {
            CCockpitBossLayoutBase *pObj = (CCockpitBossLayoutBase *)FindObj(pObjIdTbl[i]);
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
                            pObj->LifeGaugeIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->LifeGaugeOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->LifeGaugeInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->LifeGaugeOutWait();
                            break;
                    }
                    break;
                case 2:
                    switch (nMove) {
                        case PART_MOVE_IN:
                            pObj->BossGaugeIn();
                            break;
                        case PART_MOVE_OUT:
                            pObj->BossGaugeOut();
                            break;
                        case PART_MOVE_IN_WAIT:
                            pObj->BossGaugeInWait();
                            break;
                        case PART_MOVE_OUT_WAIT:
                            pObj->BossGaugeOutWait();
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

void msw::CCockpitBoss::SetPartMove(long nPart, long nMove)
{
    CCockpitPartMove::s_Instance.Push(nPart, nMove, 0);
}

long msw::CCockpitBoss::CreateObj(long nObjId, long nParam)
{
    CCockpit *pCockpit = GetTask();

    if (pCockpit == 0) {
        return -2;
    }
    return pCockpit->CreateObjImpl(nObjId, nParam);
}

long msw::CCockpitBoss::CreateObjImpl(long nObjId, long nParam)
{
    CMenuLinkBase *pObj = 0;

    // The cases are in the order of the original's code.
    switch (nObjId) {
        case OBJ_CODER:
            pObj = new CCockpitBossCoder(nObjId);
            break;
        case OBJ_COIN:
            pObj = new CCockpitBossCoin(nObjId);
            break;
        case OBJ_BALL_POWER:
            pObj = new CCockpitBossBallPower(nObjId);
            break;
        case OBJ_RADAR_ICON:
            pObj = new CRadarIcon2DBoss(nObjId);
            break;
        case 4: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xA, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case 5: {
            CCockpitTelopSwitchColor *pTelop = new CCockpitTelopSwitchColor(nObjId, 0xB, 0.0f);
            if (pTelop != 0) {
                pObj = pTelop;
            }
            break;
        }
        case OBJ_MINI_TELOP:
            pObj = new CCockpitTelopMiniPos(nObjId, 0.0f, 2);
            break;
        default:
            pObj = CreateObjCommonImpl(nObjId, nParam);
            break;
    }
    return AddObj(pObj);
}

void msw::CCockpitBoss::SetMenuState(long nState)
{
    CCockpit *pTask = GetTask();

    if (pTask != 0) {
        pTask->ChangeState(nState);
    }
}

void msw::CCockpitBoss::Initialize_In()
{
    // (Object 3, the ball power, is built but never created here.)
    CreateObj(OBJ_CODER, 0);
    CreateObj(OBJ_RADAR_ICON, 0);
    CreateObj(0x1004, 0);
    CreateObj(OBJ_COIN, 0);
    CreateObj(0x1011, 0);
    CreateObj(0x1001, 0);
    Build(OBJ_CODER);
    Build(OBJ_RADAR_ICON);
    Build(0x1004);
    Build(OBJ_COIN);
    Build(OBJ_BALL_POWER);
    Build(0x1011);
    Build(0x1001);
    ChangeState(STATE_GAMEPLAY);
    CreateDemoTelop();
}

void msw::CCockpitBoss::Initialize_Loop()
{
}

void msw::CCockpitBoss::Initialize_Out()
{
}

void msw::CCockpitBoss::GameRestart_In()
{
    long nNextState = -1;

    if (CGame::g_GameWork.GetGameType() == 0) {
        nNextState = STATE_GAMEPLAY;
    }
    if (nNextState >= 0) {
        ChangeState(nNextState);
    }
}

void msw::CCockpitBoss::GameRestart_Loop()
{
}

void msw::CCockpitBoss::GameRestart_Out()
{
}

void msw::CCockpitBoss::GamePlay_In()
{
}

void msw::CCockpitBoss::GamePlay_Loop()
{
}

void msw::CCockpitBoss::GamePlay_Out()
{
}

void msw::CCockpitBoss::GamePause_In()
{
    CCockpitBossPause::Create(GetTask());
}

void msw::CCockpitBoss::GamePause_Loop()
{
}

void msw::CCockpitBoss::GamePause_Out()
{
    CCockpitPause::Remove();
}

void msw::CCockpitBoss::GameResult_In()
{
    m_Unk44 = 0;
}

void msw::CCockpitBoss::GameResult_Loop()
{
}

void msw::CCockpitBoss::GameResult_Out()
{
}
