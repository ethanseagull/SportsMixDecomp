// msw::CCameraHockeyGame: the camera of a hockey match. It picks a camera
// mode from the game state (attacking or defending, and the special views
// the game asks for), finds the point to look at, and places the camera on
// a curve read from the mode's parameters, which SetCamParam fills.
#include "game/msw/CCameraHockeyGame.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/CWorldEnv.hpp"

namespace {

    // The odd modes of each pair are the attacking views.
    inline bool IsOffenseMode(unsigned long mode)
    {
        switch (mode) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
            case 11:
            case 13:
            case 15:
            case 17:
                return true;
            default:
                return false;
        }
    }

    // The even modes of each pair are the defending views.
    inline bool IsDefenseMode(unsigned long mode)
    {
        switch (mode) {
            case 2:
            case 4:
            case 6:
            case 8:
            case 10:
            case 12:
            case 14:
            case 16:
            case 18:
                return true;
            default:
                return false;
        }
    }

    // The larger of a and b.
    inline float Max(float a, float b)
    {
        if (a > b) {
            b = a;
        }
        return b;
    }
}

msw::CCameraHockeyGame::CCameraHockeyGame(long view) : CCameraHockey(view)
{
    bcs::CWorldEnv::SetViewFovy(view, 40.0f);
    m_Unk250 = 0.0f;
    SetCamParam();
}

bool msw::CCameraHockeyGame::IsNormalCamMode() const
{
    if ((m_Unk228 == 1 || m_Unk228 == 2) && m_Unk224 < 0.1f) {
        return true;
    }
    return false;
}

void msw::CCameraHockeyGame::MainCamera()
{
    if (!CanbeMain()) {
        return;
    }

    CGameCamera::MainCamera();
    bcs::CWorldEnv::SetViewFovy(m_nView, m_fFovy);
    m_Unk100 = m_fNear;
    m_Unk104 = m_fFar;
    m_Unk108 = 0.0f;
    m_Unk10C = 0.0f;
    m_Unk110 = -(m_fFar + m_fBack);

    long mode;
    if (CGame::g_GameWork.m_nFlag18 & 0x8) {
        mode = 0;
    } else {
        mode = -1;
        long type = CGame::GetExceptionalCameraType();
        switch (type) {
            case 3:
                if (IsOffense()) {
                    mode = 11;
                } else {
                    mode = 12;
                }
                break;
            case 7:
                mode = IsOffense() ? 15 : 16;
                break;
        }
        if (type != 3 && IsSpModeForBaHo()) {
            mode = IsOffense() ? 17 : 18;
        }
        if (mode < 0) {
            mode = IsOffense() ? 1 : 2;
        }
    }
    SetCamMode(mode, 0);

    if (IsOffenseMode(m_Unk228)) {
        if (mode == 17) {
            float frontRate = m_fFrontRate;
            float slant;
            float scaleX;
            GetOffDefParam(&scaleX, &slant);
            float lookRate = m_fLookRate;
            bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
            long mask;
            if (IsSingleChrCamera()) {
                mask = GetFirstLookInfo_SinglePlay(look, arg2, arg3);
            } else {
                mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
            }
            bcs::FVEC3 pos = CalcLookInfo(look, arg2, arg3, lookRate, mask);
            float len;
            float tarY;
            float posY;
            GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
            pos.x *= scaleX;
            bcs::FVEC3 target;
            MakeingOffSlantCamTar(target, pos, GetUnk108Vec(), slant);
            MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
        } else {
            float frontRate = m_fFrontRate;
            float slant;
            float scaleX;
            GetOffDefParam(&scaleX, &slant);
            float lookRate = m_fLookRate;
            bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
            long mask;
            if (IsSingleChrCamera()) {
                mask = GetFirstLookInfo_SinglePlay(look, arg2, arg3);
            } else {
                mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
            }
            bcs::FVEC3 pos = CalcLookInfo(look, arg2, arg3, lookRate, mask);
            float len;
            float tarY;
            float posY;
            GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
            pos.x *= scaleX;
            bcs::FVEC3 target(pos.x, 0.0f, m_Unk110);
            MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
        }
    } else if (IsDefenseMode(m_Unk228)) {
        if (mode == 18) {
            float frontRate = m_fFrontRate;
            float slant;
            float scaleX;
            GetOffDefParam(&scaleX, &slant);
            float lookRate = m_fLookRate;
            bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
            bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
            long mask;
            if (IsSingleChrCamera()) {
                mask = GetFirstLookInfo_SinglePlay(look, arg2, arg3);
            } else {
                mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
            }
            bcs::FVEC3 pos = CalcLookInfo(look, arg2, arg3, lookRate, mask);
            float len;
            float tarY;
            float posY;
            GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
            pos.x *= scaleX;
            bcs::FVEC3 target;
            MakeingDefSlantCamTar(target, pos, GetUnk108Vec(), slant);
            MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
        } else {
            float frontRate = m_fFrontRate;
            float posY;
            float tarY;
            float len;
            float scaleX;
            float slant;
            GetOffDefParam(&scaleX, &slant);
            float lookRate = m_fLookRate;
            bcs::FVEC3 target;
            bcs::FVEC3 pos;
            bcs::FVEC3 arg3;
            bcs::FVEC3 arg2;
            bcs::FVEC3 look;
            look.Set(0.0f, 0.0f, 0.0f);
            arg2.Set(0.0f, 0.0f, 0.0f);
            arg3.Set(0.0f, 0.0f, 0.0f);
            long mask;
            if (IsSingleChrCamera()) {
                mask = GetFirstLookInfo_SinglePlay(look, arg2, arg3);
            } else {
                mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
            }
            pos = CalcLookInfo(look, arg2, arg3, lookRate, mask);
            GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
            pos.x *= scaleX;
            target.Set(pos.x, 0.0f, m_Unk110);
            MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
        }
    } else {
        float frontRate = m_fFrontRate;
        float lookRate = m_fLookRate;
        bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
        long mask;
        if (IsSingleChrCamera()) {
            mask = GetFirstLookInfo_SinglePlay(look, arg2, arg3);
        } else {
            mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
        }
        bcs::FVEC3 pos = CalcLookInfo(look, arg2, arg3, lookRate, mask);
        bcs::FVEC3 target(pos.x - m_fNormalOffX, 0.0f, m_Unk110);
        MakeingFrontCamData(pos, target, m_fNormalPosY, m_fNormalTarY, m_fNormalLen, frontRate, 0.0f, m_Unk1C0);
    }

    CalcCamData(GetCamModeLerpRate());
}

