// msw::CStageHockeyCrushSkate86: hockey party stage 86, a crush skate
// stage with a fire pillar that starts burning some seconds into each turn
// (CStgGmk_FirePillar_Hockey) and fire pillars in the background
// (STAGE11::CBgFirePillarCluster, as on stage 11). The rolling floor has
// four tunings; the party game's pattern (GetLocPatternNo) picks one.
#include "game/msw/CStageHockeyCrushSkate86.hpp"

#include "bcs/New.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace {

    // (names guessed) One entry of the parameter table GetPrmTbl gives: the
    // type of a tuning value, the value, and the offset of the word it
    // fills. The table ends with a zero entry.
    enum PRM_TYPE {
        PRM_TYPE_END,
        PRM_TYPE_LONG,
        PRM_TYPE_FLOAT
    };

    struct PRM_TBL {
        long m_nType;   // 0x00
        void *m_pValue; // 0x04
        long m_nOffset; // 0x08
    };

    using namespace msw;

    const PRM_TBL s_PrmTbl[] = {
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_COURT_RADIUS, 0x00 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_BG_ROT_SPEED, 0x04 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S86_FLOOR0_ROT_ENABLE, 0x08 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_INIT_WAIT, 0x0C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_CYCLE, 0x10 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_STOP_TIME, 0x14 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_STOP_TIME_RND, 0x18 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_STOP_TIME2, 0x1C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_STOP_TIME_RND2, 0x20 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_CHR_FLOW_VEL, 0x24 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR0_ROT_ANGLE, 0x28 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_INIT_INTERVAL, 0x2C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_PRESTART_TIME0, 0x30 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_STAY_TIME0, 0x34 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_INTERVAL0, 0x38 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_TIME1, 0x3C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_PRESTART_TIME1, 0x40 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_STAY_TIME1, 0x44 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_INTERVAL1, 0x48 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_TIME2, 0x4C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_PRESTART_TIME2, 0x50 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_STAY_TIME2, 0x54 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE0_INTERVAL2, 0x58 },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S86_FLOOR1_ROT_ENABLE, 0x5C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_INIT_WAIT, 0x60 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_CYCLE, 0x64 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_STOP_TIME, 0x68 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_STOP_TIME_RND, 0x6C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_STOP_TIME2, 0x70 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_STOP_TIME_RND2, 0x74 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_CHR_FLOW_VEL, 0x78 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FLOOR1_ROT_ANGLE, 0x7C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_INIT_INTERVAL, 0x80 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_PRESTART_TIME0, 0x84 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_STAY_TIME0, 0x88 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_INTERVAL0, 0x8C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_TIME1, 0x90 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_PRESTART_TIME1, 0x94 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_STAY_TIME1, 0x98 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_INTERVAL1, 0x9C },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_TIME2, 0xA0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_PRESTART_TIME2, 0xA4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_STAY_TIME2, 0xA8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE1_INTERVAL2, 0xAC },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE_RADIUS, 0xB0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE_RADIUS2, 0xB4 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE_RADIUS3, 0xB8 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE_RESIST, 0xBC },
        { PRM_TYPE_LONG, &HOCKEY_PRM_S86_FIRE_SLOWDOWN_RATE, 0xC0 },
        { PRM_TYPE_FLOAT, &HOCKEY_PRM_S86_FIRE_HEIGHT, 0xC4 },
        { PRM_TYPE_END, 0, 0 },
    };
}

// Models, sound slots, sounds and camera vibration of the stage.
enum {
    MDL_BG_FIRE_PILLAR = 100,
    SE_SLOT_AMBIENT0 = 0,
    SE_AMBIENT0 = 0xDB9,
    SE_SHIP = 0xDB7,
    SE_BG_FIRE_PILLAR = 0xDB8,
    CVID_BG_FIRE_PILLAR = 0x3F
};

const void *msw::CStageHockeyCrushSkate86::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockeyCrushSkate86::CStageHockeyCrushSkate86(bcs::CTask *pParent)
    : CStageHockeyCrushSkate(pParent), m_fFireWait(-1.0f)
{
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        m_pFirePillar[i] = 0;
    }
}

