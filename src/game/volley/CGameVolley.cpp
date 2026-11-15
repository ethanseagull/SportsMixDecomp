// msw::CGameVolley, the game task of the volleyball match (VolleyF), with
// the two helper tasks of the file: the camera of the arts (special shot)
// event and the except demo shown before a set or match point.
//
// Written from the drafts of each function, after the state flow of BossF's
// game/boss/GameBoss.cpp and VolleyF's game/volley/CGameVolleyVsBoss.cpp.
#include "game/msw/CGameVolley.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CCapture.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/New.hpp"
#include "bcs/dwc/CDwc.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManVolley.hpp"
#include "game/msw/CCameraVolley.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManVolley.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitVolley.hpp"
#include "game/msw/CCollManVolley.hpp"
#include "game/msw/CComManVolleyGame.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCtrlVolley.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CEventDemoVolley.hpp"
#include "game/msw/CFootmarkMan.hpp"
#include "game/msw/CGameBGM.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameExDemoBase.hpp"
#include "game/msw/CGameExDemoMaskObj.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CGimmickManVolley.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CShadowTexMan.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageManVolley.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/TournamentVolley.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"

#include <math.h>

namespace msw {
    namespace {

        // The states of CGame's state queue (the places of the
        // Ctor/Loop/Dtor triples in CGame's vtable plus two; names after
        // those functions).
        enum {
            STATE_GAME_SETUP = 3,
            STATE_WAIT_START = 4,
            STATE_MAIN_LOOP = 5,
            STATE_GOAL_EVENT = 6,
            STATE_TURN_RESULT = 7,
            STATE_TIME_UP = 8, // here: the set is over
            STATE_CHG_COURT = 9,
            STATE_TURN_OVER = 10,
            STATE_CHG_OFFENSE = 11,
            STATE_GAME_SET = 12,
            STATE_GAME_RESULT = 13,
            STATE_RESULT_MENU = 14,
            STATE_END_MENU = 15,
            STATE_EVENT_DEMO = 19,
            STATE_EXCEPT_DEMO = 20,
            STATE_ONLINE_END = 26 // (name guessed) queued in an online game
        };

        // Telops (CGame::TelopCreate_ numbers; names guessed).
        enum {
            TELOP_SCORE_COUNT = 7,
            TELOP_SET_COUNT = 8,
            TELOP_DEUCE = 0xB,
            TELOP_SET_POINT = 0xC,
            TELOP_MATCH_POINT = 0xD,
            TELOP_GAME_SET = 0x10,
            TELOP_FINAL_POINT = 0x1027
        };

        // The work memory of CGame's event demo state, as the game setup
        // fills it in (as in BossF's GameBoss.cpp; only the words written
        // here are known).
        struct EVENT_DEMO_WORK {
            unsigned long nFlag; // 0x00
            long nUnk04;
            long nUnk08;         // 0x08
            long nUnk0C;         // 0x0C
            long nUnk10;         // 0x10
            long nUnk14;
            long nUnk18;         // 0x18
            float fUnk1C;        // 0x1C
        };

        // For each screen split (two tables: the second for the online
        // game), the view place of each pad (names guessed).
        const unsigned char s_aanShakeTelopPlace[2][4][4] = {
            {
                {0, 0, 0, 0},
                {1, 2, 1, 2},
                {3, 4, 3, 4},
                {1, 2, 1, 2},
            },
            {
                {0, 0, 0, 0},
                {2, 1, 2, 1},
                {4, 3, 4, 3},
                {2, 1, 2, 1},
            },
        };

        // The "shake the remote" telops, one per view place.
        const long s_anRemoconShakeTelop[5] = {0x1072, 0x1073, 0x1074, 0x1075, 0x1076};

        // Where the characters start, by team and place: x, the yaw in
        // radians, z. Row 0 is the serving team (its server at the back),
        // row 1 the receiving one. For the normal start and for the
        // result.
        bcs::FVEC3 s_aaChrPosNormal[2][3] = {
            {
                bcs::FVEC3(0.0f, 3.1415927f, 11.5f),
                bcs::FVEC3(-2.0f, 3.1415927f, 1.5f),
                bcs::FVEC3(2.0f, 3.1415927f, 1.5f),
            },
            {
                bcs::FVEC3(2.0f, 3.1415927f, 6.0f),
                bcs::FVEC3(-2.0f, 3.1415927f, 1.5f),
                bcs::FVEC3(2.0f, 3.1415927f, 1.5f),
            },
        };
        bcs::FVEC3 s_aChrPosResult[3] = {
            bcs::FVEC3(0.0f, 0.0f, 3.0f),
            bcs::FVEC3(2.0f, 0.0f, 2.0f),
            bcs::FVEC3(-2.0f, 0.0f, 2.0f),
        };

        // (guessed helpers) An angle brought into [0, 2 pi), and clamps.
        inline float WrapRad(float rad)
        {
            while (rad > 6.2831855f) {
                rad -= 6.2831855f;
            }
            while (rad < 0.0f) {
                rad += 6.2831855f;
            }
            return rad;
        }

        inline float Clamp(float fValue, float fMin, float fMax)
        {
            return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
        }

        inline long Clamp(long nValue, long nMin, long nMax)
        {
            return nValue > nMax ? nMax : (nValue < nMin ? nMin : nValue);
        }

        inline float Max(float fValue, float fMin)
        {
            return fMin < fValue ? fValue : fMin;
        }

        inline float Min(float fValue, float fMax)
        {
            return fMax > fValue ? fValue : fMax;
        }

        inline float DegToRad(float fDeg)
        {
            return 0.017453292f * fDeg;
        }

        // (guessed getter) The number of sets of the match, read signed.
        inline long GetSetNum()
        {
            return CGame::g_GameWork.m_Unk0B;
        }

        // (guessed helper) n, or 0 for a negative n.
        inline long NonNegative(long n)
        {
            return n < 0 ? 0 : n;
        }

        inline float Lerp(float a, float b, float t)
        {
            return t * (b - a) + a;
        }

        // The length from the character to the camera's target point in
        // front of it: longer for two characters (ids 7 and 10).
        inline float GetChrFrontLen(const CChrCore *pChr)
        {
            float fLen = 0.3f;
            if (pChr->GetChrTypeID() == 7 || pChr->GetChrTypeID() == 10) {
                fLen = 0.8f;
            }
            return fLen;
        }

        // The demo before a set or match point: both team leaders come to
        // the net, each view's camera turns to its leader, then the
        // screen wipes back. 0x54 bytes. (name guessed)
        class CcVolleyExceptDemo : public CGameExDemoBase {
            public:
                CcVolleyExceptDemo(bcs::CTask *pParent) : CGameExDemoBase(pParent) { m_fUnk3C = 4.0f; }
                virtual ~CcVolleyExceptDemo();
                virtual void MainProc();
        };

        // The camera of the arts event: turns around the ball while it
        // rises. 0x138 bytes. (name guessed)
        class CcVolleyArtsCamera : public bcs::CCamera {
            public:
                CcVolleyArtsCamera(long nCourt, float fFrame);
                virtual ~CcVolleyArtsCamera();
                virtual void MainCamera();

            private:
                bcs::FVEC3 m_BallPos;   // 0xFC, the ball's place at the start
                bcs::FVEC3 m_At;        // 0x108
                float m_fRot;           // 0x114, the direction from the ball
                float m_fRotAdd;        // 0x118, the turn over the event
                float m_fRoll;          // 0x11C
                float m_fRollAdd;       // 0x120
                float m_fHeight0;       // 0x124
                float m_fHeight1;       // 0x128
                float m_fFrame;         // 0x12C
                float m_fFrameMax;      // 0x130
                unsigned long m_hBall;  // 0x134, the ball's task id
        };

        CcVolleyExceptDemo::~CcVolleyExceptDemo()
        {
        }

        void CcVolleyExceptDemo::MainProc()
        {
            // (the step word is read signed here)
            switch (static_cast<long>(m_Unk38)) {
                case 0:
                    m_fUnk3C -= GetFrameRateF();
                    if (m_fUnk3C <= 0.0f) {
                        m_fUnk3C = 12.0f;
                        CChrMan::AllSetState(4, -1, 0);
                        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                        CChrMan::SetChrMarkerDisp(0);
                        long nNum = CChrMan::GetChrCoreNum();
                        for (long i = 0; i < nNum; i++) {
                            CChrCore *pChr = CChrMan::GetChrCore(i);
                            if (pChr && !pChr->IsDisable()) {
                                pChr->OnDispFlag(4);
                            }
                        }
                        for (long i = 0; i < CAMERA_NUM; i++) {
                            bcs::FVEC3 at(0.0f, 0.0f, 0.0f);
                            bcs::FVEC3 pos(-2.0f, 1.0f, 3.0f);
                            float fCamDist = 4.2f;
                            CChrCore *pChr = CChrMan::GetTeamLeader(i);
                            if (pChr && pChr->IsEnable()) {
                                float fRot = -0.87266463f;
                                pChr->OffDispFlag(4);
                                fRot = WrapRad(fRot);
                                if (i != 0) {
                                    fRot = WrapRad(-fRot);
                                }
                                pChr->GetTransNodePos(at);
                                at.y = 0.5f;
                                if (pChr->GetFlag(1)) {
                                    float fLen = GetChrFrontLen(pChr);
                                    bcs::FVEC3 front;
                                    bcs::BCS_VecZ_RotY(front, fLen, pChr->GetRotY());
                                    at.x += front.x;
                                    at.z += front.z;
                                    pChr->AnimPlay(3, 0x42, 1, 0.0f, 0.0f);
                                } else {
                                    bcs::FVEC3 net = CStageMan::GetInstance()->GetNetPos();
                                    bcs::FVEC3 chrPos = pChr->GetPos();
                                    float fDist = fabsf(net.z - chrPos.z);
                                    if (fDist <= 3.0f) {
                                        bcs::FVEC3 move(0.0f, 0.0f, 4.0f - fDist);
                                        if (net.z > chrPos.z) {
                                            move.z *= -1.0f;
                                        }
                                        pChr->AddPosXZ(move);
                                        at.x += move.x;
                                        at.z += move.z;
                                    }
                                    float fLen = GetChrFrontLen(pChr);
                                    bcs::FVEC3 front;
                                    bcs::BCS_VecZ_RotY(front, fLen, pChr->GetRotY());
                                    at.x += front.x;
                                    at.z += front.z;
                                    pChr->AnimPlay(3, 0x84, 1, 0.0f, 0.0f);
                                }
                                bcs::BCS_VecZ_RotY(pos, fCamDist, WrapRad(fRot + pChr->GetRotY()));
                                pos.y = 0.0f;
                                at.y = 1.0f;
                                bcs::FVEC3Add(&pos, &pos, &at);
                            }
                            m_apCamera[i] = new bcs::CCamera(i, 0x80, 0);
                            if (m_apCamera[i]) {
                                m_apCamera[i]->SetPos(pos);
                                m_apCamera[i]->SetAt(at);
                                m_apCamera[i]->SetRoll(0.0f);
                                m_apCamera[i]->SetActive(-1);
                            }
                        }
                        m_Unk38++;
                    }
                    break;
                case 1: {
                    m_fUnk3C -= GetFrameRateF();
                    float fRate = Clamp(m_fUnk3C / 12.0f, 0.0f, 1.0f);
                    if (m_pMask) {
                        m_pMask->SetPosRate(fRate, 1);
                    }
                    if (m_fUnk3C <= 0.0f) {
                        CGameSnd::Play(0xC06, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        float fPower = 0.03f;
                        for (long i = 0; i < CAMERA_NUM; i++) {
                            bcs::FVEC3 power(fPower, 0.0f, 0.0f);
                            if (i != 0) {
                                power.x = -fPower;
                            }
                            if (m_apCamera[i]) {
                                m_apCamera[i]->StartQuakeDirect(0x80, power, 36.0f, 0, 0);
                            }
                        }
                        m_fUnk3C = 12.0f;
                        m_Unk38++;
                    }
                    break;
                }
                case 2:
                    m_fUnk3C -= GetFrameRateF();
                    if (m_fUnk3C <= 0.0f) {
                        m_nStep = 1;
                        m_Unk38++;
                    }
                    break;
                case 3:
                    if (m_nStep > 1) {
                        bcs::CCapture *pCapture = bcs::CCapture::GetInstance(0);
                        if (pCapture) {
                            pCapture->OffFlag(1);
                        }
                        m_nStep++;
                        m_fUnk3C = 2.0f;
                        m_Unk38++;
                    }
                    break;
                case 4:
                    if (m_apCamera[0]) {
                        m_apCamera[0]->Delete();
                        m_apCamera[0] = 0;
                    }
                    if (m_apCamera[1]) {
                        m_apCamera[1]->Delete();
                        m_apCamera[1] = 0;
                    }
                    // fall through
                case 5:
                    m_fUnk3C -= GetFrameRateF();
                    if (m_fUnk3C <= 0.0f) {
                        m_fUnk3C = 12.0f;
                        m_Unk38++;
                    }
                    break;
                case 6: {
                    m_fUnk3C -= GetFrameRateF();
                    float fRate = Clamp(m_fUnk3C / 12.0f, 0.0f, 1.0f);
                    if (m_pMask) {
                        m_pMask->SetPosRate(1.0f - fRate, 0);
                    }
                    if (m_fUnk3C <= 0.0f) {
                        bcs::CCapture *pCapture = bcs::CCapture::GetInstance(0);
                        if (pCapture) {
                            pCapture->OnFlag(1);
                        }
                        m_Unk38++;
                    }
                    break;
                }
                default:
                    Delete();
                    break;
            }
        }
    }

