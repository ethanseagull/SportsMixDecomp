// msw::CChrCoreVolleyKinopio (Toad) in the volleyball game.
//
// Toad does not dive for a receive: he puts a mushroom (the receiver,
// CC09_VOLLEY::CcKinopioReceiver) on the ground where the ball comes down,
// and the ball is caught on it. His air trick puts two bouncing mushrooms
// (CC09_VOLLEY::CcKinopioBoundMushroom) in the other court, and his block
// is a jump with a mushroom effect.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGrdMdlEffect.hpp"
#include "game/msw/CMdlEffect.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CcBoundMushroom.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

namespace msw {
    namespace CC09_VOLLEY {

        // The mushroom Toad receives the ball on: a ground effect with a
        // body collision that grows from 0.01 to 0.5 and keeps to the court.
        // 0x68 bytes. (member names guessed)
        class CcKinopioReceiver : public CC_CMN::CGrdMdlEffect {
            public:
                CcKinopioReceiver(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot,
                                  long nCourt, bcs::BCS_Color color);
                virtual ~CcKinopioReceiver();
                virtual void Main();

                static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);
                void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);

            private:
                CBodyCollision m_BodyColl;  // 0x50
                float m_fRadius;            // 0x5C
                long m_nCourt;              // 0x60, the court it stays in (CCollMan::DetectMapHit)
                float m_fSeTimer;           // 0x64, frames until the second sound
        };

        // Toad's bouncing mushroom in the volleyball game: bigger than the
        // common one, swinging from side to side while it turns round its
        // point. 0x128 bytes, as its base.
        class CcKinopioBoundMushroom : public CC_CMN::KINOPIO::CcBoundMushroom {
            public:
                CcKinopioBoundMushroom(bcs::CTask *pParent, nw4r::g3d::ResFile mdl, nw4r::g3d::ResFile mot,
                                       const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo, const bcs::FVEC3 &pos,
                                       float fScale, long nCourt, bcs::BCS_Color color, long nPlayer)
                    : CcBoundMushroom(pParent, mdl, mot, pInfo, pos, fScale, nCourt, color, nPlayer)
                {
                    m_BodyColl.SetHitEffectFrameAll(40.0f);
                }
                virtual ~CcKinopioBoundMushroom();

                virtual float GetParamScale_() const;
                virtual void SetAppearEffect_();
                virtual void SetDisappearEffect_();
                virtual void MovePos_(float fFrame);
                virtual void CalcPos_();
        };
    }
}

namespace {

    // (guessed helpers) fValue kept within [fMin, fMax]; the smaller and the
    // larger of two values; an angle brought into [0, 2 pi); degrees to
    // radians; the distance between two points in the XZ plane.
    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

    inline float Max(float fValue, float fMin)
    {
        return fMin < fValue ? fValue : fMin;
    }

    inline void Swap(float &rA, float &rB)
    {
        float fTemp = rA;
        rA = rB;
        rB = fTemp;
    }

    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }
}

msw::CChrCoreVolleyKinopio::CChrCoreVolleyKinopio()
{
    m_hReceiver = 0;
}

msw::CChrCoreVolleyKinopio::~CChrCoreVolleyKinopio()
{
}

inline int msw::CChrCoreVolleyKinopio::IsReceiverAlive_()
{
    if (m_hReceiver == 0) {
        return 0;
    }
    bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
    if (pTask && !pTask->IsDelete()) {
        return 1;
    }
    m_hReceiver = 0;
    return 0;
}

inline void msw::CChrCoreVolleyKinopio::ReleaseReceiver_()
{
    bcs::CTaskMan::GetTask(m_hReceiver);
    m_hReceiver = 0;
}

void msw::CChrCoreVolleyKinopio::Reset_()
{
    CChrCoreVolley::Reset_();
    ReleaseReceiver_();
}

