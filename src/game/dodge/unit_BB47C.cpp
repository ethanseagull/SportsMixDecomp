// msw::CChrCoreDodge's air trick state: the team special shot of the
// dodgeball game (the "arts shot"). The character jumps, a target marker
// appears on the other court and can be moved with the stick, he throws at
// it and falls back to his court. The characters override the motions and
// effects (SubStateArtsShotPlay*), and some the whole steps.
#include "game/msw/CChrCoreDodge.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

// 60 degrees in radians. Not used here: nearly every DodgeF file has this
// object and the static initialiser that fills it (see CChrCoreDodgeLuigi.cpp).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

// x kept within [min, max]. A macro: the original computes each bound only
// on the branch that needs it.
#define CLAMP(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // The state of the air trick.
    const long STATE_AIR_TRICK = 0x27;

    // The steps of the state (STATE_WORK_AIR_TRICK_CMN::m_nStep).
    enum {
        STEP_INIT,
        STEP_START_TRICK,
        STEP_TRICK,
        STEP_READY,
        STEP_SHOT,
        STEP_HITBACK,
        STEP_LAND
    };

    inline float WrapRad(float rad)
    {
        while (rad > PI2) {
            rad -= PI2;
        }
        while (rad < 0.0f) {
            rad += PI2;
        }
        return rad;
    }


    inline float Min(float a, float b)
    {
        return a > b ? b : a;
    }

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    // The square of the distance between two points in the XZ plane.
    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }

    // A court's edges, as CStageMan::GetTeamCourtRect gives them.
    struct CourtRect {
        float fLeft;
        float fRight;
        float fFront;
        float fBack;
    };

    template <class T>
    inline void Swap(T &a, T &b)
    {
        T t = a;
        a = b;
        b = t;
    }
}

void msw::CChrCoreDodge::CtorStateAirTrick()
{
    CtorStateArtsShot_();
}

void msw::CChrCoreDodge::LoopStateAirTrick()
{
    LoopStateArtsShot_();
}

void msw::CChrCoreDodge::DtorStateAirTrick()
{
    Eff_StopGroup(4);
    m_nStatusFlag &= ~0x800;
    ArtsOptDestroy();
    m_nStatusFlag &= ~0x40000000;
    m_nStatusFlag &= ~0x1000;
    m_nFlagF4 &= ~0x80000;
    Util_DetachEffect((ATTACH_EFFECT_TYPE)6);
    StopEffectAll();
    StopSeSlot(3);
    ArtsFadeIn();
}

// The marker shows from the start of the trick (SubStateArtsShotStartTargetMarker_)
// until the shot.
int msw::CChrCoreDodge::IsDodgeArtsTargetMarkerActive() const
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    return (pWork->m_nFlag04 & 2) != 0;
}

// Character 15 has a marker of its own kind.
long msw::CChrCoreDodge::GetDodgeArtsTargetMarkerType() const
{
    return m_nChrId == 15 ? 3 : 0;
}

long msw::CChrCoreDodge::GetDodgeArtsTargetMarkerPos(bcs::FVEC3 &rPos, long)
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    if (pWork->m_nFlag04 & 2) {
        rPos = pWork->m_TargetPos;
        return 1;
    }
    return 0;
}

long msw::CChrCoreDodge::GetDodgeArtsTargetMarkerNum() const
{
    return 1;
}

// The middle of the other team's court.
void msw::CChrCoreDodge::GetDodgeArtsTargetMarkerDefaultPos(bcs::FVEC3 &rPos, long)
{
    CourtRect rect;
    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &rect.fLeft, &rect.fRight, &rect.fFront,
                                               &rect.fBack);
    bcs::FVEC3 pos;
    pos.x = 0.5f * (rect.fLeft + rect.fRight);
    pos.y = 0.0f;
    pos.z = 0.5f * (rect.fFront + rect.fBack);
    rPos = pos;
}

long msw::CChrCoreDodge::GetDodgeArtsTargetMarkerMoved(bcs::FVEC3 &rMove)
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    if (pWork->m_nFlag04 & 2) {
        rMove = pWork->m_TargetMove;
        return 1;
    }
    return 0;
}

// The stick moves the marker towards fRot this frame.
int msw::CChrCoreDodge::MoveDodgeArtsTargetMarkerPos(float fRot)
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    if (pWork->m_nFlag04 & 2) {
        pWork->m_fMarkerRot = WrapRad(fRot);
        return 1;
    }
    return 0;
}

int msw::CChrCoreDodge::CanbeMoveDodgeArtsTargetMarkerPos()
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    return (pWork->m_nFlag04 & 2) != 0;
}

