// msw::CChrCoreVolleyWario (Wario) in the volleyball game, and his special
// shot's smoke bomb (CC05_VOLLEY::CcWarioSmokeBomb) with the cloud of smoke
// it leaves (CC05_VOLLEY::CcWarioSmokeMgr and its CcWarioSmokeUnit puffs).
//
// In his air trick Wario jumps to the back of his court with the bomb in
// the hand of his option model; at the end of the trick motion he throws it
// at the middle of the other court and jumps again (motion 0x76), 2 further
// back. Where the bomb lands a cloud of smoke grows, as in the other
// modules (game/dodge/CcWarioSmokeBomb.cpp, game/basket/CcWarioSmokeMgr.cpp):
// every frame a ring of puffs, 1.25 further out than the last, until the
// rings pass 7. It only hides the court. When told to go (messages 0xD and
// 0xE) the puffs fade out.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CScnGroup.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CChrOptMdlObj.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGameTask.hpp"
#include "game/msw/CStageMan.hpp"
#include "nw4r/math/triangular.h"
#include "nw4r/ut/List.hpp"
#include <math.h>

namespace {

    // The three smoke effects a puff picks from.
    const long s_SmokeEffect[3] = {0x594, 0x595, 0x596};

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (guessed helpers) An angle brought into [0, 2 pi); the larger and the
    // smaller of two values; fValue kept within [fMin, fMax]; degrees to radians.
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

    inline float Max(float a, float b)
    {
        return a < b ? b : a;
    }

    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

    inline float Clamp(float fValue, float fMin, float fMax)
    {
        return fValue > fMax ? fMax : (fValue < fMin ? fMin : fValue);
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    // (guessed helpers) The squared distance of two points seen from
    // above, and the distance; rIn flattened onto the XZ plane and scaled to length 1.
    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }

    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    inline void NormalizeXZ(bcs::FVEC3 &rOut, const bcs::FVEC3 &rIn)
    {
        bcs::BCS_Vec_Normalize(rOut, bcs::FVEC3(rIn.x, 0.0f, rIn.z));
    }
}

namespace msw {
namespace CC05_VOLLEY {

    class CcWarioSmokeMgr;

    // The bomb: the option model, thrown on a parabola to a point on the
    // court; an obstacle of type 4. 0xC8 bytes. (member names guessed)
    class CcWarioSmokeBomb : public CChrObstacle, public CChrOptMdlObj {
        public:
            CcWarioSmokeBomb(bcs::CTask *pParent, CChrCore *pChr, nw4r::g3d::ResFile mdlFile,
                             nw4r::g3d::ResFile motFile, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                             const bcs::FVEC3 &rPos, const bcs::QUATERNION &rQuat, const bcs::FVEC3 &rTarget);
            virtual ~CcWarioSmokeBomb();
            virtual void Main();
            // One frame of the flight (the frame rate is read again).
            virtual void MainFunc(float fRate);
            // The model at the bomb's place and turn.
            virtual void RecalcMtx();

        private:
            CChrCore *m_pChr;       // 0xB0, the thrower
            bcs::FVEC3 m_Axis;           // 0xB4, the bomb tumbles about it
            long m_nStep;                // 0xC0: 0 flying, then 3 frames, then gone
            unsigned long m_hEffect;     // 0xC4, the fuse's effect
    };

    // One puff of smoke: an effect in the cloud's second scene group that
    // rises a little while it fades in and fades out when told. 0x88 bytes,
    // kept in its cloud's list. (member names guessed)
    class CcWarioSmokeUnit : public CGameTask {
        public:
            CcWarioSmokeUnit(CcWarioSmokeMgr *pMgr, bcs::CScnGroup *pGroup, bcs::CScnGroup *pMdlGroup,
                             const bcs::FVEC3 &rPos);
            virtual ~CcWarioSmokeUnit();
            virtual void Main();

            // Starts fading out, unless it already does.
            void Vanish()
            {
                if (m_nStep < STEP_VANISH) {
                    m_nStep = STEP_VANISH;
                }
            }

        private:
            enum {
                STEP_START,
                STEP_APPEAR, // fades in and rises
                STEP_LOOP,
                STEP_VANISH,
                STEP_DISAPPEAR, // fades out and swells
            };

