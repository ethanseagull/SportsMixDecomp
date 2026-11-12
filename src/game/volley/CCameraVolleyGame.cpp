// msw::CCameraVolleyGame: the camera of a volley match. It picks a camera
// mode from the game state (a team mate serving, spiking, doing an air
// trick, blocking or enduring, else the normal view), finds the point to
// look at and places the camera with the mode's numbers, which SetCamParam
// fills. HockeyF's CCameraHockeyGame and DodgeF's CCameraDodgeGame have the
// same shape.
#include "game/msw/CCameraVolleyGame.hpp"
#include <math.h>
#include "bcs/BCS_Vec.hpp"
#include "bcs/CWorldEnv.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"

namespace {

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

    // (name guessed) A look point brought into the court: x within 8.4, z
    // from 1.1 to 13.4.
    inline void ClampToCourt(bcs::FVEC3 &rPos)
    {
        rPos.x = Clamp(rPos.x, -8.4f, 8.4f);
        rPos.z = Clamp(rPos.z, 1.1f, 13.4f);
    }

    // (names guessed) The first character of the camera's team that is
    // about to serve; the first one, driven by a pad, starting a spike jump
    // (state 45) or a block jump (state 5).
    inline msw::CChrCore *SearchServeCore(const msw::CGameCamera *pCam)
    {
        for (long i = 0; i < msw::CChrMan::GetChrCoreNum(); i++) {
            msw::CChrCore *pCore = msw::CChrMan::GetChrCore(i);
            if (pCore != 0 && !pCore->IsDisable() && pCam->IsMyTeamCore(pCore) && pCore->IsVolleyServeMode()) {
                return pCore;
            }
        }
        return 0;
    }

    inline msw::CChrCore *SearchSpikeCore(const msw::CGameCamera *pCam)
    {
        for (long i = 0; i < msw::CChrMan::GetChrCoreNum(); i++) {
            msw::CChrCore *pCore = msw::CChrMan::GetChrCore(i);
            if (pCore != 0 && !pCore->IsDisable() && pCam->IsMyTeamCore(pCore) && !pCam->IsComCore(pCore) &&
                pCore->GetState() == msw::CChrCore::STATE_SPIKE && pCore->IsVolleySpikeJumpStart()) {
                return pCore;
            }
        }
        return 0;
    }

    inline msw::CChrCore *SearchBlockCore(const msw::CGameCamera *pCam)
    {
        for (long i = 0; i < msw::CChrMan::GetChrCoreNum(); i++) {
            msw::CChrCore *pCore = msw::CChrMan::GetChrCore(i);
            if (pCore != 0 && !pCore->IsDisable() && pCam->IsMyTeamCore(pCore) && !pCam->IsComCore(pCore) &&
                pCore->GetState() == msw::CChrCore::STATE_JUMP) {
                return pCore;
            }
        }
        return 0;
    }

    // (names guessed) Whether the camera follows pCore in the spike, air
    // trick, block or endure mode. Only the air trick mode is kept while
    // another character does an air trick.
    inline int IsSpikeCam(const msw::CGameCamera *pCam, msw::CChrCore *pCore)
    {
        if (pCam->GetAirTrickCore() != 0) {
            return 0;
        }
        if (pCore == 0) {
            return 0;
        }
        if (pCore->GetState() == msw::CChrCore::STATE_SPIKE && pCore->IsVolleySpikeJumpStart() &&
            !pCore->IsVolleySpikeSwingEnd() && !pCore->IsVolleySpikeFall()) {
            return 1;
        }
        return 0;
    }

    inline int IsAirTrickCam(msw::CChrCore *pCore)
    {
        if (pCore == 0) {
            return 0;
        }
        if (pCore->GetState() == msw::CChrCore::STATE_AIR_TRICK) {
            if (pCore->CanbeMoveVolleyTargetMarkerPos()) {
                return 1;
            }
            if (!pCore->IsVolleyAirTrickSwingEnd() && !pCore->IsVolleyAirTrickFall()) {
                return 1;
            }
        }
        return 0;
    }

    inline int IsBlockCam(const msw::CGameCamera *pCam, unsigned long flags, msw::CChrCore *pCore)
    {
        if (!(flags & 0x20)) {
            return 0;
        }
        if (pCam->GetAirTrickCore() != 0) {
            return 0;
        }
        if (pCore == 0) {
            return 0;
        }
        if (pCore->GetState() == msw::CChrCore::STATE_JUMP && !pCore->IsVolleyBlockSuccess() &&
            !pCore->IsVolleyBlockFall()) {
            return 1;
        }
        return 0;
    }

    inline int IsEndureCam(const msw::CGameCamera *pCam, msw::CChrCore *pCore)
    {
        if (pCam->GetAirTrickCore() != 0) {
            return 0;
        }
        if (pCore == 0) {
            return 0;
        }
        return (pCore->GetStatusFlag() >> 13) & 1;
    }
}

msw::CCameraVolleyGame::CCameraVolleyGame(long view) : CCameraVolley(view)
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
    SetCamParam();
}

// The zoom (m_Unk1C0) while the team's leader stands close to where the
// ball comes down and may touch it. Returns 1, or 0 when there is no leader
// or no ball. As CCameraVolleyBoss::MakeingCamZoom, with its own speed.
int msw::CCameraVolleyGame::MakeingCamZoom()
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
        if (nTarget == 0 || m_Unk228 != 0 || pBall->GetTouchNum() != m_Unk1C8) {
            m_Unk1BC = 0;
            m_Unk1C4 = 0.0f;
        }
    }

    if (m_Unk1BC == 0) {
        m_Unk1C0 = Blend(0.9f, m_Unk1C0, 0.0f);
    } else if (m_Unk1BC == 1) {
        m_Unk1C0 += m_fZoomSpeed / 55.0f;
        m_Unk1C0 = Clamp(m_Unk1C0, 0.0f, 2.2f);
    }
    m_Unk1C4 += GetFrameRateF();
    return 1;
}

