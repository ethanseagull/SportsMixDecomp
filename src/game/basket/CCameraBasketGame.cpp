// msw::CCameraBasketGame: the camera of a basketball match. It picks a
// camera mode from the game state (attacking or defending, a long
// one-on-one, the special views the game asks for), finds the point to look
// at, and places the camera on a curve read from the mode's parameters,
// which SetCamParam fills. The shape follows HockeyF's CCameraHockeyGame.
#include "game/msw/CCameraBasketGame.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"

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

msw::CCameraBasketGame::CCameraBasketGame(long view) : CCameraBasket(view)
{
    bcs::CWorldEnv::SetViewFovy(view, 40.0f);
    m_Unk250 = 0.0f;
    m_f1on1Time = 0.0f;
    m_fFreeBallTime = 0.0f;
    SetCamParam();
}

bool msw::CCameraBasketGame::IsNormalCamMode() const
{
    if ((m_Unk228 == 1 || m_Unk228 == 2) && m_Unk224 < 0.1f) {
        return true;
    }
    return false;
}

// The look point moved by the mode's z offset and kept within the court
// (in place), then handed to CalcLookPos; returns the look position it made.
inline const bcs::FVEC3 &msw::CCameraBasketGame::CalcLookInfo(bcs::FVEC3 &rLook, const bcs::FVEC3 &rArg2,
                                                              const bcs::FVEC3 &rArg3, float rate, long mask)
{
    m_Unk250 = 0.0f;
    if (IsOffenseMode(m_Unk228)) {
        m_Unk250 = m_fLookOffZOff;
    } else if (IsDefenseMode(m_Unk228)) {
        m_Unk250 = m_fLookOffZDef;
    }
    rLook.z += m_Unk250;

    rLook.x = rLook.x > 7.5f ? 7.5f : (rLook.x < -7.5f ? -7.5f : rLook.x);
    rLook.z = rLook.z > 11.5f ? 11.5f : (rLook.z < -11.5f ? -11.5f : rLook.z);

    CalcLookPos(rLook, rArg2, rArg3, rate, mask);
    return m_Unk118;
}

void msw::CCameraBasketGame::MainCamera()
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

    // Count how long the one-on-one view lasts, and how long since it
    // ended (much longer at once while nobody holds the ball).
    if (CChrMan::Is1on1CameraDirection()) {
        m_f1on1Time += GetFrameRateF();
        m_fFreeBallTime = 0.0f;
    } else {
        m_f1on1Time = 0.0f;
        m_fFreeBallTime += GetFrameRateF();
        if (GetHoldBallChrCore(-1) == 0) {
            m_fFreeBallTime += 1000.0f;
        }
    }

    long mode;
    if (CGame::g_GameWork.m_nFlag18 & 0x8) {
        mode = 0;
    } else {
        mode = -1;
        if (m_Unk228 == 3 || m_Unk228 == 4) {
            if (m_fFreeBallTime < m_fFreeBallWait) {
                mode = m_Unk228;
            }
        } else if ((m_Unk228 == 1 || m_Unk228 == 2) && m_f1on1Time >= m_f1on1Wait) {
            mode = IsOffense() ? 3 : 4;
        }

        long type = CGame::GetExceptionalCameraType();
        switch (type) {
            case 4:
                if (IsOffense()) {
                    mode = 9;
                } else {
                    mode = 10;
                }
                break;
            case 5:
                if (IsOffense()) {
                    mode = 11;
                } else {
                    mode = 12;
                }
                break;
            case 3:
                mode = IsOffense() ? 13 : 14;
                break;
        }
        if (type != 3 && IsSpModeForBaHo()) {
            mode = IsOffense() ? 15 : 16;
        }
        if (mode < 0) {
            mode = IsOffense() ? 1 : 2;
        }
    }
    SetCamMode(mode, 0);

    if (IsOffenseMode(m_Unk228)) {
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
        bcs::FVEC3 at = look;
        bcs::FVEC3 pos = CalcLookInfo(at, arg2, arg3, lookRate, mask);
        float len;
        float tarY;
        float posY;
        GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
        pos.x *= scaleX;
        bcs::FVEC3 target;
        MakeingOffSlantCamTar(target, pos, GetUnk108Vec(), slant);
        MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
    } else if (IsDefenseMode(m_Unk228)) {
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
        bcs::FVEC3 at = look;
        bcs::FVEC3 pos = CalcLookInfo(at, arg2, arg3, lookRate, mask);
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
        bcs::FVEC3 at = look;
        bcs::FVEC3 pos = CalcLookInfo(at, arg2, arg3, lookRate, mask);
        bcs::FVEC3 target(pos.x - m_fNormalOffX, 0.0f, m_Unk110);
        MakeingFrontCamData(pos, target, m_fNormalPosY, m_fNormalTarY, m_fNormalLen, frontRate, 0.0f, m_Unk1C0);
    }

    CalcCamData(GetCamModeLerpRate());
}