msw::CStageHockeyCrushSkate86::~CStageHockeyCrushSkate86()
{
}

void msw::CStageHockeyCrushSkate86::Main_(float frame)
{
    CStageHockeyCrushSkate::Main_(frame);
    Fire_Update(frame);
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockeyCrushSkate86::GetMdlGenRule_() const
{
    // The end entry chains to the base class's table.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { "^s[0-9]+_hibasira_[0-9]+$", MDL_BG_FIRE_PILLAR, 0, 0, NewMdl },
        { 0, reinterpret_cast<long>(CStageHockeyCrushSkate::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

void msw::CStageHockeyCrushSkate86::OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                    CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                                    long n)
{
    if (pRule->m_nId == MDL_BG_FIRE_PILLAR) {
        m_BgFirePillar.PushBack(new (1) STAGE11::CStgRndAnmObj(this, pNode, CVID_BG_FIRE_PILLAR, SE_BG_FIRE_PILLAR,
                                                               60.0f, 600.0f));
        pNode->OffFlag(1);
    }

    CStageManHockey::OnPostAppendNode(pRule, rRegex, pNode, rFile, rArn, n);
}

// The court is round.
float msw::CStageHockeyCrushSkate86::GetBaseCourtWidth_() const
{
    return 2.0f * HOCKEY_PRM_S86_COURT_RADIUS;
}

float msw::CStageHockeyCrushSkate86::GetBaseCourtDepth_() const
{
    return HOCKEY_PRM_S86_COURT_RADIUS;
}

float msw::CStageHockeyCrushSkate86::GetBaseCourtRadius_() const
{
    return HOCKEY_PRM_S86_COURT_RADIUS;
}

long msw::CStageHockeyCrushSkate86::GetParam_ShipSe() const
{
    return SE_SHIP;
}

void msw::CStageHockeyCrushSkate86::OnSceneEntry()
{
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        CStgGmk_FirePillar_Hockey *pPillar = new (1) CStgGmk_FirePillar_Hockey(this, 0);
        if (pPillar != 0) {
            pPillar->Create();
            pPillar->SceneEntry();
            pPillar->SetAuto(1);
            m_pFirePillar[i] = pPillar;
        }
    }
}

void msw::CStageHockeyCrushSkate86::OnSceneReentry()
{
    m_BgFirePillar.ResetAll();
}

// The pillars start HOCKEY_PRM_S86_FIRE<n>_INIT_INTERVAL seconds into the
// turn.
void msw::CStageHockeyCrushSkate86::OnTurnInit()
{
    CStageHockeyCrushSkate::OnTurnInit();

    float wait;
    if (GetLocPatternNo() == 0) {
        wait = HOCKEY_PRM_S86_FIRE0_INIT_INTERVAL;
    } else if (GetLocPatternNo() == 1) {
        wait = HOCKEY_PRM_S86_FIRE1_INIT_INTERVAL;
    } else if (GetLocPatternNo() == 2) {
        wait = HOCKEY_PRM_S86_FIRE2_INIT_INTERVAL;
    } else {
        wait = HOCKEY_PRM_S86_FIRE3_INIT_INTERVAL;
    }
    m_fFireWait = 60.0f * wait;
}

void msw::CStageHockeyCrushSkate86::OnGimmickReset()
{
    Fire_ResetAll();
}

void msw::CStageHockeyCrushSkate86::OnGimmickStart()
{
}

void msw::CStageHockeyCrushSkate86::OnGimmickStop()
{
    CStageHockeyCrushSkate::OnGimmickStop();
    Fire_InvalidateAll();
}

void msw::CStageHockeyCrushSkate86::OnGimmickCancel()
{
    Fire_StopAll();
}

void msw::CStageHockeyCrushSkate86::OnGimmickPause()
{
    Fire_StopAll();
}

void msw::CStageHockeyCrushSkate86::OnGimmickResume()
{
}

unsigned long msw::CStageHockeyCrushSkate86::OnCalcSum() const
{
    unsigned long sum = 0;
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        if (m_pFirePillar[i] != 0) {
            sum += m_pFirePillar[i]->CalcSum();
        }
    }
    return sum;
}

// While the gimmicks run and the pillars are not burning yet, the wait
// counts down; at its end the pillars start.
void msw::CStageHockeyCrushSkate86::Fire_Update(float frame)
{
    // A negative wait means the pillars have started. (Written as a negation,
    // which matches the original; it differs from ">= 0" only for NaN.)
    if (IsGimmickRunning() && !(m_fFireWait < 0.0f) && !Fire_IsSwooshAll()) {
        m_fFireWait -= frame;
        if (m_fFireWait <= 0.0f) {
            m_fFireWait = -1.0f;
            for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
                if (m_pFirePillar[i] != 0) {
                    m_pFirePillar[i]->Start();
                }
            }
        }
    }
}