// The diving receive: 0 starts the motion, 1 puts the receiver down at the
// motion's trigger frame, 3 waits for the motion and goes on to 4 while the
// receiver is there, 4 waits for the receiver to go. The ball can be caught
// on the receiver while it exists. Returns not zero when the state is over.
int msw::CChrCoreVolleyKinopio::SubStateReceiveDoProcDiveReceive_(STATE_WORK_RECEIVE *pWork)
{
    int bEnd = 0;

    switch (pWork->m_nStep) {
        case 0:
            SE_PlayWrapAct(0xD, 0, -1);
            PlayVoice(0x3D, 0);
            SetMoveMotType(0);
            AnimPlay(3, 0x1E, 0, 4.0f, 0.0f);
            pWork->m_fTrgFrame = GetBallShotTrgFrame_(0x1E, 8.0f);
            RotateYRate_(pWork->m_fRotY, 0.2f);
            pWork->m_nStep = 1;
            break;
        case 1:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (IsAnimEnd(3) || AnimGetFrame(3) >= pWork->m_fTrgFrame) {
                FireReceiver_();
                pWork->m_nStep = 3;
            }
            break;
        case 3:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (IsAnimEnd(3)) {
                if (IsReceiverAlive_()) {
                    AnimPlay(3, 0x20, 1, 4.0f, 0.0f);
                    pWork->m_nStep = 4;
                } else {
                    pWork->m_nStep = 5;
                    bEnd = 1;
                }
            }
            break;
        case 4:
            RotateYRate_(pWork->m_fRotY, 0.2f);
            if (!IsReceiverAlive_()) {
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }

    if (!(m_nStatusFlag & 1)) {
        if (IsReceiverAlive_()) {
            bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_hReceiver);
            if (pTask && !pTask->IsDelete()) {
                CC09_VOLLEY::CcKinopioReceiver *pReceiver = static_cast<CC09_VOLLEY::CcKinopioReceiver *>(pTask);
                SetBallCatchEnableWorld_(1.0f, 0.5f, pReceiver->GetPos());
            }
        } else {
            SetBallCatchDisable_();
        }
    }

    if (m_nStatusFlag & 1) {
        long nType = 0;
        if (pWork->m_nFlag & 2) {
            nType = 1;
        }
        CBall *pBall = GetAttachBall();
        if (pBall) {
            unsigned long nReact = pBall->GetVolleyReceiveReaction(3, GetCcParam_()->m_Unk04);
            pBall->GetVolleyFlickDownPower(3);
            long nCatch = 0;
            nReact = SubStateReceiveDoCheckDiveReactFlag_(pWork, pBall, nReact);
            if (nReact & 0x100) {
                nCatch = (unsigned char)nReact;
            }
            if (nReact & (0x10000 | 0x8000 | 0x1000)) {
                if (nReact & 0x8000) {
                    nType = 3;
                } else {
                    nType = 2;
                    if (nReact & 0x10000) {
                        nType = 4;
                    }
                }
            }
            pBall->TrgVolleyReceiveReaction(0, 1);
            ReceiveBall_(3, nCatch, -1.0f, nType, pWork->m_nReceiveType, (pWork->m_nFlag >> 8) & 1);
            ReleaseReceiver_();
        }
    }

    m_afCountWork[0] = pWork->m_afCountWork[0];
    m_afCountWork[1] = pWork->m_afCountWork[1];
    return bEnd;
}

void msw::CChrCoreVolleyKinopio::DtorStateReceive()
{
    CChrCoreVolley::DtorStateReceive();
    ReleaseReceiver_();
}

// The block: a jump with its own motion and push settings.
void msw::CChrCoreVolleyKinopio::SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_fActStartFrame = 0.0f;
    pWork->m_fActTrgFrame = GetBallShotTrgFrame_(0x3E, 8.0f);
    PlayVoice(0x40, 0);
    SE_PlayWrapAct(0xB, 0, -1);
    SetMoveMotStep_(0);
    SetMoveMotType(0);
    m_UnkEC &= ~0x10;
    SetCollBodyPushPower_(1);
    SetCollBodyPushType_((BODY_COLL_SIZE_SET_TYPE)0, 8.0f);
    SetCollBodyPushRank_(4);
    AnimPlay(3, 0x3E, 0, 4.0f, 0.0f);
}

