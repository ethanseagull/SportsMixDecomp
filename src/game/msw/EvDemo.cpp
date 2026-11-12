// game/msw/EvDemo
// The victory demos every sport starts (.text 0x8011AF54..0x8011D808): two
// event scenes, one for the sports games, one for the party games, and
// EvDemo::CreateVictoryDemo, which picks one.
#include "game/msw/EvDemo.hpp"
#include <stdio.h>
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCameraAnm.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CEvNodeData.hpp"
#include "game/msw/CEventChrCore.hpp"
#include "game/msw/CEventDemoBase.hpp"
#include "game/msw/CEventMove.hpp"
#include "game/msw/CEventScene.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CSceneCalc.hpp"
#include "game/msw/CTournament.hpp"

namespace msw {

    // The victory demo of a sports game (class name guessed; unnamed in the
    // DOL, vtable lbl_8046F6A4, 0xD8 bytes). The two teams stand on the node
    // paths of the judge cuts and play the motion of their result.
    class CVictoryDemo : public CEventScene {
        public:
            // nTeam is the team the camera follows; nArg goes to CEventScene.
            CVictoryDemo(long nTeam, long nArg, bcs::CTask *pParent, const VICTORY_DATA *pData);
            virtual ~CVictoryDemo();

            virtual nw4r::g3d::ResFile GetRes();
            virtual int EventFirst();
            virtual void EventStart();
            // (slot 0x24) Hands changed demo flags to EventFlags and runs
            // the timers.
            virtual void EventKill();
            virtual long EventPlay(CUT_WORK &rCut);
            virtual void EventTimer(short nId, short nArg, unsigned long ulArg);
            virtual void EventTimerReset(short nId, short nArg, unsigned long ulArg);
            virtual void EventFlags(unsigned long nFlags, unsigned long nNew);

        private:
            // Puts the models of team nTeam on the node paths of animation
            // pAnm and plays their result: motion, voice and sound; only the
            // first model of the team followed is heard at full volume.
            void _PlayResult(const char *pAnm, long nTeam);

            CSceneCalc *m_pCalc;         // 0xA4
            const VICTORY_DATA *m_pData; // 0xA8: motion table, telops, voices, sounds
            short m_nUnkAC;              // 0xAC
            short m_nTelop;              // 0xAE: -1 for none
            short m_nResult;             // 0xB0: 0 won, 1 lost, 2 draw, -1 none
            short m_nTeam;               // 0xB2
            short m_nResultCut;          // 0xB4: the cut that shows the result
            float m_afUnkB8[8];          // 0xB8
    }; // size 0xD8

    // The victory demo of a party game (class name guessed; unnamed in the
    // DOL, vtable lbl_8046F660, 0xDC bytes): every player stands on a node
    // path of its own and the new record and perfect telops can follow.
    class CVictoryDemoParty : public CEventScene {
        public:
            CVictoryDemoParty(long nTeam, long nArg, bcs::CTask *pParent, const VICTORY_DATA *pData);
            virtual ~CVictoryDemoParty();

            virtual nw4r::g3d::ResFile GetRes();
            virtual int EventFirst();
            virtual void EventStart();
            virtual void EventKill();
            virtual long EventPlay(CUT_WORK &rCut);
            virtual void EventTimer(short nId, short nArg, unsigned long ulArg);
            virtual void EventTimerReset(short nId, short nArg, unsigned long ulArg);
            virtual void EventFlags(unsigned long nFlags, unsigned long nNew);

        private:
            // Puts model nChr on its node path ("player%02dW" for a winner)
            // and returns its result.
            long _SetPlayerNode(long nChr, long nStep);
            // _SetPlayerNode for every nStep-th model of animation pAnm,
            // then the motion, voice and sound of its result.
            void _PlayResult(const char *pAnm, long nStep);

            CSceneCalc *m_pCalc;         // 0xA4
            const VICTORY_DATA *m_pData; // 0xA8
            short m_nUnkAC;              // 0xAC
            short m_nTelop;              // 0xAE
            short m_nResult;             // 0xB0: 0 won, 1 lost, 2 draw
            short m_nTeam;               // 0xB2
            short m_nResultCut;          // 0xB4
            short m_nStep;               // 0xB6: 2 when the teams are pairs
            signed char m_abNoTeam[4];   // 0xB8: team n plays no part
            float m_afVolume[8];         // 0xBC: per model
    }; // size 0xDC

    namespace EvDemo {