            void CreateMdl_(const bcs::FVEC3 &rPos);

            nw4r::ut::Link m_Link;          // 0x34
            CcWarioSmokeMgr *m_pMgr;        // 0x3C
            unsigned long m_hEffect;        // 0x40
            long m_nStep;                   // 0x44
            float m_fRot;                   // 0x48, about Z
            float m_fRotSpeed;              // 0x4C, per frame
            float m_fScale;                 // 0x50
            float m_fScaleAdd;              // 0x54, grows as it fades out
            float m_fRate;                  // 0x58, its alpha (0 to 1)
            bcs::FVEC3 m_StartPos;          // 0x5C, local to the cloud
            bcs::FVEC3 m_EndPos;            // 0x68
            bcs::FVEC3 m_Pos;               // 0x74
            bcs::CScnGroup *m_pGroup;       // 0x80 (unused)
            bcs::CScnGroup *m_pMdlGroup;    // 0x84, holds the models
    };

    // The cloud: an obstacle of type 4 with two scene groups and the list
    // of puffs. 0xC0 bytes. (member names guessed)
    class CcWarioSmokeMgr : public CChrObstacle {
        public:
            enum {
                STEP_APPEAR, // the rings grow
                STEP_LOOP,
                STEP_DISAPPEAR,
            };

            CcWarioSmokeMgr(bcs::CTask *pParent, long nCourt, const bcs::FVEC3 &rPos);
            virtual ~CcWarioSmokeMgr();
            virtual void Main();
            virtual long SendMessage(long nMsg, long, long, long);

            void Append(CcWarioSmokeUnit *pUnit) { nw4r::ut::List_Append(&m_List, pUnit); }
            void Remove(CcWarioSmokeUnit *pUnit) { nw4r::ut::List_Remove(&m_List, pUnit); }

        private:
            CcWarioSmokeUnit *GetFirst()
            {
                return static_cast<CcWarioSmokeUnit *>(nw4r::ut::List_GetNext(&m_List, 0));
            }
            CcWarioSmokeUnit *GetNext(CcWarioSmokeUnit *pUnit)
            {
                if (pUnit) {
                    return static_cast<CcWarioSmokeUnit *>(nw4r::ut::List_GetNext(&m_List, pUnit));
                }
                return 0;
            }

            nw4r::ut::List m_List;       // 0x94, of CcWarioSmokeUnit
            bcs::CScnGroup *m_pGroup;    // 0xA0
            bcs::CScnGroup *m_pMdlGroup; // 0xA4, holds the puffs' models
            long m_nCourt;               // 0xA8, the court it is in
            long m_nStep;                // 0xAC
            float m_fRadius;             // 0xB0, of the next ring
            float m_fRingWait;           // 0xB4, frames to the next ring
            float m_fStartRot;           // 0xB8, where the next ring starts
            int m_bVanish;               // 0xBC, set by SendMessage
    };
}
}

msw::CChrCoreVolleyWario::CChrCoreVolleyWario()
{
}

msw::CChrCoreVolleyWario::~CChrCoreVolleyWario()
{
}

