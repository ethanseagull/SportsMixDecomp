// msw::CStgGmk_FirePillar_Hockey_Act and its gimmick and driver: the fire
// pillars of hockey party stage 86. They are water pillars that burn: a
// burning pillar hurts what it touches and shakes the camera. Their timing
// depends on the party game's pattern (GetPartyGameParam(0), four sets) and
// gets faster as the game time runs down (three phases, switched at
// HOCKEY_PRM_S86_FIRE_TIME1 and FIRE_TIME2 seconds).
#include "game/msw/CStgGmk_FirePillar_Hockey.hpp"

#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CStgSrt.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace {

    // The value of the party game's pattern (0 to 3).
    inline float SelectByPattern(const float &r0, const float &r1, const float &r2, const float &r3)
    {
        if (msw::CGame::GetPartyGameParam(0) == 0) {
            return r0;
        } else if (msw::CGame::GetPartyGameParam(0) == 1) {
            return r1;
        } else if (msw::CGame::GetPartyGameParam(0) == 2) {
            return r2;
        } else {
            return r3;
        }
    }
}

// Camera quakes (CGame::StartCameraQuake).
enum {
    QUAKE_HIT = 0x2F,
    QUAKE_PRESTART = 0x39,
    QUAKE_START = 0x3A
};

// Sounds.
enum {
    SE_FIRE = 0xDDB,
    SE_FIRE_START = 0xDDC
};

msw::CStgGmk_FirePillar_Hockey_Act::~CStgGmk_FirePillar_Hockey_Act()
{
}

// The attack collision covers the fire; it is off until UpdateColl turns it
// on.
void msw::CStgGmk_FirePillar_Hockey_Act::OnInit()
{
    CStgGmk_WaterPillar_Act::OnInit();

    // The sizes are read but not used here.
    float radius = GetParam_Radius();
    GetParam_Radius2();
    float height = GetParam_Height();

    // (guessed) The original's 1.0f and 0.0f come first in .rodata,
    // though WaterStartAnim_ is the first function to load them: code that
    // used the sizes with them was dropped. Behaviour-free stand-in, see
    // notes/decisions.md, "Dead float comparison".
    if (radius == 1.0f || height == 0.0f) {
    }

    m_AtkColl.Create(1, 5, 1, 0);
    m_AtkColl.SetCollType(0, 1);
    m_AtkColl.SetTargetFlag(0, 2);
    m_AtkColl.SetAtkType(0, 0x1003);
    RegistAttackHitCallback(m_AtkColl);
    m_AtkColl.SetEnable(0);
}

// The fire hurts while the body collision is on, in a cylinder of
// GetParam_Radius by GetParam_Height at the pillar.
void msw::CStgGmk_FirePillar_Hockey_Act::UpdateColl()
{
    CStgGmk_WaterPillar_Act::UpdateColl();

    const CStgSrt *pSrt = GetSrt();
    if (m_BodyColl.IsEnable(0)) {
        float radius = GetParam_Radius();
        GetParam_Radius2();
        float height = GetParam_Height();

        bcs::FVEC3 pos = pSrt->GetPos();
        m_AtkColl.SetPos(0, bcs::FVEC3(pos));
        m_AtkColl.SetSize(0, bcs::FVEC3(radius, height, radius));
        m_AtkColl.SetEnable(0);
    } else {
        m_AtkColl.SetDisableAll();
    }
}

void msw::CStgGmk_FirePillar_Hockey_Act::CtorState_PRESTART()
{
    CStgGmk_WaterPillar_Act::CtorState_PRESTART();
    CGame::StartCameraQuake(QUAKE_PRESTART, -1, -1);
}

void msw::CStgGmk_FirePillar_Hockey_Act::DtorState_PRESTART()
{
    CStgGmk_WaterPillar_Act::DtorState_PRESTART();
    CGame::StopCameraQuake(QUAKE_PRESTART);
}

void msw::CStgGmk_FirePillar_Hockey_Act::CtorState_START()
{
    CStgGmk_WaterPillar_Act::CtorState_START();
    CGame::StartCameraQuake(QUAKE_START, -1, -1);
}