        // (names guessed) Set by SetNewRecord and SetPerfect, cleared when
        // the party victory demo ends.
        static signed char s_bNewRecord;
        static signed char s_bPerfect;
    }

    namespace {
        // The motion sequences of the three results (struct name guessed).
        struct RESULT_MOT {
            CEventModel::MOT_DATA aWin[3];
            CEventModel::MOT_DATA aLose[3];
            CEventModel::MOT_DATA aDraw[3];
        };
        const RESULT_MOT s_ResultMot = {
            {{1, 0, 8.0f, 0.0f}, {2, 2, 8.0f, 0.0f}, {0, 0, 0.0f, 0.0f}},
            {{3, 0, 8.0f, 0.0f}, {4, 2, 8.0f, 0.0f}, {0, 0, 0.0f, 0.0f}},
            {{5, 0, 8.0f, 0.0f}, {6, 2, 8.0f, 0.0f}, {0, 0, 0.0f, 0.0f}},
        };
        // The telops (CEventDemoBase::GetTelopId) of an extra match: mission
        // cleared or not.
        const long s_anExtraTelop[3] = {0, 1, 1};
        // The telop objects of a mixed-sports party game: won, lost, draw.
        const long s_anMixTelop[3] = {0x1069, 0x106A, 0x106A};
    }

    CVictoryDemo::CVictoryDemo(long nTeam, long nArg, bcs::CTask *pParent, const VICTORY_DATA *pData)
        : CEventScene(8, pParent, nArg), m_pData(pData)
    {
        m_nChrNum = CChrMan::GetChrCoreNum();
        m_pCalc = new CSceneCalc(this);
        bcs::CRenderMan::EnableDepthOfFieldEffect(0);
        m_nUnkAC = 0;
        m_nTeam = nTeam;
        m_nTelop = -1;
        for (int i = 0; i < 8; i++) {
            m_afUnkB8[i] = 0.0f;
        }

        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (CTournament::IsExtraMatchGame() && pDemo != 0) {
            m_nResult = !CTournament::IsMissionClear();
            m_nTelop = pDemo->GetTelopId(s_anExtraTelop[m_nResult]);
        } else {
            switch (CGame::GetTeamResult(m_nTeam)) {
                case 1:
                    m_nResult = 0;
                    break;
                case 2:
                    m_nResult = 1;
                    break;
                case 3:
                    m_nResult = 2;
                    break;
                default:
                    m_nResult = -1;
                    break;
            }
            if (m_nResult != -1) {
                m_nTelop = m_pData->pData[1][m_nResult];
            }
        }
    }

