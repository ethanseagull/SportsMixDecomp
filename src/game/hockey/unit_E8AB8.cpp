// msw::CChrCoreHockey's special shot: the arts event that starts it, the
// arts state (CtorStateAirTrick_ ... DtorStateAirTrick_, driven by
// ArtsProc) and the air trick state (state 0x27), the "arts shot". The
// character jumps with a trick motion while he moves back to a point away
// from the goal, hangs in the air until the driver asks for the shot (or
// the time runs out), shoots the puck and is pushed back by the shot. The
// characters override the motions and effects (SubStateArtsShotPlay*) and
// some of the steps.
#include "game/msw/CChrCoreHockey.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CC_PARAM_HOCKEY.hpp"
#include "game/msw/CChrManHockey.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/triangular.h"

// The motions of the trick, played one after the other; the parameters say
// how many (SubStateArtsShotGetTrickMotFrame_).
static const long s_aTrickMotId[] = {0x9C, 0x9D, 0x9E, 0x9F, 0xA0};

// x kept within [min, max]. A macro: the original computes each bound only
// on the branch that needs it.
#define CLAMP(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // The air trick state.
    const long STATE_AIR_TRICK = 0x27;

    // The steps of the special shot (STATE_WORK_AIR_TRICK::m_nStep).
    enum {
        STEP_INIT,
        STEP_START_TRICK,
        STEP_TRICK,
        STEP_READY,
        STEP_SHOT,
        STEP_HITBACK,
        STEP_LAND,

        // ArtsProc sets this when the arts event is over.
        STEP_ARTS_END = 200
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

    // The distance between two points in the XZ plane.
    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    // vec without its height, scaled to length 1 (zero when it is zero).
    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &vec)
    {
        bcs::FVEC3 xz(vec.x, 0.0f, vec.z);
        bcs::BCS_Vec_Normalize(rOut, xz);
    }

    // The puck goes into the special shot: it is told so, gets flag 0x10
    // and loses its marker.
    inline void SetArtsBall(msw::CBall *pBall)
    {
        pBall->SetHockeyArtsStart();
        pBall->OnFlag(0x10);
        pBall->SetDisableMarker(1);
    }
}

float msw::CChrCoreHockey::GetArtsEventRot()
{
    return GetRotY_ToPosXZ(GetTargetGoalPos());
}

void msw::CChrCoreHockey::DoPlayArtsEvent()
{
    AnimPlay(3, 0x9B, 0, 0.0f, 0.0f);
    Eff_CreateSimple(0xF, GetPos(), GetRotY(), 3, 0, 0);
    PlaySe(0xC06, 1, -1, 0.0f, 1.0f, 0.0f);
}

void msw::CChrCoreHockey::ArtsFadeOut() const
{
    CGame::BgSetFadeOut(0.0f, 30.0f);
}

void msw::CChrCoreHockey::ArtsFadeIn() const
{
    CGame::BgSetFadeIn(0.0f, 30.0f);
}

void msw::CChrCoreHockey::HockeyArtsEnd()
{
    Util_SendMsgToAll((CChrDrv::MSG_ID)0x7D, 0, 0, 0, 0);
}

void msw::CChrCoreHockey::ArtsParamInit()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();

    pWork->m_nStep = 0;
    pWork->m_fArtsUnk78 = GetCcParam_()->m_UnkC8;
    m_nStatusFlag |= 0x40000000;
    SetCollBodyPushPower_(5);
    SetCollBodyPushRank_(3);
}

// The arts event: the character turns to the goal he shoots at and the
// event runs (ArtsProc) until it sets STEP_ARTS_END.
void msw::CChrCoreHockey::CtorStateAirTrick_()
{
    ArtsParamInit();
    CBall *pBall = GetAttachBall();
    if (pBall) {
        SetArtsBall(pBall);
    }
    Eff_StopGroup(3);
    ArtsProc((ARTS_EVENT)0);

    float fRotY = GetRotY_ToPosXZ(CGame::GetGoalPos(GetTargetGoalIdx()));
    SetRotY(WrapRad(fRotY));
    SkateWorkClearAccXZTimer_();
    m_fSkateUnk5A4 = -1.0f / 30.0f;
    CGame::TrgArtsEvent(m_nPlayerNo);
    CGame::SendMessage(0xD, 0, 0, 0);
    Util_SendMsgToAll((CChrDrv::MSG_ID)0x7C, m_nPlayerNo, m_nChrId, 0, 0);
}

