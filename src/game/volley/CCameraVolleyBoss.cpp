// msw::CCameraVolleyBoss: the camera of the volley boss game. It looks at
// the boss from behind the camera's team, backs off to keep the ball and the
// other characters in view and zooms in while the team's leader is about to
// touch the ball.
#include "game/msw/CCameraVolleyBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNpcBase.hpp"

namespace {
    // (guessed helpers, as in BossF's CCameraBoss) The horizontal distance
    // between two points, the horizontal direction from one to another, and
    // a vector brought to a horizontal length with its height kept.
    inline float DistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        return bcs::FVEC3Dist(&from, &to);
    }

    inline void DirXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rTo.x - rFrom.x, 0.0f, rTo.z - rFrom.z));
    }

    inline void SetLenXZ(bcs::FVEC3 &rVec, float fLen)
    {
        float y = rVec.y;
        bcs::BCS_Vec_Normalize(rVec, bcs::FVEC3(rVec.x, 0.0f, rVec.z));
        rVec.x *= fLen;
        rVec.z *= fLen;
        rVec.y = y;
    }

    // (guessed helper) From fTo towards fFrom by fRate.
    inline float Blend(float fRate, float fFrom, float fTo)
    {
        return fRate * (fFrom - fTo) + fTo;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        if (fValue > fMax) {
            return fMax;
        }
        if (fValue < fMin) {
            return fMin;
        }
        return fValue;
    }
}

msw::CCameraVolleyBoss::CCameraVolleyBoss(long view) : CCameraVolley(view)
{
    float dist;
    bcs::CWorldEnv::SetViewFovy(view, 40.0f);

    m_Unk100 = 9.5f;
    dist = 14.5f;
    float y = -(4.5f + dist);

    m_Unk104 = dist;
    m_Unk108 = 0.0f;
    m_Unk10C = 0.0f;
    m_Unk110 = y;
    m_Unk1D8 = 0;
    m_Unk1BC = 0;
    m_Unk1C0 = 0.0f;
    m_Unk1C4 = 0.0f;
    m_Unk1C8 = 0;
    m_Unk250 = 0.0f;
    SetCamParam();
}

int msw::CCameraVolleyBoss::MakeingCamZoom()
{
    if (m_UnkFC & 0x1) {
        m_Unk1C0 = 0.0f;
        return 1;
    }

    CChrCore *pLeader = GetMyTeamLeaderCore();
    CBall *pBall = GetActiveBall();
    if (pLeader == 0 || pBall == 0) {
        return 0;
    }

    bcs::FVEC3 chrPos = pLeader->GetPos();
    bcs::FVEC3 targetPos = m_Unk234;
    unsigned long nTarget = m_Unk230;

    if (m_Unk1BC == 0) {
        // Start zooming when the leader may touch the ball, stands within 3
        // of where it comes down and it comes down in less than 55 frames.
        if (pLeader->IsVolleyRightBallTouch(pBall->GetBallNo())) {
            long nUserTeam = CGame::g_GameWork.m_nUserTeam;
            if (nUserTeam == GetTeamNo() && m_Unk228 == 0 && nTarget != 0) {
                float fDist = DistXZ(chrPos, targetPos);
                if (pBall->GetTouchDownFrame(0.0f) < 55.0f && fDist < 3.0f) {
                    m_Unk1BC = 1;
                    m_Unk1C4 = 0.0f;
                    m_Unk1C8 = pBall->GetTouchNum();
                }
            }
        }
    } else if (m_Unk1BC == 1) {
        // Stop when the ball has been touched.
        if (nTarget == 0 || m_Unk228 != 0 || pBall->GetTouchNum() != m_Unk1C8) {
            m_Unk1BC = 0;
            m_Unk1C4 = 0.0f;
        }
    }

    if (m_Unk1BC == 0) {
        m_Unk1C0 = Blend(0.9f, m_Unk1C0, 0.0f);
    } else if (m_Unk1BC == 1) {
        m_Unk1C0 += 0.04f;
        m_Unk1C0 = Clamp(m_Unk1C0, 0.0f, 2.2f);
    }
    m_Unk1C4 += GetFrameRateF();
    return 1;
}

void msw::CCameraVolleyBoss::MainCamera()
{
    if (!CanbeMain()) {
        return;
    }

    CGameCamera::MainCamera();
    bcs::CWorldEnv::SetViewFovy(m_nView, m_fFovy);
    SetCamMode(0, 0);
    MakeingCamZoom();
    MainCamera_Normal();
    CalcCamData(0.964355f);
}

