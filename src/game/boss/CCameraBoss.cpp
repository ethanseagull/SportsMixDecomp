// msw::CCameraBoss: the camera of the boss game. It looks at the boss from
// behind the players and backs off to keep the ball and the characters in
// view.
#include "game/msw/CCameraBoss.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CNpcBase.hpp"
#include "game/msw/CStageMan.hpp"

namespace {
    // (guessed helpers) The horizontal distance between two points, the
    // horizontal direction from one to another, and a vector brought to a
    // horizontal length with its height kept.
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

    // rPos brought inside the circle of radius fRadius about rCenter.
    inline void ClampXZ(bcs::FVEC3 &rPos, const bcs::FVEC3 &rCenter, float fRadius)
    {
        if (DistXZ(rCenter, rPos) > fRadius) {
            bcs::FVEC3 dir;
            DirXZ(dir, rCenter, rPos);
            SetLenXZ(dir, fRadius);
            bcs::FVEC3Add(&rPos, &rCenter, &dir);
        }
    }
}

void msw::CCameraBoss::Initialize()
{
    CGameCamera::Initialize();

    for (long i = 0; i < 4; i++) {
        g_pInstance[i] = 0;
    }

    g_nInstanceNum = bcs::CWorldEnv::GetViewSplitNum();

    for (long i = 0; i < g_nInstanceNum; i++) {
        g_pInstance[i] = new CCameraBoss(i);
        g_pInstance[i]->SetActive(-1);
    }
}

msw::CCameraBoss::CCameraBoss(long view) : CGameCamera(view, 0x80)
{
    float dist;
    bcs::CWorldEnv::SetViewFovy(view, 40.0f);

    // The sum is computed at run time in the original (fadds), as in
    // CCameraBasketHB, so the distance is a local here.
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
    m_nLookChr = 0;
    m_FromPos.Set(0.0f, 0.0f, 0.0f);
    m_BossPos.Set(0.0f, 0.0f, 0.0f);
    m_LookPos.Set(0.0f, 0.0f, 0.0f);
    SetCamParam();
}

void msw::CCameraBoss::MainCamera()
{
    if (!CanbeMain()) {
        return;
    }

    CGameCamera::MainCamera();
    bcs::CWorldEnv::SetViewFovy(m_nView, m_fFovy);

    m_BossPos.Set(0.0f, 0.0f, 0.0f);
    m_LookPos.Set(0.0f, 0.0f, 0.0f);

    CNpcBase *pBoss = CChrMan::NpcSearch(0, 0);
    if (pBoss != 0) {
        m_BossPos = pBoss->GetPos();
        m_BossPos.y = 0.0f;
        m_LookPos.Set(m_BossPos.x, 0.0f, m_BossPos.z);
    }

    // Without bit 0x20 the camera looks at a point 4 beyond the boss as
    // seen from the middle of the court.
    bcs::FVEC3 center(0.0f, 0.0f, 0.0f);
    if (!(m_UnkFC & 0x20)) {
        float fDist = DistXZ(center, m_BossPos);
        bcs::FVEC3 dir;
        DirXZ(dir, center, m_BossPos);
        SetLenXZ(dir, fDist + 4.0f);
        m_LookPos.x = center.x + dir.x;
        m_LookPos.z = center.z + dir.z;
    }

    SetLookChr();
    int bForce = 0;
    if (!(m_UnkFC & 0x20)) {
        bForce = 1;
    }
    SetCamMode(m_nLookChr, bForce);
    if (m_UnkFC & 0x1) {
        m_Unk1C0 = 0.0f;
    }
    MainCamera_Normal();
    CalcCamData(m_fCamDataRate);
}

bool msw::CCameraBoss::CanbeMain() const
{
    return true;
}

