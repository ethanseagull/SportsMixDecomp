// The dodge event demo: which demos dodge plays, the telops (cockpit
// objects) they show, and the damage demos (a replay of each hit, with its
// camera, slow motion, life gauge and "out" telop run by small timer
// tasks). One file with msw::EvDemoDodge and the weak CEventDemoBase::Disp
// the vtable needs.
#include "game/msw/CEventDemoDodge.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitDodge.hpp"
#include "game/msw/CEventDemo.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageDodge.hpp"
#include "game/msw/EvDemo.hpp"

// The C library is not in include/ yet.
extern "C" int strcmp(const char *, const char *);

namespace msw {

    // (name guessed) The handle of a task just made, 0 when `new` failed.
    static inline unsigned long GetTaskHandle(bcs::CTask *pTask)
    {
        return pTask != 0 ? pTask->GetId() : 0;
    }

    // (name guessed) Shows the "out" telop of character nChr: object 0x1E
    // or 0x1D by the colour of its team.
    static inline void CreateOutTelop(long nChr)
    {
        if (nChr > -1) {
            long nColor = CGame::GetTeamColorIdx(nChr & 1);
            long nObjId = 0x1E;
            if (nColor == 0) {
                nObjId = 0x1D;
            }
            CCockpit::RemoveObj(nObjId);
            CCockpitDodge::CreateObj(nObjId, -1);
            CChrCore *pChr = CChrMan::GetChrCore(nChr);
            if (pChr != 0) {
                CCockpitDodge::SetOutSign_ChrNo(nObjId, nChr, pChr->GetTeamNo());
            }
            CCockpit::SetAnimEndClose(nObjId, 1);
        }
    }

    // The damage demo's timer tasks. Each waits m_fWait frames (counted
    // down by GetFrameRateF) and then does its one thing; with no wait it
    // does it at once from the constructor. Declared in the reverse order of
    // their vtables in .data.

    // Shows the "out" telop of a character.
    class CTelopOut : public bcs::CTask {
        public:
            CTelopOut(bcs::CTask *pParent, float fWait, long nChr) : bcs::CTask(pParent, -1)
            {
                m_fWait = fWait;
                m_nChr = nChr;
                ClearUnk08(4);
                if (m_fWait <= 0.0f) {
                    Exec();
                }
            }
            // (name guessed) Makes the task under the running game and
            // returns its handle.
            static unsigned long Create(float fWait, long nChr)
            {
                bcs::CTask *pParent = CGame::m_pInstance;
                return GetTaskHandle(new CTelopOut(pParent, fWait, nChr));
            }
            virtual ~CTelopOut();
            virtual void Main();

        private:
            void Exec() { CreateOutTelop(m_nChr); }

            float m_fWait; // 0x34
            long m_nChr;   // 0x38
    };

    // Shows telop 0x1058 (a voice telop, name guessed).
    class CTelopVoice : public bcs::CTask {
        public:
            CTelopVoice(bcs::CTask *pParent, float fWait) : bcs::CTask(pParent, -1)
            {
                m_fWait = fWait;
                ClearUnk08(4);
                if (m_fWait <= 0.0f) {
                    Exec();
                }
            }
            // (name guessed) As CTelopOut::Create.
            static unsigned long Create(float fWait)
            {
                bcs::CTask *pParent = CGame::m_pInstance;
                return GetTaskHandle(new CTelopVoice(pParent, fWait));
            }
            virtual ~CTelopVoice();
            virtual void Main();

        private:
            void Exec()
            {
                CCockpit::RemoveObj(0x1058);
                CCockpitDodge::CreateObj(0x1058, -1);
                CCockpit::SetAnimEndClose(0x1058, 1);
            }

            float m_fWait; // 0x34
    };

