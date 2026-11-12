// msw::CGameBoss, the game task of the boss battle (BossF), with the three
// helper tasks of its arts (special shot) event: the event's camera, the
// marker circle that turns under the character starting the special shot,
// and that circle's model.
//
// The file is GameBoss.cpp, not CGameBoss.cpp: the unnamed namespace of the
// original is mangled "@unnamed@GameBoss_cpp@" (see the CStateTemplate
// instances of CcVsBossSpCircle).
#include "game/msw/CGameBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/EF/CEffect.hpp"
#include "bcs/EF/CResource.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManBoss.hpp"
#include "game/msw/CCameraBoss.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBoss.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitBoss.hpp"
#include "game/msw/CCollManBoss.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCtrlBoss.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CEventDemoBoss.hpp"
#include "game/msw/CFootmarkMan.hpp"
#include "game/msw/CGameBGM.hpp"
#include "game/msw/CGameCamera.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CGimmickManBoss.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CShadowTexMan.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageManBoss.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/TournamentBoss.hpp"
#include "game/msw/Unlock.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "nw4r/g3d/ResFile.hpp"

#include <math.h>

namespace msw {
    namespace {

        // The states of CGame's state queue this class changes to (the
        // numbers are the places of the Ctor/Loop/Dtor triples in CGame's
        // vtable plus two; names after those functions).
        enum {
            STATE_WAIT_START = 4,
            STATE_TURN_RESULT = 7,
            STATE_TIME_UP = 8,
            STATE_CHG_OFFENSE = 11,
            STATE_GAME_SET = 12,
            STATE_GAME_RESULT = 13,
            STATE_RESULT_MENU = 14,
            STATE_END_MENU = 15,
            STATE_EVENT_DEMO = 19
        };

        // Telops (CGame::TelopCreate_ numbers; names guessed).
        enum {
            TELOP_READY = 4,
            TELOP_TIME_UP = 5,
            TELOP_GAME_SET = 0x1061
        };

        // (guessed) The work memory of CGame's event demo state, as the game
        // setup fills it in. Only the words written here are known.
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

        // The angle (degrees) around the boss at which each character of the
        // arts event is put; the first is the one starting it. Only the
        // third row is read. (meaning of the rows not known)
        const float s_afArtsChrAngle[3][3] = {
            { 0.0f, -45.0f, 45.0f },
            { 0.0f, -45.0f, 45.0f },
            { 0.0f, -45.0f, 45.0f },
        };

        // The camera moves of the arts event (CcVsBossArtsCamera): from
        // pos0/at0 to pos1/at1 over fFrame frames, the roll (degrees) from
        // fRoll0 to fRoll1, along t to the power fPow. (names guessed)
        struct ARTS_CAM {
            bcs::FVEC3 pos0;
            bcs::FVEC3 at0;
            bcs::FVEC3 pos1;
            bcs::FVEC3 at1;
            float fFrame;
            float fRoll0;
            float fRoll1;
            float fPow;
        };

        ARTS_CAM s_aArtsCam[4] = {
            {
                bcs::FVEC3(-0.98f, 1.11f, 7.0f), bcs::FVEC3(-0.9f, 1.0f, 6.0f),
                bcs::FVEC3(-2.08f, 2.34f, 20.03f), bcs::FVEC3(-1.99f, 2.25f, 19.04f),
                180.0f, 3.0f, -3.0f, 0.2857143f,
            },
            {
                bcs::FVEC3(-4.54f, 1.16f, 1.98f), bcs::FVEC3(-3.84f, 1.53f, 2.6f),
                bcs::FVEC3(-5.36f, 0.74f, 2.14f), bcs::FVEC3(-4.5f, 1.21f, 1.97f),
                180.0f, -3.0f, 0.0f, 0.6666667f,
            },
            {
                bcs::FVEC3(-3.98f, 7.09f, 7.1f), bcs::FVEC3(-3.64f, 6.68f, 6.25f),
                bcs::FVEC3(-2.84f, 5.72f, 4.27f), bcs::FVEC3(-2.5f, 5.31f, 3.42f),
                120.0f, 0.0f, 0.0f, 0.6666667f,
            },
            {
                bcs::FVEC3(2.09f, 0.19f, 8.89f), bcs::FVEC3(2.04f, 0.67f, 8.02f),
                bcs::FVEC3(1.92f, 1.81f, 5.96f), bcs::FVEC3(1.87f, 2.29f, 5.09f),
                120.0f, 0.0f, 0.0f, 0.6666667f,
            },
        };

        // The model of the marker circle and its start, loop and end motions.
        const char *s_apSpCircleMdlName[1] = {"boss_sp_marker00"};
        const char *s_apSpCircleAnmName[1][3] = {
            {"boss_sp_marker_start", "boss_sp_marker_loop", "boss_sp_marker_end"},
        };

        // Where the characters start, per pair of players (two to a
        // place): x, the yaw in radians, z. For the normal start and for
        // the result. The other team's side is the mirror image.
        bcs::FVEC3 s_aChrPosNormal[3] = {
            bcs::FVEC3(0.0f, 3.1415927f, 11.5f),
            bcs::FVEC3(-2.0f, 3.1415927f, 1.5f),
            bcs::FVEC3(2.0f, 3.1415927f, 1.5f),
        };
        bcs::FVEC3 s_aChrPosResult[3] = {
            bcs::FVEC3(0.0f, 0.0f, 3.0f),
            bcs::FVEC3(2.0f, 0.0f, 2.0f),
            bcs::FVEC3(-2.0f, 0.0f, 2.0f),
        };

        // (guessed helpers) An angle brought into [0, 2 pi), degrees to
        // radians, and a clamp.
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

        inline float DegToRad(float fDeg)
        {
            return 0.017453292f * fDeg;
        }

        inline float Max(float fValue, float fMin)
        {
            return fMin < fValue ? fValue : fMin;
        }

        inline float Clamp(float fValue, float fMin, float fMax)
        {
            return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
        }

        // The cockpit shows five power points a coin.
        inline void SetBallPower(long nTeam, long nCoin)
        {
            CCockpitBoss::SetBallPowerValue(nTeam, nCoin * 5);
        }

