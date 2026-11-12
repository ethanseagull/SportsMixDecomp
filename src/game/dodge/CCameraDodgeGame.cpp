// msw::CCameraDodgeGame: the camera of a dodgeball match. It picks a camera
// mode from the game state (attacking or defending, an air trick, a team
// mate jumping for the ball, the special views the game asks for), finds the
// point to look at, and places the camera on a curve read from the mode's
// parameters, which SetCamParam fills. The shape follows HockeyF's
// CCameraHockeyGame; the look point and the zoom are the dodgeball's own.
#include "game/msw/CCameraDodgeGame.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"

namespace {

    // The larger of a and b.
    inline float Max(float a, float b)
    {
        if (a > b) {
            b = a;
        }
        return b;
    }

    // fValue brought into [fMin, fMax].
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

    // (name guessed) The zoom works only with bit 0x20 of the camera's flags
    // set and bit 0x10 clear.
    inline int IsZoomEnable(unsigned long flags)
    {
        return (flags & 0x20) ? !(flags & 0x10) : 0;
    }

    // (name guessed) A look point brought into the court: x within 7, z
    // from 0 to 12.
    inline void ClampToCourt(bcs::FVEC3 &rPos)
    {
        rPos.x = Clamp(rPos.x, -7.0f, 7.0f);
        rPos.z = Clamp(rPos.z, 0.0f, 12.0f);
    }

    // (guessed helper) From fTo towards fFrom by fRate.
    inline float Blend(float fRate, float fFrom, float fTo)
    {
        return fRate * (fFrom - fTo) + fTo;
    }

    // (guessed helper, as in VolleyF's CCameraVolleyBoss) The horizontal
    // distance between two points.
    inline float DistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        return bcs::FVEC3Dist(&from, &to);
    }

    // (name guessed) The character doing an air trick the camera follows:
    // none once it has finished its swing or is falling, unless it can still
    // move the arts target marker.
    inline msw::CChrCore *GetAirTrickCore(const msw::CGameCamera *pCam)
    {
        msw::CChrCore *pCore = pCam->GetAirTrickCoreAll();
        if (pCore == 0) {
            return 0;
        }
        if (pCore->CanbeMoveDodgeArtsTargetMarkerPos()) {
            return pCore;
        }
        if (pCore->IsAirTrickSwingEnd()) {
            return 0;
        }
        if (pCore->IsAirTrickFall()) {
            return 0;
        }
        return pCore;
    }

    // (name guessed) The first character of the camera's team, driven by a
    // pad, that is in state 5 jumping for the ball.
    inline msw::CChrCore *SearchBallJumpCore(const msw::CGameCamera *pCam)
    {
        for (long i = 0; i < msw::CChrMan::GetChrCoreNum(); i++) {
            msw::CChrCore *pCore = msw::CChrMan::GetChrCore(i);
            if (pCore != 0 && !pCore->IsDisable() && pCam->IsMyTeamCore(pCore) && !pCam->IsComCore(pCore) &&
                pCore->GetState() == 5 && pCore->IsDodgeOnBallJump()) {
                return pCore;
            }
        }
        return 0;
    }
}

msw::CCameraDodgeGame::CCameraDodgeGame(long view) : CCameraDodge(view)
{
    float dist;
    bcs::CWorldEnv::SetViewFovy(view, 33.0f);

    // The sum is computed at run time in the original (fadds), as in
    // CCameraBoss, so the distance is a local here.
    m_Unk100 = 8.0f;
    dist = 12.0f;
    float y = -(4.5f + dist);

    m_Unk104 = dist;
    m_Unk108 = 0.0f;
    m_Unk10C = 0.0f;
    m_Unk110 = y;
    m_Unk1D8 = 0;
    m_nUnk250 = 0;
    m_nOffense = 1;
    m_Unk1BC = 0;
    m_Unk1C0 = 0.0f;
    m_Unk1C4 = 0.0f;
    m_Unk1CC = 0.0f;
    m_Unk1D0 = 0.0f;
    m_Unk1D4 = 0.0f;
    m_Unk240 = 0.97f;
    SetCamParam();
}