void msw::CStageHockeyCrushSkate86::Fire_StopAll()
{
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        if (m_pFirePillar[i] != 0) {
            m_pFirePillar[i]->Stop();
        }
    }
}

void msw::CStageHockeyCrushSkate86::Fire_ResetAll()
{
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        CStgGmk_FirePillar_Hockey *pPillar = m_pFirePillar[i];
        if (pPillar != 0) {
            pPillar->Reset();
            pPillar->SetAuto(1);
        }
    }
}

void msw::CStageHockeyCrushSkate86::Fire_InvalidateAll()
{
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        CStgGmk_FirePillar_Hockey *pPillar = m_pFirePillar[i];
        if (pPillar != 0) {
            pPillar->Reset();
            pPillar->SetAuto(0);
        }
    }
}

bool msw::CStageHockeyCrushSkate86::Fire_IsSwooshAll() const
{
    for (long i = 0; i < FIRE_PILLAR_NUM; i++) {
        if (m_pFirePillar[i] != 0 && !m_pFirePillar[i]->IsSwoosh()) {
            return false;
        }
    }
    return true;
}

void msw::CStageHockeyCrushSkate86::OnSoundEvent(long event)
{
    if (event == 0) {
        if (!SE_IsAlive(SE_SLOT_AMBIENT0)) {
            SE_Play(SE_SLOT_AMBIENT0, SE_AMBIENT0, 0.0f, 3);
        }
    }
}

long msw::CStageHockeyCrushSkate86::GetFirePillarMaxNum()
{
    return FIRE_PILLAR_NUM;
}

msw::CStgGmk_FirePillar_Hockey *msw::CStageHockeyCrushSkate86::GetFirePillar(long idx)
{
    CStageHockeyCrushSkate86 *pStage = static_cast<CStageHockeyCrushSkate86 *>(CStageMan::GetInstance());
    if (idx < 0 || idx >= GetFirePillarMaxNum()) {
        return 0;
    }

    CStgGmk_FirePillar_Hockey *pPillar = pStage->m_pFirePillar[idx];
    if (pPillar == 0 || !pPillar->IsSwoosh()) {
        return 0;
    }
    return pPillar;
}

void msw::CStageHockeyCrushSkate86::Disp()
{
    CStageHockeyCrushSkate::Disp();
}

float msw::CStageHockeyCrushSkate86::GetParam_ShipCycle() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S86_FLOOR0_CYCLE;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S86_FLOOR1_CYCLE;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S86_FLOOR2_CYCLE;
    } else {
        return HOCKEY_PRM_S86_FLOOR3_CYCLE;
    }
}

float msw::CStageHockeyCrushSkate86::GetParam_ShipStopTime() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S86_FLOOR0_STOP_TIME;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S86_FLOOR1_STOP_TIME;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S86_FLOOR2_STOP_TIME;
    } else {
        return HOCKEY_PRM_S86_FLOOR3_STOP_TIME;
    }
}