        // Puts every character in play in its first defence state.
        inline void SetAllDefenseLeaderFirstState()
        {
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr && pChr->IsEnable()) {
                    pChr->Util_SetDefenseLeaderFirstState();
                }
            }
        }

        // Queues a scene object for deletion and forgets it.
        template <class T>
        inline void DeleteScnObj(T *&pObj)
        {
            if (pObj) {
                bcs::CScnObj::AppendDelayDeleteObject(pObj);
            }
            pObj = 0;
        }

        // Asks a task to delete itself and forgets it.
        template <class T>
        inline void DeleteTask(T *&pTask)
        {
            if (pTask) {
                pTask->Delete();
            }
            pTask = 0;
        }

        // Not zero when no character is left to play: with leader changes
        // allowed, when every character is beaten; otherwise when every
        // character played by a pad is.
        inline int IsAllChrDead()
        {
            int bDead = 1;
            if (CGame::IsPlayTypeCanChangeLeader(0)) {
                long nNum = CChrMan::GetChrCoreNum();
                for (long i = 0; i < nNum; i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && !pChr->IsVsBossDead()) {
                        bDead = 0;
                        break;
                    }
                }
            } else {
                long nNum = CChrMan::GetChrCoreNum();
                for (long i = 0; i < nNum; i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr && !pChr->IsDisable() && !pChr->IsVsBossDead() && pChr->IsPadCtrl()) {
                        bDead = 0;
                        break;
                    }
                }
            }
            return bDead;
        }

        // The camera of the arts event: moves along one of s_aArtsCam.
        // 0x140 bytes. (name guessed)
        class CcVsBossArtsCamera : public bcs::CCamera {
            public:
                CcVsBossArtsCamera(long nType);
                virtual ~CcVsBossArtsCamera();
                virtual void MainCamera();

            private:
                bcs::FVEC3 m_aPos[2];  // 0xFC
                bcs::FVEC3 m_aAt[2];   // 0x114
                float m_afRoll[2];     // 0x12C, degrees
                float m_fFrame;        // 0x134
                float m_fFrameMax;     // 0x138
                float m_fPow;          // 0x13C
        };

        // The model of the marker circle with its animations: the start
        // motion, the loop, and the end motion once End is called. 0x64
        // bytes. (name guessed)
        class CcVsBossSpCircleObj : public CGameTask {
            public:
                CcVsBossSpCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                    nw4r::g3d::ResFile file);
                virtual ~CcVsBossSpCircleObj();
                virtual void Main();
                // Plays the end motions; the task deletes itself after them.
                virtual void End() { m_bEnd = 1; }

                bcs::CMdlObj *GetMdl() const { return m_pMdl; }

            private:
                enum {
                    STEP_DONE = 99
                };

                void Create_(bcs::CScnGroup *pGroup, nw4r::g3d::ResFile file);

                nw4r::g3d::ResFile m_File;     // 0x34
                long m_nType;                  // 0x38, index in s_apSpCircleAnmName
                bcs::CMdlObj *m_pMdl;          // 0x3C
                bcs::CAnmMotion *m_pAnmChr;    // 0x40
                bcs::CAnmMatClr *m_pAnmClr;    // 0x44
                bcs::CAnmTexSrt *m_pAnmTexSrt; // 0x48
                bcs::CAnmTexPat *m_pAnmTexPat; // 0x4C
                // 0x50: the step of each animation (motion, colour, texture
                // SRT, texture pattern); STEP_DONE once it has ended.
                long m_anStep[4];
                int m_bEnd;                    // 0x60
        };

        // The marker circle of the arts event. 0xAC bytes. (member names
        // guessed)
        class CcVsBossSpCircle : public CGameTask, public bcs::CFsm<CcVsBossSpCircle> {
            public:
                enum {
                    STATE_WAIT,      // until the arts event starts
                    STATE_APPEAR,
                    STATE_TURN,
                    STATE_LOOP,      // while the character does the trick
                    STATE_DISAPPEAR,
                    STATE_NUM
                };

                CcVsBossSpCircle(bcs::CTask *pParent, const bcs::FVEC3 &rPos, long nChr);
                virtual ~CcVsBossSpCircle();
                virtual void Main();

            private:
                enum {
                    EFFECT_NUM = 4
                };

                void CtorStateWait();
                void LoopStateWait();
                void DtorStateWait();
                void CtorStateAppear();
                void LoopStateAppear();
                void DtorStateAppear();
                void CtorStateTurn();
                void LoopStateTurn();
                void DtorStateTurn();
                void CtorStateLoop();
                void LoopStateLoop();
                void DtorStateLoop();
                void CtorStateDisappear();
                void LoopStateDisappear();
                void DtorStateDisappear();

                long m_nStep;                         // 0x7C
                bcs::FVEC3 m_Pos;                     // 0x80
                bcs::CScnGroup *m_pGroup;             // 0x8C
                unsigned long m_hObj;                 // 0x90, the CcVsBossSpCircleObj (task id)
                unsigned long m_ahEffect[EFFECT_NUM]; // 0x94
                float m_fWait;                        // 0xA4
                long m_nChr;                          // 0xA8, the character doing the trick
        };
    }
}

namespace msw {

    CGameBoss::CGameBoss(bcs::CTask *pParent) : CGame(pParent, GAME_MODE_BOSS)
    {
        m_nUnk160 = 0;
        m_nCount = 0;
        m_fBossLife = 0.0f;
        m_pInstance = this;
        Initialize_();
    }

    CGameBoss::~CGameBoss()
    {
        Finalize_();
    }

    void CGameBoss::Initialize_()
    {
        InitWork_();
        CTournament::SetDemoType(0x5A);
    }

    void CGameBoss::InitWork_()
    {
        m_nUnk150 = -1;
        m_nUnk154 = -1;
        m_fUnk158 = 120.0f;
        m_bPartMove = 0;
        m_fPowerUpTelop = 0.0f;
        for (long i = 0; i < CHR_MAX; i++) {
            m_afUnk19C[i] = 0.0f;
            m_abOutTelop[i] = 0;
        }
        m_bSPController = 0;
        m_nArtsChrNum = 0;
        for (long i = 0; i < CHR_MAX; i++) {
            m_anArtsChr[i] = -1;
        }
        m_pArtsCam = 0;
    }

    void CGameBoss::Finalize_()
    {
        CLytCtrl::Dispose();
        CLytCtrl::DetachResource();
        CLytCtrl::Close();
    }

    void CGameBoss::MainFunc()
    {
        UpdateCockpitCount_();
    }

    // The life gauges: the boss's, each character's (with its "out" telop),
    // and the special-shot controller shown while a pad's character can do
    // its air trick.
    void CGameBoss::UpdateCockpitCount_()
    {
        CCockpitBoss::SetBossLifePercent(m_fBossLife);
        // (The original also compares g_GameWork.m_fUnk20 <= 0 here and
        // uses nothing of it; left out.)

        int bSPController = 0;
        long nNum = CChrMan::GetChrCoreNum();
        long nGauge = 0;
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            float fLife = pChr->GetVsBossHPRate();
            CCockpitBoss::SetLifePercent(nGauge, fLife, fLife);
            if (fLife <= 0.0f) {
                if (!m_abOutTelop[nGauge]) {
                    CCockpitBoss::SetOutTelopState(nGauge, 1);
                    m_abOutTelop[nGauge] = 1;
                }
            } else {
                if (m_abOutTelop[nGauge]) {
                    CCockpitBoss::SetOutTelopState(nGauge, 0);
                    m_abOutTelop[nGauge] = 0;
                }
                if (pChr->CanbeAirTrick() && pChr->IsPadCtrl()) {
                    bSPController = 1;
                }
            }
            nGauge++;
        }

        if (bSPController) {
            if (!m_bSPController) {
                CCockpitBoss::SetSPContrllerDisp(0, 1);
                m_bSPController = 1;
            }
        } else if (m_bSPController) {
            CCockpitBoss::SetSPContrllerDisp(0, 0);
            m_bSPController = 0;
        }

