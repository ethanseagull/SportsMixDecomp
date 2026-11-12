// msw::CStageVolley03: volley stage 03, the fountain stage. Twelve water
// jets stand along the net; every few seconds a pattern of them gushes up
// and blocks the ball. A small sound task (CGmkSnd) follows the risen jets.
#include "game/msw/CStageVolley03.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CStgAct.hpp"
#include "game/msw/CStgGmk_WaterWorks_Volley.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/VolleyPrm.hpp"

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_AMBIENT = 1,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_AMBIENT = 0xD18,
    SE_AMBIENT2 = 0xD19,
    SE_EVENT = 0xD1A,
    SE_WATER_UP = 0xD12, // (names guessed) played by CGmkSnd
    SE_WATER_DOWN = 0xD13
};

// Stage messages handled here (sent through CStageMan::SendMessage).
enum {
    MSG_STOP_WATER = 0x25 // arg1 1: stop every jet
};

namespace msw {

    // (names guessed) One entry of the parameter table that GetPrmTbl
    // gives: the type of a tuning value, its address, and the offset of the
    // word it fills. The table ends with a zero entry.
    enum S03_PRM_TYPE {
        S03_PRM_TYPE_END,
        S03_PRM_TYPE_LONG,
        S03_PRM_TYPE_FLOAT
    };

    struct S03_PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    static const S03_PRM_TBL s_PrmTbl[] = {
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_WATER_HEIGHT, 0x00 },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_WATER_PRESTART_TIME, 0x04 },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_WATER_STAY_TIME, 0x08 },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_WATER_INTERVAL, 0x0C },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_WATER_SPEED, 0x10 },
        { S03_PRM_TYPE_LONG, &VOLLEY_PRM_S03_WATER_NUM, 0x14 },
        { S03_PRM_TYPE_LONG, &VOLLEY_PRM_S03_WATER_PATTERN, 0x18 },
        { S03_PRM_TYPE_LONG, &VOLLEY_PRM_S03_PAT0_NUM, 0x1C },
        { S03_PRM_TYPE_LONG, &VOLLEY_PRM_S03_PAT0B_NUM, 0x20 },
        { S03_PRM_TYPE_LONG, &VOLLEY_PRM_S03_PAT1_NUM, 0x24 },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_PAT2_SPEED, 0x28 },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_PAT2_UNIT_STAY_TIME, 0x2C },
        { S03_PRM_TYPE_FLOAT, &VOLLEY_PRM_S03_PAT2_PRESTART_TIME, 0x30 },
        { S03_PRM_TYPE_LONG, &VOLLEY_PRM_S03_TEST_ALWAYS, 0x34 },
        { S03_PRM_TYPE_END, 0, 0 },
    };

}

namespace {

    using namespace msw;

    // (names guessed) The gush sound of a pattern: a task that follows the
    // mean position of the risen jets, plays the rising sound, then the
    // falling one once the first has finished, and ends. Size 0x4C.
    class CGmkSnd : public bcs::CTask {
        public:
            CGmkSnd(CStageVolley03 *pStage) : bcs::CTask(CGame::GetTempParent(), -1)
            {
                m_nSndId = 0;
                m_nStep = 0;
                m_pStage = pStage;

                bcs::FVEC3 pos;
                if (m_pStage->Water_GetCenterPos(pos)) {
                    m_Pos = pos;
                } else {
                    // Nothing has risen: play nothing.
                    m_nStep = 3;
                }
            }

            virtual ~CGmkSnd()
            {
                bcs::CSndTask *pSnd = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_nSndId));
                if (pSnd != 0) {
                    pSnd->Stop(5.0f);
                }
            }

            virtual void Main();

        private:
            bcs::FVEC3 m_Pos;            // 0x34
            unsigned long m_nSndId;      // 0x40, task id of the sound playing
            long m_nStep;                // 0x44, sounds played so far
            CStageVolley03 *m_pStage;    // 0x48
    };

    // The number of jets a pattern starts: `num`, kept between 0 and `max`.
    static inline long ClampWaterNum(long num, long max)
    {
        if (num > max) {
            return max;
        }
        return num < 0 ? 0 : num;
    }

    // Starts one jet of a pattern.
    static inline void StartWater(CStgGmk_WaterWorks_Volley *pWater, float stayTime, float delay, int bDelayed)
    {
        if (pWater != 0) {
            pWater->SetParam_StayTime(stayTime);
            pWater->SetParam_Height(VOLLEY_PRM_S03_WATER_HEIGHT);
            pWater->Start(delay, bDelayed);
        }
    }
}