// Not zero once the block jump has landed. At the motion's trigger frame the
// block effect comes out in front of the character; once he falls below the
// block height it no longer can.
int msw::CChrCoreVolleyKinopio::SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    Util_DoJumpMoveEX(1);
    if (m_nJumpStep >= 2) {
        SubStateBlockJumpLandStart_(pWork);
        return 1;
    }

    if (pWork->m_fRotY >= 0.0f) {
        RotateYRate_(pWork->m_fRotY, 0.2f);
    }
    if (pWork->m_nFlag & 4) {
        SubStateBlockJumpColSlide_(pWork);
    }
    if (pWork->m_nFlag & 2) {
        float fHeight = GetCcParam_()->m_Unk74;
        if (GetPosY() <= fHeight && m_fJumpVelY < 0.0f) {
            pWork->m_nFlag &= ~6;
            pWork->m_fActTrgFrame = -1.0f;
        }
    }

    if (AnimGetFrame(3) >= pWork->m_fActTrgFrame && pWork->m_fActTrgFrame >= 0.0f && !(pWork->m_nFlag & 8) &&
        !(pWork->m_nFlag & 2)) {
        pWork->m_nFlag |= 2;
        bcs::FVEC3 ofs(0.0f, 0.5f * GetCcParam_()->m_Unk58 + GetCcParam_()->m_Unk5C, GetCcParam_()->m_Unk84);
        bcs::BCS_Vec_RotY(pWork->m_EffOfs, ofs, GetBlockTargetRot_());
        Eff_CreateAttach(0x12, 0x681, 0, &pWork->m_EffOfs, 0, 3, 0);
    }
    return 0;
}

int msw::CChrCoreVolleyKinopio::SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_nFlag &= ~2;
    return 0;
}

// A ball hit on the block: the block effect is replaced by one that hits
// back (0x682 for kind 2, 0x683 for kinds 0 and 3).
void msw::CChrCoreVolleyKinopio::SubStateBlockJumpBlockHit_(const CCollDefUnit *pDef)
{
    if (GetState() == 5) {
        STATE_WORK_BLOCK_JUMP *pWork = static_cast<STATE_WORK_BLOCK_JUMP *>(FsmqGetWork_());
        if (pWork->m_nFlag & 2) {
            long nKind = pDef->GetKind();
            if ((unsigned long)(nKind - 2) <= 1 || nKind == 0) {
                Eff_StopForce(0x12);
                bcs::FVEC3 pos(0.0f, 0.5f * GetCcParam_()->m_Unk58 + GetCcParam_()->m_Unk5C,
                               GetCcParam_()->m_Unk84);
                bcs::BCS_Vec_RotY(pWork->m_EffOfs, pos, GetBlockTargetRot_());
                bcs::FVEC3Add(&pos, &pWork->m_EffOfs, &GetPos());
                long nEff = 0x683;
                if (nKind == 2) {
                    nEff = 0x682;
                }
                unsigned long hEffect = Eff_CreateSimple(nEff, pos, GetBlockTargetRot_(), 3, 0, 0);
                float fZOffset = GetCcParam_()->m_Unk88;
                if (hEffect && fZOffset != 0.0f) {
                    CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                    if (pCluster) {
                        pCluster->SetZSortOffset(fZOffset);
                    }
                }
            }
        }
    }
    CChrCoreVolley::SubStateBlockJumpBlockHit_(pDef);
}

void msw::CChrCoreVolleyKinopio::SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickStartTrick_(pWork);
    CreateBoundMushroom_();
}

