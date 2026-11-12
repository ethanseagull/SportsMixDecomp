// msw::CChrCoreVolleyKoopaJr: Bowser Jr. in the volleyball game. His block
// is a jump with a paint effect in front of him; in his air trick he throws
// a spray of paint balls (CC11_VOLLEY::CcKoopaJrPaint) on rings that widen
// outwards around the middle of the other court.
//
// msw::CC11_VOLLEY::CcKoopaJrPaint: one paint ball. The base class
// (msw::CC_CMN::KOOPAJR::CcPaintBall, in main.dol) does the work; this class
// only gives its tuning values.
#include "game/msw/CChrCoreVolley.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CC_PARAM_VOLLEY.hpp"
#include "game/msw/CCollDefUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcPaintBall.hpp"

namespace msw {
    namespace CC11_VOLLEY {

        class CcKoopaJrPaint : public CC_CMN::KOOPAJR::CcPaintBall {
            public:
                CcKoopaJrPaint(bcs::CTask *pParent, long nTeam, int bFirst)
                    : CcPaintBall(pParent, nTeam, bFirst)
                {
                }
                virtual ~CcKoopaJrPaint() {}

            protected:
                virtual long GetEffectID_(long nKind) const
                {
                    switch (nKind) {
                        case 0:
                            return 0x5DD;
                        case 1:
                            return 0x5DC;
                        case 2:
                            return 0x5DB;
                    }
                    return -1;
                }
                virtual long GetBallFlyFrmMin_() const { return 55; }
                virtual long GetBallFlyFrmMax_() const { return 65; }
                virtual float GetBallFlyPassHeight_() const { return 6.2f; }
                virtual float GetBallDispScale_() const { return 1.0f; }
                virtual float GetBallAtkCollRadius_() const { return 0.2f; }
                virtual float GetPaintDispScale_() const { return 1.8f; }
                virtual float GetPaintAtkCollRadius_() const { return 1.1f; }
                virtual float GetPaintLifeFrame_() const { return 200.0f; }
                virtual float GetPaintFadeOutFrame_() const { return 20.0f; }
                virtual float GetPaintFadeOutScale_() const { return 1.2f; }
        };
    }
}

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (guessed helpers) The smaller of two values; an angle brought into
    // [0, 2 pi]; the distance and the square of the distance between two
    // points in the XZ plane.
    inline float Min(float fValue, float fMax)
    {
        return fValue > fMax ? fMax : fValue;
    }

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

    inline float DistXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3Dist(&pos1, &pos2);
    }

    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }
}

msw::CChrCoreVolleyKoopaJr::CChrCoreVolleyKoopaJr()
{
}

msw::CChrCoreVolleyKoopaJr::~CChrCoreVolleyKoopaJr()
{
}

// The block: a jump with its own motion and push settings.
void msw::CChrCoreVolleyKoopaJr::SubStateBlockJumpActStart_(STATE_WORK_BLOCK_JUMP *pWork)
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

// Not zero once the block jump has landed. From the motion's trigger frame
// the block is up (flag bit 1) for the time in the parameters, with the
// paint effect in front of him at the block's height.
int msw::CChrCoreVolleyKoopaJr::SubStateBlockJumpIsActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
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

    float fFrame = AnimGetFrame(3);
    if (fFrame >= pWork->m_fActTrgFrame + GetCcParam_()->m_Unk78) {
        if (pWork->m_nFlag & 2) {
            pWork->m_nFlag &= ~2;
        }
    } else if (fFrame >= pWork->m_fActTrgFrame && !(pWork->m_nFlag & 8) && !(pWork->m_nFlag & 2)) {
        pWork->m_nFlag |= 2;
        GetTransNodePos(pWork->m_EffOfs);
        pWork->m_EffOfs.y = 3.6f;

        bcs::FVEC3 pos;
        bcs::BCS_VecZ_RotY(pos, GetCcParam_()->m_Unk84, GetBlockTargetRot_());
        bcs::FVEC3Add(&pos, &pos, &pWork->m_EffOfs);
        Eff_CreateSimple(0x12, 0x68D, pos, GetBlockTargetRot_(), 3, 0, 0);
    }
    return 0;
}

int msw::CChrCoreVolleyKoopaJr::SubStateBlockJumpActEnd_(STATE_WORK_BLOCK_JUMP *pWork)
{
    pWork->m_nFlag &= ~2;
    return 0;
}