// The zoom (m_Unk1C0) while the team's leader holds the ball close to it:
// state 1 holds, state 2 is left when the ball is more than 3.5 away.
// Returns 0 when the zoom is off for this camera.
int msw::CCameraDodgeGame::MakeingCamZoom()
{
    if (m_UnkFC & 0x1) {
        m_Unk1C0 = 0.0f;
        return 1;
    }

    if (!IsZoomEnable(m_UnkFC)) {
        return 0;
    }

    CChrCore *pLeader = GetMyTeamLeaderCore();
    CBall *pBall = GetActiveBall();
    if (pLeader == 0 || pBall == 0) {
        return 0;
    }

    m_Unk1D4 = 2.8f;
    float fMid = (m_Unk1D4 - m_Unk1CC) * 0.5f + m_Unk1CC;
    m_Unk1D0 = fMid * 0.8f;

    bcs::FVEC3 chrPos = pLeader->GetPos();
    bcs::FVEC3 ballPos = pBall->GetPos();
    float fDist = DistXZ(chrPos, ballPos);

    switch (m_Unk1BC) {
        case 1:
            if (!pLeader->IsHoldBall()) {
                m_Unk1BC = 0;
                m_Unk1C4 = 0.0f;
            }
            break;
        case 2:
            if (fDist > 3.5f) {
                m_Unk1BC = 0;
                m_Unk1C4 = 0.0f;
            }
            break;
    }

    if (m_Unk1BC == 0) {
        m_Unk1C0 = Blend(0.96f, m_Unk1C0, 0.0f);
    } else if (m_Unk1BC == 1) {
        if (m_Unk1CC <= 0.0f) {
            m_Unk1C0 = 0.9f * (m_Unk1C0 - m_Unk1D4) + m_Unk1D4;
        } else {
            // Over 30 frames along a spline through the three heights.
            float fStart = 3.0f;
            float fEnd = 30.0f;
            float fFrame = (fEnd - fStart) * 0.5f + fStart;
            if (m_Unk1C4 <= fStart) {
                m_Unk1C0 = m_Unk1CC * (m_Unk1C4 / fStart);
            } else {
                m_Unk1C0 = bcs::BCS_Vec_BetaSplineCurveInter3(fStart, m_Unk1CC, fFrame, m_Unk1D0, fEnd, m_Unk1D4,
                                                              m_Unk1C4);
            }
        }
    } else if (m_Unk1BC == 2) {
        m_Unk1C0 = 0.9f * (m_Unk1C0 - m_Unk1D4) + m_Unk1D4;
    }
    m_Unk1C4 += GetFrameRateF();
    return 1;
}

void msw::CCameraDodgeGame::MainCamera()
{
    if (!CanbeMain()) {
        return;
    }

    CGameCamera::MainCamera();
    bcs::CWorldEnv::SetViewFovy(m_nView, m_fFovy);

    // Attacking while a team mate holds the ball or does an air trick.
    CChrCore *pHold = GetHoldBallChrCore(-1);
    if (pHold != 0) {
        m_nOffense = 1;
        if (pHold->GetTeamNo() != GetTeamNo()) {
            m_nOffense = 0;
        }
    } else {
        CChrCore *pTrick = GetAirTrickCore(this);
        if (pTrick != 0) {
            m_nOffense = 1;
            if (pTrick->GetTeamNo() != GetTeamNo()) {
                m_nOffense = 0;
            }
        }
    }

    long mode;
    int bForce = 0;
    if (CGame::g_GameWork.m_nFlag18 & 0x8) {
        mode = 0;
    } else {
        // Keep following the character the camera followed last frame.
        mode = -1;
        if (m_Unk228 == 11 || m_Unk228 == 12) {
            if (m_Unk1D8 == reinterpret_cast<unsigned long>(GetAirTrickCore(this))) {
                mode = GetCamMode();
            }
        } else if (m_Unk228 == 9 || m_Unk228 == 10) {
            if (m_Unk1D8 == reinterpret_cast<unsigned long>(SearchBallJumpCore(this))) {
                mode = GetCamMode();
            }
        }

        if (mode < 0) {
            m_Unk1D8 = 0;
            CChrCore *pCore = GetAirTrickCore(this);
            if (pCore != 0) {
                mode = IsOffense() ? 11 : 12;
                m_Unk1D8 = reinterpret_cast<unsigned long>(pCore);
                bForce = 1;
            } else {
                pCore = SearchBallJumpCore(this);
                if (pCore != 0) {
                    mode = IsOffense() ? 9 : 10;
                    m_Unk1D8 = reinterpret_cast<unsigned long>(pCore);
                    bForce = 1;
                }
            }
        }
        if (mode < 0 && CGame::GetExceptionalCameraType() == 2 && pHold != 0 && pHold->IsDodgeInfield()) {
            mode = IsOffense() ? 13 : 14;
        }
        if (mode < 0) {
            mode = IsOffense() ? 1 : 2;
        }
    }
    SetCamMode(mode, bForce);
    MakeingCamZoom();

    if (GetCamMode() == 0) {
        float lookRate = m_fLookRate;
        float frontRate = m_fFrontRate;
        bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
        long mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
        bcs::FVEC3 at = look;
        ClampToCourt(at);
        CalcLookPos(at, arg2, arg3, lookRate, mask);
        bcs::FVEC3 pos = m_Unk118;
        bcs::FVEC3 target(pos.x - m_fNormalOffX, 0.0f, m_Unk110);
        MakeingFrontCamData(pos, target, m_fNormalPosY, m_fNormalTarY, m_fNormalLen, frontRate, 0.0f, m_Unk1C0);
    } else if (GetCamMode() == 11) {
        float frontRate = m_fFrontRate;
        float slant;
        float scaleX;
        GetOffDefParam(&scaleX, &slant);
        float lookRate = m_fLookRate;
        bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
        long mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
        bcs::FVEC3 at = look;
        ClampToCourt(at);
        CalcLookPos(at, arg2, arg3, lookRate, mask);
        bcs::FVEC3 pos = m_Unk118;
        float len;
        float tarY;
        float posY;
        GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
        pos.x *= scaleX;
        bcs::FVEC3 target;
        MakeingOffSlantCamTar(target, pos, GetUnk108Vec(), slant);
        MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
    } else if (GetCamMode() == 12) {
        float frontRate = m_fFrontRate;
        float slant;
        float scaleX;
        GetOffDefParam(&scaleX, &slant);
        float lookRate = m_fLookRate;
        bcs::FVEC3 look(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
        bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
        long mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
        bcs::FVEC3 at = look;
        ClampToCourt(at);
        CalcLookPos(at, arg2, arg3, lookRate, mask);
        bcs::FVEC3 pos = m_Unk118;
        float len;
        float tarY;
        float posY;
        GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);
        pos.x *= scaleX;
        bcs::FVEC3 target;
        MakeingDefSlantCamTar(target, pos, GetUnk108Vec(), slant);
        MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
    } else {
        MainCamera_Normal();
    }

    CalcCamData(GetCamModeLerpRate());
}

