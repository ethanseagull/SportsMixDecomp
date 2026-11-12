// msw::CGame, the base of the per-sport game tasks, with its static game
// settings (g_GameWork) and the small tasks it creates (pause, hit stop,
// slow motion). Written in the original's function order; see
// notes/attempts/CGame.md for what is still missing.
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSpeedCtrl.hpp"
#include "game/msw/CGameBGM.hpp"
#include "game/msw/CGameGoalDemo.hpp"
#include "game/msw/CGameSaturatedLinework.hpp"
#include "game/msw/CGameScoredView.hpp"
#include "bcs/BCS_Rand.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGamePadCheck.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CSave.hpp"
#include "bcs/CCamera.hpp"
#include "game/msw/CGameCamera.hpp"
#include "bcs/CCameraMan.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMiiMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CPad.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CNowLoading.hpp"
#include "game/msw/CNameMesg.hpp"
#include "game/msw/CGameAccInput.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CSnd.hpp"
#include "bcs/dwc/CDwc.hpp"
#include "bcs/CWifiPad.hpp"
#include "bcs/CPadWI.hpp"
#include "game/msw/dwc/CGameDwc.hpp"
#include "game/msw/CShadowTexMan.hpp"
#include <string.h>

// Functions of the DOL that have no name yet (declared with their symbol
// names until their classes are known).
extern "C" {
    void fn_8018E068();                       // loads the game's resources
    int fn_8017AB9C();                        // CTournament: the final match is being played
    void fn_80137964(long nTeam, long nCoin, long nOld); // the cockpit's coin counter
    void fn_8026AA04(long nTeam, long, float);
    void fn_80073FB4(int);
    void fn_80073FEC(int);
    int fn_8026D0A0(void *p);                 // the object at CGame 0x134: its message can be shown
    void fn_8026C990(void *p);                // closes that message
    void fn_8013A7A8(long nId, int bDisp);
    int fn_800EE434();
    int fn_8001275C(long nQuake);             // the quake is one the camera knows
    void fn_80163BF0();                       // CStageMan: the background fades out
    void fn_80163C50(const bcs::BCS_Color *pColor); // CStageMan: the background's fade colour
    void fn_80269E9C();
    bcs::BCS_Color fn_80163CA4(long nChr);    // the background colour of character nChr's arts event
    void fn_800ED788(long nChr);
    void fn_800ED874(long nChr);
    void fn_800ED900();
    void fn_800EE5A0(long nTeam);
    void fn_800B1DCC();                       // CDwc: online synchronisation
    int fn_800B1DD8();
    void fn_800B1E04();
    int fn_800B1E10();
    void fn_80012778(bcs::CCamera *pCamera, long nQuake); // stops a camera quake
    void fn_801A37C4();
    void fn_8026BB40(void *p, int bDelete);   // destroys the object at CGame 0x134
    void fn_80250CAC();
    void fn_80236790();
}

namespace msw {

    // A value brought into [lo, hi]; a macro (the shot clock's limit is read
    // twice by SetShotTime, so the original evaluates its bounds where used).
#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

    namespace {
        // Asks the task with id nId to delete itself, if it still exists.
        inline void DeleteTask(unsigned long nId)
        {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(nId);
            if (pTask && !pTask->IsDelete()) {
                pTask->Delete();
            }
        }
    }

    // The pause controller (0x80 bytes, vtable lbl_804764B0; only the
    // members used so far, names guessed).
    class CGamePauseTask : public bcs::CTask {
        public:
            virtual ~CGamePauseTask();
            virtual void Main(); // fn_801882F4, not written yet
            virtual void Disp();

            int m_bEnable; // 0x34, pausing is allowed
            long m_nUnk38; // 0x38
    };

    // A task holding a list of its own (0x48 bytes, vtable lbl_804767C8;
    // name guessed): the parent of the tasks that run during an arts event.
    class CGameTaskGroup : public bcs::CTask {
        public:
            virtual ~CGameTaskGroup();
            virtual void Main(); // fn_8018966C, not written yet
            virtual void Disp() {}

            nw4r::ut::List m_List; // 0x34, the CGameSpeedCtrl tasks (linked at their 0x38)
    };

    // The hit stop (0x44 bytes, vtable lbl_80476498; names guessed).
    class CGameStopTask : public bcs::CTask {
        public:
            virtual ~CGameStopTask();
            virtual void Main(); // fn_80188FC0, not written yet
            virtual void Disp();

            long m_nState;  // 0x34, 1 while a stop runs
            float m_fFrame; // 0x38
            float m_fStop;  // 0x3C
            long m_nType;   // 0x40
    };

    // A team's item roulette (defined before CGame in this file, not written
    // yet; only what CGame uses, names guessed). Its state machine is the
    // second base (0x34; the state number at 0x44 is CFsm's).
    class CItemRoulette : public bcs::CTask, public bcs::CFsm<CItemRoulette> {
            friend class CGame;
        public:
            long m_nUnk7C; // 0x7C, the cockpit's item icon id (-1: none)
    };

    CGame::GameWork CGame::g_GameWork;
    CGame::ScoreWork CGame::g_ScoreWork;
    CGame *CGame::m_pInstance;

    namespace {
        // The background fade's colour and two more values of it (names and
        // meanings guessed; lbl_805C0CE0..E8, set by the static initialiser
        // and reset by BgResetFade_).
        bcs::BCS_Color s_BgFadeColor(110, 90, 140, 255);
        nw4r::ut::Color s_BgCurColor;
        float s_fBgFade;

        // The default colours: a purple to fade to, white now.
        inline void ResetBgFadeColor()
        {
            s_BgFadeColor.r = 110;
            s_BgFadeColor.g = 90;
            s_BgFadeColor.b = 140;
            s_BgFadeColor.a = 255;
            s_BgCurColor = 0xFFFFFFFF;
            s_fBgFade = 0.0f;
        }

        // The background colour at fade level s_fBgFade (0: white, 1:
        // s_BgFadeColor).
        inline void UpdateBgFadeColor()
        {
            float f = CLAMP(s_fBgFade, 0.0f, 1.0f);
            s_fBgFade = f;
            s_BgCurColor.r = f * (s_BgFadeColor.r - 255.0f) + 255.0f;
            s_BgCurColor.g = f * (s_BgFadeColor.g - 255.0f) + 255.0f;
            s_BgCurColor.b = f * (s_BgFadeColor.b - 255.0f) + 255.0f;
        }

        // Online, the console waits for the others with pWait; without the
        // network object it goes on.
        inline int IsDwcReady(int (*pWait)())
        {
            if (bcs::dwc::CDwc::GetInstance()) {
                return pWait();
            }
            return 1;
        }
    }

    // ---- the constructor (0x8017C3AC) not written yet; the weak
    // destructors of CStateTemplate<CFsmQueue<CGame> >, CFsmQueue<CGame> and
    // CStateTemplate<CGame> follow it in the original ----

    CGame::~CGame()
    {
        Finalize();
        m_pInstance = 0;
    }

    // Ends everything the game started: its tasks, cameras, effects,
    // cockpit messages, sounds and resources, and resets the background
    // fade, the pause bits and the nunchuk stick.
    void CGame::Finalize()
    {
        if (m_pTempParent) {
            m_pTempParent->Delete();
            m_pTempParent = 0;
        }
        for (int i = 0; i < 4; i++) {
            if (m_apItemRoulette[i]) {
                m_apItemRoulette[i]->Delete();
            }
            m_apItemRoulette[i] = 0;
        }
        fn_801A37C4();
        for (int i = 0; i < 4; i++) {
            m_apCamera[i] = 0;
        }
        CGameCamera::Finalize();
        bcs::CCameraMan::RemoveEntryAll();
        menu::CSndEff::s_Instance.Finalize();
        menu::CVibEff::s_Instance.Finalize();
        CEffectCtrl::Dispose();
        CEffectCtrl::DetachResource();
        CEffectCtrl::Close();
        CNowLoading::SetDisp(0);
        CNameMesg::Remove();
        if (m_pUnk134) {
            fn_8026BB40(m_pUnk134, 1); // its deleting destructor
        }
        m_pUnk134 = 0;
        fn_80250CAC();
        fn_80236790();
        CGameAccInput::Finalize();
        CGameBGM::SetBgmState(CGameBGM::STATE_CUT);
        // TODO: the original passes a fade time of 0.0f here (f1 is loaded
        // before the call): CGameSnd::StopAllSound and bcs::CSnd::StopAllSound
        // take a float, but their headers (and symbols) still say no
        // argument.
        CGameSnd::StopAllSound();
        RsrcUnLoad_();
        ResetBgFadeColor();
        g_uPauseBit = 0;
        g_nEventChrIdx = -1;
        bcs::CPad::SetNunchukStickClamp(0x47, 0xF, 0);
    }

    // 0x8017CAEC..: the resource hooks.
    void CGame::RsrcLoadStart_()
    {
        fn_8018E068();
    }

    int CGame::RsrcIsLoadEnd_()
    {
        return CLoadData::IsComplete(-1) != 0;
    }

    void CGame::RsrcInitialize_()
    {
        CLoadData::InitData();
        CGameSnd::SaveGameHeapState();
    }

    void CGame::RsrcUnLoad_()
    {
        CLoadData::UnLoad();
    }

    // Switches to the star music while a star character or ball is about.
    void CGame::CheckBgmChange_()
    {
        int bStar = 0;
        if (CChrMan::IsExistStarChr()) {
            bStar = 1;
        }
        if (CBallMan::IsExistStarBall()) {
            bStar = 1;
        }
        if (bStar && CGameBGM::GetBgmState() == CGameBGM::STATE_PLAY) {
            CGameBGM::SetBgmState(CGameBGM::STATE_STAR);
        }
    }

    bcs::CTask *CGame::GetTempParent()
    {
        if (m_pInstance) {
            return m_pInstance->m_pTempParent;
        }
        return 0;
    }

    void CGame::KillTempChild_()
    {
        if (m_pTempParent) {
            m_pTempParent->DeleteAllChild();
        }
    }

    void CGame::ScrnSetBlackOut_()
    {
        bcs::CMainApp::Start3DFade(0, 1.0f);
        bcs::CMainApp::StartMasterFade(0, 1.0f);
    }

    // Bit 0 of nMask: the 3D layer; bit 1: the whole screen.
    void CGame::ScrnFadeStart_(unsigned long nMask, long nType, float fFrame)
    {
        if (nMask & 1) {
            bcs::CMainApp::Start3DFade(nType, fFrame);
        }
        if (nMask & 2) {
            bcs::CMainApp::StartMasterFade(nType, fFrame);
        }
    }

    int CGame::IsScrnFadeEnd_(unsigned long nMask) const
    {
        int bEnd = 1;
        if ((nMask & 1) && bcs::CMainApp::IsPlay3DFade()) {
            bEnd = 0;
        }
        if ((nMask & 2) && bcs::CMainApp::IsPlayMasterFade()) {
            bEnd = 0;
        }
        return bEnd;
    }