    // Shows the current hit on the life gauge.
    class CLifeGaugeTask : public bcs::CTask {
        public:
            CLifeGaugeTask(bcs::CTask *pParent, float fWait) : bcs::CTask(pParent, -1)
            {
                m_fWait = fWait;
                ClearUnk08(4);
                if (m_fWait <= 0.0f) {
                    Exec();
                }
            }
            // (name guessed) As CTelopOut::Create.
            static unsigned long Create(float fWait)
            {
                bcs::CTask *pParent = CGame::m_pInstance;
                return GetTaskHandle(new CLifeGaugeTask(pParent, fWait));
            }
            virtual ~CLifeGaugeTask();
            virtual void Main();

        private:
            void Exec() { CEventDemoDodge::PopDamageLife(0); }

            float m_fWait; // 0x34
    };

    // Plays the demo camera.
    class CStartEffect : public bcs::CTask {
        public:
            CStartEffect(bcs::CTask *pParent, float fWait) : bcs::CTask(pParent, -1)
            {
                m_fWait = fWait;
                if (m_fWait <= 0.0f) {
                    Exec();
                }
            }
            // (name guessed) As CTelopOut::Create.
            static unsigned long Create(float fWait)
            {
                bcs::CTask *pParent = CGame::m_pInstance;
                return GetTaskHandle(new CStartEffect(pParent, fWait));
            }
            virtual ~CStartEffect();
            virtual void Main();

        private:
            void Exec() { CEventDemo::PlayDemoCamera(0); }

            float m_fWait; // 0x34
    };

    // Starts the slow motion; stops it when deleted. The four values go to
    // CGame::StartSlowEffect (meanings not known).
    class CSlowEffect : public bcs::CTask {
        public:
            CSlowEffect(bcs::CTask *pParent, float fWait, float f0, float f1, float f2, float f3)
                : bcs::CTask(pParent, -1)
            {
                m_fWait = fWait;
                m_fParam[0] = f0;
                m_fParam[1] = f1;
                m_fParam[2] = f2;
                m_fParam[3] = f3;
                if (m_fWait <= 0.0f) {
                    Exec();
                }
            }
            // (name guessed) As CTelopOut::Create.
            static unsigned long Create(float fWait, float f0, float f1, float f2, float f3)
            {
                bcs::CTask *pParent = CGame::m_pInstance;
                return GetTaskHandle(new CSlowEffect(pParent, fWait, f0, f1, f2, f3));
            }
            virtual ~CSlowEffect();
            virtual void Main();

        private:
            void Exec() { CGame::StartSlowEffect(m_fParam[0], m_fParam[1], m_fParam[2], m_fParam[3]); }

            float m_fWait;     // 0x34
            float m_fParam[4]; // 0x38
    };
}

namespace {
    // What each demo gets from CreateDemo. What the halfwords mean is not
    // known; the trailing zeros may be padding.
    const short sVictory0[8] = { 1, 0x8E, 0x8F, 0x90, 0x91, 0x90, 0x91, 0 };
    const short sVictory1[3] = { 0x106E, 0x106B, 0x1071 };
    const short sVictory2[3] = { 0x69, 0x6A, 0x6A };
    const short sVictory3[3] = { -1, -1, -1 };
    const msw::VICTORY_DATA sVictory = { { sVictory0, sVictory1, sVictory2, sVictory3 } };
    const short sEntrance[8] = { 1, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0 };
    const short sReadyStart[2] = { 1, 1 };
    const short sChangeCourt[3] = { 1, 0x10, 0x11 };
}

unsigned long msw::CEventDemoDodge::m_hDamageLifeGauge;
unsigned long msw::CEventDemoDodge::m_hDamageVoice;
unsigned long msw::CEventDemoDodge::m_hDamageTelop;
unsigned long msw::CEventDemoDodge::m_hDamageSpeed;
unsigned long msw::CEventDemoDodge::m_hDamageCamera;
long msw::CEventDemoDodge::m_nChangeCount;
long msw::CEventDemoDodge::m_nDamageCount;
long msw::CEventDemoDodge::m_nDamageIndexNow = -1;