// The attacking and defending views: the look point from the players, the
// camera on the mode's curve, or on its fixed numbers while the ball's
// thrower is out of the court (modes 3/4) or while the ball holder is in it
// (modes 13/14).
void msw::CCameraDodgeGame::MainCamera_Normal()
{
    // Declared in the order of the original's stack layout.
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
    bcs::FVEC3 at;
    bcs::FVEC3 arg3;
    bcs::FVEC3 arg2;
    bcs::FVEC3 look;
    look.Set(0.0f, 0.0f, 0.0f);
    arg2.Set(0.0f, 0.0f, 0.0f);
    arg3.Set(0.0f, 0.0f, 0.0f);
    long mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
    at = look;
    ClampToCourt(at);
    CalcLookPos(at, arg2, arg3, lookRate, mask);
    pos = m_Unk118;
    GetPosYTarYLen(&posY, &tarY, &len, pos.x, pos.z);

    if (m_Unk228 == 3 || m_Unk228 == 4) {
        CBall *pBall = GetActiveBall();
        if (pBall != 0) {
            long nShot = pBall->GetShotChrIdx();
            if (nShot >= 0) {
                CChrCore *pShot = CChrMan::GetChrCore(nShot);
                if (pShot != 0 && !pShot->IsDodgeInfield()) {
                    if (GetCamMode() == 3) {
                        posY = m_ModeParam[1].m_Curve[0].m_fHoldPosY;
                        tarY = m_ModeParam[1].m_Curve[0].m_fHoldTarY;
                        len = m_ModeParam[1].m_Curve[0].m_fHoldLen;
                    } else {
                        posY = m_ModeParam[1].m_Curve[1].m_fHoldPosY;
                        tarY = m_ModeParam[1].m_Curve[1].m_fHoldTarY;
                        len = m_ModeParam[1].m_Curve[1].m_fHoldLen;
                    }
                }
            }
        }
    } else if (m_Unk228 == 13 || m_Unk228 == 14) {
        CChrCore *pHold = GetHoldBallChrCore(-1);
        if (pHold != 0 && !pHold->IsDodgeInfield()) {
            if (GetCamMode() == 13) {
                posY = m_ModeParam[6].m_Curve[0].m_fHoldPosY;
                tarY = m_ModeParam[6].m_Curve[0].m_fHoldTarY;
                len = m_ModeParam[6].m_Curve[0].m_fHoldLen;
            } else {
                posY = m_ModeParam[6].m_Curve[1].m_fHoldPosY;
                tarY = m_ModeParam[6].m_Curve[1].m_fHoldTarY;
                len = m_ModeParam[6].m_Curve[1].m_fHoldLen;
            }
        }
    }

    target.Set(0.0f, 0.0f, m_Unk110);
    pos.x *= scaleX;
    target.x = pos.x;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
}