// The air trick jumps twice: a low hop (0), then the trick jump (1) up to
// the height in the parameters.
void msw::CChrCoreVolleyKinopio::SubStateAirTrickSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    if (nStep == 0) {
        float fHeight = 3.0f;
        Util_SetJumpParam(pWork->m_fTrickFrame, GetPosY(), fHeight, 2.0f + fHeight);
        m_fJumpUnk224 = 0.0f;
        m_fJumpUnk228 = 0.0f;
    } else {
        float fHeight = GetCcParam_()->m_Unk37C;
        float fPosY = GetPosY();
        if (fHeight < fPosY) {
            fHeight = fPosY;
        }
        Util_SetJumpParam(pWork->m_fTrickFrame, fPosY, fHeight, fHeight);
        m_fJumpUnk224 = pWork->m_fJumpUnk;
        m_fJumpUnk228 = 0.0f;
    }
}

// The hop goes to the back line of the character's court half; the trick
// jump goes 2 further back.
void msw::CChrCoreVolleyKinopio::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep)
{
    static const float fDist = 2.0f;

    if (nStep == 0) {
        ClearMoveParamWork_(pWork->m_MoveParam);
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fUnk20 = 0.0f;
        pWork->m_TargetPos = GetPos();

        bcs::FVEC4 rect;
        Util_GetGuardCourtRect_(rect);
        pWork->m_TargetPos.x = 0.5f * (rect.x + rect.y);
        if (GetGuardGoalIdx()) {
            pWork->m_TargetPos.z = rect.z;
        } else {
            pWork->m_TargetPos.z = rect.w;
        }
        pWork->m_MoveParam.m_nType = 0;
        pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
        pWork->m_MoveParam.m_fFrame = 1.0f;
        pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    } else {
        ClearMoveParamWork_(pWork->m_MoveParam);
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fUnk20 = 0.0f;
        pWork->m_TargetPos = GetPos();

        pWork->m_TargetPos.z += GetGuardGoalIdx() ? -fDist : fDist;
        pWork->m_MoveParam.m_nType = 0xB;
        float fMove = DistXZ(pWork->m_TargetPos, GetPos());
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fDist = -1.0f * fMove;
        pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
    }
}

float msw::CChrCoreVolleyKinopio::SubStateAirTrickGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long nStep)
{
    if (nStep == 0) {
        return AnimGetResFrameNum(0x75);
    }
    return AnimGetResFrameNum(0x76);
}

float msw::CChrCoreVolleyKinopio::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long nStep)
{
    if (nStep == 0) {
        return 0.0f;
    }
    return GetCcParam_()->m_Unk380;
}

int msw::CChrCoreVolleyKinopio::SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *)
{
    return 0;
}

// Not zero once the trick jump is over. The hop (step 0) ends in the trick
// jump, with its motion, effects and sound.
int msw::CChrCoreVolleyKinopio::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    if (pWork->m_nTrickStep == 0) {
        pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
        pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
        SubStateAirTrickDoJumpMove_(pWork);
        SubStateAirTrickDoBallAdsorb_(pWork);
        if (!Eff_IsAlive(0x16) && AnimGetFrame(3) >= 5.0f) {
            Eff_CreateAttachAuto(0x16, 0x8A, 3);
        }
        if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
            SubStateAirTrickSetTrickFrameParam_(pWork, 1);
            AnimPlay(3, 0x76, 0, 4.0f, 0.0f);
            Eff_CreateAttachAuto(0x14, 0x89, 3);
            Eff_CreateAttachAuto(0x17, 0x8B, 3);
            AnimSetSpeed(3, pWork->m_fAnimSpeed);
            PlaySePos(0x440, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            SubStateAirTrickSetTrickJumpParam_(pWork, 1);
            SubStateAirTrickSetTrickMoveParam_(pWork, 1);
            SubStateAirTrickStartTMDisp_(pWork);
            pWork->m_nTrickStep++;
        }
    } else {
        pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
        pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
        if (Eff_IsAlive(0x16) && AnimGetFrame(3) >= 8.0f) {
            Eff_Stop(0x16);
        }
        SubStateAirTrickDoJumpMove_(pWork);
        if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
            bReady = 1;
        }
    }
    return bReady;
}