        UpdatePowerUpTelop_(GetFrameRateF());
    }

    void CGameBoss::DispFunc()
    {
    }

    void CGameBoss::FinishFirstLoadData()
    {
    }

    void CGameBoss::GenerateInstance()
    {
        CShadowTexMan::CreateInstance(this);
        CFootmarkMan::CreateInstance(this);
        CCollManBoss::CreateInstance(this);
        CGimmickManBoss::CreateInstance(this);
        CChrManBoss::CreateInstance(this);
        CBallManBoss::CreateInstance(this);
        CEffectCtrlBoss::CreateInstance(this);
        CStageManBoss::CreateInstance(this, g_GameWork.m_nStage);
        CLytCtrl::CreateInstance(this);
        EvDemoBoss::CreateInstance(this);
        TournamentBoss::CreateInstance(this);
        CCockpitBoss::Create(this);
        menu::CSndEff::s_Instance.Initialize();
        menu::CVibEff::s_Instance.Initialize();
        CCameraBoss::Initialize();
        for (long i = 0; i < bcs::CWorldEnv::GetViewSplitNum(); i++) {
            m_apCamera[i] = CGameCamera::GetInstance(i);
        }
    }

    // The characters are put in a circle 8 apart around the centre, the
    // one starting the special shot in front, all facing the centre; the
    // others take the waiting motion.
    void CGameBoss::StartArtsEvent_(long nChr)
    {
        bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
        m_nArtsChrNum = 0;
        for (long i = 0; i < CHR_MAX; i++) {
            m_anArtsChr[i] = -1;
        }

        long nSlot = 1;
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            long nIdx = nSlot;
            if (nChr == pChr->GetPlayerNo()) {
                nIdx = 0;
            }
            if (nChr != pChr->GetPlayerNo()) {
                nSlot++;
            }
            m_anArtsChr[nIdx] = pChr->GetPlayerNo();

            float fRot = WrapRad(DegToRad(s_afArtsChrAngle[2][nIdx]));
            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, 8.0f, fRot);
            pos.x += center.x;
            pos.z += center.z;
            pChr->SetPos(pos);
            pChr->SetRotY(pChr->GetRotY_ToPosXZ(center));
            if (nChr != pChr->GetPlayerNo()) {
                pChr->AnimPlay(3, 0xA3, 0, 0.0f, 0.0f);
            }
        }
        m_nArtsChrNum = nSlot;

        KillTempChild_();
        CGimmickMan::AllReset();
        CEffectCtrl::EraseEffectAll();
    }

    void CGameBoss::EndArtsEvent_(long)
    {
        if (m_pArtsEventTask) {
            m_pArtsEventTask->Delete();
            m_pArtsEventTask = 0;
        }
    }

    float CGameBoss::GetArtsEventFrameNum_()
    {
        return CGame::GetArtsEventFrameNum_();
    }

    void CGameBoss::ArtsEventLoop_()
    {
        CGame::ArtsEventLoop_();
    }

    // Starts camera move nType of the arts event; the first one also puts
    // the marker circle in front of the boss.
    void CGameBoss::ArtsSetCam_(long nType)
    {
        ArtsEndCam_();
        m_pArtsCam = new (0x100) CcVsBossArtsCamera(nType);
        SetCockpitDisp_(0);
        SetDispRadarIcon_(0);
        if (nType == 0) {
            bcs::FVEC3 pos(0.0f, 0.0f, -4.0f);
            new (0x100) CcVsBossSpCircle(this, pos, g_nEventChrIdx);
        }
    }

    void CGameBoss::ArtsEndCam_()
    {
        DeleteTask(m_pArtsCam);
        SetCockpitDisp_(1);
        SetDispRadarIcon_(1);
    }

    namespace {

        CcVsBossArtsCamera::CcVsBossArtsCamera(long nType) : CCamera(0, 0x80, 0)
        {
            OnUnk08(0xFCFF);
            const ARTS_CAM &rCam = s_aArtsCam[nType];
            m_fFrameMax = rCam.fFrame;
            m_fFrameMax = Max(m_fFrameMax, 1.0f);
            m_fFrame = 0.0f;
            m_aPos[0] = rCam.pos0;
            m_aPos[1] = rCam.pos1;
            m_aAt[0] = rCam.at0;
            m_aAt[1] = rCam.at1;
            m_afRoll[0] = rCam.fRoll0;
            m_afRoll[1] = rCam.fRoll1;
            m_fPow = rCam.fPow;
            m_Data.pos = m_aPos[0];
            m_Data.at = m_aAt[0];
            m_Data.roll = DegToRad(m_afRoll[0]);
            SetActive(-1);
        }

        CcVsBossArtsCamera::~CcVsBossArtsCamera()
        {
        }

        void CcVsBossArtsCamera::MainCamera()
        {
            m_fFrame += GetFrameRateF();
            m_fFrame = Clamp(m_fFrame, 0.0f, m_fFrameMax);
            float fRate = Clamp(m_fFrame * (1.0f / m_fFrameMax), 0.0f, 1.0f);
            float t = Clamp((float)pow(fRate, m_fPow), 0.0f, 1.0f);

            bcs::FVEC3 pos;
            bcs::FVEC3 at;
            bcs::BCS_Vec_Lerp(pos, m_aPos[0], m_aPos[1], t);
            bcs::BCS_Vec_Lerp(at, m_aAt[0], m_aAt[1], t);
            float fRoll = DegToRad(m_afRoll[0] + t * (m_afRoll[1] - m_afRoll[0]));
            m_Data.pos = pos;
            m_Data.at = at;
            m_Data.roll = fRoll;
        }

        CcVsBossSpCircleObj::CcVsBossSpCircleObj(bcs::CTask *pParent, bcs::CScnGroup *pGroup, long nType,
                                                 nw4r::g3d::ResFile file)
            : CGameTask(pParent, -1), m_File(file), m_nType(nType)
        {
            m_pMdl = 0;
            m_pAnmChr = 0;
            m_pAnmClr = 0;
            m_pAnmTexSrt = 0;
            m_pAnmTexPat = 0;
            m_bEnd = 0;
            for (long i = 0; i < 4; i++) {
                m_anStep[i] = 0;
            }
            Create_(pGroup, file);
        }

        CcVsBossSpCircleObj::~CcVsBossSpCircleObj()
        {
            DeleteScnObj(m_pMdl);
        }

        // The model, drawn on top without depth test, with every animation
        // the file has, put in pGroup.
        void CcVsBossSpCircleObj::Create_(bcs::CScnGroup *pGroup, nw4r::g3d::ResFile file)
        {
            if (!file.IsValid()) {
                return;
            }
            nw4r::g3d::ResMdl mdl = file.GetResMdl(0);
            if (!mdl.IsValid()) {
                return;
            }

            m_pMdl = new bcs::CMdlObj;
            unsigned long anmFlag = 0;
            file.HasResAnmChr(); // (guessed) tested, but adds no flag
            if (file.HasResAnmClr()) {
                anmFlag |= 0x108;
            }
            if (file.HasResAnmTexSrt()) {
                anmFlag |= 0x204;
            }
            if (file.HasResAnmTexPat()) {
                anmFlag |= 0x3;
            }
            if (file.HasResAnmShp()) {
                anmFlag |= 0x7000;
            }
            m_pMdl->Create(mdl, anmFlag, 0);
            m_pMdl->SetExBlendMode(0);
            m_pMdl->OnFlag(0x100);
            m_pMdl->OnFlag(0x4000);
            m_pMdl->MatSetZModeAll(0, GX_ALWAYS, 0);
            m_pMdl->SetDrawPriority(0xC9, 0xC9);
            m_pMdl->OnFlag(0x2000);

            if (file.HasResAnmChr()) {
                m_pAnmChr = new (0x100) bcs::CAnmMotion;
                if (m_pAnmChr) {
                    m_pAnmChr->Create(m_pMdl, file, 0, 3, 0);
                    m_pMdl->AttachAnmObj(m_pAnmChr, bcs::CMdlObj::ANMOBJTYPE_CHR);
                }
            }
            if (file.HasResAnmClr()) {
                nw4r::g3d::ResAnmClr res = file.GetResAnmClr(0);
                m_pAnmClr = new (0x100) bcs::CAnmMatClr;
                if (m_pAnmClr) {
                    m_pAnmClr->Create(m_pMdl, res, 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmClr, bcs::CMdlObj::ANMOBJTYPE_MATCLR);
                }
            }
            if (file.HasResAnmTexSrt()) {
                nw4r::g3d::ResAnmTexSrt res = file.GetResAnmTexSrt(0);
                m_pAnmTexSrt = new (0x100) bcs::CAnmTexSrt;
                if (m_pAnmTexSrt) {
                    m_pAnmTexSrt->Create(m_pMdl, res, 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmTexSrt, bcs::CMdlObj::ANMOBJTYPE_TEXSRT);
                }
            }
            if (file.HasResAnmTexPat()) {
                nw4r::g3d::ResAnmTexPat res = file.GetResAnmTexPat(0);
                m_pAnmTexPat = new (0x100) bcs::CAnmTexPat;
                if (m_pAnmTexPat) {
                    m_pAnmTexPat->Create(m_pMdl, res, 0, 0);
                    m_pMdl->AttachAnmObj(m_pAnmTexPat, bcs::CMdlObj::ANMOBJTYPE_3);
                }
            }

            m_pMdl->CalcRootMtx();
            m_pMdl->CalcWorldMatrixAll(0);
            pGroup->AppendChild(m_pMdl);
        }

        // Each animation plays its start motion, then the loop, then (after
        // End) the end motion; once all have ended the task deletes itself.
        void CcVsBossSpCircleObj::Main()
        {
            if (m_pMdl) {
                if (m_pAnmChr) {
                    switch (m_anStep[0]) {
                        case 0: {
                            nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apSpCircleAnmName[m_nType][0]);
                            if (res.IsValid()) {
                                m_pAnmChr->Play(res, 0, 1.0f, 0.0f, 0.0f);
                            }
                            m_anStep[0]++;
                            break;
                        }
                        case 1:
                            if (m_pAnmChr->IsEnd()) {
                                nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apSpCircleAnmName[m_nType][1]);
                                if (res.IsValid()) {
                                    m_pAnmChr->Play(res, 2, 1.0f, 0.0f, 0.0f);
                                }
                                m_anStep[0]++;
                            }
                            break;
                        case 2:
                            if (m_bEnd) {
                                nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(s_apSpCircleAnmName[m_nType][2]);
                                if (res.IsValid()) {
                                    m_pAnmChr->Play(res, 0, 1.0f, 0.0f, 0.0f);
                                }
                                m_anStep[0]++;
                            }
                            break;
                        case 3:
                            if (m_pAnmChr->IsEnd()) {
                                m_anStep[0] = STEP_DONE;
                            }
                            break;
                    }
                } else {
                    m_anStep[0] = STEP_DONE;
                }

                if (m_pAnmClr) {
                    switch (m_anStep[1]) {
                        case 0: {
                            nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apSpCircleAnmName[m_nType][0]);
                            if (res.IsValid()) {
                                m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                            }
                            m_anStep[1]++;
                            break;
                        }
                        case 1:
                            if (m_pAnmClr->IsEnd()) {
                                nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apSpCircleAnmName[m_nType][1]);
                                if (res.IsValid()) {
                                    m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                                }
                                m_anStep[1]++;
                            }
                            break;
                        case 2:
                            if (m_bEnd) {
                                nw4r::g3d::ResAnmClr res = m_File.GetResAnmClr(s_apSpCircleAnmName[m_nType][2]);
                                if (res.IsValid()) {
                                    m_pAnmClr->ChangeRes(m_pMdl->GetResMdl(), res);
                                }
                                m_anStep[1]++;
                            }
                            break;
                        case 3:
                            if (m_pAnmClr->IsEnd()) {
                                m_anStep[1] = STEP_DONE;
                            }
                            break;
                    }
                } else {
                    m_anStep[1] = STEP_DONE;
                }

                if (m_pAnmTexSrt) {
                    switch (m_anStep[2]) {
                        case 0: {
                            nw4r::g3d::ResAnmTexSrt res = m_File.GetResAnmTexSrt(s_apSpCircleAnmName[m_nType][0]);
                            if (res.IsValid()) {
                                m_pAnmTexSrt->ChangeRes(m_pMdl->GetResMdl(), res);
                            }
                            m_anStep[2]++;
                            break;
                        }
                        case 1:
                            if (m_pAnmTexSrt->IsEnd()) {
                                nw4r::g3d::ResAnmTexSrt res = m_File.GetResAnmTexSrt(s_apSpCircleAnmName[m_nType][1]);
                                if (res.IsValid()) {
                                    m_pAnmTexSrt->ChangeRes(m_pMdl->GetResMdl(), res);
                                }
                                m_anStep[2]++;
                            }
                            break;
                        case 2:
                            if (m_bEnd) {
                                nw4r::g3d::ResAnmTexSrt res = m_File.GetResAnmTexSrt(s_apSpCircleAnmName[m_nType][2]);
                                if (res.IsValid()) {
                                    m_pAnmTexSrt->ChangeRes(m_pMdl->GetResMdl(), res);
                                }
                                m_anStep[2]++;
                            }
                            break;
                        case 3:
                            if (m_pAnmTexSrt->IsEnd()) {
                                m_anStep[2] = STEP_DONE;
                            }
                            break;
                    }
                } else {
                    m_anStep[2] = STEP_DONE;
                }

                // The texture pattern waits on the texture SRT animation's end
                // (as in the original).
                if (m_pAnmTexPat) {
                    switch (m_anStep[3]) {
                        case 0: {
                            nw4r::g3d::ResAnmTexPat res = m_File.GetResAnmTexPat(s_apSpCircleAnmName[m_nType][0]);
                            if (res.IsValid()) {
                                m_pAnmTexPat->ChangeRes(m_pMdl->GetResMdl(), res);
                            }
                            m_anStep[3]++;
                            break;
                        }
                        case 1:
                            if (m_pAnmTexSrt->IsEnd()) {
                                nw4r::g3d::ResAnmTexPat res = m_File.GetResAnmTexPat(s_apSpCircleAnmName[m_nType][1]);
                                if (res.IsValid()) {
                                    m_pAnmTexPat->ChangeRes(m_pMdl->GetResMdl(), res);
                                }
                                m_anStep[3]++;
                            }
                            break;
                        case 2:
                            if (m_bEnd) {
                                nw4r::g3d::ResAnmTexPat res = m_File.GetResAnmTexPat(s_apSpCircleAnmName[m_nType][2]);
                                if (res.IsValid()) {
                                    m_pAnmTexPat->ChangeRes(m_pMdl->GetResMdl(), res);
                                }
                                m_anStep[3]++;
                            }
                            break;
                        case 3:
                            if (m_pAnmTexSrt->IsEnd()) {
                                m_anStep[3] = STEP_DONE;
                            }
                            break;
                    }
                } else {
                    m_anStep[3] = STEP_DONE;
                }

                m_pMdl->UpdateFrame(GetFrameRateF());

                int bEnd = 1;
                for (long i = 0; i < 4; i++) {
                    if (m_anStep[i] < STEP_DONE) {
                        bEnd = 0;
                        break;
                    }
                }
                if (bEnd) {
                    Delete();
                }
            } else {
                Delete();
            }
        }

        CcVsBossSpCircle::CcVsBossSpCircle(bcs::CTask *pParent, const bcs::FVEC3 &rPos, long nChr)
            : CGameTask(pParent, -1), m_nStep(0), m_pGroup(0), m_nChr(nChr)
        {
            m_Pos = rPos;
            m_hObj = 0;
            for (long i = 0; i < EFFECT_NUM; i++) {
                m_ahEffect[i] = 0;
            }

            InitFsm(this, STATE_NUM);
            SetState(STATE_WAIT, &CcVsBossSpCircle::CtorStateWait, &CcVsBossSpCircle::LoopStateWait,
                     &CcVsBossSpCircle::DtorStateWait);
            SetState(STATE_APPEAR, &CcVsBossSpCircle::CtorStateAppear, &CcVsBossSpCircle::LoopStateAppear,
                     &CcVsBossSpCircle::DtorStateAppear);
            SetState(STATE_TURN, &CcVsBossSpCircle::CtorStateTurn, &CcVsBossSpCircle::LoopStateTurn,
                     &CcVsBossSpCircle::DtorStateTurn);
            SetState(STATE_LOOP, &CcVsBossSpCircle::CtorStateLoop, &CcVsBossSpCircle::LoopStateLoop,
                     &CcVsBossSpCircle::DtorStateLoop);
            SetState(STATE_DISAPPEAR, &CcVsBossSpCircle::CtorStateDisappear,
                     &CcVsBossSpCircle::LoopStateDisappear, &CcVsBossSpCircle::DtorStateDisappear);
            ChangeState(STATE_APPEAR);

            m_pGroup = new (0) bcs::CScnGroup(0x10, 0);
            m_pGroup->m_vTrans = m_Pos;
            m_pGroup->CalcRootMtx();
            bcs::CRenderMan::PushBack(m_pGroup, 5);
        }

        CcVsBossSpCircle::~CcVsBossSpCircle()
        {
            for (long i = 0; i < EFFECT_NUM; i++) {
                if (m_ahEffect[i]) {
                    CEffectCtrl::EraseAttachEffect(m_ahEffect[i]);
                    m_ahEffect[i] = 0;
                }
            }
            DeleteScnObj(m_pGroup);
        }

        void CcVsBossSpCircle::Main()
        {
            ExecState();
        }

        void CcVsBossSpCircle::CtorStateWait()
        {
        }

        void CcVsBossSpCircle::LoopStateWait()
        {
            if (!(CGame::g_GameWork.m_nFlag18 & 0x4306) && CGame::IsArtsEventMode()) {
                m_nChr = CGame::g_nEventChrIdx;
                ChangeState(STATE_APPEAR);
            }
        }

        void CcVsBossSpCircle::DtorStateWait()
        {
        }

        // The circle appears with its effect, turned a quarter round, ten
        // times its size (flat).
        void CcVsBossSpCircle::CtorStateAppear()
        {
            CGameSnd::Play(0xE08, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);

            bcs::EF::CResource *pRes = bcs::EF::CResource::GetInstance();
            if (pRes) {
                if (m_pGroup) {
                    m_pGroup->SetRotY(4.712389f);
                    m_pGroup->CalcRootMtx();
                }

                unsigned long hEffect = CEffectCtrl::CreateAttachEffect(0x55F, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
                if (hEffect) {
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                    if (pCluster) {
                        pCluster->SetTranslate(&m_Pos);
                        bcs::FVEC3 scale(10.0f, 10.0f, 10.0f);
                        bcs::FVEC3 rot(0.0f, 4.712389f, 0.0f);
                        pCluster->SetScale(&scale);
                        pCluster->SetRotateRad(&rot);
                    }
                }

                float fScaleY = 1.0f;
                float fScaleXZ = 10.0f;
                if (s_apSpCircleMdlName[0]) {
                    nw4r::g3d::ResFile file = pRes->GetG3DResFileByMdlName(s_apSpCircleMdlName[0]);
                    if (file.IsValid()) {
                        CcVsBossSpCircleObj *pObj = new (0x100) CcVsBossSpCircleObj(this, m_pGroup, 0, file);
                        bcs::CMdlObj *pMdl = pObj->GetMdl();
                        if (pMdl) {
                            pMdl->m_vScale.x = fScaleXZ;
                            pMdl->m_vScale.y = fScaleY;
                            pMdl->m_vScale.z = fScaleXZ;
                        }
                        m_hObj = pObj->GetId();
                    }
                }
                m_fWait = 90.0f;
            }
        }

        void CcVsBossSpCircle::LoopStateAppear()
        {
            m_fWait -= GetFrameRateF();
            if (m_fWait <= 0.0f) {
                ChangeState(STATE_TURN);
            }
        }

        void CcVsBossSpCircle::DtorStateAppear()
        {
        }

        void CcVsBossSpCircle::CtorStateTurn()
        {
            m_nStep = 0;
        }

        void CcVsBossSpCircle::LoopStateTurn()
        {
            if (m_pGroup) {
                m_pGroup->SetRotY(m_pGroup->m_fRotY + 0.007272206f);
                m_pGroup->CalcRootMtx();
            }
            ChangeState(STATE_LOOP);
        }

        void CcVsBossSpCircle::DtorStateTurn()
        {
        }

        void CcVsBossSpCircle::CtorStateLoop()
        {
            m_fWait = 30.0f;
            m_nStep = 0;
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(0x561, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
            if (hEffect) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                if (pCluster) {
                    pCluster->SetTranslate(&m_Pos);
                    bcs::FVEC3 scale(10.0f, 10.0f, 10.0f);
                    pCluster->SetScale(&scale);
                }
            }
        }

        // The circle turns; after 30 frames the two loop effects start. The
        // state ends when the character is gone or no longer in its trick.
        void CcVsBossSpCircle::LoopStateLoop()
        {
            if (m_pGroup) {
                m_pGroup->SetRotY(m_pGroup->m_fRotY + 0.007272206f);
                m_pGroup->CalcRootMtx();
            }

            switch (m_nStep) {
                case 0:
                    m_fWait -= GetFrameRateF();
                    if (m_fWait <= 0.0f) {
                        m_nStep++;
                    }
                    break;
                case 1: {
                    unsigned long hEffect = m_ahEffect[0] =
                        CEffectCtrl::CreateAttachEffect(0x562, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0);
                    if (hEffect) {
                        CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                        if (pCluster) {
                            pCluster->SetTranslate(&m_Pos);
                            bcs::FVEC3 scale(10.0f, 10.0f, 10.0f);
                            pCluster->SetScale(&scale);
                        }
                    }
                    hEffect = m_ahEffect[1] = CEffectCtrl::CreateAttachEffect(0x563, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0);
                    if (hEffect) {
                        CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                        if (pCluster) {
                            pCluster->SetTranslate(&m_Pos);
                            bcs::FVEC3 scale(10.0f, 40.0f, 10.0f);
                            pCluster->SetScale(&scale);
                            pCluster->CallEffectFunc(&bcs::EF::CEffect::SetFrameRateCalcDisable, 1);
                        }
                    }
                    m_nStep++;
                    break;
                }
            }

            int bEnd = 1;
            if (m_nChr >= 0) {
                CChrCore *pChr = CChrMan::GetChrCore(m_nChr);
                if (pChr && pChr->IsEnable() && pChr->GetState() == CChrCore::STATE_AIR_TRICK) {
                    bEnd = 0;
                }
            }
            if (bEnd) {
                ChangeState(STATE_DISAPPEAR);
            }
        }

        void CcVsBossSpCircle::DtorStateLoop()
        {
        }

        // The vanishing effect; the loop effects stop and the model plays
        // its end motion.
        void CcVsBossSpCircle::CtorStateDisappear()
        {
            unsigned long hEffect = CEffectCtrl::CreateAttachEffect(0x560, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
            if (hEffect) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                if (pCluster) {
                    pCluster->SetTranslate(&m_Pos);
                    bcs::FVEC3 scale(10.0f, 10.0f, 10.0f);
                    pCluster->SetScale(&scale);
                }
            }
            if (m_ahEffect[0]) {
                CEffectCtrl::StopAttachEffect(m_ahEffect[0]);
                m_ahEffect[0] = 0;
            }
            if (m_ahEffect[1]) {
                CEffectCtrl::StopAttachEffectForce(m_ahEffect[1]);
                m_ahEffect[1] = 0;
            }
            CcVsBossSpCircleObj *pObj = static_cast<CcVsBossSpCircleObj *>(bcs::CTaskMan::GetTask(m_hObj));
            if (pObj && !pObj->IsDelete()) {
                pObj->End();
            }
            m_nStep = 0;
        }

        // A few frames, then the task ends once the model has.
        void CcVsBossSpCircle::LoopStateDisappear()
        {
            int bEnd = 0;
            switch (m_nStep) {
                case 0:
                case 1:
                case 2:
                    m_nStep++;
                    break;
                default: {
                    bEnd = 1;
                    bcs::CTask *pObj = bcs::CTaskMan::GetTask(m_hObj);
                    if (pObj && !pObj->IsDelete()) {
                        bEnd = 0;
                    }
                    break;
                }
            }
            if (bEnd) {
                Delete();
            }
        }

        void CcVsBossSpCircle::DtorStateDisappear()
        {
        }
    }

    void CGameBoss::SetDispRadarIcon_(int bDisp)
    {
        CCockpitBoss::SetRadarIconDisp(bDisp);
    }

    void CGameBoss::SetCockpitPartMove_(long nPart, long nMove)
    {
        CCockpitBoss::SetPartMove(nPart, nMove);
    }

    void CGameBoss::SetCockpitDisp_(int bDisp)
    {
        CCockpitBoss::SetDisp(bDisp);
    }

    void CGameBoss::SetCockpitCoinDisp_(int bDisp)
    {
        CCockpitBoss::SetCoinDispEnable(bDisp);
    }

    void CGameBoss::SetCockpitTimeDisp_(int)
    {
    }

    void CGameBoss::SetCockpitScoreDisp_(int)
    {
    }

    void CGameBoss::SetCockpitSetCountDisp_(int)
    {
    }

    // The gauges at the start: the life gauges of the characters in play
    // (in the three-player layout from three of them on), full.
    void CGameBoss::SetCockpitSetting_()
    {
        CCockpitBoss::SetSPGaugeRatio(0, 0.0f, 0.0f);

        long nLayout = 0;
        long nNum = CChrMan::GetChrCoreNum();
        long nChrNum = 0;
        for (long i = 0; i < nNum; i++) {
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr && !pChr->IsDisable()) {
                nChrNum++;
            }
        }
        if (nChrNum >= 3) {
            nLayout = 1;
        }
        CCockpitBoss::SetLifeGaugeLayout(nLayout);

        nNum = CChrMan::GetChrCoreNum();
        long nGauge = 0;
        for (long i = 0; i < nNum; i++) {
            m_abOutTelop[i] = 0;
            CChrCore *pChr = CChrMan::GetChrCore(i);
            if (pChr == 0 || pChr->IsDisable()) {
                continue;
            }
            CCockpitBoss::SetLifeGaugeState(nGauge, 0);
            CCockpitBoss::SetLifePercent(nGauge, 1.0f, 1.0f);
            CCockpitBoss::SetOutTelopState(nGauge, 0);
            m_abOutTelop[nGauge] = 0;
            nGauge++;
        }

        long nLevel = GetPartyGameParam(0);
        long nIcon = 0;
        if (nLevel >= 2) {
            nIcon = 1;
        }
        CCockpitBoss::SetBossIcon(nIcon);
        m_bSPController = 0;
    }

    void CGameBoss::CheckDispRemoconShake_()
    {
    }

    void CGameBoss::EnableDispRemoconShake_(long)
    {
    }

    void CGameBoss::DisableDispRemoconShake_()
    {
    }

    void CGameBoss::SendMessage_(long nMsg, long nArg1, long nArg2, long nArg3)
    {
        switch (nMsg) {
            case 3:
                CCockpitBoss::SetPartMove(0, 3);
                SetDispRadarIcon_(0);
                break;
            case 0x12:
                m_nUnk160 = 0;
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
            case 0x63:
                switch (nArg1) {
                    case 1:
                        if (nArg2 >= 0 && nArg2 < CChrMan::GetChrCoreNum()) {
                            CChrCore *pChr = CChrMan::GetChrCore(nArg2);
                            // (as in the original: the member function itself is
                            // tested, which is always true, and the result of
                            // IsPadCtrl is not used)
                            if (&CChrCore::IsEnable) {
                                pChr->IsPadCtrl();
                            }
                        }
                        break;
                    case 2: {
                        // The boss's life: nArg2 of nArg3.
                        float fMax = nArg3;
                        float fLife = nArg2;
                        if (fMax > 0.0f) {
                            m_fBossLife = fLife / fMax;
                            m_fBossLife = Clamp(m_fBossLife, 0.0f, 1.0f);
                        }
                        break;
                    }
                    case 3:
                        // The boss is beaten.
                        m_fBossLife = 0.0f;
                        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
                        m_nGameFlag |= 3;
                        break;
                    case 5:
                        if (m_pArtsCam) {
                            m_pArtsCam->StartQuake(0x46);
                        }
                        bcs::CMainApp::StartMasterFade(1, 1.0f / 60.0f);
                        SetGamePauseEnable_(0);
                        m_fPauseWait = 9999.0f;
                        break;
                    case 6:
                        ArtsSetCam_(nArg2);
                        break;
                    case 7:
                        ArtsEndCam_();
                        if (!(g_GameWork.m_nFlag18 & 0x4306) && IsMainLoopState(1)) {
                            CameraReset_();
                            bcs::CMainApp::StartMasterFade(4, 1.0f / 60.0f);
                            SetGamePauseEnable_(0);
                            m_fPauseWait = 60.0f;
                        }
                        break;
                }
                break;
        }
        CGame::SendMessage_(nMsg, nArg1, nArg2, nArg3);
    }

    void CGameBoss::CtorStateGameInit()
    {
        SetTurnStartTelopType_(1, 75.0f);
        g_GameWork.m_nFlag18 = 0;
        SetFirstGoalIdx_();
        GameInit_();
        SetPreparationMode(1);
        SetCockpitSetting_();
        m_nStateStep = 0;
    }

    void CGameBoss::LoopStateEntranceDemo()
    {
        if (m_nStateStep == 1) {
            SetCockpitPartMove_(0, 3);
            m_bPartMove = 1;
        }
        CGame::LoopStateEntranceDemo();
    }

    void CGameBoss::CtorStateGameSetup()
    {
        g_GameWork.m_nFlag18 = 0;
        GameSetUp_();
        SetPreparationMode(1);
        SetCockpitSetting_();
        CCockpitBoss::SetDisp(1);
        SetDispRadarIcon_(0);
        CCockpitBoss::SetBallPowerValue(0, 0);
        CCockpitBoss::SetBallPowerValueColor(0, 0);
        SetCockpitCoinDisp_(0);
        SetCockpitScoreDisp_(1);

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
        m_nStateStep = 0;
    }

    void CGameBoss::LoopStateGameSetup()
    {
        if (CEventDemo::IsEnable() && m_nUnk104 > 0) {
            EVENT_DEMO_WORK *pWork = static_cast<EVENT_DEMO_WORK *>(FsmqPushState(STATE_EVENT_DEMO));
            pWork->nFlag |= 1;
            pWork->nUnk08 = 4;
            pWork->nUnk0C = m_nUnk108;
            pWork->nUnk10 = 1;
            pWork->nUnk18 = 3;
            pWork->fUnk1C = 30.0f;
            m_nUnk104 = 0;
        }
        FsmqPush(STATE_WAIT_START);
        FsmqNext();
        SetCockpitPartMove_(0, 3);
    }

    void CGameBoss::DtorStateGameSetup()
    {
    }

    void CGameBoss::CtorStateWaitStart()
    {
        ScrnFadeStart_(2, 4, 0.0625f);
        ScrnFadeStart_(1, 4, 0.0625f);
        SetReadyDemoMode(1);
        SetPreparationMode(0);
        m_nGameFlag = 0;
        m_fStateCount = 0.0f;
        CCockpitBoss::SetDisp(1);
        if (g_GameWork.m_nUnk40 & 8) {
            SetCockpitArtsGaugeDisp_(0);
        } else {
            SetCockpitArtsGaugeDisp_(1);
        }
        if (g_GameWork.m_nUnk40 & 1) {
            CCockpitBoss::SetCoinDispEnable(0);
        } else {
            CCockpitBoss::SetCoinDispEnable(1);
        }
        CChrMan::SetChrMarkerDisp(1);
        CChrMan::ResetEventDoll();
        SetDispRadarIcon_(1);
        m_fUnk158 = 120.0f;
        m_nStateStep = 0;

        if (g_GameWork.m_nGameType == 0) {
            ChrPosSetNormal_(1);
            SetAllDefenseLeaderFirstState();
        }
    }

    // The ready telop and the count-down, then the start.
    void CGameBoss::LoopStateWaitStart()
    {
        switch (m_nStateStep) {
            case 0:
                if (IsScrnFadeEnd_(3)) {
                    SetCockpitPartMove_(0, 0);
                    SetCockpitItemIconDisp_(1, 1);
                    m_bPartMove = 0;
                    TelopCreate_(TELOP_READY, 0, 0);
                    m_fStateCount = 180.0f;
                    m_fBossLife = 1.0f;
                    m_nCount = 0;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (m_fStateCount < 24.0f) {
                    TelopEnd_(TELOP_READY);
                    m_nStateStep++;
                }
                // fall through
            default:
                if (StateWorkDecStateCount_()) {
                    m_fBossLife = 1.0f;
                    m_nCount = 12;
                    TelopCreate_(-1, 1, 0);
                    CGameSnd::Play(0xBF4, 0, 1, -1, 0.0f, 1.0f, 0.0f);
                    FsmqChangeState(m_nNextState);
                    SendMessage_(0x10, 0, 0, 0);
                } else {
                    m_nCount = (180.0f - m_fStateCount) / 15.0f;
                    m_nCount = m_nCount > 12 ? 12 : (m_nCount < 0 ? 0 : m_nCount);
                }
                break;
        }
    }

    void CGameBoss::DtorStateWaitStart()
    {
    }

    void CGameBoss::CtorStateMainLoop()
    {
        SetReadyDemoMode(0);
        SetPreparationMode(0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(0), static_cast<CChrCore::STOP_MODE>(0));
        CChrMan::AllSendMsg(static_cast<CChrDrv::MSG_ID>(4), 0, 0, 0, 0);
        SendMessage_(0x16, 0, 0, 0);
        m_nGameFlag &= ~1;
        m_fPauseWait = 0.0f;
        SetGamePauseEnable_(1);
    }

    // Play ends when every character is beaten (or the boss is: message
    // 0x63/3); pausing comes back once m_fPauseWait has run out.
    void CGameBoss::LoopStateMainLoop()
    {
        if (IsAllChrDead()) {
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
            CChrMan::AllSetState(1, -1, 0);
            m_nGameFlag |= 3;
        }

        if ((m_nGameFlag & 1) && CChrMan::CanbeTurnOver()) {
            m_nGameFlag |= 8;
            if (m_nGameFlag & 4) {
                FsmqChangeState(STATE_TIME_UP);
            } else {
                FsmqChangeState(STATE_TURN_RESULT);
            }
            CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        }

        if (m_fPauseWait > 0.0f) {
            m_fPauseWait -= GetFrameRateF();
            if (m_fPauseWait <= 0.0f) {
                m_fPauseWait = 0.0f;
                SetGamePauseEnable_(1);
            }
        }
    }

    void CGameBoss::DtorStateMainLoop()
    {
    }

    void CGameBoss::CheckTimeUp_()
    {
        if (m_nGameFlag & 2) {
            return;
        }
        if (IsReqArtsEvent_()) {
            return;
        }
        if (g_GameWork.m_fUnk20 > 0.0f) {
            float fTime = g_GameWork.m_fGameTime;
            if (fTime <= 0.0f) {
                m_nGameFlag |= 5;
            } else {
                SetTime(fTime - GetFrameRateF());
            }
        }
    }

    void CGameBoss::CtorStateTurnResult()
    {
        SetReplayMode(0);
        SetResultMode(1);
        m_fStateCount = 0.0f;
        SetCockpitPartMove_(0, 2);
        SetDispRadarIcon_(0);
        SetCockpitItemIconDisp_(0, 1);
        m_bPartMove = 1;
        m_nStateStep = 0;
    }

    // With every character beaten, slow motion and the game is set;
    // otherwise (the boss is beaten) a longer slow motion, a fade, and the
    // result.
    void CGameBoss::LoopStateTurnResult()
    {
        if (m_nStateStep == 0) {
            if (StateWorkDecStateCount_()) {
                if (IsAllChrDead()) {
                    StartSlowEffect_(0.05f, 50.0f, 16.0f, 30.0f);
                    FsmqChangeState(STATE_GAME_SET);
                } else {
                    StartSlowEffect_(0.05f, 200.0f, 16.0f, 30.0f);
                    ScrnFadeStart_(2, 1, 1.0f / 220.0f);
                    m_nStateStep++;
                }
            }
        } else if (IsScrnFadeEnd_(3)) {
            StopSlowEffect_();
            BgSetFadeIn(1.0f, 30.0f);
            SetPreparationMode(1);
            FsmqChangeState(STATE_GAME_RESULT);
        }
    }

    void CGameBoss::DtorStateTurnResult()
    {
    }

    void CGameBoss::CtorStateTimeUp()
    {
        CGameSnd::Play(0xBF5, 0, 1, -1, 0.0f, 1.0f, 0.0f);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        SetResultMode(1);
        TelopCreate_(TELOP_TIME_UP, 0, 0);
        m_fStateCount = 30.0f;
        m_nStateStep = 0;
    }

    void CGameBoss::LoopStateTimeUp()
    {
        switch (m_nStateStep) {
            case 0:
                if (StateWorkDecStateCount_()) {
                    CChrMan::AllSetState(1, -1, 0);
                    m_fStateCount = 90.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_TIME_UP);
                    m_nStateStep++;
                }
                break;
            default:
                if (TelopIsEnd_(TELOP_TIME_UP)) {
                    FsmqChangeState(STATE_GAME_SET);
                }
                break;
        }
    }

    void CGameBoss::DtorStateTimeUp()
    {
        TelopRemove_(TELOP_TIME_UP);
    }

    void CGameBoss::CtorStateTurnOver()
    {
    }

    void CGameBoss::LoopStateTurnOver()
    {
        SetPreparationMode(1);
        FsmqChangeState(STATE_CHG_OFFENSE);
    }

    void CGameBoss::DtorStateTurnOver()
    {
    }

    void CGameBoss::CtorStateGameSet()
    {
        if (!m_bPartMove) {
            SetCockpitPartMove_(0, 2);
            m_bPartMove = 1;
        }
        TelopCreate_(TELOP_GAME_SET, 0, 0);
        m_fStateCount = 330.0f;
        m_nStateStep = 1;
        SetGameResultAll_(2);
        SetTeamResult(1, 1);
        SetTeamResult(0, 2);
    }

    void CGameBoss::LoopStateGameSet()
    {
        switch (m_nStateStep) {
            case 0:
                if (TelopIsWait_(TELOP_GAME_SET)) {
                    m_fStateCount = 210.0f;
                    m_nStateStep++;
                }
                break;
            case 1:
                if (StateWorkDecStateCount_()) {
                    TelopEnd_(TELOP_GAME_SET);
                    m_nStateStep++;
                }
                break;
            default:
                if (TelopIsEnd_(TELOP_GAME_SET)) {
                    FsmqChangeState(STATE_END_MENU);
                }
                break;
        }
    }

    void CGameBoss::DtorStateGameSet()
    {
        TelopRemove_(TELOP_GAME_SET);
    }

    void CGameBoss::CtorStateGameResult()
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
        SetPeriodResult_();
        SetGameResult_();
        CalcAddSkillPoint_();
        SendMessage_(0x11, 0, 0, 0);
        Unlock::CheckClear();
        m_nStateStep = 0;
    }

    // The result demo with its music, then the result or end menu.
    void CGameBoss::LoopStateGameResult()
    {
        switch (m_nStateStep) {
            case 0:
                bcs::CRenderMan::ReqPushViewSplitType(static_cast<bcs::CWorldEnv::ViewSplitType>(0), 0);
                m_nStateStep++;
                break;
            case 1:
                CChrMan::AllStockItemDelete();
                CChrMan::AllReset();
                EventDemoSetUp();
                CGame::SendMessage(5, 0, 0, 0);
                CEventDemo::CreateDemo(1, this, -1.0f, -1.0f);
                m_nStateStep++;
                break;
            case 2:
                if (IsScrnFadeEnd_(3)) {
                    CEventDemo::SetFlags(2);
                    if (GetTeamResult(g_GameWork.m_nUnk00) == 1) {
                        CGameBGM::SetBgmState(0xB);
                    } else {
                        PlayGameResultBGM_();
                    }
                    m_nStateStep++;
                }
                break;
            case 3:
                if (CEventDemo::m_Work.bSkip) {
                    if (CGameBGM::GetBgmState() == 3 || GetTeamResult(g_GameWork.m_nUnk00) == 1) {
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

    void CGameBoss::DtorStateGameResult()
    {
        CGame::SendMessage(6, 0, 0, 0);
    }

    void CGameBoss::CalcAddSkillPoint_()
    {
    }

    void CGameBoss::SetMissionResult_()
    {
        if (CTournament::IsExtraMatchGame() || m_Unk130 > 1) {
            SetGameResultAll_(2);
            if (CTournament::IsMissionClear()) {
                SetTeamResult(0, 1);
            } else {
                SetTeamResult(1, 1);
            }
        }
    }

    int CGameBoss::SetGameResultNext_()
    {
        if (m_Unk130 == 2) {
            FsmqChangeState(STATE_END_MENU);
        } else {
            FsmqChangeState(STATE_RESULT_MENU);
        }
        return 0;
    }

    // The result screen: each team's score per period and who won it.
    int CGameBoss::MenuCallGameResult_()
    {
        CCockpitBoss::SetMenuState(4);
        long nPeriodNum = g_GameWork.m_Unk0B;
        long anScore0[5] = {0, 0, 0, 0, 0};
        long anScore1[5] = {0, 0, 0, 0, 0};
        long anWin0[5] = {0, 0, 0, 0, 0};
        long anWin1[5] = {0, 0, 0, 0, 0};
        long anUnk4[5] = {0, 0, 0, 0, 0};
        long anUnk5[5] = {0, 0, 0, 0, 0};
        for (long i = 0; i < nPeriodNum; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriodNum, nPeriodNum, anScore0, anScore1, anWin0, anWin1, anUnk4, anUnk5, -1);
        return 1;
    }

    // The same between periods, up to the period being played.
    int CGameBoss::MenuCallInterval_()
    {
        CCockpitBoss::SetMenuState(5);
        long nPeriod = g_GameWork.m_nPeriod;
        long nPeriodNum = g_GameWork.m_Unk0B;
        long anScore0[5] = {0, 0, 0, 0, 0};
        long anScore1[5] = {0, 0, 0, 0, 0};
        long anWin0[5] = {0, 0, 0, 0, 0};
        long anWin1[5] = {0, 0, 0, 0, 0};
        long anUnk4[5] = {0, 0, 0, 0, 0};
        long anUnk5[5] = {0, 0, 0, 0, 0};
        for (long i = 0; i < nPeriod; i++) {
            anScore0[i] = GetTeamPeriodScore(0, i);
            anScore1[i] = GetTeamPeriodScore(1, i);
            if (anScore0[i] > anScore1[i]) {
                anWin0[i] = 1;
            } else if (anScore0[i] < anScore1[i]) {
                anWin1[i] = 1;
            }
        }
        CCockpit::SetResultParameter(nPeriod, nPeriodNum, anScore0, anScore1, anWin0, anWin1, anUnk4, anUnk5, -1);
        return 1;
    }

    int CGameBoss::MenuCallPause_()
    {
        CCockpitBoss::SetMenuState(3);
        return 1;
    }

    void CGameBoss::SetCockpitArtsGaugeRatio_(long nTeam, float fRatio, float fTelop)
    {
        CCockpitBoss::SetSPGaugeRatio(nTeam, fRatio, fTelop);
    }

    void CGameBoss::SetCockpitArtsGaugeSpAnim_(long nTeam, int bSpecial)
    {
        CCockpitBoss::SetCoinSpecialAnim(nTeam, bSpecial);
    }

    void CGameBoss::PrepareRestartGame_()
    {
        CCockpitBoss::SetMenuState(1);
        ScrnSetBlackOut_();
        g_GameWork.m_nFlag18 = 0;
        DeleteTask(m_pArtsCam);
    }

    long CGameBoss::GetMaxCoin_() const
    {
        return 100;
    }

    void CGameBoss::PowerUpTelopIn_()
    {
        CCockpitBoss::SetPowerUpTelopState(0, 0);
        m_fPowerUpTelop = 90.0f;
    }

    void CGameBoss::UpdatePowerUpTelop_(float fFrame)
    {
        if (fFrame > 0.0f && m_fPowerUpTelop > 0.0f) {
            m_fPowerUpTelop -= fFrame;
            if (m_fPowerUpTelop <= 0.0f) {
                CCockpitBoss::SetPowerUpTelopState(0, 2);
                m_fPowerUpTelop = 0.0f;
            }
        }
    }

    int CGameBoss::IsSetOver_()
    {
        return 1;
    }

    int CGameBoss::IsGameSet_()
    {
        return IsSetOver_() != 0;
    }

    void CGameBoss::ChangeGoalIdx_()
    {
        SetGoalIdx(0, 0);
        SetGoalIdx(1, 1);
    }

    void CGameBoss::SetGameResult_()
    {
        ClearGameResult_();
        SetTeamResult(1, 2);
        SetTeamResult(0, 1);
    }

    long CGameBoss::GetFirstGoalIdxMode_()
    {
        return 0;
    }

    void CGameBoss::EventDemoSetUp()
    {
        CChrMan::AllSetState(4, -1, 0);
        CChrMan::AllStop(static_cast<CChrCore::STOP_MODE>(2), static_cast<CChrCore::STOP_MODE>(2));
        CChrMan::SetChrMarkerDisp(0);
        SetDispRadarIcon_(0);
    }

    // Coins power up the ball: the cockpit shows five power points a coin,
    // the full gauge from GetMaxCoin_ coins on.
    void CGameBoss::AddTeamCoin_(long nTeam, long nCoin)
    {
        if (nTeam >= 0 && !(g_GameWork.m_nUnk40 & 1)) {
            long nPrev = GetTeamCoin(nTeam);
            CGame::AddTeamCoin_(nTeam, nCoin);
            long nNow = GetTeamCoin(nTeam);
            if (nNow > nPrev && nNow >= GetMaxCoin_()) {
                CGameSnd::Play(0xC11, 0, 1, -1, 0.0f, 1.0f, 0.0f);
            }
            SetBallPower(nTeam, GetTeamCoin(nTeam));

            if (nNow > nPrev) {
                if (nNow >= GetMaxCoin_()) {
                    CCockpitBoss::SetBallPowerState(nTeam, 2);
                } else {
                    CCockpitBoss::SetBallPowerState(nTeam, 1);
                }
                PowerUpTelopIn_();
            } else if (nNow >= GetMaxCoin_()) {
                CCockpitBoss::SetBallPowerState(nTeam, 2);
            } else if (nNow < nPrev) {
                CCockpitBoss::SetBallPowerState(nTeam, 0);
            }
            CCockpitBoss::SetBallPowerValueColor(nTeam, nNow >= 5);

            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall && pBall->IsActive()) {
                pBall->SetVsBossCoinPower(GetTeamCoin(nTeam));
            }
        }
    }

    // Ends the game with the characters in state nState, unless it is
    // already ending (from the main loop only).
    void CGameBoss::ForceGameExit_(long nState)
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

    void CGameBoss::GameInit_()
    {
        m_nGameFlag = 0;
        GameWorkReset_();
        CChrMan::InitLeader(1);
        SendMessage_(0xF, 0, 0, 0);
        SendMessage_(0x12, 0, 0, 0);
        EventDemoSetUp();
        m_nUnk104 = 1;
        m_nUnk108 = 0;
        m_nUnk12C = 0;
        m_Unk130 = 0;
        m_nUnk160 = 0;
        m_nUnk11C = 0;
    }

    void CGameBoss::GameSetUp_()
    {
        m_nGameFlag = 0;
        m_nUnk154 = -1;
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
        CChrMan::SetChrMarkerDisp(1);
        ClearLastGoalInfo();
        g_GameWork.m_fUnk38 = 0.0f;
        ClearCoinHistory();
        SetOffenseTeam(0);
        ChrPosSetNormal_(0);
        SetAllDefenseLeaderFirstState();
        SetLooseBallModeFlag(0);
        CameraReset_();
        SetCockpitFaceIcon_();
        CChrMan::AllSendReset();
    }

    // The characters at their start places, two to a place; a team's
    // leader (status bit 0x100) takes the first place.
    void CGameBoss::ChrPosSetNormal_(int)
    {
        long anIdx[6];
        for (long i = 0; i < 6; i++) {
            anIdx[i] = i;
        }
        long nNum = CChrMan::GetChrCoreNum();
        for (long i = 0; i < 2; i++) {
            if (nNum > 4) {
                if (CChrMan::GetChrCore(i + 4)->GetStatusFlag() & 0x100) {
                    anIdx[i] = i + 4;
                    anIdx[i + 2] = i + 2;
                    anIdx[i + 4] = i;
                } else if (CChrMan::GetChrCore(i + 2)->GetStatusFlag() & 0x100) {
                    anIdx[i] = i + 2;
                    anIdx[i + 2] = i;
                    anIdx[i + 4] = i + 4;
                } else {
                    anIdx[i] = i;
                    anIdx[i + 2] = i + 4;
                    anIdx[i + 4] = i + 2;
                }
            } else if (nNum > 2) {
                if (CChrMan::GetChrCore(i + 2)->GetStatusFlag() & 0x100) {
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
            const bcs::FVEC3 &rPos = s_aChrPosNormal[i >> 1];
            float x = rPos.x;
            float fRot = rPos.y;
            float z = rPos.z;
            if (GetGoalIdx(pChr->GetTeamNo())) {
                x *= -1.0f;
                z *= -1.0f;
                fRot = WrapRad(3.1415927f + fRot);
            }
            bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(GetTargetGoalIdx(pChr->GetTeamNo()));
            x += ofs.x;
            z += ofs.z;
            pChr->SetRotY(fRot);
            pChr->SetPos(bcs::FVEC3(x, 0.0f, z));
            pChr->RecalcAllMatrix();
        }
    }

    void CGameBoss::ChrPosSetResult_()
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