// A ball hit on the block ends the block and knocks him back with the move
// and jump of the parameters (another set when the hit makes a splash: kinds
// 0 and 3 replace the block effect with effect 0x68E).
void msw::CChrCoreVolleyKoopaJr::SubStateBlockJumpBlockHit_(const CCollDefUnit *pDef)
{
    if (GetState() == 5) {
        STATE_WORK_BLOCK_JUMP *pWork = static_cast<STATE_WORK_BLOCK_JUMP *>(FsmqGetWork_());
        int bSplash = 0;

        pWork->m_nFlag = (pWork->m_nFlag & ~6) | 0x28;
        m_fBlockUnk888 = 0.0f;
        m_fBlockUnk88C = 0.0f;

        unsigned long nKind = pDef->GetKind();
        if (nKind == 0 || nKind == 3) {
            Eff_StopForce(0x12);

            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, GetCcParam_()->m_Unk84, GetBlockTargetRot_());
            bcs::FVEC3Add(&pos, &pos, &pWork->m_EffOfs);
            unsigned long hEffect = Eff_CreateSimple(0x68E, pos, GetBlockTargetRot_(), 3, 0, 0);
            float fZOffset = GetCcParam_()->m_Unk88;
            if (hEffect && fZOffset != 0.0f) {
                CEffectCluster *pCluster = CEffectCtrl::GetObject(hEffect);
                if (pCluster) {
                    pCluster->SetZSortOffset(fZOffset);
                }
            }
            pWork->m_nFlag |= 0x10;
            bSplash = 1;
        }

        float fFrame = GetCcParam_()->m_UnkA0;
        float fDist = GetCcParam_()->m_UnkA4;
        float fJumpFrame = GetCcParam_()->m_UnkA8;
        float fJumpFrom = GetCcParam_()->m_UnkAC;
        float fJumpUnk = GetCcParam_()->m_UnkB0;
        if (bSplash) {
            fFrame = GetCcParam_()->m_Unk8C;
            fDist = GetCcParam_()->m_Unk90;
            fJumpFrame = GetCcParam_()->m_Unk94;
            fJumpFrom = GetCcParam_()->m_Unk98;
            fJumpUnk = GetCcParam_()->m_Unk9C;
        }

        pWork->m_MoveParam.m_nType = 0xB;
        pWork->m_MoveParam.m_fFrame = 0.0f;
        pWork->m_MoveParam.m_fFrameMax = fFrame;
        pWork->m_MoveParam.m_fDist = fDist;
        Util_SetDirMoveParam(pWork->m_DirMove, -1.0f);
        pWork->m_fDirMoveUnk2C = 0.0f;
        if (fJumpFrame > 0.0f) {
            Util_SetJumpParam(fJumpFrame, fJumpFrom, fJumpUnk);
            m_fJumpGravity = -0.0125f;
        }
    }
    DriverSendMessage((CChrDrv::MSG_ID)0x53, 5, 0, 0, 0);
    CGame::StartCameraQuake(7, m_nPlayerNo, -1);
}

void msw::CChrCoreVolleyKoopaJr::SubStateBlockJumpCollSet_(STATE_WORK_BLOCK_JUMP *pWork)
{
    if (pWork->m_nFlag & 2) {
        UpdateCollDefSetParam_(pWork->m_EffOfs, pWork->m_fCollSize, GetBlockTargetRot_());
    }
}

void msw::CChrCoreVolleyKoopaJr::SubStateSpikeDoSmashSpikeEffect_(STATE_WORK_SPIKE *)
{
    Eff_CreateAttach(0x13, 0x68F, m_nEffNode, 0, 1, 3, 0);
}

// The trick goes to the middle of the attack zone of his own court.
void msw::CChrCoreVolleyKoopaJr::SubStateAirTrickSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long)
{
    bcs::FVEC4 rect;
    Util_GetGuardCourtAttackZoneRect_(rect);
    pWork->m_TargetPos.Set(0.5f * (rect.x + rect.y), 0.0f, 0.5f * (rect.z + rect.w));
    pWork->m_MoveParam.m_nType = 0xB;

    pWork->m_MoveParam.m_fDist = DistXZ(pWork->m_TargetPos, GetPos());
    pWork->m_MoveParam.m_fFrame = 0.0f;
    pWork->m_MoveParam.m_fFrameMax = pWork->m_fTrickFrame;
}

float msw::CChrCoreVolleyKoopaJr::SubStateAirTrickGetTrickRotFrame_(STATE_WORK_AIR_TRICK *)
{
    return 60.0f;
}

// The paint is thrown at frame 60 of the trick motion (or at its end); the
// shot is ready when the trick's time is over. The loop motion starts the
// first time the motion ends (m_nArtsLevel serves as its flag here).
int msw::CChrCoreVolleyKoopaJr::SubStateAirTrickIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    pWork->m_fTrickCount += m_fFrameStep * m_fTimeScale;
    pWork->m_fTrickCount = Min(pWork->m_fTrickCount, pWork->m_fTrickFrame);
    SubStateAirTrickDoJumpMove_(pWork);

    if (pWork->m_nTrickStep == 0 && (IsAnimEnd(3) || AnimGetFrame(3) >= 60.0f)) {
        SubStateAirTrickFirePaintBomb_();
        pWork->m_nTrickStep++;
    }
    SubStateAirTrickDoBallAdsorb_(pWork);
    if (pWork->m_fTrickCount >= pWork->m_fTrickFrame) {
        bReady = 1;
    }

    if (pWork->m_nArtsLevel < 1 && IsAnimEnd(3)) {
        SubStateAirTrickPlayLoopMotion_(pWork);
        pWork->m_nArtsLevel = 1;
    }
    return bReady;
}