void msw::CChrCoreHockey::LoopStateAirTrick_()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();

    ArtsProc((ARTS_EVENT)1);
    if (pWork->m_nStep == STEP_ARTS_END) {
        ArtsProc((ARTS_EVENT)2);
        FsmqDoChain();
    }
}

void msw::CChrCoreHockey::DtorStateAirTrick_()
{
    Eff_Stop(0x13);
    Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
    Eff_StopGroup(4);
    m_nStatusFlag &= ~0x40000000;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(1);
    CBall *pBall = CBallMan::GetBallPtr(0);
    if (pBall) {
        pBall->OffFlag(0x10);
    }
}

void msw::CChrCoreHockey::CtorStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();

    SetCollBodyPushRank_(3);
    SetCollBodyPushPower_(5);
    m_UnkEC &= ~0x10;
    m_nStatusFlag &= ~0x1F000000;
    m_nStatusFlag &= ~0x20;
    SetCondition(0, 0.0f, 0);
    m_nStatusFlag |= 0x40000000;
    SetMoveMotType(4);
    Eff_StopGroup(3);
    CCollMan::SetArtsExColl(GetGuardGoalIdx());
    CBall *pBall = GetAttachBall();
    if (pBall) {
        SetArtsBall(pBall);
    }
    CChrManHockey::GetInstance()->ArtsStart_();

    pWork->m_nStep = STEP_INIT;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_nLoopPlayed = 0;
    pWork->m_nTrickStep = 0;
    pWork->m_fTrickMotFrameNum = 60.0f;
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fUnk0C = GetCcParam_()->m_Unk1E0;
    pWork->m_fTargetRotY = -1.0f;
    pWork->m_nJumpType = 0;
    SkateWorkInit_();
    m_fSkateUnk5B4 = 0.0f;
    pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());

    CGame::ClearTeamArtsGauge(m_nTeamNo);
    CGame::TrgArtsEvent(m_nPlayerNo);
    CGame::SendMessage(0xD, m_nPlayerNo, 0, 0);
    Util_SendMsgToAll((CChrDrv::MSG_ID)0x7C, m_nPlayerNo, m_nChrId, 0, 0);
}

void msw::CChrCoreHockey::LoopStateAirTrick()
{
    STATE_WORK_AIR_TRICK *pWork = FsmqGetWork<STATE_WORK_AIR_TRICK>();
    int bEnd = 0;

    SubStateArtsShotAskTargetMarkerMove_(pWork);
    switch (pWork->m_nStep) {
        case STEP_INIT:
            pWork->m_nStep = STEP_START_TRICK;
            break;
        case STEP_START_TRICK:
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

void msw::CChrCoreHockey::DtorStateAirTrick()
{
    Eff_StopGroup(4);
    SE_StopGroup(1, 0.0f);
    m_nStatusFlag &= ~0x800;
    ArtsOptDestroy();
    m_nStatusFlag &= ~0x40000000;
    CBall *pBall = GetAttachBall();
    if (pBall) {
        pBall->SetDisableMarker(0);
    }
}

// The voice (nType 0 only) and the character's own jump sound.
void msw::CChrCoreHockey::SE_PlayArtsJumpSE(long nType)
{
    if (nType == 0) {
        PlayVoice(0x82, 0);
    }

    long nSe = -1;
    switch (m_nChrId) {
        case 0:
            nSe = 0x3E9;
            break;
        case 1:
            nSe = 0x3FA;
            break;
        case 2:
            nSe = 0x3FB;
            break;
        case 3:
            nSe = 0x40B;
            break;
        case 4:
            nSe = 0x413;
            break;
        case 5:
            nSe = 0x414;
            break;
        case 6:
            nSe = 0x426;
            break;
        case 7:
            nSe = 0x427;
            break;
        case 8:
            nSe = 0x43B;
            break;
        case 9:
            nSe = 0x4C4;
            break;
        case 10:
            nSe = 0x446;
            break;
        case 11:
            nSe = 0x450;
            break;
        case 12:
            nSe = 0x4D2;
            break;
        case 13:
            nSe = 0x468;
            break;
        case 14:
            nSe = 0x471;
            break;
        case 15:
            nSe = 0x4E3;
            break;
        case 16:
            nSe = 0x481;
            break;
        case 17:
            nSe = 0x489;
            break;
        case 19:
            nSe = 0x490;
            break;
        case 20:
            nSe = 0x499;
            break;
    }
    if (nSe >= 0) {
        PlaySePos(nSe, (const bcs::FVEC3 *)0, 0, -1, 0.0f, 1.0f, 0.0f);
    }
}

void msw::CChrCoreHockey::SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    SubStateArtsShotSetSetbackFrame_(pWork, 0);
    SubStateArtsShotPlayTrickMotion_(pWork);
    Util_AttachEffect((ATTACH_EFFECT_TYPE)1, 0);
    SubStateArtsShotSetTrickJumpParam_(pWork, 0);
    SubStateArtsShotSetTrickMoveParam_(pWork, 0);

    // Turns to face the goal from where the trick moves to.
    bcs::FVEC3 dir(GetTargetGoalPos());
    dir.x -= pWork->m_TargetPos.x;
    dir.z -= pWork->m_TargetPos.z;
    pWork->m_fRotY = WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z));
    SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, SubStateArtsShotGetTrickRotFrame_(pWork));
}