long msw::CCameraHockeyGame::GetFirstLookInfo_SinglePlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    CBall *pBall = GetActiveBall();
    CChrCore *pCore = GetFirstPadChrCore();

    if (pBall == 0) {
        return 0x100;
    }
    if (pCore == 0) {
        return 0x100;
    }

    float rate = 0.1f;
    float base = 0.5f;
    GetYParam(&base, &rate);

    long mask;
    float height;
    if (m_Unk228 == 0) {
        mask = 0x100;
        rLook = GetBallPosXZ(pBall);
        rLook.y = 0.0f;
        height = pBall->GetPos().y;
    } else {
        mask = 0x10;
        rLook = GetCorePosXZ(pCore);
        rLook.y = 0.0f;
        height = pCore->GetTransNodeHeight();
    }
    rArg3.y = rate * (height - base);
    return mask;
}

long msw::CCameraHockeyGame::GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    float ballY;
    CBall *pBall = GetActiveBall();

    float rate = 0.1f;
    float base = 0.5f;
    GetYParam(&base, &rate);
    float height = base;

    bcs::FVEC3 pos;
    // Not used further.
    bcs::FVEC3 ballPos;
    ballPos = GetBallPosXZ(pBall);
    ballY = pBall->GetPos().y;

    GetHoldBallChrPos(rLook, -1);
    rLook.y = 0.0f;

    long mask;
    CChrCore *pHold = GetHoldBallChrCore(-1);
    if (pHold != 0 && !pHold->IsHockeyFighting()) {
        mask = pHold->GetPlayerNo();
        height = Max(height, pHold->GetTransNodeHeight());
    } else {
        mask = 0x100;
        if (m_Unk228 == 0) {
            height = Max(height, ballY);
        } else if (pBall != 0 && pBall->IsShootMode()) {
            // (nothing is done here)
        }
    }

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pCore = CChrMan::GetChrCore(i);
        if (pCore != 0 && !pCore->IsDisable() && !IsComCore(pCore) && pCore->IsHoldBall()) {
            height = Max(height, pCore->GetTransNodeHeight());
        }
    }
    rArg3.y = rate * (height - base);

    float minX = rLook.x;
    float maxZ = rLook.z;
    float maxX = minX;
    if (m_Unk228 == 1 || m_Unk228 == 2) {
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pCore = CChrMan::GetChrCore(i);
            if (pCore != 0 && !pCore->IsDisable() && !IsComCore(pCore) && !pCore->IsHoldBall()) {
                mask |= 1 << (i + 16);
                pos = GetCorePosXZ(pCore);
                if (pos.x > maxX) {
                    maxX = pos.x;
                }
                if (pos.x < minX) {
                    minX = pos.x;
                }
                if (pos.z > maxZ) {
                    maxZ = pos.z;
                }
            }
        }
    }

    // Shift the look point by the parts of the spread beyond the margins,
    // scaled and limited. Only the far side counts in depth.
    float right = (maxX - rLook.x) - m_fMarginX;
    float left = m_fMarginX + (minX - rLook.x);
    if (right < 0.0f) {
        right = 0.0f;
    }
    if (left > 0.0f) {
        left = 0.0f;
    }
    float shift = (right + left) * m_fRateX;
    rLook.x += shift > m_fLimitX ? m_fLimitX : (shift < -m_fLimitX ? -m_fLimitX : shift);

    float far = (maxZ - rLook.z) - m_fMarginZ;
    float near = 0.0f;
    if (far < 0.0f) {
        far = 0.0f;
    }
    if (near > 0.0f) {
        near = 0.0f;
    }
    shift = (far + near) * m_fRateZ;
    rLook.z += shift > m_fLimitZ ? m_fLimitZ : (shift < -m_fLimitZ ? -m_fLimitZ : shift);
    return mask;
}

