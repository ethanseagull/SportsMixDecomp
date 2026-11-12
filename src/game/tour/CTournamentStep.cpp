#include "game/msw/CTournamentStep.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CPadWI.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CWifiPad.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitMenuWinBtn.hpp"
#include "game/msw/CCockpitTournamentMenuWinTrophy.hpp"
#include "game/msw/CCokcpitMenuDataList.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameAccInput.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CMainStep.hpp"
#include "game/msw/CNowLoading.hpp"
#include "game/msw/CRouteBase.hpp"
#include "game/msw/CTmGameHandler.hpp"
#include "game/msw/EvDemoVs.hpp"
#include "game/msw/TournamentRule.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CStateSave.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CMenuPad.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/EvDemo.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CLoadMan.hpp"
#include "game/msw/CEventDemo.hpp"
#include "bcs/CRenderMan.hpp"
#include "game/msw/CNameMesg.hpp"
#include "game/msw/CTmScript.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CTournamentExit.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/CTournamentRouteMan.hpp"
#include "game/msw/CUnlockFlags.hpp"
#include "game/msw/Tournament.hpp"
#include "game/msw/TournamentData.hpp"
#include "game/msw/TournamentWork.hpp"
#include "game/msw/menu/CVibEff.hpp"

// The message files of the menus this file opens.
static const char *s_pMenuMessage = "GameMenuMessage";
static const char *s_pDemoMessage = "Text_Demonstration";
static const char *s_pViewerMessage = "Text_Map_Viewer";

// The map's music: the sound task playing it, and the music asked for that
// starts once it ended (-1: none).
static unsigned long s_hBgm[2];
static long s_nBgmReq[2];

namespace {
    bcs::CSndTask *GetBgmTask()
    {
        if (s_hBgm[0] == 0) {
            return 0;
        }
        return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(s_hBgm[0]));
    }

    void PlayBgm(long nBgm)
    {
        bcs::CSndTask *pTask = GetBgmTask();
        if (pTask != 0) {
            pTask->Stop(0.0f);
        }
        s_hBgm[0] = 0;
        s_nBgmReq[0] = -1;
        s_hBgm[0] = msw::CGameSnd::PlayBGM(nBgm, 1, 1.0f, 0.0f);
        s_nBgmReq[0] = -1;
    }
}

// Opens the trophy window (object nObjId) with the text pText.
extern "C" void CreateMenuGetTrophy(long nObjId, const wchar_t *pText)
{
    msw::CCockpit *pCockpit = msw::CCockpit::GetTask();
    if (pCockpit != 0) {
        msw::CCockpitTournamentMenuWinTrophy *pWin = new msw::CCockpitTournamentMenuWinTrophy(nObjId, 0.0f, 0);
        pCockpit->AddObj(pWin);
        pWin->SetMenuResultId(0, 1000);
        pWin->SetWindowText(0, pText);
        pWin->Setup();
    }
}

namespace msw {

    bcs::CTask *Tournament::CreateInstance(bcs::CTask *pParent)
    {
        return new CTournamentStep(pParent);
    }

    bool CTournamentExit::m_bEnable;

    CTournamentExit::CTournamentExit(CTournamentStep *pStep, long nPad) : bcs::CTask(pStep, -1)
    {
        m_pStep = pStep;
        m_nWindow = -1;
        m_nOutWindow = -1;
        m_nPad = nPad;
        m_bPause = 1;
        ClearUnk08(1);
        m_nPushNum = 0;
        m_nStep = 0;
        m_bEnable = false;
    }

    CTournamentExit::~CTournamentExit()
    {
        bcs::CMainApp::SetPause(1, 0);
        if (CCokcpitMenuDataList::GetInstance() != 0) {
            while (m_nPushNum > 0) {
                CCokcpitMenuDataList::GetInstance()->Pop();
                m_nPushNum--;
            }
        }
        if (CCockpit::GetTask() != 0 && m_nWindow != -1) {
            CCockpit::RemoveObj(m_nWindow);
        }
        m_nOutWindow = -1;
        m_nWindow = -1;
    }