void msw::CCameraBoss::MainCamera_Normal()
{
    // Declared in the order of the original's stack layout (the last
    // declared at the lowest address); the order changes nothing else.
    bcs::FVEC3 target;
    bcs::FVEC3 camPos;
    bcs::FVEC3 lookPos;
    bcs::FVEC3 dir;
    bcs::FVEC3 at;
    float fLookRate = m_fLookRate;
    bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
    long mask;

    if (m_UnkFC & 0x20) {
        // Follow one character, at a height in proportion to its own.
        GetTeamNo();
        CChrCore *pCore = CChrMan::GetChrCore(m_nLookChr);
        look = GetCorePosXZ(pCore);
        arg3.y = 0.0f;
        arg3.y = pCore->GetPosY() * m_fLookHeightRate + arg3.y;
        mask = 0;
    } else {
        mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
    }

    at = look;
    CalcLookPos(at, arg2, arg3, fLookRate, mask);

    camPos = m_Unk118;
    lookPos = m_LookPos;
    target = m_LookPos;
    float f280 = m_Unk280;
    float f284 = m_Unk284;
    float f288 = m_Unk288;

    if (!(m_UnkFC & 0x20)) {
        float fRate = 0.99f;
        if (m_UnkFC & 0x1) {
            fRate = 0.0f;
        }
        float fLen = GetLenHosei(camPos, target);
        m_Unk250 = fRate * (m_Unk250 - fLen) + fLen;
        f288 += m_Unk250;
    }

    // Look 100 beyond the look point, on the ground.
    DirXZ(dir, camPos, lookPos);
    SetLenXZ(dir, 100.0f);
    bcs::FVEC3Add(&target, &lookPos, &dir);
    target.y = 0.0f;

    MakeingFrontCamDataBase(camPos, target, f280, f284, f288, m_Unk28C, m_Unk290, 0.0f, 0.0f, m_Unk1C0);
}

float msw::CCameraBoss::GetLenHosei(const bcs::FVEC3 &rCamPos, const bcs::FVEC3 &rTarget)
{
    float fRadius = CStageMan::GetInstance()->GetCourtRadius() - 3.0f;
    float fMaxLen = 0.0f;
    float fMaxDist = 0.0f;
    bcs::FVEC3 camPos(rCamPos.x, 0.0f, rCamPos.z);
    bcs::FVEC3 target(rTarget.x, 0.0f, rTarget.z);
    bcs::FVEC3 dir;
    bcs::FVEC3 diff;
    bcs::FVEC3Sub(&diff, &target, &camPos);
    bcs::BCS_Vec_Normalize(dir, diff);

    // -2 is the ball, -1 nothing, then the characters.
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
            if (pCore == 0 || pCore->IsDisable()) {
                continue;
            }
            pos = GetCorePosXZ(pCore);
            ClampXZ(pos, bcs::FVEC3(0.0f, 0.0f, 0.0f), fRadius);
            pos.y = 0.0f;
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

    float fLenHosei = 0.8f * fMaxDist + 0.5f * fMaxLen;
    if (fLenHosei > m_fMaxLenHosei) {
        fLenHosei = m_fMaxLenHosei;
    }
    return fLenHosei;
}

long msw::CCameraBoss::GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    GetTeamNo();

    bcs::FVEC3 lookPos = m_LookPos;
    bcs::FVEC3 fromPos = m_FromPos;
    float fRadius = CStageMan::GetInstance()->GetCourtRadius() - 7.0f;

    // The characters in play, brought inside the court.
    bcs::FVEC3 pos[4];
    long nNum = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pCore = CChrMan::GetChrCore(i);
        if (pCore == 0 || pCore->IsDisable()) {
            continue;
        }
        bcs::FVEC3 &rPos = pos[nNum];
        rPos = GetCorePosXZ(pCore);
        ClampXZ(rPos, bcs::FVEC3(0.0f, 0.0f, 0.0f), fRadius);
        rPos.y = 0.0f;
        nNum++;
    }

    // The point to look from: the one character, the middle of two, or
    // where the lines from two characters through the middle of the other
    // two cross.
    if (nNum == 2) {
        bcs::FVEC3 half;
        bcs::FVEC3Sub(&half, &pos[1], &pos[0]);
        half.x *= 0.5f;
        half.z *= 0.5f;
        fromPos.x = half.x + pos[0].x;
        fromPos.z = half.z + pos[0].z;
    } else if (nNum == 3) {
        bcs::FVEC3 half;
        bcs::FVEC3Sub(&half, &pos[1], &pos[0]);
        half.x *= 0.5f;
        half.z *= 0.5f;
        float mid01x = half.x + pos[0].x;
        float mid01z = half.z + pos[0].z;
        bcs::FVEC3Sub(&half, &pos[2], &pos[0]);
        half.x *= 0.5f;
        half.z *= 0.5f;
        float mid02x = half.x + pos[0].x;
        float mid02z = half.z + pos[0].z;

        float x;
        float z;
        int bCross = bcs::BCS_GetSegLineToSegLineCross(&x, &z, mid01x, mid01z, pos[2].x, pos[2].z, mid02x, mid02z,
                                                       pos[1].x, pos[1].z);
        if (bCross) {
            fromPos.Set(x, 0.0f, z);
        }
        if (!bCross) {
            fromPos.Set(0.0f, 0.0f, 0.0f);
        }
    } else if (nNum >= 1) {
        fromPos = pos[0];
    }

    // Turn the old direction from the look point slowly towards the new one
    // (more slowly when it has to turn more than a right angle), and keep
    // the distance of the new point.
    float fDist = DistXZ(fromPos, lookPos);
    bcs::FVEC3 oldDir(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 newDir(0.0f, 0.0f, 0.0f);
    DirXZ(oldDir, m_FromPos, lookPos);
    DirXZ(newDir, fromPos, lookPos);

    bcs::FVEC3 dir;
    if (bcs::FVEC3Dot(&oldDir, &newDir) <= 0.0f) {
        bcs::BCS_Vec_Slerp(dir, oldDir, newDir, 0.007f);
    } else {
        bcs::BCS_Vec_Slerp(dir, oldDir, newDir, 0.01f);
    }
    dir.x *= -fDist;
    dir.z *= -fDist;
    m_FromPos.x = dir.x + lookPos.x;
    m_FromPos.z = dir.z + lookPos.z;

    // Look 6 beyond the look point, away from the new place to look from.
    DirXZ(dir, lookPos, m_FromPos);
    SetLenXZ(dir, 6.0f);
    bcs::FVEC3Add(&rLook, &lookPos, &dir);
    rArg3.y = 0.0f;
    return 0;
}