    CVictoryDemo::~CVictoryDemo()
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            pDemo->RemoveTelopAll();
            pDemo->SetCockpitState(0, 1);
        }
    }

    nw4r::g3d::ResFile CVictoryDemo::GetRes()
    {
        return CLoadData::GetRes(0x60);
    }

    // Makes the six timers, one cut per scene of the camera animation, the
    // two judge cuts of the team followed and the models of all players.
    int CVictoryDemo::EventFirst()
    {
        CEventDemoBase::GetInstance()->SetCockpitState(0, 2);
        CChrMan::SetChrMarkerDisp(0);
        CBallMan::AllReset();
        CBallMan::AllSleep();
        CreateTimer(6);

        long num = m_pCamera->GetSceneNum();
        if (num > 0) {
            if (m_pCut != 0) {
                delete[] m_pCut;
            }
            m_pCut = 0;
            m_pCut = new CUT_WORK[num];
            const char *pNoData = "NoData";
            for (long i = 0; i < num; i++) {
                CUT_WORK &rCut = m_pCut[i];
                rCut.pName = pNoData;
                rCut.nUnk04 = -1;
                rCut.nUnk06 = -1;
                rCut.fUnk1C = 0.0f;
                rCut.fUnk20 = 1.0f;
                rCut.nUnk08 = 0x7F;
                rCut.nUnk0A = 0;
                rCut.fUnk10 = 0.0f;
                rCut.fUnk18 = 0.0f;
                rCut.fUnk14 = 0.0f;
                rCut.nUnk0C = -1;
            }

            if (m_nResult >= 0) {
                switch (CGame::g_GameWork.m_nUnk06) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 12:
                    case 17:
                    case 20:
                    case 22:
                    case 23:
                        switch (m_nTeam) {
                            case 0:
                                AddCut("judge_event_1P_cut02", 4, 0, 0.0f);
                                m_nResultCut = m_nCutNum;
                                AddCut("judge_event_1P_cut03", 5, 0, 0.0f);
                                break;
                            case 1:
                                AddCut("judge_event_2P_cut02", 6, 1, 0.0f);
                                m_nResultCut = m_nCutNum;
                                AddCut("judge_event_2P_cut03", 7, 1, 0.0f);
                                break;
                        }
                        break;
                    default:
                        switch (m_nTeam) {
                            case 0:
                                AddCut("judge_event_1P_cut00", 4, 0, 0.0f);
                                m_nResultCut = m_nCutNum;
                                AddCut("judge_event_1P_cut01", 5, 0, 0.0f);
                                break;
                            case 1:
                                AddCut("judge_event_2P_cut00", 6, 1, 0.0f);
                                m_nResultCut = m_nCutNum;
                                AddCut("judge_event_2P_cut01", 7, 1, 0.0f);
                                break;
                        }
                        break;
                }
            }

            if (m_nCutNum > 0 && m_nResultCut != m_nCutNum) {
                CUT_WORK &rFirst = m_pCut[0];
                rFirst.nUnk08 = 0x7E;
                rFirst.nUnk0A = 2;

                for (long i = 0; i < m_nChrNum; i++) {
                    long nColor = CGame::GetCharColor(i);
                    long nType = CGame::GetCharType(i);
                    if (nType != -1) {
                        m_ppChr[i] = new CEventChrCore(nType, i, m_pData->pData[0]);
                        if (m_ppChr[i]->IsObj()) {
                            m_ppChr[i]->Create();
                            m_ppChr[i]->SetDisp(0);
                            new CEventMove(m_ppChr[i], m_pNodeData, 1);
                            m_ppChr[i]->SetFloorMove(1);
                            m_ppChr[i]->SetEnableFlag18(0);
                            if (nColor < 4) {
                                m_nChrMask |= 1 << i;
                            }
                        } else {
                            if (m_ppChr[i] != 0) {
                                delete m_ppChr[i];
                            }
                            m_ppChr[i] = 0;
                        }
                    }
                }
            }
        }
        return 1;
    }

    void CVictoryDemo::EventKill()
    {
        unsigned long nFlags = _GetDemoFlags();
        if (m_nDemoFlags != nFlags) {
            EventFlags(nFlags, nFlags & (nFlags ^ m_nDemoFlags));
        }
        m_nDemoFlags = nFlags;
        _UpdateTimer(GetFrameRateF());
    }

    // Follows the team, moves the scene to the stage of a party game and
    // puts both teams on the node paths of the first cut.
    void CVictoryDemo::EventStart()
    {
        CUT_WORK *pCut = m_pCut;
        switch (m_nTeam) {
            case 0:
                m_pCalc->SetFloorChr(0);
                break;
            case 1:
                m_pCalc->SetFloorChr(1);
                break;
        }
        m_pCalc->SetCalc(1);

        if (CGame::g_GameWork.GetGameType() == 2) {
            switch (CGame::g_GameWork.m_nStage) {
                case 0x28:
                case 0x2A:
                case 0x46:
                case 0x55:
                    AddOffset(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                    break;
                case 0x48:
                    AddOffset(bcs::FVEC3(0.0f, 0.0f, -3.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                    break;
                case 0x47:
                    AddOffset(bcs::FVEC3(0.0f, -0.05f, -3.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                    break;
                case 0x29:
                    AddOffset(bcs::FVEC3(0.0f, -0.05f, -1.5f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                    break;
                case 0x57:
                    AddOffset(bcs::FVEC3(2.25f, 0.0f, 4.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                    break;
                default:
                    AddOffset(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 0.0f, 0.0f), 1);
                    break;
            }
        }

        _SetDemoTask(this);
        SetDispChr(0);
        SetFloorMove(CGame::g_GameWork.m_nStage, 0, 0);
        SetFloorMove(CGame::g_GameWork.m_nStage, 1, 0);
        _SetTeamNode(pCut->pName, 0, 0, 2, 1.0f);
        _SetTeamNode(pCut->pName, 1, 0, 2, 1.0f);
        AddTimer(0, 10.0f, 0, 0);
    }

    long CVictoryDemo::EventPlay(CUT_WORK &rCut)
    {
        if (IsFadeOutOnly(rCut.nUnk08)) {
            return StartFadeOut(rCut, rCut.nUnk08);
        }
        if (m_nResultCut == m_nUnk9D) {
            AddTimerRel(2, 120.0f, 0, 0);
            AddTimerRel(4, 200.0f, 0, 0);
            return 6;
        }
        return StartFadeOut(rCut, rCut.nUnk08);
    }

    // Timer 0 lets the camera play on, 1 ends the scene, 2 shows the
    // result and its telop, 3 closes the telop and ends the scene, 5 (from
    // EventFlags) starts timer 3; the others go to the demo.
    void CVictoryDemo::EventTimer(short nId, short nArg, unsigned long ulArg)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetInstance();
        switch (nId) {
            case 0:
                m_pCamera->Resume();
                break;
            case 1:
                m_nUnk9E = 7;
                break;
            case 2: {
                CUT_WORK &rCut = m_pCut[m_nUnk9D];
                _UpdateCutStep();
                _PlayResult(rCut.pName, 0);
                _PlayResult(rCut.pName, 1);
                _PlayCutCamera(rCut);
                pDemo->CreateTelop(m_nTelop, 0);
                break;
            }
            case 3:
                pDemo->PlayEndTelop(m_nTelop);
                m_nUnk9E = 7;
                break;
            case 4:
                break;
            case 5:
                AddTimerRel(3, 10.0f, 0, 0);
                break;
            default:
                pDemo->TimerEvent(nId, nArg, ulArg);
                break;
        }
    }

    void CVictoryDemo::EventTimerReset(short nId, short nArg, unsigned long ulArg)
    {
    }

    void CVictoryDemo::EventFlags(unsigned long nFlags, unsigned long nNew)
    {
        if (nNew & 1) {
            EventTimer(5, 0, 0);
        }
    }

    void CVictoryDemo::_PlayResult(const char *pAnm, long nTeam)
    {
        const RESULT_MOT *pMot = &s_ResultMot;
        if (pAnm == 0) {
            return;
        }
        m_pNodeData->SetResAnm(CEvNodeData::_GetResAnm(m_resFile, pAnm));
        for (long i = 0; i < m_nChrNum; i++) {
            CEventChrCore *pChr = static_cast<CEventChrCore *>(m_ppChr[i]);
            if (pChr == 0 || !(m_nChrMask & (1 << i)) || nTeam != pChr->GetTeamNo()) {
                continue;
            }
            long nNode = _SetChrNode(i, i + 1);
            if (nNode == -1 || !m_ppChr[i]->SetNode(nNode, 0.0f)) {
                continue;
            }

            long nResult = CGame::GetTeamResult(nTeam);
            if (CTournament::IsExtraMatchGame()) {
                switch (nTeam) {
                    case 1: {
                        int bClear = CTournament::IsMissionClear();
                        nResult = 1;
                        if (bClear) {
                            nResult = 2;
                        }
                        break;
                    }
                    case 0: {
                        int bClear = CTournament::IsMissionClear();
                        nResult = 2;
                        if (bClear) {
                            nResult = 1;
                        }
                        break;
                    }
                }
            }
            switch (nResult) {
                case 1:
                    m_ppChr[i]->AnimPlay(pMot->aWin, 0, 0.0f, -1);
                    pChr->PlayVoice(m_pData->pData[2][0]);
                    pChr->PlaySe(m_pData->pData[3][0]);
                    break;
                case 2:
                    m_ppChr[i]->AnimPlay(pMot->aLose, 0, 0.0f, -1);
                    pChr->PlayVoice(m_pData->pData[2][1]);
                    pChr->PlaySe(m_pData->pData[3][1]);
                    break;
                case 3:
                    m_ppChr[i]->AnimPlay(pMot->aDraw, 0, 0.0f, -1);
                    pChr->PlayVoice(m_pData->pData[2][2]);
                    pChr->PlaySe(m_pData->pData[3][2]);
                    break;
            }

            if (m_nTeam == nTeam) {
                if (i & 0xE) {
                    pChr->SetVoiceVolume(0.3f, 0.05f);
                    pChr->SetSeVolume(0.3f);
                } else {
                    pChr->SetVoiceVolume(1.0f, 0.05f);
                    pChr->SetSeVolume(1.0f);
                }
            } else {
                pChr->SetVoiceVolume(0.0f, 0.05f);
                pChr->SetSeVolume(0.0f);
            }
        }
    }

    CVictoryDemoParty::CVictoryDemoParty(long nTeam, long nArg, bcs::CTask *pParent, const VICTORY_DATA *pData)
        : CEventScene(8, pParent, nArg), m_pData(pData)
    {
        m_nChrNum = CChrMan::GetChrCoreNum();
        m_pCalc = new CSceneCalc(this);
        bcs::CRenderMan::EnableDepthOfFieldEffect(0);
        for (int i = 0; i < 8; i++) {
            m_afVolume[i] = 0.0f;
        }
        for (int i = 0; i < 4; i++) {
            m_abNoTeam[i] = 1;
        }
        // The teams the game type has.
        switch (CGame::g_GameWork.m_nUnk06) {
            case 8:
            case 10:
            case 14:
            case 19:
            case 22:
            case 23:
            case 24:
            case 25:
                m_abNoTeam[3] = 0;
            case 7:
            case 9:
            case 15:
            case 16:
            case 17:
            case 20:
            case 21:
                m_abNoTeam[2] = 0;
            case 1:
            case 3:
            case 5:
            case 6:
            case 18:
                m_abNoTeam[1] = 0;
            case 0:
            case 2:
            case 4:
                m_abNoTeam[0] = 0;
                break;
        }
        m_nTeam = nTeam;
        m_nTelop = -1;
        m_nResult = -1;
        m_nStep = 1;

        long nLose = 0;
        long nWin = 0;
        for (long i = 0; i < 4; i++) {
            if (m_abNoTeam[i]) {
                continue;
            }
            switch (CGame::GetTeamResult(i)) {
                case 1:
                    nWin++;
                    break;
                case 2:
                    nLose++;
                    break;
            }
        }

        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (CGame::g_GameWork.GetGameMode() == 1) {
            // Mixed sports: the players play in pairs, team 0 decides.
            m_nStep = 2;
            if (CGame::GetTeamResult(0) == 1) {
                m_nResult = 0;
            } else if (CGame::GetTeamResult(0) == 2) {
                m_nResult = 1;
            } else {
                m_nResult = 2;
            }
            if (CTournament::IsExtraMatchGame() && pDemo != 0) {
                m_nResult = !CTournament::IsMissionClear();
                m_nTelop = pDemo->GetTelopId(s_anExtraTelop[m_nResult]);
            } else {
                m_nTelop = s_anMixTelop[m_nResult];
            }
        } else {
            if (nWin > 0) {
                m_nResult = 0;
            } else if (nLose > 0) {
                m_nResult = 1;
            } else {
                m_nResult = 2;
            }
            if (CTournament::IsExtraMatchGame() && pDemo != 0) {
                m_nResult = !CTournament::IsMissionClear();
                m_nTelop = pDemo->GetTelopId(s_anExtraTelop[m_nResult]);
            } else {
                m_nTelop = m_pData->pData[1][m_nResult];
            }
        }
    }

    CVictoryDemoParty::~CVictoryDemoParty()
    {
        EvDemo::s_bNewRecord = 0;
        EvDemo::s_bPerfect = 0;
        CEventDemoBase *pDemo = CEventDemoBase::GetPtr();
        if (pDemo != 0) {
            pDemo->RemoveTelopAll();
            pDemo->SetCockpitState(0, 1);
        }
    }

    nw4r::g3d::ResFile CVictoryDemoParty::GetRes()
    {
        return CLoadData::GetRes(0x60);
    }

    // As CVictoryDemo::EventFirst, with ten timers and the party judge cut.
    int CVictoryDemoParty::EventFirst()
    {
        CEventDemoBase::GetInstance()->SetCockpitState(0, 2);
        CChrMan::SetChrMarkerDisp(0);
        CBallMan::AllReset();
        CBallMan::AllSleep();
        CreateTimer(10);

        long num = m_pCamera->GetSceneNum();
        if (num > 0) {
            if (m_pCut != 0) {
                delete[] m_pCut;
            }
            m_pCut = 0;
            m_pCut = new CUT_WORK[num];
            const char *pNoData = "NoData";
            for (long i = 0; i < num; i++) {
                CUT_WORK &rCut = m_pCut[i];
                rCut.pName = pNoData;
                rCut.nUnk04 = -1;
                rCut.nUnk06 = -1;
                rCut.fUnk1C = 0.0f;
                rCut.fUnk20 = 1.0f;
                rCut.nUnk08 = 0x7F;
                rCut.nUnk0A = 0;
                rCut.fUnk10 = 0.0f;
                rCut.fUnk18 = 0.0f;
                rCut.fUnk14 = 0.0f;
                rCut.nUnk0C = -1;
            }

            if (m_nResult >= 0) {
                AddCut("sub_game_judge", 4, 0, 0.0f);
                m_nResultCut = m_nCutNum;
                AddCut("sub_game_judge", 4, 0, 0.0f);
            }

            if (m_nCutNum > 0) {
                CUT_WORK &rFirst = m_pCut[0];
                rFirst.nUnk08 = 0x7E;
                rFirst.nUnk0A = 2;

                for (long i = 0; i < m_nChrNum; i++) {
                    long nColor = CGame::GetCharColor(i);
                    long nType = CGame::GetCharType(i);
                    if (nType != -1) {
                        m_ppChr[i] = new CEventChrCore(nType, i, m_pData->pData[0]);
                        if (m_ppChr[i]->IsObj()) {
                            m_ppChr[i]->Create();
                            m_ppChr[i]->SetDisp(0);
                            m_ppChr[i]->SetFloorMove(1);
                            m_ppChr[i]->SetEnableFlag18(0);
                            if (nColor < 4) {
                                m_nChrMask |= 1 << i;
                            }
                        } else {
                            if (m_ppChr[i] != 0) {
                                delete m_ppChr[i];
                            }
                            m_ppChr[i] = 0;
                        }
                    }
                }
            }
        }
        return 1;
    }

    void CVictoryDemoParty::EventKill()
    {
        unsigned long nFlags = _GetDemoFlags();
        if (m_nDemoFlags != nFlags) {
            EventFlags(nFlags, nFlags & (nFlags ^ m_nDemoFlags));
        }
        m_nDemoFlags = nFlags;
        _UpdateTimer(GetFrameRateF());
    }

    // Follows the team, moves the scene to the stage and puts every player
    // on its node path of the first cut.
    void CVictoryDemoParty::EventStart()
    {
        CUT_WORK *pCut = m_pCut;
        switch (m_nTeam) {
            case 0:
                m_pCalc->SetFloorChr(0);
                break;
            case 1:
                m_pCalc->SetFloorChr(1);
                break;
        }
        m_pCalc->SetCalc(1);

        switch (CGame::g_GameWork.m_nStage) {
            case 0x28:
            case 0x2A:
            case 0x46:
            case 0x55:
                AddOffset(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                break;
            case 0x48:
                AddOffset(bcs::FVEC3(0.0f, 0.0f, -3.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                break;
            case 0x47:
                AddOffset(bcs::FVEC3(0.0f, -0.05f, -3.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                break;
            case 0x29:
                AddOffset(bcs::FVEC3(0.0f, -0.05f, -1.5f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                break;
            case 0x57:
                AddOffset(bcs::FVEC3(0.0f, 0.0f, 4.8f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                break;
            case 0x37:
                AddOffset(bcs::FVEC3(0.0f, 0.0f, -2.0f), bcs::FVEC3(0.0f, 3.1415927f, 0.0f), 1);
                break;
            case 0x38:
            case 0x39:
                AddOffset(bcs::FVEC3(0.0f, 0.0f, 2.0f), bcs::FVEC3(0.0f, 0.0f, 0.0f), 1);
                break;
            default:
                AddOffset(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 0.0f, 0.0f), 1);
                break;
        }

        _SetDemoTask(this);
        SetDispChr(0);
        SetFloorMove(CGame::g_GameWork.m_nStage, -1, 0);

        const char *pAnm = pCut->pName;
        long nStep = m_nStep;
        if (pAnm != 0) {
            m_pNodeData->SetResAnm(CEvNodeData::_GetResAnm(m_resFile, pAnm));
            for (long i = 0; i < m_nChrNum; i += nStep) {
                if (m_ppChr[i] != 0 && (m_nChrMask & (1 << i))) {
                    _SetPlayerNode(i, nStep);
                    m_ppChr[i]->AnimPlay(0L, 2, 0.0f, 0.0f, 1);
                }
            }
        }
        AddTimer(0, 10.0f, 0, 0);
    }

    long CVictoryDemoParty::EventPlay(CUT_WORK &rCut)
    {
        if (IsFadeOutOnly(rCut.nUnk08)) {
            return StartFadeOut(rCut, rCut.nUnk08);
        }
        if (m_nResultCut == m_nUnk9D) {
            AddTimerRel(2, 120.0f, 0, 0);
            AddTimerRel(8, 200.0f, 0, 0);
            if (EvDemo::s_bNewRecord) {
                AddTimerRel(4, 170.0f, 0, 0);
            }
            if (EvDemo::s_bPerfect) {
                AddTimerRel(6, 170.0f, 0, 0);
            }
            return 6;
        }
        return StartFadeOut(rCut, rCut.nUnk08);
    }

    // As CVictoryDemo::EventTimer; timers 4 to 7 show and close the new
    // record (telop 5) and perfect (telop 6) telops, 9 (from EventFlags)
    // closes them at once.
    void CVictoryDemoParty::EventTimer(short nId, short nArg, unsigned long ulArg)
    {
        CEventDemoBase *pDemo = CEventDemoBase::GetInstance();
        switch (nId) {
            case 0:
                m_pCamera->Resume();
                break;
            case 1:
                m_nUnk9E = 7;
                break;
            case 2: {
                CUT_WORK &rCut = m_pCut[m_nUnk9D];
                _UpdateCutStep();
                _PlayResult(rCut.pName, m_nStep);
                pDemo->CreateTelop(m_nTelop, 0);
                break;
            }
            case 3:
                pDemo->PlayEndTelop(m_nTelop);
                m_nUnk9E = 7;
                break;
            case 4:
                pDemo->CreateTelop(pDemo->GetTelopId(5), 0);
                break;
            case 5:
                pDemo->PlayEndTelop(pDemo->GetTelopId(5));
                break;
            case 6:
                pDemo->CreateTelop(pDemo->GetTelopId(6), 0);
                break;
            case 7:
                pDemo->PlayEndTelop(pDemo->GetTelopId(6));
                break;
            case 8:
                break;
            case 9:
                AddTimerRel(3, 10.0f, 0, 0);
                if (EvDemo::s_bNewRecord) {
                    AddTimerRel(5, 0.0f, 0, 0);
                }
                if (EvDemo::s_bPerfect) {
                    AddTimerRel(7, 0.0f, 0, 0);
                }
                break;
            default:
                pDemo->TimerEvent(nId, nArg, ulArg);
                break;
        }
    }

    void CVictoryDemoParty::EventTimerReset(short nId, short nArg, unsigned long ulArg)
    {
    }

    void CVictoryDemoParty::EventFlags(unsigned long nFlags, unsigned long nNew)
    {
        if (nNew & 1) {
            EventTimer(9, 0, 0);
        }
    }

    long CVictoryDemoParty::_SetPlayerNode(long nChr, long nStep)
    {
        long nResult = 0;
        CEventChrCore *pChr = static_cast<CEventChrCore *>(m_ppChr[nChr]);
        if (pChr != 0 && (m_nChrMask & (1 << nChr))) {
            long nNo = nChr / nStep + 1;
            char aName[64];
            if (CGame::g_GameWork.GetGameMode() == 1) {
                nResult = CGame::GetTeamResult(0);
                sprintf(aName, "player%02dW", nNo);
                m_afVolume[nChr] = 1.0f;
            } else {
                nResult = CGame::GetTeamResult(pChr->GetTeamNo());
                switch (m_nResult) {
                    case 0:
                        if (nResult != 1) {
                            sprintf(aName, "player%02d", nNo);
                            m_afVolume[nChr] = 0.0f;
                        } else {
                            sprintf(aName, "player%02dW", nNo);
                            m_afVolume[nChr] = 1.0f;
                        }
                        break;
                    case 1:
                        if (nResult != 2) {
                            sprintf(aName, "player%02d", nNo);
                            m_afVolume[nChr] = 0.0f;
                        } else {
                            sprintf(aName, "player%02dW", nNo);
                            m_afVolume[nChr] = 1.0f;
                        }
                        break;
                    default:
                        sprintf(aName, "player%02dW", nNo);
                        m_afVolume[nChr] = 1.0f;
                        break;
                }
            }

            long nId = m_pNodeData->GetAnmId(aName);
            if (nId == -1) {
                return 0;
            }
            bcs::FVEC3 vPos;
            bcs::FVEC3 vRot;
            bcs::FVEC3 vScale;
            m_pNodeData->GetResultEx(vPos, vRot, vScale, 0.0f, nId, m_nNodeFlip);
            bcs::BCS_Vec_RotY(vPos, vPos, m_fNodeRotY);
            bcs::FVEC3Add(&vPos, &vPos, &m_vNodeOffset);
            bcs::FVEC3Add(&vRot, &vRot, &m_vNodeOffsetRot);
            m_ppChr[nChr]->SetPosition(vPos);
            m_ppChr[nChr]->SetRotation(vRot);
            m_ppChr[nChr]->SetScale(vScale);
            m_ppChr[nChr]->SetDisp(1);
        }
        return nResult;
    }

    void CVictoryDemoParty::_PlayResult(const char *pAnm, long nStep)
    {
        const RESULT_MOT *pMot = &s_ResultMot;
        if (pAnm == 0) {
            return;
        }
        m_pNodeData->SetResAnm(CEvNodeData::_GetResAnm(m_resFile, pAnm));
        int bFirst = 1;
        for (long i = 0; i < m_nChrNum; i += nStep) {
            CEventChrCore *pChr = static_cast<CEventChrCore *>(m_ppChr[i]);
            if (pChr == 0 || !(m_nChrMask & (1 << i))) {
                continue;
            }
            switch (_SetPlayerNode(i, nStep)) {
                case 1:
                    m_ppChr[i]->AnimPlay(pMot->aWin, 0, 0.0f, -1);
                    pChr->PlayVoice(m_pData->pData[2][0]);
                    pChr->PlaySe(m_pData->pData[3][0]);
                    break;
                case 2:
                    m_ppChr[i]->AnimPlay(pMot->aLose, 0, 0.0f, -1);
                    pChr->PlayVoice(m_pData->pData[2][1]);
                    pChr->PlaySe(m_pData->pData[3][1]);
                    break;
                case 3:
                    m_ppChr[i]->AnimPlay(pMot->aDraw, 0, 0.0f, -1);
                    pChr->PlayVoice(m_pData->pData[2][2]);
                    pChr->PlaySe(m_pData->pData[3][2]);
                    break;
            }
            // Only the first player at full volume is heard fully.
            if (m_afVolume[i] >= 1.0f) {
                if (!bFirst) {
                    m_afVolume[i] = 0.5f;
                }
                bFirst = 0;
            }
            pChr->SetVoiceVolume(m_afVolume[i], 0.05f);
            pChr->SetSeVolume(m_afVolume[i]);
        }
    }

    namespace EvDemo {

        // Starts the victory demo of the game being played.
        void CreateVictoryDemo(bcs::CTask *pParent, const VICTORY_DATA *pData)
        {
            switch (CGame::g_GameWork.GetGameType()) {
                case 0:
                    bcs::CWorldEnv::GetViewSplitNum();
                    if (CGame::IsLocalVsMode()) {
                        // The local player's team when it won, otherwise a
                        // random winner, otherwise a random team with
                        // result 3 (nNum counts the candidates).
                        long nNum = 0;
                        for (long nTeam = 0; nTeam < CGame::g_GameWork.m_nTeamNum; nTeam++) {
                            if (CGame::GetTeamResult(nTeam) == 1) {
                                if (nTeam == CGame::g_GameWork.m_nUnk00) {
                                    new CVictoryDemo(nTeam, 0, pParent, pData);
                                    return;
                                }
                                nNum++;
                            }
                        }
                        if (nNum > 0) {
                            long nPick = bcs::BCS_RandMax(nNum - 1);
                            nNum = 0;
                            for (long nTeam = 0; nTeam < CGame::g_GameWork.m_nTeamNum; nTeam++) {
                                if (CGame::GetTeamResult(nTeam) == 1) {
                                    if (nNum == nPick) {
                                        new CVictoryDemo(nTeam, 0, pParent, pData);
                                        return;
                                    }
                                    nNum++;
                                }
                            }
                        } else {
                            nNum = 0;
                            for (long nTeam = 0; nTeam < CGame::g_GameWork.m_nTeamNum; nTeam++) {
                                if (CGame::GetTeamResult(nTeam) == 3) {
                                    nNum++;
                                }
                            }
                            if (nNum > 0) {
                                long nPick = bcs::BCS_RandMax(nNum - 1);
                                nNum = 0;
                                for (long nTeam = 0; nTeam < CGame::g_GameWork.m_nTeamNum; nTeam++) {
                                    if (CGame::GetTeamResult(nTeam) == 3) {
                                        if (nNum == nPick) {
                                            new CVictoryDemo(nTeam, 0, pParent, pData);
                                            return;
                                        }
                                        nNum++;
                                    }
                                }
                            }
                        }
                    } else {
                        new CVictoryDemo(CGame::g_GameWork.m_nUnk00, 0, pParent, pData);
                    }
                    break;

                case 2:
                    if ((CGame::g_GameWork.GetGameMode() == 0 || CGame::g_GameWork.GetGameMode() == 3)
                        && CTournament::IsTournamentGame()) {
                        new CVictoryDemo(CGame::g_GameWork.m_nUnk00, 0, pParent, pData);
                    } else {
                        new CVictoryDemoParty(CGame::g_GameWork.m_nUnk00, 0, pParent, pData);
                    }
                    break;
            }
        }

        void SetNewRecord(int bNewRecord)
        {
            s_bNewRecord = bNewRecord;
        }

        void SetPerfect(int bPerfect)
        {
            s_bPerfect = bPerfect;
        }
    }
}