void msw::CCameraVolleyBoss::MainCamera_Normal()
{
    // Declared in the order of the original's stack layout (the last
    // declared at the lowest address); the order changes nothing else.
    bcs::FVEC3 target;
    bcs::FVEC3 camPos;
    bcs::FVEC3 bossPos;
    bcs::FVEC3 dir;
    bcs::FVEC3 at;
    float fLookRate = m_fLookRate;
    bcs::FVEC3 arg3;
    bcs::FVEC3 arg2;
    bcs::FVEC3 look;
    look.Set(0.0f, 0.0f, 0.0f);
    arg2.Set(0.0f, 0.0f, 0.0f);
    arg3.Set(0.0f, 0.0f, 0.0f);

    // Follow the team's character.
    look = GetCorePosXZ(CChrMan::GetChrCore(GetTeamNo()));
    at = look;
    CalcLookPos(at, arg2, arg3, fLookRate, 0);

    camPos = m_Unk118;
    bossPos.Set(0.0f, 0.0f, 0.0f);
    float f258 = m_Unk258;
    float f25C = m_Unk25C;
    float f260 = m_Unk260;

    CNpcBase *pBoss = CChrMan::NpcSearch(0, 0);
    if (pBoss != 0) {
        bossPos.x = pBoss->GetPos().x;
        bossPos.z = pBoss->GetPos().z;
        bossPos.y = 0.0f;
    }
    target = bossPos;

    float fRate = 0.99f;
    if (m_UnkFC & 0x1) {
        fRate = 0.0f;
    }
    float fLen = GetLenHosei(camPos, target);
    m_Unk250 = fRate * (m_Unk250 - fLen) + fLen;
    f260 += m_Unk250;

    // Look 100 beyond the boss, on the ground.
    dir.Set(0.0f, 0.0f, 0.0f);
    DirXZ(dir, camPos, bossPos);
    SetLenXZ(dir, 100.0f);
    bcs::FVEC3Add(&target, &bossPos, &dir);
    target.y = 0.0f;

    MakeingFrontCamDataBase(camPos, target, f258, f25C, f260, m_Unk264, m_Unk268, 0.0f, 0.0f, m_Unk1C0);
}

float msw::CCameraVolleyBoss::GetLenHosei(const bcs::FVEC3 &rCamPos, const bcs::FVEC3 &rTarget)
{
    float fMaxLen = 0.0f;
    float fMaxDist = fMaxLen;

    CChrCore *pMine = CChrMan::GetChrCore(GetTeamNo());
    if (pMine == 0) {
        return fMaxLen;
    }

    bcs::FVEC3 camPos(rCamPos.x, 0.0f, rCamPos.z);
    bcs::FVEC3 target(rTarget.x, 0.0f, rTarget.z);
    bcs::FVEC3 dir(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 diff;
    bcs::FVEC3Sub(&diff, &target, &camPos);
    bcs::BCS_Vec_Normalize(dir, diff);

    // -2 is the ball, -1 nothing, then the characters other than the
    // camera's own and the one holding the ball.
    for (long i = -2; i < CChrMan::GetChrCoreNum(); i++) {
        bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);

        if (i == -2) {
            CBall *pBall = GetActiveBall();
            if (pBall == 0) {
                continue;
            }
            pos = GetBallPosXZ(pBall);
        } else {
            if (i == -1) {
                continue;
            }
            CChrCore *pCore = CChrMan::GetChrCore(i);
            if (pCore == 0 || pCore->IsDisable() || pCore->IsHoldBall() || i == pMine->GetPlayerNo()) {
                continue;
            }
            pos = GetCorePosXZ(pCore);
        }

        bcs::FVEC3 cross;
        if (!bcs::BCS_GetPosToLineCross(cross, pos, camPos, dir)) {
            continue;
        }
        cross.y = 0.0f;

        // How far the point is off the line of sight, and how far along it.
        float fLen = DistXZ(cross, pos);
        float fDist = DistXZ(cross, camPos);
        bcs::FVEC3 back;
        bcs::FVEC3 toCam;
        bcs::FVEC3Sub(&toCam, &camPos, &cross);
        bcs::BCS_Vec_Normalize(back, toCam);

        if (bcs::FVEC3Dot(&back, &dir) >= 0.0f) {
            // Behind the camera.
            if (fDist > fMaxDist) {
                fMaxDist = fDist;
            }
        } else {
            fLen *= 1.0f - GetRate(fDist, 0.0f, 15.0f);
        }
        if (fLen > fMaxLen) {
            fMaxLen = fLen;
        }
    }

    float fLenHosei = 0.7f * fMaxDist + 0.3f * fMaxLen;
    if (fLenHosei > m_fMaxLenHosei) {
        fLenHosei = m_fMaxLenHosei;
    }
    return fLenHosei;
}

void msw::CCameraVolleyBoss::Reset()
{
    CGameCamera::Reset();
    // (the result is not used in the original either)
    GetTeamNo();
    m_UnkFC &= ~0x2;
    SetCamParam();
}

msw::CCameraVolleyBoss::~CCameraVolleyBoss()
{
}