    void CTournamentExit::MenuOutWindow()
    {
        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit != 0 && m_nWindow != -1) {
            CCockpitMenuBase *pMenu = static_cast<CCockpitMenuBase *>(pCockpit->FindObj(m_nWindow));
            if (pMenu != 0) {
                if (!pMenu->IsMenuOut()) {
                    pMenu->MenuOut();
                }
                m_nOutWindow = m_nWindow;
                m_nWindow = -1;
            }
        }
    }

    void CTournamentExit::RemoveWindow()
    {
        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit != 0 && m_nWindow != -1) {
            if (pCockpit->FindObj(m_nWindow) != 0) {
                CCockpit::RemoveObj(m_nWindow);
            }
            m_nOutWindow = -1;
            m_nWindow = -1;
        }
    }

    int CTournamentExit::IsMenuOutWindow()
    {
        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit == 0 || m_nOutWindow == -1) {
            return 1;
        }
        CCockpitMenuBase *pMenu = static_cast<CCockpitMenuBase *>(pCockpit->FindObj(m_nOutWindow));
        if (pMenu == 0) {
            m_nOutWindow = -1;
            return 1;
        }
        if (!pMenu->IsMenuOut()) {
            pMenu->MenuOut();
        }
        return 0;
    }

    void CTournamentExit::CreateButtonWindow2(const CCockpitButtonInfo::ButtonInfo *pInfo, const char *pFile,
                                              const char *pLabel)
    {
        CCockpit *pCockpit = CCockpit::GetTask();
        m_nWindow = pCockpit->GetNextCockpitMenuObjId();
        CCockpitMenuWinBtn2 *pWin = new CCockpitMenuWinBtn2(m_nWindow, 0.0f, 0);
        pCockpit->AddObj(pWin);
        pWin->SetButtonInfo(pInfo);
        long nGroup;
        long nLabel;
        pWin->SetWindowText(0, CNameMesg::_GetMessageText(nGroup, nLabel, pFile, pLabel));
        pWin->Setup();
    }

    void CTournamentExit::Main()
    {
        switch (m_nStep) {
        case 0:
            if (_CanbeExit() && _InputCheck()) {
                CLytCtrl::SetPauseStop();
                CCockpit::GetTask()->SetPauseObj(1);
                if (CCokcpitMenuDataList::GetInstance() != 0) {
                    CCokcpitMenuDataList::GetInstance()->Push();
                    m_nPushNum++;
                }
                m_nStep++;
            }
            break;
        case 1:
            _PauseStart();
            CGameSnd::Play(0xBC2, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            m_nStep++;
            break;
        case 2:
            m_nStep++;
            break;
        case 3:
            SetState(-1000);
            m_nStep++;
            break;
        case 4:
            if (IsMenuOutWindow()) {
                m_fWait = 0.0f;
                m_nStep = 10;
            }
            break;
        case 8:
            if (IsMenuOutWindow()) {
                m_fWait = 0.0f;
                m_nStep++;
            }
            break;
        case 9:
            m_fWait += GetFrameRateF();
            if (m_fWait >= 5.0f) {
                SetState(m_nResult);
                m_fWait = 0.0f;
                m_nStep++;
            }
            break;
        case 10:
            if (bcs::CWifiPad::IsTrg(0x900, 0)) {
                m_nStep++;
            } else {
                _CheckResult(_InputCheck());
            }
            break;
        case 11:
            _CheckResult(0);
            break;
        case 12:
            if (IsMenuOutWindow()) {
                m_fWait = 0.0f;
                _PauseEnd();
                m_nStep++;
            }
            break;
        case 13:
        case 14:
            m_nStep++;
            break;
        case 15:
            CLytCtrl::ClearPauseStop();
            CCockpit::GetTask()->SetPauseObj(0);
            if (m_nPushNum > 0 && CCokcpitMenuDataList::GetInstance() != 0) {
                CCokcpitMenuDataList::GetInstance()->Pop();
                m_nPushNum--;
            }
            m_fWait = 0.0f;
            m_nStep++;
            break;
        case 16:
            m_fWait += GetFrameRateF();
            if (m_fWait >= 5.0f) {
                m_nStep++;
            }
            break;
        default:
            m_nStep = 0;
            break;
        }
    }

    void CTournamentExit::_CheckResult(int bCancel)
    {
        m_nResult = CCockpit::GetResult();
        m_fWait += GetFrameRateF();
        m_bPause = 1;
        switch (m_nResult) {
        case -998:
            CTournament::Save(CTournament::GetGameMode(), 0, -1);
            if (m_pStep != 0) {
                m_pStep->m_UnkD8 = CTournamentStep::STATE_GAME_EXIT;
                m_pStep->ChangeState(CTournamentStep::STATE_GAME_SAVE);
                m_bPause = 0;
            }
            m_nStep = 12;
            break;
        case -1:
            if (bCancel) {
                CGameSnd::Play(0xBC3, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                MenuOutWindow();
                m_nStep = 12;
            }
            break;
        case 1001:
            m_nStep = 12;
            break;
        default:
            if (m_nResult < 1001) {
                m_nStep = 8;
            } else {
                m_nStep = 12;
            }
            break;
        }
    }

    int CTournamentExit::_CanbeExit()
    {
        bool bFade = false;
        if (bcs::CMainApp::IsPlay3DFade()) {
            return 0;
        }
        if (bcs::CMainApp::IsPlayMasterFade()) {
            return 0;
        }
        if (bcs::CMainApp::IsPlayCrossFade()) {
            return 0;
        }
        if (bcs::CMainApp::IsPlay3DCrossFade()) {
            return 0;
        }
        if (!m_bEnable) {
            return 0;
        }
        if (!m_bPause) {
            return 0;
        }
        if (bcs::CMainApp::IsPause(0xFC01)) {
            return 0;
        }
        return bcs::CMainApp::IsPauseReady(0xFC01) == 0;
    }

    int CTournamentExit::_InputCheck()
    {
        if (m_nPad != -1 && bcs::CWifiPad::IsTrg(m_nPad, 0)) {
            return 1;
        }
        return 0;
    }

    void CTournamentExit::_PauseStart()
    {
        CGameSnd::GamePauseAll(1, 18.0f);
        bcs::CPadWI::PushMotorAll();
        bcs::CMainApp::SetPause(1, 1);
    }

    void CTournamentExit::_PauseEnd()
    {
        menu::CVibEff *pVib = menu::CVibEff::GetInstance();
        if (pVib != 0) {
            pVib->Stop(0);
        }
        if (m_bPause) {
            bcs::CMainApp::SetPause(1, 0);
            bcs::CPadWI::PopMotorAll();
            CGameSnd::GamePauseAll(0, 18.0f);
        } else {
            bcs::CPadWI::PopMotorAll();
            if (m_pStep != 0) {
                m_pStep->ClearUnk08(1);
            }
        }
    }

    int CTournamentExit::SetState(long nState)
    {
        RemoveWindow();
        m_nState = nState;
        switch (nState) {
        case -1000: {
            const CCockpitButtonInfo::ButtonInfo aInfo[2] = {
                {0xCF, 1001, s_pMenuMessage, "PAUSE_25_00", 1},
                {0xD0, -999, s_pMenuMessage, "PAUSE_26_00", 0},
            };
            CreateButtonWindow2(aInfo, s_pMenuMessage, "PAUSE_24_00");
            break;
        }
        case -999: {
            const CCockpitButtonInfo::ButtonInfo aInfo[2] = {
                {0xD0, -998, s_pMenuMessage, "END_TOU_B_01", 0},
                {0xCF, -1000, s_pMenuMessage, "END_TOU_B_02", 0},
            };
            CreateButtonWindow2(aInfo, s_pMenuMessage, "END_TOU_B_00_01");
            break;
        }
        }
        return 1;
    }

    void CTournamentExit::Disp() {}

    // How far a team stands from the centre of its map point, by the
    // spacing setting of the point.
    static const float s_afPointOffset[] = {1.3f, 1.3f, 2.0f, 2.0f, 1.3f, 1.3f, 2.6f};

    // The team standing on map point nPoint, -1 for none.
    static inline long GetPointTeam(long nPoint)
    {
        for (long i = 0; i < 16; i++) {
            const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(i);
            if (team.nPoint != 0xFF && team.nPoint == static_cast<unsigned char>(nPoint)) {
                return i;
            }
        }
        return -1;
    }

    float TournamentData::GetPointOffsetTeam(long nPoint, long nTeam)
    {
        if (nPoint != -1) {
            const TournamentWork::POINT_INFO *pPoint = TournamentWork::GetPointInfo(nPoint);
            if (pPoint->m_Unk0E > 0) {
                return s_afPointOffset[pPoint->m_Unk0E];
            }
            long nOther = GetPointTeam(nPoint);
            if (nOther >= 0 && nOther != nTeam) {
                return -15.0f;
            }
        }
        return 0.0f;
    }

    // The flag bit of character nType in colour nColor.
    static inline unsigned long long GetCharBit(long nType, long nColor)
    {
        return 1ULL << (CUnlockFlags::GetCharFlagPos(nType, nColor, 0) % 32);
    }

    unsigned long long TournamentWork::GetUsedChar(long nTeam, int bSkipBoss)
    {
        unsigned long long nUsed = 0;
        const TEAM_INFO &player = GetTeamInfo(GetTournamentInfo().GetPlayerTeam());
        if (player.nPoint != 0xFF) {
            for (long i = 1; i < 3; i++) {
                if (player.aCharType[i] != -1) {
                    nUsed |= GetCharBit(player.aCharType[i], player.aCharFlag[i] & 3);
                }
            }
        }
        if (nTeam != -1) {
            const TEAM_INFO &team = GetTeamInfo(nTeam);
            if (team.nPoint != 0xFF) {
                for (long i = 1; i < 3; i++) {
                    if (team.aCharType[i] != -1) {
                        nUsed |= GetCharBit(team.aCharType[i], team.aCharFlag[i] & 3);
                    }
                }
            }
        }
        for (long i = 0; i < 16; i++) {
            if (!bSkipBoss || i != 8) {
                const TEAM_INFO &team = GetTeamInfo(i);
                if (team.nPoint != 0xFF && team.aCharType[0] != -1) {
                    nUsed |= GetCharBit(team.aCharType[0], team.aCharFlag[0] & 3);
                }
            }
        }
        return nUsed;
    }

    unsigned long long TournamentWork::GetSettingChar(long nGameMode, long nLevel)
    {
        const TOURNAMENT_INFO &info = GetTournamentInfo();
        unsigned long long nFlag = info.aLevelCharFlag[nGameMode];
        unsigned long long nChar = 0;
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        for (long i = 0; i < 64; i++) {
            long nType;
            long nColor;
            CUnlockFlags::GetCharType(nType, nColor, i);
            long nPos = CUnlockFlags::GetCharFlagPos(nType, nColor, 0);
            unsigned long long nBit = 1ULL << nPos;
            if ((nFlag & nBit) != 0 && !flags.IsUnlockChar(nType, nColor, nGameMode)) {
                nChar |= nBit;
            }
        }
        return nChar;
    }

    void TournamentWork::RandomSelectSubChar(long nTeam, long nUnk, long nGameMode, int bNoSecret)
    {
        long nMax = CTournament::GetTeamMax();
        long nLevel = CTournament::GetLevel();
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        TEAM_INFO &team = TeamInfo(nTeam);
        for (long i = 0; i < nMax; i++) {
            if (team.aCharType[i] == -1) {
                unsigned long long nMask = flags.GetUnlockChar(nGameMode, nLevel, 1);
                if (!bNoSecret) {
                    nMask |= flags.GetUnlockChar(nGameMode, nLevel, 2);
                }
                long nType;
                long nColor;
                CUnlockFlags::GetCharType(nType, nColor,
                                          flags.GetCharRandom(nGameMode, -1, nMask, GetUsedChar(nTeam, 1)));
                team.aCharType[i] = nType;
                team.aCharFlag[i] = (team.aCharFlag[i] & 0xF8) | static_cast<unsigned char>(nColor);
            }
        }
    }

    void TournamentWork::RandomSelectLeaderChar(long nTeamNum, int bSkipBoss)
    {
        long nLevel = CTournament::GetLevel();
        long nGameMode = CTournament::GetGameMode();
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        for (long i = 0; i < nTeamNum; i++) {
            TEAM_INFO &team = TeamInfo(i);
            if (team.aCharType[0] == -1) {
                unsigned long long nMask = 0;
                if (CTournament::IsMixTournamentGame()) {
                    for (long nMode = 0; nMode <= 3; nMode++) {
                        nMask |= GetSettingChar(nMode, nLevel);
                        nMask |= flags.GetUnlockChar(nMode, nLevel, 2);
                    }
                } else {
                    nMask = GetSettingChar(nGameMode, nLevel);
                    nMask |= flags.GetUnlockChar(nGameMode, nLevel, 2);
                }
                long nType;
                long nColor;
                CUnlockFlags::GetCharType(nType, nColor,
                                          flags.GetCharRandom(nGameMode, -1, nMask, GetUsedChar(-1, bSkipBoss)));
                if (team.nPoint == 0xFF) {
                    team.nTargetPoint = team.nPoint = i + 1;
                }
                if (nColor >= 0x10) {
                    nColor -= 0x10;
                }
                team.aCharType[0] = nType;
                team.aCharFlag[0] = (team.aCharFlag[0] & 0xF8) | static_cast<unsigned char>(nColor);
            }
        }
    }

    void TournamentWork::SetLeaderNoUnlock(long nTeam, long nChar, unsigned long nFlag)
    {
        long nGameMode = CTournament::GetGameModeNoMix();
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        TEAM_INFO &team = TeamInfo(nTeam);
        if (team.aCharType[0] == -1) {
            long nType;
            long nColor;
            CUnlockFlags::GetCharType(nType, nColor, nChar);
            if (!flags.IsChar(nType, nColor, nGameMode)) {
                if (team.nPoint == 0xFF) {
                    team.nTargetPoint = team.nPoint = nTeam + 1;
                }
                if (nColor >= 0x10) {
                    nColor -= 0x10;
                }
                team.aCharType[0] = nType;
                team.aCharFlag[0] = (team.aCharFlag[0] & 0xF8) | static_cast<unsigned char>(nColor);
                team.nFlag22 |= nFlag;
            }
        }
    }

    void TournamentWork::BgmPlay()
    {
        if (GetTournamentInfo().nBgm >= 0) {
            if (GetBgmTask() == 0) {
                PlayBgm(GetTournamentInfo().nBgm);
            }
        }
    }

    void CTournamentStep::MapObjSet(long nObj, const bcs::FVEC3 &vPos, float fRotY, const char *pName, int nFlag)
    {
        CTournamentRouteMan::GetInstance()->_StageSet(vPos, nObj, fRotY, pName, 0.0f, 0x1F, nFlag);
    }

    void CTournamentStep::RouteWrite(long nRoute, int bChanged)
    {
        CTournamentRouteMan::GetInstance()->SetRouteWrite(nRoute, bChanged);
    }

    void CTournamentStep::ChipWrite(long nPoint, unsigned long nUnk1, unsigned long nUnk2, long nUnk3, int nUnk4)
    {
        CTournamentRouteMan::GetInstance()->SetChipWrite(nPoint, nUnk1, nUnk2, nUnk3, nUnk4);
    }

    void CTournamentStep::SendRouteEvent(long nRoute, long nEvent, float fStart, unsigned long nUnk, float fEnd)
    {
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        pRouteMan->_AddTimer(nRoute, nRoute, pRouteMan->m_fTimer + fEnd, nEvent, fStart, nUnk);
    }

    void CTournamentStep::SetRoutePoints(long nPoint, const unsigned char *pRoute, long nNum)
    {
        CRouteBase *pRouteObj = CTournamentRouteMan::GetInstance()->GetRoute(nPoint);
        if (pRouteObj != 0) {
            pRouteObj->SetPoints(pRoute, nNum);
        }
    }
}

namespace msw {

    // Marks team nTeam as arrived at the point it was moving to; returns
    // that point.
    inline unsigned char SetMoveEnd(long nTeam)
    {
        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        team.nPoint = team.nTargetPoint;
        return team.nPoint;
    }

    // The routes a team may take: the flags MakeRouteSelect masks the
    // routes with (1 for a team of human players, 2 otherwise).
    static inline unsigned char GetRouteMask(const TournamentWork::TEAM_INFO &team)
    {
        return 2 - ((team.aCharFlag[0] >> 7) & 1);
    }

    // Which player markers and shadow map point nPoint shows (see
    // TOURNAMENT_INFO::nMarkerFlag).
    static inline unsigned char GetPointMarkerFlag(long nPoint)
    {
        unsigned char nFlag = 0;
        unsigned char nPointFlag = TournamentWork::GetPointInfo(nPoint)->nFlag;
        if (nPointFlag & 0x10) {
            nFlag = 1;
        }
        if (nPointFlag & 0x08) {
            nFlag |= 2;
        }
        if (nPointFlag & 0x04) {
            nFlag |= 4;
        }
        return nFlag;
    }

    void CTournamentStep::_AddTimerRel(short nEvent, float fTime, short nParam, unsigned long ulParam)
    {
        _AddTimer(nEvent, m_fTimer + fTime, nParam, ulParam);
    }

    void CTournamentStep::TeamErase(long nTeam, long nType)
    {
        if (nTeam == -1) {
            return;
        }
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pModel = pMan->GetChr(nTeam);
        if (pModel == 0) {
            return;
        }
        if (nType == 1) {
            float fFrame = 18.0f + pModel->AnimResFrame(8);
            _AddTimerRel(-0x6D, 22.0f, 0, nTeam);
            _AddTimerRel(-0x69, fFrame, 0, nTeam);
            _AddTimerRel(-0x68, 10.0f + fFrame, 0, nTeam);
            CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
            pRouteMan->_AddTimer(0x3F4, 0x3F4, pRouteMan->m_fTimer + fFrame, nTeam, 0.0f, nTeam);
        }
        pMan->EraseEffect(nTeam + 0x15, nTeam + 0x16);
        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        if (team.nPoint != 0xFF) {
            team.nFlag22 &= ~2;
        }
        pModel->SetDisp(0);
    }

    void CTournamentStep::TeamPos(long nTeam, long nPoint, long nUnk)
    {
        if (nTeam == -1) {
            return;
        }
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        SetMoveEnd(nTeam);
        pMan->CleanMoveRoute(nTeam, 1);
        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        if (team.nPoint != 0xFF) {
            team.fUnk00[0] = 0.0f;
            team.fUnk00[1] = 0.0f;
            team.fUnk00[2] = 0.0f;
            team.nTargetPoint = team.nPoint = nPoint;
            team.aRoute[0] = 0;
            team.aRoute[1] = 0;
            team.aRoute[2] = 0;
            team.aRoute[3] = 0;
        }
        if (nTeam == 0) {
            unsigned char nFlag = GetPointMarkerFlag(nPoint);
            long bMarker0 = nFlag & 1;
            TournamentWork::TournamentInfo().nMarkerFlag = nFlag;
            pMan->PlayerMarkerDisp(bMarker0 != 0, 200, 0);
            long bMarker1 = nFlag & 2;
            pMan->PlayerMarkerDisp(bMarker1 != 0, 201, 0);
            pMan->PlayerShadowDisp((nFlag >> 2) & 1, 0, 0);
            if (bMarker0 != 0) {
                pMan->PlayerMarkerSetPosY(TournamentWork::GetTournamentInfo().fMarkerY, 200);
            }
            if (bMarker1 != 0) {
                pMan->PlayerMarkerSetPosY(TournamentWork::GetTournamentInfo().fMarkerY, 201);
            }
        }
        CEventModel *pModel = pMan->GetChr(nTeam);
        if (pModel != 0) {
            bcs::FVEC3 vPos;
            TournamentWork::GetPointViewPos(vPos, team.nPoint);
            pModel->SetPosition(vPos);
            pMan->InitMoveRoute(nTeam);
            TournamentData::SetIdlePause(pModel, 0.0f);
        }
    }

    void CTournamentStep::PointCamera(long nPoint, long nCamera, float fUnk1, float fUnk2)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        if (!(TournamentWork::GetPointInfo(nPoint)->nFlag & 0x20)) {
            pMan->SetCameraActive(0);
            pMan->SetCameraPoint(nPoint, nCamera);
            pMan->SetCameraRate(fUnk1, fUnk2, 0.0007f);
        }
    }

    void CTournamentStep::FadeIn(float fSpeed, int nType)
    {
        if (m_bFadeInReq) {
            if (nType == 0) {
                CTournamentRouteMan::GetInstance()->StageDispSe(1);
                _StartFade(4, fSpeed, 0);
                CTournamentExit::m_bEnable = true;
                TournamentWork::BgmPlay();
            }
            m_bFadeInReq = 0;
        }
    }

    void CTournamentStep::MoveTarget(long nTeam, long nPoint, const unsigned char *pVia)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        pMan->MakePointData(0);
        pMan->CleanMoveRoute(nTeam, 1);
        const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
        m_nRouteNum0 =
            pMan->MakeRouteSelect(m_aRoute0, team.nPoint, team.nTargetPoint, GetRouteMask(team), pVia, 0.7853982f);
        float fOffset = TournamentData::GetPointOffsetTeam(nPoint, nTeam);
        int bContinue = 0;
        if (m_nRouteNum0 > 0) {
            if (m_aRoute0[0] == team.GetLastRoute()) {
                bContinue = 1;
            }
        }
        m_UnkB4 |= pMan->SetMoveRoute(nTeam, m_nFadeType == 0, m_aRoute0, m_nRouteNum0, fOffset, bContinue, 0.0f);
        pMan->PushMoveRoute(nTeam, m_aRoute0, m_nRouteNum0);
        CTournamentMan::m_pInstance->m_ulMoveStop |= m_UnkB4;
        m_UnkD0 = -1;
        m_UnkC5 = (TournamentWork::GetPointInfo(nPoint)->nFlag >> 6) & 1;
    }

    void CTournamentStep::MoveResult(long nMatch, int bWin, float fTime)
    {
        const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(nMatch);
        if (bWin) {
            signed char nWin = TournamentWork::GetMatchInfo(nMatch).nWinTeam;
            if (nWin != -1 && !(TournamentWork::GetTeamInfo(nWin).aCharFlag[0] & 0x10)) {
                _AddTimerRel(-0x76, fTime, nMatch, nWin);
            }
        } else {
            signed char nLose = TournamentWork::GetLoseTeam(nMatch);
            if (nLose != -1 && !(TournamentWork::GetTeamInfo(nLose).aCharFlag[0] & 0x10) && !(match.nState & 0x10)) {
                _AddTimerRel(-0x73, fTime, nMatch, nLose);
            }
        }
    }

    void CTournamentStep::RouteSelectSetup(unsigned char nPoint, const unsigned char *pRoute, long nNum, long nUnk2,
                                           long nUnk3)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        pMan->SetupSelect(m_apSelect, nPoint, pRoute, nNum, nUnk2, nUnk3, 1.2f, 1.2f, 0.0f);
        m_pSelectRoute = pRoute;
        m_nSelectNum = nNum;
        m_nSelectPoint = nPoint;
        m_fSelectTime = 0.0f;
        m_nSelectStep = 0;
        m_nSelect = 0;
        unsigned long nMode = 3;
        long bShadow = (TournamentWork::GetTournamentInfo().nMarkerFlag >> 2) & 1;
        if (bShadow) {
            nMode = 7;
        }
        pMan->PlaySe(0xE15, static_cast<const bcs::FVEC3 *>(0), 6, 0.0f, 1.0f, 0.0f, 1);
        for (long i = 0; i < nNum; i++) {
            CRouteBase *pSelect = m_apSelect[i];
            if (pSelect != 0) {
                CEventModel *pArrow = pMan->m_apObjModel[195 + i];
                pSelect->SetSelect(pArrow, i == m_nSelect);
                pArrow->SetDisp(1);
                pArrow->SetShadowDisp(bShadow);
                pArrow->SetFade(nMode, 0.0f, 0.0f);
                pArrow->SetFade(nMode, 1.0f, 1.0f / 15.0f);
            }
        }
    }

    CTournamentStep::CTournamentStep(bcs::CTask *pParent) : CTmScriptHandler(pParent)
    {
        m_pScript = 0;
        m_pExit = 0;
        m_pDemo = 0;
        m_Unk8C = 0;
        m_fTimer = 0.0f;
        m_pTimerPool = 0;
        m_pTimerFree = 0;
        m_pTimerList = 0;
        m_pTimerPush = 0;
        m_nSubStep = 0;
        m_ulMoveMask = 0;
        m_UnkB4 = 0;
        m_bFadeInReq = 0;
        m_nEndParam = 0;
        m_UnkC5 = 0;
        m_UnkC6 = 0;
        m_UnkC8 = 0;
        m_UnkC9 = 0;
        m_UnkDA = 0;
        m_nTrophy = 0;
        m_UnkDC = 0;
        _InitStep();
        _CreateTimer(0x40);
        bcs::CMainApp::SetUnivSpeed(1.0f);
        CTournament::SetExPauseFlag(0);
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        CTmGameHandler::_SetBattleWin(1);
        info.nUnkCA1 = (CGame::g_GameWork.m_nUnk40 >> 8) & 1;
        info.nUnkCA6 = CGame::g_GameWork.m_nUnk1C;
        info.aTeamChrChangeCtrlType[0] = CGame::GetTeamChrChangeCtrlType(0);
        info.aTeamChrChangeCtrlType[1] = CGame::GetTeamChrChangeCtrlType(1);
        info.aTeamAutoSpikeJumpType[0] = CGame::GetTeamAutoSpikeJumpType(0);
        info.aTeamAutoSpikeJumpType[1] = CGame::GetTeamAutoSpikeJumpType(1);
    }

    CTournamentStep::~CTournamentStep()
    {
        _CreateTimer(0);
        CTournament::SetExPauseFlag(0);
        CNameMesg::Remove();
        CCockpit::Remove();
        CEffectCtrl::Dispose();
        CEffectCtrl::DetachResource();
        CEffectCtrl::Close();
        menu::CSndEff::GetInstance()->Finalize();
        menu::CVibEff::GetInstance()->Finalize();
        CLytCtrl::Dispose();
        CLytCtrl::DetachResource();
        CLytCtrl::Close();
        CGameAccInput::Finalize();
        CNowLoading::SetDisp(0);
        if (m_pScript != 0) {
            delete m_pScript;
        }
        m_pScript = 0;
        if (m_pExit != 0) {
            m_pExit->Delete();
        }
        m_pExit = 0;
        if (m_pDemo != 0) {
            m_pDemo->Delete();
        }
        m_pDemo = 0;
        EvDemoVs::CopyRes();
        CLoadData::UnLoad();
    }

    void CTournamentStep::ChangeStateProc()
    {
        m_fStepTime = 0.0f;
        m_nSubStep = 0;
    }

    inline void CTournamentStep::_SetTimerOwner()
    {
        m_nTimerState = m_nState;
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        m_nTimerTeam = info.aTurnTeam[info.nTurnIndex];
    }

    void CTournamentStep::Main()
    {
        float fFrame = GetFrameRateF();
        m_fStepTime += fFrame;
        if (bcs::CMainApp::IsPause(1)) {
            ExecState();
            return;
        }
        m_fUnkB8 -= fFrame;
        if (m_fUnkB8 < 0.0f) {
            m_fUnkB8 = 0.0f;
        }
        _SetTimerOwner();
        ExecState();
        _CheckTimer(fFrame);
        if (CTournamentRouteMan::m_pInstance != 0) {
            CTournamentRouteMan::m_pInstance->_UpdateFrame(fFrame);
        }
        CTournamentMan *pMan = CTournamentMan::GetPtr();
        if (pMan != 0) {
            pMan->UpdateFrame(fFrame);
        }
        long nBgm = GetBgmTask() == 0 ? s_nBgmReq[0] : -1;
        if (nBgm != -1) {
            PlayBgm(nBgm);
        }
    }

    void CTournamentStep::Disp() {}

    void CTournamentStep::_MakeRouteSelect(long nTeam)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
        m_nRouteNum0 =
            pMan->MakeRouteSelect(m_aRoute0, team.nPoint, team.nTargetPoint, GetRouteMask(team), 0, 0.7853982f);
    }

    void CTournamentStep::_MakeRouteSelect(long nTeam0, long nTeam1)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        const TournamentWork::TEAM_INFO &team0 = TournamentWork::GetTeamInfo(nTeam0);
        const TournamentWork::TEAM_INFO &team1 = TournamentWork::GetTeamInfo(nTeam1);
        m_nRouteNum0 =
            pMan->MakeRouteSelect(m_aRoute0, team0.nPoint, team0.nTargetPoint, GetRouteMask(team0), 0, 0.7853982f);
        m_nRouteNum1 =
            pMan->MakeRouteSelect(m_aRoute1, team1.nPoint, team1.nTargetPoint, GetRouteMask(team1), 0, 0.7853982f);
    }

    void CTournamentStep::_Backup()
    {
        TournamentWork::Backup();
        m_pScript->_Setup("FlagsBackup", 1, 0, 0, 0, 0);
    }

    void CTournamentStep::_Restore()
    {
        TournamentWork::Restore();
        m_pScript->_Setup("FlagsRestore", 1, 1, 0, 0, 0);
    }

    void CTournamentStep::_SetReslutMoveRoute(long nTeam, int bWin, int bAnim)
    {
        if (nTeam < 16) {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
            CTournamentRouteMan::GetInstance()->SetWinRoute(team.aRoute, 4, bWin, bAnim);
        }
    }

    void CTournamentStep::_SetupGame(long nMatch, int b)
    {
        TournamentRule::SetupGame(m_pScript, m_UnkD0, m_nMoveTeam0, m_nMoveTeam1, nMatch, b);
    }

    int CTournamentStep::_IsStarCupDemo(long nMatch)
    {
        const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(nMatch);
        if ((match.nSideFlag0D == TournamentWork::GetTournamentInfo().GetPlayerTeam() ||
             match.nSideFlag0E == TournamentWork::GetTournamentInfo().GetPlayerTeam()) &&
            CTournament::GetCup() == 3 && match.nPoint == 0x14) {
            return 1;
        }
        return 0;
    }

    float CTournamentStep::_MoveSetFrame(unsigned long ulMask)
    {
        float fMin;
        float fMax;
        CTournamentMan::GetInstance()->GetChrMoveFrame(fMin, fMax, m_ulMoveMask);
        CTournamentMan::m_pInstance->m_ulMoveStop &= ~m_ulMoveMask;
        return fMax;
    }

    // Takes team nTeam off the map and clears its members.
    static inline void ClearTeam(long nTeam)
    {
        if (nTeam != -1) {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
            if (team.nPoint != 0xFF) {
                team.nPoint = 0xFF;
                for (long i = 0; i < 3; i++) {
                    if (team.aCharType[i] != -1) {
                        team.aCharType[i] = -1;
                        team.aCharFlag[i] = 0;
                    }
                }
            }
        }
    }

    void CTournamentStep::_CleanAfterErase()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        for (long i = 0; i < 16; i++) {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(i);
            if (team.nPoint != 0xFF) {
                for (long j = 0; j < 3; j++) {
                    if (team.aCharFlag[j] & 0x10) {
                        pMan->CleanChrModel(i, 1);
                        ClearTeam(i);
                        break;
                    }
                }
            }
        }
        for (long i = 0; i < 16; i++) {
            TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(i);
            if (match.nState != 0 && (match.nState & 0x40)) {
                match.nPoint = 0xFF;
                match.aTeamPoint[0] = 0xFF;
                match.aTeamPoint[1] = 0xFF;
                match.m_Unk03[0] = 0xFF;
                match.m_Unk03[1] = 0xFF;
                match.m_Unk03[2] = 0xFF;
                match.nSideFlag0D = -1;
                match.nSideFlag0E = -1;
                match.nWinTeam = -1;
                match.nState = 0;
                match.nStage = -1;
                match.nComLevel = 0;
                match.nFlags06 = 0;
                match.nRound = 0;
                match.nRule = 0;
            }
        }
    }

    void CTournamentStep::_SetupTeamPos(int nMot)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        for (long i = 0; i < 16; i++) {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(i);
            if (team.nPoint != 0xFF) {
                CEventModel *pModel = pMan->GetChr(i);
                if (pModel != 0) {
                    bcs::FVEC3 vPos;
                    TournamentWork::GetPointViewPos(vPos, team.nPoint);
                    vPos.x += team.fUnk00[0];
                    vPos.y += team.fUnk00[1];
                    vPos.z += team.fUnk00[2];
                    pModel->SetPosition(vPos);
                    pModel->SetRotation(bcs::FVEC3(0.0f, team.fUnk00[3], 0.0f));
                    pMan->InitMoveRoute(i);
                    pModel->SetAnimSpeed(1.0f);
                    if (nMot != 0) {
                        pModel->AnimPlay(2, 2, 8.0f, 0.0f, nMot);
                    }
                    pModel->AnimPlay(1, 2, 8.0f, 0.0f, 1);
                    if (team.nFlag22 & 4) {
                        pModel->SetDisp(0);
                    }
                    if (team.nFlag22 & 2) {
                        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
                        pRouteMan->_AddTimer(0x3F5, 0x3F5, pRouteMan->m_fTimer + 0.0f, i, 0.0f, i);
                    }
                }
            }
        }
    }

    void CTournamentStep::_UpdeteOffsetPos()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        for (long i = 0; i < 16; i++) {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(i);
            if (team.nPoint != 0xFF) {
                CEventModel *pModel = pMan->GetChr(i);
                if (pModel != 0) {
                    bcs::FVEC3 vPos;
                    TournamentWork::GetPointViewPos(vPos, team.nPoint);
                    bcs::FVEC3 vOffset;
                    vOffset.x = pModel->GetPos().x - vPos.x;
                    vOffset.y = pModel->GetPos().y - vPos.y;
                    vOffset.z = pModel->GetPos().z - vPos.z;
                    team.fUnk00[0] = vOffset.x;
                    team.fUnk00[1] = vOffset.y;
                    team.fUnk00[2] = vOffset.z;
                    if (i == 0) {
                        TournamentWork::TournamentInfo().nMarkerFlag = GetPointMarkerFlag(team.nPoint);
                        TournamentWork::TournamentInfo().fMarkerY = 0.0025f;
                    }
                    bcs::FVEC3 vRot = pModel->GetRot();
                    team.fUnk00[3] = vRot.y;
                }
            }
        }
    }

    void CTournamentStep::_CreateTimer(long nNum)
    {
        if (m_pTimerPool != 0) {
            delete[] m_pTimerPool;
        }
        m_pTimerPool = 0;
        m_pTimerList = 0;
        m_pTimerFree = 0;
        m_pTimerPush = 0;
        m_fTimer = 0.0f;
        if (nNum > 0) {
            m_pTimerPool = new TIMER[nNum];
            m_pTimerFree = &m_pTimerPool[nNum - 1];
            for (long i = nNum - 1; i > 0; i--) {
                m_pTimerPool[i].pNext = &m_pTimerPool[i - 1];
            }
            m_pTimerPool[0].pNext = 0;
        }
    }

    void CTournamentStep::_AddTimer(short nEvent, float fTime, short nParam, unsigned long ulParam)
    {
        TIMER *pTimer = m_pTimerFree;
        TIMER *pPrev = 0;
        pTimer->fTime = fTime;
        pTimer->nEvent = nEvent;
        pTimer->nParam = nParam;
        pTimer->ulParam = ulParam;
        pTimer->nPushId = -1;
        pTimer->nState = m_nTimerState;
        pTimer->nTeam = m_nTimerTeam;
        m_pTimerFree = pTimer->pNext;

        // In order of time, after the timers of the same time.
        TIMER *p = m_pTimerList;
        while (p != 0) {
            if (p->fTime > fTime) {
                break;
            }
            pPrev = p;
            p = p->pNext;
        }
        if (pPrev != 0) {
            pTimer->pNext = pPrev->pNext;
            pPrev->pNext = pTimer;
        } else {
            pTimer->pNext = m_pTimerList;
            m_pTimerList = pTimer;
        }
    }

    void CTournamentStep::_CheckTimer(float fFrame)
    {
        if (m_pTimerList != 0) {
            m_fTimer += fFrame;
            do {
                TIMER *pTimer = m_pTimerList;
                float fOver = pTimer->fTime - m_fTimer;
                if (fOver > 0.0f) {
                    break;
                }
                m_nTimerState = pTimer->nState;
                m_nTimerTeam = pTimer->nTeam;
                m_fTimerOver = fOver;
                _TimerEvent(pTimer->nState, pTimer->nEvent, pTimer->nParam, pTimer->ulParam, 0);
                m_pTimerList = pTimer->pNext;
                pTimer->pNext = m_pTimerFree;
                m_pTimerFree = pTimer;
            } while (m_pTimerList != 0);
        }
    }

    // The same as the route manager's PushTimer (inline).
    void CTournamentStep::_PushTimer(short nId, short nEvent, float fTime, short nParam, unsigned long ulParam)
    {
        TIMER *pTimer = m_pTimerFree;
        pTimer->fTime = fTime;
        pTimer->nEvent = nEvent;
        pTimer->nParam = nParam;
        pTimer->ulParam = ulParam;
        pTimer->nPushId = nId;
        pTimer->nState = -1;
        pTimer->nTeam = -1;
        m_pTimerFree = pTimer->pNext;
        pTimer->pNext = m_pTimerPush;
        m_pTimerPush = pTimer;
    }

    void CTournamentStep::_PopTimer(short nId, float fTime)
    {
        TIMER *pTimer = m_pTimerPush;
        m_pTimerPush = 0;
        while (pTimer != 0) {
            TIMER *pFree = pTimer;
            if (nId == -1 || pTimer->nPushId == nId) {
                _AddTimer(pTimer->nEvent, fTime + pTimer->fTime, pTimer->nParam, pTimer->ulParam);
            } else {
                _PushTimer(pTimer->nPushId, pTimer->nEvent, pTimer->fTime, pTimer->nParam, pTimer->ulParam);
            }
            pTimer = pTimer->pNext;
            pFree->pNext = m_pTimerFree;
            m_pTimerFree = pFree;
        }
    }

    void CTournamentStep::_ResetTimer()
    {
        TIMER *pTimer = m_pTimerList;
        if (pTimer == 0) {
            return;
        }

        // Run every queued timer now.
        do {
            m_nTimerState = pTimer->nState;
            m_nTimerTeam = pTimer->nTeam;
            m_fTimerOver = 0.0f;
            _TimerEvent(pTimer->nState, pTimer->nEvent, pTimer->nParam, pTimer->ulParam, 1);
            m_pTimerList = pTimer->pNext;
            pTimer->pNext = m_pTimerFree;
            TIMER *pNext = m_pTimerList;
            m_pTimerFree = pTimer;
            pTimer = pNext;
        } while (pTimer != 0);
        _SetTimerOwner();
        m_pTimerList = 0;
        m_fTimer = 0.0f;
    }

    void CTournamentStep::_CleanPushTimer()
    {
        TIMER *pTimer = m_pTimerPush;
        if (pTimer == 0) {
            return;
        }

        // Run every pushed timer now.
        do {
            m_nTimerState = pTimer->nState;
            m_nTimerTeam = pTimer->nTeam;
            m_fTimerOver = 0.0f;
            _TimerEvent(pTimer->nState, pTimer->nEvent, pTimer->nParam, pTimer->ulParam, 1);
            m_pTimerPush = pTimer->pNext;
            pTimer->pNext = m_pTimerFree;
            TIMER *pNext = m_pTimerPush;
            m_pTimerFree = pTimer;
            pTimer = pNext;
        } while (pTimer != 0);
        _SetTimerOwner();
        m_pTimerList = 0;
    }

    int CTournamentStep::_IsTimerEvent() const
    {
        return m_pTimerList != 0;
    }

    int CTournamentStep::_IsTimerEvent(char nState) const
    {
        for (TIMER *p = m_pTimerList; p != 0; p = p->pNext) {
            if (nState == p->nState) {
                return 1;
            }
        }
        return 0;
    }

    int CTournamentStep::_IsTimerEvent(char nState, char nTeam) const
    {
        for (TIMER *p = m_pTimerList; p != 0; p = p->pNext) {
            if (nState == p->nState && nTeam == p->nTeam) {
                return 1;
            }
        }
        return 0;
    }

    int CTournamentStep::_IsTimerEventGroup(char nTeam) const
    {
        for (TIMER *p = m_pTimerList; p != 0; p = p->pNext) {
            if (nTeam == p->nTeam) {
                return 1;
            }
        }
        return 0;
    }

    void CTournamentStep::_StartFade(long nType, float fSpeed, int nFadeType)
    {
        bcs::CMainApp::Start3DFade(nType, fSpeed);
        bcs::CMainApp::StartMasterFade(nType, fSpeed);
        m_nFadeType = nFadeType;
    }

    void CTournamentStep::_StartFadeCross(float fSpeed)
    {
        bcs::CMainApp::StopCrossFade();
        bcs::CMainApp::StartCrossFade(fSpeed, 1, 0);
        m_nFadeType = 0;
    }

    void CTournamentStep::_Start3DFadeCross(float fSpeed)
    {
        bcs::CMainApp::Stop3DCrossFade();
        bcs::CMainApp::Start3DCrossFade(fSpeed, 1, 0);
        m_nFadeType = 0;
    }

    int CTournamentStep::_WaitFade() const
    {
        if (bcs::CMainApp::IsPlay3DFade()) {
            return 1;
        }
        if (bcs::CMainApp::IsPlayMasterFade()) {
            return 1;
        }
        if (bcs::CMainApp::IsPlayCrossFade()) {
            return 1;
        }
        return bcs::CMainApp::IsPlay3DCrossFade() != 0;
    }

    int CTournamentStep::_IsNext(int b)
    {
        int bNext = 0;
        if (bcs::CWifiPad::IsTrg(0x900, 0)) {
            bNext = 1;
        }
        return bNext;
    }

    void CTournamentStep::_SkipStateCockpit(long nSubStep)
    {
        if (CCockpit::GetResult() == 1000 || _IsNext(0)) {
            CCockpit::ClearResult(-1);
            m_nSubStep = nSubStep;
        }
    }

    void CTournamentStep::_SetTrophy(long nType)
    {
        static const char *_KinokoZoomName[] = {"t00_f_model_s", "t00_f_model_zoom", "t00_f_model_l"};
        static const char *_FlowerZoomName[] = {"t01_f_model_s", "t01_f_model_zoom", "t01_f_model_l"};
        static const char *_StarZoomName[] = {"t02_f_model_s", "t02_f_model_zoom", "t02_f_model_l"};
        if (nType == 1) {
            if (!m_nTrophy) {
                return;
            }
            m_nTrophy = 0;
        } else {
            m_nTrophy = nType;
        }
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pTrophy = pMan->GetObjModel(0);
        if (pTrophy != 0) {
            long nCup = CTournament::GetCup();
            switch (nCup) {
            case 1:
                pTrophy->AnimPlay(_KinokoZoomName[nType], 1, 0.0f, 0.0f, 1);
                break;
            case 2:
                pTrophy->AnimPlay(_FlowerZoomName[nType], 1, 0.0f, 0.0f, 1);
                break;
            case 3:
                pTrophy->AnimPlay(_StarZoomName[nType], 1, 0.0f, 0.0f, 1);
                break;
            }
        }
    }

    void CTournamentStep::_EndDemoStep(long nDemo)
    {
        TournamentWork::TournamentInfo().nDemoType = nDemo;
        CTournament::SetState(0x18);
        EvDemoVs::CopyResMode(0);
        CMainStep::SetNextStep(3);
        m_nSubStep = 99;
    }

    void CTournamentStep::_ExitStep(long nStep, int nParam)
    {
        if (Unlock::IsDisp(0)) {
            m_nEndParam = 0;
            EvDemoVs::CopyResMode(0);
            if (nStep == 2) {
                menu::CStateSave::GetInstance()->SetStartMenuMode(2);
            }
            Unlock::SetNextStep(nStep);
            return;
        }
        if (nStep == 2) {
            menu::CStateSave::GetInstance()->SetStartMenuMode(2);
        }
        m_nEndParam = nParam;
        EvDemoVs::CopyResMode(0);
        CMainStep::SetNextStep(nStep);
    }

    void CTournamentStep::_EndTutorialStep(long nStep, int nParam)
    {
        if (nStep == 2) {
            menu::CStateSave::GetInstance()->SetStartMenuMode(5);
        }
        m_nEndParam = nParam;
        EvDemoVs::CopyResMode(0);
        CMainStep::SetNextStep(nStep);
    }

    void CTournamentStep::_EndStep(long nStep, int nParam)
    {
        if (nStep == 2) {
            menu::CStateSave::GetInstance()->SetStartMenuMode(2);
        }
        m_nEndParam = nParam;
        EvDemoVs::CopyResMode(0);
        CMainStep::SetNextStep(nStep);
    }

    void CTournamentStep::_InitStep()
    {
        InitFsm(this, STATE_NUM);
        SetState(STATE_INITIALIZE, &CTournamentStep::CtorStateInitialize, &CTournamentStep::LoopStateInitialize,
                 &CTournamentStep::DtorStateInitialize);
        SetState(STATE_FINALIZE, &CTournamentStep::CtorStateFinalize, &CTournamentStep::LoopStateFinalize,
                 &CTournamentStep::DtorStateFinalize);
        SetState(STATE_LOADING, &CTournamentStep::CtorStateLoading, &CTournamentStep::LoopStateLoading,
                 &CTournamentStep::DtorStateLoading);
        SetState(STATE_LOAD_END, &CTournamentStep::CtorStateLoadEnd, &CTournamentStep::LoopStateLoadEnd,
                 &CTournamentStep::DtorStateLoadEnd);
        SetState(STATE_EVENT_SETUP, &CTournamentStep::CtorStateEventSetup, &CTournamentStep::LoopStateEventSetup,
                 &CTournamentStep::DtorStateEventSetup);
        SetState(STATE_EVENT_PLAY, &CTournamentStep::CtorStateEventPlay, &CTournamentStep::LoopStateEventPlay,
                 &CTournamentStep::DtorStateEventPlay);
        SetState(STATE_EVENT_END, &CTournamentStep::CtorStateEventEnd, &CTournamentStep::LoopStateEventEnd,
                 &CTournamentStep::DtorStateEventEnd);
        SetState(STATE_MOVE_SETUP, &CTournamentStep::CtorStateMoveSetup, &CTournamentStep::LoopStateMoveSetup,
                 &CTournamentStep::DtorStateMoveSetup);
        SetState(STATE_MOVE_PLAY, &CTournamentStep::CtorStateMovePlay, &CTournamentStep::LoopStateMovePlay,
                 &CTournamentStep::DtorStateMovePlay);
        SetState(STATE_MOVE_END, &CTournamentStep::CtorStateMoveEnd, &CTournamentStep::LoopStateMoveEnd,
                 &CTournamentStep::DtorStateMoveEnd);
        SetState(STATE_MOVE_RESULT, &CTournamentStep::CtorStateMoveResult, &CTournamentStep::LoopStateMoveResult,
                 &CTournamentStep::DtorStateMoveResult);
        SetState(STATE_NPC_CALL_MOVE, &CTournamentStep::CtorStateNpcCallMove, &CTournamentStep::LoopStateNpcCallMove,
                 &CTournamentStep::DtorStateNpcCallMove);
        SetState(STATE_NPC_CALL_PLAY, &CTournamentStep::CtorStateNpcCallPlay, &CTournamentStep::LoopStateNpcCallPlay,
                 &CTournamentStep::DtorStateNpcCallPlay);
        SetState(STATE_GAME_CALL_MOVE, &CTournamentStep::CtorStateGameCallMove,
                 &CTournamentStep::LoopStateGameCallMove, &CTournamentStep::DtorStateGameCallMove);
        SetState(STATE_GAME_CALL_PLAY, &CTournamentStep::CtorStateGameCallPlay,
                 &CTournamentStep::LoopStateGameCallPlay, &CTournamentStep::DtorStateGameCallPlay);
        SetState(STATE_GAME_CALL_END, &CTournamentStep::CtorStateGameCallEnd, &CTournamentStep::LoopStateGameCallEnd,
                 &CTournamentStep::DtorStateGameCallEnd);
        SetState(STATE_TURN_START, &CTournamentStep::CtorStateTurnStart, &CTournamentStep::LoopStateTurnStart,
                 &CTournamentStep::DtorStateTurnStart);
        SetState(STATE_TURN_END, &CTournamentStep::CtorStateTurnEnd, &CTournamentStep::LoopStateTurnEnd,
                 &CTournamentStep::DtorStateTurnEnd);
        SetState(STATE_GAME_SETUP, &CTournamentStep::CtorStateGameSetup, &CTournamentStep::LoopStateGameSetup,
                 &CTournamentStep::DtorStateGameSetup);
        SetState(STATE_GAME_START, &CTournamentStep::CtorStateGameStart, &CTournamentStep::LoopStateGameStart,
                 &CTournamentStep::DtorStateGameStart);
        SetState(STATE_GAME_VICTORY, &CTournamentStep::CtorStateGameVictory, &CTournamentStep::LoopStateGameVictory,
                 &CTournamentStep::DtorStateGameVictory);
        SetState(STATE_GAME_EXIT, &CTournamentStep::CtorStateGameExit, &CTournamentStep::LoopStateGameExit,
                 &CTournamentStep::DtorStateGameExit);
        SetState(STATE_GAME_SAVE, &CTournamentStep::CtorStateGameSave, &CTournamentStep::LoopStateGameSave,
                 &CTournamentStep::DtorStateGameSave);
        SetState(STATE_NEXT_CUP, &CTournamentStep::CtorStateNextCup, &CTournamentStep::LoopStateNextCup,
                 &CTournamentStep::DtorStateNextCup);
        SetState(STATE_DEMO_SETUP, &CTournamentStep::CtorStateDemoSetup, &CTournamentStep::LoopStateDemoSetup,
                 &CTournamentStep::DtorStateDemoSetup);
        SetState(STATE_DEMO_PLAY, &CTournamentStep::CtorStateDemoPlay, &CTournamentStep::LoopStateDemoPlay,
                 &CTournamentStep::DtorStateDemoPlay);
        SetState(STATE_DEMO_END, &CTournamentStep::CtorStateDemoEnd, &CTournamentStep::LoopStateDemoEnd,
                 &CTournamentStep::DtorStateDemoEnd);
        SetState(STATE_BOSS_CALL_PLAY, &CTournamentStep::CtorStateBossCallPlay,
                 &CTournamentStep::LoopStateBossCallPlay, &CTournamentStep::DtorStateBossCallPlay);
        SetState(STATE_BOSS_CALL_END, &CTournamentStep::CtorStateBossCallEnd, &CTournamentStep::LoopStateBossCallEnd,
                 &CTournamentStep::DtorStateBossCallEnd);
        ChangeState(STATE_INITIALIZE);
    }

    void CTournamentStep::_ChangeStateEvent(char nState)
    {
        if (nState == STATE_NPC_CALL_PLAY || nState == STATE_GAME_CALL_PLAY || nState == STATE_MOVE_RESULT ||
            nState == STATE_MOVE_SETUP) {
            TournamentWork::TournamentInfo().nEventNextState = nState;
            ChangeState(STATE_EVENT_SETUP);
        } else {
            ChangeState(nState);
        }
    }

    void CTournamentStep::_TimerEvent(char nState, char nEvent, short nParam, unsigned long ulParam, int bReset)
    {
        switch (nState) {
        case STATE_GAME_CALL_PLAY:
            TimeStateGameCallPlay(nEvent, nParam, ulParam, bReset);
            break;
        case STATE_GAME_CALL_MOVE:
            TimeStateGameCallMove(nEvent, nParam, ulParam, bReset);
            break;
        case STATE_NPC_CALL_PLAY:
            TimeStateNpcCallPlay(nEvent, nParam, ulParam, bReset);
            break;
        case STATE_NPC_CALL_MOVE:
            TimeStateNpcCallMove(nEvent, nParam, ulParam, bReset);
            break;
        case STATE_GAME_START:
            TimeStateGameStart(nEvent, nParam, ulParam, bReset);
            break;
        default:
            if (bReset) {
                _TimeEventResetDefaultProc(nEvent, nParam, ulParam);
            } else {
                _TimeEventDefaultProc(nEvent, nParam, ulParam);
            }
            break;
        }
    }
}