// The trick jump goes to the back line of his court half (kept between its
// sides), up to 3 and down to 3.5, over the trick motion; the bomb's model
// fades in.
void msw::CChrCoreVolleyWario::SubStateAirTrickStartTrick_(STATE_WORK_AIR_TRICK *pWork)
{
    // (guessed) Dead comparison: the original creates 0.0f before this
    // function's other constants (notes/decisions.md); it does nothing.
    if (pWork->m_fTrickFrame == 0.0f) {
    }

    pWork->m_TargetPos = GetPos();
    SubStateAirTrickPlayTrickMotion_(pWork);
    ArtsOptCreate(1, 1, 1);
    ArtsOptSetFadeInSE(0x419);
    ArtsOptFadeIn(5.0f, 70.0f, 1);

    float fMotFrame = Max(1.0f, SubStateAirTrickGetTrickMotFrame_(pWork, 0));
    pWork->m_fAnimSpeed = 1.0f;
    pWork->m_fAdsorbUnk60 = fMotFrame;

    pWork->m_TargetPos = GetPos();
    bcs::FVEC4 rect;
    Util_GetGuardCourtRect_(rect);
    float &rX = pWork->m_TargetPos.x;
    rX = rX > rect.y ? rect.y : (rX < rect.x ? rect.x : rX);
    if (GetGuardGoalIdx()) {
        pWork->m_TargetPos.z = rect.z;
    } else {
        pWork->m_TargetPos.z = rect.w;
    }

    // (as in the original, the trick has no set length here, so the trick
    // motion's length is used at normal speed)
    float fFrame = 0.0f;
    if (fFrame <= 0.0f) {
        fFrame = fMotFrame;
    } else {
        pWork->m_fAnimSpeed = fMotFrame / fFrame;
        AnimSetSpeed(3, pWork->m_fAnimSpeed);
    }
    pWork->m_fTrickCount = 0.0f;
    pWork->m_fTrickFrame = fFrame;
    pWork->m_fJumpUnk = GetCcParam_()->m_Unk388 + GetCcParam_()->m_Unk38C;
    Util_SetJumpParam(fFrame, GetPosY(), 3.0f, 3.5f);
    m_fJumpUnk224 = 0.0f;
    m_fJumpUnk228 = 0.0f;

    ClearMoveParamWork_(pWork->m_MoveParam);
    Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
    pWork->m_fUnk20 = 0.0f;
    pWork->m_MoveParam.m_nType = 0xB;
    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
    pWork->m_MoveParam.m_fFrame = fFrame > 10.0f ? 5.0f : 0.0f;
    pWork->m_MoveParam.m_fFrameMax = fFrame;
    CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, 16.0f);

    CBall *pBall = GetTargetBall_();
    if (pBall) {
        SubStateAirTrickStartBallAdsorb_(pWork, pBall);
        pBall->TrgVolleyArtsAttach(m_nTeamNo, m_nPlayerNo, m_nChrId, 1);
    }
    pWork->m_nTrickStep = 0;
}

float msw::CChrCoreVolleyWario::SubStateAirTrickGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long)
{
    return 0.0f;
}

void msw::CChrCoreVolleyWario::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);
    Eff_CreateAttachAuto(0x13, 0x73, 3);
}

int msw::CChrCoreVolleyWario::SubStateAirTrickIsTMStartAuto_(STATE_WORK_AIR_TRICK *)
{
    return 0;
}

// Step 0: his voice at frame 70 of the trick motion. At its end the bomb is
// thrown and the second jump (motion 0x76) starts, 2 further back, turning
// over three quarters of it. The shot is ready when that jump is over.
int msw::CChrCoreVolleyWario::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    static const float fDist = 2.0f;
    int bReady = 0;

    switch (pWork->m_nTrickStep) {
        case 0:
            if (IsAnimEnd(3) || AnimGetFrame(3) >= 70.0f) {
                PlayVoice(3, 0);
                pWork->m_nTrickStep++;
            }
            // fall through
        case 1:
            SubStateAirTrickDoJumpMove_(pWork);
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0x76, 0, 4.0f, 0.0f);
                Eff_CreateAttachAuto(0x14, 0x74, 3);
                ThrowSmokeBomb_();
                ArtsOptDestroy();

                // (as in the trick start, no set length)
                float fMotFrame = AnimGetFrameNum(3);
                fMotFrame = Max(1.0f, fMotFrame - GetCcParam_()->m_Unk384);
                float fFrame = 0.0f;
                pWork->m_fAnimSpeed = 1.0f;
                if (fFrame <= 0.0f) {
                    fFrame = fMotFrame;
                } else {
                    pWork->m_fAnimSpeed = fMotFrame / fFrame;
                    AnimSetSpeed(3, pWork->m_fAnimSpeed);
                }

                pWork->m_TargetPos = GetPos();
                pWork->m_TargetPos.z += GetGuardGoalIdx() ? -fDist : fDist;
                pWork->m_fTrickFrame = fFrame;
                SubStateAirTrickSetTrickJumpParam_(pWork, 1);

                ClearMoveParamWork_(pWork->m_MoveParam);
                Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
                pWork->m_fUnk20 = 0.0f;
                pWork->m_MoveParam.m_nType = 0xB;
                float fMove = DistXZ(pWork->m_TargetPos, GetPos());
                pWork->m_MoveParam.m_fFrame = 0.0f;
                pWork->m_MoveParam.m_fDist = -1.0f * fMove;
                pWork->m_MoveParam.m_fFrameMax = fFrame;
                pWork->m_fTrickCount = 0.0f;
                pWork->m_fTrickFrame = fFrame;
                CalcRotParam_(pWork->m_RotLerp, pWork->m_fRotY, 0.75f * fFrame);
                pWork->m_nTrickStep++;
            }
            break;
        default:
            pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
            pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
            SubStateAirTrickDoJumpMove_(pWork);
            if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
                bReady = 1;
            }
            break;
    }

    SubStateAirTrickDoBallAdsorb_(pWork);
    if (!CChrMan::IsArtsChrStop()) {
        SubStateAirTrickStartTMDisp_(pWork);
    }
    return bReady;
}