msw::CEventDemoDodge::CEventDemoDodge(bcs::CTask *pParent) : CEventDemoBase(pParent)
{
    for (int i = 0; i < TELOP_MAX; i++) {
        m_nTelop[i] = -1;
    }
    m_hDamageLifeGauge = 0;
    m_hDamageVoice = 0;
    m_hDamageTelop = 0;
    m_hDamageSpeed = 0;
    m_hDamageCamera = 0;
    InitDamage();
    InitChgField();
    m_pInstance = this;
    _Init();
}

msw::CEventDemoDodge::~CEventDemoDodge()
{
    _Destroy();
}

void msw::CEventDemoDodge::CreateDemo(long nDemo, bcs::CTask *pParent)
{
    switch (nDemo) {
        case 5:
            EvDemo::CreateChangeCourtDemo(pParent, sChangeCourt);
            break;
        case 4:
            EvDemo::CreateReadyStartDemo(pParent, sReadyStart);
            break;
        case 0:
            EvDemo::CreateEntranceDemo(pParent, sEntrance);
            break;
        case 1:
            EvDemo::CreateVictoryDemo(pParent, &sVictory);
            break;
        case 6:
            EvDemoDodge::CreateChgFieldDemo(pParent, m_nChgField, m_nChangeCount);
            break;
    }
}

int msw::CEventDemoDodge::IsDemoPlay(long nDemo) const
{
    switch (nDemo) {
        case 0:
        case 1:
        case 4:
        case 6:
            return 1;
    }
    return 0;
}

void msw::CEventDemoDodge::CreateTelop(long nObjId, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitDodge::CreateObj(nObjId, -1);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoDodge::CreateTelop(long nObjId, long nParam, long nCloseAnim)
{
    if (nObjId < 0) {
        return;
    }
    CCockpit::RemoveObj(nObjId);
    CCockpitDodge::CreateObj(nObjId, nParam);
    if (nCloseAnim >= 0) {
        CCockpit::SetAnimEndClose(nObjId, nCloseAnim);
    }
    if (!IsTelop(nObjId)) {
        RegistTelop(nObjId);
    }
}

void msw::CEventDemoDodge::PlayEndTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::PlayEndAnim(nObjId);
        ClearTelop(nObjId);
    }
}

int msw::CEventDemoDodge::IsEndTelop(long nObjId)
{
    if (nObjId >= 0 && IsTelop(nObjId)) {
        return CCockpit::IsEndObj(nObjId);
    }
    return 1;
}

void msw::CEventDemoDodge::RemoveTelop(long nObjId)
{
    if (nObjId < 0) {
        return;
    }
    if (IsTelop(nObjId)) {
        CCockpit::RemoveObj(nObjId);
        ClearTelop(nObjId);
    }
}