float msw::CStageHockeyCrushSkate86::GetParam_ShipStopTimeRnd() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S86_FLOOR0_STOP_TIME_RND;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S86_FLOOR1_STOP_TIME_RND;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S86_FLOOR2_STOP_TIME_RND;
    } else {
        return HOCKEY_PRM_S86_FLOOR3_STOP_TIME_RND;
    }
}

float msw::CStageHockeyCrushSkate86::GetParam_ShipStopTime2() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S86_FLOOR0_STOP_TIME2;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S86_FLOOR1_STOP_TIME2;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S86_FLOOR2_STOP_TIME2;
    } else {
        return HOCKEY_PRM_S86_FLOOR3_STOP_TIME2;
    }
}

float msw::CStageHockeyCrushSkate86::GetParam_ShipStopTimeRnd2() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S86_FLOOR0_STOP_TIME_RND2;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S86_FLOOR1_STOP_TIME_RND2;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S86_FLOOR2_STOP_TIME_RND2;
    } else {
        return HOCKEY_PRM_S86_FLOOR3_STOP_TIME_RND2;
    }
}

float msw::CStageHockeyCrushSkate86::GetParam_ChrFlowVel() const
{
    if (GetLocPatternNo() == 0) {
        return HOCKEY_PRM_S86_FLOOR0_CHR_FLOW_VEL;
    } else if (GetLocPatternNo() == 1) {
        return HOCKEY_PRM_S86_FLOOR1_CHR_FLOW_VEL;
    } else if (GetLocPatternNo() == 2) {
        return HOCKEY_PRM_S86_FLOOR2_CHR_FLOW_VEL;
    } else {
        return HOCKEY_PRM_S86_FLOOR3_CHR_FLOW_VEL;
    }
}

// The angle is given in degrees.
float msw::CStageHockeyCrushSkate86::GetParam_ShipRotWidth() const
{
    float value;
    if (GetLocPatternNo() == 0) {
        value = HOCKEY_PRM_S86_FLOOR0_ROT_ANGLE;
    } else if (GetLocPatternNo() == 1) {
        value = HOCKEY_PRM_S86_FLOOR1_ROT_ANGLE;
    } else if (GetLocPatternNo() == 2) {
        value = HOCKEY_PRM_S86_FLOOR2_ROT_ANGLE;
    } else {
        value = HOCKEY_PRM_S86_FLOOR3_ROT_ANGLE;
    }
    return 0.017453292f * value;
}

// The wait is given in seconds.
float msw::CStageHockeyCrushSkate86::GetParam_ShipRotInterval() const
{
    float value;
    if (GetLocPatternNo() == 0) {
        value = HOCKEY_PRM_S86_FLOOR0_INIT_WAIT;
    } else if (GetLocPatternNo() == 1) {
        value = HOCKEY_PRM_S86_FLOOR1_INIT_WAIT;
    } else if (GetLocPatternNo() == 2) {
        value = HOCKEY_PRM_S86_FLOOR2_INIT_WAIT;
    } else {
        value = HOCKEY_PRM_S86_FLOOR3_INIT_WAIT;
    }
    return 60.0f * value;
}

bool msw::CStageHockeyCrushSkate86::GetParam_IsShipRotEnable() const
{
    long enable;
    if (GetLocPatternNo() == 0) {
        enable = HOCKEY_PRM_S86_FLOOR0_ROT_ENABLE;
    } else if (GetLocPatternNo() == 1) {
        enable = HOCKEY_PRM_S86_FLOOR1_ROT_ENABLE;
    } else if (GetLocPatternNo() == 2) {
        enable = HOCKEY_PRM_S86_FLOOR2_ROT_ENABLE;
    } else {
        enable = HOCKEY_PRM_S86_FLOOR3_ROT_ENABLE;
    }
    return enable;
}

// The background turns once in BG_ROT_SPEED minutes (0: not at all).
float msw::CStageHockeyCrushSkate86::GetParam_BgRotSpeed() const
{
    float minutes = HOCKEY_PRM_S86_BG_ROT_SPEED;
    if (minutes > 0.0f) {
        return 6.2831855f / (60.0f * (60.0f * minutes));
    }
    return 0.0f;
}