// The trick lasts the frames the character's parameters give it; its
// motion is played faster or slower to fit them.
void msw::CChrCoreHockey::SubStateArtsShotSetSetbackFrame_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    pWork->m_fSetbackWait = SubStateArtsShotGetSetbackWaitFrame_(nTrick);
    float fMotFrame = SubStateArtsShotGetTrickMotFrame_(pWork, nTrick);
    fMotFrame = Max(1.0f, fMotFrame - GetCcParam_()->m_Unk1DC);
    float fFrame = SubStateArtsShotGetTrickDoFrame_(pWork, nTrick);
    if (fFrame <= 0.0f) {
        fFrame = fMotFrame;
        pWork->m_fAnimSpeed = 1.0f;
    } else {
        pWork->m_fAnimSpeed = fMotFrame / fFrame;
    }
    pWork->m_fTrickMotFrameNum = fFrame;
}

float msw::CChrCoreHockey::SubStateArtsShotGetSetbackWaitFrame_(long nTrick)
{
    if (nTrick != 0) {
        return 0.0f;
    }
    return GetCcParam_()->m_Unk1B4;
}

void msw::CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    AnimPlay(3, 0x9C, 0, GetCcParam_()->m_Unk1F8, 0.0f);
    AnimSetSpeed(3, pWork->m_fAnimSpeed);
    ArtsOptAnimSetSpeed(pWork->m_fAnimSpeed);
}

// A turn to fRot over fFrame frames, the short way round.
void msw::CChrCoreHockey::SubStateArtsShotCalcRotParam_(ROT_LERP1 &rLerp, float fRot, float fFrame)
{
    rLerp.m_afUnk[0] = WrapRad(fRot);
    rLerp.m_afUnk[2] = fFrame;
    rLerp.m_afUnk[2] = Max(rLerp.m_afUnk[2], 1.0f);
    float fDiff = WrapRad(rLerp.m_afUnk[0] - GetRotY());
    if (fDiff > PI) {
        fDiff -= PI2;
    }
    rLerp.m_afUnk[1] = fDiff / rLerp.m_afUnk[2];
    rLerp.m_afUnk[3] = 0.0f;
}

void msw::CChrCoreHockey::SubStateArtsShotClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &rParam)
{
    rParam.m_nType = 2;
    rParam.m_fFrame = 0.0f;
    rParam.m_fFrameMax = 0.0f;
    rParam.m_fDist = 0.0f;
}