// From the ground, the trick leaves a mushroom effect in Toad's colour.
void msw::CChrCoreVolleyKinopio::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    if (GetPosY() < 0.1f) {
        bcs::FVEC3 pos(GetPos());
        pos.y = 0.0f;
        CC_CMN::CMdlEffect *pEffect = new CC_CMN::CMdlEffect(CGame::GetTempParent(), 0x5C6, pos, GetRot(), 0, 3);
        if (pEffect) {
            bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
            pEffect->SetMatTevKColorRGB(GX_KCOLOR0, color);
        }
        Eff_CreateSimple(0x5C5, pos, GetRotY(), 3, 0, 0);
    }
    Eff_CreateAttachAuto(0x13, 0x88, 3);
}

void msw::CChrCoreVolleyKinopio::SubStateAirTrickPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLoopMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x8C, 3);
}

void msw::CChrCoreVolleyKinopio::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_Stop(0x15);
    Eff_Stop(0x17);
    Eff_CreateAttachAuto(0x18, 0x8D, 3);
}

// Two mushrooms on the middle line of the court the team attacks, a quarter
// and three quarters of the way back; the first is Toad's, and the second
// is larger.
void msw::CChrCoreVolleyKinopio::CreateBoundMushroom_()
{
    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo =
        CC_DATA::GetArtsOptionMdl_CreateInfo(GetChrIdS(), CGame::g_GameWork.GetGameMode());
    if (!pInfo) {
        return;
    }
    nw4r::g3d::ResFile mdl = GetChrOptResFile();
    if (!mdl.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile mot = GetChrOptMotResFile();

    bcs::FVEC4 rect;
    CStageMan::GetInstance()->GetTeamCourtRect(GetTargetCourtIdx(), &rect.x, &rect.y, &rect.z, &rect.w);
    if (rect.x > rect.y) {
        Swap(rect.x, rect.y);
    }
    if (rect.z > rect.w) {
        Swap(rect.z, rect.w);
    }
    float fParts = 4.0f;
    float fStep = (rect.w - rect.z) / fParts;
    bcs::FVEC3 pos(0.5f * (rect.x + rect.y), 0.0f, rect.z + fStep);

    long nCourt = GetTargetCourtIdx();
    bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
    for (long i = 0; i < 2; i++) {
        float fScale = 0.5f * (i & 1) + 0.25f;
        long nPlayer;
        if (i == 0) {
            nPlayer = GetPlayerNo();
        } else {
            nPlayer = -1;
        }
        new (0x100) CC09_VOLLEY::CcKinopioBoundMushroom(CGame::GetTempParent(), mdl, mot, pInfo, pos, fScale, nCourt,
                                                        color, nPlayer);
        pos.z += 2.0f * fStep;
    }
}

// Puts the receiver where the ball will land, or as far as the parameter
// block allows towards it, kept inside the court.
void msw::CChrCoreVolleyKinopio::FireReceiver_()
{
    ReleaseReceiver_();

    bcs::FVEC3 pos(GetPos());
    CBall *pBall = GetTargetBall_();
    if (pBall) {
        pBall->GetPosAfterFrame(pos, pBall->GetTouchDownFrame(pBall->GetBgCollRadius()));
        pos.y = 0.0f;
    }

    bcs::FVEC3 dir(0.0f, 0.0f, 0.0f);
    const bcs::FVEC3 &rChrPos = GetPos();
    dir.x = pos.x - rChrPos.x;
    dir.z = pos.z - rChrPos.z;
    float fMaxDist = GetCcParam_()->m_UnkDC;
    if (dir.x * dir.x + dir.z * dir.z > fMaxDist * fMaxDist) {
        bcs::BCS_Vec_Normalize(dir, bcs::FVEC3(dir.x, 0.0f, dir.z));
        dir.x *= fMaxDist;
        dir.z *= fMaxDist;
        bcs::FVEC3Add(&pos, &GetPos(), &dir);
        pos.y = 0.0f;
    }

    long nCourt = GetGuardCourtIdx();
    bcs::FVEC3 hitPos(pos);
    bcs::FVEC3 hitNormal;
    float fHitTime;
    bcs::FVEC3 from(pos);
    if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, from, from, 0.2f, 1,
                               (unsigned char)nCourt | 0x2000000)) {
        pos.x = hitPos.x;
        pos.z = hitPos.z;
        pos.y = 0.0f;
    }

    bcs::BCS_Color color = CC_DATA::GetChrOptColor_Kinopio(m_nColor);
    Eff_CreateSimple(0x680, pos, GetRot(), 3, 0, 0);
    CC09_VOLLEY::CcKinopioReceiver *pReceiver =
        new CC09_VOLLEY::CcKinopioReceiver(CGame::GetTempParent(), 0x5C6, pos, GetRot(), GetGuardCourtIdx(), color);
    if (pReceiver) {
        m_hReceiver = pReceiver->GetId();
    }
}