const bcs::FVEC3 &msw::CCameraHockeyGame::CalcLookInfo(const bcs::FVEC3 &rLook, const bcs::FVEC3 &rArg2,
                                                       const bcs::FVEC3 &rArg3, float rate, long mask)
{
    bcs::FVEC3 look = rLook;

    m_Unk250 = 0.0f;
    if (IsOffenseMode(m_Unk228)) {
        m_Unk250 = m_fLookOffZOff;
    } else if (IsDefenseMode(m_Unk228)) {
        m_Unk250 = m_fLookOffZDef;
    }
    if (m_Unk228 == 15 || m_Unk228 == 16) {
        m_Unk250 = m_fLookOffZDef;
    }
    look.z += m_Unk250;

    if (m_Unk228 == 15 || m_Unk228 == 16) {
        look.x = look.x > 8.4f ? 8.4f : (look.x < -8.4f ? -8.4f : look.x);
        look.z = look.z > m_fLookLimitZSp ? m_fLookLimitZSp
                                          : (look.z < -m_fLookLimitZSp ? -m_fLookLimitZSp : look.z);
    } else {
        look.x = look.x > 8.4f ? 8.4f : (look.x < -8.4f ? -8.4f : look.x);
        look.z = look.z > m_fLookLimitZ ? m_fLookLimitZ : (look.z < -m_fLookLimitZ ? -m_fLookLimitZ : look.z);
    }

    CalcLookPos(look, rArg2, rArg3, rate, mask);
    return m_Unk118;
}

void msw::CCameraHockeyGame::Reset()
{
    CGameCamera::Reset();
    if (CGame::GetTargetGoalIdx(GetTeamNo()) != 0) {
        m_UnkFC |= 0x2;
    } else {
        m_UnkFC &= ~0x2;
    }
    SetCamParam();
}