void msw::CCameraVolleyGame::MainCamera()
{
    if (!CanbeMain()) {
        return;
    }

    CGameCamera::MainCamera();
    bcs::CWorldEnv::SetViewFovy(m_nView, m_Unk250);

    long mode = 0;
    int bForce = 0;
    CChrCore *pServe = SearchServeCore(this);
    if (pServe != 0) {
        if (pServe->IsHoldBall() || pServe->IsVolleyJumpServeMode()) {
            mode = 1;
        } else {
            mode = 2;
        }
    } else {
        // Keep following the character the camera followed last frame.
        long nowMode = GetCamMode();
        if (nowMode == 3) {
            CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
            if (IsSpikeCam(this, pCore)) {
                mode = 3;
            }
        } else if (nowMode == 4) {
            CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
            if (IsAirTrickCam(pCore)) {
                mode = 4;
            }
        } else if (nowMode == 5) {
            CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
            if (IsBlockCam(this, m_UnkFC, pCore)) {
                mode = 5;
            }
        } else if (nowMode == 6) {
            CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
            if (IsEndureCam(this, pCore)) {
                mode = 6;
            }
        }

        if (mode == 0) {
            m_Unk1D8 = 0;
            CChrCore *pCore = SearchSpikeCore(this);
            if (IsSpikeCam(this, pCore)) {
                m_Unk1D8 = reinterpret_cast<unsigned long>(pCore);
                mode = 3;
                bForce = 1;
            } else {
                pCore = GetAirTrickCore();
                if (IsAirTrickCam(pCore)) {
                    m_Unk1D8 = reinterpret_cast<unsigned long>(pCore);
                    mode = 4;
                    bForce = 1;
                } else {
                    pCore = SearchBlockCore(this);
                    if (IsBlockCam(this, m_UnkFC, pCore)) {
                        m_Unk1D8 = reinterpret_cast<unsigned long>(pCore);
                        mode = 5;
                        bForce = 1;
                    } else {
                        pCore = GetEndureCore();
                        if (IsEndureCam(this, pCore)) {
                            m_Unk1D8 = reinterpret_cast<unsigned long>(pCore);
                            mode = 6;
                            bForce = 1;
                        }
                    }
                }
            }
        }
    }
    SetCamMode(mode, bForce);
    MakeingCamZoom();

    long nMode = GetCamMode();
    if (nMode == 1) {
        MainCamera_ServeWait();
    } else if (nMode == 2) {
        MainCamera_ServeThrow();
    } else if (nMode == 3) {
        MainCamera_Spike();
    } else if (nMode == 4) {
        MainCamera_AirTrick();
    } else if (nMode == 5) {
        MainCamera_Block();
    } else if (nMode == 6) {
        MainCamera_Endure();
    } else {
        MainCamera_Normal();
    }
    CalcCamData(0.964355f);
}

// Waiting to serve: look at the server, higher for a jump serve and for the
// big characters (6, 7 and 10).
void msw::CCameraVolleyGame::MainCamera_ServeWait()
{
    CChrCore *pCore = SearchServeCore(this);
    if (pCore == 0) {
        return;
    }

    float frontRate = 0.0f;
    if (pCore->IsVolleyJumpServeMode()) {
        frontRate = 0.982f;
    }
    bcs::FVEC3 corePos = GetCorePosXZ(pCore);
    float posY = m_ServeWait.m_fPosY;
    float tarY = m_ServeWait.m_fTarY;
    float len = m_ServeWait.m_fLen;
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);

    long id = pCore->GetChrTypeID();
    float rateY = m_fServeRateY;
    float tarRateY = m_fServeTarRateY;
    switch (id) {
        case 6:
        case 7:
        case 10:
            rateY = m_fServeRateYBig;
            tarRateY = m_fServeTarRateYBig;
            break;
    }
    arg2.y = rateY * pCore->GetPosY();
    arg3.y = tarRateY * pCore->GetPosY();

    bcs::FVEC3 look = corePos;
    ClampToCourt(look);
    CalcLookPos(look, arg2, arg3, 0.0f, 30);

    pos = m_Unk118;
    MakeingSlantCamTar(target, pos, GetUnk108Vec(), m_ServeWait.m_fSlantA, m_ServeWait.m_fSlantB);
    pos.x *= m_ServeWait.m_fScaleX;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, 0.0f, m_Unk1C0);
}

// The serve thrown up: look at the server.
void msw::CCameraVolleyGame::MainCamera_ServeThrow()
{
    CChrCore *pCore = SearchServeCore(this);
    if (pCore == 0) {
        return;
    }

    float frontRate = 0.85f;
    bcs::FVEC3 corePos = GetCorePosXZ(pCore);
    float posY = m_ServeThrow.m_fPosY;
    float tarY = m_ServeThrow.m_fTarY;
    float len = m_ServeThrow.m_fLen;
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
    arg2.y = 1.3f * pCore->GetPosY();
    arg3.y = pCore->GetPosY();

    bcs::FVEC3 look = corePos;
    ClampToCourt(look);
    CalcLookPos(look, arg2, arg3, 0.0f, 30);

    pos = m_Unk118;
    MakeingSlantCamTar(target, pos, GetUnk108Vec(), m_ServeThrow.m_fSlantA, m_ServeThrow.m_fSlantB);
    pos.x *= m_ServeThrow.m_fScaleX;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, 0.0f, m_Unk1C0);
}

// A team mate's spike (m_Unk1D8): look at the spiker's hand.
void msw::CCameraVolleyGame::MainCamera_Spike()
{
    CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
    if (pCore == 0) {
        return;
    }

    float frontRate = 0.982f;
    bcs::FVEC3 corePos = GetCorePosXZ(pCore);
    float posY = m_Spike.m_fPosY;
    float tarY = m_Spike.m_fTarY;
    float len = m_Spike.m_fLen;
    float slantA = m_Spike.m_fSlantA;
    float slantB = m_Spike.m_fSlantB;
    float scaleX = m_Spike.m_fScaleX;
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 arg2;
    bcs::FVEC3 arg3;
    bcs::FVEC3 offset(0.0f, 0.0f, 0.0f);
    if (pCore->IsVolleySpikeJumpStart()) {
        if (pCore->IsVolleySuperSpikeJump()) {
            posY = m_SuperSpike.m_fPosY;
            tarY = m_SuperSpike.m_fTarY;
            len = m_SuperSpike.m_fLen;
            slantA = m_SuperSpike.m_fSlantA;
            slantB = m_SuperSpike.m_fSlantB;
            scaleX = m_SuperSpike.m_fScaleX;
            pCore->GetVolleySpikeOffsetPos(1, offset);
        } else {
            pCore->GetVolleySpikeOffsetPos(0, offset);
        }
    }
    arg2.Set(0.0f, 0.0f, 0.0f);
    arg3.Set(0.0f, 0.0f, 0.0f);
    arg2.y = 1.2f * (0.75f * offset.y + pCore->GetPosY());
    arg3.y = 0.75f * offset.y + pCore->GetPosY();

    bcs::FVEC3 look = corePos;
    ClampToCourt(look);
    CalcLookPos(look, arg2, arg3, 0.0f, 40);

    pos = m_Unk118;
    MakeingSlantCamTar(target, pos, GetUnk108Vec(), slantA, slantB);
    pos.x *= scaleX;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
}