void msw::CCameraBoss::Reset()
{
    CGameCamera::Reset();
    // (the result is not used in the original either)
    GetTeamNo();
    m_UnkFC &= ~0x2;
    SetCamParam();
}

void msw::CCameraBoss::SetLookChr()
{
    CChrCore *pCore = GetHoldBallChrCore(-1);

    if (pCore != 0 && pCore->IsPadCtrl()) {
        m_nLookChr = pCore->GetPlayerNo();
        return;
    }

    // The character followed so far is not a player's: take the first one
    // a pad controls.
    pCore = CChrMan::GetChrCore(m_nLookChr);
    if (pCore != 0 && !pCore->IsPadCtrl()) {
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pOther = CChrMan::GetChrCore(i);

            if (pOther != 0 && !pOther->IsDisable() && pOther->IsPadCtrl()) {
                m_nLookChr = pOther->GetPlayerNo();
                return;
            }
        }
    }
}

void msw::CCameraBoss::SetCamParam()
{
    if (m_UnkFC & 0x20) {
        if (m_UnkFC & 0x8) {
            m_fFovy = 50.0f;
            m_Unk280 = 2.25f;
            m_Unk284 = 2.0f;
            m_Unk288 = 9.0f;
            m_Unk28C = 0.92f;
            m_Unk290 = 0.99f;
            m_fLookRate = 0.937f;
            m_fLookHeightRate = 0.35f;
            m_fCamDataRate = 0.964355f;
            m_fMaxLenHosei = 4.0f;
        } else {
            m_fFovy = 50.0f;
            m_Unk280 = 3.0f;
            m_Unk284 = 2.5f;
            m_Unk288 = 11.0f;
            m_Unk28C = 0.92f;
            m_Unk290 = 0.99f;
            m_fLookRate = 0.937f;
            m_fLookHeightRate = 0.3f;
            m_fCamDataRate = 0.964355f;
            m_fMaxLenHosei = 4.0f;
        }
    } else {
        if (m_UnkFC & 0x8) {
            m_fFovy = 50.0f;
            m_Unk280 = 4.9f;
            m_Unk284 = 3.8f;
            m_Unk288 = 10.999998f;
            m_Unk28C = 0.9f;
            m_Unk290 = 0.9f;
            m_fLookRate = 0.999f;
            m_fLookHeightRate = 0.42f;
            m_fCamDataRate = 0.964355f;
            m_fMaxLenHosei = 25.0f;
        } else {
            m_fFovy = 50.0f;
            m_Unk280 = 4.9f;
            m_Unk284 = 3.8f;
            m_Unk288 = 11.000002f;
            m_Unk28C = 0.9f;
            m_Unk290 = 0.9f;
            m_fLookRate = 0.999f;
            m_fLookHeightRate = 0.42f;
            m_fCamDataRate = 0.964355f;
            m_fMaxLenHosei = 30.0f;
        }
    }
}