void msw::CCameraHockeyGame::GetPosYTarYLen(float *pPosY, float *pTarY, float *pLen, float x, float z)
{
    float near = m_Unk100;
    float far = m_Unk104;
    float curveX[6];
    float curveY[6];
    float curveZ[6];

    switch (m_Unk228) {
        default:
            curveX[0] = m_ModeParam[0].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[0].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[0].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[0].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[0].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[0].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[0].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[0].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[0].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[0].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[0].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[0].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[0].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[0].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[0].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[0].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[0].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[0].m_Curve[0].m_Point[5].z;
            break;
        case 2:
            curveX[0] = m_ModeParam[0].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[0].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[0].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[0].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[0].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[0].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[0].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[0].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[0].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[0].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[0].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[0].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[0].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[0].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[0].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[0].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[0].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[0].m_Curve[1].m_Point[5].z;
            break;
        case 3:
            curveX[0] = m_ModeParam[1].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[1].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[1].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[1].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[1].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[1].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[1].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[1].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[1].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[1].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[1].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[1].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[1].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[1].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[1].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[1].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[1].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[1].m_Curve[0].m_Point[5].z;
            break;
        case 4:
            curveX[0] = m_ModeParam[1].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[1].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[1].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[1].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[1].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[1].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[1].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[1].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[1].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[1].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[1].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[1].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[1].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[1].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[1].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[1].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[1].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[1].m_Curve[1].m_Point[5].z;
            break;
        case 5:
            curveX[0] = m_ModeParam[2].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[2].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[2].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[2].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[2].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[2].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[2].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[2].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[2].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[2].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[2].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[2].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[2].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[2].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[2].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[2].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[2].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[2].m_Curve[0].m_Point[5].z;
            break;
        case 6:
            curveX[0] = m_ModeParam[2].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[2].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[2].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[2].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[2].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[2].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[2].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[2].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[2].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[2].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[2].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[2].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[2].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[2].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[2].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[2].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[2].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[2].m_Curve[1].m_Point[5].z;
            break;
        case 7:
            curveX[0] = m_ModeParam[3].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[3].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[3].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[3].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[3].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[3].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[3].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[3].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[3].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[3].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[3].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[3].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[3].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[3].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[3].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[3].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[3].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[3].m_Curve[0].m_Point[5].z;
            break;
        case 8:
            curveX[0] = m_ModeParam[3].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[3].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[3].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[3].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[3].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[3].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[3].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[3].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[3].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[3].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[3].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[3].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[3].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[3].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[3].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[3].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[3].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[3].m_Curve[1].m_Point[5].z;
            break;
        case 9:
            curveX[0] = m_ModeParam[4].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[4].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[4].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[4].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[4].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[4].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[4].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[4].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[4].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[4].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[4].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[4].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[4].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[4].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[4].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[4].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[4].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[4].m_Curve[0].m_Point[5].z;
            break;
        case 10:
            curveX[0] = m_ModeParam[4].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[4].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[4].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[4].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[4].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[4].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[4].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[4].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[4].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[4].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[4].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[4].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[4].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[4].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[4].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[4].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[4].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[4].m_Curve[1].m_Point[5].z;
            break;
        case 11:
            curveX[0] = m_ModeParam[5].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[5].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[5].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[5].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[5].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[5].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[5].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[5].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[5].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[5].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[5].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[5].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[5].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[5].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[5].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[5].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[5].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[5].m_Curve[0].m_Point[5].z;
            break;
        case 12:
            curveX[0] = m_ModeParam[5].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[5].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[5].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[5].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[5].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[5].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[5].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[5].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[5].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[5].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[5].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[5].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[5].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[5].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[5].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[5].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[5].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[5].m_Curve[1].m_Point[5].z;
            break;
        case 13:
            curveX[0] = m_ModeParam[6].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[6].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[6].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[6].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[6].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[6].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[6].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[6].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[6].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[6].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[6].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[6].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[6].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[6].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[6].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[6].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[6].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[6].m_Curve[0].m_Point[5].z;
            break;
        case 14:
            curveX[0] = m_ModeParam[6].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[6].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[6].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[6].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[6].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[6].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[6].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[6].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[6].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[6].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[6].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[6].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[6].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[6].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[6].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[6].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[6].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[6].m_Curve[1].m_Point[5].z;
            break;
        case 15:
            curveX[0] = m_ModeParam[7].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[7].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[7].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[7].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[7].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[7].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[7].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[7].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[7].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[7].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[7].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[7].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[7].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[7].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[7].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[7].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[7].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[7].m_Curve[0].m_Point[5].z;
            break;
        case 16:
            curveX[0] = m_ModeParam[7].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[7].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[7].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[7].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[7].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[7].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[7].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[7].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[7].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[7].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[7].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[7].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[7].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[7].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[7].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[7].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[7].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[7].m_Curve[1].m_Point[5].z;
            break;
        case 17:
            curveX[0] = m_ModeParam[8].m_Curve[0].m_Point[0].x;
            curveX[1] = m_ModeParam[8].m_Curve[0].m_Point[1].x;
            curveX[2] = m_ModeParam[8].m_Curve[0].m_Point[2].x;
            curveX[3] = m_ModeParam[8].m_Curve[0].m_Point[3].x;
            curveX[4] = m_ModeParam[8].m_Curve[0].m_Point[4].x;
            curveX[5] = m_ModeParam[8].m_Curve[0].m_Point[5].x;
            curveY[0] = m_ModeParam[8].m_Curve[0].m_Point[0].y;
            curveY[1] = m_ModeParam[8].m_Curve[0].m_Point[1].y;
            curveY[2] = m_ModeParam[8].m_Curve[0].m_Point[2].y;
            curveY[3] = m_ModeParam[8].m_Curve[0].m_Point[3].y;
            curveY[4] = m_ModeParam[8].m_Curve[0].m_Point[4].y;
            curveY[5] = m_ModeParam[8].m_Curve[0].m_Point[5].y;
            curveZ[0] = m_ModeParam[8].m_Curve[0].m_Point[0].z;
            curveZ[1] = m_ModeParam[8].m_Curve[0].m_Point[1].z;
            curveZ[2] = m_ModeParam[8].m_Curve[0].m_Point[2].z;
            curveZ[3] = m_ModeParam[8].m_Curve[0].m_Point[3].z;
            curveZ[4] = m_ModeParam[8].m_Curve[0].m_Point[4].z;
            curveZ[5] = m_ModeParam[8].m_Curve[0].m_Point[5].z;
            break;
        case 18:
            curveX[0] = m_ModeParam[8].m_Curve[1].m_Point[0].x;
            curveX[1] = m_ModeParam[8].m_Curve[1].m_Point[1].x;
            curveX[2] = m_ModeParam[8].m_Curve[1].m_Point[2].x;
            curveX[3] = m_ModeParam[8].m_Curve[1].m_Point[3].x;
            curveX[4] = m_ModeParam[8].m_Curve[1].m_Point[4].x;
            curveX[5] = m_ModeParam[8].m_Curve[1].m_Point[5].x;
            curveY[0] = m_ModeParam[8].m_Curve[1].m_Point[0].y;
            curveY[1] = m_ModeParam[8].m_Curve[1].m_Point[1].y;
            curveY[2] = m_ModeParam[8].m_Curve[1].m_Point[2].y;
            curveY[3] = m_ModeParam[8].m_Curve[1].m_Point[3].y;
            curveY[4] = m_ModeParam[8].m_Curve[1].m_Point[4].y;
            curveY[5] = m_ModeParam[8].m_Curve[1].m_Point[5].y;
            curveZ[0] = m_ModeParam[8].m_Curve[1].m_Point[0].z;
            curveZ[1] = m_ModeParam[8].m_Curve[1].m_Point[1].z;
            curveZ[2] = m_ModeParam[8].m_Curve[1].m_Point[2].z;
            curveZ[3] = m_ModeParam[8].m_Curve[1].m_Point[3].z;
            curveZ[4] = m_ModeParam[8].m_Curve[1].m_Point[4].z;
            curveZ[5] = m_ModeParam[8].m_Curve[1].m_Point[5].z;
            break;
    }

    *pPosY = CalcCurveBlend(x, z, near, far, curveX[0], curveX[1], curveX[2], curveX[3], curveX[4], curveX[5]);
    *pTarY = CalcCurveBlend(x, z, near, far, curveY[0], curveY[1], curveY[2], curveY[3], curveY[4], curveY[5]);
    *pLen = CalcCurveBlend(x, z, near, far, curveZ[0], curveZ[1], curveZ[2], curveZ[3], curveZ[4], curveZ[5]);
}