// A team mate's air trick (m_Unk1D8): the camera's height, target height
// and length come from the m_AirTrick curve over the court.
void msw::CCameraVolleyGame::MainCamera_AirTrick()
{
    float curveX[6];
    float curveY[6];
    float curveZ[6];
    CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
    if (pCore == 0) {
        return;
    }

    float frontRate = 0.982f;
    bcs::FVEC3 corePos = GetCorePosXZ(pCore);
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
    arg2.y = 1.2f * pCore->GetPosY();
    arg3.y = pCore->GetPosY();

    bcs::FVEC3 look = corePos;
    ClampToCourt(look);
    CalcLookPos(look, arg2, arg3, 0.0f, 50);

    pos = m_Unk118;
    float maxX = 8.4f;
    float maxZ = 12.3f;
    float z = pos.z - 1.1f;
    curveX[0] = m_AirTrick.m_Point[0].x;
    curveX[1] = m_AirTrick.m_Point[1].x;
    curveX[2] = m_AirTrick.m_Point[0].x;
    curveX[3] = m_AirTrick.m_Point[1].x;
    curveX[4] = m_AirTrick.m_Point[2].x;
    curveX[5] = m_AirTrick.m_Point[3].x;
    curveY[0] = m_AirTrick.m_Point[0].y;
    curveY[1] = m_AirTrick.m_Point[1].y;
    curveY[2] = m_AirTrick.m_Point[0].y;
    curveY[3] = m_AirTrick.m_Point[1].y;
    curveY[4] = m_AirTrick.m_Point[2].y;
    curveY[5] = m_AirTrick.m_Point[3].y;
    curveZ[0] = m_AirTrick.m_Point[0].z;
    curveZ[1] = m_AirTrick.m_Point[1].z;
    curveZ[2] = m_AirTrick.m_Point[0].z;
    curveZ[3] = m_AirTrick.m_Point[1].z;
    curveZ[4] = m_AirTrick.m_Point[2].z;
    curveZ[5] = m_AirTrick.m_Point[3].z;
    z = Clamp(z, 0.0f, maxZ);
    float posY = CalcCurveBlend(pos.x, z, maxX, maxZ, curveX[0], curveX[1], curveX[2], curveX[3], curveX[4],
                                curveX[5]);
    float tarY = CalcCurveBlend(pos.x, z, maxX, maxZ, curveY[0], curveY[1], curveY[2], curveY[3], curveY[4],
                                curveY[5]);
    float len = CalcCurveBlend(pos.x, z, maxX, maxZ, curveZ[0], curveZ[1], curveZ[2], curveZ[3], curveZ[4],
                               curveZ[5]);

    MakeingSlantCamTar(target, pos, GetUnk108Vec(), m_AirTrick.m_fSlantA, m_AirTrick.m_fSlantB);
    pos.x *= m_AirTrick.m_fScaleX;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
}

// A team mate's block (m_Unk1D8).
void msw::CCameraVolleyGame::MainCamera_Block()
{
    CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
    if (pCore == 0) {
        return;
    }

    float frontRate = 0.85f;
    bcs::FVEC3 corePos = GetCorePosXZ(pCore);
    float posY = m_Block.m_fPosY;
    float tarY = m_Block.m_fTarY;
    float len = m_Block.m_fLen;
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
    arg2.y = 1.2f * pCore->GetPosY();
    arg3.y = 1.1f * pCore->GetPosY();

    bcs::FVEC3 look = corePos;
    ClampToCourt(look);
    CalcLookPos(look, arg2, arg3, 0.0f, 60);

    pos = m_Unk118;
    MakeingSlantCamTar(target, pos, GetUnk108Vec(), m_Block.m_fSlantA, m_Block.m_fSlantB);
    pos.x *= m_Block.m_fScaleX;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
}

// A team mate enduring (m_Unk1D8).
void msw::CCameraVolleyGame::MainCamera_Endure()
{
    CChrCore *pCore = reinterpret_cast<CChrCore *>(m_Unk1D8);
    if (pCore == 0) {
        return;
    }

    float frontRate = 0.85f;
    bcs::FVEC3 corePos = GetCorePosXZ(pCore);
    float posY = m_Endure.m_fPosY;
    float tarY = m_Endure.m_fTarY;
    float len = m_Endure.m_fLen;
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 arg2(0.0f, 0.0f, 0.0f);
    bcs::FVEC3 arg3(0.0f, 0.0f, 0.0f);
    arg2.y = 1.2f * pCore->GetPosY();
    arg3.y = 1.1f * pCore->GetPosY();

    bcs::FVEC3 look = corePos;
    ClampToCourt(look);
    CalcLookPos(look, arg2, arg3, 0.0f, 70);

    pos = m_Unk118;
    MakeingSlantCamTar(target, pos, GetUnk108Vec(), m_Endure.m_fSlantA, m_Endure.m_fSlantB);
    pos.x *= m_Endure.m_fScaleX;
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
}

// The normal view: look at the ball, the target markers and the team, the
// camera placed by the m_Normal curve and facing straight down the court.
void msw::CCameraVolleyGame::MainCamera_Normal()
{
    float curveX[6];
    float curveY[6];
    float curveZ[6];
    bcs::FVEC3 target;
    bcs::FVEC3 pos;
    bcs::FVEC3 at;
    bcs::FVEC3 arg3;
    bcs::FVEC3 arg2;
    bcs::FVEC3 look;
    if (GetActiveBall() == 0) {
        return;
    }
    GetTeamNo();

    float lookRate = m_fLookRate;
    look.Set(0.0f, 0.0f, 0.0f);
    arg2.Set(0.0f, 0.0f, 0.0f);
    arg3.Set(0.0f, 0.0f, 0.0f);
    long mask = GetFirstLookInfo_MultiPlay(look, arg2, arg3);
    at = look;
    ClampToCourt(at);
    CalcLookPos(at, arg2, arg3, lookRate, mask);

    pos = m_Unk118;
    float maxX = 8.4f;
    float maxZ = 12.3f;
    float z = pos.z - 1.1f;
    curveX[0] = m_Normal.m_Point[0].x;
    curveX[1] = m_Normal.m_Point[1].x;
    curveX[2] = m_Normal.m_Point[0].x;
    curveX[3] = m_Normal.m_Point[1].x;
    curveX[4] = m_Normal.m_Point[2].x;
    curveX[5] = m_Normal.m_Point[3].x;
    curveY[0] = m_Normal.m_Point[0].y;
    curveY[1] = m_Normal.m_Point[1].y;
    curveY[2] = m_Normal.m_Point[0].y;
    curveY[3] = m_Normal.m_Point[1].y;
    curveY[4] = m_Normal.m_Point[2].y;
    curveY[5] = m_Normal.m_Point[3].y;
    curveZ[0] = m_Normal.m_Point[0].z;
    curveZ[1] = m_Normal.m_Point[1].z;
    curveZ[2] = m_Normal.m_Point[0].z;
    curveZ[3] = m_Normal.m_Point[1].z;
    curveZ[4] = m_Normal.m_Point[2].z;
    curveZ[5] = m_Normal.m_Point[3].z;
    z = Clamp(z, 0.0f, maxZ);
    float posY = CalcCurveBlend(pos.x, z, maxX, maxZ, curveX[0], curveX[1], curveX[2], curveX[3], curveX[4],
                                curveX[5]);
    float tarY = CalcCurveBlend(pos.x, z, maxX, maxZ, curveY[0], curveY[1], curveY[2], curveY[3], curveY[4],
                                curveY[5]);
    float len = CalcCurveBlend(pos.x, z, maxX, maxZ, curveZ[0], curveZ[1], curveZ[2], curveZ[3], curveZ[4],
                               curveZ[5]);

    pos.x *= m_Normal.m_fScaleX;
    target.Set(pos.x, 0.0f, m_Unk110);
    float frontRate = m_fFrontRate;
    if (CGame::g_GameWork.m_nFlag18 & 0x80) {
        frontRate = 0.98877f;
    }
    MakeingFrontCamData(pos, target, posY, tarY, len, frontRate, m_Unk240, m_Unk1C0);
}