// The turn, and once the wait is over the move towards m_TargetPos.
inline void msw::CChrCoreHockey::SubStateArtsShotDoTrickMove_(STATE_WORK_AIR_TRICK *pWork)
{
    RotateYParam_(pWork->m_RotLerp);
    if (pWork->m_fSetbackWait <= 0.0f && pWork->m_fMoveFrame < pWork->m_Move.m_fFrameMax) {
        pWork->m_fMoveFrame += m_fFrameStep * m_fTimeScale;
        pWork->m_DirMove.m_fRotY = GetRotY_ToPosXZ(pWork->m_TargetPos);
        Util_DoDirMove(pWork->m_Move, pWork->m_fMoveFrame, pWork->m_DirMove, 0);
    }
}

// One frame of the trick: with jump kind 2 the jump (returns
// Util_DoJumpMoveEX's step), with kind 1 until the motion ends (then 2),
// with kind 0 no jump.
int msw::CChrCoreHockey::SubStateArtsShotDoJumpMove_(STATE_WORK_AIR_TRICK *pWork)
{
    int nResult = 0;

    if (pWork->m_fSetbackWait > 0.0f) {
        pWork->m_fSetbackWait -= m_fFrameStep * m_fTimeScale;
    }
    switch (pWork->m_nJumpType) {
        case 0:
            SubStateArtsShotDoTrickMove_(pWork);
            break;
        case 1:
            if (IsAnimEnd(3)) {
                nResult = 2;
            } else {
                SubStateArtsShotDoTrickMove_(pWork);
            }
            break;
        case 2:
            nResult = Util_DoJumpMoveEX(0);
            if (nResult == 1) {
                SubStateArtsShotDoTrickMove_(pWork);
            }
            break;
    }
    return nResult;
}

// The frames of the trick motions (1 to 5 of them, as the parameters say),
// less the wait before the move.
float msw::CChrCoreHockey::SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *pWork, long)
{
    long nNum = GetCcParam_()->m_nUnk1B8;
    nNum = CLAMP(nNum, 1, 5);
    float fFrame = 0.0f;
    for (long i = 0; i < nNum; i++) {
        fFrame += AnimGetResFrameNum(s_aTrickMotId[i]);
    }
    fFrame -= pWork->m_fSetbackWait;
    return Max(fFrame, 1.0f);
}

float msw::CChrCoreHockey::SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return GetCcParam_()->m_Unk1D8;
}

float msw::CChrCoreHockey::SubStateArtsShotGetTrickRotFrame_(STATE_WORK_AIR_TRICK *pWork)
{
    return Min(16.0f, pWork->m_fTrickMotFrameNum);
}

// Where the trick moves the character: away from the goal, by the
// parameter's distance but at least to the parameter's distance from the
// goal. Standing to the side of the goal, he is turned towards the line in
// front of it, by up to the parameter's angle.
void msw::CChrCoreHockey::SubStateArtsShotCalcSetbackPos_(bcs::FVEC3 &rPos)
{
    bcs::FVEC3 goal(GetTargetGoalPos());
    bcs::FVEC3 pos(GetPos());
    bcs::FVEC3 dir;
    bcs::FVEC3 front;
    bcs::FVEC3 cross;
    rPos = pos;

    float fDist = DistXZ(goal, pos);
    float fLen = GetCcParam_()->m_Unk1C4;
    float fMin = GetCcParam_()->m_Unk1C8;
    if (fLen + fDist < fMin) {
        fLen = fMin - fDist;
    }

    dir.x = pos.x - goal.x;
    dir.z = pos.z - goal.z;
    NormalizeXZ(dir, dir);

    bcs::BCS_VecOneZ_RotY(front, GetTargetGoalYRot());
    float fAngle = bcs::DEG_TO_RAD(GetCcParam_()->m_Unk1CC);
    if (fDist < GetCcParam_()->m_Unk1D0) {
        fAngle = 0.0f;
    }
    float fDot = front.x * dir.x + front.z * dir.z;
    if (fDot < nw4r::math::CosRad(fAngle)) {
        PSVECCrossProduct((const Vec *)&front, (const Vec *)&dir, (Vec *)&cross);
        if (cross.y < 0.0f) {
            fAngle = -fAngle;
        }
        bcs::BCS_Vec_RotY(dir, front, WrapRad(fAngle));
        float fScale = fDist + fLen;
        dir.x *= fScale;
        dir.z *= fScale;
        rPos.x = goal.x + dir.x;
        rPos.z = goal.z + dir.z;
    } else {
        dir.x *= fLen;
        dir.z *= fLen;
        rPos.x = pos.x + dir.x;
        rPos.z = pos.z + dir.z;
    }
}