// A character burnt by the fire shakes its own camera.
void msw::CStgGmk_FirePillar_Hockey_Act::OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                                     const bcs::FVEC3 &rPos, unsigned long flag)
{
    if (pBody == 0) {
        return;
    }
    if (pBody->GetOwnerKind() != 1 || flag != 1) {
        return;
    }
    CGame::StartCameraQuake(QUAKE_HIT, pBody->m_nOwnerIdx, -1);
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_Radius() const
{
    return HOCKEY_PRM_S86_FIRE_RADIUS;
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_Radius2() const
{
    return HOCKEY_PRM_S86_FIRE_RADIUS2;
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_Radius3() const
{
    return HOCKEY_PRM_S86_FIRE_RADIUS3;
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_Height() const
{
    return HOCKEY_PRM_S86_FIRE_HEIGHT;
}

// The times are given in seconds; the actor counts frames.

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_PrestartTime() const
{
    float time = CGame::g_GameWork.m_fGameTime;
    if (time < 60.0f * HOCKEY_PRM_S86_FIRE_TIME2) {
        return SelectByPattern(HOCKEY_PRM_S86_FIRE0_PRESTART_TIME2, HOCKEY_PRM_S86_FIRE1_PRESTART_TIME2,
                               HOCKEY_PRM_S86_FIRE2_PRESTART_TIME2, HOCKEY_PRM_S86_FIRE3_PRESTART_TIME2);
    } else if (time < 60.0f * HOCKEY_PRM_S86_FIRE_TIME1) {
        return SelectByPattern(HOCKEY_PRM_S86_FIRE0_PRESTART_TIME1, HOCKEY_PRM_S86_FIRE1_PRESTART_TIME1,
                               HOCKEY_PRM_S86_FIRE2_PRESTART_TIME1, HOCKEY_PRM_S86_FIRE3_PRESTART_TIME1);
    } else {
        return SelectByPattern(HOCKEY_PRM_S86_FIRE0_PRESTART_TIME0, HOCKEY_PRM_S86_FIRE1_PRESTART_TIME0,
                               HOCKEY_PRM_S86_FIRE2_PRESTART_TIME0, HOCKEY_PRM_S86_FIRE3_PRESTART_TIME0);
    }
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_WaitTime() const
{
    return 15.0f;
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_StayTime() const
{
    float time = CGame::g_GameWork.m_fGameTime;
    if (time < 60.0f * HOCKEY_PRM_S86_FIRE_TIME2) {
        return 60.0f * SelectByPattern(HOCKEY_PRM_S86_FIRE0_STAY_TIME2, HOCKEY_PRM_S86_FIRE1_STAY_TIME2,
                                       HOCKEY_PRM_S86_FIRE2_STAY_TIME2, HOCKEY_PRM_S86_FIRE3_STAY_TIME2);
    } else if (time < 60.0f * HOCKEY_PRM_S86_FIRE_TIME1) {
        return 60.0f * SelectByPattern(HOCKEY_PRM_S86_FIRE0_STAY_TIME1, HOCKEY_PRM_S86_FIRE1_STAY_TIME1,
                                       HOCKEY_PRM_S86_FIRE2_STAY_TIME1, HOCKEY_PRM_S86_FIRE3_STAY_TIME1);
    } else {
        return 60.0f * SelectByPattern(HOCKEY_PRM_S86_FIRE0_STAY_TIME0, HOCKEY_PRM_S86_FIRE1_STAY_TIME0,
                                       HOCKEY_PRM_S86_FIRE2_STAY_TIME0, HOCKEY_PRM_S86_FIRE3_STAY_TIME0);
    }
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_Interval() const
{
    float time = CGame::g_GameWork.m_fGameTime;
    if (time < 60.0f * HOCKEY_PRM_S86_FIRE_TIME2) {
        return 60.0f * SelectByPattern(HOCKEY_PRM_S86_FIRE0_INTERVAL2, HOCKEY_PRM_S86_FIRE1_INTERVAL2,
                                       HOCKEY_PRM_S86_FIRE2_INTERVAL2, HOCKEY_PRM_S86_FIRE3_INTERVAL2);
    } else if (time < 60.0f * HOCKEY_PRM_S86_FIRE_TIME1) {
        return 60.0f * SelectByPattern(HOCKEY_PRM_S86_FIRE0_INTERVAL1, HOCKEY_PRM_S86_FIRE1_INTERVAL1,
                                       HOCKEY_PRM_S86_FIRE2_INTERVAL1, HOCKEY_PRM_S86_FIRE3_INTERVAL1);
    } else {
        return 60.0f * SelectByPattern(HOCKEY_PRM_S86_FIRE0_INTERVAL0, HOCKEY_PRM_S86_FIRE1_INTERVAL0,
                                       HOCKEY_PRM_S86_FIRE2_INTERVAL0, HOCKEY_PRM_S86_FIRE3_INTERVAL0);
    }
}

long msw::CStgGmk_FirePillar_Hockey_Act::GetParam_Effect(long type) const
{
    switch (type) {
        case 4:
            return 0x4A9;
        case 7:
            return 0x4A8;
        case 8:
            return 0x4AA;
    }
    return -1;
}

long msw::CStgGmk_FirePillar_Hockey_Act::GetParam_SE(long type) const
{
    if (type == 0) {
        return SE_FIRE;
    }
    return -1;
}

float msw::CStgGmk_FirePillar_Hockey_Act::GetParam_MoveResist() const
{
    return HOCKEY_PRM_S86_FIRE_RESIST;
}

void msw::CStgGmk_FirePillar_Hockey_Act::WaterStartAnim_()
{
    CStgGmk_WaterPillar_Act::WaterStartAnim_();
    CGameSnd::Play(SE_FIRE_START, GetSrt()->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
}

msw::CStgGmk_FirePillar_Hockey_Drv::~CStgGmk_FirePillar_Hockey_Drv()
{
}

void msw::CStgGmk_FirePillar_Hockey_Drv::Main(float rate)
{
    CStgGmk_WaterPillar_Drv::Main(rate);
}

msw::CStgGmk_FirePillar_Hockey::CStgGmk_FirePillar_Hockey(bcs::CTask *pParent, int arg)
    : CStgGmk_WaterPillar(pParent, arg)
{
}

msw::CStgGmk_FirePillar_Hockey::~CStgGmk_FirePillar_Hockey()
{
}

msw::CStgMdl *msw::CStgGmk_FirePillar_Hockey::NewMdl(long heap, unsigned long idx)
{
    return 0;
}

msw::CStgDrv *msw::CStgGmk_FirePillar_Hockey::NewDrv(long heap)
{
    return new (heap) CStgGmk_FirePillar_Hockey_Drv(this);
}

msw::CStgAct *msw::CStgGmk_FirePillar_Hockey::NewAct(long heap)
{
    return new (heap) CStgGmk_FirePillar_Hockey_Act(this);
}