void msw::CCameraHockeyGame::GetOffDefParam(float *pScaleX, float *pSlant)
{
    switch (m_Unk228) {
        default:
            *pScaleX = m_ModeParam[0].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[0].m_Curve[0].m_fSlant;
            break;
        case 2:
            *pScaleX = m_ModeParam[0].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[0].m_Curve[1].m_fSlant;
            break;
        case 3:
            *pScaleX = m_ModeParam[1].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[1].m_Curve[0].m_fSlant;
            break;
        case 4:
            *pScaleX = m_ModeParam[1].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[1].m_Curve[1].m_fSlant;
            break;
        case 5:
            *pScaleX = m_ModeParam[2].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[2].m_Curve[0].m_fSlant;
            break;
        case 6:
            *pScaleX = m_ModeParam[2].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[2].m_Curve[1].m_fSlant;
            break;
        case 7:
            *pScaleX = m_ModeParam[3].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[3].m_Curve[0].m_fSlant;
            break;
        case 8:
            *pScaleX = m_ModeParam[3].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[3].m_Curve[1].m_fSlant;
            break;
        case 9:
            *pScaleX = m_ModeParam[4].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[4].m_Curve[0].m_fSlant;
            break;
        case 10:
            *pScaleX = m_ModeParam[4].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[4].m_Curve[1].m_fSlant;
            break;
        case 11:
            *pScaleX = m_ModeParam[5].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[5].m_Curve[0].m_fSlant;
            break;
        case 12:
            *pScaleX = m_ModeParam[5].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[5].m_Curve[1].m_fSlant;
            break;
        case 13:
            *pScaleX = m_ModeParam[6].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[6].m_Curve[0].m_fSlant;
            break;
        case 14:
            *pScaleX = m_ModeParam[6].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[6].m_Curve[1].m_fSlant;
            break;
        case 15:
            *pScaleX = m_ModeParam[7].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[7].m_Curve[0].m_fSlant;
            break;
        case 16:
            *pScaleX = m_ModeParam[7].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[7].m_Curve[1].m_fSlant;
            break;
        case 17:
            *pScaleX = m_ModeParam[8].m_Curve[0].m_fLookScaleX;
            *pSlant = m_ModeParam[8].m_Curve[0].m_fSlant;
            break;
        case 18:
            *pScaleX = m_ModeParam[8].m_Curve[1].m_fLookScaleX;
            *pSlant = m_ModeParam[8].m_Curve[1].m_fSlant;
            break;
    }
}

