// Written from src/game/basket/CStageBasket03.cpp by tools/decomp/port_unit.py (Basket -> Hockey); check against HockeyF.
// msw::CStageHockey03: hockey stage 03, the fountain stage. Water pillars
// gush up in front of both goals (two CStgGmk_WaterPillarCluster_Hockey);
// the pillars at a goal stop while its team is not the one attacking it.
// HockeyF's CStageHockey03 is the same stage on its own stage manager.
#include "game/msw/CStageHockey03.hpp"

#include "bcs/FVEC3.hpp"
#include "game/msw/HockeyPrm.hpp"
#include "game/msw/CGame.hpp"

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
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_RADIUS, 0x00 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_RADIUS2, 0x04 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_RADIUS3, 0x08 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_RESIST, 0x0C },
        { S03_PRM_TYPE_LONG, &HOCKEY_PRM_S03_WATER_SLOWDOWN_RATE, 0x10 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_HEIGHT, 0x14 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_PRESTART_TIME, 0x18 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_STAY_TIME, 0x1C },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WATER_INTERVAL, 0x20 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WC_ZONE_ANGLE, 0x24 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WC_ZONE_WIDTH, 0x28 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WC_ZONE_MARGIN, 0x2C },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WC_OFFSET_Z, 0x30 },
        { S03_PRM_TYPE_FLOAT, &HOCKEY_PRM_S03_WC_LOC_RADIUS, 0x34 },
        { S03_PRM_TYPE_LONG, &HOCKEY_PRM_S03_WC_LOC_NUM, 0x38 },
        { S03_PRM_TYPE_LONG, &HOCKEY_PRM_S03_WC_LOC_NUM_PER_GRP, 0x3C },
        { S03_PRM_TYPE_LONG, &HOCKEY_PRM_S03_DEB_DRAW_INFO, 0x40 },
        { S03_PRM_TYPE_END, 0, 0 },
    };

    // (names guessed) Two sets of pillar places in front of a goal: two
    // pillars, and three. Nothing in HockeyF reads the table; it is filled
    // by the file's static initialiser.
    struct S03_WATER_LOC {
        long m_nNum;           // 0x00
        bcs::FVEC3 m_aPos[12]; // 0x04, m_nNum used
    };

    static S03_WATER_LOC s_WaterLoc[] = {
        { 2, { bcs::FVEC3(-2.679f, 0.0f, -6.509f), bcs::FVEC3(2.679f, 0.0f, -6.509f) } },
        { 3, { bcs::FVEC3(-4.25f, 0.0f, -8.861f), bcs::FVEC3(0.0f, 0.0f, -5.6f), bcs::FVEC3(4.25f, 0.0f, -8.861f) } },
    };
}

// Sound slots and sounds of the stage.
enum {
    SE_SLOT_AMBIENT = 1,
    SE_SLOT_AMBIENT2 = 2,
    SE_SLOT_EVENT = 3,
    SE_AMBIENT = 0xD18,
    SE_AMBIENT2 = 0xD19,
    SE_EVENT = 0xD1A
};

// Stage messages handled here (sent through CStageMan::SendMessage).
enum {
    MSG_STOP_WATER = 0x25, // arg1 1: stop every pillar
    MSG_GUARD_TEAM = 0x27  // arg1 1: arg2 is the team whose guarded goal keeps its pillars stopped
};

const void *msw::CStageHockey03::GetPrmTbl() const
{
    return s_PrmTbl;
}

msw::CStageHockey03::CStageHockey03(bcs::CTask *pParent)
    : CStageManHockey(pParent), m_Cluster0(this), m_Cluster1(this)
{
    m_fUnk2A8 = 0.0f;
    m_nUnk2AC = -1;
    m_nUnk2B0 = 0;
    m_nGuardTeam = -1;
    m_apCluster[0] = &m_Cluster0;
    m_apCluster[1] = &m_Cluster1;
}

msw::CStageHockey03::~CStageHockey03()
{
}

void msw::CStageHockey03::Main_(float frame)
{
    CStageManHockey::Main_(frame);
    Water_Update(frame);
}

void msw::CStageHockey03::Water_Update(float rate)
{
    long ballTeam = GetBallTeam();

    for (long i = 0; i < 2; i++) {
        CStgGmk_WaterPillarCluster *pCluster = m_apCluster[i];
        if (pCluster == 0) {
            continue;
        }

        pCluster->SetGuardGoalIdx(i);
        pCluster->SetCenterBasePos(GetGoalPos(i));
        pCluster->SetVec(GetGoalVec(i));

        if (m_nGuardTeam >= 0 && i == CGame::GetGuardGoalIdx(m_nGuardTeam)) {
            pCluster->StopAll();
        } else if (ballTeam >= 0 && ballTeam != CGame::GetTeamIdxByGoalIdx(i)) {
            pCluster->StopAll();
        } else {
            pCluster->Main(rate);
        }
    }
}