void msw::CChrCoreVolleyKoopaJr::SubStateAirTrickPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x93, 3);
    PlaySePos(0x456, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    // The paint brush he holds (the special-shot model).
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0x92, 3);
}

void msw::CChrCoreVolleyKoopaJr::SubStateAirTrickPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x94, 3);
    ArtsOptSetFadeOutSE(0x459);
    ArtsOptFadeOut(5.0f, 65.0f, 1);
}

void msw::CChrCoreVolleyKoopaJr::SubStateAirTrickPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreVolley::SubStateAirTrickPlayLandMotion_(pWork);

    ArtsOptSetFadeOutSE(0x459);
    ArtsOptFadeOut(5.0f, 0.0f, 0);
}

// Throws up to 64 paint balls from the brush's tip (node SP_item_02, or his
// position without the model) to points on rings around the middle of the
// court he attacks: the first ring 0.3 out, then every 3.44 out to 6.5,
// with a ball about every 3 along each ring, a little scattered. Points the
// court's edge moves by more than one unit are left out.
void msw::CChrCoreVolleyKoopaJr::SubStateAirTrickFirePaintBomb_()
{
    // static-order swap (notes/decisions.md): the game numbers
    // PAINT_UNIT_RADIUS first and stores it second.
    static const float PAINT_COLL_RADIUS = 0.1f;
    static const float PAINT_UNIT_RADIUS = 4.3f;

    bcs::FVEC3 from(GetPos());
    bcs::CMdlObj *pMdl = GetArtsOptMdl();

    if (pMdl != 0) {
        nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode("SP_item_02");
        const bcs::FMTX34 *pMtx = pMdl->GetNodeWorldMtxPtr(node.GetID());

        from.x = pMtx->m[0][3];
        from.y = pMtx->m[1][3];
        from.z = pMtx->m[2][3];
    }

    PlaySePos(0x457, 0, 0, -1, 0.0f, 1.0f, 0.0f);

    bcs::FVEC4 rect;
    Util_GetTargetCourtRect_(rect);
    bcs::FVEC3 court(0.5f * (rect.x + rect.y), 0.0f, 0.5f * (rect.z + rect.w));
    unsigned long nHitFlag = (unsigned char)GetTargetCourtIdx() | 0x2000000;
    // (A second copy of the court's middle: the original keeps one too, its
    // members stored to the stack and the rings read from it.)
    bcs::FVEC3 center(court.x, 0.0f, court.z);
    int bFirst = 1;
    long nThrown = 0;
    float fRadius = 0.3f;
    float fStartRot = 0.0f;

    while (nThrown < 64) {
        // One ball per 0.7 unit radius of the ring's length.
        float fLength = PI * (fRadius * 2.0f);
        long nNum = (long)(fLength * (1.0f / (PAINT_UNIT_RADIUS * 0.7f))) + 1;
        float fStep = PI2 / nNum;
        float fRot = WrapRad(fStartRot);

        for (long i = 0; i < nNum; i++) {
            float fDist = fRadius + bcs::BCS_RandRange(-PAINT_UNIT_RADIUS * 0.1f, PAINT_UNIT_RADIUS * 0.1f);
            float fAngle = WrapRad(fRot + bcs::BCS_RandRange(-0.3f * fStep, 0.3f * fStep));
            bcs::FVEC3 pos;
            int bThrow = 1;

            bcs::BCS_VecZ_RotY(pos, fDist, fAngle);
            pos.x += center.x;
            pos.z += center.z;

            bcs::FVEC3 hitPos(pos);
            bcs::FVEC3 hitNormal;
            float fHitTime;

            if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitPos, hitPos, PAINT_COLL_RADIUS, 1, nHitFlag)) {
                if (DistSqXZ(pos, hitPos) <= 1.0f) {
                    pos = hitPos;
                } else {
                    bThrow = 0;
                }
            }

            if (bThrow) {
                CC11_VOLLEY::CcKoopaJrPaint *pPaint =
                    new (0x100) CC11_VOLLEY::CcKoopaJrPaint(CGame::GetTempParent(), m_nTeamNo, bFirst);

                if (pPaint != 0) {
                    pPaint->Create(this, from, pos, nThrown & 3);
                    bFirst = 0;
                }
                nThrown++;
            }
            fRot = WrapRad(fRot + fStep);
        }

        // Each ring starts half a step further round.
        fStartRot = WrapRad(fStartRot + 0.5f * fStep);
        fRadius += PAINT_UNIT_RADIUS * 0.8f;
        if (fRadius > 6.5f) {
            break;
        }
    }
}