const void *msw::CStageVolley03::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageVolley03::CStageVolley03(bcs::CTask *pParent) : CStageManVolley(pParent)
{
    m_nPattern = 0;
    for (long i = 0; i < WATER_MAX; i++) {
        m_apWater[i] = 0;
    }
    m_fInterval = 60.0f * VOLLEY_PRM_S03_WATER_INTERVAL;
}

msw::CStageVolley03::~CStageVolley03()
{
    for (long i = 0; i < WATER_MAX; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Delete();
        }
        m_apWater[i] = 0;
    }
}

void msw::CStageVolley03::Main_(float frame)
{
    UpdateWaterPattern_(frame);
    UpdateWaterPos_(frame);
    CStageManVolley::Main_(frame);
}

// The interval runs only while the gimmick does and no jet is up; when it
// is over the next pattern starts.
void msw::CStageVolley03::UpdateWaterPattern_(float frame)
{
    if ((CGame::g_GameWork.m_nFlag18 & 8) || !IsGimmickRunning()) {
        m_fInterval = 60.0f * VOLLEY_PRM_S03_WATER_INTERVAL;
    } else if (!Water_IsSwooshAll()) {
        m_fInterval -= frame;
        if (m_fInterval <= 0.0f && VOLLEY_PRM_S03_WATER_STAY_TIME > 0.0f) {
            m_fUnk23C = 0.0f;
            m_fStayTime = 60.0f * VOLLEY_PRM_S03_WATER_STAY_TIME;
            Water_Start(m_nPattern);
            Water_NextPattern();
        }
    } else {
        m_fInterval = 60.0f * VOLLEY_PRM_S03_WATER_INTERVAL;
    }
}

void msw::CStageVolley03::UpdateWaterPos_(float frame)
{
    bcs::FVEC3 base(0.0f, 0.0f, 0.0f);
    float width = GetNetWidth();
    const bcs::FVEC3 &rNet = GetNetPos();
    base.x = rNet.x;
    base.y = 2.0f;
    base.z = rNet.z;
    float left = rNet.x - 0.5f * width;
    float right = rNet.x + 0.5f * width;
    long num = VOLLEY_PRM_S03_WATER_NUM;

    long i;
    for (i = 0; i < num; i++) {
        CStgGmk_WaterWorks_Volley *pWater = m_apWater[i];
        if (pWater != 0) {
            // Each jet in the middle of its share of the net.
            float rate = static_cast<float>(i) / num + 1.0f / (num * 2);
            bcs::FVEC3 pos(rate * (right - left) + left, base.y, base.z);
            pWater->SetDefaultPos(pos);
            pWater->SetPos(pos);
            pWater->Activate();
        }
    }

    for (; i < WATER_MAX; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Inactivate();
        }
    }
}