void msw::CCameraHockeyGame::GetYParam(float *pBase, float *pRate)
{
    switch (m_Unk228) {
        case 1:
            *pBase = m_ModeParam[0].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[0].m_Curve[0].m_fHeightRate;
            break;
        case 2:
            *pBase = m_ModeParam[0].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[0].m_Curve[1].m_fHeightRate;
            break;
        case 3:
            *pBase = m_ModeParam[1].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[1].m_Curve[0].m_fHeightRate;
            break;
        case 4:
            *pBase = m_ModeParam[1].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[1].m_Curve[1].m_fHeightRate;
            break;
        case 5:
            *pBase = m_ModeParam[2].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[2].m_Curve[0].m_fHeightRate;
            break;
        case 6:
            *pBase = m_ModeParam[2].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[2].m_Curve[1].m_fHeightRate;
            break;
        case 7:
            *pBase = m_ModeParam[3].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[3].m_Curve[0].m_fHeightRate;
            break;
        case 8:
            *pBase = m_ModeParam[3].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[3].m_Curve[1].m_fHeightRate;
            break;
        case 9:
            *pBase = m_ModeParam[4].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[4].m_Curve[0].m_fHeightRate;
            break;
        case 10:
            *pBase = m_ModeParam[4].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[4].m_Curve[1].m_fHeightRate;
            break;
        case 11:
            *pBase = m_ModeParam[5].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[5].m_Curve[0].m_fHeightRate;
            break;
        case 12:
            *pBase = m_ModeParam[5].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[5].m_Curve[1].m_fHeightRate;
            break;
        case 13:
            *pBase = m_ModeParam[6].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[6].m_Curve[0].m_fHeightRate;
            break;
        case 14:
            *pBase = m_ModeParam[6].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[6].m_Curve[1].m_fHeightRate;
            break;
        case 15:
            *pBase = m_ModeParam[7].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[7].m_Curve[0].m_fHeightRate;
            break;
        case 16:
            *pBase = m_ModeParam[7].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[7].m_Curve[1].m_fHeightRate;
            break;
        case 17:
            *pBase = m_ModeParam[8].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[8].m_Curve[0].m_fHeightRate;
            break;
        case 18:
            *pBase = m_ModeParam[8].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[8].m_Curve[1].m_fHeightRate;
            break;
        default:
            *pBase = 0.5f;
            *pRate = 0.0f;
            break;
    }
}

float msw::CCameraHockeyGame::GetCamModeLerpRate() const
{
    switch (m_Unk228) {
        case 1:
            return m_ModeParam[0].m_fLerpRate;
        case 2:
            return m_ModeParam[0].m_fLerpRate;
        case 3:
            return m_ModeParam[1].m_fLerpRate;
        case 4:
            return m_ModeParam[1].m_fLerpRate;
        case 5:
            return m_ModeParam[2].m_fLerpRate;
        case 6:
            return m_ModeParam[2].m_fLerpRate;
        case 7:
            return m_ModeParam[3].m_fLerpRate;
        case 8:
            return m_ModeParam[3].m_fLerpRate;
        case 9:
            return m_ModeParam[4].m_fLerpRate;
        case 10:
            return m_ModeParam[4].m_fLerpRate;
        case 11:
            return m_ModeParam[5].m_fLerpRate;
        case 12:
            return m_ModeParam[5].m_fLerpRate;
        case 13:
            return m_ModeParam[6].m_fLerpRate;
        case 14:
            return m_ModeParam[6].m_fLerpRate;
        case 15:
            return m_ModeParam[7].m_fLerpRate;
        case 16:
            return m_ModeParam[7].m_fLerpRate;
        case 17:
            return m_ModeParam[8].m_fLerpRate;
        case 18:
            return m_ModeParam[8].m_fLerpRate;
        default:
            return 0.96435547f;
    }
}

