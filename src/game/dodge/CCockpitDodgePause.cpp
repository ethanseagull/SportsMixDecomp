// msw::CCockpitDodgePause: the pause menu of the dodge games.
// Written from src/game/basket/CCockpitBasketPause.cpp (tools/decomp/
// port_unit.py); the dodge menu differs from the basket one in its per-mode
// tab tables and texts (Create) and in the calls that fill its own score
// board page (ScoreBoard_Loop).
#include "game/msw/CCockpitDodgePause.hpp"

#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitDodgePauseResult.hpp"
#include "game/msw/CCockpitPauseLayoutBase.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"

namespace {

    // ---- Sport-specific part (dodge) ----------------------------------

    // The modes the tables below are indexed by: the game type (0, or any
    // other, which uses the "ptg" layout) and whether the tournament match
    // is the extra match. Names guessed.
    enum {
        MODE_STANDARD,       // game type 0
        MODE_STANDARD_EXTRA, // game type 0, extra match
        MODE_OTHER,          // any other game type
        MODE_OTHER_EXTRA,    // any other game type, extra match
        MODE_NUM
    };

    // Declared before the tab tables: the original .rodata has the file
    // names first.
    const char *s_pLayoutName[MODE_NUM] = {
        "DO_pose_icon.brlyt",
        "DO_pose_icon.brlyt",
        "DO_pose_ptg_icon.brlyt",
        "DO_pose_ptg_icon.brlyt",
    };

    // The tabs of each mode (see CCockpitPause::TabParamInfo).
    const msw::CCockpitPause::TabParamInfo s_TabParamInfoStandard[] = {
        { 9, 6, 0xDA, 0xE1 },
        { 0, 2, 0xDB, 0 },
        { 1, 3, 0xDC, 0 },
        { 7, 4, 0xDD, 0 },
        { 8, 5, 0xDE, 0xE6 },
    };

    const msw::CCockpitPause::TabParamInfo s_TabParamInfoStandardExtra[] = {
        { 9, 6, 0xDA, 0xE1 },
        { 0, 2, 0xDB, 0 },
        { 1, 3, 0xDC, 0 },
        { 6, 7, 0xDD, 0 },
        { 8, 5, 0xDE, 0xE6 },
    };

    const msw::CCockpitPause::TabParamInfo s_TabParamInfoOther[] = {
        { 9, 11, 0xDA, 0xE3 },
        { 1, 3, 0xDB, 0 },
        { 2, 8, 0xDC, 0 },
    };

    const msw::CCockpitPause::TabParamInfo s_TabParamInfoOtherExtra[] = {
        { 9, 6, 0xDA, 0xE1 },
        { 1, 3, 0xDB, 0 },
        { 2, 8, 0xDC, 0 },
        { 6, 7, 0xDD, 0 },
    };

    const msw::CCockpitPause::TabParamInfo *s_pTabParamInfo[MODE_NUM] = {
        s_TabParamInfoStandard,
        s_TabParamInfoStandardExtra,
        s_TabParamInfoOther,
        s_TabParamInfoOtherExtra,
    };

    const long s_nTabParamNum[MODE_NUM] = { 5, 5, 3, 4 };

    // The message group and number of the text CCockpitPause keeps at 0xA4
    // (group meaning guessed).
    enum {
        MSG_GROUP_PAUSE = 0,
        MSG_NUM_PAUSE = 8,
        MSG_GROUP_OTHER_EXTRA = 2,
        MSG_NUM_OTHER_EXTRA = 0x12
    };

    // The three pages of the score board: the points, the characters, and
    // the dodge games' own page.
    const long s_ScoreBoardObjId[] = { 0x1087, 0x1089, 0x1088 };

    // ---- Common part ---------------------------------------------------

    // (name guessed; inline) Replaces the page nObjId by a new
    // CCockpitPausePlayInfo.
    inline void OpenPlayInfo(long nObjId, long nKind, float fSortZ)
    {
        msw::CCockpit *pCockpit = msw::CCockpit::GetTask();

        if (pCockpit != 0) {
            msw::CCockpit::RemoveObj(nObjId);
            msw::CCockpitPausePlayInfo *pPage = new (1) msw::CCockpitPausePlayInfo(nObjId, nKind, fSortZ);
            if (pPage != 0) {
                pCockpit->AddObj(pPage);
            }
        }
    }

    // (names guessed; inline) The same for the score board pages.
    inline void OpenResult(long nObjId, float fSortZ)
    {
        msw::CCockpit *pCockpit = msw::CCockpit::GetTask();

        if (pCockpit != 0) {
            msw::CCockpit::RemoveObj(nObjId);
            msw::CCockpitPauseResult *pPage = new (1) msw::CCockpitPauseResult(nObjId, 0, fSortZ);
            if (pPage != 0) {
                pCockpit->AddObj(pPage);
            }
        }
    }

    inline void OpenResultChr(long nObjId, float fSortZ)
    {
        msw::CCockpit *pCockpit = msw::CCockpit::GetTask();

        if (pCockpit != 0) {
            msw::CCockpit::RemoveObj(nObjId);
            msw::CCockpitPauseResultChr *pPage = new (1) msw::CCockpitPauseResultChr(nObjId, 0, fSortZ);
            if (pPage != 0) {
                pCockpit->AddObj(pPage);
            }
        }
    }

    inline void OpenDodgeResult(long nObjId, float fSortZ)
    {
        msw::CCockpit *pCockpit = msw::CCockpit::GetTask();

        if (pCockpit != 0) {
            msw::CCockpit::RemoveObj(nObjId);
            msw::CCockpitDodgePauseResult *pPage = new (1) msw::CCockpitDodgePauseResult(nObjId, fSortZ);
            if (pPage != 0) {
                pCockpit->AddObj(pPage);
            }
        }
    }

