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
#include "game/msw/CGameGuideMan.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/menu/CStateSave.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/CSndTask.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/CMainStep.hpp"
#include "game/msw/CCockpitPause.hpp"
#include "game/msw/CMenuPadInfo.hpp"
#include "game/msw/CCockpitMenuWinBtn.hpp"
#include <string.h>
#include <math.h>

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
    void *fn_8026B418(void *p);              // the constructor of the object at CGame 0x134
    long fn_800C1514(long nAID);              // the first local pad number of console nAID (guess)
    void fn_80236704();
    void fn_80074064(long nViewSplit, int);
    unsigned long fn_80190114();              // a colour of the stage (guess)
    void fn_800783E8(long nLayer, const unsigned long *pColor, float, float, float); // a screen fade's colour (guess)
    void fn_80139D28(long nTeam, int, long nItem); // the cockpit's item icon of a team
    void fn_800ED4A4();
    int fn_80236880(long, float);             // the end menu (guess)
    long fn_80237590();                       // the end menu's choice, -1 while open (guess)
    void fn_8026CE30(void *pMsg);             // the message object at CGame 0x134 (not named yet)
    void fn_8026C7B0(void *pMsg);
    int fn_8026CDC8(void *pMsg);
    int fn_8026CE3C(void *pMsg, int);
    void fn_8026BBB0(void *pMsg, int);
    void fn_8026D074(void *pMsg);
    void fn_801D23B8(long);
    void fn_80070F88();
    void fn_80071128();                       // resumes after an outside pause (guess)
    long fn_800CFEDC();                       // the view split of a local versus game (guess)
    long fn_800EC394(long nMsg, long, long, long); // CChrMan: the running manager's SendMessage_ (-1 without one)
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
            CGamePauseTask(bcs::CTask *pParent) : bcs::CTask(pParent, 0x15)
            {
                m_bEnable = 1;
                m_nUnk38 = 0;
                m_nStep = 0;
                AddUnk08(-0x68);
                for (int i = 0; i < 4; i++) {
                    m_anUnk40[i] = 0;
                    m_afUnk50[i] = 0.0f;
                    m_anUnk60[i] = 0;
                    m_afUnk70[i] = 0.0f;
                }
            }
            virtual ~CGamePauseTask();
            virtual void Main();
            virtual void Disp();

            // (inline, names guessed) Forgets the held buttons; stops the
            // game for the pause menu; resumes it.
            void ClearHold()
            {
                for (int i = 0; i < 4; i++) {
                    m_anUnk40[i] = 0;
                    m_afUnk50[i] = 0.0f;
                    m_anUnk60[i] = 0;
                    m_afUnk70[i] = 0.0f;
                }
            }
            void StopGame();
            void ResumeGame();

            // (fn_801889A8, fn_80188AF0, fn_80188D1C; names guessed) Not
            // zero when the game may be paused now; the pad that pressed
            // the + or the - button (with bHold, held for six frames), -1
            // for none.
            int CanbePause() const;
            long GetPlusPad(int bHold);
            long GetMinusPad(int bHold);

            int m_bEnable; // 0x34, pausing is allowed
            // 0x38 (meaning guessed): not zero when the pause comes from the
            // outside (CGame::SetGamePauseUnk38) rather than a button.
            long m_nUnk38;
            long m_nStep;  // 0x3C, the step of Main
            // 0x40-0x7C: per pad (names guessed)
            long m_anUnk40[4];
            float m_afUnk50[4];
            long m_anUnk60[4];
            float m_afUnk70[4];
    };

    // A task holding a list of its own (0x48 bytes, vtable lbl_804767C8;
    // name guessed): the parent of the tasks that run during an arts event.
    class CGameTaskGroup : public bcs::CTask {
        public:
            CGameTaskGroup(bcs::CTask *pParent) : bcs::CTask(pParent, 0x37)
            {
                nw4r::ut::List_Init(&m_List, 0x38);
            }
            virtual ~CGameTaskGroup();
            virtual void Main();
            virtual void Disp() {}

            nw4r::ut::List m_List; // 0x34, the CGameSpeedCtrl tasks (linked at their 0x38)
    };

    // The hit stop (0x44 bytes, vtable lbl_80476498; names guessed).
    class CGameStopTask : public bcs::CTask {
        public:
            CGameStopTask(bcs::CTask *pParent) : bcs::CTask(pParent, -1)
            {
                m_nState = 0;
                m_fFrame = 0.0f;
                m_fStop = 0.0f;
                m_nType = 0;
                SetUnk08Low(1);
            }
            virtual ~CGameStopTask();
            virtual void Main();
            virtual void Disp();

            long m_nState;  // 0x34, 1 while a stop runs
            float m_fFrame; // 0x38
            float m_fStop;  // 0x3C
            long m_nType;   // 0x40
    };

    // A team's item roulette (0x98 bytes; names guessed). Its code comes
    // first in the file (0x8017B920..0x8017C3AC); CGame builds it inline. Its state machine is the second base (0x34;
    // the state number at 0x44 is CFsm's). States 0 and 2 accept a new
    // item, 1 waits with one, 3 spins, 4 stops and 5 gives the item at once
    // (meanings guessed from CGame's calls).
    class CItemRoulette : public bcs::CTask, public bcs::CFsm<CItemRoulette> {
            friend class CGame;
        public:
            CItemRoulette(bcs::CTask *pParent, long nTeam) : bcs::CTask(pParent, -1)
            {
                m_nItem = -1;
                m_nTeam = nTeam;
                m_nUnk84 = 0;
                m_fUnk88 = 0.0f;
                m_bUseRoulette = 1;
                m_nSndId = 0;
                m_fUnk94 = 0.0f;
                SetUnk08Low(0x21);
                InitFsm(this, 6);
                SetState(0, &CItemRoulette::CtorState0, &CItemRoulette::LoopState0, &CItemRoulette::DtorState0);
                SetState(1, &CItemRoulette::CtorState1, &CItemRoulette::LoopState1, &CItemRoulette::DtorState1);
                SetState(3, &CItemRoulette::CtorState3, &CItemRoulette::LoopState3, &CItemRoulette::DtorState3);
                SetState(4, &CItemRoulette::CtorState4, &CItemRoulette::LoopState4, &CItemRoulette::DtorState4);
                SetState(2, &CItemRoulette::CtorState2, &CItemRoulette::LoopState2, &CItemRoulette::DtorState2);
                SetState(5, &CItemRoulette::CtorState5, &CItemRoulette::LoopState5, &CItemRoulette::DtorState5);
                ChangeState(0);
            }
            virtual ~CItemRoulette();
            virtual void Main();

            // (fn_8017C198, name guessed) Hides the item icon (bDisp 0: a
            // held item is kept, a spinning roulette is stopped) or shows it
            // again (the icon kind by bAnim).
            void SetIconDisp(int bDisp, int bAnim);

            // Stops the roulette's sound, if it plays.
            void StopSnd()
            {
                if (m_nSndId) {
                    bcs::CSndTask *pSnd = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_nSndId));
                    if (pSnd && !pSnd->IsDelete()) {
                        pSnd->Stop(0.0f);
                    }
                    m_nSndId = 0;
                }
            }

            // Not zero while the roulette takes a new item (states 0 and 2).
            int IsPushable() const
            {
                switch (m_nState) {
                    case 0:
                    case 2:
                        return 1;
                    default:
                        return 0;
                }
            }

            // The states, in the original's order (state numbers 0, 1, 3,
            // 4, 2, 5); defined with the rest of the class.
            void CtorState0();
            void LoopState0();
            void DtorState0();
            void CtorState1();
            void LoopState1();
            void DtorState1();
            void CtorState3();
            void LoopState3();
            void DtorState3();
            void CtorState4();
            void LoopState4();
            void DtorState4();
            void CtorState2();
            void LoopState2();
            void DtorState2();
            void CtorState5();
            void LoopState5();
            void DtorState5();

            long m_nItem;          // 0x7C, the item (-1: none)
            long m_nTeam;          // 0x80
            long m_nUnk84;         // 0x84
            float m_fUnk88;        // 0x88
            long m_bUseRoulette;   // 0x8C, 0: the item is given without spinning
            unsigned long m_nSndId; // 0x90, the roulette sound's task id
            float m_fUnk94;        // 0x94
    };

    // (fn_80145B50, fn_80145E98: members of dwc::CGameDwc not named yet)
    // Store the results of the online game before saving.
    extern "C" void fn_80145B50(dwc::CGameDwc *pDwc);
    extern "C" void fn_80145E98(dwc::CGameDwc *pDwc);

    // The synchronisation message each console sends in the online
    // states (names guessed): a state number (1000 ready, 2000 started,
    // 3000 ended) and the frame counter to start from.
    struct OnlineSyncMsg {
        long m_nState;
        long m_nTick;
        long m_nUnk08;
    };
    // (dwc::CGameDwc and bcs::dwc::CDwc members not named yet) This
    // console's message, sending it and console nAID's last message; the
    // consoles in the match, whether console nAID is connected, the
    // synchronisation steps, and starting the shared frame counter.
    extern "C" OnlineSyncMsg *fn_80146248();
    extern "C" void fn_80146250(dwc::CGameDwc *pDwc);
    extern "C" OnlineSyncMsg *fn_80146260(dwc::CGameDwc *pDwc, long nAID);
    extern "C" long fn_800B19D8(bcs::dwc::CDwc *pDwc);
    extern "C" int fn_800B1A30(bcs::dwc::CDwc *pDwc, long nAID);
    extern "C" void fn_800B1AA8(bcs::dwc::CDwc *pDwc, int);
    extern "C" int fn_800B1C30(bcs::dwc::CDwc *pDwc, int);
    extern "C" void fn_800B1D04(bcs::dwc::CDwc *pDwc);
    extern "C" void fn_800B1FB4(long nTick);
    extern "C" long fn_8007C9BC();

    // (SAVE_DATA_PLAYER members and two DOL functions not named yet) The
    // score book's records: a game of kind nKind played, the counters, the
    // stage, a character's and a Mii's result.
    extern "C" void fn_801C1F8C(SAVE_DATA_PLAYER *pData, long nKind, long nGameMode);
    extern "C" void fn_8017A4C4(long nGameMode, long);
    extern "C" void fn_80145F8C();
    extern "C" void fn_801C22A8(SAVE_DATA_PLAYER *pData, long nGameMode, long bWin, long, long, long);
    extern "C" void fn_801C2B7C(SAVE_DATA_PLAYER *pData, long nStage, long nGameMode);
    extern "C" void fn_801C29E8(SAVE_DATA_PLAYER *pData, long nChrType, long nGameMode, long nResult);
    extern "C" void fn_801C2E94(SAVE_DATA_PLAYER *pData, RFLStoreData data, long nGameMode, long nResult);
    extern "C" void fn_8026C9D8(void *pMsg, int);

    CItemRoulette::~CItemRoulette()
    {
        StopSnd();
    }

    void CItemRoulette::Main()
    {
        m_fUnk94 = GetFrameRateF();
        ExecState();
    }

    // State 0: no item.
    void CItemRoulette::CtorState0()
    {
        StopSnd();
        m_nItem = -1;
        fn_80139D28(m_nTeam, 3, -1);
    }

    void CItemRoulette::LoopState0() {}

    void CItemRoulette::DtorState0() {}

    // State 1: the item is held.
    void CItemRoulette::CtorState1() {}

    void CItemRoulette::LoopState1() {}

    void CItemRoulette::DtorState1() {}

    // State 3: the roulette spins for 150 frames.
    void CItemRoulette::CtorState3()
    {
        StopSnd();
        m_nSndId = CGameSnd::Play(0xC3D, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        m_fUnk88 = 150.0f;
        fn_80139D28(m_nTeam, 1, m_nItem);
    }

    void CItemRoulette::LoopState3()
    {
        m_fUnk88 -= m_fUnk94;
        if (m_fUnk88 <= 0.0f) {
            StopSnd();
            CGameSnd::Play(0xC3E, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            if (m_nItem == 4) {
                ChangeState(5);
            } else {
                ChangeState(4);
            }
        }
    }

    void CItemRoulette::DtorState3()
    {
        StopSnd();
    }

    // State 4: the roulette has stopped; the item is given at once.
    void CItemRoulette::CtorState4()
    {
        StopSnd();
        if (m_nItem == 4) {
            ChangeState(5);
        }
        m_fUnk88 = 0.0f;
    }

    void CItemRoulette::LoopState4()
    {
        m_fUnk88 -= m_fUnk94;
        if (m_fUnk88 <= 0.0f) {
            CGame::g_GameWork.m_anTeamItem[m_nTeam] = m_nItem;
            ChangeState(1);
        }
    }

    void CItemRoulette::DtorState4() {}

    // State 2: the item has been used; a new one is taken after 10 frames.
    void CItemRoulette::CtorState2()
    {
        if (m_bUseRoulette) {
            fn_80139D28(m_nTeam, 2, m_nItem);
        }
        m_nItem = -1;
        m_fUnk88 = 10.0f;
    }

    void CItemRoulette::LoopState2()
    {
        m_fUnk88 -= m_fUnk94;
        if (m_fUnk88 <= 0.0f) {
            ChangeState(0);
        }
    }

    void CItemRoulette::DtorState2() {}

    // State 5: item 4 (the star): the team's characters change to their
    // star state, and the state ends when none of them is a star any more.
    void CItemRoulette::CtorState5()
    {
        m_nUnk84 = 0;
    }

    void CItemRoulette::LoopState5()
    {
        int bEnd = 1;
        if (m_nUnk84 == 0) {
            if (!(CGame::g_GameWork.m_nFlag18 & 0x4306) && CChrMan::IsActive()) {
                long nNum = CChrMan::GetChrCoreNum();
                for (long i = 0; i < nNum; i++) {
                    CChrCore *pCore = CChrMan::GetChrCore(i);
                    if (pCore && !pCore->IsDisable() && m_nTeam == pCore->GetTeamNo()) {
                        pCore->MethdChgStarState();
                    }
                }
            }
            m_nUnk84++;
            bEnd = 0;
        } else if (CChrMan::IsActive()) {
            long nNum = CChrMan::GetChrCoreNum();
            for (long i = 0; i < nNum; i++) {
                CChrCore *pCore = CChrMan::GetChrCore(i);
                if (pCore && !pCore->IsDisable() && m_nTeam == pCore->GetTeamNo() &&
                    ((pCore->GetStatusFlag() >> 17) & 1)) {
                    bEnd = 0;
                    break;
                }
            }
        }
        if (bEnd) {
            ChangeState(2);
        }
    }

    void CItemRoulette::DtorState5() {}

    void CItemRoulette::SetIconDisp(int bDisp, int bAnim)
    {
        if (bDisp == 0) {
            if (m_bUseRoulette && m_nItem >= 0) {
                fn_80139D28(m_nTeam, 3, m_nItem);
                if (m_nItem == 4) {
                    if (m_nState == 5 || m_nState == 2) {
                        m_nItem = -1;
                        ChangeState(0);
                    } else {
                        // The star is swapped for another item.
                        long nItem = CGimmickMan::QPanelGetSubstituteItem(m_nTeam);
                        if (nItem == 4 || nItem < 0) {
                            m_nItem = -1;
                            ChangeState(0);
                        } else {
                            m_nItem = nItem;
                            CGame::g_GameWork.m_anTeamItem[m_nTeam] = nItem;
                            ChangeState(1);
                        }
                    }
                } else if (m_nState == 2) {
                    m_nItem = -1;
                    ChangeState(0);
                } else {
                    CGame::g_GameWork.m_anTeamItem[m_nTeam] = m_nItem;
                    ChangeState(1);
                }
            }
        } else {
            long nItem = CGame::g_GameWork.m_anTeamItem[m_nTeam];
            if (nItem >= 0 && nItem != m_nItem) {
                m_nItem = nItem;
                m_bUseRoulette = 0;
            }
            if (m_nItem >= 0 && m_bUseRoulette == 0) {
                if (bAnim) {
                    fn_80139D28(m_nTeam, 0, m_nItem);
                } else {
                    fn_80139D28(m_nTeam, 4, m_nItem);
                }
                ChangeState(1);
            }
        }
        m_bUseRoulette = bDisp;
    }

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

        // The kind of the message object at CGame 0x134 (its byte at 0x25;
        // 3 is shown over the running game, 2 is the change of court).
        // (names guessed; the class is not known yet)
        inline long GetMsgKind(const void *pMsg)
        {
            return static_cast<const unsigned char *>(pMsg)[0x25];
        }
        inline void SetMsgKind(void *pMsg, long nKind)
        {
            static_cast<unsigned char *>(pMsg)[0x25] = nKind;
        }

        // The work memory of the event demo state (19), filled by whoever
        // queues it (BossF's game setup). Names guessed.
        struct EventDemoWork {
            unsigned long m_nFlag; // 0x00, bit 0: the demo cannot be skipped
            long m_nStep;          // 0x04
            long m_nDemo;          // 0x08, the demo CEventDemo::CreateDemo plays
            long m_nUnk0C;         // 0x0C, passed to fn_801D23B8 first
            long m_bNoFadeIn;      // 0x10, not zero: the screen does not fade in
            long m_Unk14;
            // 0x18: the fade at the end (1: 3D fade type 0, 2: type 1, 3:
            // cross fade, 0: none) over m_fFadeFrame frames.
            long m_nFadeType;
            float m_fFadeFrame;    // 0x1C
        };

        // The network connection inside the game's Wi-Fi object (a
        // bcs::dwc::CDwc at 0x38; how CGameDwc holds it is not known).
        inline bcs::dwc::CDwc *GetDwc(dwc::CGameDwc *pGameDwc)
        {
            return reinterpret_cast<bcs::dwc::CDwc *>(reinterpret_cast<char *>(pGameDwc) + 0x38);
        }

        // The pads in play as bits: those of the online match while one
        // runs, else this console's.
        inline unsigned long GetActivePadBits()
        {
            int bOnline = CGame::g_GameWork.m_Unk151;
            if (bOnline) {
                dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
                if (!pDwc) {
                    bOnline = 0;
                } else if (!GetDwc(pDwc)->IsInMatch()) {
                    bOnline = 0;
                }
            }
            if (bOnline) {
                return CGame::g_GameWork.m_anAllPadBits[1];
            }
            return CGame::g_GameWork.m_anAllPadBits[0];
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

    CGame::CGame(bcs::CTask *pParent, GAME_MODE mode) : bcs::CTask(pParent, -1)
    {
        AddUnk08(-0x63);
        m_nNextState = 5;
        m_fStateCount = 0.0f;
        m_fBgFadeWait = 0.0f;
        m_fBgFadeStep = 0.0f;
        m_fArtsEventFrame = 0.0f;
        m_fTurnStartTelopFrame = 0.0f;
        m_fUnk14C = 1800.0f;
        m_pPauseTask = 0;
        m_pStopTask = 0;
        m_pArtsTaskRoot = 0;
        m_nGoalDemoId = 0;
        m_nSatLineId = 0;
        m_pTempParent = 0;
        m_nSlowTaskId = 0;
        m_nScoredViewId = 0;
        m_pArtsEventTask = 0;
        m_bExit = 0;
        m_nUnkCC = 0;
        m_nStateStep = 0;
        m_nGameFlag = 0;
        m_nBgFadeMode = 0;
        m_nReqArtsEvent = 0;
        m_nArtsEventStep = 0;
        m_nArtsEventSync = 0;
        m_nTurnStartTelopType = 0;
        m_nUnk104 = 0;
        m_nUnk108 = 0;
        m_nUnk11C = 0;
        m_nUnk12C = 0;
        m_Unk130 = 0;
        m_pUnk134 = 0;
        m_nUnk148 = 0;
        for (int i = 0; i < 4; i++) {
            m_apCamera[i] = 0;
        }
        for (int i = 0; i < 4; i++) {
            m_apItemRoulette[i] = 0;
        }
        g_GameWork.m_nGameMode = mode;
        ResetBgFadeColor();
        g_uPauseBit = 0;
        g_nEventChrIdx = -1;
        InitState();

        // Team n plays in colour n; the second console swaps the pairs.
        g_GameWork.m_anTeamColorIdx[0] = 0;
        g_GameWork.m_anTeamColorIdx[1] = 1;
        g_GameWork.m_anTeamColorIdx[2] = 2;
        g_GameWork.m_anTeamColorIdx[3] = 3;
        if (g_GameWork.m_nUnk00 != 0) {
            g_GameWork.m_anTeamColorIdx[0] = 1;
            g_GameWork.m_anTeamColorIdx[1] = 0;
            g_GameWork.m_anTeamColorIdx[2] = 3;
            g_GameWork.m_anTeamColorIdx[3] = 2;
        }
        if (g_GameWork.m_Unk151) {
            // Online: the local pad number of each online pad, counted on
            // from this console's first pad for the other console's pads.
            long nMyAID = g_GameWork.m_nUnk00;
            long nBase = fn_800C1514(nMyAID);
            long nTotal = bcs::CWifiPadBase::GetOnlinePadTotal();
            for (long i = 0; i < nTotal; i++) {
                if (nMyAID == bcs::CWifiPadBase::GetAIDFromOnlinePadID(i)) {
                    g_GameWork.m_anUnk84[i] = bcs::CWifiPadBase::GetLocalPadIDFromOnlinePadID(i);
                } else {
                    g_GameWork.m_anUnk84[i] = nBase + bcs::CWifiPadBase::GetLocalPadIDFromOnlinePadID(i);
                }
            }
        }
        InitTeamPadBits_();
        m_pTempParent = new bcs::CTask(this, -1);
        m_pPauseTask = new CGamePauseTask(this);
        m_pStopTask = new CGameStopTask(this);
        m_pArtsTaskRoot = new CGameTaskGroup(this);
        CGameGuideMan::Create(this);
        // The 0x28-byte message object (class not known yet; fn_8026B418
        // is its constructor).
        void *pMsg = ::operator new(0x28);
        if (pMsg) {
            pMsg = fn_8026B418(pMsg);
        }
        m_pUnk134 = pMsg;
        fn_80236704();
    }

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
        rWork.m_nUnk1A8 = 0;
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

    // The pads of each team as bits. Offline they follow from the play
    // type (one to four pads) and every team gets all of them; online
    // each team gets the pads of its console, and a team without any
    // gets all.
    void CGame::InitTeamPadBits_()
    {
        unsigned long nBits = 0;
        if (g_GameWork.m_Unk151) {
            long nTotal = bcs::CWifiPadBase::GetOnlinePadTotal();
            for (int i = 0; i < nTotal; i++) {
                nBits |= 1 << i;
            }
            if (nBits == 0) {
                nBits = 1;
            }
            g_GameWork.m_anAllPadBits[0] = nBits;
            g_GameWork.m_anAllPadBits[1] = nBits;
            for (int i = 0; i < 4; i++) {
                g_GameWork.m_anTeamPadBits[i] = 0;
                g_GameWork.m_anTeamOnlinePadBits[i] = 0;
            }
            for (long i = 0; i < nTotal; i++) {
                long nAID = bcs::CWifiPadBase::GetAIDFromOnlinePadID(i);
                if (nAID >= 0 && nAID < 4) {
                    long nPad = bcs::CWifiPadBase::GetLocalPadIDFromOnlinePadID(i);
                    g_GameWork.m_anTeamPadBits[nAID] |= 1 << nPad;
                    g_GameWork.m_anTeamOnlinePadBits[nAID] |= 1 << i;
                }
            }
            for (int i = 0; i < 4; i++) {
                if (g_GameWork.m_anTeamPadBits[i] == 0) {
                    g_GameWork.m_anTeamPadBits[i] = nBits;
                }
                if (g_GameWork.m_anTeamOnlinePadBits[i] == 0) {
                    g_GameWork.m_anTeamOnlinePadBits[i] = nBits;
                }
            }
        } else {
            switch (g_GameWork.m_nUnk06) {
                case 0:
                case 2:
                case 4:
                    nBits |= 0x1;
                    break;
                case 1:
                case 3:
                case 5:
                case 6:
                case 18:
                    nBits |= 0x3;
                    break;
                case 7:
                case 9:
                case 15:
                case 16:
                case 17:
                case 20:
                case 21:
                    nBits |= 0x7;
                    break;
                case 8:
                case 10:
                case 14:
                case 19:
                case 22:
                case 23:
                case 24:
                case 25:
                    nBits |= 0xF;
                    break;
            }
            if (nBits == 0) {
                nBits = 1;
            }
            g_GameWork.m_anAllPadBits[0] = nBits;
            g_GameWork.m_anAllPadBits[1] = nBits;
            for (int i = 0; i < 4; i++) {
                g_GameWork.m_anTeamPadBits[i] = nBits;
                g_GameWork.m_anTeamOnlinePadBits[i] = nBits;
            }
        }
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

    // Passes a message to every part of the game, then acts on the ones
    // the game itself handles: 0x1A ends the game (1: back to the menu, 2:
    // retry), 0x2E counts in the score book.
    void CGame::SendMessage_(long nMsg, long n1, long n2, long n3)
    {
        CGameGuideMan::SendMessage(nMsg, n1, n2, n3);
        if (CStageMan::IsValidInstance()) {
            CStageMan::GetInstance()->SendMessage(nMsg, n1, n2, n3);
        }
        fn_800EC394(nMsg, n1, n2, n3);
        CGimmickMan::SendMessage(nMsg, n1, n2, n3);
        CTournament::SendMessage(nMsg, n1, n2, n3);
        CEventDemo::SendMessage(nMsg, n1, n2, n3);

        if (nMsg == 0x1A) {
            switch (n1) {
                case 1:
                    ClearUnk08(1);
                    g_GameWork.m_nReturnStep = 0;
                    menu::CStateSave::GetInstance()->SetStartMenuMode(2);
                    FsmqChangeStateRet(18);
                    break;
                case 2:
                    ClearUnk08(1);
                    FsmqChangeStateRet(16);
                    break;
            }
        }
        if (nMsg == 0x2E) {
            ScoreBookAddCountWork_(n1, n2, n3);
            switch (n1) {
                case 0:
                    if (n2 < 1) {
                        n2 = 1;
                    }
                    m_anScoreBookCount[0] += n2;
                    break;
                case 1:
                    m_anScoreBookCount[1]++;
                    break;
                case 2:
                    m_anScoreBookCount[2]++;
                    break;
            }
        }
        if (g_GameWork.m_nUnk180) {
            switch (nMsg) {
                case 0x18:
                case 0x36:
                case 0x40:
                case 0x54:
                case 0x61:
                    m_fUnk14C = 0.0f;
                    break;
            }
        }
    }

    int CGame::IsLineMsgDispEnable()
    {
        if (m_pInstance) {
            return m_pInstance->IsLineMsgDispEnable_();
        }
        return 0;
    }

    // Which goal each team attacks first (GetFirstGoalIdxMode_: 0 or 1 for
    // a fixed side, otherwise drawn), and which team each view shows. In a
    // local versus game on one screen the views follow the swapped goals.
    void CGame::SetFirstGoalIdx_()
    {
        long nFirst;
        switch (GetFirstGoalIdxMode_()) {
            case 0:
                nFirst = 0;
                break;
            case 1:
                nFirst = 1;
                break;
            default:
                nFirst = bcs::BCS_Rand() % 2;
                break;
        }
        for (long i = 0; i < 4; i++) {
            SetViewTeam(i, i % 2);
        }
        if (nFirst != 0) {
            for (long i = 0; i < 4; i++) {
                SetGoalIdx(i, 1 - i % 2);
            }
            if (IsLocalVsMode() && bcs::CWorldEnv::m_vstViewSplitType == 0) {
                for (long i = 0; i < 4; i++) {
                    SetViewTeam(i, 1 - i % 2);
                }
            }
        } else {
            for (long i = 0; i < 4; i++) {
                SetGoalIdx(i, i % 2);
            }
        }
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

    // Starts the event demo and the loading of the game's resources, then
    // queues the states that follow: creating the objects, the online
    // synchronisation, the game start, the music, the save, the entrance
    // demo and the online start; and finally the game's own init state.
    void CGame::LoopStateGameLoad()
    {
        int bNext = 0;
        switch (m_nStateStep) {
            case 0:
                CNowLoading::SetDisp(1);
                CEventDemo::CreateDemo(2, this, -1.0f, -1.0f);
                bcs::CLoadMan::ClearDvdReadSize();
                RsrcLoadStart_();
                m_nStateStep++;
                break;
            case 1:
                if (CEventDemo::m_Work.bSkip && RsrcIsLoadEnd_()) {
                    m_nUnkCC = 1;
                    CNowLoading::SetDisp(0);
                    RsrcInitialize_();
                    FinishFirstLoadData();
                    CGameAccInput::Initialize();
                    FsmqCutNext();
                    FsmqPush(22);
                    if (g_GameWork.m_nUnk180 == 0) {
                        if (bcs::dwc::CDwc::GetInstance()) {
                            FsmqPush(24);
                        }
                        if (g_GameWork.m_nUnk40 & 0x100) {
                            FsmqPush(21);
                        }
                        if (g_GameWork.m_nGameMode != GAME_MODE_BOSS || !CEventDemo::IsEnable()) {
                            FsmqPush(23);
                        }
                        if (g_GameWork.m_nReturnStep == 0) {
                            FsmqPush(28);
                        }
                        if (CEventDemo::IsEnable()) {
                            FsmqPush(2);
                        }
                        if (bcs::dwc::CDwc::GetInstance()) {
                            FsmqPush(25);
                        }
                    } else {
                        FsmqPush(23);
                    }
                    FsmqPush(1);
                    fn_80074064(g_GameWork.m_nViewSplit, 0);
                    bNext = 1;
                    m_nStateStep++;
                }
                break;
            default:
                bNext = 1;
                break;
        }
        if (bNext) {
            FsmqNext();
        }
    }

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

    // State 21: the start of the game proper (the common sounds are
    // loaded, the start demo plays). The game is paused and the cockpit
    // hidden meanwhile.
    void CGame::CtorStateGameStart()
    {
        bcs::CMainApp::SetPause(0x40, 1);
        bcs::CMainApp::Start3DFade(0, 1.0f);
        bcs::CMainApp::StartMasterFade(0, 1.0f);
        fn_80073FEC(1);
        CGamePadCheck::SetPadCheckDisable(1);
        SetCockpitDisp_(0);
        SetCockpitItemIconDisp_(0, 0);
        SetDispRadarIcon_(0);
        bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
        BgResetFade_();
        m_nStateStep = 0;
    }

    void CGame::LoopStateGameStart()
    {
        switch (m_nStateStep) {
            case 0:
                m_nStateStep++;
                break;
            case 1:
                SendMessage_(1, 0, 0, 0);
                if (g_GameWork.m_nUnk40 & 0x100) {
                    CGameSnd::LoadCommonSnd();
                    m_nStateStep++;
                } else {
                    m_nStateStep = 99;
                }
                break;
            case 2:
                if (bcs::CSnd::IsReady()) {
                    CGameBGM::SetBgmState(CGameBGM::STATE_INTRO);
                    CEventDemo::CreateDemo(3, this, 1.0f / 60.0f, 1.0f / 60.0f);
                    m_nStateStep++;
                }
                break;
            case 3:
                if (CEventDemo::m_Work.bSkip) {
                    if (!CEventDemo::IsEnable()) {
                        g_GameWork.m_nStartFadeType = 0;
                    }
                    m_fStateCount = 2.0f;
                    m_nStateStep++;
                }
                break;
            case 4:
                if (StateWorkDecStateCount_()) {
                    CGameSnd::ReloadGameSnd();
                    m_nStateStep++;
                }
                break;
            case 5:
                if (bcs::CSnd::IsReady()) {
                    CGameSnd::SaveGameHeapStateAlways();
                    m_nStateStep++;
                }
                break;
            default:
                bcs::CRenderMan::ReqPopViewSplitType(0);
                FsmqNext();
                break;
        }
    }

    void CGame::DtorStateGameStart()
    {
        bcs::CMainApp::SetPause(0x40, 0);
        CGamePadCheck::SetPadCheckDisable(0);
        fn_80073FEC(0);
        SendMessage_(2, 0, 0, 0);
    }

    // State 2: the entrance demo. The cockpit is hidden; the boss game
    // fades its music out first.
    void CGame::CtorStateEntranceDemo()
    {
        g_GameWork.m_nFlag18 |= 2;
        bcs::CMainApp::Start3DFade(0, 1.0f);
        bcs::CMainApp::StartMasterFade(0, 1.0f);
        SetCockpitDisp_(0);
        SetCockpitItemIconDisp_(0, 0);
        SetDispRadarIcon_(0);
        bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
        BgResetFade_();
        if (g_GameWork.m_nGameMode == GAME_MODE_BOSS) {
            CGameBGM::SetBgmState(CGameBGM::STATE_FADE_OUT);
        }
        m_nStateStep = 0;
    }

    void CGame::LoopStateEntranceDemo()
    {
        switch (m_nStateStep) {
            case 0:
                m_nStateStep++;
                break;
            case 1:
                SendMessage_(3, 0, 0, 0);
                EntranceDemoSetUp();
                m_nStateStep++;
                break;
            case 2:
                if (g_GameWork.m_nUnk40 & 0x100) {
                    m_nStateStep++;
                } else {
                    // Without the start demo the screen fades from the
                    // stage's colour.
                    unsigned long color = fn_80190114();
                    fn_800783E8(4, &color, 0.0f, 0.0f, 1.0f / 60.0f);
                    bcs::CMainApp::Start3DFade(4, 1.0f);
                    m_nStateStep = 4;
                }
                break;
            case 3:
                bcs::CMainApp::Start3DFade(4, 1.0f / 60.0f);
                bcs::CMainApp::StartMasterFade(4, 1.0f / 60.0f);
                m_nStateStep++;
                // fall through
            case 4:
                CEventDemo::CreateDemo(0, this, 1.0f / 60.0f, 1.0f / 30.0f);
                m_nStateStep++;
                break;
            default:
                if (CEventDemo::m_Work.bSkip) {
                    if (g_GameWork.m_nGameMode == GAME_MODE_BOSS && CGameBGM::GetBgmState() != CGameBGM::STATE_PLAY) {
                        CGameBGM::SetBgmState(CGameBGM::STATE_START);
                    }
                    CEventDemo::m_Work.bSkip = 1;
                    bcs::CRenderMan::ReqPopViewSplitType(0);
                    FsmqNext();
                }
                break;
        }
    }

    void CGame::DtorStateEntranceDemo()
    {
        g_GameWork.m_nFlag18 &= ~2;
        SendMessage_(4, 0, 0, 0);
    }

    // State 22: creates the game's objects.
    void CGame::CtorStateGameCreate() {}

    // Creates the game's objects (GenerateInstance, the balls), stops the
    // characters and gives each team of a normal game its item roulette
    // (the boss game has one, for team 0).
    void CGame::LoopStateGameCreate()
    {
        GenerateInstance();
        fn_800ED4A4();
        CBallMan::Create();
        CChrMan::AllSetState(4, -1, 0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        CChrMan::SetChrMarkerDisp(0);
        if (g_GameWork.m_nGameType == GAME_TYPE_NORMAL) {
            switch (g_GameWork.m_nGameMode) {
                case GAME_MODE_BASKET:
                case GAME_MODE_VOLLEY:
                case GAME_MODE_DODGE:
                case GAME_MODE_HOCKEY:
                    for (long i = 0; i < 2; i++) {
                        m_apItemRoulette[i] = new (0x100) CItemRoulette(this, i);
                    }
                    break;
                case GAME_MODE_BOSS:
                    m_apItemRoulette[0] = new (0x100) CItemRoulette(this, 0);
                    break;
            }
        }
        FsmqDoChain();
    }

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

    inline int CGame::IsDemoPlayEnd_()
    {
        if (g_GameWork.m_nUnk180 == 0) {
            return 0;
        }
        if (m_fUnk14C > 0.0f) {
            m_fUnk14C -= GetFrameRateF();
        }
        if (m_fUnk14C <= 0.0f) {
            return 1;
        }
        if (bcs::CWifiPad::IsDown(0x900, 0)) {
            CGameSnd::Play(0xBBB, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            m_fUnk14C = 0.0f;
            return 1;
        }
        return 0;
    }

    // In the attract demo the game exits (state 18) when the demo is over.
    void CGame::LoopStateWaitStartBase()
    {
        LoopStateWaitStart();
        if (IsDemoPlayEnd_()) {
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            FsmqCutNext();
            FsmqPush(18);
            FsmqDoChain();
        }
    }

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

    void CGame::LoopStateMainLoopBase()
    {
        LoopStateMainLoop();
        if (IsDemoPlayEnd_()) {
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            FsmqCutNext();
            FsmqPush(18);
            FsmqDoChain();
        }
    }

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

    // State 6: waits for the goal demo, then for m_fStateCount, and goes on
    // to the turn result with the scorer's team set up for the next turn.
    void CGame::LoopStateGoalEvent()
    {
        if (m_nStateStep == 0) {
            if (IsEndGoalDemo_()) {
                EndGoalDemo_();
                StartGoalTelop_();
                SetCockpitMapDisp_(0);
                m_nStateStep++;
            }
        } else if (StateWorkDecStateCount_()) {
            CChrMan::ReserveAllSetState(0, g_GameWork.m_nGoalChar, g_GameWork.m_nGoalOwnGoal ? -1 : 0);
            FsmqChangeState(7);
        }
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

    // State 7: the scored view, a pause, then the game set (12), the time
    // up (8, when the game time has run out in a timed game) or the next
    // turn (10).
    void CGame::LoopStateTurnResult()
    {
        switch (m_nStateStep) {
            case 0:
                if (CanbeScoredView_()) {
                    m_fStateCount = 3.0f;
                    StartScoredView_(-1);
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    // (both waits are one second in this version)
                    if (IsGameSet_()) {
                        m_fStateCount = 60.0f;
                    } else {
                        m_fStateCount = 60.0f;
                    }
                    m_nStateStep++;
                }
                break;
            default:
                if (StateWorkDecStateCount_()) {
                    if (IsGameSet_()) {
                        FsmqChangeState(12);
                    } else if (g_GameWork.m_fUnk20 > 0.0f && g_GameWork.m_fGameTime <= 0.0f) {
                        FsmqChangeState(8);
                    } else {
                        FsmqChangeState(10);
                    }
                }
                break;
        }
    }

    void CGame::DtorStateTurnResult() {}

    void CGame::CtorStateTimeUp() {}

    void CGame::LoopStateTimeUp() {}

    void CGame::DtorStateTimeUp() {}

    int CGame::MenuCallInterval_()
    {
        return 0;
    }

    // State 9: the change of court between periods. Everything is reset,
    // the next period starts with the time full and the goals swapped, and
    // the court change demo plays (when event demos are on).
    void CGame::CtorStateChgCourt()
    {
        g_GameWork.m_nFlag18 = (g_GameWork.m_nFlag18 & ~0x300) | 0x4000;
        EndScoredView_();
        KillTempChild_();
        CChrMan::AllMotStop();
        CChrMan::AllStockItemDelete();
        CChrMan::AllReset();
        CBallMan::AllReset();
        CGimmickMan::AllReset();
        CGimmickMan::QPanelAllReset();
        SendMessage_(0x17, 0, 0, 0);
        SetPeriodResult_();
        g_GameWork.m_nPeriod++;
        SetTime(g_GameWork.m_fUnk20);
        ChangeGoalIdx_();
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            ClearTeamArtsGauge_(i);
            g_GameWork.m_anTeamItem[i] = -1;
        }
        SetCockpitDisp_(0);
        SetCockpitItemIconDisp_(0, 0);
        SetDispRadarIcon_(0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        CChrMan::SetChrMarkerDisp(0);
        SetDispRadarIcon_(0);
        m_nStateStep = 0;
        if (CEventDemo::IsEnable()) {
            bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
        } else {
            m_nStateStep = 99;
        }
        m_nUnk11C = 0;
    }

    void CGame::LoopStateChgCourt()
    {
        switch (m_nStateStep) {
            case 0:
                m_nStateStep++;
                break;
            case 1: {
                SendMessage_(9, 0, 0, 0);
                EventDemoSetUp();
                CBallMan::AllSleep();
                long nNum = CChrMan::GetChrCoreNum();
                for (long i = 0; i < nNum; i++) {
                    CChrCore *pCore = CChrMan::GetChrCore(i);
                    if (pCore && !pCore->IsDisable()) {
                        pCore->OnDispFlag(4);
                    }
                }
                bcs::CRenderMan::SetDepthOfFieldParam(1, 6.0f, 7.5f, 1.5f);
                bcs::CRenderMan::EnableDepthOfFieldEffect(1);
                m_fStateCount = 30.0f;
                m_nStateStep++;
                break;
            }
            case 2:
                if (StateWorkDecStateCount_()) {
                    CEventDemo::SetupDemoCamera("ChgCourt", 0);
                    CEventDemo::PlayDemoCamera(1);
                    bcs::CMainApp::Start3DFade(4, 0.0f);
                    unsigned long color = fn_80190114();
                    fn_800783E8(4, &color, 0.0f, 0.0f, 1.0f / 60.0f);
                    m_nStateStep++;
                }
                break;
            case 3:
                MenuCallInterval_();
                m_nStateStep++;
                break;
            case 4:
                if (MenuGetResult_() != -1) {
                    if (fn_8026D0A0(m_pUnk134)) {
                        bcs::CMainApp::Start3DFade(0, 1.0f / 60.0f);
                        bcs::CMainApp::StartMasterFade(0, 1.0f / 60.0f);
                    } else {
                        unsigned long color = fn_80190114();
                        fn_800783E8(0, &color, 0.0f, 0.0f, 1.0f / 60.0f);
                    }
                    m_nStateStep++;
                }
                break;
            case 5: {
                int bEnd = 1;
                if (bcs::CMainApp::IsPlay3DFade()) {
                    bEnd = 0;
                }
                if (bcs::CMainApp::IsPlayMasterFade()) {
                    bEnd = 0;
                }
                if (bEnd) {
                    CEventDemo::SetEndDemoCamera();
                    bcs::CRenderMan::EnableDepthOfFieldEffect(0);
                    bcs::CRenderMan::ResetDepthOfFieldParam();
                    bcs::CRenderMan::ReqPopViewSplitType(0);
                    SendMessage_(10, 0, 0, 0);
                    m_fStateCount = 30.0f;
                    m_nStateStep++;
                }
                break;
            }
            case 6:
                if (StateWorkDecStateCount_()) {
                    m_nStateStep = 99;
                }
                break;
            default:
                // Offline, the message of the change (state 29) comes
                // first; then the music and the wait for the next start.
                if (!bcs::dwc::CDwc::GetInstance()) {
                    FsmqCutNext();
                    if (m_pUnk134) {
                        SetMsgKind(m_pUnk134, 2);
                    }
                    FsmqPush(29);
                }
                FsmqPush(23);
                FsmqPush(3);
                FsmqNext();
                break;
        }
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

    // State 13: the result of the game. The court is cleared, the results
    // and the score book are worked out (a tournament match also from the
    // tournament's own result), and the unlocks are checked.
    void CGame::CtorStateGameResult()
    {
        SetReplayMode(0);
        SetResultMode(1);
        SetPreparationMode(0);
        KillTempChild_();
        CEffectCtrl::EraseEffectAll();
        CBallMan::AllReset();
        CGimmickMan::AllReset();
        CGimmickMan::QPanelAllReset();
        SendMessage_(0x17, 0, 0, 0);
        SetCockpitDisp_(0);
        SetCockpitItemIconDisp_(0, 0);
        SetDispRadarIcon_(0);
        SetPeriodResult_();
        SetGameResult_();
        CalcAddSkillPoint_();
        ScoreBookApplyCountWork_();
        SendMessage(0x11, 0, 0, 0);
        if (CTournament::IsTournamentGame() && !CTournament::IsExtraMatchGame()) {
            SetGameResultAll_(2);
            switch (CTournament::GetBattleResult()) {
                case 1:
                    g_GameWork.m_anTeamResult[0] = 1;
                    break;
                case 2:
                    g_GameWork.m_anTeamResult[1] = 1;
                    break;
                default:
                    SetGameResultAll_(3);
                    break;
            }
        }
        Unlock::CheckClear();
        CChrMan::RemakeChrBustImg(1);
        m_nStateStep = 0;
    }

    void CGame::LoopStateGameResult()
    {
        switch (m_nStateStep) {
            case 0:
                bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
                m_nStateStep++;
                break;
            case 1:
                CChrMan::AllStockItemDelete();
                CChrMan::AllMotStop();
                CChrMan::AllReset();
                CChrMan::AllSetState(4, -1, 0);
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                SendMessage(5, 0, 0, 0);
                CEventDemo::CreateDemo(1, this, -1.0f, -1.0f);
                bcs::CMainApp::StartMasterFade(4, 1.0f / 30.0f);
                m_nStateStep++;
                break;
            case 2: {
                int bEnd = 1;
                if (bcs::CMainApp::IsPlay3DFade()) {
                    bEnd = 0;
                }
                if (bcs::CMainApp::IsPlayMasterFade()) {
                    bEnd = 0;
                }
                if (bEnd) {
                    CEventDemo::SetFlags(2);
                    PlayGameResultBGM_();
                    m_nStateStep++;
                }
                break;
            }
            case 3:
                if (CEventDemo::m_Work.bSkip && CGameBGM::GetBgmState() == CGameBGM::STATE_PLAY) {
                    CEventDemo::SetFlags(1);
                    m_fStateCount = 50.0f;
                    m_nStateStep++;
                }
                break;
            case 4:
                if (StateWorkDecStateCount_()) {
                    CEventDemo::Cleanup(SetGameResultNext_());
                    m_nStateStep++;
                }
                break;
        }
    }

    void CGame::DtorStateGameResult()
    {
        SendMessage(6, 0, 0, 0);
    }

    // Where the game goes after its result: the end menu (15) after a
    // tournament game that is over (m_Unk130 == 2), else the result menu
    // (14).
    int CGame::SetGameResultNext_()
    {
        if (CTournament::IsTournamentGame()) {
            if (m_Unk130 == 2) {
                FsmqChangeState(15);
            } else {
                FsmqChangeState(14);
            }
        } else {
            FsmqChangeState(14);
        }
        return 0;
    }

    void CGame::CtorStateResultMenu()
    {
        SetReplayMode(0);
        SetResultMode(1);
        SetPreparationMode(0);
        CGameBGM::SetBgmState(CGameBGM::STATE_MUSIC_3C);
        m_nStateStep = 0;
    }

    // State 14: the result menu (when the game has one), a pause, then the
    // end menu (15).
    void CGame::LoopStateResultMenu()
    {
        switch (m_nStateStep) {
            case 0:
                if (MenuCallGameResult_()) {
                    m_nStateStep++;
                } else {
                    m_fStateCount = 30.0f;
                    m_nStateStep = 99;
                }
                break;
            case 1:
                if (MenuGetResult_() != -1) {
                    m_fStateCount = 45.0f;
                    m_nStateStep = 99;
                }
                break;
            case 99:
                if (StateWorkDecStateCount_()) {
                    m_nStateStep++;
                }
                break;
            default:
                FsmqChangeState(15);
                break;
        }
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

    // State 15: saves (when saving is on), then runs the end menu and acts
    // on its choice: 1000 and 1002 play again (16), the others leave the
    // game (18) for a menu page of the title menu.
    void CGame::LoopStateEndMenu()
    {
        switch (m_nStateStep) {
            case 0:
                if (CSave::CanbeWrite()) {
                    CSave::WriteAsync();
                    m_nStateStep++;
                }
                break;
            case 1:
                if (CSave::IsComplete()) {
                    m_nStateStep++;
                }
                break;
            case 2:
                if (!fn_80236880(0, 0.0f)) {
                    FsmqChangeStateRet(18);
                }
                m_nStateStep++;
                break;
            default:
                switch (fn_80237590()) {
                    case -1:
                        break;
                    case 1000:
                    case 1002:
                        FsmqChangeStateRet(16);
                        break;
                    case 1001:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(2);
                        FsmqChangeStateRet(18);
                        break;
                    case 1003:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(3);
                        FsmqChangeStateRet(18);
                        break;
                    case 1004:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(4);
                        FsmqChangeStateRet(18);
                        break;
                    case 1009:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(8);
                        FsmqChangeStateRet(18);
                        break;
                    case 1010:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(6);
                        FsmqChangeStateRet(18);
                        break;
                    case 1011:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(7);
                        FsmqChangeStateRet(18);
                        break;
                    case 1007:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(1);
                        FsmqChangeStateRet(18);
                        break;
                    case 1008:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(5);
                        FsmqChangeStateRet(18);
                        break;
                    case 1006:
                        SetReturnStep(0);
                        // fall through
                    default:
                        menu::CStateSave::GetInstance()->SetStartMenuMode(2);
                        FsmqChangeStateRet(18);
                        break;
                }
                break;
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

    void CGame::LoopStateSave()
    {
        switch (m_nStateStep) {
            case 0:
                m_nStateStep++;
                break;
            case 1:
                m_nStateStep++;
                break;
            case 2:
                if (CSave::CanbeWrite()) {
                    // Online, the network results are stored first.
                    if (bcs::dwc::CDwc::GetInstance()) {
                        dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
                        if (pDwc) {
                            fn_80145B50(pDwc);
                            fn_80145E98(pDwc);
                        }
                    }
                    CSave::WriteAsync();
                    m_nStateStep++;
                }
                break;
            case 3:
                if (CSave::IsComplete()) {
                    m_nStateStep = 99;
                }
                break;
            case 99:
                FsmqNext();
                m_nStateStep++;
                break;
        }
    }

    void CGame::DtorStateSave() {}

    // State 29: shows the message of the object at 0x134 (99 skips it).
    void CGame::CtorStateMessage()
    {
        m_nStateStep = 0;
        if (!fn_8026D0A0(m_pUnk134)) {
            m_nStateStep = 99;
        }
    }

    void CGame::LoopStateMessage()
    {
        switch (m_nStateStep) {
            case 0:
                if (GetMsgKind(m_pUnk134) != 3) {
                    bcs::CMainApp::Start3DFade(0, 1.0f);
                    bcs::CMainApp::StartMasterFade(0, 1.0f);
                    fn_8026CE30(m_pUnk134);
                }
                CChrMan::AllReset();
                CChrMan::AllSetState(4, -1, 0);
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                fn_8026C7B0(m_pUnk134);
                CNameMesg::Initialize();
                m_nStateStep++;
                break;
            case 1:
                if (fn_8026CDC8(m_pUnk134)) {
                    m_nStateStep++;
                }
                break;
            case 2:
                if (fn_8026CE3C(m_pUnk134, 0)) {
                    if (GetMsgKind(m_pUnk134) != 3) {
                        SetCockpitDisp_(0);
                        SetCockpitItemIconDisp_(0, 0);
                        SetDispRadarIcon_(0);
                        bcs::CMainApp::Start3DFade(4, 1.0f / 30.0f);
                        bcs::CMainApp::StartMasterFade(4, 1.0f / 30.0f);
                        bcs::CMainApp::SetPause(0x40, 1);
                    } else {
                        fn_80073FB4(1);
                        bcs::CMainApp::SetPause(0x40, 1);
                    }
                    fn_8026BBB0(m_pUnk134, 1);
                    m_nStateStep++;
                } else {
                    m_nStateStep = 99;
                }
                break;
            case 3: {
                int bEnd = 1;
                if (bcs::CMainApp::IsPlay3DFade()) {
                    bEnd = 0;
                }
                if (bcs::CMainApp::IsPlayMasterFade()) {
                    bEnd = 0;
                }
                if (bEnd) {
                    m_fStateCount = 5.0f;
                    m_nStateStep++;
                }
                break;
            }
            case 4:
                if (StateWorkDecStateCount_()) {
                    m_fStateCount = 895.0f;
                    CCockpit::ClearResult(-1);
                    CNameMesg::CreateButtonWindow("Btn_A_Skip");
                    m_nStateStep++;
                }
                break;
            case 5: {
                // Ends after its time or on the button.
                int bEnd = StateWorkDecStateCount_();
                if (CCockpit::GetResult() != -1) {
                    bEnd = 1;
                }
                if (bEnd) {
                    if (GetMsgKind(m_pUnk134) == 3) {
                        fn_8026D074(m_pUnk134);
                        m_fStateCount = 10.0f;
                        fn_80073FB4(0);
                        bcs::CMainApp::SetPause(0x40, 0);
                    } else {
                        bcs::CMainApp::Start3DFade(0, 1.0f / 30.0f);
                        bcs::CMainApp::StartMasterFade(0, 1.0f / 30.0f);
                        bcs::CMainApp::SetPause(0x40, 0);
                    }
                    CNameMesg::MenuOutWindow();
                    m_nStateStep++;
                }
                break;
            }
            case 6:
                if (GetMsgKind(m_pUnk134) == 3) {
                    if (StateWorkDecStateCount_()) {
                        m_nStateStep = 99;
                    }
                } else {
                    int bEnd = 1;
                    if (bcs::CMainApp::IsPlay3DFade()) {
                        bEnd = 0;
                    }
                    if (bcs::CMainApp::IsPlayMasterFade()) {
                        bEnd = 0;
                    }
                    if (bEnd) {
                        m_nStateStep = 99;
                    }
                }
                break;
            case 99:
                // With nothing queued after it, the game waits for its
                // next start (state 3).
                if (FsmqGetNextState() == 0) {
                    FsmqPush(3);
                }
                FsmqNext();
                m_nStateStep++;
                break;
        }
    }

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

    // State 16: plays the game again from the start. After the fade the
    // game is reset (sounds, effects, balls, gimmicks, roulettes and
    // characters) and the start states are queued again as at the first
    // load.
    void CGame::LoopStateRestart()
    {
        if (m_nStateStep == 0) {
            int bEnd = 1;
            if (bcs::CMainApp::IsPlay3DFade()) {
                bEnd = 0;
            }
            if (bcs::CMainApp::IsPlayMasterFade()) {
                bEnd = 0;
            }
            if (bEnd) {
                m_fStateCount = 30.0f;
                m_nStateStep++;
            }
        } else if (StateWorkDecStateCount_()) {
            bcs::CMainApp::SetPause(0x21, 0);
            OnUnk08(1);
            CGameSnd::StopAllSound();
            EndScoredView_();
            fn_80074064(g_GameWork.m_nViewSplit, 0);
            KillTempChild_();
            CEffectCtrl::EraseEffectAll();
            CBallMan::AllReset();
            CGimmickMan::AllReset();
            CGimmickMan::QPanelAllReset();
            for (long i = 0; i < 4; i++) {
                CItemRoulette *pRoulette = m_apItemRoulette[i];
                if (pRoulette) {
                    pRoulette->StopSnd();
                    pRoulette->ChangeState(0);
                }
            }
            CChrMan::AllStockItemDelete();
            CChrMan::AllReset();
            CChrMan::AllSetState(4, -1, 0);
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            CChrMan::AllMotStop();
            fn_80236790();
            PrepareRestartGame_();
            CChrMan::RemakeChrBustImg(0);

            FsmqCutNext();
            if (bcs::dwc::CDwc::GetInstance()) {
                FsmqPush(24);
                if (g_GameWork.m_nReturnStep == 0) {
                    FsmqPush(28);
                }
            }
            if (g_GameWork.m_nGameMode != GAME_MODE_BOSS || !CEventDemo::IsEnable()) {
                FsmqPush(23);
            }
            if (!bcs::dwc::CDwc::GetInstance()) {
                if (m_pUnk134) {
                    SetMsgKind(m_pUnk134, 1);
                }
                FsmqPush(29);
            }
            if (CEventDemo::IsEnable()) {
                FsmqPush(2);
            }
            if (bcs::dwc::CDwc::GetInstance()) {
                FsmqPush(25);
            }
            FsmqPush(1);
            FsmqNext();
        }
    }

    void CGame::PrepareRestartGame_() {}

    void CGame::DtorStateRestart() {}

    void CGame::CtorStateChgField() {}

    void CGame::LoopStateChgField() {}

    void CGame::DtorStateChgField() {}

    // State 19: plays an event demo (EventDemoWork), skippable with the
    // demo skip input unless its flag forbids it.
    void CGame::CtorStateEventDemo()
    {
        EventDemoWork *pWork = FsmqGetWork<EventDemoWork>();
        pWork->m_nStep = 0;
        SetPreparationMode(1);
        if (pWork->m_bNoFadeIn == 0) {
            bcs::CMainApp::Start3DFade(0, 1.0f);
            bcs::CMainApp::StartMasterFade(0, 1.0f);
        }
        bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
        EventDemoSetUp();
        m_nStateStep = 0;
    }

    void CGame::LoopStateEventDemo()
    {
        EventDemoWork *pWork = FsmqGetWork<EventDemoWork>();
        switch (pWork->m_nStep) {
            case 0:
                bcs::CMainApp::Start3DFade(4, 1.0f);
                pWork->m_nStep++;
                break;
            case 1:
                fn_801D23B8(pWork->m_nUnk0C);
                CEventDemo::CreateDemo(pWork->m_nDemo, this, 1.0f / 60.0f, -1.0f);
                pWork->m_nStep++;
                break;
            case 2:
                if (!(pWork->m_nFlag & 1) && CheckDemoSkipInput_()) {
                    CEventDemo::m_Work.bCancel = 1;
                    pWork->m_nStep++;
                }
                // fall through
            default:
                if (CEventDemo::m_Work.bSkip) {
                    CEventDemo::m_Work.bSkip = 1;
                    bcs::CRenderMan::ReqPopViewSplitType(0);
                    // With nothing queued after it, the game goes on to
                    // its wait-start state (4).
                    if (FsmqGetNextState() == 0) {
                        FsmqPush(4);
                    }
                    if (pWork->m_nFadeType) {
                        float fSpeed = pWork->m_fFadeFrame;
                        if (fSpeed > 0.0f) {
                            fSpeed = 1.0f / fSpeed;
                        }
                        if (!(fSpeed > 0.0f)) {
                            fSpeed = 0.0f;
                        }
                        switch (pWork->m_nFadeType) {
                            case 1:
                                bcs::CMainApp::Start3DFade(0, fSpeed);
                                break;
                            case 2:
                                bcs::CMainApp::Start3DFade(1, fSpeed);
                                break;
                            case 3:
                                bcs::CMainApp::Start3DCrossFade(fSpeed, 0, 1);
                                break;
                        }
                    }
                    FsmqNext();
                }
                break;
        }
    }

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

    void CGame::EnterHaltState()
    {
        CGame *pGame = m_pInstance;
        if (pGame && pGame->FsmqGetState() != 27) {
            bcs::CMainApp::Start3DFade(4, 0.0f);
            bcs::CMainApp::StartMasterFade(4, 0.0f);
            bcs::CMainApp::Start3DFade(0, 0.0f);
            pGame->m_nUnk148 = 0;
            CGameSnd::GamePauseAll(1, 16.0f);
            fn_80070F88();
            bcs::CMainApp::SetPause(0x2000, 1);
            fn_80073FB4(1);
            pGame->FsmqChangeState(27);
        }
    }

    void CGame::ExitHaltState(int bToMenu)
    {
        CGame *pGame = m_pInstance;
        if (pGame && pGame->FsmqGetState() == 27) {
            pGame->ClearUnk08(0x2004);
            g_GameWork.m_nReturnStep = 0;
            if (bToMenu) {
                menu::CStateSave::GetInstance()->SetStartMenuMode(2);
            } else {
                menu::CStateSave::GetInstance()->SetStartMenuMode(8);
            }
            CGamePadCheck::Finalize();
            pGame->FsmqChangeState(18);
        }
    }

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

    // Before the game: every console reports ready with its frame counter,
    // and all start from console 0's.
    void CGame::LoopStateOnlineSync0()
    {
        long *pStep = FsmqGetWork<long>();
        dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
        if (!pDwc) {
            FsmqNext();
            return;
        }
        int bNext = 0;
        switch (*pStep) {
            case 0: {
                OnlineSyncMsg *pMsg = fn_80146248();
                pMsg->m_nState = 1000;
                pMsg->m_nTick = fn_8007C9BC();
                pMsg->m_nUnk08 = 0;
                fn_80146250(pDwc);
                fn_800B1AA8(GetDwc(pDwc), 0);
                (*pStep)++;
                break;
            }
            case 1: {
                int bReady = 1;
                for (long i = 0; i < fn_800B19D8(GetDwc(pDwc)); i++) {
                    long nState = fn_80146260(pDwc, i)->m_nState;
                    if (nState != 1000 && nState != 2000) {
                        bReady = 0;
                        break;
                    }
                }
                if (bReady) {
                    fn_800B1FB4(fn_80146260(pDwc, 0)->m_nTick);
                    fn_800B1D04(GetDwc(pDwc));
                    (*pStep)++;
                }
                break;
            }
            default:
                bNext = 1;
                break;
        }
        if (bNext) {
            FsmqNext();
        }
    }

    void CGame::DtorStateOnlineSync0() {}

    void CGame::CtorStateOnlineSync1()
    {
        *FsmqGetWork<long>() = 0;
    }

    // Starting the game: every console reports started (2000), then the
    // match is synchronised.
    void CGame::LoopStateOnlineSync1()
    {
        long *pStep = FsmqGetWork<long>();
        dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
        if (!pDwc) {
            FsmqNext();
            return;
        }
        int bNext = 0;
        switch (*pStep) {
            case 0:
                fn_80146248()->m_nState = 2000;
                fn_80146250(pDwc);
                (*pStep)++;
                break;
            case 1: {
                int bReady = 1;
                for (long i = 0; i < fn_800B19D8(GetDwc(pDwc)); i++) {
                    OnlineSyncMsg *pMsg = fn_80146260(pDwc, i);
                    if ((i == GetDwc(pDwc)->GetMyAID() || !fn_800B1A30(GetDwc(pDwc), i)) && pMsg->m_nState != 2000) {
                        bReady = 0;
                        break;
                    }
                }
                if (bReady) {
                    (*pStep)++;
                }
                break;
            }
            case 2:
            case 3:
            case 4:
                (*pStep)++;
                break;
            case 5:
                if (fn_800B1C30(GetDwc(pDwc), 1)) {
                    (*pStep)++;
                }
                break;
            case 6:
                if (GetDwc(pDwc)->IsInMatch()) {
                    (*pStep)++;
                }
                break;
            case 7:
            case 8:
            case 9:
            case 10:
                (*pStep)++;
                break;
            default:
                bNext = 1;
                break;
        }
        if (bNext) {
            FsmqNext();
        }
    }

    void CGame::DtorStateOnlineSync1() {}

    void CGame::CtorStateOnlineSync2()
    {
        *FsmqGetWork<long>() = 0;
        m_nUnk148 = 0;
    }

    // Ending the game: the match is closed, then every console reports
    // ended (3000; 1000 from one that has not started counts too).
    void CGame::LoopStateOnlineSync2()
    {
        long *pStep = FsmqGetWork<long>();
        dwc::CGameDwc *pDwc = dwc::CGameDwc::GetInstance();
        if (!pDwc) {
            FsmqNext();
            return;
        }
        int bNext = 0;
        switch (*pStep) {
            case 0:
                if (fn_800B1C30(GetDwc(pDwc), 0)) {
                    fn_800B1AA8(GetDwc(pDwc), 1);
                    (*pStep)++;
                }
                break;
            case 1:
                if (!GetDwc(pDwc)->IsInMatch()) {
                    (*pStep)++;
                }
                break;
            case 2:
                fn_80146248()->m_nState = 3000;
                fn_80146250(pDwc);
                (*pStep)++;
                break;
            case 3: {
                int bReady = 1;
                for (long i = 0; i < fn_800B19D8(GetDwc(pDwc)); i++) {
                    long nState = fn_80146260(pDwc, i)->m_nState;
                    if (nState != 3000 && nState != 1000) {
                        bReady = 0;
                        break;
                    }
                }
                if (bReady) {
                    (*pStep)++;
                }
                break;
            }
            default:
                bNext = 1;
                break;
        }
        if (bNext) {
            FsmqNext();
        }
    }

    void CGame::DtorStateOnlineSync2() {}

    void CGame::CtorStateGameExit()
    {
        SetPreparationMode(1);
        m_bExit = 1;
        m_nStateStep = 0;
    }

    // State 18: leaves the game. The sound fades out, everything the game
    // made is reset and deleted, and the main step moves on: back to the
    // tournament (3), the unlock screen first (4) when it has something to
    // show, or the menus (2).
    void CGame::LoopStateGameExit()
    {
        switch (m_nStateStep) {
            case 0:
                bcs::CMainApp::StartMasterFade(0, 1.0f / 30.0f);
                if (!bcs::CMainApp::IsPause(0x2021)) {
                    CGameBGM::SetBgmState(CGameBGM::STATE_FADE_OUT);
                    CGameSnd::StopAllSound(); // (30 frames in the original; see Finalize)
                }
                m_nStateStep++;
                break;
            case 1: {
                int bEnd = 1;
                if (bcs::CMainApp::IsPlay3DFade()) {
                    bEnd = 0;
                }
                if (bcs::CMainApp::IsPlayMasterFade()) {
                    bEnd = 0;
                }
                if (bEnd) {
                    KillTempChild_();
                    BgResetFade_();
                    CChrMan::AllReset();
                    CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                    CBallMan::AllReset();
                    CGimmickMan::AllReset();
                    CGimmickMan::QPanelAllReset();
                    SendMessage_(0x17, 0, 0, 0);
                    CEffectCtrl::EraseEffectAll();
                    m_nStateStep++;
                }
                break;
            }
            case 2:
                if (m_pTempParent) {
                    m_pTempParent->Delete();
                    m_pTempParent = 0;
                }
                for (int i = 0; i < 4; i++) {
                    m_apCamera[i] = 0;
                }
                fn_80236790();
                CGameCamera::Finalize();
                bcs::CCameraMan::RemoveEntryAll();
                CGameAccInput::Finalize();
                m_nStateStep++;
                break;
            case 3:
                DeleteAllChild();
                // fall through
            case 4:
                bcs::CRenderMan::ReqCleanUp();
                // fall through
            case 5:
                m_nStateStep++;
                break;
            case 6:
                bcs::CMainApp::SetPause(0x2021, 0);
                m_nStateStep++;
                break;
            default:
                fn_80073FB4(0);
                if (g_GameWork.m_nReturnStep == 1) {
                    if (g_GameWork.m_nUnk180 == 0 && Unlock::IsDisp(0)) {
                        Unlock::SetNextStep(3);
                        CMainStep::ChangeStep(4);
                    } else {
                        CMainStep::ChangeStep(3);
                    }
                } else {
                    long nMode = menu::CStateSave::GetInstance()->GetStartMenuMode();
                    int bUnlock = (nMode < 6 || nMode >= 9) ? 1 : 0;
                    if (bUnlock && g_GameWork.m_nUnk180 == 0 && Unlock::IsDisp(0)) {
                        Unlock::SetNextStep(2);
                        CMainStep::ChangeStep(4);
                    } else {
                        CTournament::ExitMode();
                        CMainStep::ChangeStep(2);
                    }
                }
                break;
        }
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

    void CGame::ChgCourtDemoSetup_()
    {
        FsmqCutNext();
        CGameBGM::SetBgmState(CGameBGM::STATE_JINGLE_2);
        if (CEventDemo::IsEnable()) {
            KillTempChild_();
            CChrMan::AllMotStop();
            CChrMan::AllStockItemDelete();
            CChrMan::AllReset();
            CChrMan::AllSetState(4, -1, 0);
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            CBallMan::AllReset();
            CGimmickMan::AllReset();
            CGimmickMan::QPanelAllReset();
            SendMessage_(0x17, 0, 0, 0);
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                ClearTeamArtsGauge_(i);
                g_GameWork.m_anTeamItem[i] = -1;
            }
            SetCockpitDisp_(0);
            SetCockpitItemIconDisp_(0, 0);
            SetDispRadarIcon_(0);
            EventDemoWork *pWork = static_cast<EventDemoWork *>(FsmqPushState(19));
            pWork->m_nFlag |= 1;
            pWork->m_nDemo = 5;
            pWork->m_nUnk0C = m_nUnk108;
            pWork->m_nFadeType = 0;
            pWork->m_fFadeFrame = 0.0f;
        }
        FsmqPush(9);
        FsmqNext();
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

    // Writes the game into the player's save data: the kind of game played
    // (0 normal, 1 tournament, 2 party, 3 boss, 4 online), and for a normal
    // game the score book counters, the stage and each character of the
    // player's team with the result (1 won, -1 lost, 0 draw).
    void CGame::ScoreBookApplyCountWork_()
    {
        SAVE_DATA_PLAYER *pData = CSave::GetPlayerData(-1);
        if (!pData) {
            return;
        }
        long nKind = 3;
        if (g_GameWork.m_nGameMode != GAME_MODE_BOSS) {
            if (g_GameWork.m_Unk151) {
                nKind = 4;
            } else if (CTournament::IsTournamentGame()) {
                nKind = 1;
            } else if (IsPartyGame()) {
                nKind = 2;
            } else {
                nKind = 0;
            }
        }
        fn_801C1F8C(pData, nKind, g_GameWork.m_nGameMode);
        if (nKind == 0) {
            fn_801C1F8C(pData, 1, g_GameWork.m_nGameMode);
        }
        if (nKind == 1) {
            fn_801C1F8C(pData, 0, g_GameWork.m_nGameMode);
        }
        fn_8017A4C4(g_GameWork.m_nGameMode, (g_GameWork.m_nUnk40 >> 9) & 1);
        if (g_GameWork.m_nGameType != GAME_TYPE_NORMAL) {
            return;
        }

        long nResult = 0;
        long bWin = 0;
        switch (g_GameWork.m_anTeamResult[g_GameWork.m_nUnk00]) {
            case 1:
                nResult = 1;
                bWin = 1;
                break;
            case 2:
                nResult = -1;
                break;
        }
        int bSecond = 0;
        if (g_GameWork.m_nUnk00 != 0) {
            bSecond = 1;
        }
        if (dwc::CGameDwc::GetInstance()) {
            fn_80145F8C();
        }
        fn_801C22A8(pData, g_GameWork.m_nGameMode, bWin, m_anScoreBookCount[0], m_anScoreBookCount[2],
                    m_anScoreBookCount[1]);
        fn_801C2B7C(pData, g_GameWork.m_nStage, g_GameWork.m_nGameMode);

        // The characters of the player's team for the play type.
        long anChr[6] = { -1, -1, -1, -1, -1, -1 };
        if (!bSecond) {
            switch (g_GameWork.m_nUnk06) {
                case 0:
                case 1:
                case 16:
                case 19:
                    anChr[0] = 0;
                    anChr[1] = 2;
                    anChr[2] = 4;
                    break;
                case 3:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 20:
                case 22:
                case 23:
                case 24:
                case 25:
                    anChr[0] = 0;
                    break;
                case 4:
                case 5:
                case 17:
                    anChr[0] = 0;
                    anChr[1] = 2;
                    break;
                case 14:
                case 15:
                case 18:
                case 21:
                    anChr[0] = 0;
                    anChr[1] = 4;
                    break;
            }
        } else {
            switch (g_GameWork.m_nUnk06) {
                case 3:
                case 8:
                case 17:
                case 19:
                case 22:
                case 23:
                    anChr[0] = 1;
                    break;
                case 5:
                case 7:
                case 20:
                    anChr[0] = 1;
                    anChr[1] = 3;
                    break;
                case 1:
                case 10:
                case 15:
                case 21:
                case 24:
                case 25:
                    anChr[0] = 1;
                    anChr[1] = 3;
                    anChr[2] = 5;
                    break;
                case 14:
                case 16:
                    anChr[0] = 1;
                    anChr[1] = 5;
                    break;
            }
        }
        for (long i = 0; i < 6; i++) {
            long nChr = anChr[i];
            if (nChr < 0) {
                continue;
            }
            if (GetCharType(nChr) >= 0x13) {
                // A Mii counts only when it is in the player's save data.
                RFLStoreData data = g_GameWork.m_aMiiStoreData[nChr];
                RFLAdditionalInfo info;
                if (bcs::CMiiMan::GetAdditionalInfo(info, data) == RFLErrcode_Success) {
                    if (pData->SearchMiiIndex(info.createID) >= 0) {
                        fn_801C2E94(pData, data, g_GameWork.m_nGameMode, nResult);
                    }
                }
            } else {
                fn_801C29E8(pData, GetCharType(nChr), g_GameWork.m_nGameMode, nResult);
            }
        }
        pData->CheckUnLockXml(g_GameWork.m_nGameMode);
        if (m_pUnk134) {
            fn_8026C9D8(m_pUnk134, 1);
        }
    }

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

    int CGamePauseTask::CanbePause() const
    {
        if (!m_bEnable) {
            return 0;
        }
        if (!CGame::IsMainLoopState(1)) {
            return 0;
        }
        if (CGame::g_GameWork.m_nFlag18 & 0x4306) {
            return 0;
        }
        if (CGame::g_GameWork.m_nUnk180) {
            return 0;
        }
        if (CGame::g_GameWork.m_Unk151) {
            return 0;
        }
        if (bcs::CMainApp::IsPause(0xFC67)) {
            return 0;
        }
        if (bcs::CMainApp::IsPauseReady(0xFC67)) {
            return 0;
        }
        if (CGame::g_uPauseBit & 0x1136) {
            return 0;
        }
        return 1;
    }

    long CGamePauseTask::GetPlusPad(int bHold)
    {
        if (bHold) {
            unsigned long nBits = GetActivePadBits();
            for (long i = 0; i < 4; i++) {
                if (!(nBits & (1 << i))) {
                    m_anUnk40[i] = 0;
                    m_afUnk50[i] = 0.0f;
                    continue;
                }
                if (bcs::CWifiPad::IsTrg(0x10, i)) {
                    m_anUnk40[i] = 1;
                    m_afUnk50[i] = 0.0f;
                }
                if (bcs::CWifiPad::IsDown(0x10, i)) {
                    if (m_anUnk40[i]) {
                        m_afUnk50[i] += GetFrameRateF();
                        if (m_afUnk50[i] > 6.0f) {
                            return i;
                        }
                    }
                } else {
                    m_anUnk40[i] = 0;
                    m_afUnk50[i] = 0.0f;
                }
            }
        } else {
            unsigned long nBits = GetActivePadBits();
            for (long i = 0; i < 4; i++) {
                if ((nBits & (1 << i)) && bcs::CWifiPad::IsTrg(0x10, i)) {
                    return i;
                }
            }
        }
        return -1;
    }

    long CGamePauseTask::GetMinusPad(int bHold)
    {
        if (bHold) {
            unsigned long nBits = GetActivePadBits();
            for (long i = 0; i < 4; i++) {
                if (!(nBits & (1 << i))) {
                    m_anUnk60[i] = 0;
                    m_afUnk70[i] = 0.0f;
                    continue;
                }
                if (bcs::CWifiPad::IsTrg(0x1000, i)) {
                    m_anUnk60[i] = 1;
                    m_afUnk70[i] = 0.0f;
                }
                if (bcs::CWifiPad::IsDown(0x1000, i)) {
                    if (m_anUnk60[i]) {
                        m_afUnk70[i] += GetFrameRateF();
                        if (m_afUnk70[i] > 6.0f) {
                            return i;
                        }
                    }
                } else {
                    m_anUnk60[i] = 0;
                    m_afUnk70[i] = 0.0f;
                }
            }
        } else {
            unsigned long nBits = GetActivePadBits();
            for (long i = 0; i < 4; i++) {
                if ((nBits & (1 << i)) && bcs::CWifiPad::IsTrg(0x1000, i)) {
                    return i;
                }
            }
        }
        return -1;
    }

    inline void CGamePauseTask::StopGame()
    {
        CGame::SendMessage(0x19, 0, 0, 0);
        CGame::StopCameraQuake(-1);
        CGame::StopAllCameraQuake();
        CGameSnd::GamePauseAll(1, 18.0f);
        bcs::CPadWI::PushMotorAll();
        bcs::CMainApp::SetPause(1, 1);
    }

    inline void CGamePauseTask::ResumeGame()
    {
        if (!m_nUnk38) {
            bcs::CMainApp::SetPause(1, 0);
        }
        if (m_nUnk38) {
            fn_80071128();
        } else {
            bcs::CPadWI::PopMotorAll();
            CGameSnd::GamePauseAll(0, 18.0f);
        }
        CGame::SendMessage(0x1A, m_nUnk38, 0, 0);
        CCockpit *pCockpit = CCockpit::GetTask();
        if (pCockpit) {
            pCockpit->ChangeState(2);
        }
        CMenuPadInfo::s_Instance.m_nPadLeader = 0;
        m_nStep = -1;
    }

    // Opens the pause menu on a + press (step 1) or, outside party and
    // boss games, the item info on a - press held for a moment (step 10),
    // and resumes the game when the menu has closed.
    void CGamePauseTask::Main()
    {
        switch (m_nStep) {
            case 0:
                if (CanbePause()) {
                    long nPad = GetPlusPad(1);
                    if (nPad >= 0) {
                        ClearHold();
                        if (bcs::CWifiPad::IsDown(0x10, nPad)) {
                            CMenuPadInfo::s_Instance.m_nPadLeader = nPad;
                            m_nUnk38 = 0;
                            StopGame();
                            if (CGame::m_pInstance) {
                                CGame::m_pInstance->MenuCallPause_();
                            }
                            menu::CSndEff::s_Instance.Play(0xBC2, 1);
                            m_nStep++;
                        }
                    }
                    if (CGame::g_GameWork.m_nGameType != CGame::GAME_TYPE_PARTY &&
                        CGame::g_GameWork.m_nGameMode != CGame::GAME_MODE_BOSS) {
                        nPad = GetMinusPad(1);
                        if (nPad >= 0) {
                            ClearHold();
                            if (bcs::CWifiPad::IsDown(0x1000, nPad)) {
                                CMenuPadInfo::s_Instance.m_nPadLeader = nPad;
                                m_nUnk38 = 0;
                                StopGame();
                                menu::CSndEff::s_Instance.Play(0xBC2, 1);
                                m_nStep = 10;
                            }
                        }
                    }
                }
                break;
            case 1:
                if (!bcs::CTaskMan::GetTask(CCockpitPause::s_TaskHandle)) {
                    ResumeGame();
                }
                break;
            case 2:
                if (!bcs::CTaskMan::GetTask(CCockpitPause::s_TaskHandle)) {
                    ResumeGame();
                }
                break;
            case 10:
                m_afUnk70[0] += GetFrameRateF();
                if (m_afUnk70[0] > 6.0f) {
                    m_afUnk70[0] = 0.0f;
                    CCockpit *pCockpit = CCockpit::GetTask();
                    CockpitItemInfoTab *pTab = new CockpitItemInfoTab(0x1082, 3, 0.0f);
                    pCockpit->AddObj(pTab);
                    pTab->Set();
                    m_nStep++;
                }
                break;
            case 11:
                if (CCockpit::IsEndObj(0x1082)) {
                    ResumeGame();
                }
                break;
            case 12:
                if (!bcs::CTaskMan::GetTask(CCockpitPause::s_TaskHandle)) {
                    ResumeGame();
                }
                break;
            default:
                m_nStep = 0;
                break;
        }
    }

    void CGamePauseTask::Disp() {}

    // Lifts the hit stop's pause when the task goes.
    CGameStopTask::~CGameStopTask()
    {
        bcs::CMainApp::SetPause(4, 0);
    }

    // The hit stop: stops the game (pause bit 4) for m_fFrame frames, then
    // brings the speed back from the slow motion over m_fStop frames.
    void CGameStopTask::Main()
    {
        switch (m_nState) {
            case 0:
                break;
            case 1:
                if (m_fFrame >= 0.0f) {
                    CGame::SendMessage(0x1B, 0, 0, 0);
                    m_nState++;
                }
                break;
            default:
                m_fFrame -= GetFrameRateF();
                if (m_fFrame <= 0.0f) {
                    CGame::SendMessage(0x1C, 0, 0, 0);
                    bcs::CMainApp::SetPause(4, 0);
                    if (m_fStop > 0.0f) {
                        CGameSpeedCtrl *pCtrl = CGame::CreateSpeedCtrl();
                        if (pCtrl) {
                            pCtrl->RestoreSpeed(m_fStop, 1, m_nType);
                        }
                    }
                    m_fFrame = 0.0f;
                    m_fStop = 0.0f;
                    m_nState = 0;
                }
                break;
        }
    }

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

    // Either a slow effect (m_nUnk5C == 1: down to m_fTo over m_fUnk54,
    // held for m_fTime, back over m_fUnk58) or a plain move from m_fFrom to
    // m_fTo over m_fTime (eased by m_nUnk60: 1 squared, 2 to the power 4.5).
    void CGameSpeedCtrl::Step(float fRate)
    {
        int bEnd = 0;
        if (m_nUnk5C == 1) {
            switch (m_nUnk64) {
                case 0:
                    if (m_fElapsed < m_fUnk54) {
                        m_fElapsed += fRate;
                        float fRatio = 1.0f;
                        if (m_fUnk54 > 0.0f) {
                            fRatio = CLAMP(m_fElapsed / m_fUnk54, 0.0f, 1.0f);
                        }
                        m_fSpeed = fRatio * (m_fTo - m_fFrom) + m_fFrom;
                    } else {
                        m_fElapsed = 0.0f;
                        m_fSpeed = m_fTo;
                        m_nUnk64++;
                    }
                    break;
                case 1:
                    m_fSpeed = m_fTo;
                    if (m_fElapsed < m_fTime) {
                        m_fElapsed += fRate;
                    } else {
                        m_nUnk64++;
                    }
                    break;
                case 2:
                    if (m_fElapsed < m_fUnk58) {
                        m_fElapsed += fRate;
                        float fRatio = 1.0f;
                        if (m_fUnk58 > 0.0f) {
                            fRatio = CLAMP(m_fElapsed / m_fUnk58, 0.0f, 1.0f);
                        }
                        m_fSpeed = fRatio * (m_fFrom - m_fTo) + m_fTo;
                    } else {
                        m_fElapsed = 0.0f;
                        bEnd = m_nUnk68;
                        m_fSpeed = m_fFrom;
                        m_nUnk64++;
                    }
                    break;
                default:
                    bEnd = m_nUnk68;
                    break;
            }
        } else if (m_fTime > 0.0f) {
            m_fElapsed = CLAMP(m_fElapsed + fRate, 0.0f, m_fTime);
            float fRatio = m_fElapsed / m_fTime;
            switch (m_nUnk60) {
                case 1:
                    fRatio *= fRatio;
                    break;
                case 2:
                    fRatio = pow(fRatio, 4.5);
                    break;
            }
            m_fSpeed = fRatio * (m_fTo - m_fFrom) + m_fFrom;
            if (m_fElapsed >= m_fTime) {
                bEnd = m_nUnk68;
            }
        } else {
            bEnd = m_nUnk68;
        }
        if (bEnd) {
            Delete();
        }
    }

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

    // Steps the speed controls and sets the slowest speed for the whole
    // game; without any, the game's own speed.
    void CGameTaskGroup::Main()
    {
        if (nw4r::ut::List_GetSize(&m_List)) {
            float fSpeed = CGame::g_GameWork.m_fGameSpeed;
            float fRate = bcs::CMainApp::GetAppFrameRateF();
            CGameSpeedCtrl *pCtrl = static_cast<CGameSpeedCtrl *>(nw4r::ut::List_GetNext(&m_List, 0));
            while (pCtrl) {
                if (!pCtrl->IsDelete()) {
                    pCtrl->Step(fRate);
                    float f = pCtrl->GetSpeed();
                    if (fSpeed > f && f > 0.0f) {
                        fSpeed = f;
                    }
                }
                pCtrl = static_cast<CGameSpeedCtrl *>(nw4r::ut::List_GetNext(&m_List, pCtrl));
            }
            bcs::CMainApp::SetUnivSpeed(fSpeed);
        } else {
            float fSpeed = CGame::g_GameWork.m_fGameSpeed;
            if (fSpeed != bcs::CMainApp::GetUnivSpeed()) {
                bcs::CMainApp::SetUnivSpeed(fSpeed);
            }
        }
    }

    // The settings of a game before anything is chosen: a normal
    // two-team game on level 2, 9000 frames (150 seconds) per period,
    // every pad in both teams, and the first eight Miis of the console
    // (topped up with the default Miis).
    void CGame::GameWork::SetDefault()
    {
        m_nUnk00 = 0;
        m_nGameMode = 0;
        m_nGameType = 0;
        m_nUnk06 = 1;
        m_nUnk07 = 0;
        m_nTournamentLevel = 0;
        m_nRound = 0;
        m_nStage = 1;
        m_Unk0B = 2;
        m_nPeriod = 0;
        m_nUnk44 = 20;
        m_nTeamNum = 2;
        m_nGoalNum = 2;
        m_Unk0E = 0;
        m_nUserTeam = 0;
        m_nGameLevel = 2;
        m_nQPanelLevel = 2;
        m_nFlag18 = 0;
        m_nUnk1C = 0;
        m_fUnk20 = 9000.0f;
        m_fGameTime = 9000.0f;
        m_fMaxShotTime = 0.0f;
        m_fUnk2C = 600.0f;
        m_fShotTime = 0.0f;
        m_fGameSpeed = 1.0f;
        m_Unk150 = 0;
        m_fUnk38 = 0.0f;
        m_nUnk40 = 0;
        m_nUnk1A8 = 0;
        m_Unk151 = 0;
        m_nSportsMode = 0;
        m_nGoalTeam = 0;
        m_nGoalChar = 0;
        m_nGoalBaseScore = 0;
        m_Unk160 = 0;
        m_nGoalScore = 0;
        m_fGoalShootRate = 0.0f;
        m_nGoalShootType = 0;
        m_nGoalBonusType = 0;
        m_nGoalBonusColor = 0;
        m_nGoalBonusPoint = 0;
        m_nGoalOwnGoal = 0;
        m_nUnk180 = 0;
        m_anAllPadBits[0] = 0xF;
        m_anAllPadBits[1] = 0xF;
        m_nUnk3C = 0;
        m_anUnk78[1] = -1;
        m_anUnk78[2] = -1;
        m_anUnk78[3] = -1;
        m_anUnk78[0] = 0;
        for (long i = 0; i < 4; i++) {
            m_anTeamCtrlType[i] = 0;
            m_anTeamColorIdx[i] = i;
            m_anUnk84[i] = i;
            m_afTeamComLevel[i] = 2.0f;
            m_anGoalIdx[i] = i & 1;
            m_anTeamCoin[i] = 0;
            m_anTeamDisableDropCoin[i] = 0;
            m_anCoinHistCount[i] = 0;
            m_anCoinHistTime[i] = 0;
            m_anTeamItem[i] = -1;
            m_anTeamResult[i] = 0;
            m_anUnk1A4[i] = i & 1;
            m_anPeriodStartCoin[i] = 0;
            m_anPeriodStartScore[i] = 0;
            m_anTeamMaxLife[i] = 100;
            m_anTeamInitLife[i] = 0;
            m_anTeamRebirthLife[i] = 20;
            m_anTeamSkillPoint[i] = 0;
            m_anTeamPadBits[i] = 0xF;
            m_anTeamOnlinePadBits[i] = 0xF;
            m_anTeamChrChangeCtrlType[i] = 0;
            m_abTeamAutoSpikeJump[i] = 0;
            m_anPassCountLimit[i] = 0;
        }
        m_anPadDefaultType[0] = -1;
        m_anPadDefaultType[1] = -1;
        m_anPadDefaultType[2] = -1;
        m_anPadDefaultType[3] = -1;
        m_anUnk1A0[0] = 0;
        m_anUnk1A0[1] = 1;
        m_anUnk1A0[2] = 0;
        m_anUnk1A0[3] = 1;
        m_anGoalTeam[0] = 0;
        m_anGoalTeam[1] = 1;
        m_anGoalTeam[2] = 2;
        m_anGoalTeam[3] = 3;
        for (long i = 0; i < 8; i++) {
            m_anCharType[i] = 0;
            m_anCharColor[i] = 0;
            m_anCharExFlag[i] = 0;
            m_anCharAssignPadNo[i] = i & 1;
            m_anCharAssignPlayerNo[i] = i & 1;
            for (long j = 0; j < 0x20; j++) {
                m_aCharName[i][j] = 0;
            }
        }
        m_anNpcType[1] = -1;
        m_anNpcType[2] = -1;
        m_anNpcType[3] = -1;
        m_anNpcType[4] = -1;
        m_anNpcType[5] = -1;
        m_anNpcType[6] = -1;
        m_anNpcType[7] = -1;
        m_anNpcType[0] = 1;
        m_anPartyGameParam[0] = 0;
        m_anPartyGameParam[1] = 0;
        m_anPartyGameParam[2] = 0;
        m_anPartyGameParam[3] = 0;
        m_nUnk1C8 = 0;
        m_nViewSplit = 0;
        m_nGimmickParamIdx = 0;
        m_nUnk430 = 0;

        long nChr = 0;
        long nIndex = -1;
        while (nChr < 8) {
            long nNext = -1;
            for (long i = nIndex + 1; i < 100; i++) {
                if (RFLIsAvailableOfficialData(i)) {
                    nNext = i;
                    break;
                }
            }
            nIndex = nNext;
            if (nIndex < 0) {
                break;
            }
            RFLStoreData data;
            switch (RFLGetStoreData(&data, RFLDataSource_Official, nIndex)) {
                case RFLErrcode_Success:
                    SetMiiStoreData(nChr, data);
                    break;
                case RFLErrcode_NotAvailable:
                case RFLErrcode_DBNodata:
                case 15:
                    break;
            }
            nChr++;
        }
        for (long i = 0; nChr < 8; i++) {
            RFLStoreData data;
            switch (RFLGetStoreData(&data, RFLDataSource_Default, i % 6)) {
                case RFLErrcode_Success:
                    SetMiiStoreData(nChr, data);
                    break;
                case RFLErrcode_NotAvailable:
                case RFLErrcode_DBNodata:
                case 15:
                    break;
            }
            nChr++;
        }
        m_nReturnStep = 0;
    }


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

    // Whether the player of team nTeam may change the team's leader in the
    // play type (GameWork::m_nUnk06): basketball and dodgeball have their
    // own tables, the other sports one for both teams.
    int CGame::IsPlayTypeCanChangeLeader(long nTeam)
    {
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_BASKET:
            case GAME_MODE_DODGE:
                if (nTeam == 0) {
                    switch (g_GameWork.m_nUnk06) {
                        case 0:
                        case 1:
                        case 4:
                        case 5:
                        case 16:
                        case 17:
                        case 19:
                            return 1;
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                        case 15:
                        case 18:
                            break;
                    }
                } else {
                    switch (g_GameWork.m_nUnk06) {
                        case 1:
                        case 5:
                        case 7:
                        case 10:
                        case 15:
                        case 20:
                        case 21:
                        case 24:
                        case 25:
                            return 1;
                        case 0:
                        case 2:
                        case 3:
                        case 4:
                        case 6:
                        case 8:
                        case 9:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                        case 16:
                        case 17:
                        case 18:
                        case 19:
                        case 22:
                        case 23:
                            break;
                    }
                }
                return 0;
            default:
                switch (g_GameWork.m_nUnk06) {
                    case 0:
                    case 1:
                    case 4:
                    case 5:
                        return 1;
                    case 16:
                    case 17:
                    case 19:
                        if (nTeam == 0) {
                            return 1;
                        }
                        break;
                    case 6:
                    case 7:
                    case 9:
                    case 10:
                    case 15:
                    case 18:
                    case 20:
                    case 21:
                    case 24:
                    case 25:
                        if (nTeam == 1) {
                            return 1;
                        }
                        break;
                }
                return 0;
        }
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

    // Shows or hides the item icons of the teams' roulettes (never shown
    // while rule option bit 2 is set).
    void CGame::SetCockpitItemIconDisp_(int bDisp, int n)
    {
        if (bDisp && !((g_GameWork.m_nUnk40 >> 2) & 1)) {
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                if (m_apItemRoulette[i]) {
                    m_apItemRoulette[i]->SetIconDisp(1, n);
                }
            }
        } else {
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                if (m_apItemRoulette[i]) {
                    m_apItemRoulette[i]->SetIconDisp(0, n);
                }
            }
        }
    }

    // Gives team nTeam item nItem: the roulette sound stops, the icon shows
    // the item (without the roulette) and the roulette stops on it; item 4
    // is given at once.
    void CGame::PushTeamItem_(long nTeam, long nItem)
    {
        if ((g_GameWork.m_nUnk40 >> 2) & 1) {
            return;
        }
        CItemRoulette *pRoulette = m_apItemRoulette[nTeam];
        if (pRoulette && pRoulette->IsPushable()) {
            pRoulette->StopSnd();
            pRoulette->m_nItem = nItem;
            if (pRoulette->m_bUseRoulette == 0) {
                fn_80139D28(pRoulette->m_nTeam, 0, pRoulette->m_nItem);
            }
            if (pRoulette->m_nItem == 4) {
                pRoulette->ChangeState(5);
            } else if (pRoulette->m_bUseRoulette) {
                pRoulette->ChangeState(4);
            } else {
                g_GameWork.m_anTeamItem[pRoulette->m_nTeam] = pRoulette->m_nItem;
            }
        }
    }

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
                    pRoulette->m_nItem = -1;
                    break;
                case 1:
                    pRoulette->ChangeState(2);
                    break;
            }
        }
    }

    // Gives team nTeam item nItem without the roulette spinning first (the
    // spin starts in state 3 when the roulette is used).
    void CGame::TeamItemStartRoulette_(long nTeam, long nItem)
    {
        if ((g_GameWork.m_nUnk40 >> 2) & 1) {
            return;
        }
        CItemRoulette *pRoulette = m_apItemRoulette[nTeam];
        if (pRoulette && pRoulette->IsPushable()) {
            pRoulette->m_nItem = nItem;
            if (pRoulette->m_bUseRoulette == 0) {
                g_GameWork.m_anTeamItem[pRoulette->m_nTeam] = pRoulette->m_nItem;
            } else {
                pRoulette->ChangeState(3);
            }
        }
    }

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

    namespace {
        // The pad of each of the eight characters (GameWork 0x60).
        inline void SetCharPads(long n0, long n1, long n2, long n3, long n4, long n5, long n6, long n7)
        {
            unsigned char *pPad = CGame::g_GameWork.m_anCharAssignPadNo;
            pPad[0] = n0;
            pPad[1] = n1;
            pPad[2] = n2;
            pPad[3] = n3;
            pPad[4] = n4;
            pPad[5] = n5;
            pPad[6] = n6;
            pPad[7] = n7;
        }
    }

    // Gives each character the pad of its player for the play type
    // (GameWork::m_nUnk06); characters without a player of their own share
    // the pads 0 and 1.
    void CGame::SetPadAssignSemiAuto()
    {
        SetCharPads(0, 1, 0, 1, 0, 1, 0, 1);
        switch (g_GameWork.m_nUnk06) {
            case 6:
            case 7:
            case 15:
            case 18:
                SetCharPads(0, 2, 1, 2, 0, 2, 0, 2);
                break;
            case 9:
            case 10:
                SetCharPads(0, 3, 1, 3, 2, 3, 0, 3);
                break;
            case 8:
            case 14:
                SetCharPads(0, 2, 1, 3, 0, 2, 0, 2);
                break;
            case 16:
            case 17:
                SetCharPads(0, 1, 0, 2, 0, 1, 0, 1);
                break;
            case 19:
                SetCharPads(0, 1, 0, 2, 0, 3, 0, 1);
                break;
            case 20:
            case 21:
                SetCharPads(0, 1, 2, 1, 0, 1, 0, 1);
                break;
            case 22:
                SetCharPads(0, 1, 2, 3, 0, 1, 0, 1);
                break;
            case 23:
                SetCharPads(0, 1, 3, 2, 0, 1, 0, 1);
                break;
            case 24:
                SetCharPads(0, 2, 1, 2, 3, 2, 0, 2);
                break;
            case 25:
                SetCharPads(0, 1, 2, 1, 3, 1, 0, 1);
                break;
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 11:
            case 12:
            case 13:
            default:
                SetCharPads(0, 1, 0, 1, 0, 1, 0, 1);
                break;
        }
    }

    namespace {
        // Leaves character slot nChr empty.
        inline void ClearChar(long nChr)
        {
            CGame::g_GameWork.m_anCharType[nChr] = -1;
            CGame::g_GameWork.m_anCharColor[nChr] = 0;
            CGame::g_GameWork.m_anCharExFlag[nChr] = 0;
        }
    }

    void CGame::SetCharSemiAuto()
    {
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_DODGE: {
                long nFirst;
                switch (g_GameWork.m_nUnk06) {
                    case 13:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 0:
                    case 9:
                    case 18:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        nFirst = 6;
                        break;
                    case 11:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 2:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 3:
                        for (long i = 2; i < 8; i++) {
                            ClearChar(i);
                        }
                        nFirst = 2;
                        break;
                    case 12:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 4:
                    case 6:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 5:
                    case 7:
                    case 8:
                    case 17:
                    case 20:
                    case 22:
                    case 23:
                        for (long i = 4; i < 8; i++) {
                            ClearChar(i);
                        }
                        nFirst = 4;
                        break;
                    default:
                        nFirst = 6;
                        break;
                }
                if (g_GameWork.m_nGameMode == GAME_MODE_DODGE && g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    nFirst = -1;
                }
                if (nFirst >= 0) {
                    g_GameWork.m_anCharType[nFirst] = 4;
                    g_GameWork.m_anCharType[nFirst + 1] = 4;
                    g_GameWork.m_anCharColor[nFirst] = 5;
                    g_GameWork.m_anCharColor[nFirst + 1] = 4;
                    g_GameWork.m_anCharExFlag[nFirst] = 3;
                    g_GameWork.m_anCharExFlag[nFirst + 1] = 3;
                }
                break;
            }
            case GAME_MODE_VOLLEY:
            default:
                switch (g_GameWork.m_nUnk06) {
                    case 13:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 0:
                    case 9:
                    case 18:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        ClearChar(6);
                        ClearChar(7);
                        break;
                    case 11:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 2:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 3:
                        for (long i = 2; i < 8; i++) {
                            ClearChar(i);
                        }
                        break;
                    case 12:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 4:
                    case 6:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 5:
                    case 7:
                    case 8:
                    case 17:
                        for (long i = 4; i < 8; i++) {
                            ClearChar(i);
                        }
                        break;
                    default:
                        ClearChar(6);
                        ClearChar(7);
                        break;
                }
                break;
        }
    }

    // Fills in the rule settings of the game from its mode, game type and
    // play type: teams, colours, periods and times, the views, the
    // computer teams and unused characters, the extra characters
    // (dodgeball, hockey), the NPCs of the stage and the party games'
    // special rules; then resets the per-game work.
    void CGame::SetParamSemiAuto()
    {
        g_GameWork.m_nTeamNum = 2;
        g_GameWork.m_nGoalNum = 2;
        g_GameWork.m_nPeriod = 0;
        g_GameWork.m_Unk0E = 0;
        g_GameWork.m_anTeamColorIdx[0] = 0;
        g_GameWork.m_anTeamColorIdx[1] = 1;
        g_GameWork.m_anTeamColorIdx[2] = 2;
        g_GameWork.m_anTeamColorIdx[3] = 3;
        if (g_GameWork.m_nUnk00 != 0) {
            g_GameWork.m_anTeamColorIdx[0] = 1;
            g_GameWork.m_anTeamColorIdx[1] = 0;
            g_GameWork.m_anTeamColorIdx[2] = 3;
            g_GameWork.m_anTeamColorIdx[3] = 2;
        }

        // Periods and times.
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_VOLLEY:
                g_GameWork.m_fUnk20 = 0.0f;
                g_GameWork.m_Unk0B = 3;
                switch (g_GameWork.m_nUnk06) {
                    case 3:
                        g_GameWork.m_nUnk06 = 5;
                        break;
                    case 2:
                        g_GameWork.m_nUnk06 = 4;
                        break;
                    case 11:
                        g_GameWork.m_nUnk06 = 12;
                        break;
                }
                g_GameWork.m_Unk150 = 0;
                g_GameWork.m_Unk0E = 0;
                g_GameWork.m_nUnk44 = 15;
                break;
            case GAME_MODE_DODGE:
                g_GameWork.m_fUnk20 = 10800.0f;
                g_GameWork.m_Unk0B = 3;
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    for (long i = 0; i < 4; i++) {
                        g_GameWork.m_anTeamMaxLife[i] = 100;
                        g_GameWork.m_anTeamInitLife[i] = 100;
                        g_GameWork.m_anTeamRebirthLife[i] = 100;
                    }
                    g_GameWork.m_fUnk20 = 7200.0f;
                } else {
                    for (long i = 0; i < 4; i++) {
                        g_GameWork.m_anTeamMaxLife[i] = 100;
                        g_GameWork.m_anTeamRebirthLife[i] = 20;
                    }
                    g_GameWork.m_fMaxShotTime = 1800.0f;
                }
                break;
            case GAME_MODE_HOCKEY:
                g_GameWork.m_fUnk20 = 10800.0f;
                g_GameWork.m_Unk0B = 2;
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    if (!CTournament::IsTournamentGame()) {
                        g_GameWork.m_nTeamNum = 4;
                    }
                    g_GameWork.m_fUnk20 = 18000.0f;
                }
                break;
            case GAME_MODE_BOSS:
                g_GameWork.m_fUnk20 = 0.0f;
                g_GameWork.m_Unk0B = 1;
                g_GameWork.m_Unk150 = 0;
                g_GameWork.m_Unk0E = 0;
                g_GameWork.m_nUnk44 = 15;
                break;
            case GAME_MODE_BASKET:
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY && !CTournament::IsTournamentGame()) {
                    g_GameWork.m_nTeamNum = 4;
                }
                // fall through
            default:
                g_GameWork.m_fUnk20 = 9000.0f;
                g_GameWork.m_Unk0B = 2;
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    g_GameWork.m_fUnk20 = 18000.0f;
                } else {
                    g_GameWork.m_fMaxShotTime = 1440.0f;
                }
                break;
        }

        // The views: one per team, split when two players play against
        // each other on one console.
        for (long i = 0; i < 4; i++) {
            SetViewTeam(i, i % 2);
        }
        long nSplit = 0;
        if (!g_GameWork.m_Unk151) {
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
                    nSplit = fn_800CFEDC();
                    break;
            }
        }
        g_GameWork.m_nViewSplit = nSplit;

        // The computer teams and the characters not in play.
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_DODGE:
            case GAME_MODE_HOCKEY: {
                long nFirst;
                switch (g_GameWork.m_nUnk06) {
                    case 13:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 0:
                    case 9:
                    case 18:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        nFirst = 6;
                        break;
                    case 11:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 2:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 3:
                        for (long i = 2; i < 8; i++) {
                            ClearChar(i);
                        }
                        nFirst = 2;
                        break;
                    case 12:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 4:
                    case 6:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 5:
                    case 7:
                    case 8:
                    case 17:
                    case 20:
                    case 22:
                    case 23:
                        for (long i = 4; i < 8; i++) {
                            ClearChar(i);
                        }
                        nFirst = 4;
                        break;
                    default:
                        nFirst = 6;
                        break;
                }
                if (g_GameWork.m_nGameMode == GAME_MODE_DODGE && g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    nFirst = -1;
                }
                // Two extra characters after the players (the dodgeball
                // outfielders, the hockey keepers).
                if (nFirst >= 0) {
                    long nType = 4;
                    if (g_GameWork.m_nGameMode == GAME_MODE_HOCKEY) {
                        nType = 0x12;
                    }
                    g_GameWork.m_anCharType[nFirst] = nType;
                    g_GameWork.m_anCharType[nFirst + 1] = nType;
                    if (g_GameWork.m_nUnk00 != 0) {
                        g_GameWork.m_anCharColor[nFirst] = 4;
                        g_GameWork.m_anCharColor[nFirst + 1] = 5;
                    } else {
                        g_GameWork.m_anCharColor[nFirst] = 5;
                        g_GameWork.m_anCharColor[nFirst + 1] = 4;
                    }
                    g_GameWork.m_anCharExFlag[nFirst] = 3;
                    g_GameWork.m_anCharExFlag[nFirst + 1] = 3;
                }
                break;
            }
            case GAME_MODE_VOLLEY:
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    break;
                }
                // fall through
            default:
                switch (g_GameWork.m_nUnk06) {
                    case 13:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 0:
                    case 9:
                    case 18:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        ClearChar(6);
                        ClearChar(7);
                        break;
                    case 11:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 2:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 3:
                        for (long i = 2; i < 8; i++) {
                            ClearChar(i);
                        }
                        break;
                    case 12:
                        g_GameWork.m_anTeamCtrlType[0] = 1;
                        // fall through
                    case 4:
                    case 6:
                        g_GameWork.m_anTeamCtrlType[1] = 1;
                        // fall through
                    case 5:
                    case 7:
                    case 8:
                    case 17:
                    case 20:
                    case 22:
                    case 23:
                        for (long i = 4; i < 8; i++) {
                            ClearChar(i);
                        }
                        break;
                    default:
                        ClearChar(6);
                        ClearChar(7);
                        break;
                }
                break;
        }

        // The NPCs of the stage.
        g_GameWork.m_anNpcType[0] = 1;
        switch (g_GameWork.m_nStage) {
            case 6:
                g_GameWork.m_anNpcType[1] = 3;
                g_GameWork.m_anNpcType[2] = 4;
                g_GameWork.m_anNpcType[3] = 5;
                break;
            case 7:
                g_GameWork.m_anNpcType[1] = 2;
                break;
            case 11:
                g_GameWork.m_anNpcType[1] = 9;
                break;
            case 15:
                g_GameWork.m_anNpcType[1] = 0x13;
                break;
            case 17:
                g_GameWork.m_anNpcType[1] = 0x14;
                break;
        }

        // The party games.
        switch (g_GameWork.m_nGameMode) {
            case GAME_MODE_BASKET:
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    g_GameWork.m_anUnk78[0] = 1;
                    g_GameWork.m_anNpcType[0] = 1;
                    g_GameWork.m_anNpcType[1] = 2;
                    g_GameWork.m_anNpcType[2] = 11;
                }
                break;
            case GAME_MODE_DODGE:
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    g_GameWork.m_anUnk78[0] = 0;
                    g_GameWork.m_anNpcType[0] = 1;
                    g_GameWork.m_anNpcType[1] = 12;
                }
                break;
            case GAME_MODE_VOLLEY:
                if (g_GameWork.m_nGameType == GAME_TYPE_PARTY) {
                    // One player per team, no periods and no time. (The
                    // original also looks for a character chosen twice
                    // here, without using the result.)
                    g_GameWork.m_nPeriod = 0;
                    g_GameWork.m_Unk0B = 1;
                    g_GameWork.m_Unk0E = 0;
                    g_GameWork.m_fUnk20 = 0.0f;
                    ClearChar(1);
                    ClearChar(3);
                    ClearChar(5);
                    ClearChar(7);
                    g_GameWork.m_anUnk78[0] = 1;
                    for (long i = 0; i < 4; i++) {
                        g_GameWork.m_anTeamCtrlType[i] = 0;
                        SetGoalIdx(i, i % 2);
                    }
                    g_GameWork.m_nViewSplit = 0;
                    g_GameWork.m_Unk150 = 1;
                    g_GameWork.m_nUnk44 = 999;
                    g_GameWork.m_anNpcType[0] = 1;
                    switch (g_GameWork.m_nStage) {
                        case 0x29:
                            g_GameWork.m_anNpcType[1] = 13;
                            break;
                        case 0x2A:
                            g_GameWork.m_anNpcType[1] = 14;
                            break;
                        default:
                            g_GameWork.m_anNpcType[1] = 6;
                            break;
                    }
                }
                break;
            case GAME_MODE_BOSS:
                g_GameWork.m_fUnk20 = 0.0f;
                ClearChar(1);
                ClearChar(3);
                ClearChar(5);
                ClearChar(7);
                g_GameWork.m_anUnk78[0] = 0;
                g_GameWork.m_nViewSplit = 0;
                for (long i = 0; i < 4; i++) {
                    g_GameWork.m_anTeamCtrlType[i] = 0;
                    SetGoalIdx(i, i % 2);
                }
                g_GameWork.m_anNpcType[0] = 1;
                g_GameWork.m_anNpcType[1] = 0;
                if (g_GameWork.m_anPartyGameParam[0] >= 2) {
                    g_GameWork.m_anNpcType[1] = 15;
                }
                break;
        }

        g_GameWork.m_fUnk38 = 0.0f;
        GameWorkReset_();
        g_GameWork.m_nFlag18 = 0;
    }

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

    int CShadowTexMan::IsEnable(long nType)
    {
        CShadowTexMan *pMan = m_pInstance;
        if (pMan == 0) {
            return 0;
        }
        if (pMan->m_bEnable == 0) {
            return 0;
        }
        switch (nType) {
            case 0:
                switch (CGame::g_GameWork.m_nGameMode) {
                    case CGame::GAME_MODE_BASKET:
                    case CGame::GAME_MODE_VOLLEY:
                    case CGame::GAME_MODE_HOCKEY:
                        if (bcs::CWorldEnv::GetViewSplitNum() >= 2) {
                            return 1;
                        }
                        return 0;
                    case CGame::GAME_MODE_DODGE:
                        if (bcs::CWorldEnv::GetViewSplitNum() >= 2) {
                            return 1;
                        }
                        if (CGame::g_GameWork.m_Unk151) {
                            return 1;
                        }
                        return 0;
                }
                return 0;
            case 1:
            case 2:
                return 1;
        }
        return 0;
    }
}