msw::CC09_VOLLEY::CcKinopioReceiver::CcKinopioReceiver(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos,
                                                       const bcs::FVEC3 &rot, long nCourt, bcs::BCS_Color color)
    : CGrdMdlEffect(pParent, nEff, pos, rot), m_BodyColl(1, 5, 2, 0)
{
    // Not used in this build. A guess at where the original had it: the
    // value is in .rodata between those of the functions before and after.
    float fUnused = 0.23f;

    m_fRadius = 0.01f;
    m_nCourt = nCourt;
    m_fSeTimer = 56.0f;
    m_BodyColl.SetTargetFlagAll(0x22);
    m_BodyColl.SetPushRankAll(3);
    m_BodyColl.SetPushPowerAll(5);
    m_BodyColl.SetPosAll(pos);
    m_BodyColl.SetSizeAll(m_fRadius, 1.5f, m_fRadius);
    m_BodyColl.SetBodyPushCallbackAll(CallbackBodyPush, this);
    m_BodyColl.SetPushEnableAll();
    m_BodyColl.SetEnableAll();

    // The effect's models take the colour of Toad's variant.
    if (m_hEffect) {
        CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
        if (pCluster) {
            long nMdl = pCluster->GetMdlObjNum();
            for (long i = 0; i < nMdl; i++) {
                bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                if (pMdl) {
                    pMdl->SetMatTevKColorRGB(GX_KCOLOR0, color);
                }
            }
        }
    }

    CGameSnd::Play(0x4C1, pos, 1, -1, 0.0f, 1.0f, 0.0f);
}

msw::CC09_VOLLEY::CcKinopioReceiver::~CcKinopioReceiver()
{
}

// Grows the collision to its full size, and keeps it a little below the
// effect. A second sound plays 56 frames after the first.
void msw::CC09_VOLLEY::CcKinopioReceiver::Main()
{
    CGrdMdlEffect::Main();

    if (!IsDelete()) {
        if (m_fSeTimer > 0.0f) {
            m_fSeTimer -= GetFrameRateF();
            if (m_fSeTimer <= 0.0f) {
                CGameSnd::Play(0x4C3, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                m_fSeTimer = 0.0f;
            }
        }
        m_fRadius += 0.05f * GetFrameRateF();
        m_fRadius = Clamp(m_fRadius, 0.01f, 0.5f);
        m_BodyColl.SetSizeAll(m_fRadius, 1.5f, m_fRadius);
        bcs::FVEC3 pos(m_Pos);
        pos.y -= 0.5f;
        m_BodyColl.SetPosAll(pos);
    }
}

void msw::CC09_VOLLEY::CcKinopioReceiver::CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                                            bcs::FVEC3 *pPush)
{
    if (pArg) {
        static_cast<CcKinopioReceiver *>(pArg)->CallbackBodyPush_(pBody, pOther, pPush);
    }
}