    CGameVolley::CGameVolley(bcs::CTask *pParent) : CGame(pParent, GAME_MODE_VOLLEY)
    {
        m_nUnk21C = 0;
        m_hExDemo = 0;
        m_nExceptTelop = -1;
        m_pInstance = this;
        Initialize_();
    }

    CGameVolley::~CGameVolley()
    {
        Finalize_();
    }

    void CGameVolley::Initialize_()
    {
        InitWork_();
    }

    void CGameVolley::InitWork_()
    {
        m_nServeTeam = -1;
        m_nMatchPointTeam = -1;
        m_nScoredTeam = g_GameWork.GetUserTeam();
        for (long i = 0; i < 4; i++) {
            m_anServeOrder[i] = -1;
            m_anUnk16C[i] = 0;
            m_anDispScore[i] = 0;
        }
        m_nRunTeam = -1;
        m_nRunCount = 0;
        m_bScored = 0;
        m_nUnk1E8 = 0;
        m_bLineMsgDisp = 0;
        m_bPartMoveLock = 1;
        m_nUnk1F4 = -1;
        m_nPointTelop = -1;
        m_fScoreDispWait = 90.0f;
        m_bPartMove = 0;
        m_bDeuceNext = 0;
        m_bDeuce = 0;
        InitServeClock_();
    }

    void CGameVolley::InitServeClock_()
    {
        m_bServeClock = 0;
        if (g_GameWork.m_nGameType == 0) {
            m_bServeClock = g_GameWork.m_Unk151;
        }
        m_bServeClockDisp = 0;
        m_fServeClock = 900.0f;
        m_nServeClockCount = -1;
    }

    void CGameVolley::Finalize_()
    {
        CLytCtrl::Dispose();
        CLytCtrl::DetachResource();
        CLytCtrl::Close();
    }

    void CGameVolley::MainFunc()
    {
    }

    void CGameVolley::DispFunc()
    {
    }

    void CGameVolley::FinishFirstLoadData()
    {
    }

    void CGameVolley::GenerateInstance()
    {
        CShadowTexMan::CreateInstance(this);
        CFootmarkMan::CreateInstance(this);
        CCollManVolley::CreateInstance(this);
        CGimmickManVolley::CreateInstance(this);
        CChrManVolley::CreateInstance(this);
        CBallManVolley::CreateInstance(this);
        CEffectCtrlVolley::CreateInstance(this);
        CStageManVolley::CreateInstance(this, g_GameWork.m_nStage);
        CLytCtrl::CreateInstance(this);
        EvDemoVolley::CreateInstance(this);
        TournamentVolley::CreateInstance(this);
        CCockpitVolley::Create(this);
        if (g_GameWork.m_nGameType == 0) {
            CComManVolleyGame::CreateInstance();
        }
        menu::CSndEff::s_Instance.Initialize();
        menu::CVibEff::s_Instance.Initialize();
        CCameraVolley::Initialize();
        for (long i = 0; i < bcs::CWorldEnv::GetViewSplitNum(); i++) {
            m_apCamera[i] = CGameCamera::GetInstance(i);
        }
    }

    void CGameVolley::StartArtsEvent_(long)
    {
        CCockpit::RemoveObj(0x22);
        CCockpit::RemoveObj(0x23);
    }

    void CGameVolley::EndArtsEvent_(long)
    {
        if (m_pArtsEventTask) {
            m_pArtsEventTask->Delete();
            m_pArtsEventTask = 0;
        }
    }

    float CGameVolley::GetArtsEventFrameNum_()
    {
        return CGame::GetArtsEventFrameNum_();
    }

    // After CGame's part, a turn of the arts camera around the ball for
    // 90 frames.
    int CGameVolley::ArtsEventLoop_()
    {
        switch (m_nArtsEventStep) {
            case 0:
                if (CGame::ArtsEventLoop_()) {
                    ResetArtsEventPauseBit_(8);
                    m_fArtsEventFrame = 90.0f;
                    m_nArtsEventStep++;
                    long nCourt = 0;
                    if (g_nEventChrIdx >= 0) {
                        CChrCore *pChr = CChrMan::GetChrCore(g_nEventChrIdx);
                        if (pChr && pChr->IsEnable()) {
                            nCourt = pChr->GetTargetCourtIdx();
                        }
                    }
                    m_pArtsEventTask = new (0x100) CcVolleyArtsCamera(nCourt, m_fArtsEventFrame);
                }
                break;
            case 1:
                m_fArtsEventFrame -= GetFrameRateF();
                if (m_fArtsEventFrame <= 0.0f) {
                    return 1;
                }
                break;
            default:
                return 1;
        }
        return 0;
    }

    namespace {

        CcVolleyArtsCamera::CcVolleyArtsCamera(long nCourt, float fFrame) : CCamera(0, 0x80, 0)
        {
            m_fFrame = 0.0f;
            m_fFrameMax = fFrame;
            m_hBall = 0;
            m_fFrameMax = Max(m_fFrameMax, 1.0f);
            if (CChrMan::IsActive() && CBallMan::IsActive() && CGame::g_nEventChrIdx >= 0) {
                CChrCore *pChr = CChrMan::GetChrCore(CGame::g_nEventChrIdx);
                if (pChr && pChr->IsEnable()) {
                    m_hBall = pChr->GetBallHandle();
                }
            }
            if (m_hBall) {
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_hBall));
                if (pBall) {
                    m_BallPos = pBall->GetPos();
                    m_At = m_BallPos;
                    float fRot = DegToRad(bcs::BCS_RandRange(-40.0f, 40.0f));
                    if (nCourt == 0) {
                        fRot += 3.1415927f;
                    }
                    m_fRot = WrapRad(fRot);
                    m_fRotAdd = bcs::BCS_RandMax(1L) ? 0.6981317f : -0.6981317f;
                    m_fHeight0 = 3.0f;
                    m_fHeight1 = 0.0f;
                    m_fRoll = 0.0f;
                    float fRoll = 0.17453292f;
                    m_fRollAdd = fRoll;
                    if (m_fRotAdd > 0.0f) {
                        m_fRollAdd = -fRoll;
                    }
                    bcs::FVEC3 pos;
                    bcs::BCS_VecZ_RotY(pos, 5.0f, m_fRot);
                    bcs::FVEC3Add(&pos, &pos, &m_BallPos);
                    pos.y += m_fHeight0;
                    pos.y = Max(pos.y, 2.0f);
                    m_Data.pos = pos;
                    m_Data.at = m_BallPos;
                    m_Data.roll = m_fRoll;
                }
            }
            if (m_hBall) {
                SetActive(-1);
            }
        }

        CcVolleyArtsCamera::~CcVolleyArtsCamera()
        {
        }

        void CcVolleyArtsCamera::MainCamera()
        {
            m_fFrame += GetFrameRateF();
            m_fFrame = Clamp(m_fFrame, 0.0f, m_fFrameMax);
            float fRate = Clamp(m_fFrame * (1.0f / m_fFrameMax), 0.0f, 1.0f);
            float t = Clamp((float)pow(fRate, 1.5), 0.0f, 1.0f);

            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, 5.0f, WrapRad(m_fRot + Lerp(0.0f, m_fRotAdd, t)));
            bcs::FVEC3Add(&pos, &pos, &m_BallPos);
            pos.y += Lerp(m_fHeight0, m_fHeight1, t);
            pos.y = Max(pos.y, 2.0f);