// The jump rises from where he stands to the parameter's height (or stays
// level when he is higher already); no jump (kind 0) when that height is
// not above the ice.
void msw::CChrCoreHockey::SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nTrick)
{
    float fTop = GetCcParam_()->m_Unk1BC;
    float fTopFrame = GetCcParam_()->m_Unk1C0;
    float fY = GetPosY();
    if (fTop < fY) {
        fTop = fY;
    }
    float fFrame = Max(1.0f, pWork->m_fTrickMotFrameNum - fTopFrame);
    if (fTop > 0.0f) {
        Util_SetJumpParam(fFrame, fY, fY, fTop);
        m_fJumpUnk224 = pWork->m_fUnk0C + (pWork->m_fTrickMotFrameNum - fFrame);
        m_fJumpUnk228 = SubStateArtsShotGetSetbackWaitFrame_(nTrick);
        pWork->m_nJumpType = 2;
    } else {
        pWork->m_nJumpType = 0;
    }
}

// The trick moves the character back to a place away from the goal.
void msw::CChrCoreHockey::SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    SubStateArtsShotCalcSetbackPos_(pWork->m_TargetPos);
    SubStateArtsShotClearMoveParamWork_(pWork->m_Move);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fMoveFrame = 0.0f;

    float fDistSq = DistSqXZ(pWork->m_TargetPos, GetPos());
    if (fDistSq > 0.0f) {
        float fFrame = pWork->m_fTrickMotFrameNum - GetCcParam_()->m_Unk1D4;
        fFrame = Max(1.0f, fFrame);
        pWork->m_Move.m_nType = 0xB;
        pWork->m_Move.m_fDist = nw4r::math::FSqrt(fDistSq);
        pWork->m_Move.m_fFrame = 0.0f;
        pWork->m_Move.m_fFrameMax = fFrame;
    }
}

int msw::CChrCoreHockey::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SubStateArtsShotDoJumpMove_(pWork);
    return IsAnimEnd(3) != 0;
}

// The loop motion, the attack sign (except for character 14) and the wait
// for the shot.
void msw::CChrCoreHockey::SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SubStateArtsShotPlayLoopMotion_(pWork);
    if (GetChrId() != 14) {
        bcs::FVEC3 pos;
        GetVNodePos(1, pos);
        CEffectCtrl::SimpleEffect(0x4E3, pos, 3, 0, 0);
        PlaySePos(0xC07, &pos, 1, -1, 0.0f, 1.0f, 0.0f);
    }
    SE_PlayArtsLoopSE();
    pWork->m_fUnk0C = GetCcParam_()->m_Unk1E0;
}

void msw::CChrCoreHockey::SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *)
{
    AnimPlay(3, 0xA7, 1, GetCcParam_()->m_Unk1FC, 0.0f);
}

// The shot starts when the driver answers or the wait is over.
int msw::CChrCoreHockey::SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bStart = 0;
    long nAnswer = DriverSendMessage((CChrDrv::MSG_ID)0x21, 0, 0, 0, 0);
    if (pWork->m_fUnk0C > 0.0f) {
        pWork->m_fUnk0C -= m_fFrameStep;
    }
    if (nAnswer != 0 || pWork->m_fUnk0C <= 0.0f) {
        bStart = 1;
    }
    SubStateArtsShotDoJumpMove_(pWork);
    if (!bStart && pWork->m_nLoopPlayed < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nLoopPlayed = 1;
    }
    return bStart;
}

// With flag 1 the driver is asked where the shot should aim (the answer
// comes as m_fTargetRotY).
void msw::CChrCoreHockey::SubStateArtsShotAskTargetMarkerMove_(STATE_WORK_AIR_TRICK *pWork)
{
    if (pWork->m_nFlag & 1) {
        pWork->m_fTargetRotY = -1.0f;
        DriverSendMessage((CChrDrv::MSG_ID)0x28, 0, 0, 0, 0);
    }
}

int msw::CChrCoreHockey::AutoMoveTargetMarkerPos()
{
    if (GetState() != STATE_AIR_TRICK) {
        return 0;
    }
    return 1;
}

