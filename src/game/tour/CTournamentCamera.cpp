#include "game/msw/CTournamentCamera.hpp"

bool msw::CInterpLockCamera::_SetRate(float add, float mul, float min)
{
    m_fRateAdd = add;
    m_fRateMul = mul;
    m_fRateMin = min;
    m_bEnd = true;
    m_fTotal = 0.0f;

    if (m_bDirect || add <= -1.0f || mul <= 0.0f) {
        m_bDirect = false;
        m_fTime = 0.0f;
        return true;
    }

    // Nothing to do if start and end are closer than `min` in every part.
    // __fabs is the compiler's fabs instruction; a float wrapper adds a frsp.
    if (bcs::FVEC3Dist(&m_Start.at, &m_End.at) < min && bcs::FVEC3Dist(&m_Start.dir, &m_End.dir) < min &&
        __fabs(m_Start.dist - m_End.dist) < min && __fabs(m_Start.roll - m_End.roll) < min &&
        __fabs(m_Start.fovy - m_End.fovy) < min) {
        return true;
    }

    float rate = 1.0f;
    float time = 0.0f;

    while (rate > min) {
        rate += add;
        rate *= mul;
        time += m_fStep;
    }
    m_fTotal = time;
    m_fTime = 0.0f;
    m_bEnd = false;
    return false;
}

float msw::CInterpLockCamera::CalcRate(float time)
{
    if (time >= m_fTotal) {
        return 0.0f;
    }

    float rate = 1.0f;
    float t = 0.0f;

    while (t < time) {
        rate += m_fRateAdd;
        rate *= m_fRateMul;
        t += m_fStep;
        if (rate <= m_fRateMin) {
            rate = m_fRateMin;
            break;
        }
    }

    // The part of the next step not reached yet. One operation to a
    // statement: written as one expression the compiler inlines this function
    // into MainCamera, which the original does not.
    float rest = rate + m_fRateAdd;

    rest *= m_fRateMul;
    rest -= rate;
    rest *= t - time;
    rest /= m_fStep;
    return rate + rest;
}

msw::CTournamentCamera::CTournamentCamera(long view, short priority) : CInterpLockCamera(view, priority)
{
    m_pTarget = 0;
    SetUnk08(0xFC01);
}

void msw::CTournamentCamera::SetValueCamera(const TournamentData::CAMERA_INFO &rInfo, const bcs::FVEC3 &rPos)
{
    m_pTarget = 0;
    m_Offset = rInfo.ofs;

    bcs::FVEC3 at;

    bcs::FVEC3Sum(at, rPos, rInfo.ofs);
    SetAt(at);
    SetDirRot(rInfo.rot.x, rInfo.rot.y);
    SetRoll(rInfo.rot.z);
    SetDist(rInfo.dist);
    SetFovy(rInfo.fovy);
}

void msw::CTournamentCamera::SetValueCamera(const _CAMERA_INFO_LOOK &rInfo)
{
    m_pTarget = 0;
    m_Offset.Set(0.0f, 0.0f, 0.0f);
    SetAt(rInfo.at);
    SetDir(rInfo.dir);
    SetRoll(rInfo.roll);
    SetDist(rInfo.dist);
    SetFovy(rInfo.fovy);
}

void msw::CTournamentCamera::MainCamera()
{
    // Declared ahead of the block below: the stack layout depends on it.
    bcs::FVEC3 pos;
    bcs::FVEC3 at;
    float roll;
    float fovy;
    long view = m_nView;
    float step = GetFrameRateF();

    if (m_pTarget != 0) {
        bcs::FVEC3 target;

        bcs::FVEC3Sum(target, *m_pTarget, m_Offset);
        SetAt(target);
        _SetRate(m_fRateAdd, m_fRateMul, m_fRateMin);
    }
    Update(step);

    GetLook(pos, at, roll, fovy);
    while (roll > 6.2831855f) {
        roll -= 6.2831855f;
    }
    while (roll < 0.0f) {
        roll += 6.2831855f;
    }
    m_Data.pos = pos;
    m_Data.at = at;
    m_Data.roll = roll;
    if (fovy != bcs::CWorldEnv::GetViewFovy(view)) {
        bcs::CWorldEnv::SetViewFovy(view, fovy);
    }
}

int msw::CTournamentCamera::GetDataMode()
{
    return bcs::CWorldEnv::m_vatViewAspectType == 1;
}

int msw::CTournamentCamera::OnActive()
{
    m_Data = m_DataSave;
    return 0;
}