void msw::CChrCoreDodge::SetDodgeArtsTargetMarkerDisable()
{
    if (GetState() != STATE_AIR_TRICK) {
        return;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    pWork->m_nFlag04 &= ~2;
}

// The marker starts in the middle of the other court.
void msw::CChrCoreDodge::SubStateArtsShotStartTargetMarker_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (!(pWork->m_nFlag04 & 2)) {
        GetDodgeArtsTargetMarkerDefaultPos(pWork->m_TargetPos, 0);
        pWork->m_nFlag04 |= 2;
    }
}

// Moves the marker by the stick (m_fMarkerRot), at the character's marker
// speeds, and keeps it on the other court.
void msw::CChrCoreDodge::SubStateArtsShotMoveTargetMarker_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (pWork) {
        pWork->m_TargetMove.x = 0.0f;
        pWork->m_TargetMove.y = 0.0f;
        pWork->m_TargetMove.z = 0.0f;
        if (pWork->m_nFlag04 & 2) {
            if (pWork->m_fMarkerRot >= 0.0f) {
                bcs::FVEC3 move;
                float fRot = WrapRad(pWork->m_fMarkerRot);
                bcs::BCS_VecOneZ_RotY(move, fRot);
                const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = GetCcParam_();
                float fSpeedX = (1.0f / 60.0f) * pParam->m_Unk5AC;
                float fSpeedZ = (1.0f / 60.0f) * pParam->m_Unk5B0;
                move.x *= fSpeedX * m_fFrameStep;
                move.z *= fSpeedZ * m_fFrameStep;
                pWork->m_TargetMove = move;
                pWork->m_TargetPos.x += move.x;
                pWork->m_TargetPos.z += move.z;
            }
            SetDodgeArtsTargetMarkerPosClip(pWork->m_TargetPos);
            pWork->m_fMarkerRot = -1.0f;
        }
    }
}

// rPos kept inside the other team's court.
void msw::CChrCoreDodge::SetDodgeArtsTargetMarkerPosClip(bcs::FVEC3 &rPos)
{
    bcs::FVEC3 pos(rPos);
    CourtRect rect;
    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &rect.fLeft, &rect.fRight, &rect.fFront,
                                               &rect.fBack);
    if (rect.fLeft > rect.fRight) {
        Swap(rect.fLeft, rect.fRight);
    }
    if (rect.fFront > rect.fBack) {
        Swap(rect.fFront, rect.fBack);
    }
    pos.x = CLAMP(pos.x, rect.fLeft, rect.fRight);
    pos.z = CLAMP(pos.z, rect.fFront, rect.fBack);
    rPos = pos;
}

void msw::CChrCoreDodge::CtorStateArtsShot_()
{
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();

    SetCollBodyPushType_((BODY_COLL_SIZE_SET_TYPE)5, 8.0f);
    SetCollBodyPushRank_(3);
    m_UnkEC &= ~0x10;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    SetCondition(0, 0.0f, 0);
    m_nStatusFlag |= 0x40000000;
    SetMoveMotType(4);

    pWork->m_nStep = STEP_INIT;
    pWork->m_nUnkA4 = 0;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_nArtsLevel = 0;
    pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
    pWork->m_TargetMove.x = 0.0f;
    pWork->m_TargetMove.y = 0.0f;
    pWork->m_TargetMove.z = 0.0f;

    CGame::ClearTeamArtsGauge(m_nTeamNo);
    CGame::TrgArtsEvent(m_nPlayerNo);
    CGame::SendMessage(0xD, m_nPlayerNo, 0, 0);
    DriverSendMessage((CChrDrv::MSG_ID)0x1E, 0, 0, 0, 0);

    pWork->m_nTrickStep = 0;
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = 40.0f;
    pWork->m_fTrickMotFrame = 40.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fJumpUnk = GetCcParam_()->m_Unk578 + GetCcParam_()->m_Unk57C;
    pWork->m_fUnkD8 = 0.0f;
}

// The trick turns the character to face from where he lands towards the
// other team's goal.
float msw::CChrCoreDodge::SubStateArtsShotGetTrickStartRot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    bcs::FVEC3 dir(GetTargetGoalPos());
    dir.x -= pWork->m_MovePos.x;
    dir.z -= pWork->m_MovePos.z;
    return WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z));
}