void msw::CStageHockey03::Water_StopAll()
{
    for (long i = 0; i < 2; i++) {
        if (m_apCluster[i] != 0) {
            m_apCluster[i]->StopAll();
        }
    }
}

void msw::CStageHockey03::Water_ResetAll()
{
    for (long i = 0; i < 2; i++) {
        if (m_apCluster[i] != 0) {
            m_apCluster[i]->Reset();
        }
    }
}

void msw::CStageHockey03::SendMessage(long msg, long arg1, long arg2, long arg3)
{
    CStageManHockey::SendMessage(msg, arg1, arg2, arg3);

    switch (msg) {
        case MSG_GUARD_TEAM:
            if (arg1 == 1) {
                m_nGuardTeam = arg2;
            }
            break;
        case MSG_STOP_WATER:
            if (arg1 == 1) {
                Water_StopAll();
            }
            break;
    }
}

const msw::STAGE::MDL_GEN_RULE *msw::CStageHockey03::GetMdlGenRule_() const
{
    // Only the end entry, which chains to the table of CStageManHockey.
    static STAGE::MDL_GEN_RULE tbl[] = {
        { 0, reinterpret_cast<long>(CStageManHockey::GetMdlGenRule_()), 0, 0, 0 },
    };

    return tbl;
}

bool msw::CStageHockey03::OnPreCreateMdl(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, nw4r::g3d::ResFile, CArn &, long)
{
    return true;
}

void msw::CStageHockey03::OnPostInitialize()
{
    CStageManHockey::OnPostInitialize();
}

// The pillars are made when the stage enters the scene.
void msw::CStageHockey03::OnSceneEntry()
{
    for (long i = 0; i < 2; i++) {
        CStgGmk_WaterPillarCluster *pCluster = m_apCluster[i];
        if (pCluster != 0) {
            pCluster->CreateGmk();
            pCluster->SetGuardGoalIdx(i);
        }
    }
}

void msw::CStageHockey03::OnGimmickReset()
{
    m_fUnk2A8 = 0.0f;
    m_nUnk2AC = -1;
    Water_ResetAll();
}

void msw::CStageHockey03::OnGimmickStart()
{
    m_fUnk2A8 = 0.0f;
}

void msw::CStageHockey03::OnGimmickStop()
{
    m_fUnk2A8 = 0.0f;
    Water_StopAll();
}

void msw::CStageHockey03::OnGimmickCancel()
{
    Water_ResetAll();
}

void msw::CStageHockey03::OnGimmickPause()
{
}

void msw::CStageHockey03::OnGimmickResume()
{
    m_fUnk2A8 = 0.0f;
}

unsigned long msw::CStageHockey03::OnCalcSum() const
{
    unsigned long sum = 0;

    for (long i = 0; i < 2; i++) {
        if (m_apCluster[i] != 0) {
            sum += m_apCluster[i]->CalcSum();
        }
    }

    return sum;
}

void msw::CStageHockey03::OnSoundEvent(long event)
{
    switch (event) {
        case 0:
            if (!SE_IsAlive(SE_SLOT_AMBIENT)) {
                SE_RepeatPlay(SE_SLOT_AMBIENT, SE_AMBIENT, 180.0f, 420.0f, -1.0f, 0.0f, 0xC);
            }
            if (!SE_IsAlive(SE_SLOT_AMBIENT2)) {
                SE_Play(SE_SLOT_AMBIENT2, SE_AMBIENT2, 0.0f, 3);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            SE_Play(SE_SLOT_EVENT, SE_EVENT, 0.0f, 0);
            break;
    }
}

long msw::CStageHockey03::GetWaterPillarMaxNum()
{
    CStageHockey03 *pStage = static_cast<CStageHockey03 *>(CStageMan::GetInstance());
    long num = 0;

    for (long i = 0; i < 2; i++) {
        if (pStage->m_apCluster[i] != 0) {
            num += pStage->m_apCluster[i]->GetMaxNum();
        }
    }

    return num;
}

msw::CStgGmk_WaterPillar *msw::CStageHockey03::GetWaterPillar(long idx)
{
    CStageHockey03 *pStage = static_cast<CStageHockey03 *>(CStageMan::GetInstance());

    if (idx < 0) {
        return 0;
    }

    long base = 0;
    CStgGmk_WaterPillar *pPillar = 0;
    for (long i = 0; i < 2; i++) {
        if (pStage->m_apCluster[i] != 0) {
            long num = pStage->m_apCluster[i]->GetMaxNum();
            if (idx < base + num) {
                pPillar = static_cast<CStgGmk_WaterPillar *>(pStage->m_apCluster[i]->GetGmk_(idx - base));
                break;
            }
            base += num;
        }
    }

    if (pPillar == 0 || !pPillar->IsSwoosh()) {
        return 0;
    }
    return pPillar;
}

void msw::CStageHockey03::Disp()
{
    CStageMan::Disp();
}

long msw::CStageHockey03::SE_GetSlotNum() const
{
    return 4;
}