// One pad character: look at the ball in mode 0, else at the character,
// lifted by how far the ball or the character is above the mode's base
// height.
long msw::CCameraBasketGame::GetFirstLookInfo_SinglePlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
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
    float rateChr = 0.1f;
    float base = 0.5f;
    GetYParam(&base, &rate, &rateChr);
    float height = base;

    long mask;
    if (m_Unk228 == 0) {
        mask = 0x100;
        rLook = GetBallPosXZ(pBall);
        rLook.y = 0.0f;
        height = Max(height, pBall->GetPos().y);
    } else {
        mask = 0x10;
        rLook = GetCorePosXZ(pCore);
        rLook.y = 0.0f;
        height = Max(height, pCore->GetTransNodeHeight());
    }
    if (m_Unk228 != 0) {
        rArg3.y = rateChr * (height - base);
    } else {
        rArg3.y = rate * (height - base);
    }
    return mask;
}

// Several pad characters: look at the ball holder (or, in mode 16 with
// nobody holding the ball, the last character in an air trick), shifted
// towards the other pad characters when they spread beyond the margins.
// rArg3.y lifts the view with the highest holder or the ball in the air.
// Returns the holder's player number (0x100 for none) with a bit per
// character looked at (bit 16 on).
long msw::CCameraBasketGame::GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    float ballY;
    CBall *pBall = GetActiveBall();

    float rate = 0.1f;
    float rateChr = 0.1f;
    float base = 0.5f;
    GetYParam(&base, &rate, &rateChr);
    float height = base;

    bcs::FVEC3 pos;
    // Not used further.
    bcs::FVEC3 ballPos;
    ballPos = GetBallPosXZ(pBall);
    ballY = pBall->GetPos().y;

    GetHoldBallChrPos(rLook, -1);
    rLook.y = 0.0f;

    CChrCore *pHold = GetHoldBallChrCore(-1);
    if (pHold == 0 && GetCamMode() == 16) {
        CChrCore *pAir = GetLastAirTrickCoreAll();
        if (pAir != 0) {
            rLook = GetCorePosXZ(pAir);
            rLook.y = 0.0f;
        }
    }

    long mask;
    if (pHold != 0) {
        mask = pHold->GetPlayerNo();
        height = Max(height, pHold->GetTransNodeHeight());
    } else {
        mask = 0x100;
        if (m_Unk228 == 0) {
            height = Max(height, ballY);
        } else if (pBall != 0 && (pBall->IsShootMode() || pBall->IsNowRebound())) {
            height = Max(height, ballY);
        }
    }

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pCore = CChrMan::GetChrCore(i);
        if (pCore != 0 && !pCore->IsDisable() && !IsComCore(pCore)) {
            long state = pCore->GetState();
            if (pCore->IsHoldBall() || state == 0x26) {
                height = Max(height, pCore->GetTransNodeHeight());
            }
        }
    }
    if (pHold != 0 && pHold->IsHoldBall()) {
        rArg3.y = rateChr * (height - base);
    } else {
        rArg3.y = rate * (height - base);
    }

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

void msw::CCameraBasketGame::Reset()
{
    CGameCamera::Reset();
    if (CGame::GetTargetGoalIdx(GetTeamNo()) != 0) {
        m_UnkFC |= 0x2;
    } else {
        m_UnkFC &= ~0x2;
    }
    m_f1on1Time = 0.0f;
    m_fFreeBallTime = 0.0f;
    SetCamParam();
}