void msw::CChrCoreDodge::LoopStateArtsShot_()
{
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    int bEnd = 0;

    SubStateArtsShotMoveTargetMarker_(pWork);
    switch (pWork->m_nStep) {
        case STEP_INIT:
            pWork->m_nStep = STEP_START_TRICK;
            break;
        case STEP_START_TRICK:
            Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
            PlayVoice(0x62, 0);
            SubStateArtsShotStartTrick_(pWork);
            pWork->m_nStep = STEP_TRICK;
            break;
        case STEP_TRICK:
            if (SubStateArtsShotIsReadyArtsShot_(pWork)) {
                SubStateArtsShotReadyArtsShot_(pWork);
                pWork->m_nStep = STEP_READY;
            }
            break;
        case STEP_READY:
            if (SubStateArtsShotIsStartArtsShot_(pWork)) {
                SubStateArtsShotStartArtsShot_(pWork);
                pWork->m_nStep = STEP_SHOT;
            }
            break;
        case STEP_SHOT:
            if (SubStateArtsShotIsEndArtsShot_(pWork)) {
                SubStateArtsShotStartArtsHitback_(pWork);
                pWork->m_nStep = STEP_HITBACK;
            }
            break;
        case STEP_HITBACK:
            if (SubStateArtsShotIsEndArtsShotAfter_(pWork)) {
                SubStateArtsShotDoLandStart_(pWork);
            }
            break;
        case STEP_LAND:
            if (!IsAnimEnd(3)) {
                break;
            }
            // the end of the landing motion ends the state
        default:
            bEnd = 1;
            break;
    }
    if (bEnd) {
        FsmqDoChain();
    }
}

void msw::CChrCoreDodge::SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_nTrickStep = 0;
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = 60.0f;
    pWork->m_fTrickMotFrame = 60.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fJumpUnk = GetCcParam_()->m_Unk578 + GetCcParam_()->m_Unk57C;

    SubStateArtsShotSetTrickFrameParam_(pWork, 0);
    SubStateArtsShotPlayTrickMotion_(pWork);
    SubStateArtsShotSetTrickJumpParam_(pWork, 0);
    SubStateArtsShotSetTrickMoveParam_(pWork, 0);
    pWork->m_fRotY = SubStateArtsShotGetTrickStartRot_(pWork);
    SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, SubStateArtsShotGetTrickRotFrame_(pWork));
    SubStateArtsShotStartTargetMarker_(pWork);
}

// The trick lasts the frames the character's parameters give it; its motion
// is played faster or slower to fit them.
void msw::CChrCoreDodge::SubStateArtsShotSetTrickFrameParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long nTrick)
{
    float fMotFrame = SubStateArtsShotGetTrickMotFrame_(pWork, nTrick);
    fMotFrame = Max(1.0f, fMotFrame - GetCcParam_()->m_Unk574);
    pWork->m_fTrickMotFrame = fMotFrame;
    float fFrame = SubStateArtsShotGetTrickDoFrame_(pWork, nTrick);
    if (fFrame <= 0.0f) {
        fFrame = fMotFrame;
        pWork->m_fAnimSpeed = 1.0f;
    } else {
        pWork->m_fAnimSpeed = fMotFrame / fFrame;
    }
    pWork->m_fTrickFrame = fFrame;
    pWork->m_fTrickCount = 0.0f;
}

// A turn to fRot over fFrame frames, the short way round.
void msw::CChrCoreDodge::SubStateArtsShotCalcRotParam_(ROT_LERP1 &rLerp, float fRot, float fFrame)
{
    rLerp.m_afUnk[0] = WrapRad(fRot);
    rLerp.m_afUnk[2] = fFrame;
    rLerp.m_afUnk[2] = Max(rLerp.m_afUnk[2], 1.0f);
    float fDiff = WrapRad(rLerp.m_afUnk[0] - GetRotY());
    if (fDiff > PI) {
        fDiff -= PI2;
    }
    rLerp.m_afUnk[3] = 0.0f;
    rLerp.m_afUnk[1] = fDiff / rLerp.m_afUnk[2];
}

void msw::CChrCoreDodge::SubStateArtsShotClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam)
{
    rParam.m_nType = 2;
    rParam.m_fFrame = 0.0f;
    rParam.m_fFrameMax = 0.0f;
    rParam.m_fDist = 0.0f;
}

// The jump, with the turn and the move of the trick while it rises.
int msw::CChrCoreDodge::SubStateArtsShotDoJumpMove_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int nResult = Util_DoJumpMoveEX(0);
    if (nResult == 1) {
        RotateYParam_(pWork->m_RotLerp);
        if (pWork->m_fUnkD4 < pWork->m_MoveParam.m_fFrameMax) {
            pWork->m_fUnkD4 += m_fFrameStep * m_fTimeScale;
            pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_MovePos);
            Util_DoDirMove(pWork->m_MoveParam, pWork->m_fUnkD4, pWork->m_DirMove, 0);
        }
    }
    return nResult;
}