// The point to look at: the middle of the balls in play (or of the
// characters holding them) sideways and the farthest of them in depth,
// shifted towards the other characters when they spread beyond the margin.
// rArg3.y lifts the view with the highest holder or ball. Returns a mask of
// the balls held (bit per ball) and the characters looked at (bit 16 on).
long msw::CCameraDodgeGame::GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    float maxX;
    float persRate = 0.035f;
    float minX = 1000.0f;
    maxX = -1000.0f;
    float maxZ = maxX;
    float rate = 0.15f;
    float base = 1.0f;
    long mask = 0;
    CChrCore *pTopCore = 0;
    CBall *pTopBall = 0;
    GetYParam(&base, &rate);
    float height = base;
    bcs::FVEC3 chrPos;

    for (long i = 0; i < CBallMan::GetBallNum(); i++) {
        CBall *pBall = CBallMan::GetBallPtr(i);
        if (pBall == 0 || !pBall->IsActive()) {
            continue;
        }

        bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
        CChrCore *pCore;
        if (m_Unk228 == 11 || m_Unk228 == 12) {
            pCore = GetAirTrickCore(this);
        } else {
            pCore = GetHoldBallChrCore(i);
        }

        if (pCore == 0) {
            pos = GetBallPosXZ(pBall);
            if (pTopBall == 0) {
                pTopBall = pBall;
            } else if (pTopBall->GetPos().y < pBall->GetPos().y) {
                pTopBall = pBall;
            }
        } else {
            mask |= 1 << i;
            // Tested with nothing done either way.
            if (pCore->IsDodgeInfield() && IsMyTeamCore(pCore) && IsPadCore(pCore)) {
            }
            if (pCore->IsDodgeInfield() && IsMyTeamCore(pCore)) {
            }
            pos = GetCorePosXZ(pCore);
            if (pTopCore == 0) {
                pTopCore = pCore;
            } else if (pTopCore->GetTransNodeHeight() < pCore->GetTransNodeHeight()) {
                pTopCore = pCore;
            }
        }

        NormalizePersPos(pos, persRate, -m_Unk104, m_Unk104);
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

    rLook.x = 0.5f * (maxX - minX) + minX;
    rLook.y = 0.0f;
    rLook.z = maxZ;

    float farZ = rLook.z;
    float leftX = rLook.x;
    float rightX = rLook.x;
    if (GetCamMode() != 0 && GetCamMode() != 11 && GetCamMode() != 12) {
        for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
            CChrCore *pCore = CChrMan::GetChrCore(i);
            if (pCore != 0 && !pCore->IsDisable() && (pCore->GetTeamNo() != GetTeamNo() || pCore->IsDodgeInfield()) &&
                !pCore->IsHoldBall()) {
                mask |= 1 << (i + 16);
                chrPos = GetCorePosXZ(pCore);
                NormalizePersPos(chrPos, persRate, -m_Unk104, m_Unk104);
                if (chrPos.x > rightX) {
                    rightX = chrPos.x;
                }
                if (chrPos.x < leftX) {
                    leftX = chrPos.x;
                }
                if (!IsComCore(pCore) &&
                    (!pCore->IsDodgeInfield() || (pCore->IsDodgeInfield() && pCore->GetTeamNo() == GetTeamNo()) ||
                     (pCore->IsDodgeInfield() && pCore->GetTeamNo() != GetTeamNo()))) {
                    if (chrPos.z > farZ) {
                        farZ = chrPos.z;
                    }
                }
            }
        }
    }

    // Shift the look point by the parts of the spread beyond the margins,
    // scaled and limited.
    float right = (rightX - rLook.x) - m_fMarginX;
    float left = m_fMarginX + (leftX - rLook.x);
    if (right < 0.0f) {
        right = 0.0f;
    }
    if (left > 0.0f) {
        left = 0.0f;
    }
    float shift = (right + left) * m_fRateX;
    rLook.x += shift > m_fLimitX ? m_fLimitX : (shift < -m_fLimitX ? -m_fLimitX : shift);
    rLook.z = farZ;

    rLook.x = Clamp(rLook.x, -7.0f, 7.0f);
    rLook.z = Clamp(rLook.z, 0.0f, 12.0f);
    rLook.z *= 0.5f;
    // Called and not used.
    IsOffense();

    // Lift the view with the highest holder (or the highest ball), less the
    // nearer it is to either end of the court.
    float lookZ = 0.0f;
    if (pTopCore != 0) {
        height = Max(height, pTopCore->GetTransNodeHeight());
        bcs::FVEC3 topPos = GetCorePosXZ(pTopCore);
        lookZ = topPos.z;
    } else if (pTopBall != 0) {
        height = Max(height, pTopBall->GetPos().y);
        bcs::FVEC3 topPos = GetBallPosXZ(pTopBall);
        lookZ = topPos.z;
    }
    float limit = lookZ >= 0.0f ? 60.0f : 30.0f;
    rArg3.y = (1.0f - GetRate(__fabsf(Clamp(lookZ, -limit, limit)), 0.0f, limit)) * (rate * (height - base));
    return mask;
}

void msw::CCameraDodgeGame::Reset()
{
    CGameCamera::Reset();
    if (CGame::GetTargetGoalIdx(GetTeamNo()) != 0) {
        m_UnkFC |= 0x2;
    } else {
        m_UnkFC &= ~0x2;
    }
    m_nOffense = 1;
    SetCamParam();
}

int msw::CCameraDodgeGame::IsOffense() const
{
    return m_nOffense;
}

int msw::CCameraDodgeGame::MessageHandler(MSG_ID id, MSG_PARAM *pParam)
{
    GetMyTeamLeaderCore();
    return 0;
}

namespace {

    // The six numbers CalcCurveBlend takes for each axis: the first two
    // points of the curve are used twice (point 0, point 1, point 0,
    // point 1, point 2, point 3).
    inline void GetCurve(float *pX, float *pY, float *pZ, const msw::CCameraDodgeGame::CurveParam &rCurve)
    {
        pX[0] = rCurve.m_Point[0].x;
        pX[1] = rCurve.m_Point[1].x;
        pX[2] = rCurve.m_Point[0].x;
        pX[3] = rCurve.m_Point[1].x;
        pX[4] = rCurve.m_Point[2].x;
        pX[5] = rCurve.m_Point[3].x;
        pY[0] = rCurve.m_Point[0].y;
        pY[1] = rCurve.m_Point[1].y;
        pY[2] = rCurve.m_Point[0].y;
        pY[3] = rCurve.m_Point[1].y;
        pY[4] = rCurve.m_Point[2].y;
        pY[5] = rCurve.m_Point[3].y;
        pZ[0] = rCurve.m_Point[0].z;
        pZ[1] = rCurve.m_Point[1].z;
        pZ[2] = rCurve.m_Point[0].z;
        pZ[3] = rCurve.m_Point[1].z;
        pZ[4] = rCurve.m_Point[2].z;
        pZ[5] = rCurve.m_Point[3].z;
    }
}