// The point to look at in the normal view: the middle of the ball, the
// other team's target markers and the ball's target, shifted towards the
// camera's team when it spreads beyond the margins. Raised while a team
// mate jumps for a spike or does an air trick. Returns the mask for
// CalcLookPos.
long msw::CCameraVolleyGame::GetFirstLookInfo_MultiPlay(bcs::FVEC3 &rLook, bcs::FVEC3 &rArg2, bcs::FVEC3 &rArg3)
{
    float maxZ2;
    bcs::FVEC3 pos;
    CBall *pBall = GetActiveBall();
    float fPersRate = 0.01f;
    float maxY = 0.0f;
    long mask = 10;

    bcs::FVEC3 ballPos;
    ballPos = GetBallPosXZ(pBall);
    NormalizePersPos(ballPos, fPersRate, -m_Unk104, m_Unk104);
    float minX = ballPos.x;
    float maxZ = ballPos.z;
    float maxX = minX;

    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pCore = CChrMan::GetChrCore(i);
        if (pCore != 0 && !pCore->IsDisable() && !IsMyTeamCore(pCore) && GetTargetMarkerPosXZ(pCore, pos)) {
            mask |= 1 << (i + 8);
            NormalizePersPos(pos, fPersRate, -m_Unk104, m_Unk104);
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

    bcs::FVEC3 target;
    if (GetBallTargetPos(target)) {
        NormalizePersPos(target, fPersRate, -m_Unk104, m_Unk104);
        if (target.x > maxX) {
            maxX = target.x;
        }
        if (target.x < minX) {
            minX = target.x;
        }
        if (target.z > maxZ) {
            maxZ = target.z;
        }
        if (m_UnkFC & 0x40) {
            mask = (m_Unk114 & 0x10000) ? (mask | 0x20000) : (mask | 0x10000);
            m_UnkFC &= ~0x40;
        } else if (m_Unk114 & 0x10000) {
            mask |= 0x10000;
        } else {
            mask |= 0x20000;
        }
    } else {
        target = ballPos;
    }

    maxZ2 = rLook.z = maxZ;
    rLook.x = 0.5f * (maxX - minX) + minX;
    float minX2 = rLook.x;
    float maxX2 = minX2;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pCore = CChrMan::GetChrCore(i);
        if (pCore != 0 && !pCore->IsDisable() && IsMyTeamCore(pCore)) {
            pos = GetCorePosXZ(pCore);
            NormalizePersPos(pos, fPersRate, -m_Unk104, m_Unk104);
            if (pos.x > maxX2) {
                maxX2 = pos.x;
            }
            if (pos.x < minX2) {
                minX2 = pos.x;
            }
            if (pos.z > maxZ2) {
                maxZ2 = pos.z;
            }
            if (pCore->GetState() == CChrCore::STATE_SPIKE && pCore->IsVolleySpikeJumpStart()) {
                float y = pCore->GetPosY();
                if (y > maxY) {
                    maxY = y;
                }
            }
        }
    }

    // Shift the look point by the parts of the spread beyond the margins,
    // scaled and limited. Only the far side counts in depth.
    float right = (maxX2 - rLook.x) - m_fMarginX;
    float left = m_fMarginX + (minX2 - rLook.x);
    if (right < 0.0f) {
        right = 0.0f;
    }
    if (left > 0.0f) {
        left = 0.0f;
    }
    float shiftX = (right + left) * m_fRateX;
    rLook.x += shiftX > m_fLimitX ? m_fLimitX : (shiftX < -m_fLimitX ? -m_fLimitX : shiftX);

    float far = (maxZ2 - rLook.z) - m_fMarginZ;
    float near = 0.0f;
    if (far < 0.0f) {
        far = 0.0f;
    }
    if (near > 0.0f) {
        near = 0.0f;
    }
    float shiftZ = (far + near) * m_fRateZ;
    rLook.z += shiftZ > m_fLimitZ ? m_fLimitZ : (shiftZ < -m_fLimitZ ? -m_fLimitZ : shiftZ);

    CChrCore *pTrick = GetAirTrickCoreAll();
    if (maxY > 0.0f) {
        float base = 1.0f;
        if (maxY >= base) {
            float range = 7.0f;
            float lookZ = rLook.z > range ? range : (rLook.z < -range ? -range : rLook.z);
            rArg3.y += (1.0f - GetRate(fabsf(lookZ), 0.0f, range)) * (0.3f * (maxY - base));
        }
    } else if (pTrick != 0) {
        float base = 1.0f;
        float y = pTrick->GetPosY();
        if (y >= base) {
            rArg3.y += m_fAirTrickRateY * (y - base);
        }
    }
    return mask;
}

void msw::CCameraVolleyGame::Reset()
{
    CGameCamera::Reset();
    if (CGame::GetTargetGoalIdx(GetTeamNo()) != 0) {
        m_UnkFC |= 0x2;
    } else {
        m_UnkFC &= ~0x2;
    }
    SetCamParam();
}