void msw::CCameraBasketGame::GetPosYTarYLen(float *pPosY, float *pTarY, float *pLen, float x, float z)
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
    }

    *pPosY = CalcCurveBlend(x, z, near, far, curveX[0], curveX[1], curveX[2], curveX[3], curveX[4], curveX[5]);
    *pTarY = CalcCurveBlend(x, z, near, far, curveY[0], curveY[1], curveY[2], curveY[3], curveY[4], curveY[5]);
    *pLen = CalcCurveBlend(x, z, near, far, curveZ[0], curveZ[1], curveZ[2], curveZ[3], curveZ[4], curveZ[5]);
}

void msw::CCameraBasketGame::GetOffDefParam(float *pScaleX, float *pSlant)
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
    }
}

void msw::CCameraBasketGame::GetYParam(float *pBase, float *pRate, float *pRateChr)
{
    switch (m_Unk228) {
        case 1:
            *pBase = m_ModeParam[0].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[0].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[0].m_Curve[0].m_fHeightRateChr;
            break;
        case 2:
            *pBase = m_ModeParam[0].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[0].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[0].m_Curve[1].m_fHeightRateChr;
            break;
        case 3:
            *pBase = m_ModeParam[1].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[1].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[1].m_Curve[0].m_fHeightRateChr;
            break;
        case 4:
            *pBase = m_ModeParam[1].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[1].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[1].m_Curve[1].m_fHeightRateChr;
            break;
        case 5:
            *pBase = m_ModeParam[2].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[2].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[2].m_Curve[0].m_fHeightRateChr;
            break;
        case 6:
            *pBase = m_ModeParam[2].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[2].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[2].m_Curve[1].m_fHeightRateChr;
            break;
        case 7:
            *pBase = m_ModeParam[3].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[3].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[3].m_Curve[0].m_fHeightRateChr;
            break;
        case 8:
            *pBase = m_ModeParam[3].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[3].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[3].m_Curve[1].m_fHeightRateChr;
            break;
        case 9:
            *pBase = m_ModeParam[4].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[4].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[4].m_Curve[0].m_fHeightRateChr;
            break;
        case 10:
            *pBase = m_ModeParam[4].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[4].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[4].m_Curve[1].m_fHeightRateChr;
            break;
        case 11:
            *pBase = m_ModeParam[5].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[5].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[5].m_Curve[0].m_fHeightRateChr;
            break;
        case 12:
            *pBase = m_ModeParam[5].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[5].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[5].m_Curve[1].m_fHeightRateChr;
            break;
        case 13:
            *pBase = m_ModeParam[6].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[6].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[6].m_Curve[0].m_fHeightRateChr;
            break;
        case 14:
            *pBase = m_ModeParam[6].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[6].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[6].m_Curve[1].m_fHeightRateChr;
            break;
        case 15:
            *pBase = m_ModeParam[7].m_Curve[0].m_fHeightBase;
            *pRate = m_ModeParam[7].m_Curve[0].m_fHeightRate;
            *pRateChr = m_ModeParam[7].m_Curve[0].m_fHeightRateChr;
            break;
        case 16:
            *pBase = m_ModeParam[7].m_Curve[1].m_fHeightBase;
            *pRate = m_ModeParam[7].m_Curve[1].m_fHeightRate;
            *pRateChr = m_ModeParam[7].m_Curve[1].m_fHeightRateChr;
            break;
        default:
            *pBase = 0.5f;
            *pRate = 0.15f;
            *pRateChr = 0.15f;
            break;
    }
}

float msw::CCameraBasketGame::GetCamModeLerpRate() const
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
        default:
            return 0.96435547f;
    }
}