void msw::CChrCoreHockey::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
}

void msw::CChrCoreHockey::SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    SE_Stop(8, 8.0f);
    SubStateArtsShotPlayArtsShotMotion_(pWork);
    SubStateArtsShotDoGetBallShotTrg_(pWork, 16.0f);
}

void msw::CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    float fBlend = GetCcParam_()->m_Unk200;
    if (pWork->m_nLoopPlayed < 1) {
        fBlend = GetCcParam_()->m_Unk204;
    }
    AnimPlay(3, 0xA1, 0, fBlend, 0.0f);
}

// The puck leaves at trigger 2 of the shot motion, or at frame fFrame
// without one.
void msw::CChrCoreHockey::SubStateArtsShotDoGetBallShotTrg_(STATE_WORK_AIR_TRICK *pWork, float fFrame)
{
    float fTrg = AnimSearchTrgFrame(AnimGetMotID(3, 0), 2, -1.0f);
    if (fTrg < 0.0f) {
        fTrg = fFrame;
    }
    pWork->m_fShotFrame = fTrg;
}

int msw::CChrCoreHockey::SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_fRotY = GetRotY_ToPosXZ(GetTargetGoalPos());
    if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fShotFrame) {
        Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
        SubStateArtsShotShootBall_(pWork);
        pWork->m_nLoopPlayed = 0;
        return 1;
    }
    return 0;
}

// Shoots the puck as a special shot (kind 9) at the goal's target point.
void msw::CChrCoreHockey::SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *)
{
    Util_DetachEffect((ATTACH_EFFECT_TYPE)1);
    CBall *pBall = GetAttachBall();
    if (pBall) {
        Util_SendMsgToAll((CChrDrv::MSG_ID)7, m_nPlayerNo, 9, 0, 0);
        bcs::FVEC3 dir;
        bcs::FVEC3 target = GetShootGoalTgtPos();
        const bcs::FVEC3 &rBallPos = pBall->GetPos();
        dir.x = target.x - rBallPos.x;
        dir.z = target.z - rBallPos.z;
        float fRotY = WrapRad(nw4r::math::Atan2Rad(dir.x, dir.z));
        Util_ShootBall(9, m_fUnk264, fRotY, 0.0f, 0);
        SE_PlayArtsShootSE(pBall->GetPos());
    }
}

// The shot pushes the character back: a move backwards, away from the
// direction he shot in.
void msw::CChrCoreHockey::SubStateArtsShotStartArtsHitback_(STATE_WORK_AIR_TRICK *pWork)
{
    pWork->m_Move.m_nType = 0xB;
    pWork->m_Move.m_fFrame = 0.0f;
    pWork->m_Move.m_fFrameMax = GetCcParam_()->m_Unk1E4;
    pWork->m_Move.m_fDist = GetCcParam_()->m_Unk1E8;
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fMoveFrame = 0.0f;
    SubStateArtsShotCalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, GetCcParam_()->m_Unk1E4);

    bcs::BCS_VecZ_RotY(pWork->m_TargetPos, -2.0f * GetCcParam_()->m_Unk1E8, pWork->m_fRotY);
    const bcs::FVEC3 &rPos = GetPos();
    pWork->m_TargetPos.x += rPos.x;
    pWork->m_TargetPos.z += rPos.z;
    pWork->m_nJumpType = 1;
}

int msw::CChrCoreHockey::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    return SubStateArtsShotDoJumpMove_(pWork) >= 2;
}

void msw::CChrCoreHockey::SubStateArtsShotDoLandStart_(STATE_WORK_AIR_TRICK *pWork)
{
    m_UnkEC |= 0x10;
    SetCollBodyPushRank_(1);
    SetCollBodyPushPower_(5);
    SubStateArtsShotPlayLandMotion_(pWork);
    SubStateArtsShotPlayLandEffect_(pWork);
    pWork->m_nStep = STEP_LAND;
}

void msw::CChrCoreHockey::SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *)
{
    AnimPlay(3, 0xA6, 0, GetCcParam_()->m_Unk208, 0.0f);
}

void msw::CChrCoreHockey::SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *)
{
    Util_SetEffect((SET_EFFECT_TYPE)0);
}