            if (m_hBall) {
                CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_hBall));
                if (pBall) {
                    bcs::FVEC3 ballPos = pBall->GetPos();
                    bcs::FVEC3 at;
                    bcs::BCS_Vec_Lerp(at, m_At, ballPos, 0.125f);
                    at.y = Min(at.y, 6.0f);
                    m_At = at;
                    m_Data.pos = pos;
                    m_Data.at = at;
                    m_Data.roll = WrapRad(m_fRoll + Lerp(0.0f, m_fRollAdd, t));
                }
            }
        }
    }

    void CGameVolley::SetCockpitTeamScoreNum_()
    {
        CCockpitVolley::SetTeamScore();
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            CCockpit::SetTeamScoreColor(i, 0);
        }
    }

    // nColor -1 hides the serve clock, -2 hides it after the count has
    // gone above 5; otherwise shows count nCount in colour nColor.
    void CGameVolley::SetCockpitServeClockCount_(long nColor, long nCount)
    {
        if (nColor == -1) {
            if (m_bServeClockDisp) {
                CCockpitVolley::SetShotClockCount(-1, m_nServeClockCount);
                m_bServeClockDisp = 0;
                m_nServeClockCount = -1;
            }
        } else if (nColor == -2) {
            if (m_bServeClockDisp) {
                if (m_nServeClockCount > 5) {
                    CCockpitVolley::SetShotClockCount(-2, m_nServeClockCount);
                }
                m_bServeClockDisp = 0;
                m_nServeClockCount = -1;
            }
        } else if (!m_bServeClockDisp || nCount != m_nServeClockCount) {
            m_bServeClockDisp = 1;
            m_nServeClockCount = nCount;
            CCockpitVolley::SetShotClockCount(nColor, nCount);
            CGameSnd::Play(0xBFB, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CGameVolley::SetDispRadarIcon_(int bDisp)
    {
        CCockpitVolley::SetRadarIconDisp(bDisp);
    }

    void CGameVolley::SetScoreDemoTeamScore_(long nTeam, long nScore)
    {
        CCockpitVolley::SetTeamScore(nTeam, nScore, 0);
    }

    void CGameVolley::SetScoreDemoTeamCoin_(long nTeam, long nCoin)
    {
        CCockpit::SetTeamCoin(nTeam, nCoin);
    }

    // Part 0 also plays the serve animation of the score board.
    void CGameVolley::SetCockpitPartMove_(long nPart, long nMove)
    {
        if (m_bPartMoveLock) {
            return;
        }
        if (nPart == 0) {
            switch (nMove) {
                case 0:
                    CCockpitVolley::SetServeAnim(0);
                    break;
                case 1:
                    CCockpitVolley::SetServeAnim(1);
                    break;
                case 2:
                    CCockpitVolley::SetServeAnim(2);
                    break;
                case 3:
                    CCockpitVolley::SetServeAnim(3);
                    break;
            }
            CCockpitVolley::SetPartMove(2, nMove, 0);
        } else {
            CCockpitVolley::SetPartMove(nPart, nMove, 0);
        }
    }

    void CGameVolley::SetCockpitDisp_(int bDisp)
    {
        CCockpitVolley::SetDisp(bDisp);
    }

    void CGameVolley::SetCockpitCoinGetTelopMove_(long nTeam, long nState)
    {
        CCockpitVolley::SetCoinTelopState(nTeam, nState);
    }

    void CGameVolley::SetCockpitCoinDisp_(int bDisp)
    {
        CCockpitVolley::SetCoinDispEnable(bDisp);
    }

    void CGameVolley::SetCockpitTimeDisp_(int)
    {
    }

    void CGameVolley::SetCockpitScoreDisp_(int bDisp)
    {
        CCockpitVolley::SetScoreDispEnable(bDisp);
    }

    void CGameVolley::SetCockpitSetCountDisp_(int bDisp)
    {
        CCockpitVolley::SetSetCountDispEnable(bDisp);
    }

    // The team one point from winning the set, if it is the user's.
    void CGameVolley::SetDispMatchPoint_()
    {
        long nTeam = -1;
        long nPeriod = g_GameWork.m_nPeriod;
        long nWinPoint = g_GameWork.m_nUnk44;
        long nScore0 = GetTeamPeriodScore(0, nPeriod);
        long nScore1 = GetTeamPeriodScore(1, nPeriod);
        if ((nScore0 >= nWinPoint - 1 || nScore1 >= nWinPoint - 1) && nScore0 != nScore1) {
            nTeam = nScore0 <= nScore1;
        }

        int abMatchPoint[2];
        if (GetSetNum() <= 1) {
            abMatchPoint[0] = 1;
            abMatchPoint[1] = 1;
        } else {
            abMatchPoint[0] = 0;
            abMatchPoint[1] = 0;
        }
        if (nTeam >= 0 && !abMatchPoint[nTeam]) {
            if (g_GameWork.m_nPeriod >= (g_GameWork.m_Unk0B >> 1)) {
                // (the set counts are worked out and not used)
                GetGetSetPoint_(g_GameWork.m_nPeriod, 0);
                GetGetSetPoint_(g_GameWork.m_nPeriod, 1);
            }
        }

        if (nTeam >= 0 && nTeam == g_GameWork.GetUserTeam()) {
            if (m_nMatchPointTeam != nTeam) {
                EnableDispMatchPoint_(nTeam);
                m_nMatchPointTeam = nTeam;
            }
        }
        if (nTeam < 0) {
            DisableDispMatchPoint_();
            m_nMatchPointTeam = -1;
        }
    }

    void CGameVolley::EnableDispMatchPoint_(long)
    {
    }

    void CGameVolley::DisableDispMatchPoint_()
    {
    }

    // The 3D score count of the last point, from the scores it showed
    // before.
    void CGameVolley::SetDispScoreCountResult_()
    {
        TelopCreate_(TELOP_SCORE_COUNT, 0, 0);
        long nTeam = g_GameWork.m_nGoalOwnGoal ? 1 - g_GameWork.m_nGoalTeam : g_GameWork.m_nGoalTeam;
        CCockpitVolley::SetTelop3DScoreCount(m_anDispScore[0], m_anDispScore[1], g_GameWork.m_nGoalTeam,
                                             g_GameWork.m_nGoalBaseScore, g_GameWork.m_Unk160,
                                             g_GameWork.m_nGoalBonusType, g_GameWork.m_nGoalBonusPoint,
                                             g_GameWork.m_nGoalBonusColor, g_GameWork.m_nGoalOwnGoal, nTeam);
        CGameSnd::Play(0xBF6, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        for (long i = 0; i < 4; i++) {
            m_anDispScore[i] = GetTeamPeriodScore(i, g_GameWork.m_nPeriod);
        }
    }

    // The 3D set count: the sets won before this one and with it.
    void CGameVolley::SetDispSetCountResult_(int bGameSet)
    {
        TelopCreate_(TELOP_SET_COUNT, 0, 0);
        long nPrev0 = GetGetSetPoint_(g_GameWork.m_nPeriod, 0);
        long nPrev1 = GetGetSetPoint_(g_GameWork.m_nPeriod, 1);
        long nNow0 = GetGetSetPoint_(g_GameWork.m_nPeriod + 1, 0);
        long nNow1 = GetGetSetPoint_(g_GameWork.m_nPeriod + 1, 1);
        CCockpitVolley::SetTelopSetCount(nPrev0, nNow0, nPrev1, nNow1, bGameSet != 0);
    }

    // The sets team nTeam has won of the first nPeriodNum.
    long CGameVolley::GetGetSetPoint_(long nPeriodNum, long nTeam)
    {
        long nOther = 1 - nTeam;
        long nPoint = 0;
        for (long i = 0; i < nPeriodNum; i++) {
            if (m_aabSetWin[nTeam][i]) {
                nPoint++;
            } else if (!m_aabSetWin[nOther][i]) {
                long nScore = GetTeamPeriodScore(nTeam, i);
                if (nScore > GetTeamPeriodScore(nOther, i)) {
                    nPoint++;
                }
            }
        }
        return nPoint;
    }

    void CGameVolley::InitSetCountTelop_()
    {
        CCockpitVolley::SetSetCount(0, 0);
    }

    void CGameVolley::UpdateSetCountTelop_()
    {
        long nPoint0 = GetGetSetPoint_(g_GameWork.m_nPeriod + 1, 0);
        long nPoint1 = GetGetSetPoint_(g_GameWork.m_nPeriod + 1, 1);
        CCockpitVolley::SetSetCount(nPoint0, nPoint1);
    }

    // The score board moves out once the serve is in the air, or after
    // m_fScoreDispWait frames.
    void CGameVolley::CheckScoreDispOut_()
    {
        if (m_nUnk1E8) {
            return;
        }
        int bOut = 0;
        if (!(g_GameWork.m_nFlag18 & 8)) {
            bOut = 1;
        } else {
            long nNum = CBallMan::GetBallNum();
            for (long i = 0; i < nNum; i++) {
                CBall *pBall = CBallMan::GetBallPtr(i);
                if (pBall && pBall->IsShootMode()) {
                    bOut = 1;
                    break;
                }
            }
        }
        if (m_fScoreDispWait > 0.0f) {
            m_fScoreDispWait -= GetFrameRateF();
        }
        if (m_fScoreDispWait <= 0.0f) {
            bOut = 1;
        }
        if (bOut) {
            CCockpitVolley::SetServeAnim(2);
            CGame::SetCockpitPartMove(1, 0);
            SetCockpitItemIconDisp_(1, 1);
            SendMessage_(0x2A, 0, 0, 0);
            m_bLineMsgDisp = 1;
            m_nUnk1E8 = 1;
        }
    }

    // The "shake the remote" telop of each view whose pad's character
    // can do its special shot.
    void CGameVolley::CheckDispRemoconShake_()
    {
        unsigned char abPad[4];
        abPad[0] = 0;
        abPad[1] = 0;
        abPad[2] = 0;
        abPad[3] = 0;
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable() && pChr->IsPadCtrl() && (pChr->GetStatusFlag() & 0x2000) &&
                (!g_GameWork.m_Unk151 || g_GameWork.m_nUnk00 == pChr->GetTeamNo())) {
                abPad[pChr->GetTeamNo()] = 1;
            }
        }

        const unsigned char *pPlace = s_aanShakeTelopPlace[g_GameWork.m_nUnk00 != 0][bcs::CWorldEnv::m_vstViewSplitType];
        long abDisp[5];
        abDisp[0] = 0;
        abDisp[1] = 0;
        abDisp[2] = 0;
        abDisp[3] = 0;
        abDisp[4] = 0;
        for (long i = 0; i < 4; i++) {
            if (abPad[i]) {
                abDisp[pPlace[i]] = 1;
            }
        }

        for (long i = 0; i < 5; i++) {
            long nTelop = s_anRemoconShakeTelop[i];
            if (abDisp[i]) {
                if (TelopIsEnd_(nTelop) && nTelop >= 0) {
                    CCockpit::CreateObj(nTelop, 0);
                }
            } else if (!TelopIsEnd_(nTelop)) {
                TelopRemove_(nTelop);
            }
        }
    }

    // (As in the original, the place table is indexed the other way round
    // from CheckDispRemoconShake_, with a telop number as its last index,
    // which reads outside it. Nothing in the game calls this function.)
    void CGameVolley::EnableDispRemoconShake_(long nTeam)
    {
        TelopCreate_(s_aanShakeTelopPlace[bcs::CWorldEnv::m_vstViewSplitType][nTeam]
                                         [s_anRemoconShakeTelop[g_GameWork.m_nUnk00 != 0]],
                     1, 0);
    }

    void CGameVolley::DisableDispRemoconShake_()
    {
        TelopRemove_(0x1072);
        TelopRemove_(0x1073);
        TelopRemove_(0x1074);
        TelopRemove_(0x1075);
        TelopRemove_(0x1076);
    }

    // Deuce: both teams one point from the set, level, and neither nine
    // past it.
    int CGameVolley::IsNeedDispDeuce_()
    {
        int bDeuce = 0;
        long nPeriod = g_GameWork.m_nPeriod;
        long nWinPoint = g_GameWork.m_nUnk44;
        long nScore0 = GetTeamPeriodScore(0, nPeriod);
        long nScore1 = GetTeamPeriodScore(1, nPeriod);
        if (m_aabSetWin[0][nPeriod]) {
            return 0;
        }
        if (m_aabSetWin[1][nPeriod]) {
            return 0;
        }
        if (nScore0 >= nWinPoint - 1 || nScore1 >= nWinPoint - 1) {
            if (nScore0 == nScore1) {
                bDeuce = 1;
            }
            if (nScore0 >= nWinPoint + 9) {
                bDeuce = 0;
            }
            if (nScore1 >= nWinPoint + 9) {
                bDeuce = 0;
            }
        }
        return bDeuce;
    }

    void CGameVolley::SetDispDeuce_()
    {
        if (IsNeedDispDeuce_()) {
            TelopRemove_(TELOP_FINAL_POINT);
            TelopRemove_(TELOP_SET_POINT);
            TelopRemove_(TELOP_MATCH_POINT);
            TelopCreate_(TELOP_DEUCE, 1, 0);
        }
    }

    // The stages that run at half the frame rate with a split screen.
    int CGameVolley::IsNeedHalfFPS_()
    {
        if (g_GameWork.m_Unk151) {
            return 0;
        }
        switch (g_GameWork.m_nViewSplit) {
            case 1:
            case 2:
                switch (g_GameWork.m_nUnk06) {
                    case 0:
                    case 1:
                    case 9:
                    case 10:
                    case 13:
                    case 14:
                    case 15:
                    case 16:
                    case 18:
                    case 19:
                    case 21:
                    case 24:
                    case 25:
                        return 1;
                }
                break;
        }
        return 0;
    }

    void CGameVolley::SendMessage_(long nMsg, long nArg1, long nArg2, long nArg3)
    {
        switch (nMsg) {
            case 0x12:
                // A new set: the scores and coins of its start, and deuce
                // straight away when every team starts one point short.
                m_bDeuceNext = 0;
                if (g_GameWork.m_nGameType == 0) {
                    long nPeriod = NonNegative(g_GameWork.m_nPeriod);
                    for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                        SetTeamPeriodScore(i, nPeriod, GetPeriodStartScore(i));
                        m_anDispScore[i] = GetPeriodStartScore(i);
                        SetTeamCoin(i, GetPeriodStartCoin(i));
                    }
                    int bDeuce = 1;
                    for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                        if (GetTeamPeriodScore(i, nPeriod) != g_GameWork.m_nUnk44 - 1) {
                            bDeuce = 0;
                            break;
                        }
                    }
                    if (bDeuce) {
                        m_bDeuceNext = 1;
                        m_bDeuce = 1;
                    }
                }
                m_nUnk21C = 0;
                break;
            case 0x4B:
                CreateMiniTelop_(0x22, nArg1);
                break;
            case 0x20:
                if (IsTournament()) {
                    m_nUnk12C = 1;
                    m_Unk130 = nArg1;
                    if (nArg1 > 0) {
                        ForceGameExit_(0);
                    }
                }
                break;
            case 0x21:
                if (IsTournament()) {
                    m_nUnk12C = 2;
                    m_Unk130 = nArg1;
                    if (nArg1 > 0) {
                        ForceGameExit_(1);
                    }
                }
                break;
            case 0x57:
                if (nArg2) {
                    ScoreBookAddCountWork_(0x14, 0, 0);
                }
                break;
        }
        CGame::SendMessage_(nMsg, nArg1, nArg2, nArg3);
    }

    int CGameVolley::IsLineMsgDispEnable_()
    {
        return m_bLineMsgDisp;
    }

    // A small telop over character nChr, in its view (or in the user's
    // view on one screen).
    void CGameVolley::CreateMiniTelop_(long nObjId, long nChr)
    {
        if (nChr < 0 || nChr >= CChrMan::GetChrCoreNum()) {
            return;
        }
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pChr && !pChr->IsDisable()) {
            bcs::FVEC3 pos;
            pChr->GetTransNodePos(pos);
            switch (nObjId) {
                case 0x22:
                    pos.y += 0.3f;
                    break;
                case 0x23:
                    pos.y += 0.3f;
                    break;
            }
            long nCamNo = pChr->GetCamNo();
            if (bcs::CWorldEnv::GetViewSplitNum() > nCamNo &&
                (bcs::CWorldEnv::GetViewSplitNum() > 1 || pChr->GetTeamNo() == g_GameWork.m_nUnk00)) {
                CCockpit::CreateTelopMiniPos(nObjId, nCamNo, pos);
            }
        }
    }

    void CGameVolley::RemoveMiniTelop_(long nObjId)
    {
        CCockpit::RemoveObj(nObjId);
    }

    void CGameVolley::CtorStateGameInit()
    {
        SetTurnStartTelopType_(1, 75.0f);
        g_GameWork.m_nFlag18 = 0;
        SetFirstGoalIdx_();
        GameInit_();
        SetPreparationMode(1);
        SetCockpitTeamScoreNum_();
        InitSetCountTelop_();
        m_nStateStep = 0;
    }

    void CGameVolley::CtorStateGameSetup()
    {
        g_GameWork.m_nFlag18 = 0;
        GameSetUp_();
        SetPreparationMode(1);
        CCockpitVolley::SetDisp(1);
        SetDispRadarIcon_(0);
        for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
            CCockpit::SetTeamCoin(i, GetTeamCoin(i));
        }
        SetCockpitScoreDisp_(1);
        SetCockpitCoinDisp_(1);
        SetCockpitTeamScoreNum_();

        if (g_GameWork.m_nUnk40 & 8) {
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                SetCockpitArtsGaugeRatio_(i, 0.0f, 0.0f);
            }
        } else {
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                SetCockpitArtsGaugeRatio_(i, GetTeamArtsGauge_(i), 0.0f);
            }
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                if (GetTeamArtsGauge_(i) >= 1.0f) {
                    SetCockpitArtsGaugeSpAnim_(i, 1);
                } else {
                    SetCockpitArtsGaugeSpAnim_(i, 0);
                }
            }
        }

        SetCockpitArtsGaugeDisp_(0);
        m_nUnk148 = 1;
        SetCockpitFaceIcon_();
        m_nStateStep = 0;
    }

    void CGameVolley::LoopStateGameSetup()
    {
        m_bPartMoveLock = 0;
        CCockpitVolley::SetPartMove(0, 3, 0);
        m_bPartMoveLock = 1;
        if (CEventDemo::IsEnable() && g_GameWork.m_nUnk180 == 0 && m_nUnk104 > 0) {
            EVENT_DEMO_WORK *pWork = static_cast<EVENT_DEMO_WORK *>(FsmqPushWork(STATE_EVENT_DEMO));
            pWork->nFlag |= 1;
            pWork->nUnk08 = 4;
            pWork->nUnk0C = m_nUnk108;
            pWork->nUnk10 = 1;
            pWork->nUnk18 = 3;
            pWork->fUnk1C = 30.0f;
            m_nUnk104 = 0;
            if (g_GameWork.m_nViewSplit != 2) {
                m_bPartMoveLock = 0;
            }
        }
        FsmqPush(STATE_WAIT_START);
        FsmqNext();
        if (!m_bPartMoveLock) {
            CCockpitVolley::SetPartMove(0, 3, 0);
        }
    }

    void CGameVolley::DtorStateGameSetup()
    {
    }

    void CGameVolley::CtorStateWaitStart()
    {
        ScrnFadeStart_(2, 4, 0.0625f);
        ScrnFadeStart_(1, 4, 0.0625f);
        SetReadyDemoMode(1);
        SetPreparationMode(0);
        m_nGameFlag = 0;
        m_fStateCount = 0.0f;
        CCockpitVolley::SetDisp(1);
        CCockpitVolley::SetMissionCountDisp(0, 0);
        SetCockpitTeamScoreNum_();
        if (g_GameWork.m_nUnk40 & 8) {
            SetCockpitArtsGaugeDisp_(0);
        } else {
            SetCockpitArtsGaugeDisp_(1);
        }
        if (g_GameWork.m_nUnk40 & 1) {
            CCockpitVolley::SetCoinDispEnable(0);
        } else {
            CCockpitVolley::SetCoinDispEnable(1);
        }
        CChrMan::SetChrMarkerDisp(1);
        CChrMan::ResetEventDoll();
        SetDispMatchPoint_();
        m_fScoreDispWait = 90.0f;
        m_nStateStep = 0;

        if (g_GameWork.m_nGameType == 0) {
            ChrPosSetNormal_(1);
            SetFirstState_();
        }
        if (m_bPartMoveLock) {
            m_bPartMoveLock = 0;
            CGame::SetCockpitPartMove(0, 0);
            CGame::SetCockpitPartMove(1, 3);
        }
    }

    void CGameVolley::LoopStateWaitStart()
    {
        if (m_nStateStep == 0) {
            if (IsScrnFadeEnd_(3)) {
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(0), static_cast<CChrCore::STOP_MODE>(0));
                m_fStateCount = 60.0f;
                m_nStateStep++;
            }
        } else if (StateWorkDecStateCount_()) {
            SetDispRadarIcon_(1);
            CGameSnd::Play(0xBF4, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            FsmqChangeState(m_nNextState);
            SendMessage_(0x10, 0, 0, 0);
        }
    }

    void CGameVolley::DtorStateWaitStart()
    {
    }

    void CGameVolley::CtorStateMainLoop()
    {
        SetReadyDemoMode(0);
        SetPreparationMode(0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(0), static_cast<CChrCore::STOP_MODE>(0));
        CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(4), 0, 0, 0, 0);
        m_nGameFlag &= ~1;
    }

    void CGameVolley::LoopStateMainLoop()
    {
        if (m_nGameFlag & 1) {
            if (CChrMan::CanbeTurnOver()) {
                m_nGameFlag |= 8;
                FsmqChangeState(STATE_GOAL_EVENT);
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            }
        } else {
            IncBallModeCounter(GetFrameRateF());
            IncCoinHistCount(GetFrameRateF());
            CheckScoreDispOut_();
            CheckDispRemoconShake_();
            UpdateShotClock_();
        }
    }

    void CGameVolley::DtorStateMainLoop()
    {
        DisableDispRemoconShake_();
        SetCockpitServeClockCount_(-2, 0);
    }

    // The serve clock: counts down while the ball waits to be served and
    // shows the last five seconds.
    void CGameVolley::UpdateShotClock_()
    {
        if (!m_bServeClock) {
            return;
        }
        int bServe = 0;
        if (g_GameWork.m_nFlag18 & 8) {
            if (CBallMan::GetActiveBallNum() > 0 && !CBallMan::GetBallPtr(0)->IsShootMode()) {
                bServe = 1;
            }
        }
        if (bServe) {
            m_fServeClock -= GetFrameRateF();
            m_fServeClock = Max(m_fServeClock, 0.0f);
            long nCount = (59.0f + m_fServeClock - 8.0f) / 60.0f;
            long nColor = GetTeamColorIdx(g_GameWork.GetUserTeam()) != 0;
            if (nCount <= 5) {
                SetCockpitServeClockCount_(nColor, nCount);
            }
        } else if (m_bServeClockDisp) {
            SetCockpitServeClockCount_(-2, 0);
        }
    }

    void CGameVolley::CtorStateTurnResult()
    {
        SetReplayMode(0);
        SetResultMode(1);
        m_nStateStep = 0;
        if (m_nUnk1F4 < 0) {
            if (m_nPointTelop < 0) {
                m_fStateCount = 30.0f;
                m_nStateStep = 2;
            } else {
                m_fStateCount = 150.0f;
                m_nStateStep = 4;
            }
        }
        if (m_Unk130 > 1) {
            CTournament::SendMessage(0x22, 0, 0, 0);
            m_fStateCount = 90.0f;
            m_nStateStep = 99;
        }
    }

    // After a point: the goal demo or its telop, the score count, deuce,
    // then the next turn, the end of the set or of the game.
    void CGameVolley::LoopStateTurnResult()
    {
        switch (m_nStateStep) {
            case 0:
                if (CanbeScoredView_()) {
                    StartScoredView_(-1);
                    TelopCreate_(m_nUnk1F4, 1, 0);
                    m_nStateStep++;
                }
                break;
            case 1:
                if (TelopIsEnd_(m_nUnk1F4)) {
                    SetDispScoreCountResult_();
                    m_nStateStep = 10;
                }
                break;
            case 2:
                if (CanbeScoredView_()) {
                    StartScoredView_(-1);
                    m_nStateStep++;
                }
                break;
            case 3:
                if (StateWorkDecStateCount_()) {
                    SetDispScoreCountResult_();
                    m_nStateStep = 10;
                }
                break;
            case 4:
                TelopCreate_(m_nPointTelop, 1, 0);
                switch (m_nPointTelop) {
                    case 27:
                        CGameSnd::Play(0xC02, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        break;
                    case 28:
                        CGameSnd::Play(0xC01, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        break;
                }
                m_nStateStep++;
                break;
            case 5:
                if (StateWorkDecStateCount_()) {
                    m_fStateCount = 30.0f;
                    m_nStateStep = 2;
                }
                break;
            case 10:
                if (DemoSkipInputCheck(-1) >= 0) {
                    if (IsNeedDispDeuce_()) {
                        float fWait = 60.0f;
                        TelopRemove_(TELOP_SCORE_COUNT);
                        TelopRemove_(TELOP_FINAL_POINT);
                        TelopRemove_(TELOP_SET_POINT);
                        TelopRemove_(TELOP_MATCH_POINT);
                        m_bDeuce = 1;
                        SetDispDeuce_();
                        m_nStateStep++;
                        fWait += 60.0f;
                        m_fStateCount = fWait;
                        break;
                    }
                    UpdateScoreAndCoin_();
                    if (IsGameSet_()) {
                        CGameSnd::Play(0xBF5, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        FsmqChangeState(STATE_GAME_SET);
                    } else if (IsSetOver_()) {
                        CGameSnd::Play(0xBF5, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        FsmqChangeState(STATE_TIME_UP);
                    } else {
                        TelopRemove_(TELOP_SCORE_COUNT);
                        TelopRemove_(TELOP_FINAL_POINT);
                        TelopRemove_(TELOP_SET_POINT);
                        TelopRemove_(TELOP_MATCH_POINT);
                        FsmqChangeState(STATE_TURN_OVER);
                    }
                } else if (TelopIsWait_(TELOP_SCORE_COUNT)) {
                    float fWait = 60.0f;
                    if (IsNeedDispDeuce_()) {
                        TelopRemove_(TELOP_SCORE_COUNT);
                        TelopRemove_(TELOP_FINAL_POINT);
                        TelopRemove_(TELOP_SET_POINT);
                        TelopRemove_(TELOP_MATCH_POINT);
                        m_bDeuce = 1;
                        SetDispDeuce_();
                        fWait += 60.0f;
                    }
                    m_fStateCount = fWait;
                    m_nStateStep++;
                }
                break;
            default:
                if (StateWorkDecStateCount_() || (TelopIsEnd_(TELOP_DEUCE) && DemoSkipInputCheck(-1) >= 0)) {
                    UpdateScoreAndCoin_();
                    if (IsGameSet_()) {
                        CGameSnd::Play(0xBF5, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        FsmqChangeState(STATE_GAME_SET);
                    } else if (IsSetOver_()) {
                        CGameSnd::Play(0xBF5, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                        FsmqChangeState(STATE_TIME_UP);
                    } else {
                        FsmqChangeState(STATE_TURN_OVER);
                    }
                }
                break;
        }
    }

    void CGameVolley::DtorStateTurnResult()
    {
    }

    // The coins of the point were counted with the score: take them back.
    void CGameVolley::UpdateScoreAndCoin_()
    {
        AddTeamCoin(g_GameWork.m_nGoalTeam, -g_GameWork.m_Unk160);
    }

    // The set is over: the set count.
    void CGameVolley::CtorStateTimeUp()
    {
        TelopRemove_(TELOP_SCORE_COUNT);
        TelopRemove_(TELOP_FINAL_POINT);
        TelopRemove_(TELOP_SET_POINT);
        TelopRemove_(TELOP_MATCH_POINT);
        CCockpitVolley::SetPartMove(0, 2, 0);
        SetCockpitItemIconDisp_(0, 0);
        UpdateSetCountTelop_();
        m_nStateStep = 0;
    }

    void CGameVolley::LoopStateTimeUp()
    {
        switch (m_nStateStep) {
            case 0:
                SetDispSetCountResult_(0);
                m_nStateStep++;
                break;
            case 1:
                if (TelopIsWait_(TELOP_SET_COUNT)) {
                    m_fStateCount = 60.0f;
                    m_nStateStep++;
                }
                break;
            case 2:
                if (StateWorkDecStateCount_()) {
                    m_nStateStep++;
                }
                break;
            case 3:
                if (TelopIsEnd_(0xE)) {
                    ScrnFadeStart_(2, 0, 0.025f);
                    m_nStateStep++;
                }
                break;
            default:
                if (IsScrnFadeEnd_(3)) {
                    EndScoredView_();
                    ChgCourtDemoSetup_();
                }
                break;
        }
    }

    void CGameVolley::DtorStateTimeUp()
    {
        TelopRemove_(TELOP_SCORE_COUNT);
        TelopRemove_(TELOP_SET_COUNT);
    }

    // The next set: the other team serves first in every other one.
    void CGameVolley::CtorStateChgCourt()
    {
        CGame::CtorStateChgCourt();
        if (g_GameWork.m_Unk151 || IsVsMode()) {
            long nTeam = static_cast<signed char>(g_GameWork.m_Unk0E);
            if (g_GameWork.m_nPeriod & 1) {
                nTeam = 1 - nTeam;
            }
            SetOffenseTeam(nTeam);
        } else {
            SetOffenseTeam(static_cast<signed char>(g_GameWork.m_Unk0E));
        }
        SetCockpitTeamScoreNum_();
        m_nServeTeam = -1;
        m_nMatchPointTeam = -1;
        long nTeam = g_GameWork.GetUserTeam();
        m_nScoredTeam = nTeam;
        for (long i = 0; i < 4; i++) {
            m_anServeOrder[i] = -1;
            m_anUnk16C[i] = 0;
            m_anDispScore[i] = 0;
        }
        m_nRunTeam = -1;
        m_nRunCount = 0;
        m_bScored = 0;
        m_nUnk1E8 = 0;
        m_bLineMsgDisp = 0;
        m_nUnk1F4 = -1;
        m_nPointTelop = -1;
        m_bDeuceNext = 0;
        m_bDeuce = 0;
        SetTurnStartMode(static_cast<GAME_TURN_START_MODE>(0));
        SetTurnStartTelopType_(1, 75.0f);
    }

    void CGameVolley::LoopStateChgCourt()
    {
        CGame::LoopStateChgCourt();
    }

    void CGameVolley::DtorStateChgCourt()
    {
        SendMessage_(0x12, 0, 0, 0);
        CChrMan::InitLeader(1);
        CGame::DtorStateChgCourt();
    }

    void CGameVolley::CtorStateTurnOver()
    {
    }

    void CGameVolley::LoopStateTurnOver()
    {
        SetPreparationMode(1);
        FsmqChangeState(STATE_CHG_OFFENSE);
    }

    void CGameVolley::DtorStateTurnOver()
    {
    }

    void CGameVolley::CtorStateChgOffense()
    {
        SetTurnStartMode(static_cast<GAME_TURN_START_MODE>(0));
        SetReplayMode(0);
        SetResultMode(0);
        SetPreparationMode(1);
        m_nStateStep = 0;
        SetOffenseTeam(m_nScoredTeam);
        if (m_nScoredTeam != m_nServeTeam) {
            m_nUnk104 = 1;
            m_nUnk108 = 0;
        }
    }

    // A wipe to the next serve, or the except demo first.
    void CGameVolley::LoopStateChgOffense()
    {
        if (IsNeedExceptDemo_()) {
            ScrnFadeStart_(2, 1, 0.0625f);
            SetPreparationMode(1);
            FsmqChangeState(STATE_EXCEPT_DEMO);
        } else {
            CChrMan::AllMotStop();
            bcs::CRenderMan::StartCrossFade(CLoadData::GetWipeMdlFile(), "cutchange01", 4.0f, 1);
            EndScoredView_();
            TelopRemove_(TELOP_SCORE_COUNT);
            TelopRemove_(TELOP_SET_COUNT);
            TelopRemove_(TELOP_DEUCE);
            FsmqChangeState(STATE_GAME_SETUP);
        }
    }

    void CGameVolley::DtorStateChgOffense()
    {
    }

    // The except demo is played when the team that scored last is one
    // point from the set; its telop says set, match or final point.
    int CGameVolley::IsNeedExceptDemo_()
    {
        m_nExceptTelop = -1;
        if (!CEventDemo::IsEnable()) {
            return 0;
        }
        long nPeriod = g_GameWork.m_nPeriod;
        long nWinPoint = g_GameWork.m_nUnk44;
        long nScore = GetTeamPeriodScore(0, nPeriod);
        long nOther = GetTeamPeriodScore(1, nPeriod);
        long nTeam = 0;
        switch (g_GameWork.m_nGoalTeam) {
            case 0:
                break;
            case 1: {
                long nTemp = nScore;
                nScore = nOther;
                nOther = nTemp;
                nTeam = 1;
                break;
            }
            default:
                return 0;
        }
        if ((nScore >= nWinPoint - 1 && nScore > nOther) || nScore >= nWinPoint + 9) {
            m_nExceptTelop = TELOP_MATCH_POINT;
            if (g_GameWork.m_nPeriod >= (g_GameWork.m_Unk0B >> 1)) {
                long anPoint[2];
                anPoint[0] = GetGetSetPoint_(g_GameWork.m_nPeriod, 0);
                anPoint[1] = GetGetSetPoint_(g_GameWork.m_nPeriod, 1);
                if (anPoint[nTeam] >= (g_GameWork.m_Unk0B >> 1)) {
                    m_nExceptTelop = TELOP_SET_POINT;
                    if (IsTourFinalMatch()) {
                        m_nExceptTelop = TELOP_FINAL_POINT;
                    }
                }
            }
            return 1;
        }
        return 0;
    }

    void CGameVolley::CtorStateExceptDemo()
    {
        m_nStateStep = 0;
    }

    void CGameVolley::LoopStateExceptDemo()
    {
        switch (m_nStateStep) {
            case 0:
                if (IsScrnFadeEnd_(3)) {
                    EndScoredView_();
                    TelopRemove_(TELOP_SCORE_COUNT);
                    TelopRemove_(TELOP_SET_COUNT);
                    TelopRemove_(TELOP_DEUCE);
                    SetCockpitDisp_(0);
                    SetCockpitItemIconDisp_(0, 0);
                    SetDispRadarIcon_(0);
                    CChrMan::SetChrMarkerDisp(0);
                    KillTempChild_();
                    BgResetFade_();
                    CChrMan::AllReset();
                    CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                    CBallMan::AllReset();
                    CGimmickMan::AllReset();
                    SendMessage_(0x17, 0, 0, 0);
                    CEffectCtrl::EraseEffectAll();
                    SetCockpitPartMove_(3, 3);
                    SetCockpitPartMove_(1, 3);
                    m_nStateStep++;
                }
                break;
            case 1: {
                SetOffenseTeam(m_nScoredTeam);
                ChangeServeChar_();
                ChrPosSetNormal_(1);
                SetFirstState_();
                SetLooseBallModeFlag(0);
                SetVolleyServeMode(1);
                bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(1), 0);
                bcs::CRenderMan::SetDepthOfFieldParam(1, 6.0f, 7.5f, 1.5f);
                bcs::CRenderMan::EnableDepthOfFieldEffect(1);
                CcVolleyExceptDemo *pDemo = new CcVolleyExceptDemo(this);
                if (pDemo) {
                    m_hExDemo = pDemo->GetId();
                }
                m_nStateStep++;
                break;
            }
            case 2:
                ScrnFadeStart_(3, 4, 0.0f);
                m_nStateStep++;
                // fall through
            case 3: {
                CGameExDemoBase *pDemo = static_cast<CGameExDemoBase *>(bcs::CTaskMan::GetTask(m_hExDemo));
                if (pDemo && pDemo->CanbeNext()) {
                    if (m_nExceptTelop >= 0) {
                        TelopCreate_(m_nExceptTelop, 0, 0);
                    }
                    m_fStateCount = 150.0f;
                    m_nStateStep++;
                }
                break;
            }
            default:
                if (StateWorkDecStateCount_() || DemoSkipInputCheck(-1) >= 0) {
                    if (m_nExceptTelop >= 0) {
                        TelopEnd_(m_nExceptTelop);
                    }
                    CGameExDemoBase *pDemo = static_cast<CGameExDemoBase *>(bcs::CTaskMan::GetTask(m_hExDemo));
                    if (pDemo && pDemo->CanbeNext()) {
                        pDemo->SetNext();
                    }
                    bcs::CRenderMan::EnableDepthOfFieldEffect(0);
                    bcs::CRenderMan::ResetDepthOfFieldParam();
                    bcs::CRenderMan::ReqPopViewSplitType(0);
                    m_nUnk104 = 0;
                    m_nUnk108 = 0;
                    FsmqChangeState(STATE_GAME_SETUP);
                }
                break;
        }
    }

    void CGameVolley::DtorStateExceptDemo()
    {
        m_hExDemo = 0;
    }

    void CGameVolley::CtorStateGameSet()
    {
        TelopRemove_(TELOP_SCORE_COUNT);
        TelopRemove_(TELOP_FINAL_POINT);
        TelopRemove_(TELOP_SET_POINT);
        TelopRemove_(TELOP_MATCH_POINT);
        UpdateSetCountTelop_();
        m_nStateStep = 0;
        if (m_Unk130) {
            m_fStateCount = 180.0f;
            m_nStateStep = 6;
            return;
        }
        if (GetSetNum() <= 1) {
            TelopCreate_(TELOP_GAME_SET, 0, 0);
            m_fStateCount = 60.0f;
            m_nStateStep = 4;
        }
    }

    void CGameVolley::LoopStateGameSet()
    {
        switch (m_nStateStep) {
            case 0:
                CCockpitVolley::SetPartMove(0, 2, 0);
                SetDispSetCountResult_(1);
                m_nStateStep++;
                break;
            case 1:
                if (TelopIsWait_(TELOP_SET_COUNT)) {
                    m_fStateCount = 60.0f;
                    m_nStateStep++;
                }
                break;
            case 2:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_SET_COUNT);
                    m_nStateStep++;
                }
                break;
            case 3:
                if (TelopIsWait_(TELOP_SET_COUNT)) {
                    ScrnFadeStart_(2, 0, 0.0625f);
                    m_nStateStep = 99;
                }
                break;
            case 4:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_GAME_SET);
                    m_nStateStep++;
                }
                break;
            case 5:
                if (TelopIsEnd_(TELOP_GAME_SET)) {
                    ScrnFadeStart_(2, 0, 0.0625f);
                    m_nStateStep = 99;
                }
                break;
            case 6:
                if (StateWorkDecStateCount_()) {
                    ScrnFadeStart_(2, 0, 0.0625f);
                    m_nStateStep = 99;
                }
                break;
            default:
                if (IsScrnFadeEnd_(3)) {
                    EndScoredView_();
                    SetPreparationMode(1);
                    FsmqCutNext();
                    if (bcs::dwc::CDwc::GetInstance()) {
                        FsmqPush(STATE_ONLINE_END);
                    }
                    FsmqPush(STATE_GAME_RESULT);
                    FsmqNext();
                }
                break;
        }
    }

    void CGameVolley::DtorStateGameSet()
    {
        TelopRemove_(TELOP_SCORE_COUNT);
        TelopRemove_(TELOP_SET_COUNT);
        TelopRemove_(TELOP_GAME_SET);
    }

    void CGameVolley::CtorStateGameResult()
    {
        SetReplayMode(0);
        SetResultMode(1);
        SetPreparationMode(0);
        KillTempChild_();
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
        SendMessage_(0x11, 0, 0, 0);
        if (CTournament::IsTournamentGame() && !CTournament::IsExtraMatchGame()) {
            SetGameResultAll_(2);
            switch (CTournament::GetBattleResult()) {
                case 1:
                    SetTeamResult(0, 1);
                    break;
                case 2:
                    SetTeamResult(1, 1);
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

    // The result demo with its music, then the result or end menu.
    void CGameVolley::LoopStateGameResult()
    {
        switch (m_nStateStep) {
            case 0:
                bcs::CMainApp::SetHalfFps(0);
                bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
                m_nStateStep++;
                break;
            case 1:
                CChrMan::AllStockItemDelete();
                CChrMan::AllReset();
                CChrMan::AllSetState(4, -1, 0);
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                CGame::SendMessage(5, 0, 0, 0);
                CEventDemo::CreateDemo(1, this, -1.0f, -1.0f);
                ScrnFadeStart_(2, 4, 0.0625f);
                m_nStateStep++;
                break;
            case 2:
                if (IsScrnFadeEnd_(3)) {
                    CEventDemo::SetFlags(2);
                    PlayGameResultBGM_();
                    m_nStateStep++;
                }
                break;
            case 3:
                if (CEventDemo::m_Work.bSkip) {
                    if (CGameBGM::GetBgmState() == 3) {
                        CEventDemo::SetFlags(1);
                        m_fStateCount = 50.0f;
                        m_nStateStep++;
                    }
                }
                break;
            case 4:
                if (StateWorkDecStateCount_()) {
                    m_nStateStep++;
                }
                break;
            default: {
                int bEnd = 0;
                if (m_Unk130 > 0) {
                    bEnd = 1;
                }
                if (bEnd) {
                    FsmqChangeState(STATE_END_MENU);
                } else {
                    FsmqChangeState(STATE_RESULT_MENU);
                }
                CEventDemo::Cleanup(1);
                break;
            }
        }
    }

    void CGameVolley::DtorStateGameResult()
    {
        CGame::SendMessage(6, 0, 0, 0);
    }

    namespace {

        // (guessed helper) floor() kept in single precision.
        inline float FloorF(float x)
        {
            return floor(x);
        }

        // The skill points of a margin of nDiff points: up to 50 for 60
        // points or more, negative for a negative margin.
        inline long GetSkillPointOfDiff(long nDiff)
        {
            nDiff = Clamp(nDiff, -60L, 60L);
            if (nDiff > 0) {
                float fRate = Clamp(nDiff / 60.0f, 0.0f, 1.0f);
                long nPoint = FloorF(Lerp(0.0f, 50.0f, fRate));
                return Clamp(nPoint, 0L, 50L);
            } else {
                float fRate = Clamp(labs(nDiff) / 60.0f, 0.0f, 1.0f);
                long nPoint = FloorF(Lerp(0.0f, 50.0f, fRate));
                return -Clamp(nPoint, 0L, 50L);
            }
        }
    }

    // Online only: each team gets 20 skill points a set won more than
    // lost (plus 10, and up to 50 for the points), or loses as many.
    void CGameVolley::CalcAddSkillPoint_()
    {
        if (!g_GameWork.m_Unk151) {
            return;
        }
        for (long nTeam = 0; nTeam < 2; nTeam++) {
            long nOther = 1 - nTeam;
            long nWin = 0;
            long nLose = 0;
            long nScore = 0;
            long nOtherScore = 0;
            for (long i = 0; i < g_GameWork.m_Unk0B; i++) {
                long nSetScore = GetTeamPeriodScore(nTeam, i);
                long nSetOther = GetTeamPeriodScore(nOther, i);
                nScore += nSetScore;
                nOtherScore += nSetOther;
                if (m_aabSetWin[nTeam][i]) {
                    nWin++;
                } else if (m_aabSetWin[nOther][i]) {
                    nLose++;
                } else if (nSetScore > nSetOther) {
                    nWin++;
                } else if (nSetScore < nSetOther) {
                    nLose++;
                }
            }
            long nSet = nWin - nLose;
            if (nSet >= 1) {
                long nPoint = nSet * 20 + 10;
                nPoint += GetSkillPointOfDiff(nScore - nOtherScore);
                SetTeamSkillPoint(nTeam, nPoint);
            } else if (nSet == 0) {
                SetTeamSkillPoint(nTeam, 0);
            } else {
                long nPoint = -nSet * 20 + 10;
                nPoint += GetSkillPointOfDiff(nOtherScore - nScore);
                SetTeamSkillPoint(nTeam, -nPoint);
            }
        }
    }

    void CGameVolley::ScoreBookInitCountWork_()
    {
        CGame::ScoreBookInitCountWork_();
        for (long i = 0; i < 10; i++) {
            m_anVolleyScoreBook[i] = 0;
        }
    }

    void CGameVolley::ScoreBookAddCountWork_(long nKind, long, long)
    {
        switch (nKind) {
            case 13:
                m_anVolleyScoreBook[0]++;
                break;
            case 14:
                m_anVolleyScoreBook[1]++;
                break;
            case 15:
                m_anVolleyScoreBook[2]++;
                break;
            case 16:
                m_anVolleyScoreBook[3]++;
                break;
            case 17:
                m_anVolleyScoreBook[4]++;
                break;
            case 18:
                m_anVolleyScoreBook[5]++;
                break;
            case 19:
                m_anVolleyScoreBook[6]++;
                break;
            case 20:
                m_anVolleyScoreBook[7]++;
                break;
            case 21:
                m_anVolleyScoreBook[8]++;
                break;
            case 22:
                m_anVolleyScoreBook[9]++;
                break;
        }
    }

    void CGameVolley::ScoreBookApplyCountWork_()
    {
        CGame::ScoreBookApplyCountWork_();
        SAVE_DATA_PLAYER *pPlayer = CSave::GetPlayerData(-1);
        if (pPlayer) {
            pPlayer->SetVolleyServiceScore(m_anVolleyScoreBook[0], m_anVolleyScoreBook[1]);
            pPlayer->SetVolleySpikeScore(m_anVolleyScoreBook[2], m_anVolleyScoreBook[3]);
            pPlayer->SetVolleyFeintScore(m_anVolleyScoreBook[4], m_anVolleyScoreBook[5]);
            pPlayer->SetVolleyBlockScore(m_anVolleyScoreBook[6], m_anVolleyScoreBook[7]);
            pPlayer->SetVolleySpikeSaveScore(m_anVolleyScoreBook[8], m_anVolleyScoreBook[9]);
        }
    }

    int CGameVolley::SetGameResultNext_()
    {
        if (m_Unk130 == 2) {
            FsmqChangeState(STATE_END_MENU);
        } else {
            FsmqChangeState(STATE_RESULT_MENU);
        }
        return 0;
    }

    // The result screen: each team's score per set and who won it (and
    // whether by a forced end).
    int CGameVolley::MenuCallGameResult_()
    {
        CCockpitVolley::SetMenuState(4);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        long anScore0[5] = {0, 0, 0, 0, 0};
        long anScore1[5] = {0, 0, 0, 0, 0};
        long anWin0[5] = {0, 0, 0, 0, 0};
        long anWin1[5] = {0, 0, 0, 0, 0};
        long anForce0[5] = {0, 0, 0, 0, 0};
        long anForce1[5] = {0, 0, 0, 0, 0};
        for (long i = 0; i < nPeriodNum; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (m_aabSetWin[0][i]) {
                anForce0[i] = 1;
                anWin0[i] = 1;
            } else if (m_aabSetWin[1][i]) {
                anForce1[i] = 1;
                anWin1[i] = 1;
            } else if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriod + 1, nPeriodNum, anScore0, anScore1, anWin0, anWin1, anForce0, anForce1,
                                     -1);
        return 1;
    }

    // The same between sets, up to the set being played.
    int CGameVolley::MenuCallInterval_()
    {
        CCockpitVolley::SetMenuState(5);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        long anScore0[5] = {0, 0, 0, 0, 0};
        long anScore1[5] = {0, 0, 0, 0, 0};
        long anWin0[5] = {0, 0, 0, 0, 0};
        long anWin1[5] = {0, 0, 0, 0, 0};
        long anForce0[5] = {0, 0, 0, 0, 0};
        long anForce1[5] = {0, 0, 0, 0, 0};
        for (long i = 0; i <= nPeriod; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (m_aabSetWin[0][i]) {
                anForce0[i] = 1;
                anWin0[i] = 1;
            } else if (m_aabSetWin[1][i]) {
                anForce1[i] = 1;
                anWin1[i] = 1;
            } else if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriod, nPeriodNum, anScore0, anScore1, anWin0, anWin1, anForce0, anForce1, -1);
        return 1;
    }

    // The same in the pause menu; the set being played has no winner yet.
    int CGameVolley::MenuCallPause_()
    {
        CCockpitVolley::SetMenuState(3);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        long anScore0[5] = {0, 0, 0, 0, 0};
        long anScore1[5] = {0, 0, 0, 0, 0};
        long anWin0[5] = {0, 0, 0, 0, 0};
        long anWin1[5] = {0, 0, 0, 0, 0};
        long anForce0[5] = {0, 0, 0, 0, 0};
        long anForce1[5] = {0, 0, 0, 0, 0};
        for (long i = 0; i <= nPeriod; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (m_aabSetWin[0][i]) {
                anForce0[i] = 1;
                anWin0[i] = 1;
            } else if (m_aabSetWin[1][i]) {
                anForce1[i] = 1;
                anWin1[i] = 1;
            } else if (i < nPeriod) {
                if (anScore0[i] > anScore1[i]) {
                    anWin0[i] = 1;
                } else if (anScore0[i] < anScore1[i]) {
                    anWin1[i] = 1;
                }
            }
        }
        CCockpit::SetResultParameter(nPeriod, nPeriodNum, anScore0, anScore1, anWin0, anWin1, anForce0, anForce1, -1);
        return 1;
    }

    void CGameVolley::SetCockpitArtsGaugeRatio_(long nTeam, float fRatio, float fTelop)
    {
        CCockpitVolley::SetSPGaugeRatio(nTeam, fRatio, fTelop);
    }

    void CGameVolley::SetCockpitArtsGaugeSpAnim_(long nTeam, int bSpecial)
    {
        CCockpitVolley::SetCoinSpecialAnim(nTeam, bSpecial);
        CCockpitVolley::SetSPContrllerDisp(nTeam, bSpecial);
    }

    void CGameVolley::PrepareRestartGame_()
    {
        CCockpitVolley::SetMenuState(1);
        ScrnSetBlackOut_();
        g_GameWork.m_nFlag18 = 0;
    }

    // The set is over at the winning score with a two-point lead after
    // deuce (one before), or at ten points past the winning score.
    int CGameVolley::IsSetOver_()
    {
        long nPeriod = g_GameWork.m_nPeriod;
        long nWinPoint = g_GameWork.m_nUnk44;
        if (m_aabSetWin[0][nPeriod]) {
            return 1;
        }
        if (m_aabSetWin[1][nPeriod]) {
            return 1;
        }
        long nScore0 = GetTeamPeriodScore(0, nPeriod);
        long nScore1 = GetTeamPeriodScore(1, nPeriod);
        long nLead = 1;
        if (m_bDeuce) {
            nLead = 2;
        }
        if (nScore0 >= nWinPoint || nScore1 >= nWinPoint) {
            if (labs(nScore0 - nScore1) >= nLead) {
                return 1;
            }
            if (nScore0 >= nWinPoint + 10) {
                return 1;
            }
            if (nScore1 >= nWinPoint + 10) {
                return 1;
            }
        }
        return 0;
    }

    // The game is set with the last set over, or when a team has won more
    // than half of the sets.
    int CGameVolley::IsGameSet_()
    {
        if (m_Unk130 > 0) {
            return 1;
        }
        if (IsSetOver_()) {
            if (g_GameWork.m_nPeriod >= g_GameWork.m_Unk0B - 1) {
                return 1;
            }
            long nLose = 0;
            long nWin = 0;
            for (long i = 0; i < g_GameWork.m_nPeriod + 1; i++) {
                if (m_aabSetWin[0][i]) {
                    nWin++;
                } else if (m_aabSetWin[1][i]) {
                    nLose++;
                } else {
                    long nScore0 = GetTeamPeriodScore(0, i);
                    long nScore1 = GetTeamPeriodScore(1, i);
                    if (nScore0 != nScore1) {
                        if (nScore0 < nScore1) {
                            nLose++;
                        }
                        if (nScore0 >= nScore1) {
                            nWin++;
                        }
                    }
                }
            }
            long nNeed = (g_GameWork.m_Unk0B >> 1) + 1;
            if (nWin >= nNeed || nLose >= nNeed) {
                return 1;
            }
        }
        return 0;
    }

    void CGameVolley::SetGameResult_()
    {
        ClearGameResult_();
        long nLose = 0;
        long nWin = 0;
        for (long i = 0; i < g_GameWork.m_Unk0B; i++) {
            if (m_aabSetWin[0][i]) {
                nWin++;
            } else if (m_aabSetWin[1][i]) {
                nLose++;
            } else {
                long nScore0 = GetTeamPeriodScore(0, i);
                long nScore1 = GetTeamPeriodScore(1, i);
                if (nScore0 != nScore1) {
                    if (nScore0 < nScore1) {
                        nLose++;
                    }
                    if (nScore0 >= nScore1) {
                        nWin++;
                    }
                }
            }
        }
        long nWinTeam;
        if (nWin > nLose) {
            nWinTeam = 0;
        } else {
            nWinTeam = -1;
            if (nWin < nLose) {
                nWinTeam = 1;
            }
        }
        if (nWinTeam < 0) {
            SetGameResultAll_(3);
        } else {
            SetGameResultAll_(2);
            SetTeamResult(nWinTeam, 1);
        }
    }

    long CGameVolley::GetFirstGoalIdxMode_()
    {
        return -1;
    }

    void CGameVolley::EventDemoSetUp()
    {
        CChrMan::AllSetState(4, -1, 0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        CChrMan::SetChrMarkerDisp(0);
        SetDispRadarIcon_(0);
    }

    void CGameVolley::AddTeamCoin_(long nTeam, long nCoin)
    {
        if (nTeam >= 0 && !(g_GameWork.m_nUnk40 & 1)) {
            long nPrev = GetTeamCoin(nTeam);
            CGame::AddTeamCoin_(nTeam, nCoin);
            if (GetTeamCoin(nTeam) > nPrev) {
                GetMaxCoin_();
            }
        }
    }

    long CGameVolley::GetMaxCoin_() const
    {
        return 10;
    }

    // A point. Bit 0 of nFlag keeps play going, bit 1 leaves the
    // characters alone, bit 2 records no score; bits 3-6 and 9 choose the
    // telop of the point, bit 7 counts it in the score book.
    void CGameVolley::TrgVolleyGoalIn_(long nChr, long nTeam, long nShootType, long nScore, long nCoin, long nPoint,
                                       const bcs::FVEC3 &rPos, unsigned long nFlag)
    {
        long nType = nShootType;
        if (nShootType < 0) {
            nType = 0;
        }
        if (m_nNextState == GetState() && !(m_nGameFlag & 8)) {
            if (!(nFlag & 1)) {
                m_nGameFlag |= 1;
            }
            long nChrTeam = 999;
            int bChr = 0;
            // (as in the original: || where && is meant, so any nChr from 0
            // up is taken without the upper bound test)
            if (nChr >= 0 || nChr < CChrMan::GetChrCoreNum()) {
                CChrCore *pChr = CChrMan::GetChrCore(nChr);
                if (pChr && pChr->IsEnable()) {
                    bChr = 1;
                    nChrTeam = pChr->GetTeamNo();
                }
            }
            if (!(nFlag & 2)) {
                if (!(m_nGameFlag & 2)) {
                    CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                }
                m_nGameFlag |= 2;
            }

            int bDemo = 0;
            if (!(nFlag & 4)) {
                bDemo = 1;
                if (nFlag & 8) {
                    if (g_GameWork.m_nGoalBaseScore > 0) {
                        bDemo = 0;
                    } else {
                        m_nUnk1F4 = 0x15;
                    }
                    nCoin = 0;
                } else if (nFlag & 0x40) {
                    m_aabSetWin[nTeam][g_GameWork.m_nPeriod] = 1;
                    m_nUnk1F4 = 0x1A;
                    SetGoalBonusType(2);
                    SetGoalBonusColor(0);
                    SetGoalBonusRate(2);
                } else if (nFlag & 0x20) {
                    m_nUnk1F4 = 0x19;
                    SetGoalBonusType(2);
                    SetGoalBonusColor(0);
                    SetGoalBonusRate(2);
                } else if (nFlag & 0x10) {
                    m_nUnk1F4 = -1;
                    nCoin = 0;
                } else if (m_nUnk1F4 < 0) {
                    if (nCoin < 1) {
                        m_nPointTelop = -1;
                    } else if (nCoin < 5) {
                        m_nPointTelop = 27;
                    } else {
                        m_nPointTelop = 28;
                    }
                }
                SetGoalFinishMode(1);
                SetGoalEventMode(1);
                SetGoalShootType(nType, 0.0f);
                SetGoalChar(nChr);
                if (nTeam != nChrTeam) {
                    if (g_GameWork.m_nGoalBaseScore <= 0) {
                        SetGoalTeam(nTeam);
                        SetGoalBaseScore(nScore);
                        SetGoalCoin(nCoin);
                        SetTeamDisableDropCoin(nTeam, nCoin);
                        SetGoalOwnGoal(1);
                    }
                } else {
                    SetGoalTeam(nTeam);
                    SetGoalBaseScore(nScore + g_GameWork.m_nGoalBaseScore);
                    if (g_GameWork.m_Unk160 < nCoin) {
                        SetGoalCoin(nCoin);
                        SetTeamDisableDropCoin(nTeam, nCoin);
                    }
                    SetGoalOwnGoal(0);
                }
                if (nFlag & 0x60) {
                    CGame::SendMessage(0x56, nTeam, 0, 0);
                }
                m_bScored = 1;
            }

            if (!(nFlag & 2)) {
                if (m_nRunTeam != g_GameWork.m_nGoalTeam) {
                    m_nRunCount = 1;
                    m_nRunTeam = g_GameWork.m_nGoalTeam;
                } else {
                    m_nRunCount++;
                }
                long nGoalScore = g_GameWork.m_nGoalBaseScore + g_GameWork.m_Unk160;
                switch (g_GameWork.m_nGoalBonusType) {
                    case 1:
                        nGoalScore += g_GameWork.m_nGoalBonusPoint;
                        break;
                    case 2:
                        nGoalScore *= g_GameWork.m_nGoalBonusPoint;
                        break;
                    case 3:
                        nGoalScore -= g_GameWork.m_nGoalBonusPoint;
                        break;
                }
                SetGoalScore(nGoalScore);
                m_GoalPos = rPos;
                if (m_nUnk1F4 < 0 && bDemo) {
                    long nDemo = -1;
                    if (!(nFlag & 0x210)) {
                        nDemo = GetGoalDemoType_(0, 0);
                    }
                    if (nDemo >= 0) {
                        bcs::FVEC3 net = CStageMan::GetInstance()->GetNetPos();
                        long nSide = 0;
                        if (net.z <= rPos.z) {
                            nSide = 1;
                        }
                        StartGoalDemo_(0, nDemo, rPos, nSide,
                                       GetGoalDemoTelop_(nGoalScore, nType, g_GameWork.m_Unk160, 0), 0.05f);
                        m_nUnk11C = 0;
                    } else {
                        m_nUnk11C++;
                    }
                } else if (m_nUnk1F4 >= 0) {
                    bcs::FVEC3 net = CStageMan::GetInstance()->GetNetPos();
                    long nSide = 0;
                    if (net.z <= rPos.z) {
                        nSide = 1;
                    }
                    StartGoalDemo_(1, m_nUnk1F4, rPos, nSide, -1, 0.0f);
                    m_nUnk1F4 = -1;
                    m_nUnk11C = 0;
                }
                AddTeamPeriodScore(g_GameWork.m_nGoalTeam, g_GameWork.m_nPeriod, nGoalScore);
                if (bChr) {
                    long nGet = 0;
                    if (nChrTeam == g_GameWork.m_nGoalTeam) {
                        nGet = 1;
                        if (nPoint > 1) {
                            nGet = nPoint;
                        }
                    }
                    CGame::SendMessage(0x54, nChr, nType, nGet);
                    CGame::SendMessage(0x55, m_nRunTeam, m_nRunCount, nGoalScore);
                }
                CChrMan::SendMsg(nChr, static_cast<CChrDrv::MSG_ID>(8), nType, 0, 0, 0);
                if ((nFlag & 0x80) && nChrTeam == g_GameWork.m_nGoalTeam) {
                    switch (nShootType) {
                        case 2:
                            ScoreBookAddCountWork_(0x12, 0, 0);
                            break;
                        case 1:
                        case 3:
                        case 4:
                            ScoreBookAddCountWork_(0x10, 0, 0);
                            break;
                        case 6:
                        case 7:
                            ScoreBookAddCountWork_(0xE, 0, 0);
                            break;
                    }
                }
                m_nScoredTeam = g_GameWork.m_nGoalTeam;
                m_bScored = 1;
            }
            if (!(nFlag & 1)) {
                CGame::SendMessage(0xE, 0, 0, 0);
                BgSetFadeIn(70.0f, 30.0f);
            }
        }
        SetReboundMode(0);
    }

    int CGameVolley::IsVolleyScored_() const
    {
        return m_bScored;
    }

    int CGameVolley::IsVolleyServeClockOver_() const
    {
        if (m_bServeClock && m_fServeClock <= 0.0f) {
            return 1;
        }
        return 0;
    }

    // The goal demo of a point, by chance and by the state of the set: a
    // first point, a catch-up, the kind of shot, set point, deuce; -1 for
    // none (1 after three points without one).
    long CGameVolley::GetGoalDemoType_(long, long) const
    {
        if (!CEventDemo::IsEnable()) {
            return -1;
        }
        if (CheckGoalDemoRnd_(100) && g_GameWork.m_nPeriod == 0) {
            int bFirst = 1;
            for (long i = 0; i < g_GameWork.m_nTeamNum; i++) {
                if (GetTeamPeriodScore(i, g_GameWork.m_nPeriod) > 0) {
                    bFirst = 0;
                    break;
                }
            }
            if (bFirst && g_GameWork.m_nGoalScore > 0) {
                return 0x10;
            }
        }
        if (CheckGoalDemoRnd_(100)) {
            long nScore0 = GetTeamPeriodScore(0, g_GameWork.m_nPeriod);
            long nScore1 = GetTeamPeriodScore(1, g_GameWork.m_nPeriod);
            if (nScore0 < nScore1) {
                if (nScore0 + g_GameWork.m_nGoalScore > nScore1) {
                    return 0x11;
                }
            } else if (nScore0 > nScore1 && nScore1 + g_GameWork.m_nGoalScore > nScore0) {
                return 0x11;
            }
        }
        switch (static_cast<unsigned long>(g_GameWork.m_nGoalShootType)) {
            case 2:
                if (CheckGoalDemoRnd_(20)) {
                    return 0x17;
                }
                break;
            case 3:
                if (CheckGoalDemoRnd_(20)) {
                    return 0x13;
                }
                break;
            case 6:
            case 7:
                if (CheckGoalDemoRnd_(20)) {
                    return 0x16;
                }
                break;
            case 4:
                if (CheckGoalDemoRnd_(20)) {
                    return 0x14;
                }
                break;
            case 9:
                if (CheckGoalDemoRnd_(100)) {
                    return 0x12;
                }
                break;
        }
        if (CheckGoalDemoRnd_(100)) {
            long nPeriod = g_GameWork.m_nPeriod;
            long nWinPoint = g_GameWork.m_nUnk44;
            long nScore0 = GetTeamPeriodScore(0, nPeriod);
            long nScore1 = GetTeamPeriodScore(1, nPeriod);
            switch (g_GameWork.m_nGoalTeam) {
                case 0:
                    nScore0 += g_GameWork.m_nGoalScore;
                    break;
                case 1:
                    nScore1 += g_GameWork.m_nGoalScore;
                    break;
            }
            if ((nScore0 >= nWinPoint - 1 || nScore1 >= nWinPoint - 1) && nScore0 != nScore1) {
                return 0x18;
            }
        }
        if (CheckGoalDemoRnd_(100)) {
            long nPeriod = g_GameWork.m_nPeriod;
            long nWinPoint = g_GameWork.m_nUnk44;
            long nScore0 = GetTeamPeriodScore(0, nPeriod);
            long nScore1 = GetTeamPeriodScore(1, nPeriod);
            if (nScore0 >= nWinPoint - 1 && nScore1 >= nWinPoint - 1) {
                return 0x19;
            }
        }
        long nType = -1;
        if (m_nUnk11C >= 3) {
            nType = 1;
        }
        return nType;
    }

    // The telop of the goal demo for shot type nType, if the point is
    // worth enough.
    long CGameVolley::GetGoalDemoTelop_(long, long nType, long nCoin, int) const
    {
        long nTelop = -1;
        long nMinCoin = 0;
        switch (nType) {
            case 2:
                nTelop = 0;
                nMinCoin = 0;
                break;
            case 1:
                nTelop = 3;
                nMinCoin = 3;
                break;
            case 3:
                nTelop = 2;
                nMinCoin = 1;
                break;
            case 4:
                nTelop = 3;
                nMinCoin = 3;
                break;
            case 6:
            case 7:
                nTelop = 2;
                nMinCoin = 1;
                break;
            case 9:
                nTelop = 1;
                nMinCoin = 0;
                break;
            case 11:
                nTelop = 3;
                nMinCoin = 0;
                break;
        }
        if (nTelop <= 0) {
            return -1;
        }
        if (nCoin >= nMinCoin) {
            return nTelop - 1;
        }
        return -1;
    }

    // Ends the game with the characters in state nState, unless it is
    // already ending (from the main loop only).
    void CGameVolley::ForceGameExit_(long nState)
    {
        if (m_nNextState == GetState() && !(m_nGameFlag & 8) && !(m_nGameFlag & 1)) {
            if (!(m_nGameFlag & 2)) {
                CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                CChrMan::AllSetState(0, nState, 0);
                m_nGameFlag |= 2;
            }
            m_nGameFlag |= 1;
        }
    }

    void CGameVolley::GameInit_()
    {
        m_nGameFlag = 0;
        GameWorkReset_();
        CChrMan::InitLeader(1);
        m_nServeTeam = -1;
        m_nMatchPointTeam = -1;
        m_nScoredTeam = g_GameWork.GetUserTeam();
        for (long i = 0; i < 4; i++) {
            m_anServeOrder[i] = -1;
        }
        for (long i = 0; i < 4; i++) {
            m_anUnk16C[i] = 0;
            m_anDispScore[i] = 0;
        }
        for (long i = 0; i < 2; i++) {
            for (long j = 0; j < 5; j++) {
                m_aabSetWin[i][j] = 0;
            }
        }
        for (long i = 0; i < 2; i++) {
            for (long j = 0; j < 5; j++) {
                m_aanUnk1A4[i][j] = 0;
            }
        }
        m_nRunTeam = -1;
        m_nRunCount = 0;
        m_bScored = 0;
        m_nUnk1E8 = 0;
        m_bLineMsgDisp = 0;
        m_nUnk1F4 = -1;
        m_nPointTelop = -1;
        SendMessage_(0xF, 0, 0, 0);
        SendMessage_(0x12, 0, 0, 0);
        m_nUnk104 = 1;
        m_nUnk108 = 0;
        m_nUnk12C = 0;
        m_Unk130 = 0;
        m_nUnk21C = 0;
        m_nUnk11C = 0;
        m_bDeuce = m_bDeuceNext;
    }

    void CGameVolley::GameSetUp_()
    {
        m_nGameFlag = 0;
        m_bScored = 0;
        m_nUnk1E8 = 0;
        m_bLineMsgDisp = 0;
        m_nUnk1F4 = -1;
        m_nPointTelop = -1;
        InitServeClock_();
        KillTempChild_();
        BgResetFade_();
        CChrMan::AllReset();
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        CBallMan::AllReset();
        CGimmickMan::AllReset();
        CEffectCtrl::EraseEffectAll();
        SendMessage_(0x13, 0, 0, 0);
        CGimmickMan::SetUp();
        CGimmickMan::QPanelSetViewBaseRot(GetTargetGoalIdx(GetViewTargetTeam(0)) ? 3.1415927f : 0.0f);
        ClearLastGoalInfo();
        g_GameWork.m_fUnk38 = 0.0f;
        ClearCoinHistory();
        SetVolleyServeMode(1);
        SetOffenseTeam(m_nScoredTeam);
        ChangeServeChar_();
        ChrPosSetNormal_(1);
        SetFirstState_();
        SetLooseBallModeFlag(0);
        SetVolleyServeMode(1);
        CameraReset_();
        CChrMan::AllSendReset();
    }

    // When the serve changes sides: the user's team's next character in
    // turn serves (a computer team skips characters the pad drives).
    void CGameVolley::ChangeServeChar_()
    {
        if (m_nScoredTeam == m_nServeTeam) {
            return;
        }
        long nTeam = g_GameWork.GetUserTeam();
        if (GetTeamCtrlType(nTeam) == 1) {
            if (m_anServeOrder[nTeam] < 0) {
                m_anServeOrder[nTeam] = nTeam;
            } else {
                m_anServeOrder[nTeam] += 2;
                m_anServeOrder[nTeam] %= CChrMan::GetChrCoreNum();
            }
        } else {
            long nStart = m_anServeOrder[nTeam];
            for (;;) {
                if (m_anServeOrder[nTeam] < 0) {
                    m_anServeOrder[nTeam] = nTeam;
                    nStart = nTeam;
                } else {
                    m_anServeOrder[nTeam] += 2;
                    m_anServeOrder[nTeam] %= CChrMan::GetChrCoreNum();
                }
                if (nStart == m_anServeOrder[nTeam]) {
                    break;
                }
                CChrCore *pChr = CChrMan::GetChrCore(m_anServeOrder[nTeam]);
                if (IsPlayTypeCanChangeLeader(nTeam) || pChr->DriverGetType() == 1) {
                    break;
                }
            }
        }
        m_nServeTeam = nTeam;
        m_nScoredTeam = nTeam;
    }

    // The server takes its serve state, every other character its
    // defence state.
    void CGameVolley::SetFirstState_()
    {
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            int bServer = 0;
            if (pChr->IsOffenseTeam() && pChr->GetPlayerNo() == m_anServeOrder[pChr->GetTeamNo()]) {
                bServer = 1;
            }
            if (bServer) {
                pChr->Util_SetOffenseLeaderFirstState(0);
                if (m_nUnk104 > 0) {
                    m_nUnk108 = pChr->GetPlayerNo();
                }
            } else {
                pChr->Util_SetDefenseLeaderFirstState();
            }
        }
    }

    // The characters at their start places: each team's server (or
    // leader) first, the serving team in its serve places.
    void CGameVolley::ChrPosSetNormal_(int bUserServe)
    {
        int abReceive[2];
        abReceive[0] = 1;
        abReceive[1] = 1;
        if (bUserServe) {
            abReceive[g_GameWork.GetUserTeam()] = 0;
        }
        long anIdx[6];
        for (long i = 0; i < 6; i++) {
            anIdx[i] = i;
        }
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < 2; i++) {
            long nFirst = i;
            if (m_anServeOrder[i] >= 0) {
                nFirst = m_anServeOrder[i];
            }
            if (nNum > 4) {
                if (i + 4 == nFirst) {
                    anIdx[i] = i + 4;
                    anIdx[i + 2] = i + 2;
                    anIdx[i + 4] = i;
                } else if (i + 2 == nFirst) {
                    anIdx[i] = i + 2;
                    anIdx[i + 2] = i;
                    anIdx[i + 4] = i + 4;
                } else {
                    anIdx[i] = i;
                    anIdx[i + 2] = i + 4;
                    anIdx[i + 4] = i + 2;
                }
            } else if (nNum > 2) {
                if (i + 2 == nFirst) {
                    anIdx[i] = i + 2;
                    anIdx[i + 2] = i;
                } else {
                    anIdx[i] = i;
                    anIdx[i + 2] = i + 2;
                }
            }
        }

        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(anIdx[i]);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            long nPlace = i >> 1;
            const bcs::FVEC3 &rPos = s_aaChrPosNormal[abReceive[pChr->GetTeamNo()]][nPlace];
            float x = rPos.x;
            float fRot = rPos.y;
            float z = rPos.z;
            if (GetGoalIdx(pChr->GetTeamNo())) {
                x *= -1.0f;
                z *= -1.0f;
                fRot = WrapRad(fRot + 3.1415927f);
            }
            bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(GetTargetGoalIdx(pChr->GetTeamNo()));
            bcs::FVEC3 net = CStageMan::GetInstance()->GetNetPos();
            // the other places stay on the team's side of the net
            if (nPlace > 0) {
                if (GetGoalIdx(pChr->GetTeamNo())) {
                    ofs.z = ofs.z > net.z ? net.z : ofs.z;
                } else {
                    ofs.z = ofs.z < net.z ? net.z : ofs.z;
                }
            }
            x += ofs.x;
            z += ofs.z;
            pChr->SetRotY(fRot);
            pChr->SetPos(bcs::FVEC3(x, 0.0f, z));
            pChr->RecalcAllMatrix();
        }
    }

    void CGameVolley::ChrPosSetResult_()
    {
        long anIdx[6];
        for (long i = 0; i < 6; i++) {
            anIdx[i] = i;
        }
        if (CChrMan::GetChrCore(2)->GetStatusFlag() & 0x100) {
            anIdx[0] = 2;
            anIdx[2] = 0;
        }
        if (CChrMan::GetChrCore(3)->GetStatusFlag() & 0x100) {
            anIdx[1] = 3;
            anIdx[3] = 1;
        }

        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pChr = CChrMan::GetChrCore(anIdx[i]);
            if (pChr->IsDisable()) {
                continue;
            }
            const bcs::FVEC3 &rPos = s_aChrPosResult[i >> 1];
            float x = rPos.x;
            float fRot = rPos.y;
            float z = rPos.z;
            if (GetGoalIdx(pChr->GetTeamNo())) {
                x *= -1.0f;
                z *= -1.0f;
                fRot = WrapRad(3.1415927f + fRot);
            }
            pChr->SetRotY(fRot);
            pChr->SetPos(bcs::FVEC3(x, 0.0f, z));
            pChr->RecalcAllMatrix();
        }
    }
}