// Patterns: 0 the outermost jets from both ends, 1 from the left end, 2
// from the right end, 3 the middle ones, 4 and 5 every jet one after the
// other from the left or from the right.
void msw::CStageVolley03::Water_Start(long pattern)
{
    m_nPattern = pattern;

    switch (pattern) {
        case 0:
            if (VOLLEY_PRM_S03_WATER_NUM > 0) {
                long num = ClampWaterNum(VOLLEY_PRM_S03_PAT0_NUM, VOLLEY_PRM_S03_WATER_NUM);
                long last = VOLLEY_PRM_S03_WATER_NUM - 1;
                for (long i = 0; i < num; i++) {
                    StartWater(m_apWater[i], m_fStayTime, -1.0f, 0);
                    StartWater(m_apWater[last - i], m_fStayTime, -1.0f, 0);
                }
            }
            break;
        case 1:
            if (VOLLEY_PRM_S03_WATER_NUM > 0) {
                long num = ClampWaterNum(VOLLEY_PRM_S03_PAT0B_NUM, VOLLEY_PRM_S03_WATER_NUM);
                for (long i = 0; i < num; i++) {
                    StartWater(m_apWater[i], m_fStayTime, -1.0f, 0);
                }
            }
            break;
        case 2:
            if (VOLLEY_PRM_S03_WATER_NUM > 0) {
                long num = ClampWaterNum(VOLLEY_PRM_S03_PAT0B_NUM, VOLLEY_PRM_S03_WATER_NUM);
                long last = VOLLEY_PRM_S03_WATER_NUM - 1;
                for (long i = 0; i < num; i++) {
                    StartWater(m_apWater[last - i], m_fStayTime, -1.0f, 0);
                }
            }
            break;
        case 3:
            if (VOLLEY_PRM_S03_WATER_NUM > 0) {
                long num = ClampWaterNum(VOLLEY_PRM_S03_PAT1_NUM, VOLLEY_PRM_S03_WATER_NUM);
                long first = VOLLEY_PRM_S03_WATER_NUM / 2 - num / 2;
                for (long i = 0; i < num; i++) {
                    StartWater(m_apWater[first + i], m_fStayTime, -1.0f, 0);
                }
            }
            break;
        case 4: {
            long num = VOLLEY_PRM_S03_WATER_NUM;
            if (m_fStayTime > 0.0f && num != 0) {
                float speed = VOLLEY_PRM_S03_PAT2_SPEED;
                float stayTime = VOLLEY_PRM_S03_PAT2_UNIT_STAY_TIME;
                float delay = VOLLEY_PRM_S03_WATER_PRESTART_TIME - VOLLEY_PRM_S03_PAT2_PRESTART_TIME;
                for (long i = 0; i < num; i++) {
                    StartWater(m_apWater[i], stayTime, i == 0 ? 0.0f : i * speed + delay, i != 0);
                }
            }
            break;
        }
        case 5: {
            long num = VOLLEY_PRM_S03_WATER_NUM;
            if (m_fStayTime > 0.0f && num != 0) {
                float speed = VOLLEY_PRM_S03_PAT2_SPEED;
                float stayTime = VOLLEY_PRM_S03_PAT2_UNIT_STAY_TIME;
                float delay = VOLLEY_PRM_S03_WATER_PRESTART_TIME - VOLLEY_PRM_S03_PAT2_PRESTART_TIME;
                long last = num - 1;
                for (long i = 0; i < num; i++) {
                    StartWater(m_apWater[last - i], stayTime, i == 0 ? 0.0f : i * speed + delay, i != 0);
                }
            }
            break;
        }
    }

    new (1) CGmkSnd(this);
}

void msw::CStageVolley03::Water_StopAll()
{
    for (long i = 0; i < WATER_MAX; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Stop();
        }
    }
}

// The interval is reset to its value in seconds here, not in frames as
// everywhere else; kept as the original has it.
void msw::CStageVolley03::Water_ResetAll()
{
    for (long i = 0; i < WATER_MAX; i++) {
        if (m_apWater[i] != 0) {
            m_apWater[i]->Reset();
        }
    }
    m_fInterval = VOLLEY_PRM_S03_WATER_INTERVAL;
    Water_NextPattern();
}

bool msw::CStageVolley03::Water_IsSwooshAll() const
{
    for (long i = 0; i < WATER_MAX; i++) {
        if (m_apWater[i] != 0 && m_apWater[i]->IsSwoosh()) {
            return true;
        }
    }
    return false;
}

// VOLLEY_PRM_S03_WATER_PATTERN 0 to 5 always runs that pattern, 6 a random
// one, 7 each in turn.
void msw::CStageVolley03::Water_NextPattern()
{
    if (VOLLEY_PRM_S03_WATER_PATTERN < 6) {
        m_nPattern = VOLLEY_PRM_S03_WATER_PATTERN;
        return;
    }

    switch (VOLLEY_PRM_S03_WATER_PATTERN - 6) {
        case 0:
            m_nPattern = bcs::BCS_RandMax(5L);
            break;
        case 1:
            m_nPattern = (m_nPattern + 1) % 6;
            break;
    }
}

bool msw::CStageVolley03::Water_GetCenterPos(bcs::FVEC3 &rPos) const
{
    bool found = false;
    long num = 0;

    rPos.x = 0.0f;
    rPos.y = 0.0f;
    rPos.z = 0.0f;

    for (long i = 0; i < WATER_MAX; i++) {
        const CStgGmk_WaterWorks_Volley *pWater = m_apWater[i];
        if (pWater != 0 && pWater->IsSwoosh2()) {
            num++;
            const bcs::FVEC3 &rWaterPos = pWater->GetAct()->GetSrt()->GetPos();
            rPos.x += rWaterPos.x;
            rPos.y += rWaterPos.y;
            rPos.z += rWaterPos.z;
        }
    }

    if (num > 0) {
        found = true;
        float scale = 1.0f / num;
        rPos.x *= scale;
        rPos.y *= scale;
        rPos.z *= scale;
    }

    return found;
}