void msw::CCameraDodgeGame::GetPosYTarYLen(float *pPosY, float *pTarY, float *pLen, float x, float z)
{
    float near = m_Unk100;
    float far = m_Unk104;
    float curveX[6];
    float curveY[6];
    float curveZ[6];

    switch (m_Unk228) {
        default:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[0].m_Curve[0]);
            break;
        case 2:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[0].m_Curve[1]);
            break;
        case 3:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[1].m_Curve[0]);
            break;
        case 4:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[1].m_Curve[1]);
            break;
        case 5:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[2].m_Curve[0]);
            break;
        case 6:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[2].m_Curve[1]);
            break;
        case 7:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[3].m_Curve[0]);
            break;
        case 8:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[3].m_Curve[1]);
            break;
        case 9:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[4].m_Curve[0]);
            break;
        case 10:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[4].m_Curve[1]);
            break;
        case 11:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[5].m_Curve[0]);
            break;
        case 12:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[5].m_Curve[1]);
            break;
        case 13:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[6].m_Curve[0]);
            break;
        case 14:
            GetCurve(curveX, curveY, curveZ, m_ModeParam[6].m_Curve[1]);
            break;
    }

    *pPosY = CalcCurveBlend(x, z, near, far, curveX[0], curveX[1], curveX[2], curveX[3], curveX[4], curveX[5]);
    *pTarY = CalcCurveBlend(x, z, near, far, curveY[0], curveY[1], curveY[2], curveY[3], curveY[4], curveY[5]);
    *pLen = CalcCurveBlend(x, z, near, far, curveZ[0], curveZ[1], curveZ[2], curveZ[3], curveZ[4], curveZ[5]);
}

void msw::CCameraDodgeGame::GetOffDefParam(float *pScaleX, float *pSlant)
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
    }
}

void msw::CCameraDodgeGame::GetYParam(float *pBase, float *pRate)
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
        default:
            *pBase = 1.0f;
            *pRate = 0.15f;
            break;
    }
}

float msw::CCameraDodgeGame::GetCamModeLerpRate() const
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
        default:
            return 0.96435547f;
    }
}