    void CGame::PlayGameResultBGM_()
    {
        if (CTournament::IsExtraMatchGame()) {
            if (CTournament::IsMissionClear()) {
                CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_37);
            } else {
                CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38);
            }
            return;
        }
        GameWork &rWork = g_GameWork;
        if (IsPartyGame()) {
            long nLose = 0;
            long nWin = 0;
            if (rWork.m_nGameMode != GAME_MODE_VOLLEY) {
                for (long i = 0; i < 8; i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && pChr->IsPadCtrl()) {
                        if (rWork.m_anTeamResult[pChr->GetTeamNo()] == 1) {
                            nWin++;
                        }
                        if (rWork.m_anTeamResult[pChr->GetTeamNo()] == 2) {
                            nLose++;
                        }
                    }
                }
                if (nWin > 0) {
                    CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_37);
                } else if (nLose > 0) {
                    CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38);
                } else {
                    CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38B);
                }
            } else {
                switch (rWork.m_anTeamResult[rWork.m_nUnk00]) {
                    case 1:
                        CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_37);
                        break;
                    case 2:
                        CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38);
                        break;
                    default:
                        CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38B);
                        break;
                }
            }
        } else {
            switch (rWork.m_anTeamResult[rWork.m_nUnk00]) {
                case 1:
                    CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_37);
                    break;
                case 2:
                    if (IsLocalVsMode()) {
                        CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_37);
                    } else {
                        CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38);
                    }
                    break;
                default:
                    CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_38B);
                    break;
            }
        }
    }

    // Clears what one game leaves in g_GameWork and the score work.
    void CGame::GameWorkReset_()
    {
        memset(&g_ScoreWork, 0, sizeof(g_ScoreWork));
        SetOffenseTeam(g_GameWork.GetFirstOffenseTeam());
        if (g_GameWork.m_nGameMode == GAME_MODE_VOLLEY) {
            SetTurnStartMode(GAME_TURN_START_MODE_0);
        } else {
            SetTurnStartMode(GAME_TURN_START_MODE_0);
        }
        SetTime(g_GameWork.m_fUnk20);
        GameWork &rWork = g_GameWork;
        rWork.m_nPeriod = 0;
        ClearLastGoalInfo();
        ClearCoinHistory();
        rWork.m_nUnk3C = 90;
        *(long *)rWork.m_Unk1A8 = 0;
        rWork.m_fUnk38 = 0.0f;
        for (long i = 0; i < 4; i++) {
            ClearTeamCoin(i);
            ClearTeamDisableDropCoin(i);
            ClearTeamArtsGauge(i);
            ClearTeamItem(i);
        }
        ClearGameResult_();
    }

    void CGame::ClearGameResult_()
    {
        for (long i = 0; i < 4; i++) {
            g_GameWork.m_anTeamResult[i] = 0;
        }
    }

    void CGame::SetGameResultAll_(long nResult)
    {
        for (long i = 0; i < 4; i++) {
            g_GameWork.m_anTeamResult[i] = nResult;
        }
    }

    void CGame::AddTeamCoin_(long nTeam, long nCoin)
    {
        if (nTeam >= 0 && !(g_GameWork.m_nUnk40 & 1)) {
            long nOld = g_GameWork.m_anTeamCoin[nTeam];
            long nNew = nCoin + nOld;
            if (nNew > GetMaxCoin()) {
                nNew = GetMaxCoin();
            } else if (nNew < 0) {
                nNew = 0;
            }
            g_GameWork.m_anTeamCoin[nTeam] = nNew;
            fn_80137964(nTeam, nNew, nOld);
            if (nNew > nOld) {
                fn_8026AA04(nTeam, 0, -1.0f);
            }
            g_GameWork.m_anCoinHistCount[nTeam]++;
            g_GameWork.m_anCoinHistTime[nTeam] = g_GameWork.m_nUnk3C;
        }
    }

    long CGame::GetMaxCoin_() const
    {
        return 100;
    }

    void CGame::UseTeamCoin_(long nTeam, long nCoin)
    {
        if (nTeam >= 0 && !(g_GameWork.m_nUnk40 & 1)) {
            long nHave = g_GameWork.m_anTeamCoin[nTeam];
            long nUse = nCoin;
            if (nCoin > nHave) {
                nUse = nHave;
            } else if (nCoin < 0) {
                nUse = 0;
            }
            long nNew = nHave - nUse;
            if (nNew > GetMaxCoin()) {
                nNew = GetMaxCoin();
            } else if (nNew < 0) {
                nNew = 0;
            }
            g_GameWork.m_anTeamCoin[nTeam] = nNew;
            fn_80137964(nTeam, -nUse, nNew);
            g_GameWork.m_anCoinHistCount[nTeam]++;
            g_GameWork.m_anCoinHistTime[nTeam] = g_GameWork.m_nUnk3C;
        }
    }

    void CGame::TrgGoalIn(long nChr, long nTeam, long nShootType, long nKind, float f, const bcs::FVEC3 &rGoalPos)
    {
        if (m_pInstance) {
            m_pInstance->TrgGoalIn_(nChr, nTeam, nShootType, nKind, f, rGoalPos);
        }
    }

    void CGame::TrgGoalIn_(long, long, long, long, float, const bcs::FVEC3 &) {}

    void CGame::TrgBasketGoalIn(long nChr, long nTeam, long nShootType, long nKind, float f,
                                const bcs::FVEC3 &rGoalPos, long n)
    {
        if (m_pInstance) {
            m_pInstance->TrgBasketGoalIn_(nChr, nTeam, nShootType, nKind, f, rGoalPos, n);
        }
    }

    void CGame::TrgBasketGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, long) {}

    void CGame::TrgVolleyGoalIn(long n0, long n1, long n2, long n3, long n4, long n5, const bcs::FVEC3 &rPos,
                                unsigned long n6)
    {
        if (m_pInstance) {
            m_pInstance->TrgVolleyGoalIn_(n0, n1, n2, n3, n4, n5, rPos, n6);
        }
    }

    void CGame::TrgVolleyGoalIn_(long, long, long, long, long, long, const bcs::FVEC3 &, unsigned long) {}

    int CGame::IsNowBonusTime()
    {
        if (m_pInstance) {
            return m_pInstance->IsNowBonusTime_();
        }
        return 0;
    }

    int CGame::IsVolleyScored()
    {
        if (m_pInstance) {
            return m_pInstance->IsVolleyScored_();
        }
        return 0;
    }

    int CGame::IsVolleyServeClockOver()
    {
        if (m_pInstance) {
            return m_pInstance->IsVolleyServeClockOver_();
        }
        return 0;
    }

    void CGame::TrgHockeyGoalIn(long nChr, long nTeam, long nShootType, long nKind, float f,
                                const bcs::FVEC3 &rGoalPos, unsigned long n)
    {
        if (m_pInstance) {
            m_pInstance->TrgHockeyGoalIn_(nChr, nTeam, nShootType, nKind, f, rGoalPos, n);
        }
    }

    void CGame::TrgHockeyGoalIn_(long, long, long, long, float, const bcs::FVEC3 &, unsigned long) {}

    void CGame::TrgArtsEvent(long nPlayer)
    {
        if (m_pInstance) {
            m_pInstance->TrgArtsEvent_(nPlayer);
        }
    }

    int CGame::IsArtsEventMode()
    {
        if (m_pInstance) {
            return m_pInstance->m_nReqArtsEvent > 1;
        }
        return 0;
    }

    int CGame::IsReqArtsEvent()
    {
        if (m_pInstance) {
            return m_pInstance->m_nReqArtsEvent > 0;
        }
        return 0;
    }

    // Not in the DOL (nothing calls it, the link strips it); its weak callee
    // _GetHockeyResultGoalId is emitted here in the original, so it existed.
    // The result without a game is a guess.
    long CGame::GetHockeyResultGoalId()
    {
        if (m_pInstance) {
            return m_pInstance->_GetHockeyResultGoalId();
        }
        return -1;
    }

    void CGame::BgResetFade_()
    {
        ResetBgFadeColor();
        m_nBgFadeMode = 0;
        m_fBgFadeStep = 0.0f;
        m_fBgFadeWait = 0.0f;
        CStageMan::FadeIn();
    }

    void CGame::BgSetFadeIn(float fWait, float fFrames)
    {
        CGame *pGame = m_pInstance;
        if (pGame) {
            pGame->m_nBgFadeMode = 10;
            pGame->m_fBgFadeWait = fWait;
            if (fFrames > 0.0f) {
                pGame->m_fBgFadeStep = -1.0f / fFrames;
            } else {
                pGame->m_fBgFadeStep = -1.0f;
            }
        }
    }

    void CGame::BgSetFadeOut(float fWait, float fFrames)
    {
        CGame *pGame = m_pInstance;
        if (pGame) {
            pGame->m_nBgFadeMode = 20;
            pGame->m_fBgFadeWait = fWait;
            if (fFrames > 0.0f) {
                pGame->m_fBgFadeStep = 1.0f / fFrames;
            } else {
                pGame->m_fBgFadeStep = 1.0f;
            }
        }
    }

    // The background fade: waits m_fBgFadeWait frames, then moves the fade
    // level to 0 (mode 10) or 1 (mode 20) by m_fBgFadeStep each frame and
    // mixes the stage's background colour from white by it.
    void CGame::BgFadeUpdate_()
    {
        switch (m_nBgFadeMode) {
            case 10:
                m_fBgFadeWait -= GetFrameRateF();
                if (m_fBgFadeWait > 0.0f) {
                    break;
                }
                m_nBgFadeMode = 11;
                CStageMan::FadeIn();
                // fall through
            case 11:
                s_fBgFade += m_fBgFadeStep * GetFrameRateF();
                if (s_fBgFade <= 0.0f) {
                    m_nBgFadeMode = 0;
                }
                UpdateBgFadeColor();
                break;
            case 20:
                m_fBgFadeWait -= GetFrameRateF();
                if (m_fBgFadeWait > 0.0f) {
                    break;
                }
                m_nBgFadeMode = 21;
                fn_80163C50(&s_BgFadeColor);
                fn_80163BF0();
                // fall through
            case 21:
                s_fBgFade += m_fBgFadeStep * GetFrameRateF();
                if (s_fBgFade >= 1.0f) {
                    m_nBgFadeMode = 0;
                }
                UpdateBgFadeColor();
                break;
        }
    }

    // An arts event is asked for by character nChr: the background takes
    // the character's colour, the screen goes to one view and the event
    // starts with the next Main.
    void CGame::TrgArtsEvent_(long nChr)
    {
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr) {
            s_BgFadeColor = fn_80163CA4(pChr->GetChrIdS());
        }
        if (m_nReqArtsEvent <= 0) {
            g_nEventChrIdx = nChr;
            if ((unsigned long)bcs::CWorldEnv::m_vstViewSplitType <= 3) {
                bcs::CRenderMan::StartCrossFade(CLoadData::GetWipeMdlFile(), "cutchange00", -20.0f, 1);
            }
            bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
            fn_80073FB4(1);
            g_uPauseBit |= 0x100;
            SendMessage_(0xB, g_nEventChrIdx, 0, 0);
            m_nReqArtsEvent = 1;
        }
    }

    void CGame::ResetArtsEventPauseBit_(unsigned long nBit)
    {
        g_uPauseBit &= ~nBit;
    }

    void CGame::ArtsEventBegin_()
    {
        CGameTaskGroup *pRoot = m_pArtsTaskRoot;
        if (pRoot) {
            pRoot->DeleteAllChild();
            nw4r::ut::List_Init(&pRoot->m_List, 0x38);
            bcs::CMainApp::SetUnivSpeed(g_GameWork.m_fGameSpeed);
        }
        fn_80269E9C();
        g_uPauseBit |= 0x113E;
        BgSetFadeOut(0.0f, 30.0f);
        SetDispRadarIcon_(0);
        fn_800ED788(g_nEventChrIdx);
        m_fArtsEventFrame = GetArtsEventFrameNum_();
        m_nArtsEventStep = 0;
        m_nArtsEventSync = 1;
        for (long i = 0; i < 4; i++) {
            if (m_apCamera[i]) {
                m_apCamera[i]->StartArtsEventDemo();
            }
        }
        StartArtsEvent_(g_nEventChrIdx);
    }

    float CGame::GetArtsEventFrameNum_()
    {
        return 85.0f;
    }

    // Not zero when the arts event is over: its frames have run out and, in
    // an online game, every console has loaded the character's sounds.
    int CGame::ArtsEventLoop_()
    {
        switch (m_nArtsEventSync) {
            case 1:
                if (g_GameWork.m_Unk151 && bcs::dwc::CDwc::GetInstance()) {
                    fn_800B1DCC();
                }
                m_nArtsEventSync++;
                break;
            case 2:
                if (!g_GameWork.m_Unk151 || IsDwcReady(fn_800B1DD8)) {
                    CGameSnd::LoadChrSnd(g_nEventChrIdx);
                    m_nArtsEventSync++;
                }
                break;
            case 3:
                if (bcs::CSnd::IsReady()) {
                    if (g_GameWork.m_Unk151 && bcs::dwc::CDwc::GetInstance()) {
                        fn_800B1E04();
                    }
                    m_nArtsEventSync++;
                }
                break;
            case 4:
                if (!g_GameWork.m_Unk151 || IsDwcReady(fn_800B1E10)) {
                    fn_800ED900();
                    m_nArtsEventSync = 0;
                }
                break;
        }
        m_fArtsEventFrame -= GetFrameRateF();
        return m_fArtsEventFrame <= 0.0f && m_nArtsEventSync == 0;
    }

    void CGame::ArtsEventEnd_()
    {
        EndArtsEvent_(g_nEventChrIdx);
        for (long i = 0; i < 4; i++) {
            if (m_apCamera[i]) {
                m_apCamera[i]->EndArtsEventDemo();
            }
        }
        fn_800ED874(g_nEventChrIdx);
        fn_80073FB4(0);
        bcs::CRenderMan::ReqPopViewSplitType(0);
        SetDispRadarIcon_(1);
        SendMessage_(0xC, g_nEventChrIdx, 0, 0);
    }

    int CGame::IsReqArtsEvent_()
    {
        return m_nReqArtsEvent > 0;
    }

    void CGame::SetGamePauseEnable_(int bEnable)
    {
        if (m_pPauseTask) {
            m_pPauseTask->m_bEnable = bEnable;
        }
    }

    // (name guessed) Sets the pause controller's word at 0x38.
    void CGame::SetGamePauseUnk38(long n)
    {
        if (m_pInstance && m_pInstance->m_pPauseTask) {
            m_pInstance->m_pPauseTask->m_nUnk38 = n;
        }
    }

    // (name guessed) Turns the cameras round to the other side of the court.
    void CGame::ChangeCameraBaseDir()
    {
        if (m_pInstance) {
            CGame *pGame = m_pInstance;
            pGame->ChangeCameraBaseDir_();
            CGimmickMan::QPanelSetViewBaseRot(g_GameWork.m_anGoalIdx[GetViewTargetTeam(0)] ? 3.1415927f : 0.0f);
            pGame->CameraReset_();
            pGame->SendMessage_(0x30, 0, 0, 0);
        }
    }

    int CGame::IsMainLoopState(int bCheckFlag)
    {
        if (m_pInstance) {
            if (m_pInstance->FsmqGetState() != 5) {
                return 0;
            }
            if (bCheckFlag && (m_pInstance->m_nGameFlag & 1)) {
                return 0;
            }
            return 1;
        }
        return 0;
    }

    void CGame::StartStopEffect(float fFrame, float fStop, long nType)
    {
        if (m_pInstance) {
            CGameStopTask *pTask = m_pInstance->m_pStopTask;
            if (pTask) {
                if (!bcs::CMainApp::IsPauseReady(4) && !bcs::CMainApp::IsPause(4)) {
                    bcs::CMainApp::SetPause(4, 1);
                }
                if (pTask->m_fFrame < fFrame) {
                    pTask->m_fFrame = fFrame;
                }
                if (pTask->m_fStop < fStop) {
                    pTask->m_fStop = fStop;
                }
                pTask->m_nType = nType;
                if (pTask->m_nState < 1) {
                    pTask->m_nState = 1;
                }
            }
        }
    }

    unsigned long CGame::StartSlowEffect(float fTo, float fTime, float f2, float f3)
    {
        if (m_pInstance) {
            return m_pInstance->StartSlowEffect_(fTo, fTime, f2, f3);
        }
        return 0;
    }

    unsigned long CGame::StartSlowEffect_(float fTo, float fTime, float f2, float f3)
    {
        StopSlowEffect();
        CGameSpeedCtrl *pCtrl;
        if (m_pArtsTaskRoot) {
            pCtrl = new (0x100) CGameSpeedCtrl(m_pArtsTaskRoot);
        } else {
            pCtrl = 0;
        }
        if (pCtrl) {
            pCtrl->StartSlow(g_GameWork.m_fGameSpeed, fTo, fTime, f2, f3);
            m_nSlowTaskId = pCtrl->GetId();
        }
        return m_nSlowTaskId;
    }

    void CGame::StopSlowEffect()
    {
        if (m_pInstance) {
            m_pInstance->StopSlowEffect_();
        }
    }

    void CGame::StopSlowEffect_()
    {
        if (m_nSlowTaskId) {
            DeleteTask(m_nSlowTaskId);
            m_nSlowTaskId = 0;
        }
    }

    int CGame::IsNowSlowEffect()
    {
        if (m_pInstance) {
            if (!m_pInstance->m_nSlowTaskId) {
                return 0;
            }
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_pInstance->m_nSlowTaskId);
            if (pTask) {
                return !pTask->IsDelete();
            }
            return 0;
        }
        return 0;
    }

    long CGame::GetExceptionalCameraType()
    {
        if (m_pInstance) {
            return m_pInstance->GetExceptionalCameraType_();
        }
        return 0;
    }

    void CGame::TelopCreate(long nTelop, int b, long n)
    {
        if (m_pInstance) {
            m_pInstance->TelopCreate_(nTelop, b, n);
        }
    }

    void CGame::TelopEnd(long nTelop)
    {
        if (m_pInstance) {
            m_pInstance->TelopEnd_(nTelop);
        }
    }

    void CGame::TelopRemove(long nTelop)
    {
        if (m_pInstance) {
            m_pInstance->TelopRemove_(nTelop);
        }
    }

    int CGame::TelopIsEnd(long nTelop)
    {
        if (m_pInstance) {
            return m_pInstance->TelopIsEnd_(nTelop);
        }
        return 1;
    }

    int CGame::IsTournament()
    {
        return CTournament::IsTournamentGame();
    }

    int CGame::IsTourFinalMatch()
    {
        if (CTournament::IsTournamentGame()) {
            return fn_8017AB9C();
        }
        return 0;
    }

    int CGame::StartCameraQuake(long nQuake, long nChr, long nView)
    {
        if (m_pInstance) {
            return m_pInstance->StartCameraQuake_(nQuake, nChr, nView);
        }
        return 0;
    }

    // (name guessed) Starts quake nQuake on view nView (-1: every view), or
    // on the view of character nChr's player when the quake allows it.
    int CGame::StartCameraQuake_(long nQuake, long nChr, long nView)
    {
        if (nQuake < 0) {
            return 0;
        }
        if (fn_8001275C(nQuake) && nChr >= 0) {
            int bFound;
            if (!CChrMan::IsActive()) {
                bFound = 0;
            } else {
                long nNum = CChrMan::GetChrCoreNum();
                if (nChr < 0 || nChr >= nNum) {
                    bFound = 0;
                } else {
                    CChrCore *pChr = CChrMan::GetChrCore(nChr);
                    if (pChr && pChr->IsEnable() && pChr->IsPadCtrl()) {
                        nView = pChr->GetCamNo();
                        bFound = 1;
                    } else {
                        bFound = 0;
                    }
                }
            }
            if (!bFound) {
                return 0;
            }
        }
        if (nView < 0) {
            int bStarted = 0;
            for (long i = 0; i < bcs::CWorldEnv::GetViewSplitNum(); i++) {
                if (m_apCamera[i] && m_apCamera[i]->StartQuake(nQuake)) {
                    bStarted = 1;
                }
            }
            return bStarted;
        }
        if (nView < 4 && m_apCamera[nView]) {
            return m_apCamera[nView]->StartQuake(nQuake);
        }
        return 0;
    }

    void CGame::StopCameraQuake(long nQuake)
    {
        if (m_pInstance) {
            for (long i = 0; i < 4; i++) {
                bcs::CCamera *pCamera = bcs::CCameraMan::GetActiveCamera(i);
                if (pCamera) {
                    fn_80012778(pCamera, nQuake);
                }
            }
        }
    }

    void CGame::StopAllCameraQuake()
    {
        CGame *pGame = m_pInstance;
        if (pGame) {
            for (long i = 0; i < 4; i++) {
                if (pGame->m_apCamera[i]) {
                    fn_80012778(pGame->m_apCamera[i], -1);
                }
            }
        }
    }

    CGameCamera *CGame::GetGameCamera(long nView)
    {
        if (m_pInstance) {
            return m_pInstance->m_apCamera[nView];
        }
        return 0;
    }

    void CGame::SendMessage(long nMsg, long n1, long n2, long n3)
    {
        if (m_pInstance) {
            m_pInstance->SendMessage_(nMsg, n1, n2, n3);
        }
    }

    int CGame::IsLineMsgDispEnable()
    {
        if (m_pInstance) {
            return m_pInstance->IsLineMsgDispEnable_();
        }
        return 0;
    }

    long CGame::GetFirstGoalIdxMode_()
    {
        return 0;
    }

    // Swaps the goals of the two teams, unless the views turn instead (a
    // local versus game on one screen keeps its goals).
    void CGame::ChangeGoalIdx_()
    {
        int bChange = 0;
        if (!IsLocalVsMode()) {
            bChange = 1;
        }
        if (g_GameWork.m_nViewSplit) {
            bChange = 1;
        }
        if (g_GameWork.m_Unk151) {
            // Online, the games with goal rings keep their goals.
            switch (g_GameWork.m_nGameMode) {
                case GAME_MODE_BASKET:
                case GAME_MODE_HOCKEY:
                    bChange = 0;
                    break;
            }
        }
        if (bChange) {
            long nGoal = g_GameWork.m_anGoalIdx[0];
            SetGoalIdx(0, 1 - nGoal);
            SetGoalIdx(1, nGoal);
        } else {
            ChangeCameraBaseDir_();
        }
    }

    // (no name, no caller) An empty function between ChangeGoalIdx_ and
    // ChangeCameraBaseDir_; probably a hook whose body is compiled out.
    extern "C" void fn_8017F490() {}

    void CGame::ChangeCameraBaseDir_()
    {
        SetViewTeam(0, 1 - GetViewTargetTeam(0));
        SetViewTeam(1, 1 - GetViewTargetTeam(1));
    }

    // Adds each team's coins to the coins it has won and clears them.
    void CGame::SetPeriodResult_()
    {
        for (long i = 0; i < 4; i++) {
            AddTeamGainCoin(i, GetTeamCoin(i));
            ClearTeamCoin(i);
        }
    }

    void CGame::EventDemoSetUp()
    {
        if (FsmqGetState() == 9 && g_GameWork.m_nGameMode == GAME_MODE_HOCKEY) {
            CChrMan::AllSetState(4, -1, 0);
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        }
    }

    void CGame::CameraReset_()
    {
        for (long i = 0; i < 4; i++) {
            if (m_apCamera[i]) {
                fn_80012778(m_apCamera[i], -1);
                m_apCamera[i]->Reset();
            }
        }
    }

    // The state table: 30 states, a queue of 16 with 0x20 bytes of work
    // each. The virtual states go through their non-virtual wrappers.
    void CGame::InitState()
    {
        FsmqInit(this, 30, 16, 0x20);
        FsmqSetState(0, &CGame::CtorStateGameLoad, &CGame::LoopStateGameLoad, &CGame::DtorStateGameLoad);
        FsmqSetState(1, &CGame::CtorStateGameInit_, &CGame::LoopStateGameInit_, &CGame::DtorStateGameInit_);
        FsmqSetState(21, &CGame::CtorStateGameStart, &CGame::LoopStateGameStart, &CGame::DtorStateGameStart);
        FsmqSetState(2, &CGame::CtorStateEntranceDemo, &CGame::LoopStateEntranceDemo,
                     &CGame::DtorStateEntranceDemo);
        FsmqSetState(3, &CGame::CtorStateGameSetup_, &CGame::LoopStateGameSetup_, &CGame::DtorStateGameSetup_);
        FsmqSetState(4, &CGame::CtorStateWaitStartBase, &CGame::LoopStateWaitStartBase,
                     &CGame::DtorStateWaitStartBase);
        FsmqSetState(5, &CGame::CtorStateMainLoopBase, &CGame::LoopStateMainLoopBase,
                     &CGame::DtorStateMainLoopBase);
        FsmqSetState(6, &CGame::CtorStateGoalEvent_, &CGame::LoopStateGoalEvent_, &CGame::DtorStateGoalEvent_);
        FsmqSetState(7, &CGame::CtorStateTurnResult_, &CGame::LoopStateTurnResult_, &CGame::DtorStateTurnResult_);
        FsmqSetState(8, &CGame::CtorStateTimeUp_, &CGame::LoopStateTimeUp_, &CGame::DtorStateTimeUp_);
        FsmqSetState(9, &CGame::CtorStateChgCourt_, &CGame::LoopStateChgCourt_, &CGame::DtorStateChgCourt_);
        FsmqSetState(10, &CGame::CtorStateTurnOver_, &CGame::LoopStateTurnOver_, &CGame::DtorStateTurnOver_);
        FsmqSetState(11, &CGame::CtorStateChgOffense_, &CGame::LoopStateChgOffense_, &CGame::DtorStateChgOffense_);
        FsmqSetState(12, &CGame::CtorStateGameSet_, &CGame::LoopStateGameSet_, &CGame::DtorStateGameSet_);
        FsmqSetState(13, &CGame::CtorStateGameResult_, &CGame::LoopStateGameResult_, &CGame::DtorStateGameResult_);
        FsmqSetState(14, &CGame::CtorStateResultMenu_, &CGame::LoopStateResultMenu_, &CGame::DtorStateResultMenu_);
        FsmqSetState(15, &CGame::CtorStateEndMenu_, &CGame::LoopStateEndMenu_, &CGame::DtorStateEndMenu_);
        FsmqSetState(16, &CGame::CtorStateRestart_, &CGame::LoopStateRestart_, &CGame::DtorStateRestart_);
        FsmqSetState(17, &CGame::CtorStateChgField_, &CGame::LoopStateChgField_, &CGame::DtorStateChgField_);
        FsmqSetState(18, &CGame::CtorStateGameExit_, &CGame::LoopStateGameExit_, &CGame::DtorStateGameExit_);
        FsmqSetState(19, &CGame::CtorStateEventDemo_, &CGame::LoopStateEventDemo_, &CGame::DtorStateEventDemo_);
        FsmqSetState(20, &CGame::CtorStateExceptDemo_, &CGame::LoopStateExceptDemo_, &CGame::DtorStateExceptDemo_);
        FsmqSetState(22, &CGame::CtorStateGameCreate, &CGame::LoopStateGameCreate, &CGame::DtorStateGameCreate);
        FsmqSetState(23, &CGame::CtorStateBgmStart, &CGame::LoopStateBgmStart, &CGame::DtorStateBgmStart);
        FsmqSetState(24, &CGame::CtorStateOnlineSync0, &CGame::LoopStateOnlineSync0, &CGame::DtorStateOnlineSync0);
        FsmqSetState(25, &CGame::CtorStateOnlineSync1, &CGame::LoopStateOnlineSync1, &CGame::DtorStateOnlineSync1);
        FsmqSetState(26, &CGame::CtorStateOnlineSync2, &CGame::LoopStateOnlineSync2, &CGame::DtorStateOnlineSync2);
        FsmqSetState(27, &CGame::CtorStateNoop, &CGame::LoopStateNoop, &CGame::DtorStateNoop);
        FsmqSetState(28, &CGame::CtorStateSave, &CGame::LoopStateSave, &CGame::DtorStateSave);
        FsmqSetState(29, &CGame::CtorStateMessage, &CGame::LoopStateMessage, &CGame::DtorStateMessage);
        FsmqChangeState(0);
    }

    // Counts the state's frames down; not zero once they have run out.
    int CGame::StateWorkDecStateCount_()
    {
        if (m_fStateCount > 0.0f) {
            m_fStateCount -= GetFrameRateF();
        }
        if (m_fStateCount <= 0.0f) {
            return 1;
        }
        return 0;
    }

    void CGame::Main()
    {
        switch (m_nReqArtsEvent) {
            case 0:
                break;
            case 1:
                m_nReqArtsEvent++;
                ArtsEventBegin_();
                // fall through
            case 2:
                if (ArtsEventLoop_()) {
                    ArtsEventEnd_();
                    m_nReqArtsEvent++;
                }
                break;
            default:
                g_uPauseBit &= ~0x113E;
                m_nReqArtsEvent = 0;
                break;
        }
        BgFadeUpdate_();
        FsmqExec();
        bcs::CMainApp::GetAppFrameRateF();
        if (!m_bExit) {
            CockpitCheckArrowState_();
            CheckBgmChange_();
            MainFunc();
        }
    }

    void CGame::MainFunc() {}

    void CGame::Disp()
    {
        DispFunc();
    }

    void CGame::DispFunc() {}

    // State 0: loads the game.
    void CGame::CtorStateGameLoad()
    {
        bcs::CMainApp::Start3DFade(0, 1.0f);
        bcs::CMainApp::StartMasterFade(0, 1.0f);
        g_GameWork.m_nFlag18 |= 0x4000;
        m_nStateStep = 0;
    }

    // ---- LoopStateGameLoad (0x80180E2C) not written yet ----

    void CGame::FinishFirstLoadData() {}

    void CGame::DtorStateGameLoad() {}

    void CGame::CtorStateGameInit() {}

    void CGame::LoopStateGameInit()
    {
        CCockpit::InitFaceIcon();
        ScoreBookInitCountWork_();
        FsmqChangeState(3);
    }

    void CGame::DtorStateGameInit() {}

    // ---- 0x8018190C..0x80181C90 (state 2) not written yet ----

    void CGame::DtorStateGameStart()
    {
        bcs::CMainApp::SetPause(0x40, 0);
        CGamePadCheck::SetPadCheckDisable(0);
        fn_80073FEC(0);
        SendMessage_(2, 0, 0, 0);
    }

    // ---- 0x80181D10..0x80182064 (state 3) not written yet ----

    void CGame::DtorStateEntranceDemo()
    {
        g_GameWork.m_nFlag18 &= ~2;
        SendMessage_(4, 0, 0, 0);
    }

    // State 22: creates the game's objects.
    void CGame::CtorStateGameCreate() {}

    // ---- LoopStateGameCreate (0x8018209C) not written yet ----

    void CGame::GenerateInstance() {}

    void CGame::DtorStateGameCreate() {}

    // State 23: starts the music and goes on at once.
    void CGame::CtorStateBgmStart() {}

    void CGame::LoopStateBgmStart()
    {
        CGameBGM::SetBgmState(CGameBGM::STATE_START);
        FsmqDoChain();
    }

    void CGame::DtorStateBgmStart() {}

    void CGame::CtorStateGameSetup() {}

    void CGame::LoopStateGameSetup() {}

    void CGame::DtorStateGameSetup() {}

    // State 5: runs the virtual WaitStart hooks.
    void CGame::CtorStateWaitStartBase()
    {
        CtorStateWaitStart();
    }

    // ---- LoopStateWaitStartBase (0x80182E88) not written yet ----

    void CGame::DtorStateWaitStartBase()
    {
        DtorStateWaitStart();
    }

    void CGame::CtorStateWaitStart() {}

    void CGame::LoopStateWaitStart() {}

    void CGame::DtorStateWaitStart() {}

    // State 6: runs the virtual MainLoop hooks.
    void CGame::CtorStateMainLoopBase()
    {
        CtorStateMainLoop();
    }

    // ---- LoopStateMainLoopBase (0x80183170) not written yet ----

    void CGame::DtorStateMainLoopBase()
    {
        DtorStateMainLoop();
    }

    void CGame::CtorStateMainLoop() {}

    void CGame::LoopStateMainLoop() {}

    void CGame::DtorStateMainLoop() {}

    void CGame::CtorStateGoalEvent()
    {
        m_fStateCount = 60.0f;
        m_nStateStep = 0;
    }

    void CGame::DtorStateGoalEvent() {}

    void CGame::CtorStateTurnResult()
    {
        SetReplayMode(0);
        SetResultMode(1);
        m_nStateStep = 0;
    }

    int CGame::IsGameSet_()
    {
        return 0;
    }

    void CGame::DtorStateTurnResult() {}

    void CGame::CtorStateTimeUp() {}

    void CGame::LoopStateTimeUp() {}

    void CGame::DtorStateTimeUp() {}

    int CGame::MenuCallInterval_()
    {
        return 0;
    }

    void CGame::DtorStateChgCourt()
    {
        CChrMan::AllReset();
        CBallMan::AllReset();
        m_nUnk104 = 1;
        m_nUnk108 = 0;
    }

    void CGame::CtorStateTurnOver() {}

    void CGame::LoopStateTurnOver() {}

    void CGame::DtorStateTurnOver() {}

    void CGame::CtorStateChgOffense() {}

    void CGame::LoopStateChgOffense() {}

    void CGame::DtorStateChgOffense() {}

    void CGame::CtorStateGameSet() {}

    void CGame::LoopStateGameSet() {}

    void CGame::DtorStateGameSet() {}

    void CGame::SetGameResult_() {}

    void CGame::CalcAddSkillPoint_() {}

    void CGame::DtorStateGameResult()
    {
        SendMessage(6, 0, 0, 0);
    }

    void CGame::CtorStateResultMenu()
    {
        SetReplayMode(0);
        SetResultMode(1);
        SetPreparationMode(0);
        CGameBGM::SetBgmState(CGameBGM::STATE_MUSIC_3C);
        m_nStateStep = 0;
    }

    int CGame::MenuCallGameResult_()
    {
        return 0;
    }

    void CGame::DtorStateResultMenu() {}

    void CGame::CtorStateEndMenu()
    {
        if (CSave::IsUseSave()) {
            m_nStateStep = 0;
        } else {
            m_nStateStep = 2;
        }
    }

    void CGame::DtorStateEndMenu() {}

    // State 28: saves (99 skips it).
    void CGame::CtorStateSave()
    {
        if (CSave::IsUseSave()) {
            m_nStateStep = 2;
        } else {
            m_nStateStep = 99;
        }
    }

    // ---- LoopStateSave (0x801851A8) not written yet ----

    void CGame::DtorStateSave() {}

    // State 29: shows the message of the object at 0x134 (99 skips it).
    void CGame::CtorStateMessage()
    {
        m_nStateStep = 0;
        if (!fn_8026D0A0(m_pUnk134)) {
            m_nStateStep = 99;
        }
    }

    // ---- LoopStateMessage (0x80185364) not written yet ----

    void CGame::DtorStateMessage()
    {
        if (m_pUnk134) {
            fn_8026C990(m_pUnk134);
        }
        fn_80073FB4(0);
        bcs::CMainApp::SetPause(0x40, 0);
    }

    void CGame::CtorStateRestart()
    {
        bcs::CMainApp::StartMasterFade(0, 1.0f / 30.0f);
        CGameBGM::SetBgmState(CGameBGM::STATE_FADE_OUT);
        m_nStateStep = 0;
    }

    void CGame::PrepareRestartGame_() {}

    void CGame::DtorStateRestart() {}

    void CGame::CtorStateChgField() {}

    void CGame::LoopStateChgField() {}

    void CGame::DtorStateChgField() {}

    void CGame::DtorStateEventDemo()
    {
        SetPreparationMode(1);
    }

    void CGame::CtorStateExceptDemo() {}

    void CGame::LoopStateExceptDemo() {}

    void CGame::DtorStateExceptDemo() {}

    // State 27 does nothing.
    void CGame::CtorStateNoop() {}

    void CGame::LoopStateNoop() {}

    void CGame::DtorStateNoop() {}

    int CGame::IsGameExist()
    {
        return m_pInstance != 0;
    }

    long CGame::GetLoadedUnkCC()
    {
        CGame *pGame = m_pInstance;
        if (pGame) {
            if (pGame->RsrcIsLoadEnd_()) {
                return pGame->m_nUnkCC;
            }
            return 0;
        }
        return 0;
    }

    // ---- 0x80186770..0x80186A8C not written yet ----

    long CGame::GetUnk148()
    {
        if (m_pInstance) {
            return m_pInstance->m_nUnk148;
        }
        return 0;
    }

    int CGame::IsRsrcLoadEnd()
    {
        if (m_pInstance) {
            return m_pInstance->RsrcIsLoadEnd_() != 0;
        }
        return 0;
    }

    // Pauses an offline game while the pad check waits for a remote.
    void CGame::PadCheckPause()
    {
        if (m_pInstance && !g_GameWork.m_Unk151 && !bcs::CMainApp::IsPause(0x1000) &&
            !bcs::CMainApp::IsPauseReady(0x1000)) {
            StopCameraQuake(-1);
            CGameSnd::GamePauseAll(1, 16.0f);
            bcs::CPadWI::PushMotorAll();
            bcs::CMainApp::SetPause(0x1000, 1);
            fn_80073FB4(1);
        }
    }

    void CGame::PadCheckResume()
    {
        if (m_pInstance && !g_GameWork.m_Unk151 && bcs::CMainApp::IsPause(0x1000)) {
            bcs::CMainApp::SetPause(0x1000, 0);
            fn_80073FB4(0);
            bcs::CPadWI::PopMotorAll();
            CGameSnd::GamePauseAll(0, 18.0f);
        }
    }

    // State 24-26: the online synchronisation (their work's first word is
    // the step).
    void CGame::CtorStateOnlineSync0()
    {
        *FsmqGetWork<long>() = 0;
    }

    // ---- LoopStateOnlineSync0 (0x80186C6C) not written yet ----

    void CGame::DtorStateOnlineSync0() {}

    void CGame::CtorStateOnlineSync1()
    {
        *FsmqGetWork<long>() = 0;
    }

    // ---- LoopStateOnlineSync1 (0x80186F38) not written yet ----

    void CGame::DtorStateOnlineSync1() {}

    void CGame::CtorStateOnlineSync2()
    {
        *FsmqGetWork<long>() = 0;
        m_nUnk148 = 0;
    }

    // ---- LoopStateOnlineSync2 (0x8018726C) not written yet ----

    void CGame::DtorStateOnlineSync2() {}

    void CGame::CtorStateGameExit()
    {
        SetPreparationMode(1);
        m_bExit = 1;
        m_nStateStep = 0;
    }

    void CGame::DtorStateGameExit() {}

    long CGame::MenuGetResult_()
    {
        return CCockpit::GetResult();
    }

    void CGame::SetTurnStartTelopType_(long nType, float fFrame)
    {
        m_nTurnStartTelopType = nType;
        m_fTurnStartTelopFrame = fFrame > 0.0f ? fFrame : 0.0f;
    }

    int CGame::CheckDemoSkipInput_()
    {
        int bSkip = 0;
        if (DemoSkipInputCheck(-1) >= 0) {
            bSkip = 1;
        }
        return bSkip;
    }

    // (name guessed) The pads of team nTeam as bits (all teams for -1);
    // online the pads of the match.
    unsigned long CGame::GetTeamPadBits(long nTeam)
    {
        int bOnline = g_GameWork.m_Unk151;
        if (bOnline) {
            dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
            if (!pDwc) {
                bOnline = 0;
            } else if (!pDwc->m_Dwc.IsInMatch()) {
                bOnline = 0;
            }
        }
        if (bOnline) {
            if (nTeam >= 0 && nTeam < 4) {
                return g_GameWork.m_anTeamOnlinePadBits[nTeam];
            }
            return g_GameWork.m_anTeamOnlinePadBits[0] | g_GameWork.m_anTeamOnlinePadBits[1] |
                   g_GameWork.m_anTeamOnlinePadBits[2] | g_GameWork.m_anTeamOnlinePadBits[3];
        }
        if (nTeam >= 0 && nTeam < 4) {
            return g_GameWork.m_anTeamPadBits[nTeam];
        }
        return g_GameWork.m_anTeamPadBits[0] | g_GameWork.m_anTeamPadBits[1] | g_GameWork.m_anTeamPadBits[2] |
               g_GameWork.m_anTeamPadBits[3];
    }

    // The pad of team nTeam's players that pressed A or 1 to skip the demo;
    // -1 for none.
    long CGame::DemoSkipInputCheck(long nTeam)
    {
        if (g_GameWork.m_nUnk180) {
            return -1;
        }
        unsigned long nPadBits = GetTeamPadBits(nTeam);
        for (long i = 0; i < 4; i++) {
            if (nPadBits & (1 << i)) {
                switch (g_GameWork.m_anPadDefaultType[i]) {
                    case 0:
                        if (bcs::CWifiPad::IsTrg(0x900, i)) {
                            return i;
                        }
                        break;
                    case 1:
                        if (bcs::CWifiPad::IsTrg(0x900, i)) {
                            return i;
                        }
                        break;
                    default:
                        if (bcs::CWifiPad::IsTrg(0x900, i)) {
                            return i;
                        }
                        break;
                }
            }
        }
        return -1;
    }

    void CGame::ScoreBookInitCountWork_()
    {
        for (long i = 0; i < 3; i++) {
            m_anScoreBookCount[i] = 0;
        }
    }

    // ---- ScoreBookApplyCountWork_ (0x80187E80) not written yet ----

    int CGame::MenuCallPause_()
    {
        return 0;
    }

    // Lifts the game pause when the pause controller goes.
    CGamePauseTask::~CGamePauseTask()
    {
        bcs::CMainApp::SetPause(1, 0);
    }

    // ---- CGamePauseTask::Main and its helpers (0x801882F4..0x80188F48) not written yet ----

    void CGamePauseTask::Disp() {}

    // Lifts the hit stop's pause when the task goes.
    CGameStopTask::~CGameStopTask()
    {
        bcs::CMainApp::SetPause(4, 0);
    }

    // ---- CGameStopTask::Main (0x80188FC0) not written yet ----

    void CGameStopTask::Disp() {}

    CGameSpeedCtrl::CGameSpeedCtrl(bcs::CTask *pParent)
        : bcs::CTask(pParent, -1),
          m_pGroup(pParent),
          m_fSpeed(1.0f),
          m_fFrom(1.0f),
          m_fTo(1.0f),
          m_fElapsed(0.0f),
          m_fTime(0.0f),
          m_fUnk54(0.0f),
          m_fUnk58(0.0f),
          m_nUnk5C(0),
          m_nUnk60(1),
          m_nUnk68(1)
    {
        m_Link.pPrev = 0;
        m_Link.pNext = 0;
        if (pParent) {
            nw4r::ut::List_Append(&static_cast<CGameTaskGroup *>(pParent)->m_List, this);
        }
    }

    CGameSpeedCtrl::~CGameSpeedCtrl()
    {
        if (m_pGroup) {
            nw4r::ut::List_Remove(&static_cast<CGameTaskGroup *>(m_pGroup)->m_List, this);
        }
    }

    // ---- 0x801892D0 (the speed control's step) not written yet ----

    void CGameSpeedCtrl::StartSpeedAnim(float from, float to, float time, int nUnk68, long nUnk60)
    {
        if (time > 0.0f) {
            m_fSpeed = m_fFrom = from;
            m_fTo = to;
            m_fElapsed = 0.0f;
            m_fTime = time;
            m_nUnk60 = nUnk60;
        } else {
            m_fFrom = m_fTo = m_fSpeed = to;
            m_nUnk60 = 0;
        }
        m_nUnk5C = 0;
        m_nUnk64 = 0;
        m_nUnk68 = nUnk68;
    }

    void CGameSpeedCtrl::RestoreSpeed(float time, int nUnk68, long nUnk60)
    {
        if (time > 0.0f) {
            m_fFrom = m_fSpeed;
            m_fTo = CGame::g_GameWork.m_fGameSpeed;
            m_fElapsed = 0.0f;
            m_fTime = time;
            m_nUnk60 = nUnk60;
        } else {
            m_fSpeed = m_fTo = m_fFrom = CGame::g_GameWork.m_fGameSpeed;
            m_nUnk60 = 0;
        }
        m_nUnk5C = 0;
        m_nUnk64 = 0;
        m_nUnk68 = nUnk68;
    }

    // Ends the arts event's tasks and gives the game its normal speed back.
    CGameTaskGroup::~CGameTaskGroup()
    {
        DeleteAllChild();
        nw4r::ut::List_Init(&m_List, 0x38);
        bcs::CMainApp::SetUnivSpeed(CGame::g_GameWork.m_fGameSpeed);
    }

    // ---- CGameTaskGroup::Main (0x8018966C) and GameWork::SetDefault (0x80189798) not written yet ----


    void CGame::SetMyTeamIdx(long nTeam)
    {
        g_GameWork.m_nUnk00 = nTeam;
    }


    // 0x80189798..: the settings (all static).
    int CGame::IsVsMode()
    {
        switch (g_GameWork.m_nUnk06) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 14:
            case 15:
            case 16:
            case 17:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                return 1;
            case 0:
            case 2:
            case 4:
            case 6:
            case 9:
            case 11:
            case 12:
            case 13:
            case 18:
            default:
                return 0;
        }
    }

    // (name guessed) IsVsMode, but never in an online game.
    int CGame::IsLocalVsMode()
    {
        if (g_GameWork.m_Unk151) {
            return 0;
        }
        switch (g_GameWork.m_nUnk06) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 14:
            case 15:
            case 16:
            case 17:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                return 1;
            case 0:
            case 2:
            case 4:
            case 6:
            case 9:
            case 11:
            case 12:
            case 13:
            case 18:
            default:
                return 0;
        }
    }

    long CGame::GetHockeyUnk1C()
    {
        if (g_GameWork.m_nGameType) {
            return 0;
        }
        if (g_GameWork.m_nUnk180) {
            return 0;
        }
        if (g_GameWork.m_Unk151) {
            return 0;
        }
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_HOCKEY:
                switch (g_GameWork.m_nUnk06) {
                    case 0:
                    case 2:
                    case 4:
                        return g_GameWork.m_nUnk1C;
                }
                break;
        }
        return 0;
    }

    void CGame::SetGameMode(GAME_MODE mode)
    {
        g_GameWork.m_nGameMode = mode;
    }

    void CGame::SetGameType(GAME_TYPE type)
    {
        g_GameWork.m_nGameType = type;
    }

    // Basketball and hockey have goal rings (name from the symbol).
    int CGame::IsExistGoalRing()
    {
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_BASKET:
                return 1;
            case GAME_MODE_HOCKEY:
                return 1;
            default:
                return 0;
        }
    }

    void CGame::SetPlayType(GAME_PLAY_TYPE type)
    {
        g_GameWork.m_nUnk06 = type;
    }

    void CGame::SetSportsMode(SPORTS_MODE mode)
    {
        g_GameWork.m_nSportsMode = mode;
    }

    void CGame::SetCup(GAME_CUP cup)
    {
        g_GameWork.m_nUnk07 = cup;
    }

    void CGame::SetTournamentLevel(GAME_TOURNAMENT_LEVEL level)
    {
        g_GameWork.m_nTournamentLevel = level;
    }

    void CGame::SetRound(long nRound)
    {
        g_GameWork.m_nRound = nRound;
    }

    void CGame::SetGameLevel(GAME_LEVEL level)
    {
        g_GameWork.m_nGameLevel = level;
    }

    void CGame::SetQPanelLevel(QPANEL_LEVEL level)
    {
        g_GameWork.m_nQPanelLevel = level;
    }

    void CGame::SetStage(long nStage)
    {
        g_GameWork.m_nStage = nStage;
    }

    void CGame::SetStageParam(long nParam)
    {
        g_GameWork.m_nUnk430 = nParam;
    }

    void CGame::SetPartyGameParam(long nIndex, long nValue)
    {
        g_GameWork.m_anPartyGameParam[nIndex] = nValue;
    }

    long CGame::GetPartyGameParam(long nIndex)
    {
        return g_GameWork.m_anPartyGameParam[nIndex];
    }

    void CGame::SetGimmickParamIdx(long nIdx)
    {
        g_GameWork.m_nGimmickParamIdx = nIdx;
    }

    long CGame::GetGimmickParamIdx()
    {
        return g_GameWork.m_nGimmickParamIdx;
    }

    void CGame::SetMaxPeriod(long nPeriod)
    {
        g_GameWork.m_Unk0B = nPeriod;
    }

    void CGame::SetNowPeriod(long nPeriod)
    {
        g_GameWork.m_nPeriod = nPeriod;
    }

    void CGame::SetWinPoint(long nPoint)
    {
        g_GameWork.m_nUnk44 = nPoint;
    }

    void CGame::SetOffenseTeam(long nTeam)
    {
        g_GameWork.m_nUserTeam = nTeam;
    }

    void CGame::SetTurnStartMode(GAME_TURN_START_MODE mode)
    {
        g_GameWork.m_Unk150 = mode;
    }

    void CGame::SetLooseBallMode(int bOn)
    {
        GameWork &rWork = g_GameWork;
        if (bOn) {
            if (!(rWork.m_nFlag18 & 1)) {
                rWork.m_nFlag18 = (rWork.m_nFlag18 & ~0x20) | 1;
                CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x13), 0, 0, 0, 0);
                rWork.m_fUnk38 = 0.0f;
            }
        } else {
            if (rWork.m_nFlag18 & 1) {
                rWork.m_nFlag18 &= ~1;
                CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(0x14), 0, 0, 0, 0);
                rWork.m_fUnk38 = 0.0f;
            }
        }
    }

    void CGame::SetLooseBallModeFlag(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 1;
        } else {
            g_GameWork.m_nFlag18 &= ~1;
        }
    }

    void CGame::IncBallModeCounter(float fRate)
    {
        g_GameWork.m_fUnk38 += fRate;
    }

    void CGame::SetChgFieldDemoMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x8000;
        } else {
            g_GameWork.m_nFlag18 &= ~0x8000;
        }
    }

    // (name guessed) Bit 0x2000 of the flags.
    void CGame::SetFlag2000Mode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x2000;
        } else {
            g_GameWork.m_nFlag18 &= ~0x2000;
        }
    }

    void CGame::SetPreparationMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x4000;
        } else {
            g_GameWork.m_nFlag18 &= ~0x4000;
        }
    }

    void CGame::SetReadyDemoMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 4;
        } else {
            g_GameWork.m_nFlag18 &= ~4;
        }
    }

    void CGame::SetJumpBallMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 8;
        } else {
            g_GameWork.m_nFlag18 &= ~8;
        }
    }

    // The same bit as the jump ball.
    void CGame::SetVolleyServeMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 8;
        } else {
            g_GameWork.m_nFlag18 &= ~8;
        }
    }

    void CGame::SetDodgeArtsCatchMode(int bOn, long n)
    {
        if (m_pInstance) {
            m_pInstance->SetDodgeArtsCatchMode_(bOn, n);
        }
    }

    // Not in the DOL (stripped by the link, like GetDodgeChrMaxLife below);
    // the weak IsDodgeArtsCatchMode_ placed here shows it existed.
    int CGame::IsDodgeArtsCatchMode()
    {
        if (m_pInstance) {
            return m_pInstance->IsDodgeArtsCatchMode_();
        }
        return 0;
    }

    // (name guessed) The dodgeball game's variable character count; 0
    // without a game.
    long CGame::GetDodgeVariableChrNum(long n)
    {
        if (m_pInstance) {
            return m_pInstance->GetDodgeVariableChrNum_(n);
        }
        return 0;
    }

    long CGame::GetDodgeChrMaxLife(long n)
    {
        if (m_pInstance) {
            return m_pInstance->GetDodgeChrMaxLife_(n);
        }
        return 0;
    }

    void CGame::SetPassCountLimit_(long nTeam, long nLimit)
    {
        g_GameWork.m_anPassCountLimit[nTeam] = nLimit;
    }

    long CGame::GetPassCountLimit_(long nTeam) const
    {
        return g_GameWork.m_anPassCountLimit[nTeam];
    }

    void CGame::SetReboundMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x20;
        } else {
            g_GameWork.m_nFlag18 &= ~0x20;
        }
    }

    void CGame::SetGoalEventMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x40;
        } else {
            g_GameWork.m_nFlag18 &= ~0x40;
        }
    }

    void CGame::SetGoalFinishMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x80;
        } else {
            g_GameWork.m_nFlag18 &= ~0x80;
        }
    }

    void CGame::SetReplayMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x100;
        } else {
            g_GameWork.m_nFlag18 &= ~0x100;
        }
    }

    void CGame::SetResultMode(int bOn)
    {
        if (bOn) {
            g_GameWork.m_nFlag18 |= 0x200;
        } else {
            g_GameWork.m_nFlag18 &= ~0x200;
        }
    }

    void CGame::SetMaxTime(float fTime)
    {
        g_GameWork.m_fUnk20 = fTime >= 0.0f ? fTime : 0.0f;
    }

    void CGame::SetTime(float fTime)
    {
        g_GameWork.m_fGameTime = fTime;
        g_GameWork.m_fGameTime = CLAMP(fTime, 0.0f, g_GameWork.m_fUnk20);
    }

    void CGame::SetMaxShotTime(float fTime)
    {
        g_GameWork.m_fMaxShotTime = fTime;
    }

    float CGame::GetMaxShotTime()
    {
        if (m_pInstance) {
            return m_pInstance->GetMaxShotTime_();
        }
        return 0.0f;
    }

    void CGame::SetShotTime(float fTime)
    {
        g_GameWork.m_fShotTime = fTime;
        g_GameWork.m_fShotTime = CLAMP(g_GameWork.m_fShotTime, 0.0f, GetMaxShotTime());
    }

    void CGame::SetCharType(long nChr, long nType)
    {
        g_GameWork.m_anCharType[nChr] = nType;
    }

    long CGame::GetCharType(long nChr)
    {
        return g_GameWork.m_anCharType[nChr];
    }

    void CGame::SetCharColor(long nChr, long nColor)
    {
        g_GameWork.m_anCharColor[nChr] = nColor;
    }

    long CGame::GetCharColor(long nChr)
    {
        return g_GameWork.m_anCharColor[nChr];
    }

    void CGame::SetCharExFlag(long nChr, unsigned long nFlag)
    {
        g_GameWork.m_anCharExFlag[nChr] = nFlag;
    }

    void CGame::AddCharExFlag(long nChr, unsigned long nFlag)
    {
        g_GameWork.m_anCharExFlag[nChr] |= (unsigned char)nFlag;
    }

    void CGame::ResetCharExFlag(long nChr, unsigned long nFlag)
    {
        g_GameWork.m_anCharExFlag[nChr] &= (unsigned char)~nFlag;
    }

    int CGame::IsCharEx_DodgeOutfielder(long nChr)
    {
        return (g_GameWork.m_anCharExFlag[nChr] >> 1) & 1;
    }

    int CGame::IsCharEx_HockeyKeeper(long nChr)
    {
        return (g_GameWork.m_anCharExFlag[nChr] >> 1) & 1;
    }

    int CGame::IsCharEx_StarChar(long nChr)
    {
        return (g_GameWork.m_anCharExFlag[nChr] >> 2) & 1;
    }

    // Copies the name of character nChr into pBuf (nBufLen characters at
    // most, terminated when there is room); the number copied.
    long CGame::GetCharName(long nChr, wchar_t *pBuf, long nBufLen)
    {
        long nLen = 0;
        if (g_GameWork.m_anCharType[nChr] >= 0) {
            const wchar_t *pName = g_GameWork.m_aCharName[nChr];
            while (*pName != 0 && nLen < nBufLen) {
                *pBuf++ = *pName++;
                nLen++;
            }
        }
        if (nLen < nBufLen) {
            *pBuf = 0;
        }
        return nLen;
    }

    long CGame::GetCharNameLen(long nChr)
    {
        long nLen = 0;
        const wchar_t *pName = g_GameWork.m_aCharName[nChr];
        if (pName) {
            while (*pName != 0) {
                pName++;
                nLen++;
            }
        }
        return nLen;
    }

    const wchar_t *CGame::GetCharName(long nChr)
    {
        return g_GameWork.m_aCharName[nChr];
    }

    void CGame::SetCharName(long nChr, const wchar_t *pName, long nLen)
    {
        wchar_t *pDst = g_GameWork.m_aCharName[nChr];
        long n = 0;
        if (pName) {
            while (*pName != 0 && n < nLen) {
                *pDst++ = *pName++;
                n++;
            }
        }
        if (n < nLen) {
            *pDst = 0;
        }
    }

    long CGame::GetNpcType(long nSlot)
    {
        return g_GameWork.m_anNpcType[nSlot];
    }

    // (name guessed) The signed byte at 0x78 + nTeam.
    long CGame::GetTeamUnk78(long nTeam)
    {
        return g_GameWork.m_anUnk78[nTeam];
    }

    void CGame::SetTeamCtrlType(long nTeam, long nType)
    {
        g_GameWork.m_anTeamCtrlType[nTeam] = nType;
    }

    long CGame::GetTeamCtrlType(long nTeam)
    {
        return g_GameWork.m_anTeamCtrlType[nTeam];
    }

    void CGame::SetTeamColorIdx(long nTeam, long nColor)
    {
        g_GameWork.m_anTeamColorIdx[nTeam] = nColor;
    }

    long CGame::GetTeamColorIdx(long nTeam)
    {
        return g_GameWork.m_anTeamColorIdx[nTeam];
    }

    // (name guessed) The local pad of online pad nPad.
    long CGame::GetLocalPadNo(long nPad)
    {
        return g_GameWork.m_anUnk84[nPad];
    }

    void CGame::SetTeamComLevel(long nTeam, float fLevel)
    {
        g_GameWork.m_afTeamComLevel[nTeam] = fLevel;
    }

    float CGame::GetTeamComLevel(long nTeam)
    {
        return g_GameWork.m_afTeamComLevel[nTeam];
    }

    void CGame::SetGoalIdx(long nTeam, long nGoal)
    {
        g_GameWork.m_anGoalIdx[nTeam] = nGoal;
        if (nTeam < 2) {
            g_GameWork.m_anGoalTeam[nGoal] = nTeam;
        }
    }

    long CGame::GetGoalIdx(long nTeam)
    {
        return g_GameWork.m_anGoalIdx[nTeam];
    }

    long CGame::GetTargetGoalIdx(long nTeam)
    {
        return g_GameWork.m_anGoalIdx[nTeam];
    }

    long CGame::GetGuardGoalIdx(long nTeam)
    {
        return 1 - g_GameWork.m_anGoalIdx[nTeam];
    }

    long CGame::GetTeamIdxByGoalIdx(long nGoal)
    {
        return g_GameWork.m_anGoalTeam[nGoal];
    }

    const bcs::FVEC3 &CGame::GetGoalPos(long nGoal)
    {
        return CStageMan::GetInstance()->GetGoalPos(nGoal);
    }

    const bcs::FVEC3 &CGame::GetGoalDir(long nGoal)
    {
        return CStageMan::GetInstance()->GetGoalVec(nGoal);
    }

    float CGame::GetGoalYRot(long nGoal)
    {
        return CStageMan::GetInstance()->GetGoalRot(nGoal).y;
    }

    const bcs::FVEC3 &CGame::GetBoardPos(long nGoal)
    {
        return CStageMan::GetInstance()->GetBoardPos(nGoal);
    }

    const bcs::FVEC3 &CGame::GetBoardDir(long nGoal)
    {
        return CStageMan::GetInstance()->GetGoalVec(nGoal);
    }

    // (name guessed) View nView shows the side of team nTeam.
    void CGame::SetViewTeam(long nView, long nTeam)
    {
        g_GameWork.m_anUnk1A0[nView] = nTeam;
        g_GameWork.m_anUnk1A4[nTeam] = nView;
    }

    long CGame::GetViewTargetTeam(long nView)
    {
        if (g_GameWork.m_Unk151) {
            switch (g_GameWork.m_nGameMode) {
                case GAME_MODE_VOLLEY:
                    return g_GameWork.m_nUnk00;
                case GAME_MODE_DODGE:
                    return g_GameWork.m_nUnk00;
            }
        }
        return g_GameWork.m_anUnk1A0[nView];
    }

    void CGame::ClearLastGoalInfo()
    {
        GameWork &rWork = g_GameWork;
        rWork.m_fGoalShootRate = 0.0f;
        rWork.m_nGoalTeam = 0;
        rWork.m_nGoalChar = 0;
        rWork.m_nGoalBaseScore = 0;
        rWork.m_Unk160 = 0;
        rWork.m_nGoalScore = 0;
        rWork.m_nGoalShootType = 0;
        rWork.m_nGoalBonusType = 0;
        rWork.m_nGoalBonusColor = 0;
        rWork.m_nGoalBonusPoint = 0;
        rWork.m_nGoalOwnGoal = 0;
        ClearTeamDisableDropCoin(0);
    }

    void CGame::SetGoalBaseScore(long n)
    {
        g_GameWork.m_nGoalBaseScore = n;
    }

    void CGame::SetGoalCoin(long n)
    {
        g_GameWork.m_Unk160 = n;
    }

    void CGame::SetGoalEffectCoin(long n)
    {
        g_GameWork.m_Unk160 = n;
    }

    void CGame::SetGoalScore(long n)
    {
        g_GameWork.m_nGoalScore = n;
    }

    void CGame::SetGoalBonusPoint(long n)
    {
        g_GameWork.m_nGoalBonusPoint = n;
    }

    void CGame::SetGoalBonusRate(long n)
    {
        g_GameWork.m_nGoalBonusPoint = n;
    }

    void CGame::SetGoalBonusColor(long n)
    {
        g_GameWork.m_nGoalBonusColor = n;
    }

    void CGame::SetGoalBonusType(long n)
    {
        g_GameWork.m_nGoalBonusType = n;
    }

    void CGame::SetGoalShootType(long nType, float fRate)
    {
        g_GameWork.m_nGoalShootType = nType;
        g_GameWork.m_fGoalShootRate = CLAMP(fRate, 0.0f, 1.0f);
    }

    void CGame::SetGoalTeam(long nTeam)
    {
        g_GameWork.m_nGoalTeam = nTeam;
    }

    void CGame::SetGoalChar(long nChr)
    {
        g_GameWork.m_nGoalChar = nChr;
    }

    void CGame::SetGoalOwnGoal(int bOwnGoal)
    {
        g_GameWork.m_nGoalOwnGoal = bOwnGoal;
    }

    void CGame::SetPeriodStartScore(long nTeam, long nScore)
    {
        if (nScore < 0) {
            nScore = 0;
        }
        g_GameWork.m_anPeriodStartScore[nTeam] = nScore;
    }

    long CGame::GetPeriodStartScore(long nTeam)
    {
        return g_GameWork.m_anPeriodStartScore[nTeam];
    }

    long CGame::GetTeamScore(long nTeam)
    {
        return g_ScoreWork.m_anScore[nTeam];
    }

    long CGame::GetTeamPeriodScore(long nTeam, long nPeriod)
    {
        return g_ScoreWork.m_aanPeriodScore[nTeam][nPeriod];
    }

    // The score of team nTeam is the sum of its period scores.
    void CGame::AddTeamPeriodScore(long nTeam, long nPeriod, long nScore)
    {
        g_ScoreWork.m_aanPeriodScore[nTeam][nPeriod] += nScore;
        g_ScoreWork.m_anScore[nTeam] = 0;
        for (long i = 0; i < 5; i++) {
            g_ScoreWork.m_anScore[nTeam] += g_ScoreWork.m_aanPeriodScore[nTeam][i];
        }
    }

    void CGame::SetTeamPeriodScore(long nTeam, long nPeriod, long nScore)
    {
        g_ScoreWork.m_aanPeriodScore[nTeam][nPeriod] = nScore;
        g_ScoreWork.m_anScore[nTeam] = 0;
        for (long i = 0; i < 5; i++) {
            g_ScoreWork.m_anScore[nTeam] += g_ScoreWork.m_aanPeriodScore[nTeam][i];
        }
    }

    void CGame::SetTeamMaxLife(long nTeam, long nLife)
    {
        if (nLife < 0) {
            nLife = 0;
        }
        g_GameWork.m_anTeamMaxLife[nTeam] = nLife;
    }

    long CGame::GetTeamMaxLife(long nTeam)
    {
        return g_GameWork.m_anTeamMaxLife[nTeam];
    }

    void CGame::SetTeamInitLife(long nTeam, long nLife)
    {
        if (nLife < 0) {
            nLife = 0;
        }
        g_GameWork.m_anTeamInitLife[nTeam] = nLife;
    }

    long CGame::GetTeamInitLife(long nTeam)
    {
        return g_GameWork.m_anTeamInitLife[nTeam];
    }

    void CGame::SetTeamRebirthLife(long nTeam, long nLife)
    {
        if (nLife < 0) {
            nLife = 0;
        }
        g_GameWork.m_anTeamRebirthLife[nTeam] = nLife;
    }

    long CGame::GetTeamRebirthLife(long nTeam)
    {
        return g_GameWork.m_anTeamRebirthLife[nTeam];
    }

    void CGame::SetPeriodStartCoin(long nTeam, long nCoin)
    {
        if (nCoin < 0) {
            nCoin = 0;
        }
        g_GameWork.m_anPeriodStartCoin[nTeam] = nCoin;
    }

    long CGame::GetPeriodStartCoin(long nTeam)
    {
        return g_GameWork.m_anPeriodStartCoin[nTeam];
    }

    long CGame::GetTeamCoin(long nTeam)
    {
        if (nTeam < 0) {
            return 0;
        }
        return g_GameWork.m_anTeamCoin[nTeam];
    }

    void CGame::ClearTeamCoin(long nTeam)
    {
        g_GameWork.m_anTeamCoin[nTeam] = 0;
        CCockpit::SetTeamCoin(nTeam, GetTeamCoin(nTeam));
    }

    void CGame::SetTeamCoin(long nTeam, unsigned long nCoin)
    {
        if (nTeam >= 0) {
            g_GameWork.m_anTeamCoin[nTeam] = nCoin > GetMaxCoin() ? GetMaxCoin() : nCoin;
            CCockpit::SetTeamCoin(nTeam, GetTeamCoin(nTeam));
        }
    }

    void CGame::AddTeamCoin(long nTeam, long nCoin)
    {
        if (m_pInstance) {
            m_pInstance->AddTeamCoin_(nTeam, nCoin);
        }
    }

    unsigned long CGame::GetTeamDisableDropCoin(long nTeam)
    {
        return g_GameWork.m_anTeamDisableDropCoin[nTeam];
    }

    // Takes a team but clears every team's flags.
    void CGame::ClearTeamDisableDropCoin(long)
    {
        for (long i = 0; i < 4; i++) {
            g_GameWork.m_anTeamDisableDropCoin[i] = 0;
        }
    }

    void CGame::SetTeamDisableDropCoin(long nTeam, unsigned long nFlag)
    {
        g_GameWork.m_anTeamDisableDropCoin[nTeam] = nFlag;
    }

    void CGame::UseTeamCoin(long nTeam, long nCoin)
    {
        if (m_pInstance) {
            m_pInstance->UseTeamCoin_(nTeam, nCoin);
        }
    }

    // Adds nCoin to the coins team nTeam has won in the game (at most 9999).
    void CGame::AddTeamGainCoin(long nTeam, long nCoin)
    {
        if (nTeam >= 0) {
            long nTotal = nCoin + g_ScoreWork.m_anTotalCoin[nTeam];
            if (nTotal > 9999) {
                nTotal = 9999;
            }
            g_ScoreWork.m_anTotalCoin[nTeam] = nTotal;
        }
    }

    void CGame::ClearCoinHistory()
    {
        for (long i = 0; i < 4; i++) {
            g_GameWork.m_anCoinHistCount[i] = 0;
            g_GameWork.m_anCoinHistTime[i] = 0;
        }
    }

    // Counts the coin history's timers down; a count is cleared when its
    // timer runs out.
    void CGame::IncCoinHistCount(float)
    {
        GameWork &rWork = g_GameWork;
        for (long i = 0; i < 4; i++) {
            if (rWork.m_anCoinHistTime[i] != 0 && --rWork.m_anCoinHistTime[i] == 0) {
                rWork.m_anCoinHistCount[i] = 0;
            }
        }
    }

    // (name guessed) Team nTeam has no item.
    void CGame::ClearTeamItem(long nTeam)
    {
        g_GameWork.m_anTeamItem[nTeam] = -1;
    }

    long CGame::GetTeamItem(long nTeam, long)
    {
        return g_GameWork.m_anTeamItem[nTeam];
    }

    void CGame::PushTeamItem(long nTeam, long nItem)
    {
        if (m_pInstance) {
            m_pInstance->PushTeamItem_(nTeam, nItem);
        }
    }

    // (name guessed) Takes team nTeam's item; the item it had.
    long CGame::PopTeamItem(long nTeam)
    {
        if (m_pInstance) {
            m_pInstance->PopTeamItem_(nTeam);
        }
        long nItem = g_GameWork.m_anTeamItem[nTeam];
        g_GameWork.m_anTeamItem[nTeam] = -1;
        return nItem;
    }

    void CGame::TeamItemStartRoulette(long nTeam, long n)
    {
        if (m_pInstance) {
            m_pInstance->TeamItemStartRoulette_(nTeam, n);
        }
    }

    float CGame::GetTeamArtsGauge(long nTeam)
    {
        if (m_pInstance) {
            return m_pInstance->GetTeamArtsGauge_(nTeam);
        }
        return 0.0f;
    }

    float CGame::GetTeamArtsGauge_(long nTeam)
    {
        return m_afTeamArtsGauge[nTeam];
    }

    void CGame::ClearTeamArtsGauge(long nTeam)
    {
        if (m_pInstance) {
            m_pInstance->ClearTeamArtsGauge_(nTeam);
        }
    }

    void CGame::ClearTeamArtsGauge_(long nTeam)
    {
        m_afTeamArtsGauge[nTeam] = 0.0f;
        SetCockpitArtsGaugeSpAnim_(nTeam, 0);
        SetCockpitArtsGaugeRatio_(nTeam, 0.0f, 0.0f);
    }

    float CGame::AddTeamArtsGauge(long nTeam, float f1, float f2)
    {
        if (m_pInstance) {
            return m_pInstance->AddTeamArtsGauge_(nTeam, f1, f2);
        }
        return 0.0f;
    }

    float CGame::SetTeamArtsGauge(long nTeam, float f1, float f2)
    {
        if (m_pInstance) {
            return m_pInstance->SetTeamArtsGauge_(nTeam, f1, f2);
        }
        return 0.0f;
    }

    // Adds fAdd to team nTeam's arts gauge (0 to 1) and moves the cockpit's
    // gauge over fFrame frames (60 when negative); a gauge that fills up
    // plays the full sound and starts the gauge's special animation.
    float CGame::AddTeamArtsGauge_(long nTeam, float fAdd, float fFrame)
    {
        float fOld = m_afTeamArtsGauge[nTeam];
        float fNew = CLAMP(fOld + fAdd, 0.0f, 1.0f);
        m_afTeamArtsGauge[nTeam] = fNew;
        if (fFrame < 0.0f) {
            fFrame = 60.0f;
        }
        if (fOld < 1.0f && fNew >= 1.0f) {
            CGameSnd::Play(0xC08, 0, 1, -1, 0.0f, 0.0f, 0.0f);
            SetCockpitArtsGaugeRatio_(nTeam, fNew, 0.0f);
            SetCockpitArtsGaugeSpAnim_(nTeam, 1);
        } else if (fOld >= 1.0f && fNew < 1.0f) {
            SetCockpitArtsGaugeRatio_(nTeam, fNew, fFrame);
            SetCockpitArtsGaugeSpAnim_(nTeam, 0);
        } else if (fOld < 1.0f && fNew < 1.0f) {
            SetCockpitArtsGaugeRatio_(nTeam, fNew, fFrame);
        }
        return fNew;
    }

    // Sets team nTeam's arts gauge to fGauge (0 to 1); the cockpit as in
    // AddTeamArtsGauge_.
    float CGame::SetTeamArtsGauge_(long nTeam, float fGauge, float fFrame)
    {
        float fNew = CLAMP(fGauge, 0.0f, 1.0f);
        float fOld = m_afTeamArtsGauge[nTeam];
        m_afTeamArtsGauge[nTeam] = fNew;
        if (fFrame < 0.0f) {
            fFrame = 60.0f;
        }
        if (fOld < 1.0f && fNew >= 1.0f) {
            CGameSnd::Play(0xC08, 0, 1, -1, 0.0f, 0.0f, 0.0f);
            SetCockpitArtsGaugeRatio_(nTeam, fNew, 0.0f);
            SetCockpitArtsGaugeSpAnim_(nTeam, 1);
        } else if (fOld >= 1.0f && fNew < 1.0f) {
            SetCockpitArtsGaugeRatio_(nTeam, fNew, fFrame);
            SetCockpitArtsGaugeSpAnim_(nTeam, 0);
        } else if (fOld < 1.0f && fNew < 1.0f) {
            SetCockpitArtsGaugeRatio_(nTeam, fNew, fFrame);
        }
        return fNew;
    }

    void CGame::SetCockpitArtsGaugeDisp_(int bDisp)
    {
        CCockpit::SetSPGaugeDispEnable(bDisp);
    }

    void CGame::SetCockpitArtsGaugeRatio_(long, float, float) {}

    void CGame::SetCockpitArtsGaugeSpAnim_(long, int) {}

    void CGame::SetDispRadarIcon_(int) {}

    void CGame::SetDispChrHeadIcon_(int bDisp)
    {
        fn_8013A7A8(0x1004, bDisp);
        fn_8013A7A8(0x1005, bDisp);
    }

    // ---- SetCockpitItemIconDisp_, PushTeamItem_ (0x8018B648..0x8018B860) not written yet ----

    // Takes team nTeam's item back: a roulette that is waiting drops its
    // icon, one that is spinning is stopped (state 2). Nothing while rule
    // option bit 2 is set.
    void CGame::PopTeamItem_(long nTeam)
    {
        if ((g_GameWork.m_nUnk40 >> 2) & 1) {
            return;
        }
        CItemRoulette *pRoulette = m_apItemRoulette[nTeam];
        if (pRoulette) {
            switch (pRoulette->m_nState) {
                case 0:
                    pRoulette->m_nUnk7C = -1;
                    break;
                case 1:
                    pRoulette->ChangeState(2);
                    break;
            }
        }
    }

    // ---- TeamItemStartRoulette_ (0x8018B8C8) not written yet ----

    int CGame::IsTeamItemRouletteRole_(long nTeam)
    {
        CItemRoulette *pRoulette = m_apItemRoulette[nTeam];
        if (pRoulette) {
            return (unsigned long)pRoulette->m_nState > 2;
        }
        return 0;
    }

    int CGame::CanbeTeamItemPush(long nTeam)
    {
        if (m_pInstance) {
            return m_pInstance->CanbeTeamItemPush_(nTeam);
        }
        return 0;
    }

    int CGame::CanbeTeamItemPush_(long nTeam)
    {
        CItemRoulette *pRoulette = m_apItemRoulette[nTeam];
        if (pRoulette) {
            switch (pRoulette->m_nState) {
                case 0:
                case 2:
                    return 1;
                default:
                    return 0;
            }
        }
        return 0;
    }

    void CGame::SetCockpitDisp_(int) {}

    void CGame::SetCockpitCoinDisp_(int) {}

    void CGame::SetCockpitTimeDisp_(int) {}

    void CGame::SetCockpitScoreDisp_(int) {}

    void CGame::SetCockpitPeriodDisp_(int) {}

    void CGame::SetCockpitSetCountDisp_(int) {}

    void CGame::SetCockpitLifeDisp_(int) {}

    void CGame::SetCockpitPassBallDisp_(int) {}

    void CGame::SetCockpitMapDisp_(int) {}

    void CGame::TelopCreate_(long nTelop, int bAnimEndClose, long nParam)
    {
        if (nTelop >= 0) {
            CCockpit::CreateObj(nTelop, nParam);
            CCockpit::SetAnimEndClose(nTelop, bAnimEndClose);
        }
    }

    void CGame::TelopEnd_(long nTelop)
    {
        if (nTelop >= 0) {
            CCockpit::PlayEndAnim(nTelop);
        }
    }

    void CGame::TelopRemove_(long nTelop)
    {
        if (nTelop >= 0) {
            CCockpit::RemoveObj(nTelop);
        }
    }

    int CGame::TelopIsWait_(long nTelop)
    {
        if (nTelop < 0) {
            return 1;
        }
        return CCockpit::IsEndOfStartAnim(nTelop);
    }

    int CGame::TelopIsEnd_(long nTelop)
    {
        if (nTelop < 0) {
            return 1;
        }
        return CCockpit::IsEndObj(nTelop);
    }

    long CGame::GetGoalDemoType_(long, long) const
    {
        return -1;
    }

    // Plays the goal demo with a chance of nRate percent.
    int CGame::CheckGoalDemoRnd_(long nRate) const
    {
        if (nRate <= 0) {
            return 0;
        }
        if (nRate >= 100) {
            return 1;
        }
        return bcs::BCS_RandMax(99L) < nRate;
    }

    // The goal demo: the cockpit is hidden while it plays.
    void CGame::StartGoalDemo_(long nType, long nTeam, const bcs::FVEC3 &rPos, long nSide, long nTelop, float fSpeed)
    {
        EndGoalDemo_();
        CGameGoalDemo *pDemo = new (0x100) CGameGoalDemo(this, nType, nTeam, rPos, nSide, nTelop, fSpeed);
        if (pDemo) {
            SetCockpitDisp_(0);
            SetCockpitItemIconDisp_(0, 0);
            SetDispRadarIcon_(0);
            SetReplayMode(1);
            m_nGoalDemoId = pDemo->GetId();
        }
    }

    int CGame::IsEndGoalDemo_()
    {
        if (!m_nGoalDemoId) {
            return 1;
        }
        CGameGoalDemo *pDemo = static_cast<CGameGoalDemo *>(bcs::CTaskMan::GetTask(m_nGoalDemoId));
        if (!pDemo || pDemo->IsDelete()) {
            m_nGoalDemoId = 0;
            return 1;
        }
        return pDemo->IsEnd();
    }

    void CGame::EndGoalDemo_()
    {
        SetCockpitDisp_(1);
        SetCockpitItemIconDisp_(0, 0);
        SetDispRadarIcon_(0);
        SetDispChrHeadIcon_(0);
        SetReplayMode(0);
        if (m_nGoalDemoId) {
            CGameGoalDemo *pDemo = static_cast<CGameGoalDemo *>(bcs::CTaskMan::GetTask(m_nGoalDemoId));
            m_nGoalDemoId = 0;
            if (pDemo && !pDemo->IsDelete()) {
                pDemo->End();
            }
        }
    }

    // The speed lines over the screen for fFrame frames (one at a time).
    void CGame::StartSaturatedLinework(float fFrame)
    {
        CGame *pGame = m_pInstance;
        if (pGame) {
            pGame->EndSaturatedLinework_();
            CGameSaturatedLinework *pLines = new (0x100) CGameSaturatedLinework(pGame, fFrame);
            if (pLines) {
                pGame->m_nSatLineId = pLines->GetId();
            }
        }
    }

    void CGame::EndSaturatedLinework()
    {
        CGame *pGame = m_pInstance;
        if (pGame) {
            pGame->EndSaturatedLinework_();
        }
    }

    int CGame::CanbeScoredView_()
    {
        return fn_800EE434();
    }

    // The view of the team that has scored (shown before the next play).
    void CGame::StartScoredView_(long nTeam)
    {
        EndScoredView_();
        SetCockpitItemIconDisp_(0, 0);
        SetDispRadarIcon_(0);
        SetCockpitArtsGaugeDisp_(0);
        SetDispChrHeadIcon_(0);
        fn_80269E9C();
        fn_800EE5A0(nTeam);
        CGameScoredView *pView = new (0x100) CGameScoredView(this);
        if (pView) {
            pView->Start(nTeam);
            m_nScoredViewId = pView->GetId();
        }
    }

    void CGame::EndScoredView_()
    {
        if (m_nScoredViewId) {
            CGameScoredView *pView = static_cast<CGameScoredView *>(bcs::CTaskMan::GetTask(m_nScoredViewId));
            if (pView && !pView->IsDelete()) {
                pView->End();
                pView->Delete();
            }
            m_nScoredViewId = 0;
        }
    }

    void *CGame::GetMiiStoreData(long nChr)
    {
        return &g_GameWork.m_aMiiStoreData[nChr];
    }

    void CGame::SetMiiStoreData(long nChr, const RFLStoreData &rData)
    {
        g_GameWork.m_aMiiStoreData[nChr] = rData;
    }

    // Mii characters are type 0x13, or 0x14 when the Mii's top bit at 0x34
    // (taken to be the sex) is set.
    long CGame::GetMiiChrType(RFLDataSource source, long nIndex)
    {
        RFLAdditionalInfo info;
        long nType = 0x13;
        if (bcs::CMiiMan::GetAdditionalInfo(info, source, 0, 0, nIndex) == 0) {
            nType = 0x13;
            if (info.sex == 1) {
                nType = 0x14;
            }
        }
        return nType;
    }

    long CGame::GetMiiChrType(RFLStoreData &rData)
    {
        RFLAdditionalInfo info;
        long nType = 0x13;
        if (bcs::CMiiMan::GetAdditionalInfo(info, RFLDataSource_MiddleDB, 0, &rData, 0) == 0) {
            nType = 0x13;
            if (info.sex == 1) {
                nType = 0x14;
            }
        }
        return nType;
    }

    // (name guessed) Copies the name of Mii rData into pBuf (nBufLen
    // characters at most, with the terminator when there is room); the
    // number of characters copied.
    long CGame::GetMiiName(RFLStoreData &rData, wchar_t *pBuf, long nBufLen)
    {
        RFLAdditionalInfo info;
        long nLen = 0;
        if (bcs::CMiiMan::GetAdditionalInfo(info, RFLDataSource_MiddleDB, 0, &rData, 0) == 0) {
            const wchar_t *pName = info.name;
            while (*pName != 0 && nLen < nBufLen - 1) {
                *pBuf++ = *pName++;
                nLen++;
            }
        }
        if (nLen < nBufLen) {
            *pBuf = 0;
        }
        return nLen;
    }

    void CGame::SetTeamResult(long nTeam, long nResult)
    {
        g_GameWork.m_anTeamResult[nTeam] = nResult;
    }

    long CGame::GetTeamResult(long nTeam)
    {
        return g_GameWork.m_anTeamResult[nTeam];
    }

    // (no name; between GetTeamResult and SetCharAssignPlayerNo) Gives its
    // argument back: CChrMan::GetAssignPadNo_ and the menus map a player to
    // a pad number with it, which is the same number in this build.
    extern "C" long fn_8018C298(long n)
    {
        return n;
    }

    void CGame::SetCharAssignPlayerNo(long nChr, long nPlayer)
    {
        g_GameWork.m_anCharAssignPlayerNo[nChr] = nPlayer;
    }

    long CGame::GetCharAssignPlayerNo(long nChr)
    {
        return g_GameWork.m_anCharAssignPlayerNo[nChr];
    }

    void CGame::SetCharAssignPadNo(long nChr, long nPad)
    {
        g_GameWork.m_anCharAssignPadNo[nChr] = nPad;
    }

    long CGame::GetCharAssignPadNo(long nChr)
    {
        return g_GameWork.m_anCharAssignPadNo[nChr];
    }

    void CGame::SetPadDefaultType(long nPad, long nType)
    {
        g_GameWork.m_anPadDefaultType[nPad] = nType;
    }

    long CGame::GetPadDefaultType(long nPad)
    {
        return g_GameWork.m_anPadDefaultType[nPad];
    }

    void CGame::SetTeamChrChangeCtrlType(long nTeam, long nType)
    {
        g_GameWork.m_anTeamChrChangeCtrlType[nTeam] = nType;
    }

    long CGame::GetTeamChrChangeCtrlType(long nTeam)
    {
        return g_GameWork.m_anTeamChrChangeCtrlType[nTeam];
    }

    void CGame::SetTeamAutoSpikeJumpType(long nTeam, int bAuto)
    {
        if (bAuto) {
            g_GameWork.m_abTeamAutoSpikeJump[nTeam] = true;
        } else {
            g_GameWork.m_abTeamAutoSpikeJump[nTeam] = false;
        }
    }

    long CGame::GetTeamAutoSpikeJumpType(long nTeam)
    {
        return g_GameWork.m_abTeamAutoSpikeJump[nTeam];
    }

    // (name guessed) The automatic spike jump for every team.
    void CGame::SetAllTeamAutoSpikeJumpType(int bAuto)
    {
        for (long i = 0; i < 4; i++) {
            SetTeamAutoSpikeJumpType(i, bAuto);
        }
    }

    void CGame::SetTeamSkillPoint(long nTeam, long nPoint)
    {
        g_GameWork.m_anTeamSkillPoint[nTeam] = nPoint;
    }

    // (name guessed) The skill points of this console's team.
    long CGame::GetMyTeamSkillPoint()
    {
        return g_GameWork.m_anTeamSkillPoint[g_GameWork.m_nUnk00];
    }

    void CGame::EnableRuleOptionFlag(unsigned long nFlag, int bEnable)
    {
        if (nFlag & 0x80) {
            for (long i = 0; i < 4; i++) {
                SetTeamAutoSpikeJumpType(i, bEnable);
            }
        }
        if (bEnable) {
            g_GameWork.m_nUnk40 |= nFlag;
        } else {
            g_GameWork.m_nUnk40 &= ~nFlag;
        }
    }

    void CGame::SetRuleOptionFlag(unsigned long nFlag)
    {
        g_GameWork.m_nUnk40 = nFlag;
    }

    void CGame::SetReturnStep(long nStep)
    {
        g_GameWork.m_nReturnStep = nStep;
    }

    void CGame::SetStartFadeType(long nType)
    {
        g_GameWork.m_nStartFadeType = nType;
    }

    void CGame::SetCockpitPartMove(long n1, long n2)
    {
        if (m_pInstance) {
            m_pInstance->SetCockpitPartMove_(n1, n2);
        }
    }

    void CGame::SetCockpitPartMove_(long, long) {}

    void CGame::SetCockpitFaceIcon()
    {
        if (m_pInstance) {
            m_pInstance->SetCockpitFaceIcon_();
        }
    }

    void CGame::SetCockpitFaceIcon_()
    {
        CCockpit::InitFaceIcon();
    }

    void CGame::SetCockpitCoinGetTelopMove(long n1, long n2)
    {
        if (m_pInstance) {
            m_pInstance->SetCockpitCoinGetTelopMove_(n1, n2);
        }
    }

    void CGame::SetCockpitCoinGetTelopMove_(long, long) {}

    void CGame::SetScoreDemoTeamScore_(long, long) {}

    void CGame::SetScoreDemoTeamCoin_(long, long) {}

    // The slow-motion task of the game (under the arts event tasks).
    CGameSpeedCtrl *CGame::CreateSpeedCtrl()
    {
        if (m_pInstance) {
            bcs::CTask *pParent = m_pInstance->m_pArtsTaskRoot;
            if (pParent) {
                return new (0x100) CGameSpeedCtrl(pParent);
            }
            return 0;
        }
        return 0;
    }

    void CGame::SetDefaultParam()
    {
        g_GameWork.SetDefault();
        memset(&g_ScoreWork, 0, sizeof(g_ScoreWork));
    }

    // ---- SetParamSemiAuto, SetPadAssignSemiAuto, fn_8018D38C not written yet ----

    CShadowTexMan *CShadowTexMan::m_pInstance;

    CShadowTexMan::~CShadowTexMan()
    {
        m_pInstance = 0;
    }

    CShadowTexMan::CShadowTexMan(bcs::CTask *pParent) : bcs::CTask(pParent, -1)
    {
        m_bEnable = 0;
        m_pTexObj = 0;
        nw4r::g3d::ResFile resFile = CLoadData::GetRes(0x23);
        nw4r::g3d::ResTex resTex = resFile.GetResTex("zz_test");
        if (resTex.IsValid()) {
            void *pImage;
            unsigned short nWidth;
            unsigned short nHeight;
            _GXTexFmt format;
            float fMinLod;
            float fMaxLod;
            unsigned char bMipmap;
            resTex.GetTexObjParam(&pImage, &nWidth, &nHeight, &format, &fMinLod, &fMaxLod, &bMipmap);
            // The texture is used as an intensity map whatever its format.
            GXInitTexObj(&m_TexObj, pImage, nWidth, nHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, bMipmap);
            GXInitTexObjLOD(&m_TexObj, GX_LIN_MIP_LIN, GX_LINEAR, fMinLod, fMaxLod, 0.0f, 0, 0, GX_ANISO_1);
            m_pTexObj = &m_TexObj;
            m_bEnable = 1;
        }
    }

    CShadowTexMan *CShadowTexMan::CreateInstance(bcs::CTask *pParent)
    {
        if (!m_pInstance) {
            m_pInstance = new (0x100) CShadowTexMan(pParent);
        }
        return m_pInstance;
    }

    GXTexObj *CShadowTexMan::GetTexObj()
    {
        CShadowTexMan *pMan = m_pInstance;
        if (pMan == 0) {
            return 0;
        }
        return pMan->m_pTexObj;
    }
}