// The numbers of every camera mode, for the four combinations of the
// view's aspect type (bit 0x8 of m_UnkFC) and bit 0x20 of m_UnkFC.
void msw::CCameraHockeyGame::SetCamParam()
{
    if (m_UnkFC & 0x20) {
        if (m_UnkFC & 0x8) {
            m_fFovy = 40.0f;
            m_fNear = 11.5f;
            m_fFar = 25.0f;
            m_fBack = 10.0f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 1.5f;
            m_fLookOffZDef = 5.0f;
            m_fLookLimitZ = 18.499998f;
            m_fLookLimitZSp = 25.0f;
            m_fMarginX = 2.0f;
            m_fLimitX = 20.0f;
            m_fRateX = 1.0f;
            m_fMarginZ = 2.0f;
            m_fLimitZ = 6.0f;
            m_fRateZ = 0.6f;
            m_fNormalPosY = 3.400333f;
            m_fNormalTarY = 1.57f;
            m_fNormalLen = 9.499782f;
            m_fNormalOffX = 0.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[0].x = 4.199999f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].y = 3.100001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].z = 10.799997f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[1].x = 4.199999f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].y = 3.100001f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].z = 10.799997f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[0].m_Point[2].x = 4.3f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[0].m_Point[2].y = 2.6f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 8.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[0].m_Point[2].z = 6.8f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[0].m_Point[3].x = 4.299999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[0].m_Point[3].y = 2.6f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 8.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[0].m_Point[3].z = 6.8f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 1.400001f;
            m_ModeParam[8].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 3.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[4].z = 11.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 1.900001f;
            m_ModeParam[8].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 2.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[5].z = 15.999996f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fSlant = 20.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[0].m_fLookScaleX = 0.93f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].z = 24.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[1].m_Point[2].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[1].m_Point[3].z = 24.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 1.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[4].z = 4.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 3.0f;
            m_ModeParam[8].m_Curve[1].m_Point[5].z = 3.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fSlant = 50.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.92035f;
            m_ModeParam[8].m_fLerpRate = 0.96435f;
        } else {
            m_fFovy = 40.0f;
            m_fNear = 11.5f;
            m_fFar = 25.0f;
            m_fBack = 10.0f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 1.5f;
            m_fLookOffZDef = 5.0f;
            m_fLookLimitZ = 18.500002f;
            m_fLookLimitZSp = 25.0f;
            m_fMarginX = 2.0f;
            m_fLimitX = 20.0f;
            m_fRateX = 1.3f;
            m_fMarginZ = 2.0f;
            m_fLimitZ = 6.0f;
            m_fRateZ = 0.6f;
            m_fNormalPosY = 3.800055f;
            m_fNormalTarY = 2.3f;
            m_fNormalLen = 11.500054f;
            m_fNormalOffX = 0.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].z = 14.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].z = 13.0f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 8.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 8.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[0].m_Point[3].z = 9.999998f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 1.400001f;
            m_ModeParam[8].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 3.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[4].z = 12.999998f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 1.900001f;
            m_ModeParam[8].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 2.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[5].z = 13.999998f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fSlant = 20.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[0].m_fLookScaleX = 0.83f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[1].m_Point[2].z = 16.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[1].m_Point[3].z = 16.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 1.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[4].z = 10.0f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 3.0f;
            m_ModeParam[8].m_Curve[1].m_Point[5].z = 10.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.92035f;
            m_ModeParam[8].m_fLerpRate = 0.96435f;
        }
    } else {
        if (m_UnkFC & 0x8) {
            m_fFovy = 40.0f;
            m_fNear = 11.5f;
            m_fFar = 25.0f;
            m_fBack = 10.0f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 1.5f;
            m_fLookOffZDef = 5.0f;
            m_fLookLimitZ = 18.500006f;
            m_fLookLimitZSp = 25.0f;
            m_fMarginX = 3.5f;
            m_fLimitX = 1.5f;
            m_fRateX = 0.5f;
            m_fMarginZ = 4.0f;
            m_fLimitZ = 4.0f;
            m_fRateZ = 0.5f;
            m_fNormalPosY = 3.400335f;
            m_fNormalTarY = 1.57f;
            m_fNormalLen = 9.50005f;
            m_fNormalOffX = 0.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[0].x = 4.199999f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].y = 3.100001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].z = 12.999998f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[1].x = 4.199999f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].y = 3.100001f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].z = 13.999998f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[0].m_Point[2].x = 4.3f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[0].m_Point[2].y = 2.6f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 8.500004f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[0].m_Point[3].x = 4.299999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[0].m_Point[3].y = 2.6f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 8.500004f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 1.400001f;
            m_ModeParam[8].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 3.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[4].z = 11.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 1.900001f;
            m_ModeParam[8].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 2.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[5].z = 15.999996f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fSlant = 20.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[0].m_fLookScaleX = 0.85f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].z = 24.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[1].m_Point[2].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[1].m_Point[3].z = 24.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 1.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[4].z = 4.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 3.0f;
            m_ModeParam[8].m_Curve[1].m_Point[5].z = 3.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fSlant = 35.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[1].m_fLookScaleX = 0.43f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.92035f;
            m_ModeParam[8].m_fLerpRate = 0.96435f;
        } else {
            m_fFovy = 40.0f;
            m_fNear = 11.5f;
            m_fFar = 25.0f;
            m_fBack = 10.0f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 1.5f;
            m_fLookOffZDef = 5.0f;
            m_fLookLimitZ = 18.500002f;
            m_fLookLimitZSp = 25.0f;
            m_fMarginX = 3.5f;
            m_fLimitX = 1.5f;
            m_fRateX = 0.5f;
            m_fMarginZ = 4.0f;
            m_fLimitZ = 4.0f;
            m_fRateZ = 0.5f;
            m_fNormalPosY = 3.800333f;
            m_fNormalTarY = 2.3f;
            m_fNormalLen = 11.500055f;
            m_fNormalOffX = 0.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[0].x = 5.6f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[0].z = 13.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[0].m_Point[1].x = 5.6f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].y = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 9.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 18.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[0].m_Point[1].z = 13.0f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[0].m_Point[2].x = 5.5f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 8.5f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[0].m_Point[2].z = 10.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 5.6f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[0].m_Point[3].x = 5.5f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 8.5f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 7.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 13.5f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[0].m_Point[3].z = 9.999998f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[0].m_Point[4].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 1.400001f;
            m_ModeParam[8].m_Curve[0].m_Point[4].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 3.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[4].z = 12.999998f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 5.5f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[0].m_Point[5].x = 5.4f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 1.900001f;
            m_ModeParam[8].m_Curve[0].m_Point[5].y = 3.2f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 12.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 14.5f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 2.999998f;
            m_ModeParam[8].m_Curve[0].m_Point[5].z = 13.999998f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fSlant = 20.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.73f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[0].m_fLookScaleX = 0.83f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[0].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[0].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[0].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 4.6f;
            m_ModeParam[8].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 2.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 16.0f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[1].z = 20.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.299999f;
            m_ModeParam[8].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[2].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 1.5f;
            m_ModeParam[8].m_Curve[1].m_Point[2].z = 16.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 5.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 5.6f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 4.699999f;
            m_ModeParam[8].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 3.1f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.3f;
            m_ModeParam[8].m_Curve[1].m_Point[3].y = 2.6f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 11.5f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 7.0f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 4.5f;
            m_ModeParam[8].m_Curve[1].m_Point[3].z = 16.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 3.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 1.4f;
            m_ModeParam[8].m_Curve[1].m_Point[4].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 4.0f;
            m_ModeParam[8].m_Curve[1].m_Point[4].z = 10.0f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 5.4f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 2.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].x = 5.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 1.9f;
            m_ModeParam[8].m_Curve[1].m_Point[5].y = 2.7f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 12.5f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 14.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 3.0f;
            m_ModeParam[8].m_Curve[1].m_Point[5].z = 10.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.73f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[8].m_Curve[1].m_fLookScaleX = 0.63f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[8].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[8].m_Curve[1].m_fHeightRate = 0.0f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.92035f;
            m_ModeParam[8].m_fLerpRate = 0.96435f;
        }
    }
}

msw::CCameraHockeyGame::~CCameraHockeyGame() {}