// Pushed by a body: moves along the push as far as the court allows, and
// passes what is left of the push back to the other body.
void msw::CC09_VOLLEY::CcKinopioReceiver::CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther,
                                                             bcs::FVEC3 *pPush)
{
    if (pPush) {
        bcs::FVEC3 from(m_Pos);
        bcs::FVEC3 to;
        to.x = from.x + pPush->x;
        to.z = from.z + pPush->z;
        to.y = from.y;
        bcs::FVEC3 hitPos;
        bcs::FVEC3 hitNormal;
        float fHitTime;
        if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, from, to, m_fRadius, 5,
                                   (unsigned char)m_nCourt | 0x2000000)) {
            hitNormal.x = hitPos.x - to.x;
            hitNormal.z = hitPos.z - to.z;
            pOther->DoBodyPushCallback(pBody, &hitNormal);
        } else {
            pOther->DoBodyPushCallback(pBody, 0);
        }
        m_Pos = hitPos;
        to.x = m_Pos.x - from.x;
        to.z = m_Pos.z - from.z;
        m_BodyColl.AddPosXZAll(to, to);
        m_BodyColl.CalcAABBAll();
    }
}

msw::CC09_VOLLEY::CcKinopioBoundMushroom::~CcKinopioBoundMushroom()
{
}

void msw::CC09_VOLLEY::CcKinopioBoundMushroom::SetAppearEffect_()
{
    CEffectCtrl::SimpleEffect(0x5C3, m_Pos, 3, 0, 0);
}

void msw::CC09_VOLLEY::CcKinopioBoundMushroom::SetDisappearEffect_()
{
    CEffectCtrl::SimpleEffect(0x5C4, m_Pos, 3, 0, 0);
}

// The swing takes 360 frames, the turn 120; the mushroom spins 4 degrees a
// frame.
void msw::CC09_VOLLEY::CcKinopioBoundMushroom::MovePos_(float fFrame)
{
    float fSwingFrame = 360.0f;
    m_fSwing += (1.0f / Max(fSwingFrame, 1.0f)) * fFrame;
    m_fSwing = fmod(m_fSwing, 1.0);
    float fTurnFrame = 120.0f;
    m_fTurn += fFrame * (1.0f / Max(fTurnFrame, 1.0f));
    m_fTurn = fmod(m_fTurn, 1.0);
    float fSpin = 4.0f;
    m_fRotY += fFrame * DegToRad(fSpin);
    m_fRotY = WrapRad(m_fRotY);
}

// Swings 3 to each side of its point and turns round it at a distance of 2,
// kept inside the court.
void msw::CC09_VOLLEY::CcKinopioBoundMushroom::CalcPos_()
{
    bcs::FVEC3 pos(m_BasePos);
    if (m_nCourt >= 0) {
        bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
        bcs::FVEC3Add(&pos, &pos, &ofs);
    }

    float fSwing = 3.0f;
    pos.x += fSwing * nw4r::math::SinRad(WrapRad(6.2831855f * m_fSwing));

    float fTurn = WrapRad(6.2831855f * m_fTurn);
    float fTurnRadius = 2.0f;
    if (fTurnRadius > 0.0f) {
        bcs::FVEC3 turn;
        bcs::BCS_VecZ_RotY(turn, fTurnRadius, fTurn);
        pos.x += turn.x;
        pos.z += turn.z;
    }

    m_Pos = pos;
    if (m_nCourt >= 0) {
        bcs::FVEC3 from(pos);
        bcs::FVEC3 hitNormal;
        float fHitTime;
        if (CCollMan::DetectMapHit(pos, hitNormal, fHitTime, from, from, 0.8f * m_fRadius, 1,
                                   (unsigned char)m_nCourt | 0x2000000)) {
            m_Pos = pos;
        }
    }
}

float msw::CC09_VOLLEY::CcKinopioBoundMushroom::GetParamScale_() const
{
    return 1.3f;
}