bool msw::CStageVolley03::Water_CheckHit(float x, float r) const
{
    float min = x - r;
    float max = x + r;

    for (long i = 0; i < WATER_MAX; i++) {
        const CStgGmk_WaterWorks_Volley *pWater = m_apWater[i];
        if (pWater != 0 && pWater->IsSwoosh3()) {
            float left = pWater->GetSrt()->GetPos().x - 0.5f * pWater->GetParam_Width();
            float right = pWater->GetSrt()->GetPos().x + 0.5f * pWater->GetParam_Width();
            if (min <= right && max >= left) {
                return true;
            }
        }
    }
    return false;
}

void msw::CStageVolley03::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManVolley::SendMessage(msg, arg1, arg2, arg3);

    if (msg == MSG_STOP_WATER && arg1 == 1) {
        Water_StopAll();
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageVolley03::GetMdlGenRule_() const
{
    // Only the end entry, chaining to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { 0, reinterpret_cast<long>(CStageManVolley::GetMdlGenRule_()) },
    };

    return tbl;
}

bool msw::CStageVolley03::OnPreCreateMdl(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, nw4r::g3d::ResFile, CArn &,
                                         long)
{
    return true;
}

void msw::CStageVolley03::OnPostInitialize()
{
    CStageManVolley::OnPostInitialize();

    for (long i = 0; i < WATER_MAX; i++) {
        CStgGmk_WaterWorks_Volley *pWater = new CStgGmk_WaterWorks_Volley(this, 0);
        if (pWater != 0) {
            pWater->Create();
            pWater->SceneEntry();
            m_apWater[i] = pWater;
        }
    }

    UpdateWaterPos_(0.0f);
}

void msw::CStageVolley03::OnTurnInit()
{
    Water_ResetAll();
}

float msw::CStageVolley03::GetBaseNetHeightBottom_() const
{
    return CStageManVolley::GetBaseNetHeightBottom_();
}

void msw::CStageVolley03::OnArtsShootStart()
{
}

void msw::CStageVolley03::OnGimmickReset()
{
    Water_ResetAll();
}

void msw::CStageVolley03::OnGimmickStart()
{
}

void msw::CStageVolley03::OnGimmickStop()
{
    Water_ResetAll();
}

void msw::CStageVolley03::OnGimmickCancel()
{
    Water_ResetAll();
}

void msw::CStageVolley03::OnGimmickPause()
{
}

void msw::CStageVolley03::OnGimmickResume()
{
}

void msw::CStageVolley03::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
                SE_RepeatPlay(SE_SLOT_AMBIENT, SE_AMBIENT, 180.0f, 420.0f, -1.0f, 0.0f, 12);
            }
            if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
                SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

unsigned long msw::CStageVolley03::OnCalcSum() const
{
    unsigned long sum = 0;
    for (long i = 0; i < WATER_MAX; i++) {
        if (m_apWater[i] != 0) {
            sum += m_apWater[i]->CalcSum();
        }
    }
    return sum;
}

void msw::CStageVolley03::Disp()
{
    CStageManVolley::Disp();
}

// Defined last: the original emits it after the stage's functions.
void CGmkSnd::Main()
{
    bcs::FVEC3 pos;
    if (!m_pStage->Water_GetCenterPos(pos)) {
        Delete();
        return;
    }
    m_Pos = pos;

    bcs::CSndTask *pSnd = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_nSndId));
    if (pSnd != 0 && !pSnd->IsDelete()) {
        if (!pSnd->IsPrepared()) {
            return;
        }
        if (pSnd->IsAttachedSound()) {
            return;
        }
    }

    switch (++m_nStep) {
        case 1:
            m_nSndId = CGameSnd::Play(SE_WATER_UP, &m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
            break;
        case 2:
            CGameSnd::Play(SE_WATER_DOWN, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
            Delete();
            break;
    }
}