    // (name guessed; inline) The cockpit object nObjId as a T, or null. The
    // cast is inside the inline: casting at the call site gives other registers.
    template <class T>
    inline T *FindPage(long nObjId)
    {
        msw::CCockpit *pCockpit = msw::CCockpit::GetTask();

        if (pCockpit == 0) {
            return 0;
        }
        return static_cast<T *>(pCockpit->FindObj(nObjId));
    }

    // (guess) As in CCockpitBasketLayoutBase.cpp: the original destructor
    // stores both vtable pointers before it calls the base destructor, which
    // the compiler only does when the body calls something, so the original
    // called a function that is empty in this build.
    inline void Nothing() {}
}

inline msw::CCockpitDodgePause::CCockpitDodgePause(bcs::CTask *pParent)
    : CCockpitPause(pParent)
{
    long nMode;

    if (CGame::g_GameWork.GetGameType() == 0) {
        nMode = CTournament::IsExtraMatchGame() ? MODE_STANDARD_EXTRA : MODE_STANDARD;
    } else {
        nMode = CTournament::IsExtraMatchGame() ? MODE_OTHER_EXTRA : MODE_OTHER;
    }

    m_pTabParamInfo = s_pTabParamInfo[nMode];
    m_nTabParamNum = s_nTabParamNum[nMode];
    m_pLayoutName = s_pLayoutName[nMode];

    if (nMode == MODE_OTHER_EXTRA) {
        m_pUnkA4 = GetMessageText(MSG_GROUP_OTHER_EXTRA, MSG_NUM_OTHER_EXTRA);
    } else {
        m_pUnkA4 = GetMessageText(MSG_GROUP_PAUSE, MSG_NUM_PAUSE);
    }
}

bcs::CTask *msw::CCockpitDodgePause::Create(bcs::CTask *pParent)
{
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(s_TaskHandle);

    if (pTask != 0) {
        return pTask;
    }

    CCockpitDodgePause *pPause = new CCockpitDodgePause(pParent);
    if (pPause != 0) {
        s_TaskHandle = pPause->GetId();
    }
    return pPause;
}

msw::CCockpitDodgePause::~CCockpitDodgePause()
{
    Nothing();
}

void msw::CCockpitDodgePause::Main()
{
    MainLoopCommon();
    CheckChangeTab(m_pTabParamInfo, m_nTabParamNum);
    ExecState();
}

void msw::CCockpitDodgePause::Disp()
{
}

void msw::CCockpitDodgePause::BallPlay_In()
{
    static const long objId = 0x1085;

    OpenPlayInfo(objId, 0, 1.0f);
}

void msw::CCockpitDodgePause::BallPlay_Loop()
{
}

void msw::CCockpitDodgePause::BallPlay_Out()
{
    CloseMenu(0x1085);
}

void msw::CCockpitDodgePause::NormalPlay_In()
{
    static const long objId = 0x1086;

    OpenPlayInfo(objId, 1, 1.0f);
}

void msw::CCockpitDodgePause::NormalPlay_Loop()
{
}

void msw::CCockpitDodgePause::NormalPlay_Out()
{
    CloseMenu(0x1086);
}

void msw::CCockpitDodgePause::ScoreBoard_In()
{
    // (guess) The original fetches the cockpit here and never uses it.
    CCockpit *pCockpit = CCockpit::GetTask();
    const long *pObjId = s_ScoreBoardObjId;

    OpenResult(*pObjId, 1.0f);
    OpenResultChr(pObjId[1], 2.0f);
    OpenDodgeResult(pObjId[2], 3.0f);
    m_nScoreBoardStep = 1;
}

// Fills the pages one frame after ScoreBoard_In made them.
void msw::CCockpitDodgePause::ScoreBoard_Loop()
{
    switch (m_nScoreBoardStep) {
        case 1:
            m_nScoreBoardStep = 0;
            break;

        case 0: {
            CCockpit *pCockpit = CCockpit::GetTask();
            const long *pObjId = s_ScoreBoardObjId;
            CCockpitPauseResult *pResult = FindPage<CCockpitPauseResult>(*pObjId);
            CCockpitPauseResultChr *pResultChr = FindPage<CCockpitPauseResultChr>(pObjId[1]);
            CCockpitDodgePauseResult *pDodgeResult = FindPage<CCockpitDodgePauseResult>(pObjId[2]);

            if (pCockpit != 0 && pResult != 0 && pResultChr != 0 && pDodgeResult != 0) {
                long aTotal[2];

                pResult->Set();
                pResultChr->Set();
                pDodgeResult->Set();
                SetPauseResultParameter(pResult, aTotal);
                SetPauseResultChrParameter(pResultChr);

                long nPeriodNum = pCockpit->m_nPeriodNum;
                long nPeriod = pCockpit->m_nPeriod;
                pDodgeResult->SetCountScore(pCockpit->m_aPeriodScore[0], pCockpit->m_aPeriodScore[1], nPeriodNum);
                pDodgeResult->SetCountPoint(pCockpit->m_aPeriodPoint[0], pCockpit->m_aPeriodPoint[1], nPeriod, nPeriodNum);
                pDodgeResult->SetTextNext(nPeriodNum, nPeriod);
                pDodgeResult->SetCountNum(nPeriod);
                pDodgeResult->SetTotalPoint(aTotal[0], aTotal[1]);
            }
            m_nScoreBoardStep = 2;
            break;
        }
    }
}

void msw::CCockpitDodgePause::ScoreBoard_Out()
{
    static const long objIdTblLen = 3;

    for (long i = 0; i < objIdTblLen; i++) {
        CloseMenu(s_ScoreBoardObjId[i]);
    }
}