float msw::CChrCoreDodge::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    return AnimGetResFrameNum(0x7A);
}

float msw::CChrCoreDodge::SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK_CMN *, long)
{
    return GetCcParam_()->m_Unk570;
}

void msw::CChrCoreDodge::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    AnimPlay(3, 0x7A, 0, GetCcParam_()->m_Unk598, 0.0f);
    AnimSetSpeed(3, pWork->m_fAnimSpeed);
    ArtsOptAnimSetSpeed(pWork->m_fAnimSpeed);
    SE_PlayArtsJumpSE(0);
}

float msw::CChrCoreDodge::SubStateArtsShotGetTrickRotFrame_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    return Min(16.0f, pWork->m_fTrickFrame);
}

// The character's place kept inside his own court: one unit from its sides,
// and no further than his parameter from its back line.
void msw::CChrCoreDodge::SubStateArtsShotCalcSetbackPos_(bcs::FVEC3 &rPos)
{
    rPos = GetPos();
    float fDepth = GetCcParam_()->m_Unk568;
    CourtRect rect;
    CStageMan::GetInstance()->GetTeamCourtRect(GetGuardCourtIdx(), &rect.fLeft, &rect.fRight, &rect.fFront,
                                               &rect.fBack);
    if (rect.fLeft > rect.fRight) {
        // (sic) the original writes the left edge back to itself here
        rect.fLeft = rect.fLeft;
    }
    rPos.x = CLAMP(rPos.x, 1.0f + rect.fLeft, rect.fRight - 1.0f);
    if (GetGuardCourtIdx() != 0) {
        rPos.z = CLAMP(rPos.z, rect.fBack - fDepth, rect.fBack);
    } else {
        rPos.z = CLAMP(rPos.z, rect.fFront, rect.fFront + fDepth);
    }
}

void msw::CChrCoreDodge::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long)
{
    float fTop = GetCcParam_()->m_Unk560;
    float fFrame = GetCcParam_()->m_Unk564;
    float fY = GetPosY();
    fTop = Max(fTop, fY);
    fFrame = Max(1.0f, pWork->m_fTrickFrame - fFrame);
    Util_SetJumpParam(fFrame, fY, fTop, fTop);
    float fUnk224 = pWork->m_fJumpUnk + (pWork->m_fTrickFrame - fFrame);
    m_fJumpUnk228 = 0.0f;
    m_fJumpUnk224 = fUnk224;
}

// The trick moves the character back to a place inside his court.
void msw::CChrCoreDodge::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK_CMN *pWork, long)
{
    SubStateArtsShotCalcSetbackPos_(pWork->m_MovePos);
    SubStateArtsShotClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnkD4 = 0.0f;

    float fDistSq = DistSqXZ(pWork->m_MovePos, GetPos());
    if (fDistSq > 0.0f) {
        float fFrame = Max(1.0f, pWork->m_fTrickFrame - GetCcParam_()->m_Unk56C);
        pWork->m_MoveParam.m_nType = 0xB;
        pWork->m_MoveParam.m_fDist = nw4r::math::FSqrt(fDistSq);
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = fFrame;
    }
}

// Waits for the trick to end; the loop motion follows the trick motion.
int msw::CChrCoreDodge::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
    SubStateArtsShotDoJumpMove_(pWork);
    if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
        return 1;
    }
    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return 0;
}

// The shot starts when the driver asks for it (it is told the frames left)
// or the time runs out.
int msw::CChrCoreDodge::SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    int bStart = 0;
    if (pWork->m_fJumpUnk > 0.0f) {
        pWork->m_fJumpUnk -= m_fFrameStep;
    }
    long nFrame = GetCcParam_()->m_Unk578 + GetCcParam_()->m_Unk57C;
    if (DriverSendMessage((CChrDrv::MSG_ID)0x21, nFrame, nFrame - pWork->m_fJumpUnk, 0, 0) != 0 ||
        pWork->m_fJumpUnk <= 0.0f) {
        bStart = 1;
    }
    SubStateArtsShotDoJumpMove_(pWork);
    if (!bStart && pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return bStart;
}

void msw::CChrCoreDodge::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK_CMN *)
{
    AnimPlay(3, 0x85, 1, GetCcParam_()->m_Unk59C, 0.0f);
}

void msw::CChrCoreDodge::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK_CMN *, long)
{
}