// The numbers of every mode, by bits 0x20, 0x10 and 0x8 of m_UnkFC (bit
// 0x8: the view's aspect type is 1).
void msw::CCameraVolleyGame::SetCamParam()
{
    long bit;
    // Guess at the original: its pool has 5.2 just before this function's
    // numbers, from a constant nothing reads. This unused local only keeps
    // that constant in place; it has no effect on behaviour.
    float fUnused = 5.2f;
    unsigned long flags = m_UnkFC;
    if (flags & 0x20) {
        bit = (flags >> 4) & 1;
        if (bit == 0 && !(flags & 0x8)) {
            m_Unk250 = 35.0f;
            m_fFrontRate = 0.854f;
            m_fLookRate = 0.937f;
            m_Unk25C = 5.0f;
            m_fZoomSpeed = 1.7f;
            m_fAirTrickRateY = 0.13f;
            m_fMarginX = 0.0f;
            m_fLimitX = 3.5f;
            m_fRateX = 0.5f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 2.7f;
            m_ServeWait.m_fTarY = 2.0f;
            m_ServeWait.m_fLen = 7.599998f;
            m_ServeWait.m_fSlantB = 3.5f;
            m_ServeWait.m_fSlantA = 0.5f;
            m_ServeWait.m_fScaleX = 0.95f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.4f;
            m_fServeTarRateYBig = 1.22f;
            m_ServeThrow.m_fPosY = 4.999999f;
            m_ServeThrow.m_fTarY = 3.299999f;
            m_ServeThrow.m_fLen = 8.049998f;
            m_ServeThrow.m_fSlantB = 3.5f;
            m_ServeThrow.m_fSlantA = 0.5f;
            m_ServeThrow.m_fScaleX = 0.95f;
            m_Spike.m_fPosY = 3.799999f;
            m_Spike.m_fTarY = 1.7f;
            m_Spike.m_fLen = 6.999999f;
            m_Spike.m_fSlantB = 3.5f;
            m_Spike.m_fSlantA = 0.5f;
            m_Spike.m_fScaleX = 0.95f;
            m_SuperSpike.m_fPosY = 4.999999f;
            m_SuperSpike.m_fTarY = 2.5f;
            m_SuperSpike.m_fLen = 7.999998f;
            m_SuperSpike.m_fSlantB = 3.5f;
            m_SuperSpike.m_fSlantA = 0.5f;
            m_SuperSpike.m_fScaleX = 0.95f;
            m_Block.m_fPosY = 3.299999f;
            m_Block.m_fTarY = 2.1f;
            m_Block.m_fLen = 9.200002f;
            m_Block.m_fSlantB = 3.5f;
            m_Block.m_fSlantA = 0.5f;
            m_Block.m_fScaleX = 0.95f;
            m_Endure.m_fPosY = 2.3f;
            m_Endure.m_fTarY = 0.7f;
            m_Endure.m_fLen = 6.8f;
            m_Endure.m_fSlantB = 3.5f;
            m_Endure.m_fSlantA = 0.5f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 6.79f;
            m_AirTrick.m_Point[0].x = 4.0f;
            m_Normal.m_Point[0].y = 5.11f;
            m_AirTrick.m_Point[0].y = 2.3f;
            m_Normal.m_Point[0].z = 12.8f;
            m_AirTrick.m_Point[0].z = 9.800004f;
            m_Normal.m_Point[1].x = 6.79f;
            m_AirTrick.m_Point[1].x = 4.199999f;
            m_Normal.m_Point[1].y = 5.11f;
            m_AirTrick.m_Point[1].y = 2.3f;
            m_Normal.m_Point[1].z = 12.8f;
            m_AirTrick.m_Point[1].z = 9.499999f;
            m_Normal.m_Point[2].x = 6.68f;
            m_AirTrick.m_Point[2].x = 5.8f;
            m_Normal.m_Point[2].y = 5.12f;
            m_AirTrick.m_Point[2].y = 3.8f;
            m_Normal.m_Point[2].z = 11.8f;
            m_AirTrick.m_Point[2].z = 7.400001f;
            m_Normal.m_Point[3].x = 6.68f;
            m_AirTrick.m_Point[3].x = 5.800003f;
            m_Normal.m_Point[3].y = 5.12f;
            m_AirTrick.m_Point[3].y = 3.800002f;
            m_Normal.m_Point[3].z = 11.8f;
            m_AirTrick.m_Point[3].z = 8.199998f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.9f;
            m_AirTrick.m_fScaleX = 0.73f;
        } else if (bit == 0 && (flags & 0x8)) {
            m_Unk250 = 40.0f;
            m_fFrontRate = 0.854f;
            m_fLookRate = 0.937f;
            m_Unk25C = 5.0f;
            m_fZoomSpeed = 1.7f;
            m_fAirTrickRateY = 0.13f;
            m_fMarginX = 0.0f;
            m_fLimitX = 5.0f;
            m_fRateX = 0.5f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 2.3f;
            m_ServeWait.m_fTarY = 1.6f;
            m_ServeWait.m_fLen = 6.0f;
            m_ServeWait.m_fSlantB = 3.5f;
            m_ServeWait.m_fSlantA = 0.5f;
            m_ServeWait.m_fScaleX = 0.95f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.42f;
            m_fServeTarRateYBig = 1.2f;
            m_ServeThrow.m_fPosY = 4.2f;
            m_ServeThrow.m_fTarY = 2.6f;
            m_ServeThrow.m_fLen = 6.45f;
            m_ServeThrow.m_fSlantB = 3.5f;
            m_ServeThrow.m_fSlantA = 0.5f;
            m_ServeThrow.m_fScaleX = 0.95f;
            m_Spike.m_fPosY = 2.3f;
            m_Spike.m_fTarY = 0.7f;
            m_Spike.m_fLen = 5.0f;
            m_Spike.m_fSlantB = 3.5f;
            m_Spike.m_fSlantA = 0.5f;
            m_Spike.m_fScaleX = 0.95f;
            m_SuperSpike.m_fPosY = 3.8f;
            m_SuperSpike.m_fTarY = 1.3f;
            m_SuperSpike.m_fLen = 5.0f;
            m_SuperSpike.m_fSlantB = 3.5f;
            m_SuperSpike.m_fSlantA = 0.5f;
            m_SuperSpike.m_fScaleX = 0.95f;
            m_Block.m_fPosY = 2.2f;
            m_Block.m_fTarY = 0.55f;
            m_Block.m_fLen = 5.5f;
            m_Block.m_fSlantB = 3.5f;
            m_Block.m_fSlantA = 0.5f;
            m_Block.m_fScaleX = 0.95f;
            m_Endure.m_fPosY = 2.3f;
            m_Endure.m_fTarY = 0.7f;
            m_Endure.m_fLen = 6.8f;
            m_Endure.m_fSlantB = 3.5f;
            m_Endure.m_fSlantA = 0.5f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 4.69f;
            m_AirTrick.m_Point[0].x = 2.3f;
            m_Normal.m_Point[0].y = 3.21f;
            m_AirTrick.m_Point[0].y = 0.7f;
            m_Normal.m_Point[0].z = 9.8f;
            m_AirTrick.m_Point[0].z = 6.8f;
            m_Normal.m_Point[1].x = 4.69f;
            m_AirTrick.m_Point[1].x = 2.3f;
            m_Normal.m_Point[1].y = 3.21f;
            m_AirTrick.m_Point[1].y = 0.7f;
            m_Normal.m_Point[1].z = 9.8f;
            m_AirTrick.m_Point[1].z = 6.8f;
            m_Normal.m_Point[2].x = 5.88f;
            m_AirTrick.m_Point[2].x = 2.3f;
            m_Normal.m_Point[2].y = 4.02f;
            m_AirTrick.m_Point[2].y = 0.7f;
            m_Normal.m_Point[2].z = 9.8f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 5.88f;
            m_AirTrick.m_Point[3].x = 2.3f;
            m_Normal.m_Point[3].y = 4.02f;
            m_AirTrick.m_Point[3].y = 0.7f;
            m_Normal.m_Point[3].z = 9.8f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.52f;
            m_AirTrick.m_fScaleX = 0.8f;
        } else if (bit != 0 && !(flags & 0x8)) {
            m_Unk250 = 25.0f;
            m_fFrontRate = 0.854f;
            m_fLookRate = 0.97f;
            m_Unk25C = 3.0f;
            m_fZoomSpeed = 1.9f;
            m_fAirTrickRateY = 0.0f;
            m_fMarginX = 0.0f;
            m_fLimitX = 2.5f;
            m_fRateX = 0.4f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 1.85f;
            m_ServeWait.m_fTarY = 1.45f;
            m_ServeWait.m_fLen = 7.5f;
            m_ServeWait.m_fSlantB = 3.874f;
            m_ServeWait.m_fSlantA = 0.6f;
            m_ServeWait.m_fScaleX = 0.8f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.7f;
            m_fServeTarRateYBig = 1.45f;
            m_ServeThrow.m_fPosY = 5.099999f;
            m_ServeThrow.m_fTarY = 3.799999f;
            m_ServeThrow.m_fLen = 10.0f;
            m_ServeThrow.m_fSlantB = 3.874f;
            m_ServeThrow.m_fSlantA = 0.6f;
            m_ServeThrow.m_fScaleX = 0.8f;
            m_Spike.m_fPosY = 2.999999f;
            m_Spike.m_fTarY = 1.8f;
            m_Spike.m_fLen = 9.000002f;
            m_Spike.m_fSlantB = 3.874f;
            m_Spike.m_fSlantA = 0.6f;
            m_Spike.m_fScaleX = 0.8f;
            m_SuperSpike.m_fPosY = 4.0f;
            m_SuperSpike.m_fTarY = 2.5f;
            m_SuperSpike.m_fLen = 9.000002f;
            m_SuperSpike.m_fSlantB = 3.874f;
            m_SuperSpike.m_fSlantA = 0.6f;
            m_SuperSpike.m_fScaleX = 0.8f;
            m_Block.m_fPosY = 2.799999f;
            m_Block.m_fTarY = 1.9f;
            m_Block.m_fLen = 9.000002f;
            m_Block.m_fSlantB = 3.874f;
            m_Block.m_fSlantA = 0.6f;
            m_Block.m_fScaleX = 0.8f;
            m_Endure.m_fPosY = 2.2f;
            m_Endure.m_fTarY = 0.9f;
            m_Endure.m_fLen = 6.200001f;
            m_Endure.m_fSlantB = 1.5f;
            m_Endure.m_fSlantA = 0.2f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 5.29f;
            m_AirTrick.m_Point[0].x = 4.6f;
            m_Normal.m_Point[0].y = 4.309999f;
            m_AirTrick.m_Point[0].y = 2.6f;
            m_Normal.m_Point[0].z = 13.200001f;
            m_AirTrick.m_Point[0].z = 9.8f;
            m_Normal.m_Point[1].x = 5.29f;
            m_AirTrick.m_Point[1].x = 4.6f;
            m_Normal.m_Point[1].y = 4.31f;
            m_AirTrick.m_Point[1].y = 2.6f;
            m_Normal.m_Point[1].z = 13.200001f;
            m_AirTrick.m_Point[1].z = 9.8f;
            m_Normal.m_Point[2].x = 4.48f;
            m_AirTrick.m_Point[2].x = 5.3f;
            m_Normal.m_Point[2].y = 3.62f;
            m_AirTrick.m_Point[2].y = 3.700001f;
            m_Normal.m_Point[2].z = 13.0f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 4.480001f;
            m_AirTrick.m_Point[3].x = 5.200001f;
            m_Normal.m_Point[3].y = 3.62f;
            m_AirTrick.m_Point[3].y = 3.799999f;
            m_Normal.m_Point[3].z = 13.0f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.8f;
            m_AirTrick.m_fScaleX = 0.8f;
        } else {
            m_Unk250 = 40.0f;
            m_fFrontRate = 0.919f;
            m_fLookRate = 0.97f;
            m_Unk25C = 2.5f;
            m_fZoomSpeed = 2.2f;
            m_fAirTrickRateY = 0.13f;
            m_fMarginX = 0.0f;
            m_fLimitX = 1.5f;
            m_fRateX = 0.3f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 2.6f;
            m_ServeWait.m_fTarY = 1.8f;
            m_ServeWait.m_fLen = 6.999998f;
            m_ServeWait.m_fSlantB = 3.874f;
            m_ServeWait.m_fSlantA = 0.6f;
            m_ServeWait.m_fScaleX = 0.8f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.4f;
            m_fServeTarRateYBig = 1.15f;
            m_ServeThrow.m_fPosY = 6.199998f;
            m_ServeThrow.m_fTarY = 4.199998f;
            m_ServeThrow.m_fLen = 9.000001f;
            m_ServeThrow.m_fSlantB = 3.874f;
            m_ServeThrow.m_fSlantA = 0.6f;
            m_ServeThrow.m_fScaleX = 0.8f;
            m_Spike.m_fPosY = 2.3f;
            m_Spike.m_fTarY = 0.7f;
            m_Spike.m_fLen = 7.8f;
            m_Spike.m_fSlantB = 3.874f;
            m_Spike.m_fSlantA = 0.6f;
            m_Spike.m_fScaleX = 0.8f;
            m_SuperSpike.m_fPosY = 3.6f;
            m_SuperSpike.m_fTarY = 1.4f;
            m_SuperSpike.m_fLen = 7.8f;
            m_SuperSpike.m_fSlantB = 3.874f;
            m_SuperSpike.m_fSlantA = 0.6f;
            m_SuperSpike.m_fScaleX = 0.8f;
            m_Block.m_fPosY = 4.299998f;
            m_Block.m_fTarY = 2.6f;
            m_Block.m_fLen = 8.999996f;
            m_Block.m_fSlantB = 3.874f;
            m_Block.m_fSlantA = 0.6f;
            m_Block.m_fScaleX = 0.8f;
            m_Endure.m_fPosY = 2.2f;
            m_Endure.m_fTarY = 0.9f;
            m_Endure.m_fLen = 6.200001f;
            m_Endure.m_fSlantB = 1.5f;
            m_Endure.m_fSlantA = 0.2f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 5.9275f;
            m_AirTrick.m_Point[0].x = 3.3f;
            m_Normal.m_Point[0].y = 4.31f;
            m_AirTrick.m_Point[0].y = 1.7f;
            m_Normal.m_Point[0].z = 13.999998f;
            m_AirTrick.m_Point[0].z = 10.8f;
            m_Normal.m_Point[1].x = 5.79f;
            m_AirTrick.m_Point[1].x = 3.3f;
            m_Normal.m_Point[1].y = 4.31f;
            m_AirTrick.m_Point[1].y = 1.7f;
            m_Normal.m_Point[1].z = 13.999998f;
            m_AirTrick.m_Point[1].z = 10.8f;
            m_Normal.m_Point[2].x = 6.38f;
            m_AirTrick.m_Point[2].x = 7.0f;
            m_Normal.m_Point[2].y = 4.82f;
            m_AirTrick.m_Point[2].y = 4.6f;
            m_Normal.m_Point[2].z = 11.0f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 6.38f;
            m_AirTrick.m_Point[3].x = 7.3f;
            m_Normal.m_Point[3].y = 4.82f;
            m_AirTrick.m_Point[3].y = 4.6f;
            m_Normal.m_Point[3].z = 11.0f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.85f;
            m_AirTrick.m_fScaleX = 0.7f;
        }
    } else {
        bit = (flags >> 4) & 1;
        if (bit == 0 && !(flags & 0x8)) {
            m_Unk250 = 35.0f;
            m_fFrontRate = 0.854f;
            m_fLookRate = 0.937f;
            m_Unk25C = 5.0f;
            m_fZoomSpeed = 1.7f;
            m_fAirTrickRateY = 0.13f;
            m_fMarginX = 0.0f;
            m_fLimitX = 3.5f;
            m_fRateX = 0.5f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 2.6f;
            m_ServeWait.m_fTarY = 1.9f;
            m_ServeWait.m_fLen = 7.499999f;
            m_ServeWait.m_fSlantB = 3.5f;
            m_ServeWait.m_fSlantA = 0.5f;
            m_ServeWait.m_fScaleX = 0.95f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.4f;
            m_fServeTarRateYBig = 1.22f;
            m_ServeThrow.m_fPosY = 4.999999f;
            m_ServeThrow.m_fTarY = 3.299999f;
            m_ServeThrow.m_fLen = 8.049998f;
            m_ServeThrow.m_fSlantB = 3.5f;
            m_ServeThrow.m_fSlantA = 0.5f;
            m_ServeThrow.m_fScaleX = 0.95f;
            m_Spike.m_fPosY = 5.999997f;
            m_Spike.m_fTarY = 3.999999f;
            m_Spike.m_fLen = 12.800015f;
            m_Spike.m_fSlantB = 2.0f;
            m_Spike.m_fSlantA = 0.5f;
            m_Spike.m_fScaleX = 0.95f;
            m_SuperSpike.m_fPosY = 6.099998f;
            m_SuperSpike.m_fTarY = 3.799998f;
            m_SuperSpike.m_fLen = 12.800015f;
            m_SuperSpike.m_fSlantB = 2.000001f;
            m_SuperSpike.m_fSlantA = 0.5f;
            m_SuperSpike.m_fScaleX = 0.95f;
            m_Block.m_fPosY = 2.999999f;
            m_Block.m_fTarY = 1.5f;
            m_Block.m_fLen = 7.999998f;
            m_Block.m_fSlantB = 3.5f;
            m_Block.m_fSlantA = 0.5f;
            m_Block.m_fScaleX = 0.95f;
            m_Endure.m_fPosY = 2.3f;
            m_Endure.m_fTarY = 0.7f;
            m_Endure.m_fLen = 6.8f;
            m_Endure.m_fSlantB = 3.5f;
            m_Endure.m_fSlantA = 0.5f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 6.79f;
            m_AirTrick.m_Point[0].x = 2.3f;
            m_Normal.m_Point[0].y = 5.11f;
            m_AirTrick.m_Point[0].y = 0.7f;
            m_Normal.m_Point[0].z = 12.8f;
            m_AirTrick.m_Point[0].z = 6.8f;
            m_Normal.m_Point[1].x = 6.79f;
            m_AirTrick.m_Point[1].x = 2.3f;
            m_Normal.m_Point[1].y = 5.11f;
            m_AirTrick.m_Point[1].y = 0.7f;
            m_Normal.m_Point[1].z = 12.8f;
            m_AirTrick.m_Point[1].z = 6.8f;
            m_Normal.m_Point[2].x = 6.68f;
            m_AirTrick.m_Point[2].x = 2.3f;
            m_Normal.m_Point[2].y = 5.12f;
            m_AirTrick.m_Point[2].y = 0.7f;
            m_Normal.m_Point[2].z = 11.8f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 6.68f;
            m_AirTrick.m_Point[3].x = 2.3f;
            m_Normal.m_Point[3].y = 5.12f;
            m_AirTrick.m_Point[3].y = 0.7f;
            m_Normal.m_Point[3].z = 11.8f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.9f;
            m_AirTrick.m_fScaleX = 0.8f;
        } else if (bit == 0 && (flags & 0x8)) {
            m_Unk250 = 40.0f;
            m_fFrontRate = 0.952f;
            m_fLookRate = 0.937f;
            m_Unk25C = 5.0f;
            m_fZoomSpeed = 1.7f;
            m_fAirTrickRateY = 0.13f;
            m_fMarginX = 0.0f;
            m_fLimitX = 5.0f;
            m_fRateX = 0.5f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 2.1f;
            m_ServeWait.m_fTarY = 1.7f;
            m_ServeWait.m_fLen = 6.2f;
            m_ServeWait.m_fSlantB = 3.5f;
            m_ServeWait.m_fSlantA = 0.5f;
            m_ServeWait.m_fScaleX = 0.8f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.4f;
            m_fServeTarRateYBig = 1.15f;
            m_ServeThrow.m_fPosY = 4.2f;
            m_ServeThrow.m_fTarY = 2.6f;
            m_ServeThrow.m_fLen = 6.45f;
            m_ServeThrow.m_fSlantB = 3.5f;
            m_ServeThrow.m_fSlantA = 0.5f;
            m_ServeThrow.m_fScaleX = 0.8f;
            m_Spike.m_fPosY = 2.899999f;
            m_Spike.m_fTarY = 1.0f;
            m_Spike.m_fLen = 9.800006f;
            m_Spike.m_fSlantB = 1.5f;
            m_Spike.m_fSlantA = 0.5f;
            m_Spike.m_fScaleX = 0.8f;
            m_SuperSpike.m_fPosY = 3.8f;
            m_SuperSpike.m_fTarY = 1.3f;
            m_SuperSpike.m_fLen = 9.800006f;
            m_SuperSpike.m_fSlantB = 1.5f;
            m_SuperSpike.m_fSlantA = 0.5f;
            m_SuperSpike.m_fScaleX = 0.8f;
            m_Block.m_fPosY = 2.2f;
            m_Block.m_fTarY = 0.55f;
            m_Block.m_fLen = 3.5f;
            m_Block.m_fSlantB = 3.5f;
            m_Block.m_fSlantA = 0.5f;
            m_Block.m_fScaleX = 0.8f;
            m_Endure.m_fPosY = 2.3f;
            m_Endure.m_fTarY = 0.7f;
            m_Endure.m_fLen = 6.8f;
            m_Endure.m_fSlantB = 3.5f;
            m_Endure.m_fSlantA = 0.5f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 4.69f;
            m_AirTrick.m_Point[0].x = 2.3f;
            m_Normal.m_Point[0].y = 3.21f;
            m_AirTrick.m_Point[0].y = 0.7f;
            m_Normal.m_Point[0].z = 9.8f;
            m_AirTrick.m_Point[0].z = 6.8f;
            m_Normal.m_Point[1].x = 4.69f;
            m_AirTrick.m_Point[1].x = 2.3f;
            m_Normal.m_Point[1].y = 3.21f;
            m_AirTrick.m_Point[1].y = 0.7f;
            m_Normal.m_Point[1].z = 9.8f;
            m_AirTrick.m_Point[1].z = 6.8f;
            m_Normal.m_Point[2].x = 5.88f;
            m_AirTrick.m_Point[2].x = 2.3f;
            m_Normal.m_Point[2].y = 4.02f;
            m_AirTrick.m_Point[2].y = 0.7f;
            m_Normal.m_Point[2].z = 9.8f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 5.88f;
            m_AirTrick.m_Point[3].x = 2.3f;
            m_Normal.m_Point[3].y = 4.02f;
            m_AirTrick.m_Point[3].y = 0.7f;
            m_Normal.m_Point[3].z = 9.8f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.52f;
            m_AirTrick.m_fScaleX = 0.8f;
        } else if (bit != 0 && !(flags & 0x8)) {
            m_Unk250 = 25.0f;
            m_fFrontRate = 0.854f;
            m_fLookRate = 0.97f;
            m_Unk25C = 3.0f;
            m_fZoomSpeed = 1.9f;
            m_fAirTrickRateY = 0.0f;
            m_fMarginX = 0.0f;
            m_fLimitX = 2.5f;
            m_fRateX = 0.4f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 1.85f;
            m_ServeWait.m_fTarY = 1.45f;
            m_ServeWait.m_fLen = 7.5f;
            m_ServeWait.m_fSlantB = 3.874f;
            m_ServeWait.m_fSlantA = 0.6f;
            m_ServeWait.m_fScaleX = 0.8f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.7f;
            m_fServeTarRateYBig = 1.45f;
            m_ServeThrow.m_fPosY = 5.099999f;
            m_ServeThrow.m_fTarY = 3.799999f;
            m_ServeThrow.m_fLen = 10.0f;
            m_ServeThrow.m_fSlantB = 3.874f;
            m_ServeThrow.m_fSlantA = 0.6f;
            m_ServeThrow.m_fScaleX = 0.8f;
            m_Spike.m_fPosY = 3.299999f;
            m_Spike.m_fTarY = 2.0f;
            m_Spike.m_fLen = 13.000017f;
            m_Spike.m_fSlantB = 0.0f;
            m_Spike.m_fSlantA = 0.0f;
            m_Spike.m_fScaleX = 0.8f;
            m_SuperSpike.m_fPosY = 4.0f;
            m_SuperSpike.m_fTarY = 2.5f;
            m_SuperSpike.m_fLen = 13.000017f;
            m_SuperSpike.m_fSlantB = 0.0f;
            m_SuperSpike.m_fSlantA = 0.0f;
            m_SuperSpike.m_fScaleX = 0.8f;
            m_Block.m_fPosY = 2.799999f;
            m_Block.m_fTarY = 1.9f;
            m_Block.m_fLen = 9.000002f;
            m_Block.m_fSlantB = 3.874f;
            m_Block.m_fSlantA = 0.6f;
            m_Block.m_fScaleX = 0.8f;
            m_Endure.m_fPosY = 2.2f;
            m_Endure.m_fTarY = 0.9f;
            m_Endure.m_fLen = 6.200001f;
            m_Endure.m_fSlantB = 1.5f;
            m_Endure.m_fSlantA = 0.2f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 5.29f;
            m_AirTrick.m_Point[0].x = 4.6f;
            m_Normal.m_Point[0].y = 4.309999f;
            m_AirTrick.m_Point[0].y = 2.6f;
            m_Normal.m_Point[0].z = 13.200001f;
            m_AirTrick.m_Point[0].z = 9.8f;
            m_Normal.m_Point[1].x = 5.29f;
            m_AirTrick.m_Point[1].x = 4.6f;
            m_Normal.m_Point[1].y = 4.31f;
            m_AirTrick.m_Point[1].y = 2.6f;
            m_Normal.m_Point[1].z = 13.200001f;
            m_AirTrick.m_Point[1].z = 9.8f;
            m_Normal.m_Point[2].x = 4.48f;
            m_AirTrick.m_Point[2].x = 5.3f;
            m_Normal.m_Point[2].y = 3.62f;
            m_AirTrick.m_Point[2].y = 3.700001f;
            m_Normal.m_Point[2].z = 13.0f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 4.480001f;
            m_AirTrick.m_Point[3].x = 5.200001f;
            m_Normal.m_Point[3].y = 3.62f;
            m_AirTrick.m_Point[3].y = 3.799999f;
            m_Normal.m_Point[3].z = 13.0f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.8f;
            m_AirTrick.m_fScaleX = 0.8f;
        } else {
            m_Unk250 = 40.0f;
            m_fFrontRate = 0.919f;
            m_fLookRate = 0.97f;
            m_Unk25C = 2.5f;
            m_fZoomSpeed = 2.2f;
            m_fAirTrickRateY = 0.13f;
            m_fMarginX = 0.0f;
            m_fLimitX = 1.5f;
            m_fRateX = 0.3f;
            m_fMarginZ = 0.0f;
            m_fLimitZ = 30.0f;
            m_fRateZ = 1.0f;
            m_ServeWait.m_fPosY = 2.6f;
            m_ServeWait.m_fTarY = 1.8f;
            m_ServeWait.m_fLen = 6.999998f;
            m_ServeWait.m_fSlantB = 3.874f;
            m_ServeWait.m_fSlantA = 0.6f;
            m_ServeWait.m_fScaleX = 0.8f;
            m_fServeRateY = 1.3f;
            m_fServeTarRateY = 1.0f;
            m_fServeRateYBig = 1.4f;
            m_fServeTarRateYBig = 1.15f;
            m_ServeThrow.m_fPosY = 6.199998f;
            m_ServeThrow.m_fTarY = 4.199998f;
            m_ServeThrow.m_fLen = 9.000001f;
            m_ServeThrow.m_fSlantB = 3.874f;
            m_ServeThrow.m_fSlantA = 0.6f;
            m_ServeThrow.m_fScaleX = 0.8f;
            m_Spike.m_fPosY = 2.5f;
            m_Spike.m_fTarY = 1.0f;
            m_Spike.m_fLen = 11.000011f;
            m_Spike.m_fSlantB = 0.0f;
            m_Spike.m_fSlantA = 0.0f;
            m_Spike.m_fScaleX = 0.8f;
            m_SuperSpike.m_fPosY = 3.2f;
            m_SuperSpike.m_fTarY = 1.2f;
            m_SuperSpike.m_fLen = 11.000011f;
            m_SuperSpike.m_fSlantB = 0.0f;
            m_SuperSpike.m_fSlantA = 0.0f;
            m_SuperSpike.m_fScaleX = 0.8f;
            m_Block.m_fPosY = 4.299998f;
            m_Block.m_fTarY = 2.6f;
            m_Block.m_fLen = 8.999996f;
            m_Block.m_fSlantB = 3.874f;
            m_Block.m_fSlantA = 0.6f;
            m_Block.m_fScaleX = 0.8f;
            m_Endure.m_fPosY = 2.2f;
            m_Endure.m_fTarY = 0.9f;
            m_Endure.m_fLen = 6.200001f;
            m_Endure.m_fSlantB = 1.5f;
            m_Endure.m_fSlantA = 0.2f;
            m_Endure.m_fScaleX = 0.8f;
            m_Normal.m_Point[0].x = 5.9275f;
            m_AirTrick.m_Point[0].x = 3.3f;
            m_Normal.m_Point[0].y = 4.31f;
            m_AirTrick.m_Point[0].y = 1.7f;
            m_Normal.m_Point[0].z = 13.999998f;
            m_AirTrick.m_Point[0].z = 10.8f;
            m_Normal.m_Point[1].x = 5.79f;
            m_AirTrick.m_Point[1].x = 3.3f;
            m_Normal.m_Point[1].y = 4.31f;
            m_AirTrick.m_Point[1].y = 1.7f;
            m_Normal.m_Point[1].z = 13.999998f;
            m_AirTrick.m_Point[1].z = 10.8f;
            m_Normal.m_Point[2].x = 6.38f;
            m_AirTrick.m_Point[2].x = 7.0f;
            m_Normal.m_Point[2].y = 4.82f;
            m_AirTrick.m_Point[2].y = 4.6f;
            m_Normal.m_Point[2].z = 11.0f;
            m_AirTrick.m_Point[2].z = 6.8f;
            m_Normal.m_Point[3].x = 6.38f;
            m_AirTrick.m_Point[3].x = 7.3f;
            m_Normal.m_Point[3].y = 4.82f;
            m_AirTrick.m_Point[3].y = 4.6f;
            m_Normal.m_Point[3].z = 11.0f;
            m_AirTrick.m_Point[3].z = 6.8f;
            m_Normal.m_fSlantB = 3.5f;
            m_AirTrick.m_fSlantB = 3.5f;
            m_Normal.m_fSlantA = 0.5f;
            m_AirTrick.m_fSlantA = 0.5f;
            m_Normal.m_fScaleX = 0.85f;
            m_AirTrick.m_fScaleX = 0.7f;
        }
    }
}

msw::CCameraVolleyGame::~CCameraVolleyGame() {}