// The numbers of every camera mode, for the four combinations of bit 0x20
// of m_UnkFC and the view's aspect type (bit 0x8).
void msw::CCameraBasketGame::SetCamParam()
{
    if (m_UnkFC & 0x20) {
        if (m_UnkFC & 0x8) {
            m_fFovy = 40.0f;
            m_fNear = 8.0f;
            m_fFar = 12.0f;
            m_fBack = 10.5f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 2.0f;
            m_fLookOffZDef = 4.0f;
            m_fMarginX = 0.5f;
            m_fLimitX = 20.0f;
            m_fRateX = 1.0f;
            m_fMarginZ = 0.5f;
            m_fLimitZ = 6.0f;
            m_fRateZ = 0.6f;
            m_fNormalPosY = 2.8f;
            m_fNormalTarY = 1.9f;
            m_fNormalLen = 8.700003f;
            m_fNormalOffX = 5.0f;
            m_f1on1Wait = 35.0f;
            m_fFreeBallWait = 50.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 3.23f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 3.23f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 4.8f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 4.800011f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 2.63f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 2.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 2.63f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.3f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 3.400001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 7.75f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 8.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 7.7f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 7.75f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 7.200002f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 9.199998f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 5.000004f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 1.52f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.3f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 1.5f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 1.52f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.3f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 3.400001f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 10.5f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 6.999999f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 9.0f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 3.51f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 3.51f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 5.000003f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 2.3f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 2.85f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 2.999999f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 2.3f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.6f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 3.699999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 5.46f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 5.46f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 4.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 5.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 3.79f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 3.7f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 3.79f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.200001f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 5.200003f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 2.46f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 2.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 2.46f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.8f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 3.900001f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 6.14f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 6.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 6.14f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 6.5f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 4.2f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 4.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 4.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 4.800002f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 4.8f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 2.55f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 2.55f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 3.5f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 3.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 4.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 3.92f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 3.899999f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 3.92f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 5.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 3.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 6.44f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 6.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 4.0f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 5.0f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 12.5f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 1.05f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 1.05f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[0].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[1].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[2].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[3].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[4].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[5].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[6].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[7].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 3.22f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 3.22f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 2.58f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 2.500002f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 2.58f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 2.999999f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 9.02f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 10.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 8.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 22.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 25.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 2.57f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 2.57f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.300001f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 9.02f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 11.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 8.999998f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 21.999998f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 25.000002f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 4.03f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 4.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 4.03f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 2.33f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 2.7f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.33f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 2.799999f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 6.65f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 5.499999f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 6.65f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 3.77f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 3.77f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 2.48f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.48f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 6.67f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 6.67f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.05f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 3.05f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.4f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 3.5f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 6.4f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 5.500001f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 7.500002f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 4.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 4.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.06f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 3.06f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.4f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 3.499999f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 6.44f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 6.3f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 6.400001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 6.000001f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 20.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.6f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 0.6f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[0].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[1].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[2].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[3].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[4].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[5].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[6].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[7].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.987f;
        } else {
            m_fFovy = 40.0f;
            m_fNear = 8.0f;
            m_fFar = 12.0f;
            m_fBack = 10.5f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 2.0f;
            m_fLookOffZDef = 4.0f;
            m_fMarginX = 0.5f;
            m_fLimitX = 20.0f;
            m_fRateX = 1.0f;
            m_fMarginZ = 0.5f;
            m_fLimitZ = 6.0f;
            m_fRateZ = 0.6f;
            m_fNormalPosY = 2.8f;
            m_fNormalTarY = 1.9f;
            m_fNormalLen = 8.700003f;
            m_fNormalOffX = 5.0f;
            m_f1on1Wait = 35.0f;
            m_fFreeBallWait = 50.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 3.23f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 3.23f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 2.63f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 2.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 2.63f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.4f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 7.75f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 8.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 7.7f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 7.75f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 6.499999f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 8.5f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 1.52f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.3f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 1.5f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 1.52f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.4f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 10.5f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 6.500001f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 8.5f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 3.51f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 3.51f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.8f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 5.8f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 2.3f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 2.85f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 2.999999f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 2.3f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 4.0f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 5.46f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 5.46f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 6.5f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 8.499998f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 3.79f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 3.7f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 3.79f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.800001f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 5.8f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 2.46f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 2.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 2.46f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 4.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 6.14f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 6.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 6.14f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 6.500002f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 8.500002f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 4.2f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 4.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 4.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 6.000002f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 2.55f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 2.55f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.7f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 3.92f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 3.899999f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 3.92f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.7f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 6.44f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 6.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 6.299999f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 12.5f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[0].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[1].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[2].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[3].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[4].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[5].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[6].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[7].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 3.22f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 3.22f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 2.58f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 2.500002f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 2.58f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.799999f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 3.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 9.02f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 10.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 8.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 22.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 25.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 2.57f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 2.57f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 4.000001f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 3.999999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 9.02f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 11.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 8.999998f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 21.999998f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 24.999998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 4.03f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 4.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 4.03f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 2.33f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 2.7f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.33f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.3f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 3.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 6.65f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 5.499999f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 6.65f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 3.77f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 3.77f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 2.48f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.48f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 3.3f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 3.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 6.67f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 6.67f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.05f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 3.05f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.900001f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 3.9f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 6.4f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 5.500001f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 7.500002f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 4.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 4.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.06f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 3.06f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 3.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 6.44f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 6.3f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 6.400001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 6.000001f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 20.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.45f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 0.45f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[0].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[1].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[2].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[3].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[4].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[5].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[6].m_Curve[1].m_fHeightRateChr = 0.09f;
            m_ModeParam[7].m_Curve[1].m_fHeightRateChr = 0.09f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.96435f;
        }
    } else {
        if (m_UnkFC & 0x8) {
            m_fFovy = 40.0f;
            m_fNear = 8.0f;
            m_fFar = 12.0f;
            m_fBack = 10.5f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 2.0f;
            m_fLookOffZDef = 4.0f;
            m_fMarginX = 0.5f;
            m_fLimitX = 3.0f;
            m_fRateX = 0.5f;
            m_fMarginZ = 0.5f;
            m_fLimitZ = 6.0f;
            m_fRateZ = 0.5f;
            m_fNormalPosY = 2.8f;
            m_fNormalTarY = 1.9f;
            m_fNormalLen = 8.699999f;
            m_fNormalOffX = 5.0f;
            m_f1on1Wait = 35.0f;
            m_fFreeBallWait = 50.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 3.23f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 5.50001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 2.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 2.63f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.4f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 3.400001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 12.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 8.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 7.7f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 7.75f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 8.000001f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 11.999997f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 5.000004f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.3f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 1.5f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 1.52f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.4f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 3.400001f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 13.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 10.5f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 7.999999f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 12.0f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 4.799999f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 3.51f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 5.000003f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 2.999999f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 2.85f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 2.999999f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 2.3f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.6f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 3.699999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 5.46f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 4.799999f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 3.7f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 3.79f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.200001f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 5.200003f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 2.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 2.46f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.8f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 3.900001f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 9.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 6.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 6.14f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 7.0f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 4.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 4.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 4.800003f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 4.8f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 2.55f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 3.5f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 7.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 3.899999f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 3.92f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 5.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 3.799999f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 3.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 7.300001f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 6.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 7.0f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 12.5f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 53.5f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 53.5f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.73f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 0.73f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[0].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[1].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[2].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[3].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[4].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[5].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[6].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[7].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 3.22f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 2.500002f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 2.58f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.199999f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 3.2f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 12.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 10.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 8.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 21.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 25.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 2.57f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.600001f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 3.6f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 14.500002f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 11.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 8.999998f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 20.999998f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 25.000002f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 4.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 4.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 4.03f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 2.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 2.7f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.33f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 5.499999f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 6.65f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 4.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 3.77f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 5.100001f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 5.1f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 2.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.48f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 9.0f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 6.67f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.1f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 5.1f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 3.05f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 3.5f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 5.500001f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 7.500004f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 3.06f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 3.499999f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 7.0f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 6.3f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 6.400001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 6.000001f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 20.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[0].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[1].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[2].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[3].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[4].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[5].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[6].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[7].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.987f;
        } else {
            m_fFovy = 40.0f;
            m_fNear = 8.0f;
            m_fFar = 12.0f;
            m_fBack = 10.5f;
            m_fFrontRate = 0.88f;
            m_fLookRate = 0.937f;
            m_fLookOffZOff = 2.0f;
            m_fLookOffZDef = 4.0f;
            m_fMarginX = 0.5f;
            m_fLimitX = 0.0f;
            m_fRateX = 0.5f;
            m_fMarginZ = 0.5f;
            m_fLimitZ = 6.0f;
            m_fRateZ = 0.5f;
            m_fNormalPosY = 2.8f;
            m_fNormalTarY = 1.9f;
            m_fNormalLen = 8.700003f;
            m_fNormalOffX = 5.0f;
            m_f1on1Wait = 35.0f;
            m_fFreeBallWait = 50.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 3.2f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 3.23f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 4.8f;
            m_ModeParam[7].m_Curve[0].m_Point[0].x = 4.800011f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 2.6f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 2.63f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.4f;
            m_ModeParam[7].m_Curve[0].m_Point[0].y = 3.400001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 16.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 8.0f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 12.5f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 7.7f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 7.75f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 7.999997f;
            m_ModeParam[7].m_Curve[0].m_Point[0].z = 11.999997f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 1.0f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[1].x = 5.000004f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.3f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 1.5f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 1.52f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 3.399999f;
            m_ModeParam[7].m_Curve[0].m_Point[1].y = 3.400001f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 17.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 10.5f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 13.5f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 6.2f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 7.999999f;
            m_ModeParam[7].m_Curve[0].m_Point[1].z = 12.0f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 4.799999f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 3.51f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 5.199999f;
            m_ModeParam[7].m_Curve[0].m_Point[2].x = 5.200003f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 2.999999f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 2.85f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 2.999999f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 2.3f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 3.7f;
            m_ModeParam[7].m_Curve[0].m_Point[2].y = 3.699999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 5.46f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 5.999998f;
            m_ModeParam[7].m_Curve[0].m_Point[2].z = 8.0f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 4.799999f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 3.7f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 3.79f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 5.200001f;
            m_ModeParam[7].m_Curve[0].m_Point[3].x = 5.200003f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.1f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 2.8f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 2.46f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 3.7f;
            m_ModeParam[7].m_Curve[0].m_Point[3].y = 3.700001f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 9.0f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 6.1f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 6.14f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 6.000002f;
            m_ModeParam[7].m_Curve[0].m_Point[3].z = 10.000002f;
            m_ModeParam[0].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[2].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[4].x = 4.5f;
            m_ModeParam[5].m_Curve[0].m_Point[4].x = 4.2f;
            m_ModeParam[6].m_Curve[0].m_Point[4].x = 4.800002f;
            m_ModeParam[7].m_Curve[0].m_Point[4].x = 4.8f;
            m_ModeParam[0].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[2].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[4].y = 3.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].y = 2.55f;
            m_ModeParam[6].m_Curve[0].m_Point[4].y = 3.5f;
            m_ModeParam[7].m_Curve[0].m_Point[4].y = 3.5f;
            m_ModeParam[0].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[2].m_Curve[0].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[0].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[0].m_Point[4].z = 4.0f;
            m_ModeParam[5].m_Curve[0].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[0].m_Point[4].z = 6.000001f;
            m_ModeParam[7].m_Curve[0].m_Point[4].z = 9.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[2].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[0].m_Point[5].x = 3.899999f;
            m_ModeParam[5].m_Curve[0].m_Point[5].x = 3.92f;
            m_ModeParam[6].m_Curve[0].m_Point[5].x = 5.0f;
            m_ModeParam[7].m_Curve[0].m_Point[5].x = 5.0f;
            m_ModeParam[0].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[0].m_Point[5].y = 3.799999f;
            m_ModeParam[2].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].y = 2.4f;
            m_ModeParam[6].m_Curve[0].m_Point[5].y = 3.8f;
            m_ModeParam[7].m_Curve[0].m_Point[5].y = 3.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[1].m_Curve[0].m_Point[5].z = 7.300001f;
            m_ModeParam[2].m_Curve[0].m_Point[5].z = 6.0f;
            m_ModeParam[3].m_Curve[0].m_Point[5].z = 8.3f;
            m_ModeParam[4].m_Curve[0].m_Point[5].z = 6.4f;
            m_ModeParam[5].m_Curve[0].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[0].m_Point[5].z = 5.5f;
            m_ModeParam[7].m_Curve[0].m_Point[5].z = 8.499998f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 38.5f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 12.5f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 12.75f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 50.0f;
            m_ModeParam[7].m_Curve[0].m_fSlant = 50.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.35f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.6f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 0.7f;
            m_ModeParam[7].m_Curve[0].m_fLookScaleX = 0.7f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[0].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[7].m_Curve[0].m_fHeightRate = 0.03f;
            m_ModeParam[0].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[1].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[2].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[3].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[4].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[5].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[6].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[7].m_Curve[0].m_fHeightRateChr = 0.16f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 3.22f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 2.500002f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 2.58f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.499998f;
            m_ModeParam[7].m_Curve[1].m_Point[0].y = 3.1f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 14.0f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 10.5f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 10.0f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 8.0f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 21.0f;
            m_ModeParam[7].m_Curve[1].m_Point[0].z = 25.0f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.2f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 3.2f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[1].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 2.57f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 3.600001f;
            m_ModeParam[7].m_Curve[1].m_Point[1].y = 3.3f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 16.500002f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 11.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 12.5f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 8.999998f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 9.02f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 21.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[1].z = 24.999998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 4.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 4.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 4.03f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[7].m_Curve[1].m_Point[2].x = 4.8f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 2.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 2.7f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 2.33f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 3.299999f;
            m_ModeParam[7].m_Curve[1].m_Point[2].y = 3.1f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 8.0f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 7.0f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 9.0f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 5.499999f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 6.65f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[2].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 4.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 4.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 3.8f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 3.77f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[3].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 2.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 2.5f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 2.48f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 3.1f;
            m_ModeParam[7].m_Curve[1].m_Point[3].y = 2.8f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 9.499998f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 9.0f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 11.0f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 6.67f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 15.000002f;
            m_ModeParam[7].m_Curve[1].m_Point[3].z = 18.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[4].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[4].x = 5.0f;
            m_ModeParam[7].m_Curve[1].m_Point[4].x = 5.0f;
            m_ModeParam[0].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[4].y = 3.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[4].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[4].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[4].y = 3.05f;
            m_ModeParam[6].m_Curve[1].m_Point[4].y = 3.400002f;
            m_ModeParam[7].m_Curve[1].m_Point[4].y = 3.2f;
            m_ModeParam[0].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[1].m_Curve[1].m_Point[4].z = 6.5f;
            m_ModeParam[2].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[3].m_Curve[1].m_Point[4].z = 7.5f;
            m_ModeParam[4].m_Curve[1].m_Point[4].z = 5.500001f;
            m_ModeParam[5].m_Curve[1].m_Point[4].z = 6.4f;
            m_ModeParam[6].m_Curve[1].m_Point[4].z = 5.5f;
            m_ModeParam[7].m_Curve[1].m_Point[4].z = 7.500002f;
            m_ModeParam[0].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[2].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[3].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[5].m_Curve[1].m_Point[5].x = 6.0f;
            m_ModeParam[6].m_Curve[1].m_Point[5].x = 4.5f;
            m_ModeParam[7].m_Curve[1].m_Point[5].x = 4.5f;
            m_ModeParam[0].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[1].m_Curve[1].m_Point[5].y = 3.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[3].m_Curve[1].m_Point[5].y = 3.8f;
            m_ModeParam[4].m_Curve[1].m_Point[5].y = 3.000001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].y = 3.06f;
            m_ModeParam[6].m_Curve[1].m_Point[5].y = 3.400001f;
            m_ModeParam[7].m_Curve[1].m_Point[5].y = 3.199999f;
            m_ModeParam[0].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[1].m_Curve[1].m_Point[5].z = 7.0f;
            m_ModeParam[2].m_Curve[1].m_Point[5].z = 6.3f;
            m_ModeParam[3].m_Curve[1].m_Point[5].z = 8.0f;
            m_ModeParam[4].m_Curve[1].m_Point[5].z = 6.400001f;
            m_ModeParam[5].m_Curve[1].m_Point[5].z = 6.44f;
            m_ModeParam[6].m_Curve[1].m_Point[5].z = 6.0f;
            m_ModeParam[7].m_Curve[1].m_Point[5].z = 8.000004f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 17.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 20.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 20.18f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[7].m_Curve[1].m_fSlant = 40.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.75f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.3f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.4f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.35f;
            m_ModeParam[7].m_Curve[1].m_fLookScaleX = 0.35f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[7].m_Curve[1].m_fHeightBase = 0.5f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[7].m_Curve[1].m_fHeightRate = 0.08f;
            m_ModeParam[0].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[1].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[2].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[3].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[4].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[5].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[6].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[7].m_Curve[1].m_fHeightRateChr = 0.19f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.96f;
            m_ModeParam[2].m_fLerpRate = 0.96f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96f;
            m_ModeParam[6].m_fLerpRate = 0.96435f;
            m_ModeParam[7].m_fLerpRate = 0.987f;
        }
    }
}

msw::CCameraBasketGame::~CCameraBasketGame() {}
