// msw::CCockpitBossPause: the pause menu of the boss games.
#include "game/msw/CCockpitBossPause.hpp"

#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitPauseLayoutBase.hpp"

namespace {

    // Cockpit object ids of the pause pages. A tab's page is PAUSE_OBJ_BASE
    // plus the tab's Fsm state number.
    enum {
        PAUSE_OBJ_BASE = 0x1085,
        PAUSE_OBJ_BALL_PLAY = 0x1085,
        PAUSE_OBJ_NORMAL_PLAY = 0x1086,
        PAUSE_OBJ_GAME_INFO_FIRST = 0x108D,
        PAUSE_OBJ_GAME_INFO_LAST = 0x108F
    };

    // The message group GetMessageText reads the page texts from.
    const long MSG_GROUP_PAUSE = 0;

    // The game modes the tables below are indexed by. The other sports keep
    // one entry per mode (BasketF picks it from the game work); every entry
    // is the same here and the boss games always use the first.
    enum {
        MODE_NORMAL,
        MODE_NUM = 4
    };

    // Declared before the tab table: the original .rodata has the file name
    // first.
    const char *s_pLayoutName[MODE_NUM] = {
        "BO_pose_icon.brlyt",
        "BO_pose_icon.brlyt",
        "BO_pose_icon.brlyt",
        "BO_pose_icon.brlyt",
    };

    const msw::CCockpitPause::TabParamInfo s_TabParamInfo[] = {
        { 9, 6, 0xDA, 0xE1 },
        { 0, 2, 0xDB, 0 },
        { 1, 3, 0xDC, 0 },
        { 3, 8, 0xDD, 0 },
        { 4, 9, 0xDE, 0 },
        { 5, 10, 0xDF, 0 },
    };

    const msw::CCockpitPause::TabParamInfo *s_pTabParamInfo[MODE_NUM] = {
        s_TabParamInfo,
        s_TabParamInfo,
        s_TabParamInfo,
        s_TabParamInfo,
    };

    // (name guessed; inline) Replaces the page nObjId by a new
    // CCockpitPausePlayInfo.
    inline void OpenPlayInfo(long nObjId, long nKind)
    {
        msw::CCockpit *pCockpit = msw::CCockpit::GetTask();

        if (pCockpit != 0) {
            msw::CCockpit::RemoveObj(nObjId);
            msw::CCockpitPausePlayInfo *pPage = new (1) msw::CCockpitPausePlayInfo(nObjId, nKind, 1.0f);
            if (pPage != 0) {
                pCockpit->AddObj(pPage);
            }
        }
    }

    // (guess) As in CCockpitBossLayoutBase.cpp: the original destructor
    // stores both vtable pointers before it calls the base destructor, which
    // the compiler only does when the body calls something, so the original
    // called a function that is empty in this build.
    inline void Nothing() {}
}

inline msw::CCockpitBossPause::CCockpitBossPause(bcs::CTask *pParent)
    : CCockpitPause(pParent)
{
    m_pTabParamInfo = s_pTabParamInfo[MODE_NORMAL];
    m_nTabParamNum = sizeof(s_TabParamInfo) / sizeof(s_TabParamInfo[0]);
    m_pLayoutName = s_pLayoutName[MODE_NORMAL];
}

bcs::CTask *msw::CCockpitBossPause::Create(bcs::CTask *pParent)
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(s_TaskHandle);

    if (pTask != 0) {
        return pTask;
    }

    CCockpitBossPause *pPause = new CCockpitBossPause(pParent);
    if (pPause != 0) {
        s_TaskHandle = pPause->GetId();
    }
    return pPause;
}

msw::CCockpitBossPause::~CCockpitBossPause()
{
    Nothing();
}

void msw::CCockpitBossPause::Main()
{
    MainLoopCommon();
    CheckChangeTab(m_pTabParamInfo, m_nTabParamNum);
    ExecState();
}

void msw::CCockpitBossPause::Disp()
{
}

void msw::CCockpitBossPause::BallPlay_In()
{
    OpenPlayInfo(PAUSE_OBJ_BALL_PLAY, 0);
}

void msw::CCockpitBossPause::BallPlay_Loop()
{
}

void msw::CCockpitBossPause::BallPlay_Out()
{
    CloseMenu(PAUSE_OBJ_BALL_PLAY);
}

void msw::CCockpitBossPause::NormalPlay_In()
{
    OpenPlayInfo(PAUSE_OBJ_NORMAL_PLAY, 1);
}

void msw::CCockpitBossPause::NormalPlay_Loop()
{
}

void msw::CCockpitBossPause::NormalPlay_Out()
{
    CloseMenu(PAUSE_OBJ_NORMAL_PLAY);
}

void msw::CCockpitBossPause::GameInfo_In()
{
    CCockpit *pCockpit;
    long nObjId = m_nState + PAUSE_OBJ_BASE;

    pCockpit = CCockpit::GetTask();
    if (pCockpit != 0) {
        CCockpit::RemoveObj(nObjId);
        CCockpitPauseRuleInfo *pPage = new (1) CCockpitPauseRuleInfo(nObjId, 1, 1.0f);
        if (pPage != 0) {
            pCockpit->AddObj(pPage);
        }
    }
    m_bGameInfoSet = 0;
}

void msw::CCockpitBossPause::GameInfo_Loop()
{
    if (!m_bGameInfoSet) {
        GameInfo_Init();
    }
}

// Fills the page in once its layout exists: the title is message
// state + 4, the text message state + 1.
void msw::CCockpitBossPause::GameInfo_Init()
{
    long nState = m_nState;
    long nObjId = nState + PAUSE_OBJ_BASE;
    CCockpit *pCockpit = CCockpit::GetTask();
    CCockpitPauseRuleInfo *pInfo;

    if (pCockpit == 0) {
        pInfo = 0;
    } else {
        pInfo = static_cast<CCockpitPauseRuleInfo *>(pCockpit->FindObj(nObjId));
    }

    if (pInfo != 0 && pInfo->hasLayoutObject()) {
        pInfo->SetTitleText(GetMessageText(MSG_GROUP_PAUSE, nState + 4));
        pInfo->SetMessageText(0, GetMessageText(MSG_GROUP_PAUSE, nState + 1));
        m_bGameInfoSet = 1;
    }
}

void msw::CCockpitBossPause::GameInfo_Out()
{
    for (long nObjId = PAUSE_OBJ_GAME_INFO_FIRST; nObjId <= PAUSE_OBJ_GAME_INFO_LAST; nObjId++) {
        CloseMenu(nObjId);
    }
}