// The character hangs in the air at the top of the jump, with a flash on
// his hand.
void msw::CChrCoreDodge::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    bcs::FVEC3 pos;
    GetVNodePos(1, pos);
    CEffectCtrl::SimpleEffect(0x4E7, pos, 3, 0, 0);
    PlaySePos(0xC07, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
    SE_PlayArtsLoopSE();

    float fUnk224 = GetCcParam_()->m_Unk578;
    float fFrame = GetCcParam_()->m_Unk57C;
    float fY = GetPosY();
    Util_SetJumpParam(fFrame, fY, fY - GetCcParam_()->m_Unk580, fY);
    m_fJumpUnk224 = fUnk224;
    m_fJumpUnk228 = 0.0f;
}

void msw::CChrCoreDodge::SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    SE_Stop(8, 8.0f);
    SubStateArtsShotPlayArtsShotMotion_(pWork);
    SubStateArtsShotDoGetBallShotTrg_(pWork);
}

void msw::CChrCoreDodge::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    float fBlend = GetCcParam_()->m_Unk5A0;
    if (pWork->m_nArtsLevel < 1) {
        fBlend = GetCcParam_()->m_Unk5A4;
    }
    AnimPlay(3, 0x7F, 0, fBlend, 0.0f);
}

// The ball leaves at trigger 2 of the shot motion, or at frame 16 without one.
void msw::CChrCoreDodge::SubStateArtsShotDoGetBallShotTrg_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    float fFrame = AnimSearchTrgFrame(AnimGetMotID(3, 0), 2, -1.0f);
    if (fFrame < 0.0f) {
        fFrame = 16.0f;
    }
    pWork->m_fShotFrame = fFrame;
}

int msw::CChrCoreDodge::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
    if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
        if (!(pWork->m_nFlag04 & 4)) {
            Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
        }
        SubStateArtsShotShootBall_(pWork);
        pWork->m_nArtsLevel = 0;
        return 1;
    }
    return 0;
}

// Throws the ball at the marker; the marker goes unless m_fUnkD8 is set.
void msw::CChrCoreDodge::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    CBall *pBall = GetAttachBall();
    if (pBall) {
        bcs::FVEC3 target(pWork->m_TargetPos);
        Util_ShootBall(target, 9, 1);
        CGame::SendMessage(0x44, m_nPlayerNo, 0, 0);
        PlayVoice(0x63, 0);
        SE_PlayArtsShootSE(pBall->GetPos());
        pWork->m_nFlag04 |= 1;
        if (pWork->m_fUnkD8 <= 0.0f) {
            pWork->m_nFlag04 &= ~2;
        }
    }
}

// The throw pushes the character back: a jump backwards, away from the
// direction he threw in.
void msw::CChrCoreDodge::SubStateArtsShotStartArtsHitback_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = GetCcParam_()->m_Unk584;
    pWork->m_MoveParam.m_fDist = GetCcParam_()->m_Unk588;
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnkD4 = 0.0f;
    SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, GetCcParam_()->m_Unk584);

    bcs::BCS_VecZ_RotY(pWork->m_MovePos, -2.0f * GetCcParam_()->m_Unk588, pWork->m_fRotY);
    const bcs::FVEC3 &rPos = GetPos();
    pWork->m_MovePos.x += rPos.x;
    pWork->m_MovePos.z += rPos.z;

    Util_SetJumpParam(GetCcParam_()->m_Unk58C, GetCcParam_()->m_Unk590, GetCcParam_()->m_Unk594);
}

int msw::CChrCoreDodge::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    return SubStateArtsShotDoJumpMove_(pWork) >= 2;
}

void msw::CChrCoreDodge::SubStateArtsShotDoLandStart_(STATE_WORK_AIR_TRICK_CMN *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SubStateArtsShotPlayLandMotion_(pWork);
    SubStateArtsShotPlayLandEffect_(pWork);
    pWork->m_nStep = STEP_LAND;
}

void msw::CChrCoreDodge::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK_CMN *)
{
    AnimPlay(3, 0x84, 0, GetCcParam_()->m_Unk5A8, 0.0f);
}

void msw::CChrCoreDodge::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK_CMN *)
{
    SE_PlayArtsLandSE();
    Util_SetEffect((SET_EFFECT_TYPE)0);
}

int msw::CChrCoreDodge::IsAirTrickSwingEnd() const
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    if (pWork->m_nFlag04 & 1) {
        return 1;
    }
    return !(m_nStatusFlag & 1);
}

int msw::CChrCoreDodge::IsAirTrickFall() const
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    STATE_WORK_AIR_TRICK_CMN *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK_CMN>();
    long nStep = pWork->m_nStep;
    if (nStep == STEP_HITBACK && m_fJumpVelY < 0.0f && m_fJumpUnk224 <= 0.0f) {
        return 1;
    }
    return nStep >= STEP_LAND;
}