// The numbers of every camera mode, for the four combinations of bit 0x20
// of m_UnkFC and the view's aspect type (bit 0x8).
void msw::CCameraDodgeGame::SetCamParam()
{
    if (m_UnkFC & 0x20) {
        if (m_UnkFC & 0x8) {
            m_fFovy = 28.0f;
            m_fFrontRate = 0.9277f;
            m_fLookRate = 0.9375f;
            m_fUnk264 = 0.75f;
            m_fMarginX = 4.200001f;
            m_fLimitX = 2.7f;
            m_fRateX = 0.55f;
            m_fUnk274 = 0.5f;
            m_fUnk278 = 30.0f;
            m_fUnk27C = 0.9f;
            m_fNormalPosY = 3.5f;
            m_fNormalTarY = 2.3f;
            m_fNormalLen = 11.500004f;
            m_fNormalOffX = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 9.400002f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 8.900003f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.000001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 6.8f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 5.8f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.3f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 16.500004f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 14.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 16.500004f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 16.500004f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 13.000002f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 11.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 11.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 7.999999f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 9.4f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 8.900002f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 8.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 6.300001f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 6.799999f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 5.8f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 16.500004f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 20.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 16.500004f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 16.500004f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 12.999998f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 11.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 19.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 7.3f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 9.4f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 7.0f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 7.299999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 5.800001f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 6.8f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 5.2f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 14.499998f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 17.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 14.499994f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 14.499994f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 12.999996f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 15.999996f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 17.49999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 7.299998f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 9.399998f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 6.999998f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 7.299997f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 5.8f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 6.8f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 5.2f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 14.499998f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 14.499994f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 14.499994f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 12.999992f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 15.999996f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[0].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[2].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[0].m_fHoldPosY = 5.299998f;
            m_ModeParam[0].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[2].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[0].m_fHoldTarY = 3.8f;
            m_ModeParam[0].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[0].m_fHoldLen = 9.999992f;
            m_ModeParam[2].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[0].m_fHoldLen = 10.499994f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 120.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 4.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 16.500008f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 12.000006f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 16.500008f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 16.500004f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 16.500004f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 21.500004f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 12.000002f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 16.500004f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 16.5f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 16.500004f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 16.5f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 21.5f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 18.999998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 14.49999f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 22.499987f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 16.499983f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 14.499994f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 16.499994f;
            m_ModeParam[0].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_fHoldPosY = 6.299998f;
            m_ModeParam[2].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_fHoldPosY = 4.799998f;
            m_ModeParam[0].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[1].m_fHoldTarY = 4.800001f;
            m_ModeParam[2].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[1].m_fHoldTarY = 3.3f;
            m_ModeParam[0].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[1].m_fHoldLen = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[1].m_fHoldLen = 14.499994f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 140.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.575f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.91f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.90435f;
        } else {
            m_fFovy = 28.0f;
            m_fFrontRate = 0.9277f;
            m_fLookRate = 0.9375f;
            m_fUnk264 = 0.75f;
            m_fMarginX = 2.5f;
            m_fLimitX = 2.7f;
            m_fRateX = 0.35f;
            m_fUnk274 = 0.5f;
            m_fUnk278 = 30.0f;
            m_fUnk27C = 0.9f;
            m_fNormalPosY = 3.5f;
            m_fNormalTarY = 2.3f;
            m_fNormalLen = 11.5f;
            m_fNormalOffX = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 8.000001f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 9.800003f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 8.800003f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.000001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 6.3f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 6.3f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.3f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 19.999994f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 14.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 19.000002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 19.000002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 14.500004f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 14.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 11.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 8.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 7.999999f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 9.800001f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 8.800001f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 8.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 6.300001f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 19.999994f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 20.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 19.000002f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 19.000002f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 14.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 19.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 7.299999f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 7.3f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 9.800001f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 7.299999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 5.799999f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 5.800001f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 7.999999f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 17.49999f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 17.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 16.499994f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 16.499994f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 14.49999f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 18.999992f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 17.49999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 7.299997f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 7.299998f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 9.799999f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 7.299997f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 5.799999f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 5.8f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 8.0f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 16.499994f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 16.499994f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 14.49999f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 18.999992f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[0].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[2].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[0].m_fHoldPosY = 5.299998f;
            m_ModeParam[0].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[2].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[0].m_fHoldTarY = 3.8f;
            m_ModeParam[0].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[0].m_fHoldLen = 9.999992f;
            m_ModeParam[2].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[0].m_fHoldLen = 10.499994f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 120.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 6.799999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 4.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 19.000002f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 12.000006f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 19.000006f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 19.000006f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 19.000006f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 21.500008f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 14.000002f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 18.999998f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 21.000002f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 18.999998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 16.499983f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 22.499987f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 16.499987f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 16.499987f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 16.499987f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 14.49999f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 16.499983f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 16.499994f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 16.499994f;
            m_ModeParam[0].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_fHoldPosY = 6.299998f;
            m_ModeParam[2].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_fHoldPosY = 4.799998f;
            m_ModeParam[0].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[1].m_fHoldTarY = 4.800001f;
            m_ModeParam[2].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[1].m_fHoldTarY = 3.3f;
            m_ModeParam[0].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[1].m_fHoldLen = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[1].m_fHoldLen = 14.499994f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 140.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.575f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.91f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.90435f;
        }
    } else {
        if (m_UnkFC & 0x8) {
            m_fFovy = 28.0f;
            m_fFrontRate = 0.9277f;
            m_fLookRate = 0.9375f;
            m_fUnk264 = 0.75f;
            m_fMarginX = 0.5f;
            m_fLimitX = 2.7f;
            m_fRateX = 0.55f;
            m_fUnk274 = 0.5f;
            m_fUnk278 = 30.0f;
            m_fUnk27C = 0.9f;
            m_fNormalPosY = 3.5f;
            m_fNormalTarY = 2.3f;
            m_fNormalLen = 11.5f;
            m_fNormalOffX = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 8.899996f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.000001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 5.800002f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.3f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 15.999998f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 14.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 16.000002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 16.000002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 16.000002f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 11.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 11.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 7.999999f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 8.900002f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 8.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 6.300001f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 5.8f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 15.999998f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 20.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 16.000002f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 16.000002f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 16.000002f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 11.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 19.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 7.3f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 6.999996f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 7.299999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 5.800001f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 4.800001f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 13.999992f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 17.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 13.999996f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 13.999996f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 13.999996f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 15.999996f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 17.49999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 7.299998f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 6.999998f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 7.299997f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 5.8f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 4.800001f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 13.999992f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 13.999996f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 13.999996f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 13.999996f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 15.999996f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[0].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[2].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[0].m_fHoldPosY = 5.299998f;
            m_ModeParam[0].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[2].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[0].m_fHoldTarY = 3.8f;
            m_ModeParam[0].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[0].m_fHoldLen = 9.999992f;
            m_ModeParam[2].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[0].m_fHoldLen = 10.499994f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 120.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 4.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 4.499998f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 17.000006f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 12.000006f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 17.000006f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 17.000006f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 17.000006f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 21.500008f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 12.000002f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.9f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 17.000002f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 17.000006f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 17.000002f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 17.000002f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 21.000006f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 18.999998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 22.499987f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 14.499987f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 16.499983f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 16.499994f;
            m_ModeParam[0].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_fHoldPosY = 6.299998f;
            m_ModeParam[2].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_fHoldPosY = 4.799998f;
            m_ModeParam[0].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[1].m_fHoldTarY = 4.800001f;
            m_ModeParam[2].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[1].m_fHoldTarY = 3.3f;
            m_ModeParam[0].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[1].m_fHoldLen = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[1].m_fHoldLen = 14.499994f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 140.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.575f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.91f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.90435f;
        } else {
            m_fFovy = 28.0f;
            m_fFrontRate = 0.9277f;
            m_fLookRate = 0.9375f;
            m_fUnk264 = 0.75f;
            m_fMarginX = 2.5f;
            m_fLimitX = 2.7f;
            m_fRateX = 0.35f;
            m_fUnk274 = 0.5f;
            m_fUnk278 = 30.0f;
            m_fUnk27C = 0.9f;
            m_fNormalPosY = 3.5f;
            m_fNormalTarY = 2.3f;
            m_fNormalLen = 11.500008f;
            m_fNormalOffX = 3.0f;
            m_ModeParam[0].m_Curve[0].m_Point[0].x = 8.000001f;
            m_ModeParam[1].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].x = 5.400002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].x = 9.800003f;
            m_ModeParam[5].m_Curve[0].m_Point[0].x = 8.800003f;
            m_ModeParam[6].m_Curve[0].m_Point[0].x = 5.000001f;
            m_ModeParam[0].m_Curve[0].m_Point[0].y = 6.3f;
            m_ModeParam[1].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[2].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[0].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[0].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[0].y = 6.3f;
            m_ModeParam[6].m_Curve[0].m_Point[0].y = 3.3f;
            m_ModeParam[0].m_Curve[0].m_Point[0].z = 19.999994f;
            m_ModeParam[1].m_Curve[0].m_Point[0].z = 14.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[0].z = 19.000002f;
            m_ModeParam[3].m_Curve[0].m_Point[0].z = 19.000002f;
            m_ModeParam[4].m_Curve[0].m_Point[0].z = 14.500004f;
            m_ModeParam[5].m_Curve[0].m_Point[0].z = 14.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[0].z = 11.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[1].x = 8.0f;
            m_ModeParam[1].m_Curve[0].m_Point[1].x = 7.999999f;
            m_ModeParam[2].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[1].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[1].x = 9.800001f;
            m_ModeParam[5].m_Curve[0].m_Point[1].x = 8.800001f;
            m_ModeParam[6].m_Curve[0].m_Point[1].x = 8.0f;
            m_ModeParam[0].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[1].m_Curve[0].m_Point[1].y = 6.300001f;
            m_ModeParam[2].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[1].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[1].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[6].m_Curve[0].m_Point[1].y = 6.3f;
            m_ModeParam[0].m_Curve[0].m_Point[1].z = 19.999994f;
            m_ModeParam[1].m_Curve[0].m_Point[1].z = 20.000002f;
            m_ModeParam[2].m_Curve[0].m_Point[1].z = 19.000002f;
            m_ModeParam[3].m_Curve[0].m_Point[1].z = 19.000002f;
            m_ModeParam[4].m_Curve[0].m_Point[1].z = 14.5f;
            m_ModeParam[5].m_Curve[0].m_Point[1].z = 14.000002f;
            m_ModeParam[6].m_Curve[0].m_Point[1].z = 19.999994f;
            m_ModeParam[0].m_Curve[0].m_Point[2].x = 7.299999f;
            m_ModeParam[1].m_Curve[0].m_Point[2].x = 7.3f;
            m_ModeParam[2].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[3].m_Curve[0].m_Point[2].x = 5.4f;
            m_ModeParam[4].m_Curve[0].m_Point[2].x = 9.800001f;
            m_ModeParam[5].m_Curve[0].m_Point[2].x = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[2].x = 7.299999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].y = 5.799999f;
            m_ModeParam[1].m_Curve[0].m_Point[2].y = 5.800001f;
            m_ModeParam[2].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[2].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[2].y = 7.999999f;
            m_ModeParam[6].m_Curve[0].m_Point[2].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[2].z = 17.49999f;
            m_ModeParam[1].m_Curve[0].m_Point[2].z = 17.500002f;
            m_ModeParam[2].m_Curve[0].m_Point[2].z = 16.499994f;
            m_ModeParam[3].m_Curve[0].m_Point[2].z = 16.499994f;
            m_ModeParam[4].m_Curve[0].m_Point[2].z = 14.49999f;
            m_ModeParam[5].m_Curve[0].m_Point[2].z = 18.999992f;
            m_ModeParam[6].m_Curve[0].m_Point[2].z = 17.49999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].x = 7.299997f;
            m_ModeParam[1].m_Curve[0].m_Point[3].x = 7.299998f;
            m_ModeParam[2].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_Point[3].x = 9.799999f;
            m_ModeParam[5].m_Curve[0].m_Point[3].x = 9.5f;
            m_ModeParam[6].m_Curve[0].m_Point[3].x = 7.299997f;
            m_ModeParam[0].m_Curve[0].m_Point[3].y = 5.799999f;
            m_ModeParam[1].m_Curve[0].m_Point[3].y = 5.8f;
            m_ModeParam[2].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[0].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[0].m_Point[3].y = 7.3f;
            m_ModeParam[5].m_Curve[0].m_Point[3].y = 8.0f;
            m_ModeParam[6].m_Curve[0].m_Point[3].y = 5.799999f;
            m_ModeParam[0].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[1].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[2].m_Curve[0].m_Point[3].z = 16.499994f;
            m_ModeParam[3].m_Curve[0].m_Point[3].z = 16.499994f;
            m_ModeParam[4].m_Curve[0].m_Point[3].z = 14.49999f;
            m_ModeParam[5].m_Curve[0].m_Point[3].z = 18.999992f;
            m_ModeParam[6].m_Curve[0].m_Point[3].z = 17.499994f;
            m_ModeParam[0].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[2].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[0].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[0].m_fHoldPosY = 5.299998f;
            m_ModeParam[0].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[2].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[0].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[0].m_fHoldTarY = 3.8f;
            m_ModeParam[0].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[0].m_fHoldLen = 9.999992f;
            m_ModeParam[2].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[0].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[0].m_fHoldLen = 10.499994f;
            m_ModeParam[0].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[0].m_fSlant = 120.0f;
            m_ModeParam[6].m_Curve[0].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[0].m_fLookScaleX = 0.475f;
            m_ModeParam[6].m_Curve[0].m_fLookScaleX = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[0].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[0].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_Curve[1].m_Point[0].x = 6.799999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].x = 5.399999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].x = 4.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[0].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[0].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[0].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[0].y = 3.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[0].z = 19.000002f;
            m_ModeParam[1].m_Curve[1].m_Point[0].z = 12.000006f;
            m_ModeParam[2].m_Curve[1].m_Point[0].z = 19.000006f;
            m_ModeParam[3].m_Curve[1].m_Point[0].z = 19.000006f;
            m_ModeParam[4].m_Curve[1].m_Point[0].z = 19.000006f;
            m_ModeParam[5].m_Curve[1].m_Point[0].z = 21.500008f;
            m_ModeParam[6].m_Curve[1].m_Point[0].z = 14.000002f;
            m_ModeParam[0].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].x = 5.299999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].x = 5.899999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].x = 6.799999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[2].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[3].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[4].m_Curve[1].m_Point[1].y = 3.899999f;
            m_ModeParam[5].m_Curve[1].m_Point[1].y = 4.499999f;
            m_ModeParam[6].m_Curve[1].m_Point[1].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[1].z = 18.999998f;
            m_ModeParam[1].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[2].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[3].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[4].m_Curve[1].m_Point[1].z = 19.000002f;
            m_ModeParam[5].m_Curve[1].m_Point[1].z = 21.000002f;
            m_ModeParam[6].m_Curve[1].m_Point[1].z = 18.999998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[1].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[2].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[2].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[2].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[2].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[2].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[2].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[2].z = 16.499983f;
            m_ModeParam[1].m_Curve[1].m_Point[2].z = 22.499987f;
            m_ModeParam[2].m_Curve[1].m_Point[2].z = 16.499987f;
            m_ModeParam[3].m_Curve[1].m_Point[2].z = 16.499987f;
            m_ModeParam[4].m_Curve[1].m_Point[2].z = 16.499987f;
            m_ModeParam[5].m_Curve[1].m_Point[2].z = 14.49999f;
            m_ModeParam[6].m_Curve[1].m_Point[2].z = 16.499983f;
            m_ModeParam[0].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[1].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[2].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_Point[3].x = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_Point[3].x = 6.799998f;
            m_ModeParam[0].m_Curve[1].m_Point[3].y = 5.299999f;
            m_ModeParam[1].m_Curve[1].m_Point[3].y = 5.300001f;
            m_ModeParam[2].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[3].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[4].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[5].m_Curve[1].m_Point[3].y = 3.9f;
            m_ModeParam[6].m_Curve[1].m_Point[3].y = 5.299999f;
            m_ModeParam[0].m_Curve[1].m_Point[3].z = 16.499994f;
            m_ModeParam[1].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[3].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[4].m_Curve[1].m_Point[3].z = 16.49999f;
            m_ModeParam[5].m_Curve[1].m_Point[3].z = 14.49999f;
            m_ModeParam[6].m_Curve[1].m_Point[3].z = 16.499994f;
            m_ModeParam[0].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[1].m_Curve[1].m_fHoldPosY = 6.299998f;
            m_ModeParam[2].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[3].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[4].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[5].m_Curve[1].m_fHoldPosY = 5.399998f;
            m_ModeParam[6].m_Curve[1].m_fHoldPosY = 4.799998f;
            m_ModeParam[0].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[1].m_Curve[1].m_fHoldTarY = 4.800001f;
            m_ModeParam[2].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[3].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[4].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[5].m_Curve[1].m_fHoldTarY = 3.9f;
            m_ModeParam[6].m_Curve[1].m_fHoldTarY = 3.3f;
            m_ModeParam[0].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[1].m_Curve[1].m_fHoldLen = 16.49999f;
            m_ModeParam[2].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[3].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[4].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[5].m_Curve[1].m_fHoldLen = 13.999992f;
            m_ModeParam[6].m_Curve[1].m_fHoldLen = 14.499994f;
            m_ModeParam[0].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[1].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[2].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[3].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[4].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[5].m_Curve[1].m_fSlant = 140.0f;
            m_ModeParam[6].m_Curve[1].m_fSlant = 0.0f;
            m_ModeParam[0].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[1].m_Curve[1].m_fLookScaleX = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[3].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[4].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[5].m_Curve[1].m_fLookScaleX = 0.575f;
            m_ModeParam[6].m_Curve[1].m_fLookScaleX = 0.475f;
            m_ModeParam[0].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[1].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[2].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[3].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[4].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[5].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[6].m_Curve[1].m_fHeightBase = 1.0f;
            m_ModeParam[0].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[1].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[2].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[3].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[4].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[5].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[6].m_Curve[1].m_fHeightRate = 0.15f;
            m_ModeParam[0].m_fLerpRate = 0.96435f;
            m_ModeParam[1].m_fLerpRate = 0.91f;
            m_ModeParam[2].m_fLerpRate = 0.96435f;
            m_ModeParam[3].m_fLerpRate = 0.96435f;
            m_ModeParam[4].m_fLerpRate = 0.96435f;
            m_ModeParam[5].m_fLerpRate = 0.96435f;
            m_ModeParam[6].m_fLerpRate = 0.90435f;
        }
    }
}

msw::CCameraDodgeGame::~CCameraDodgeGame() {}