void msw::CEventDemoDodge::PlayEndTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::PlayEndAnim(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

void msw::CEventDemoDodge::RemoveTelopAll()
{
    for (int i = 0; i < TELOP_MAX; i++) {
        if (m_nTelop[i] != -1) {
            CCockpit::RemoveObj(m_nTelop[i]);
            m_nTelop[i] = -1;
        }
    }
}

long msw::CEventDemoDodge::GetTelopId(long nTelop)
{
    static const long sTelopId[5] = { 0x1063, 0x1064, 0x16, 0x12, 0x11 };

    return sTelopId[nTelop];
}

void msw::CEventDemoDodge::SetCockpitState(long nPart, long nState)
{
    switch (nPart) {
        case 0:
            if (nState == 2) {
                CCockpitDodge::SetPartMove(0, 3);
                CCockpitDodge::SetRadarIconDisp(0);
            } else if (nState == 1) {
                CCockpitDodge::SetPartMove(0, 1);
                CCockpitDodge::SetRadarIconDisp(1);
            }
            break;
        case 1:
            if (nState == 2) {
                CCockpitDodge::SetMissionCountDisp(0, 0);
            } else if (nState == 1) {
                CCockpitDodge::SetMissionCountDisp(0, 1);
            }
            break;
    }
}

void msw::CEventDemoDodge::TimerEvent(short, short, unsigned long)
{
}

// "MakeScore" events of the damage demo's camera: 1 when the replay
// reaches the hit (the effects for its demo type), 2 when it ends (on to
// the next queued hit), 3 when it is cut short (every queued hit is shown
// on the gauges and the queue emptied).
void msw::CEventDemoDodge::CameraEvent(const char *pName, long nArg, long, float)
{
    if (pName == 0 || strcmp(pName, "MakeScore") != 0) {
        return;
    }
    switch (nArg) {
        case 1:
            _CleanDamageTask();
            if (CGame::g_GameWork.GetStage() == 0x11) {
                CStageDodge17::TrolleyForceReset();
            }
            switch (m_nDamageType) {
                case 0:
                    PopDamageLife(0);
                    CEventDemo::SetDemoCameraFrame(10.0f, 1);
                    m_hDamageSpeed = CSlowEffect::Create(0.0f, 0.05f, 60.0f, 5.0f, 5.0f);
                    break;
                case 2:
                    if (m_nLife <= 0) {
                        CEventDemo::SetDemoCameraFrame(25.0f, 1);
                        m_hDamageSpeed = CSlowEffect::Create(0.0f, 0.05f, 90.0f, 5.0f, 5.0f);
                        m_hDamageVoice = CTelopVoice::Create(0.0f);
                        m_hDamageLifeGauge = CLifeGaugeTask::Create(65.0f);
                        m_hDamageTelop = CTelopOut::Create(93.0f, m_nDamageIndexNow);
                    } else {
                        CEventDemo::SetDemoCameraFrame(25.0f, 1);
                        m_hDamageSpeed = CSlowEffect::Create(0.0f, 0.05f, 90.0f, 5.0f, 10.0f);
                        m_hDamageVoice = CTelopVoice::Create(0.0f);
                        m_hDamageLifeGauge = CLifeGaugeTask::Create(65.0f);
                    }
                    break;
                case 1:
                case 0x16:
                    PopDamageLife(0);
                    CEventDemo::SetDemoCameraFrame(10.0f, 1);
                    m_hDamageSpeed = CSlowEffect::Create(0.0f, 0.05f, 85.0f, 5.0f, 5.0f);
                    if (m_nLife <= 0) {
                        m_hDamageTelop = CTelopOut::Create(28.0f, m_nDamageIndexNow);
                    }
                    break;
            }
            break;
        case 2:
            PopDamageLife(m_nPrevLife >= 0);
            m_nDamageIndexNow = -1;
            _CleanDamageTask();
            EvDemoDodge::StartDamageDemo();
            break;
        case 3:
            if (m_nDamageIndexNow >= 0) {
                PopDamageLife(m_nPrevLife >= 0);
                for (int i = 0; i < m_nDamageCount; i++) {
                    long nTeam = m_nDamageChr[i] & 1;
                    long nNo = m_nDamageChr[i] >> 1;
                    // The result is not used.
                    CGame::GetTeamMaxLife(nTeam);
                    long nLife = m_nLifeList[i];
                    long nPrevLife = m_nPrevLifeList[i];
                    float fRate = CGameDodge::GetChrRestLifeRate(m_nDamageChr[i]);
                    if (fRate > 1.0f) {
                        fRate = 1.0f;
                    } else if (fRate < 0.0f) {
                        fRate = 0.0f;
                    }
                    CCockpitDodge::AddLifeCoder(nTeam, nNo, nPrevLife, nLife, 0, 0);
                    CCockpitDodge::SetLifeGaugeState(nTeam, nNo, 3, -1, -1);
                    CCockpitDodge::SetLifePercent(nTeam, nNo, fRate, fRate);
                    CCockpitDodge::SetLifeValue(nTeam, nNo, nLife);
                }
                CEventDemoDodge *p = GetPtr();
                if (p != 0) {
                    p->InitDamage();
                }
            }
            break;
    }
}

void msw::CEventDemoDodge::SendMessage(long nMsg, long nArg, long, long)
{
    switch (nMsg) {
        case 0xF:
            InitDamage();
            InitChgField();
            break;
        case 0x10:
            InitDamage();
            InitChgField();
            break;
        case 0x12:
            InitDamage();
            InitChgField();
            break;
        case 0x13:
            InitDamage();
            InitChgField();
            break;
        case 0x40:
            m_nChgField[nArg] = 1;
            m_nChangeCount++;
            break;
        case 0x41:
            m_nChgField[nArg] = 0;
            m_nChangeCount++;
            break;
    }
}

// Deletes each damage task that still exists and clears its handle.
// Written out per handle: through an inline helper this function becomes
// small enough for the compiler to inline it into its callers, which the
// original does not.
void msw::CEventDemoDodge::_CleanDamageTask()
{
    bcs::CTask *pTask;

    if (m_hDamageCamera != 0) {
        pTask = bcs::CTaskMan::GetTask(m_hDamageCamera);
        if (pTask != 0) {
            pTask->Delete();
        }
        m_hDamageCamera = 0;
    }
    if (m_hDamageSpeed != 0) {
        pTask = bcs::CTaskMan::GetTask(m_hDamageSpeed);
        if (pTask != 0) {
            pTask->Delete();
        }
        m_hDamageSpeed = 0;
    }
    if (m_hDamageTelop != 0) {
        pTask = bcs::CTaskMan::GetTask(m_hDamageTelop);
        if (pTask != 0) {
            pTask->Delete();
        }
        m_hDamageTelop = 0;
    }
    if (m_hDamageVoice != 0) {
        pTask = bcs::CTaskMan::GetTask(m_hDamageVoice);
        if (pTask != 0) {
            pTask->Delete();
        }
        m_hDamageVoice = 0;
    }
    if (m_hDamageLifeGauge != 0) {
        pTask = bcs::CTaskMan::GetTask(m_hDamageLifeGauge);
        if (pTask != 0) {
            pTask->Delete();
        }
        m_hDamageLifeGauge = 0;
    }
}

long msw::CEventDemoDodge::PopDamageDemo(long &rLife, long &rType)
{
    CEventDemoDodge *p = GetPtr();
    if (p == 0 || m_nDamageCount <= 0) {
        return -1;
    }
    m_nDamageIndexNow = p->m_nDamageChr[0];
    p->m_nPrevLife = p->m_nPrevLifeList[0];
    p->m_nLife = p->m_nLifeList[0];
    p->m_nDamageType = p->m_nDamageTypeList[0];
    for (int i = 1; i < m_nDamageCount; i++) {
        p->m_nDamageChr[i - 1] = p->m_nDamageChr[i];
        p->m_nPrevLifeList[i - 1] = p->m_nPrevLifeList[i];
        p->m_nLifeList[i - 1] = p->m_nLifeList[i];
        p->m_nDamageTypeList[i - 1] = p->m_nDamageTypeList[i];
    }
    rLife = p->m_nLife;
    rType = p->m_nDamageType;
    m_nDamageCount--;
    return m_nDamageIndexNow;
}

void msw::CEventDemoDodge::PushDamageDemo(long nChr, long nLife, long nPrevLife, long nType)
{
    CEventDemoDodge *p = GetPtr();
    if (p == 0) {
        return;
    }
    for (int i = 0; i < m_nDamageCount; i++) {
        if (nChr == p->m_nDamageChr[i]) {
            nType = -1;
            break;
        }
    }
    p->m_nDamageChr[m_nDamageCount] = nChr;
    p->m_nPrevLifeList[m_nDamageCount] = nPrevLife > 0 ? nPrevLife : 0;
    p->m_nLifeList[m_nDamageCount] = nLife > 0 ? nLife : 0;
    p->m_nDamageTypeList[m_nDamageCount] = nType;
    m_nDamageCount++;
}

void msw::CEventDemoDodge::PopDamageLife(int bOut)
{
    CEventDemoDodge *p = GetPtr();
    if (p != 0 && m_nDamageIndexNow != -1 && p->m_nPrevLife >= 0) {
        long nTeam = m_nDamageIndexNow & 1;
        long nNo = m_nDamageIndexNow >> 1;
        // The result is not used.
        CGame::GetTeamMaxLife(nTeam);
        long nLife = p->m_nLife;
        long nPrevLife = p->m_nPrevLife;
        float fRate = CGameDodge::GetChrRestLifeRate(m_nDamageIndexNow);
        fRate = fRate > 1.0f ? 1.0f : (fRate < 0.0f ? 0.0f : fRate);
        CCockpitDodge::AddLifeCoder(nTeam, nNo, nPrevLife, nLife, 0, 0);
        CCockpitDodge::SetLifeGaugeState(nTeam, nNo, 3, -1, -1);
        CCockpitDodge::SetLifePercent(nTeam, nNo, fRate, fRate);
        CCockpitDodge::SetLifeValue(nTeam, nNo, nLife);
        if (bOut && nPrevLife > 0 && nLife <= 0) {
            CreateOutTelop(m_nDamageIndexNow);
        }
        p->m_nPrevLife = -1;
        if (bOut) {
            m_nDamageIndexNow = -1;
        }
    }
}

// The tasks are made here without CStartEffect::Create: with it the function
// is small enough for the compiler to inline it into StartDamageDemo, which
// the original does not.
void msw::CEventDemoDodge::SetDamegeCamera(long nType, long nLife)
{
    bcs::CTask *pParent;
    CStartEffect *pTask;

    if (GetPtr() != 0 && m_nDamageIndexNow != -1) {
        switch (nType) {
            case 0:
                pParent = CGame::m_pInstance;
                pTask = new CStartEffect(pParent, 10.0f);
                m_hDamageVoice = GetTaskHandle(pTask);
                break;
            case 2:
                // Both branches are the same in the original.
                if (nLife <= 0) {
                    pParent = CGame::m_pInstance;
                    pTask = new CStartEffect(pParent, 30.0f);
                    m_hDamageVoice = GetTaskHandle(pTask);
                } else {
                    pParent = CGame::m_pInstance;
                    pTask = new CStartEffect(pParent, 30.0f);
                    m_hDamageVoice = GetTaskHandle(pTask);
                }
                break;
            case 1:
                pParent = CGame::m_pInstance;
                pTask = new CStartEffect(pParent, 10.0f);
                m_hDamageVoice = GetTaskHandle(pTask);
                break;
            default:
                CEventDemo::PlayDemoCamera(0);
                break;
        }
    }
}

void msw::EvDemoDodge::CreateInstance(bcs::CTask *pParent)
{
    new CEventDemoDodge(pParent);
}

int msw::EvDemoDodge::IsChgFieldDemo()
{
    return CEventDemoDodge::m_nChangeCount > 0;
}

void msw::EvDemoDodge::ResetChangeFieldDemo()
{
    CEventDemoDodge *p = CEventDemoDodge::GetPtr();
    if (p != 0) {
        p->InitChgField();
    }
}

int msw::EvDemoDodge::IsDamageDemo()
{
    if (CEventDemoDodge::m_nDamageCount > 0) {
        return 1;
    }
    return CEventDemoDodge::m_nDamageIndexNow != -1;
}

void msw::EvDemoDodge::PushDamageDemo(long nChr, long nLife, long nPrevLife, long nType)
{
    CEventDemoDodge::PushDamageDemo(nChr, nLife, nPrevLife, nType);
}

void msw::EvDemoDodge::SetDamageDemoType(long nChr, long nType)
{
    CEventDemoDodge *p = CEventDemoDodge::GetPtr();
    if (p != 0 && nType != -1) {
        for (int i = 0; i < CEventDemoDodge::m_nDamageCount; i++) {
            if (nChr == p->m_nDamageChr[i]) {
                p->m_nDamageTypeList[i] = nType;
            }
        }
    }
}

// (name guessed) True with a chance of nPercent in 100.
static inline int IsRandHit(long nPercent)
{
    return bcs::BCS_RandMax(99L) < nPercent;
}

// (name guessed) rOut += rAdd. A stand-in for what was probably an
// operator+= of the vector class: the paired-single registers come out in
// the original's order only with this extra inline level.
static inline void AddTo(bcs::FVEC3 &rOut, const bcs::FVEC3 &rAdd)
{
    bcs::FVEC3Add(&rOut, &rOut, &rAdd);
}

// Starts the replay of the next queued hit: picks the demo type (a fall
// off the court, or a 50% chance of none for types 0 and 2), points the
// demo camera between the character and the ball in play, and starts it.
// A hit with no demo goes straight to the gauge and the next one.
void msw::EvDemoDodge::StartDamageDemo()
{
    if (CEventDemoDodge::m_nDamageIndexNow != -1) {
        return;
    }
    long nLife;
    long nType;
    long nChr = CEventDemoDodge::PopDamageDemo(nLife, nType);
    if (nChr < 0) {
        return;
    }
    CChrCore *pChr = CChrMan::GetChrCore(nChr);
    if (pChr == 0) {
        return;
    }
    bcs::FVEC3 pos;
    pChr->GetCompPos(pos);
    if (nType >= 0 && pos.y < -0.1f) {
        if (!CGameDodge::IsNowArtsShootMode()) {
            pos.y = 0.0f;
            nType = 0x16;
        } else {
            nType = -1;
        }
    } else {
        switch (nType) {
            case 0:
                if (!IsRandHit(50)) {
                    nType = -1;
                }
                break;
            case 1:
                break;
            case 2:
                if (!IsRandHit(50)) {
                    nType = -1;
                }
                break;
        }
    }
    if (nType < 0) {
        CEventDemoDodge::PopDamageLife(1);
        StartDamageDemo();
        return;
    }
    CEventDemo::SetupDemoCamera("MakeScore", nType);
    bcs::FVEC3 target(0.0f, 0.0f, 0.0f);
    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall != 0 && pBall->IsActive()) {
            bcs::FVEC3 ballPos;
            bcs::FVEC3Sum(ballPos, pBall->GetPos(), pBall->GetOffsetPos());
            target = ballPos;
            break;
        }
    }
    AddTo(target, pos);
    CEventDemo::SetDemoCameraInfo(pos, target, 0);
    CEventDemoDodge::SetDamegeCamera(nType, nLife);
}