float msw::CChrCoreVolleyWario::SubStateAirTrickGetBallAdsorbFrame_(STATE_WORK_AIR_TRICK *)
{
    return AnimGetResFrameNum(0x75) + AnimGetResFrameNum(0x76);
}

void msw::CChrCoreVolleyWario::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);
    Eff_CreateAttachAuto(0x15, 0x75, 3);
}

// The bomb leaves from node 1 of the option model (Wario's position when
// there is none), turned as the node is, for the middle of the court he
// attacks.
void msw::CChrCoreVolleyWario::ThrowSmokeBomb_()
{
    bcs::FVEC3 pos(GetPos());
    bcs::QUATERNION quat;
    quat.x = quat.y = quat.z = 0.0f;
    quat.w = 1.0f;

    bcs::CMdlObj *pMdl = GetArtsOptMdl();
    if (pMdl) {
        bcs::FMTX34 mtx;
        pMdl->GetNodeWorldMtx(1, mtx);
        pos.Set(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
        bcs::BCS_Quat_FromMatrix(quat, mtx);
    }

    const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo =
        CC_DATA::GetArtsOptionMdl_CreateInfo(GetChrIdS(), CGame::g_GameWork.GetGameMode());
    if (!pInfo) {
        return;
    }
    nw4r::g3d::ResFile file = GetChrOptResFile();
    if (!file.IsValid()) {
        return;
    }
    nw4r::g3d::ResFile motFile = GetChrOptMotResFile();

    bcs::FVEC3 target;
    bcs::FVEC4 rect;
    Util_GetTargetCourtRect_(rect);
    target.x = 0.5f * (rect.x + rect.y);
    target.y = 0.0f;
    target.z = 0.5f * (rect.z + rect.w);

    new (0x100) CC05_VOLLEY::CcWarioSmokeBomb(CGame::GetTempParent(), this, file, motFile, pInfo, pos, quat, target);
    PlaySePos(0x41A, pos, 0, -1, 0.0f, 1.0f, 0.0f);
}

namespace msw {
namespace CC05_VOLLEY {

    // The bomb at rPos turned by rQuat, for rTarget: it gets there in 30
    // frames on a parabola, tumbling about the horizontal axis across
    // its way.
    CcWarioSmokeBomb::CcWarioSmokeBomb(bcs::CTask *pParent, CChrCore *pChr, nw4r::g3d::ResFile mdlFile,
                                       nw4r::g3d::ResFile motFile,
                                       const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo, const bcs::FVEC3 &rPos,
                                       const bcs::QUATERNION &rQuat, const bcs::FVEC3 &rTarget)
        : CChrObstacle(pParent, 4, 0x17)
    {
        m_pChr = pChr;
        m_nStep = 0;
        m_hEffect = 0;

        nw4r::g3d::ResMdl mdl = mdlFile.GetResMdl(pInfo->m_pMdlName);
        CreateObj(mdl);
        CreateAnm(motFile, pInfo->m_pNodeExInfo);
        bcs::CMdlObj *pMdl = m_pMdl;
        pMdl->OffFlag(0x4000);
        if (pMdl && pInfo->m_pShadowName) {
            CreateShadow(mdlFile.GetResMdl(pInfo->m_pShadowName), pInfo->m_nShadowType);
        }
        if (pMdl && pInfo->m_pRefName) {
            // (as in the original, the reflection is made from the main
            // model, not from the one looked up)
            mdlFile.GetResMdl(pInfo->m_pRefName);
            CreateRef(mdl);
        }

        long nFrame = 30;
        float fFrame = nFrame;
        pMdl->m_vTrans = rPos;
        m_Pos = rPos;
        m_Quat = rQuat;
        m_Velocity.Set(0.0f, 0.0f, 0.0f);
        fFrame = Max(1.0f, fFrame);
        float fEndY = 0.5f + rTarget.y;
        float fTopY = Max(m_Pos.y, rPos.y);

        bcs::FVEC3 move;
        move.x = rTarget.x - rPos.x;
        move.z = rTarget.z - rPos.z;
        if (move.x * move.x + move.z * move.z > 0.0f) {
            float fDist = bcs::BCS_Vec_GetLenXZ(move);
            NormalizeXZ(m_Velocity, move);
            m_Velocity.x *= fDist / fFrame;
            m_Velocity.z *= fDist / fFrame;
        }

        bcs::FVEC3 dir(m_Velocity);
        NormalizeXZ(dir, dir);
        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        PSVECCrossProduct((const Vec *)&up, (const Vec *)&dir, (Vec *)&m_Axis);
        bcs::BCS_Phys_CalcYVG(fFrame, rPos.y, fEndY, fTopY, m_Velocity.y, m_fGravity);

        RecalcMtx();
        FadeIn_(0.0f);
        m_hEffect = CEffectCtrl::CreateAttachEffect(5, m_pMdl, 0, GetNodeIdx("SP_item_01", 0), 0, 0, 0, 3, 0,
                                                    0, 0);
    }

    CcWarioSmokeBomb::~CcWarioSmokeBomb()
    {
        if (m_hEffect) {
            CEffectCtrl::StopAttachEffect(m_hEffect);
            m_hEffect = 0;
        }
    }

    void CcWarioSmokeBomb::Main()
    {
        float fRate = GetFrameRateF();
        MainFunc(fRate);
        UpdateFade_(fRate);
        RecalcMtx();
    }

    // In flight the bomb falls and tumbles 2 degrees a frame. Where it
    // reaches the ground the camera shakes, the model turns red, and the
    // cloud of smoke starts there; three frames later the bomb goes.
    void CcWarioSmokeBomb::MainFunc(float)
    {
        switch (m_nStep) {
            case 0: {
                int bLand = 0;
                bcs::FVEC3 oldPos(m_Pos);
                float fRate = GetFrameRateF();
                float fGravity = m_fGravity * fRate;
                bcs::FVEC3 move;

                bcs::FVEC3Scale(&move, &m_Velocity, fRate);
                move.y += 0.5f * (fGravity * fRate);
                m_Velocity.y += fGravity;
                bcs::FVEC3Add(&m_Pos, &m_Pos, &move);
                if (m_Pos.y <= 0.5f) {
                    m_Pos.y = 0.5f;
                    bLand = 1;
                } else {
                    float fSin;
                    float fCos;
                    nw4r::math::SinCosRad(&fSin, &fCos, 0.5f * (DegToRad(2.0f) * GetFrameRateF()));
                    bcs::QUATERNION rot;
                    rot.x = m_Axis.x * fSin;
                    rot.y = m_Axis.y * fSin;
                    rot.z = m_Axis.z * fSin;
                    rot.w = fCos;
                    PSQUATMultiply((const Quaternion *)&rot, (const Quaternion *)&m_Quat, (Quaternion *)&m_Quat);
                    PSQUATNormalize((const Quaternion *)&m_Quat, (Quaternion *)&m_Quat);
                }

                if (bLand) {
                    CGame::StartCameraQuake(0x12, -1, -1);
                    if (m_pMdl) {
                        m_pMdl->SetSilhouetteColor(bcs::BCS_Color(0x00000000UL), bcs::BCS_Color(0xFF0000FFUL));
                    }
                    CGameSnd::Play(0x41C, m_Pos, 1, -1, 0.0f, 1.0f, 0.0f);
                    CEffectCtrl::SimpleEffect(0x58C, m_Pos, 3, 0, 0);
                    bcs::FVEC3 groundPos(m_Pos.x, 0.0f, m_Pos.z);
                    CEffectCtrl::SimpleEffect(0x58D, groundPos, 3, 0, 0);
                    new (0x100) CcWarioSmokeMgr(CGame::GetTempParent(), m_pChr->GetTargetCourtIdx(), groundPos);
                    m_nStep++;
                }
                break;
            }
            case 1:
            case 2:
            case 3:
                m_nStep++;
                break;
            default:
                Delete();
                break;
        }
    }

    void CcWarioSmokeBomb::RecalcMtx()
    {
        bcs::CMdlObj *pMdl = m_pMdl;
        if (pMdl) {
            pMdl->m_vTrans = m_Pos;
            bcs::FMTX34 mtx;
            PSMTXQuat(mtx.m, (const Quaternion *)&m_Quat);
            mtx.m[0][3] = m_Pos.x;
            mtx.m[1][3] = m_Pos.y;
            mtx.m[2][3] = m_Pos.z;
            pMdl->SetMtx(bcs::CScnObj::SCNOBJMTXTYPE_ROOT, mtx);
            pMdl->CalcWorldMatrixAll(0);
        }
    }

    // A puff at rPos (local to the cloud).
    CcWarioSmokeUnit::CcWarioSmokeUnit(CcWarioSmokeMgr *pMgr, bcs::CScnGroup *pGroup,
                                       bcs::CScnGroup *pMdlGroup, const bcs::FVEC3 &rPos)
        : CGameTask(pMgr, -1)
    {
        m_pMgr = pMgr;
        m_hEffect = 0;
        m_nStep = STEP_START;
        m_fRot = 0.0f;
        m_fRotSpeed = 0.0f;
        m_fScale = 1.0f;
        m_fScaleAdd = 0.0f;
        m_fRate = 0.0f;
        m_pGroup = pGroup;
        m_pMdlGroup = pMdlGroup;
        m_Link.pPrev = 0;
        m_Link.pNext = 0;
        if (pMgr) {
            pMgr->Append(this);
        }
        CreateMdl_(rPos);
    }

    CcWarioSmokeUnit::~CcWarioSmokeUnit()
    {
        if (m_pMgr) {
            m_pMgr->Remove(this);
        }
        if (m_hEffect) {
            CEffectCtrl::EraseAttachEffect(m_hEffect);
        }
    }

    // One of the three smoke effects, hidden until it fades in, about
    // 2.8 big and turning slowly one way or the other; it rises from a
    // little above rPos to a little higher.
    void CcWarioSmokeUnit::CreateMdl_(const bcs::FVEC3 &rPos)
    {
        long nEffect = s_SmokeEffect[bcs::BCS_RandMax(2L)];
        if (nEffect >= 0) {
            m_hEffect = CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0);
        }
        m_Pos.Set(0.0f, 0.0f, 0.0f);
        m_StartPos.Set(0.0f, 0.0f, 0.0f);
        m_EndPos = rPos;

        if (m_hEffect) {
            CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
            if (pCluster) {
                m_fRot = WrapRad(bcs::BCS_RandMax(PI2));
                m_fRotSpeed = bcs::BCS_RandMax(1L) ? DegToRad(-0.1f) : DegToRad(0.1f);
                m_fScale = 2.8f;
                m_fScale += m_fScale * bcs::BCS_RandRange(-0.1f, 0.1f);
                m_StartPos.y += 0.1f * m_fScale;
                m_EndPos.y += 0.4f * m_fScale;

                bcs::FVEC3 rot(0.0f, 0.0f, m_fRot);
                bcs::FVEC3 scale(m_fScale, m_fScale, m_fScale);
                pCluster->SetTranslate(&m_StartPos);
                pCluster->SetRotateRad(&rot);

                long nNum = pCluster->GetMdlObjNum();
                for (long i = 0; i < nNum; i++) {
                    bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                    if (pMdl) {
                        pMdl->SetExBlendMode(0);
                        pMdl->SetHide(1);
                        pMdl->SetAlphaRateF(0.0f);
                        pMdl->m_vScale = scale;
                        pMdl->DetachFromParent();
                        m_pMdlGroup->AppendChild(pMdl);
                    }
                }
            }
        }
    }

    void CcWarioSmokeUnit::Main()
    {
        float fRate = GetFrameRateF();
        m_fRot += m_fRotSpeed * fRate;
        m_fRot = WrapRad(m_fRot);

        switch (m_nStep) {
            case STEP_START:
                m_nStep++;
                break;
            case STEP_APPEAR: {
                m_fRate += 0.0625f * fRate;
                float fMove = 1.0f - (float)pow(1.0f - Clamp(m_fRate, 0.0f, 1.0f), 2.2f);
                bcs::BCS_Vec_Lerp(m_Pos, m_StartPos, m_EndPos, fMove);
                if (m_fRate >= 1.0f) {
                    m_fRate = 1.0f;
                    m_nStep++;
                }
                break;
            }
            case STEP_LOOP:
                break;
            case STEP_VANISH:
                m_nStep++;
                // fall through
            case STEP_DISAPPEAR:
                m_fRate -= (1.0f / 15.0f) * fRate;
                if (m_fRate <= 0.0f) {
                    m_fRate = 0.0f;
                    m_nStep++;
                }
                m_fScaleAdd = 0.2f * (1.0f - m_fRate) * m_fScale;
                break;
            default:
                Delete();
                break;
        }

        if (m_hEffect) {
            CEffectCluster *pCluster = CEffectCtrl::GetObject(m_hEffect);
            if (pCluster) {
                bcs::FVEC3 rot(0.0f, 0.0f, m_fRot);
                pCluster->SetTranslate(&m_Pos);
                pCluster->SetRotateRad(&rot);

                float fScale = m_fScale + m_fScaleAdd;
                bcs::FVEC3 scale(fScale, fScale, fScale);
                long nNum = pCluster->GetMdlObjNum();
                for (long i = 0; i < nNum; i++) {
                    bcs::CMdlObj *pMdl = pCluster->GetMdlObj(i);
                    if (pMdl && m_fRate > 0.0f) {
                        pMdl->SetHide(0);
                        pMdl->m_vTrans = m_Pos;
                        pMdl->m_vScale = scale;
                        pMdl->SetAlphaRateF(m_fRate);
                    }
                }
            }
        }
    }

    // The cloud at rPos in court nCourt.
    CcWarioSmokeMgr::CcWarioSmokeMgr(bcs::CTask *pParent, long nCourt, const bcs::FVEC3 &rPos)
        : CChrObstacle(pParent, 4, 0x17), m_nCourt(nCourt), m_nStep(STEP_APPEAR), m_fRadius(0.0f),
          m_fRingWait(0.0f), m_fStartRot(0.0f), m_bVanish(0)
    {
        m_Pos = rPos;
        bcs::FVEC3 ofs = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
        m_Pos.x -= ofs.x;
        m_Pos.z -= ofs.z;

        m_pGroup = new (0x100) bcs::CScnGroup(0x10, 0);
        m_pGroup->m_vTrans = rPos;
        m_pGroup->CalcRootMtx();
        bcs::CRenderMan::PushBack(m_pGroup, 5);

        m_pMdlGroup = new (0x100) bcs::CScnGroup(0x10, 0);
        m_pMdlGroup->m_vTrans = rPos;
        m_pMdlGroup->CalcRootMtx();
        bcs::CRenderMan::PushBack(m_pMdlGroup, 2);

        nw4r::ut::List_Init(&m_List, 0x34);

        // The first ring grows in the next frame.
        m_fRingWait = 1.0f - GetFrameRateF();
    }

    CcWarioSmokeMgr::~CcWarioSmokeMgr()
    {
        DeleteAllChild();
        nw4r::ut::List_Init(&m_List, 0x34);
        if (m_pGroup) {
            delete m_pGroup;
        }
        m_pGroup = 0;
        if (m_pMdlGroup) {
            delete m_pMdlGroup;
        }
        m_pMdlGroup = 0;
    }

    void CcWarioSmokeMgr::Main()
    {
        // (the names are the original's, taken over from Daisy's flower
        // garden; static-order swap, notes/decisions.md: the game numbers
        // FLOWER_UNIT_RADIUS first and stores it second.)
        static const float FLOWER_COLL_RADIUS = 0.1f;
        static const float FLOWER_UNIT_RADIUS = 2.5f;

        // The groups follow the court.
        if (m_pGroup) {
            bcs::FVEC3 pos = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
            bcs::FVEC3Add(&pos, &pos, &m_Pos);
            m_pGroup->m_vTrans = pos;
            m_pGroup->CalcRootMtx();
        }
        if (m_pMdlGroup) {
            bcs::FVEC3 pos = CStageMan::GetInstance()->GetTeamCourtOffset(m_nCourt);
            bcs::FVEC3Add(&pos, &pos, &m_Pos);
            m_pMdlGroup->m_vTrans = pos;
            m_pMdlGroup->CalcRootMtx();
        }

        switch (m_nStep) {
            // Every frame a ring of puffs, 1.25 further out than the last and
            // starting half a step further round, with a puff about every
            // 1.75 along it, a little scattered; until the rings pass 7.
            // Puffs are left out where the court's edge would move them by
            // more than one unit.
            case STEP_APPEAR: {
                m_fRingWait += GetFrameRateF();
                unsigned long nHitFlag = (unsigned char)m_nCourt | 0x2000000;
                bcs::FVEC3 center(m_pGroup->m_vTrans);

                while (m_fRingWait > 0.0f) {
                    float fLength = PI * (m_fRadius * 2.0f);
                    long nNum = (long)(fLength * (1.0f / (FLOWER_UNIT_RADIUS * 0.7f))) + 1;
                    float fStep = PI2 / nNum;
                    float fRot = WrapRad(m_fStartRot);
                    // (as in the original, nothing sets it, so only the
                    // radius ends the growth)
                    int bNoRoom = 0;

                    for (long i = 0; i < nNum; i++) {
                        float fDist = m_fRadius + bcs::BCS_RandRange(-FLOWER_UNIT_RADIUS * 0.1f,
                                                                     FLOWER_UNIT_RADIUS * 0.1f);
                        float fAngle = WrapRad(fRot + bcs::BCS_RandRange(-0.3f * fStep, 0.3f * fStep));
                        bcs::FVEC3 pos;
                        int bCreate = 1;

                        bcs::BCS_VecZ_RotY(pos, fDist, fAngle);

                        bcs::FVEC3 hitPos(pos);
                        bcs::FVEC3 hitNormal;
                        float fHitTime;
                        bcs::FVEC3Add(&hitPos, &hitPos, &center);
                        if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitPos, hitPos,
                                                        FLOWER_COLL_RADIUS, 1, nHitFlag)) {
                            bcs::FVEC3Sub(&hitPos, &hitPos, &center);
                            if (DistSqXZ(pos, hitPos) <= 1.0f) {
                                pos = hitPos;
                            } else {
                                bCreate = 0;
                            }
                        }

                        if (bCreate) {
                            bNoRoom = 0;
                            new (0x100) CcWarioSmokeUnit(this, m_pGroup, m_pMdlGroup, pos);
                        }
                        fRot = WrapRad(fRot + fStep);
                    }

                    m_fStartRot = WrapRad(m_fStartRot + 0.5f * fStep);
                    m_fRadius += FLOWER_UNIT_RADIUS * 0.5f;
                    if (m_fRadius > 7.0f || bNoRoom) {
                        m_nStep++;
                        break;
                    }
                    m_fRingWait -= 1.0f;
                }
                break;
            }

            // Told to go, every puff fades out.
            case STEP_LOOP:
                if (m_bVanish) {
                    for (CcWarioSmokeUnit *pUnit = GetFirst(); pUnit; pUnit = GetNext(pUnit)) {
                        pUnit->Vanish();
                    }
                    m_nStep++;
                }
                break;

            // Ends once the last puff and model are gone.
            default:
                if (nw4r::ut::List_GetSize(&m_List) <= 0 && m_pGroup->GetChildNum() == 0 &&
                    m_pMdlGroup->GetChildNum() == 0) {
                    Delete();
                }
                break;
        }
    }

    // Messages 0xD and 0xE make the cloud go.
    long CcWarioSmokeMgr::SendMessage(long nMsg, long, long, long)
    {
        switch (nMsg) {
            case 0xD:
            case 0xE:
                m_bVanish = 1;
                break;
        }
        return 0;
    }
}
}