namespace msw {

    // The match team nTeam plays next, -1 for none; rOther receives the
    // other team of that match. (name guessed)
    static inline long FindTeamMatch(signed char nTeam, signed char &rOther)
    {
        if (nTeam != -1) {
            TournamentWork::GetTeamInfo(nTeam);
            for (long i = 1; i < 16; i++) {
                const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(i);
                if (match.nState != 0 && !(match.nState & 0x80)) {
                    if (match.nSideFlag0D == nTeam) {
                        rOther = match.nSideFlag0E;
                        return i;
                    }
                    if (match.nSideFlag0E == nTeam) {
                        rOther = match.nSideFlag0D;
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    // Bit 6 of the flags of the point match nMatch is played at (0 for no
    // match). (name guessed)
    static inline signed char GetMatchMoveFlag(long nMatch)
    {
        if (static_cast<signed char>(nMatch) != -1) {
            unsigned char nPoint = TournamentWork::GetMatchInfo(static_cast<signed char>(nMatch)).m_Unk03[0];
            if (nPoint != 0xFF) {
                return (TournamentWork::GetPointInfo(nPoint)->nFlag >> 6) & 1;
            }
        }
        return 0;
    }

    // Whether demo nDemo is one of the tutorials.
    static inline int IsTutorialDemo(short nDemo)
    {
        switch (nDemo) {
        case 1000:
        case 1001:
        case 1010:
        case 1011:
        case 1100:
        case 1101:
        case 1110:
        case 1111:
        case 1200:
        case 1201:
        case 1210:
        case 1211:
        case 1300:
        case 1301:
        case 1310:
        case 1311:
            return 1;
        }
        return 0;
    }

    void CTournamentStep::CtorStateInitialize()
    {
        EvDemoVs::Initialize();
        CNameMesg::Initialize();
        s_hBgm[0] = 0;
        s_nBgmReq[0] = -1;
        s_hBgm[1] = 0;
        s_nBgmReq[1] = -1;
        CGame::SetGameMode(static_cast<CGame::GAME_MODE>(CTournament::GetStageGameMode()));
        CGame::SetCup(static_cast<CGame::GAME_CUP>(CTournament::GetCup()));
        CGame::SetRound(0);
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        if (info.nUnk105A == 0) {
            info.nUnk105A = 0x12;
            info.nDemoType = -1;
            if (CTournament::GetCup() == 1 && !CTournament::IsMixTournamentGame()) {
                SAVE_DATA_PLAYER *pSave = CSave::GetPlayerData(-1);
                if (pSave != 0 && pSave->bFirstDemo == 0) {
                    pSave->bFirstDemo = 1;
                    info.nUnk105A = 0x18;
                    info.nDemoType = 10;
                }
            }
        } else if (info.nUnk105A == 0x1C) {
            info.nDemoType = 110;
        }
        if (info.nDemoType != -1) {
            m_nMoveTeam0 = 0;
            m_nMoveTeam1 = 8;
        } else {
            m_nMoveTeam0 = -1;
            m_nMoveTeam1 = -1;
        }
        CTournamentMan *pMan = CTournamentMan::CreateInstance(this, 0);
        if (info.nUnk105A != 0x1C && info.nUnk105A != 0x18) {
            return;
        }
        switch (info.nDemoType) {
        case 10:
            info.nStage = 0x22;
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(1));
            CGame::SetStage(info.nStage);
            CLoadData::LoadTournament(1);
            ChangeState(STATE_LOADING);
            m_nSubStep = 99;
            break;
        case 50:
        case 60:
        case 71:
            CTournament::SetCup(3);
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(CTournament::GetStageGameMode()));
            CGame::SetStage(info.nStage);
            CGame::SetCup(static_cast<CGame::GAME_CUP>(CTournament::GetCup()));
            break;
        case 80:
        case 81:
            info.nStage = 0x15;
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(2));
            CGame::SetStage(0x15);
            CLoadData::LoadTournament(1);
            CLoadData::Load(0x70);
            ChangeState(STATE_LOADING);
            m_nSubStep = 99;
            break;
        case 130:
        case 131:
            CTournament::SetCup(3);
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(CTournament::GetStageGameMode()));
            CGame::SetStage(info.nStage);
            CLoadData::LoadTournament(1);
            pMan->ScriptLoad(0x5E, 0);
            m_nSubStep = 1;
            break;
        case 140:
            info.nStage = 99;
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(2));
            CGame::SetStage(99);
            CLoadData::LoadTournament(1);
            pMan->ScriptLoad(0x5E, 0);
            m_nSubStep = 1;
            break;
        case 1000:
        case 1001:
        case 1010:
        case 1011:
        case 1100:
        case 1101:
        case 1110:
        case 1111:
        case 1200:
        case 1201:
        case 1210:
        case 1211:
        case 1300:
        case 1301:
        case 1310:
        case 1311:
            info.nStage = 0x13;
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(2));
            CGame::SetStage(0x13);
            CLoadData::LoadTournament(1);
            ChangeState(STATE_LOADING);
            m_nSubStep = 99;
            break;
        default:
            info.nStage = 0x15;
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(2));
            CGame::SetStage(0x15);
            CLoadData::LoadTournament(1);
            ChangeState(STATE_LOADING);
            m_nSubStep = 99;
            break;
        }
    }

    void CTournamentStep::LoopStateInitialize()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        switch (m_nSubStep) {
        case 0:
            CGame::SetGameMode(static_cast<CGame::GAME_MODE>(CTournament::GetStageGameMode()));
            CGame::SetStage(TournamentWork::GetTournamentInfo().nStage);
            CLoadData::LoadTournament(1);
            pMan->ScriptLoad(0x5E, 0);
            m_nSubStep++;
            break;
        case 1:
            if (CLoadData::IsComplete(0x5E)) {
                pMan->ScriptBindData(0x5E, 0);
                m_pScript = new CTmScript(0x400);
                m_pScript->m_pData = pMan->GetScript(0);
                m_pScript->UpdateBind();
                m_pScript->SetupInitial(this);
                m_nSubStep++;
            }
            break;
        case 2: {
            signed char nState = TournamentWork::GetTournamentInfo().nUnk105A;
            if (nState == 0x12) {
                if (m_pScript != 0) {
                    m_pScript->_Setup("FlagsRestore", 1, 1, 1, 0, 0);
                }
                ChangeState(TournamentWork::GetTournamentInfo().nUnk105A);
                m_nSubStep++;
            } else if (nState == 0x0E) {
                TournamentWork::Restore();
                m_pScript->_Setup("FlagsRestore", 1, 1, 0, 0, 0);
                m_bFadeInReq = 1;
                const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
                m_nMoveTeam0 = info.aTurnTeam[info.nTurnIndex];
                m_UnkD0 = FindTeamMatch(m_nMoveTeam0, m_nMoveTeam1);
                m_UnkC5 = GetMatchMoveFlag(m_UnkD0);
                m_UnkC6 = _IsStarCupDemo(m_UnkD0);
                ChangeState(STATE_LOADING);
                m_nSubStep++;
            } else if (nState == 0x0F) {
                if (!CTournament::IsCupClear(CTournament::GetCup(), CTournament::GetGameMode(), CTournament::GetLevel(),
                                             -1) &&
                    TournamentWork::GetTournamentInfo().aFlag[0] != 0) {
                    TournamentWork::TournamentInfo().aFlag[0] = 0;
                }
                TournamentWork::Restore();
                m_pScript->_Setup("FlagsRestore", 1, 1, 0, 0, 0);
                m_bFadeInReq = 1;
                const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
                m_nMoveTeam0 = info.aTurnTeam[info.nTurnIndex];
                m_UnkD0 = FindTeamMatch(m_nMoveTeam0, m_nMoveTeam1);
                m_UnkC5 = GetMatchMoveFlag(m_UnkD0);
                m_UnkC6 = 0;
                ChangeState(STATE_LOADING);
                m_nSubStep++;
            } else if (nState == 0x1B) {
                m_UnkD0 = -1;
                m_UnkC6 = 0;
                m_UnkC5 = 0;
                ChangeState(TournamentWork::GetTournamentInfo().nUnk105A);
                m_nSubStep++;
            } else if (nState == 0x10) {
                TournamentWork::Restore();
                m_pScript->_Setup("FlagsRestore", 1, 1, 0, 0, 0);
                m_bFadeInReq = 1;
                m_UnkD0 = -1;
                m_UnkC6 = 0;
                m_UnkC5 = 0;
                ChangeState(STATE_LOADING);
                m_nSubStep++;
            } else {
                ChangeState(STATE_LOADING);
                m_nSubStep++;
            }
            break;
        }
        }
    }

    void CTournamentStep::DtorStateInitialize() {}

    void CTournamentStep::CtorStateFinalize()
    {
        CTournament::SetExPauseFlag(0);
    }

    void CTournamentStep::LoopStateFinalize()
    {
        switch (m_nSubStep) {
        case 0:
        case 1:
        case 2:
            m_nSubStep++;
            break;
        case 3:
            _CreateTimer(0);
            DeleteAllChild();
            if (m_pScript != 0) {
                delete m_pScript;
            }
            m_pScript = 0;
        case 4:
            bcs::CRenderMan::ReqCleanUp();
        case 5:
        case 6:
            m_nSubStep++;
            break;
        case 7:
            if (m_nEndParam) {
                CTournament::ExitMode();
            }
            for (long i = 0; i < 4; i++) {
                if (CMenuPad::s_Instance.GetPadStyle(i) == 0) {
                    CGame::SetPadDefaultType(i, 1);
                } else {
                    CGame::SetPadDefaultType(i, 0);
                }
            }
            CGame::EnableRuleOptionFlag(0x100, TournamentWork::GetTournamentInfo().nUnkCA1);
            CGame::g_GameWork.m_nUnk1C = TournamentWork::GetTournamentInfo().nUnkCA6;
            CGame::SetTeamAutoSpikeJumpType(0, TournamentWork::GetTournamentInfo().aTeamAutoSpikeJumpType[0]);
            CGame::SetTeamAutoSpikeJumpType(1, TournamentWork::GetTournamentInfo().aTeamAutoSpikeJumpType[1]);
            CMainStep::ChangeStep(-1);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateFinalize() {}

    void CTournamentStep::CtorStateLoadEnd()
    {
        m_pExit = new CTournamentExit(this, 0x10);
        CGame::SetGameMode(static_cast<CGame::GAME_MODE>(CTournament::GetStageGameMode()));
        CGame::SetCup(static_cast<CGame::GAME_CUP>(CTournament::GetCup()));
        CGame::SetRound(0);
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        pMan->SetupDataLoadEnd(1);
        if (info.nUnk105A != 0x1C && info.nUnk105A != 0x18) {
            return;
        }
        if (IsTutorialDemo(info.nDemoType)) {
            m_UnkD8 = STATE_DEMO_SETUP;
            CTournament::SetState(0x16);
            m_nSubStep = 3;
            return;
        }
        if (info.nStage != 99) {
            pMan->SetupStageDemoModel(info.nStage);
        }
        const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(info.GetPlayerTeam());
        if (team.nPoint != 0xFF) {
            long nMax = CTournament::GetTeamMax();
            for (long i = 0; i < nMax; i++) {
                if (team.aCharType[i] != -1) {
                    pMan->SetupChrModel(i, team.aCharType[i], team.aCharFlag[i] & 3, 0, 0, i, 0);
                }
            }
        }
        CTournament::SetState(0x18);
        m_nSubStep = 3;
    }

    void CTournamentStep::LoopStateLoadEnd()
    {
        switch (m_nSubStep) {
        case 0: {
            const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
            CTournamentMan *pMan = CTournamentMan::GetInstance();
            pMan->SetSceneData(CLoadData::GetRes(0x60));
            pMan->SetupStageModel(info.nStage);
            pMan->SetupChrModelLeader(1);
            pMan->CreatePlayerMarker("maker", 0, 0, 0, 200);
            pMan->CreatePlayerMarker("maker", 0, 1, 0, 201);
            m_nSubStep++;
            break;
        }
        case 1:
            CTournamentRouteMan::GetInstance()->SetupRoute();
            m_nSubStep++;
            break;
        case 2:
            if (m_pScript != 0) {
                m_pScript->_Setup("RestoreMap", 1, 0, 0, 0, 0);
            }
            m_nSubStep++;
            break;
        case 3:
            if (TournamentWork::GetTournamentInfo().nUnk105A != 0x18) {
                CTournament::SetExPauseFlag(1);
            }
            _ChangeStateEvent(TournamentWork::GetTournamentInfo().nUnk105A);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateLoadEnd() {}

    void CTournamentStep::_UnLoadData()
    {
        EvDemoVs::CopyRes();
        CLoadData::UnLoad();
    }

    void CTournamentStep::CtorStateLoading()
    {
        CNowLoading::SetDisp(1);
        bcs::CLoadMan::ClearDvdReadSize();
        if (m_UnkC5) {
            CLoadData::Load(0x5D);
            const TournamentWork::TEAM_INFO &team =
                TournamentWork::GetTeamInfo(TournamentWork::GetTournamentInfo().GetPlayerTeam());
            if (team.nPoint != 0xFF) {
                long nMax = CTournament::GetTeamMax();
                for (long i = 0; i < nMax; i++) {
                    if (team.aCharType[i] != -1) {
                        CLoadData::LoadEventMotion(i + 0x67, team.aCharType[i], 0, 0);
                    }
                }
            }
        }
        if (m_UnkC6) {
            TournamentWork::GetMatchInfo(7);
            signed char nOther = m_nMoveTeam0 != 0 ? m_nMoveTeam0 : m_nMoveTeam1;
            signed char nChr0 = -1;
            signed char nChr1 = -1;
            const TournamentWork::TEAM_INFO &team0 =
                TournamentWork::GetTeamInfo(TournamentWork::GetTournamentInfo().GetPlayerTeam());
            if (team0.nPoint != 0xFF) {
                nChr0 = team0.aCharType[0];
            }
            if (nOther != -1) {
                const TournamentWork::TEAM_INFO &team1 = TournamentWork::GetTeamInfo(nOther);
                if (team1.nPoint != 0xFF) {
                    nChr1 = team1.aCharType[0];
                }
            }
            CLoadData::LoadEventData(0x61, "script", "demo070", -1, 0, -1);
            CLoadData::LoadEventData(0x62, "event/demos", "demo070", -1, 1, -1);
            if (nChr0 != -1) {
                CLoadData::LoadEventMotion(0x67, nChr0, 0, 0);
            }
            if (nChr1 != -1) {
                CLoadData::LoadEventMotion(0x68, nChr1, 0, 0);
            }
            CLoadData::LoadEventMotion(0x69, 0xE, 0, 0);
        }
        CLoadData::LoadTournament(0);
        m_UnkDC = 1;
    }

    void CTournamentStep::LoopStateLoading()
    {
        switch (m_nSubStep) {
        case 0:
            if (CLoadData::IsComplete(-1)) {
                m_nSubStep++;
            }
            break;
        case 1:
            CLoadData::InitData();
            if (CLoadData::IsLoaded(0x5C)) {
                EvDemo::InitArcResData(CLoadData::GetPtr(0x5C), 0);
            }
            m_nSubStep++;
            break;
        case 2:
            ChangeState(STATE_LOAD_END);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateLoading()
    {
        CNowLoading::SetDisp(0);
    }

    int CTournamentStep::_CheckEventEntry(long nTeam, long nPoint)
    {
        m_UnkC2 = 0;
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        for (long i = 0; i < info.nTurnIndex; i++) {
            if (nTeam == info.aTurnTeam[i]) {
                return 0;
            }
        }
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        unsigned short nRoute = TournamentWork::GetTeamInfo(nTeam).GetLastRoute();
        if (nRoute >= 0x100) {
            nRoute -= 0x100;
        }
        if (nTeam != 0) {
            return 0;
        }
        m_pScript->Reset();
        m_pScript->SetValue("gTargetPoint", nPoint);
        m_pScript->SetValue("gTargetPassRoute", nRoute);
        m_pScript->SetValue("gTargetTeam", 0);
        m_pScript->SetValue("gTargetState", info.nEventNextState);
        if (info.nTurnMode == 2) {
            if (info.nEventNextState != STATE_MOVE_SETUP) {
                return 0;
            }
            m_pScript->_SetCall("ReturnEntry", 0, 0, 0, 0);
        } else {
            m_pScript->SetCall("EventEntry", 0, 4, 1);
        }
        m_UnkC2 = m_pScript->CallEventEntry();
        return m_UnkC2 != 0;
    }

    void CTournamentStep::CtorStateEventSetup()
    {
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        signed char nTeam = info.aTurnTeam[info.nTurnIndex];
        m_UnkB4 = 0;
        m_nCameraBusy = 1;
        switch (TournamentWork::GetTournamentInfo().nEventNextState) {
        case STATE_MOVE_RESULT:
        case STATE_NPC_CALL_PLAY:
        case STATE_GAME_CALL_PLAY: {
            signed char nPoint;
            signed char nTeam0;
            signed char nTeam1;
            if (m_UnkD0 == -1) {
                nPoint = TournamentWork::TournamentInfo().aFlag[0x3C];
                nTeam0 = TournamentWork::TournamentInfo().aFlag[0x3B];
                nTeam1 = TournamentWork::TournamentInfo().aFlag[0x3A];
            } else {
                nTeam0 = m_nMoveTeam0;
                nTeam1 = m_nMoveTeam1;
                nPoint = TournamentWork::GetMatchInfo(m_UnkD0).nPoint;
            }
            if (nTeam != m_nMoveTeam0) {
                if (!_CheckEventEntry(nTeam1, nPoint)) {
                    _CheckEventEntry(nTeam0, nPoint);
                }
            } else {
                if (!_CheckEventEntry(nTeam0, nPoint)) {
                    _CheckEventEntry(nTeam1, nPoint);
                }
            }
            break;
        }
        case STATE_MOVE_SETUP: {
            int bNoMatch = 1;
            for (long i = 1; i < 16; i++) {
                TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(i);
                if (match.nState != 0 && !(match.nState & 0x80)) {
                    long nTeam0 = GetPointTeam(match.aTeamPoint[0]);
                    long nTeam1 = GetPointTeam(match.aTeamPoint[1]);
                    if (nTeam0 != -1 && nTeam1 != -1 && (nTeam0 == nTeam || nTeam1 == nTeam)) {
                        if (_CheckEventEntry(nTeam0, match.aTeamPoint[0])) {
                            bNoMatch = 0;
                            break;
                        }
                        if (_CheckEventEntry(nTeam1, match.aTeamPoint[1])) {
                            bNoMatch = 0;
                            break;
                        }
                    }
                }
            }
            if (bNoMatch) {
                _CheckEventEntry(nTeam, TournamentWork::GetTeamInfo(nTeam).nPoint);
            }
            break;
        }
        default:
            _CheckEventEntry(nTeam, TournamentWork::GetTeamInfo(nTeam).nPoint);
            break;
        }
    }

    void CTournamentStep::LoopStateEventSetup()
    {
        CTournamentMan::GetInstance();
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        switch (m_nSubStep) {
        case 0:
            if (m_UnkC2 < 2 || m_UnkC2 == 12) {
                pRouteMan->CleanPushTimer();
                pRouteMan->ResetTimer();
                m_nSubStep++;
                ChangeState(STATE_EVENT_END);
            } else {
                m_nSubStep = 10;
            }
            break;
        case 10:
            if (!CTournamentRouteMan::IsTimerEvent()) {
                const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
                if (!_IsTimerEventGroup(info.aTurnTeam[info.nTurnIndex])) {
                    m_nSubStep++;
                    break;
                }
            }
            _IsNext(0);
            break;
        case 11:
            m_nCameraBusy = 0;
            m_nSubStep = 12;
            break;
        case 12:
            pRouteMan->TimerEvent(0x3F6, 0, 0.0f, 0, 0);
            ChangeState(STATE_EVENT_PLAY);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateEventSetup() {}

    void CTournamentStep::CtorStateEventPlay()
    {
        CTournamentExit::m_bEnable = false;
    }

    void CTournamentStep::DtorStateEventPlay() {}

    void CTournamentStep::CtorStateEventEnd()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        pRouteMan->CleanPushTimer();
        pRouteMan->ResetTimer();
        _CleanPushTimer();
        m_nCameraBusy = 0;
        pMan->MakePointData(0);
    }

    void CTournamentStep::LoopStateEventEnd()
    {
        if (m_nSubStep == 0) {
            ChangeState(TournamentWork::GetTournamentInfo().nEventNextState);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateEventEnd() {}
}

namespace msw {

    // Stops the map's music, fading it out over fFade frames.
    static inline void StopBgmAll(float fFade)
    {
        for (long i = 0; i < 2; i++) {
            bcs::CSndTask *pTask = s_hBgm[i] == 0 ? 0 : static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(s_hBgm[i]));
            if (pTask != 0) {
                pTask->Stop(fFade);
            }
            s_hBgm[i] = 0;
            s_nBgmReq[i] = -1;
        }
    }

    // Points the map camera at the point team nTeam is moving to.
    static inline void SetCameraTeamTarget(signed char nTeam)
    {
        const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        pMan->SetCameraActive(0);
        pMan->SetCameraPoint(team.nTargetPoint, 0);
        pMan->SetCameraRate(-0.0001f, 0.94921875f, 0.0007f);
    }

    void CTournamentStep::CtorStateNpcCallMove() {}

    void CTournamentStep::LoopStateNpcCallMove()
    {
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        switch (m_nSubStep) {
        case 0:
            if (m_UnkDA) {
                SetCameraTeamTarget(m_nMoveTeam0);
                _SetTrophy(1);
            }
            m_nSubStep++;
            break;
        case 1:
            if (!m_UnkDA) {
                m_nSubStep = 2;
            } else {
                if (CTournamentMan::GetInstance()->IsCameraEnd()) {
                    m_nSubStep++;
                } else if (m_UnkCA && (m_UnkC8 || _IsNext(0))) {
                    m_UnkC8 = 1;
                    m_nSubStep++;
                }
            }
            break;
        case 2: {
            unsigned char nPoint = TournamentWork::GetMatchInfo(m_UnkD0).nPoint;
            if (!(TournamentWork::GetPointInfo(nPoint)->nFlag & 0x20)) {
                float fMin;
                float fMax;
                CTournamentMan::GetInstance()->GetChrMoveFrame(fMin, fMax, m_ulMoveMask);
                CTournamentMan::m_pInstance->m_ulMoveStop &= ~m_ulMoveMask;
                _AddTimerRel(1, fMax + 5.0f, m_UnkD0, 0);
                _AddTimerRel(0, fMax, m_UnkD0, 0);
                m_fUnkB8 = fMax;
            } else {
                m_fUnkB8 = 0.0f;
            }
            m_fStepTime = 0.0f;
            m_nSubStep++;
            break;
        }
        case 3:
            if (!_IsTimerEvent(STATE_NPC_CALL_MOVE, info.aTurnTeam[info.nTurnIndex])) {
                m_fStepTime = 0.0f;
                m_nSubStep++;
            } else if (m_fStepTime > 60.0f && m_UnkCA && (m_UnkC8 || _IsNext(0))) {
                m_UnkC8 = 1;
                m_nSubStep++;
            }
            break;
        case 4:
            m_UnkD4 = m_nMoveTeam0;
            TournamentWork::TournamentInfo().nEventNextState = STATE_NPC_CALL_PLAY;
            ChangeState(STATE_EVENT_SETUP);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateNpcCallMove() {}

    void CTournamentStep::CtorStateNpcCallPlay()
    {
        unsigned char nPoint = TournamentWork::GetMatchInfo(m_UnkD0).nPoint;
        if (!(TournamentWork::GetPointInfo(nPoint)->nFlag & 0x20)) {
            _AddTimerRel(0, 30.0f + m_fUnkB8, m_UnkD0, 0);
            m_fUnkB8 += 140.0f;
        }
        _StartFade(1, 1.0f / 60.0f, 1);
        m_nSubStep = 50;
    }

    void CTournamentStep::LoopStateNpcCallPlay()
    {
        switch (m_nSubStep) {
        case 0: {
            const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
            if (!_IsTimerEvent(STATE_NPC_CALL_PLAY, info.aTurnTeam[info.nTurnIndex])) {
                m_fStepTime = 0.0f;
                m_nSubStep++;
            } else if (m_UnkCA && (m_UnkC8 || _IsNext(0))) {
                m_UnkC8 = 1;
                m_nSubStep = 50;
            }
            break;
        }
        case 1:
            if (m_fStepTime > 0.0f) {
                m_nSubStep = 50;
            } else if (m_UnkCA && (m_UnkC8 || _IsNext(0))) {
                m_UnkC8 = 1;
                m_nSubStep = 50;
            }
            break;
        case 50:
            TournamentWork::TournamentInfo().nEventNextState = STATE_MOVE_RESULT;
            ChangeState(STATE_EVENT_SETUP);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateNpcCallPlay() {}

    void CTournamentStep::CtorStateGameCallMove() {}

    void CTournamentStep::LoopStateGameCallMove()
    {
        if (_WaitFade()) {
            return;
        }
        CTournamentMan::GetInstance();
        switch (m_nSubStep) {
        case 0:
            m_nSubStep = 1;
            break;
        case 1:
            if (m_UnkDA) {
                SetCameraTeamTarget(m_nMoveTeam0);
                _SetTrophy(1);
            }
            m_nSubStep++;
            break;
        case 2:
            if (!m_UnkDA) {
                m_nSubStep = 3;
            } else {
                if (CTournamentMan::GetInstance()->IsCameraEnd()) {
                    m_nSubStep++;
                } else if (m_UnkCA && (m_UnkC8 || _IsNext(0))) {
                    m_UnkC8 = 1;
                    m_nSubStep++;
                }
            }
            break;
        case 3: {
            unsigned char nPoint = TournamentWork::GetMatchInfo(m_UnkD0).nPoint;
            if (!(TournamentWork::GetPointInfo(nPoint)->nFlag & 0x20)) {
                float fMin;
                float fMax;
                CTournamentMan::GetInstance()->GetChrMoveFrame(fMin, fMax, m_ulMoveMask);
                CTournamentMan::m_pInstance->m_ulMoveStop &= ~m_ulMoveMask;
                _AddTimerRel(0, 10.0f, m_UnkD0, 0);
                _AddTimerRel(1, fMax + 15.0f, m_UnkD0, 0);
            }
            m_nSubStep++;
            break;
        }
        case 4: {
            const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
            if (!_IsTimerEvent(STATE_GAME_CALL_MOVE, info.aTurnTeam[info.nTurnIndex])) {
                m_nSubStep++;
            } else {
                _IsNext(0);
            }
            break;
        }
        case 5:
            m_nSubStep = 6;
        case 6:
            TournamentWork::TournamentInfo().nEventNextState = STATE_GAME_CALL_PLAY;
            ChangeState(STATE_EVENT_SETUP);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateGameCallMove() {}

    void CTournamentStep::DtorStateGameCallPlay() {}

    void CTournamentStep::TimeStateGameCallPlay(char nEvent, short nParam, unsigned long ulParam, int bReset)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        switch (nEvent) {
        case 0:
            if (!bReset) {
                pMan->PlaySe(0xE1D, static_cast<const bcs::FVEC3 *>(0), 4, 0.0f, 1.0f, 0.0f, 1);
                pMan->SetEffect(0x1E7, 0x552, 4, 0, 0);
            }
            break;
        case 2:
        case 1:
            if (!bReset) {
                _StartFade(1, 1.0f / 30.0f, 1);
                StopBgmAll(30.0f);
                pMan->StopSe(30.0f, 0, 0x131);
            }
            break;
        default:
            if (bReset) {
                _TimeEventResetDefaultProc(nEvent, nParam, ulParam);
            } else {
                _TimeEventDefaultProc(nEvent, nParam, ulParam);
            }
            break;
        }
    }

    // Shows the player markers and the shadow nFlag asks for (see
    // TOURNAMENT_INFO::nMarkerFlag).
    static inline void SetMarkerDisp(CTournamentMan *pMan, unsigned char nFlag)
    {
        long bMarker0 = nFlag & 1;
        pMan->PlayerMarkerDisp(bMarker0 != 0, 200, 0);
        long bMarker1 = nFlag & 2;
        pMan->PlayerMarkerDisp(bMarker1 != 0, 201, 0);
        pMan->PlayerShadowDisp((nFlag >> 2) & 1, 0, 0);
        if (bMarker0 != 0) {
            pMan->PlayerMarkerSetPosY(TournamentWork::GetTournamentInfo().fMarkerY, 200);
        }
        if (bMarker1 != 0) {
            pMan->PlayerMarkerSetPosY(TournamentWork::GetTournamentInfo().fMarkerY, 201);
        }
    }

    void CTournamentStep::CtorStateGameCallEnd()
    {
        CTournament::SetState(0xF);
        TournamentWork::TournamentSave();
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        m_fUnkB8 = 10.0f;
        pMan->MakePointData(0);
        _SetupTeamPos(1);
        SetMarkerDisp(pMan, TournamentWork::GetTournamentInfo().nMarkerFlag);
        unsigned char nPoint = TournamentWork::GetTeamInfo(m_nMoveTeam0).nTargetPoint;
        if (!(TournamentWork::GetPointInfo(nPoint)->nFlag & 0x20)) {
            pMan->SetCameraActive(0);
            pMan->SetCameraPoint(nPoint, 0);
            pMan->SetCameraRate(-0.0001f, 0.9765625f, 0.0007f);
        }
        _SetTrophy(0);
        pMan->AttachEffectModel(0x1E6, pMan->GetObjModel(0), 0x54B, 3, 0, 0, 1, 0, 0);
    }

    void CTournamentStep::LoopStateGameCallEnd()
    {
        if (m_nSubStep == 0) {
            TournamentWork::TournamentInfo().nEventNextState = STATE_MOVE_RESULT;
            ChangeState(STATE_EVENT_SETUP);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateGameCallEnd() {}

    void CTournamentStep::CtorStateMoveResult()
    {
        if (m_bFadeInReq &&
            (TournamentWork::GetTournamentInfo().nTurnMode != 2 || m_UnkD0 == -1 ||
             !(TournamentWork::GetMatchInfo(m_UnkD0).nState & 0x20))) {
            CTournamentRouteMan::GetInstance()->StageDispSe(1);
            _StartFade(4, 1.0f / 60.0f, 0);
            m_bFadeInReq = 0;
            CTournamentExit::m_bEnable = true;
            TournamentWork::BgmPlay();
        }
    }

    void CTournamentStep::DtorStateMoveResult() {}

    void CTournamentStep::DtorStateMoveSetup() {}

    void CTournamentStep::CtorStateMovePlay() {}

    void CTournamentStep::LoopStateMovePlay()
    {
        if (m_nSubStep == 0) {
            ChangeState(STATE_MOVE_END);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateMovePlay() {}

    void CTournamentStep::CtorStateMoveEnd() {}

    void CTournamentStep::DtorStateMoveEnd() {}

    void CTournamentStep::DtorStateTurnStart() {}

    void CTournamentStep::CtorStateTurnEnd() {}

    void CTournamentStep::LoopStateTurnEnd()
    {
        if (m_nSubStep == 0) {
            m_UnkCC = 1;
            ChangeState(STATE_TURN_START);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateTurnEnd() {}
}

namespace msw {

    // The order the teams move in turn mode 1: by their m_Unk21, then by
    // the x of their map point.
    long SortCmpParamTurn(const char *pA, const char *pB)
    {
        const TournamentWork::TEAM_INFO &teamA = TournamentWork::GetTeamInfo(*pA);
        const TournamentWork::TEAM_INFO &teamB = TournamentWork::GetTeamInfo(*pB);
        if (static_cast<signed char>(teamA.m_Unk21) < static_cast<signed char>(teamB.m_Unk21)) {
            return 1;
        }
        if (static_cast<signed char>(teamB.m_Unk21) == static_cast<signed char>(teamA.m_Unk21)) {
            const TournamentWork::POINT_INFO *pPointA = TournamentWork::GetPointInfo(teamA.nPoint);
            const TournamentWork::POINT_INFO *pPointB = TournamentWork::GetPointInfo(teamB.nPoint);
            if (pPointB->nViewX < pPointA->nViewX) {
                return 1;
            }
        }
        return 0;
    }

    void CTournamentStep::_LoadVictoryMotion(long nTeam)
    {
        const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
        if (team.nPoint != 0xFF) {
            long nMax = CTournament::GetTeamMax();
            for (long i = 0; i < nMax; i++) {
                if (team.aCharType[i] != -1) {
                    CLoadData::LoadEventMotion(i + 0x67, team.aCharType[i], 0, 0);
                }
            }
        }
    }

    void CTournamentStep::CtorStateGameVictory()
    {
        TournamentWork::GetTournamentInfo();
        m_UnkD3 = 0;
    }

    void CTournamentStep::DtorStateGameStart() {}

    void CTournamentStep::DtorStateGameVictory() {}

    void CTournamentStep::_SetupVictoryUnlock(int bClearDemo)
    {
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        CUnlockFlags::m_nGameMode = CTournament::GetGameMode();
        long nLevel = CTournament::GetLevel();
        long nGameMode = CTournament::GetGameModeNoMix();
        long nId = nLevel + (CTournament::GetCup() - 1) * 4;
        long nCup = CTournament::GetCup();
        if (nCup == 1) {
            if (!flags.IsChar(0xC, 0, nGameMode)) {
                flags.PushChar(nId + 4, 0, 0, 0xC, 0, nGameMode);
            }
        } else if (nCup == 3 && nLevel == 0 && !flags.IsCup(1, 1, nGameMode)) {
            flags.PushCup(nId + 4, 1, 1, nGameMode);
        }
        if (bClearDemo) {
            SAVE_DATA_PLAYER *pSave = CSave::GetPlayerData(-1);
            if (pSave != 0) {
                pSave->bFirstDemo = 0;
            }
        }
    }

    void CTournamentStep::_SetupStarUnlock()
    {
        // The characters the star cup opens, in the order they are tried
        // (type, colour).
        static const long g_sStarUnlock[6][2] = {{0xE, 0}, {0xF, 0}, {0x11, 0}, {0xE, 1}, {0xF, 1}, {0x11, 1}};
        if (TournamentWork::GetTournamentInfo().aFlag[0x3E] != 0) {
            TournamentWork::TournamentInfo().aFlag[0x3E] = 0;
            CUnlockFlags flags;
            flags.SetPlayerData(-1);
            for (long i = 0; i < 6; i++) {
                long nType = g_sStarUnlock[i][0];
                long nColor = g_sStarUnlock[i][1];
                long nPushed = 0;
                for (long nMode = 0; nMode < 4; nMode++) {
                    if (!flags.IsChar(nType, nColor, nMode)) {
                        CUnlockFlags::m_nGameMode = nMode;
                        flags.PushChar(0x15, 0, 0, nType, nColor, nMode);
                        nPushed++;
                    }
                }
                if (nPushed > 0) {
                    return;
                }
            }
        }
    }

    void CTournamentStep::_PopUnlock() {}

    void CTournamentStep::DtorStateNextCup() {}

    void CTournamentStep::CtorStateGameSetup()
    {
        long nPoint = bcs::BCS_RandMax(7L);
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        info.m_Unk1058 = 2;
        info.nUnk105A = 0x13;
        _SetupDefalutParam(nPoint + 1, info.nPlayerMax, CTournament::GetGameMode());
        m_UnkCE = nPoint + 1;
        m_UnkCC = 0;
    }

    void CTournamentStep::LoopStateGameSetup()
    {
        if (m_nSubStep == 0) {
            ChangeState(STATE_LOADING);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateGameSetup() {}

    void CTournamentStep::CtorStateGameSave()
    {
        if (!CSave::IsUseSave()) {
            m_nSubStep = 99;
        }
    }

    void CTournamentStep::LoopStateGameSave()
    {
        switch (m_nSubStep) {
        case 0:
            if (CSave::CanbeWrite()) {
                CSave::WriteAsync();
                m_nSubStep++;
            }
            break;
        case 1:
            if (CSave::IsComplete()) {
                m_nSubStep = 99;
            }
            break;
        case 99:
            ChangeState(m_UnkD8);
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateGameSave() {}

    void CTournamentStep::CtorStateGameExit() {}

    void CTournamentStep::DtorStateGameExit() {}

    void CTournamentStep::LoopStateDemoSetup()
    {
        if (m_nSubStep == 0) {
            ChangeState(STATE_DEMO_PLAY);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateDemoSetup() {}

    void CTournamentStep::CtorStateDemoPlay() {}

    void CTournamentStep::LoopStateDemoPlay()
    {
        switch (m_nSubStep) {
        case 0:
            if (CEventDemo::m_Work.bSkip) {
                ChangeState(STATE_DEMO_END);
                m_nSubStep++;
            }
            break;
        case 10:
            m_nSubStep = 0;
            break;
        }
    }

    void CTournamentStep::DtorStateDemoPlay() {}

    void CTournamentStep::DtorStateDemoEnd() {}

    void CTournamentStep::CtorStateBossCallPlay()
    {
        CTournament::SetState(0x1C);
        TournamentWork::GetTournamentInfo();
        CMainStep::SetNextStep(TournamentRule::SetupGameBoss(0));
    }

    void CTournamentStep::LoopStateBossCallPlay()
    {
        switch (m_nSubStep) {
        case 0:
            CLoadData::LoadTournament(1);
            CLoadData::Load(0x70);
            if (m_UnkDC == 0) {
                CLoadData::LoadTournament(0);
                m_UnkDC = 1;
            }
            m_nSubStep++;
            break;
        case 1:
            if (CLoadData::IsComplete(-1)) {
                m_nSubStep++;
            }
            break;
        case 2:
            CLoadData::InitData();
            if (CLoadData::IsLoaded(0x70)) {
                EvDemo::InitArcResData(CLoadData::GetPtr(0x70), 0);
            }
            EvDemoVs::CopyResMode(4);
            CGame::SetStage(0x14);
            ChangeState(STATE_FINALIZE);
            m_nEndParam = 0;
            m_nSubStep++;
            break;
        }
    }

    void CTournamentStep::DtorStateBossCallPlay() {}

    void CTournamentStep::CtorStateBossCallEnd() {}

    void CTournamentStep::LoopStateBossCallEnd()
    {
        if (m_nSubStep == 0) {
            ChangeState(STATE_DEMO_SETUP);
            m_nSubStep++;
        }
    }

    void CTournamentStep::DtorStateBossCallEnd() {}

    // Sends the winner of match nMatch to the point its route ends at.
    inline void SetWinPos(long nMatch, long nTeam)
    {
        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        team.nTargetPoint = TournamentWork::GetMatchInfo(nMatch).m_Unk03[0];
    }

    // Sends the loser of match nMatch back to the point it came from.
    inline void SetLosePos(long nMatch, long nTeam)
    {
        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(nMatch);
        if (match.nSideFlag0D == match.nWinTeam) {
            team.nTargetPoint = match.m_Unk03[2];
        } else {
            team.nTargetPoint = match.m_Unk03[1];
        }
    }
}

namespace msw {

    // The cup that comes after each cup, -1 for none.
    static const long s_aNextCup[] = {1, 2, 3, -1, -1};

    void CTournamentStep::_SetClear()
    {
        long nCup = CTournament::GetCup();
        long nLevel = CTournament::GetLevel();
        TournamentWork::TournamentInfo().aFirstFlag[(nCup - 1) * 0x20 + 1] |= 1 << nLevel;
        CTournament::SetCupClear(CTournament::GetCup(), CTournament::GetGameMode(), CTournament::GetLevel(), -1);
        if (!CTournament::IsMixTournamentGame()) {
            SAVE_DATA_PLAYER *pSave = CSave::GetPlayerData(-1);
            if (pSave != 0) {
                pSave->SetTournamentClearCount(CTournament::GetGameMode(), CTournament::GetCup() - 1);
                pSave->CheckUnLockXml(CTournament::GetGameMode());
            }
        }
    }

    void CTournamentStep::CtorStateNextCup()
    {
        _SetClear();
        _SetupVictoryUnlock(0);
        m_UnkC7 = TournamentWork::GetTournamentInfo().m_Unk1058;
        long nNext = s_aNextCup[CTournament::GetCup()];
        if (nNext == -1) {
            m_nSubStep = 2;
        } else {
            CUnlockFlags flags;
            flags.SetPlayerData(-1);
            CUnlockFlags::m_nGameMode = CTournament::GetGameMode();
            flags.SetCup(nNext, CTournament::GetLevel(), CTournament::GetGameMode(), 1);
        }
        CTournament::NextCup(nNext, 2);
    }

    void CTournamentStep::LoopStateGameExit()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        switch (m_nSubStep) {
        case 0:
            CTournamentExit::m_bEnable = false;
            _StartFade(0, 1.0f / 60.0f, 1);
            StopBgmAll(40.0f);
            pMan->StopSe(40.0f, 0, 0x131);
            m_nSubStep++;
            break;
        case 1:
            if (!_WaitFade()) {
                _ExitStep(TournamentWork::GetTournamentInfo().m_Unk1058, 1);
                ChangeState(STATE_FINALIZE);
                m_nSubStep++;
            }
            break;
        }
    }
}