void msw::EvDemoDodge::ResetDamageDemo()
{
    CEventDemo::SetEndDemoCamera();
    CEventDemoDodge *p = CEventDemoDodge::GetPtr();
    if (p != 0) {
        p->InitDamage();
    }
}

void msw::CSlowEffect::Main()
{
    if (m_fWait <= 0.0f) {
        if (!CGame::IsNowSlowEffect()) {
            Delete();
        }
    } else {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            Exec();
        }
    }
}

msw::CSlowEffect::~CSlowEffect()
{
    CGame::StopSlowEffect();
}

void msw::CStartEffect::Main()
{
    if (m_fWait <= 0.0f) {
        Delete();
    } else {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            Exec();
            Delete();
        }
    }
}

msw::CStartEffect::~CStartEffect()
{
}

void msw::CLifeGaugeTask::Main()
{
    if (m_fWait <= 0.0f) {
        Delete();
    } else {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            Exec();
            Delete();
        }
    }
}

msw::CLifeGaugeTask::~CLifeGaugeTask()
{
}

void msw::CTelopVoice::Main()
{
    if (m_fWait <= 0.0f) {
        Delete();
    } else {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            Exec();
            Delete();
        }
    }
}

msw::CTelopVoice::~CTelopVoice()
{
}

void msw::CTelopOut::Main()
{
    if (m_fWait <= 0.0f) {
        Delete();
    } else {
        m_fWait -= GetFrameRateF();
        if (m_fWait <= 0.0f) {
            Exec();
            Delete();
        }
    }
}

msw::CTelopOut::~CTelopOut()
{
}
