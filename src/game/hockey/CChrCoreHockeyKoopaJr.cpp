// msw::CChrCoreHockeyKoopaJr: Bowser Jr. in the hockey game. In his special
// shot he throws a spray of paint balls around himself
// (CC11_HOCKEY::CcKoopaJrPaint), on rings that widen outwards, only on the
// side facing the goal.
//
// msw::CC11_HOCKEY::CcKoopaJrPaint: one paint ball. The base class
// (msw::CC_CMN::KOOPAJR::CcPaintBall, in main.dol) does the work; this class
// only gives its tuning values.
#include "game/msw/CChrCoreHockey.hpp"

#include <math.h>
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcPaintBall.hpp"

namespace msw {
    namespace CC11_HOCKEY {

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
                            return 0x5FD;
                        case 1:
                            return 0x5FC;
                        case 2:
                            return 0x5FB;
                    }
                    return -1;
                }
                virtual long GetBallFlyFrmMin_() const { return 55; }
                virtual long GetBallFlyFrmMax_() const { return 65; }
                virtual float GetBallFlyPassHeight_() const { return 6.2f; }
                virtual float GetBallDispScale_() const { return 1.0f; }
                virtual float GetBallAtkCollRadius_() const { return 0.2f; }
                virtual float GetPaintDispScale_() const { return 1.3f; }
                virtual float GetPaintAtkCollRadius_() const { return 1.02f; }
                virtual float GetPaintLifeFrame_() const { return 200.0f; }
                virtual float GetPaintFadeOutFrame_() const { return 20.0f; }
                virtual float GetPaintFadeOutScale_() const { return 1.5f; }
        };
    }
}

namespace {

    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    // (guessed helpers) An angle brought into [0, 2 pi]; the signed
    // difference a - b of two angles, in (-pi, pi]; degrees to radians; the
    // square of the distance between two points in the XZ plane.
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

    inline float RadDiff(float a, float b)
    {
        float diff = WrapRad(a - b);

        if (diff > PI) {
            diff -= PI2;
        }
        return diff;
    }

    inline float DegToRad(float fDeg)
    {
        return 0.017453292f * fDeg;
    }

    inline float DistSqXZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2)
    {
        bcs::FVEC3 pos1(rPos1.x, 0.0f, rPos1.z);
        bcs::FVEC3 pos2(rPos2.x, 0.0f, rPos2.z);
        return bcs::FVEC3DistSq(&pos1, &pos2);
    }
}

msw::CChrCoreHockeyKoopaJr::CChrCoreHockeyKoopaJr()
{
}

msw::CChrCoreHockeyKoopaJr::~CChrCoreHockeyKoopaJr()
{
}

void msw::CChrCoreHockeyKoopaJr::Reset_()
{
    CChrCoreHockey::Reset_();
}

// The paint is thrown at frame 53 of the trick motion (or at its end); the
// shot is ready when the motion is over.
int msw::CChrCoreHockeyKoopaJr::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
{
    int bReady = 0;

    SubStateArtsShotDoJumpMove_(pWork);
    switch (pWork->m_nTrickStep) {
        case 0:
            if (!IsAnimEnd(3) && !(AnimGetFrame(3) >= 53.0f)) {
                break;
            }
            SubStateArtsShotFireBullet_(pWork, 0);
            pWork->m_nTrickStep++;
            // fall through: the motion may be over already
        case 1:
            if (IsAnimEnd(3)) {
                pWork->m_nTrickStep++;
                bReady = 1;
            }
            break;
        default:
            bReady = 1;
            break;
    }
    return bReady;
}

void msw::CChrCoreHockeyKoopaJr::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0xB1, 3);
    PlaySePos(0x456, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    SE_PlayArtsJumpSE(0);
    // The paint brush he holds (the special-shot model).
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0xB3, 3);
}

void msw::CChrCoreHockeyKoopaJr::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreHockey::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0xB4, 3);
    ArtsOptSetFadeOutSE(0x459);
    ArtsOptFadeOut(5.0f, 65.0f, 1);
    pWork->m_nShotStep = 0;
}

// Throws up to 64 paint balls from the brush's tip (node SP_item_02, or his
// position without the model) to points on rings around him: the rings are
// 2.45 apart, out to 10.5, with a ball about every 2.45 along each ring, a
// little scattered. Points more than 40 degrees off the direction of the
// goal are left out, and so are points the court's edge moves by more than
// one unit.
void msw::CChrCoreHockeyKoopaJr::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    // static-order swap (notes/decisions.md): the game numbers
    // PAINT_UNIT_RADIUS first and stores it second.
    static const float PAINT_COLL_RADIUS = 0.1f;
    static const float PAINT_UNIT_RADIUS = 3.5f;

    bcs::FVEC3 from(0.0f, 0.0f, 0.0f);
    bcs::CMdlObj *pMdl = GetArtsOptMdl();

    if (pMdl != 0) {
        nw4r::g3d::ResNode node = pMdl->GetResMdl().GetResNode("SP_item_02");
        const bcs::FMTX34 *pMtx = pMdl->GetNodeWorldMtxPtr(node.GetID());

        from.x = pMtx->m[0][3];
        from.y = pMtx->m[1][3];
        from.z = pMtx->m[2][3];
    } else {
        from = GetPos();
    }

    PlaySePos(0x457, 0, 0, -1, 0.0f, 1.0f, 0.0f);

    const bcs::FVEC3 &rPos = GetPos();
    bcs::FVEC3 center(rPos.x, 0.0f, rPos.z);
    float fGoalRot = GetRotY_ToGoal();
    // How far off the direction of the goal a ball may land.
    float fAngleLimit = DegToRad(40.0f);
    int bFirst = 1;
    long nThrown = 0;
    float fRadius = 0.0f;
    float fStartRot = 0.0f;

    while (nThrown < 64) {
        // One ball per 0.7 unit radius of the ring's length.
        float fLength = PI * (fRadius * 2.0f);
        long nNum = (long)(fLength * (1.0f / (PAINT_UNIT_RADIUS * 0.7f))) + 1;
        float fStep = PI2 / nNum;
        float fRot = WrapRad(fStartRot);

        for (long i = 0; i < nNum; i++) {
            float fDist = fRadius + bcs::BCS_RandRange(-0.35f, 0.35f);
            float fAngle = WrapRad(fRot + bcs::BCS_RandRange(-0.3f * fStep, 0.3f * fStep));
            bcs::FVEC3 pos;
            int bThrow = 1;

            if (fabsf(RadDiff(fAngle, fGoalRot)) > fAngleLimit) {
                bThrow = 0;
            }

            if (bThrow) {
                bcs::BCS_VecZ_RotY(pos, fDist, fAngle);
                pos.x += center.x;
                pos.z += center.z;

                bcs::FVEC3 hitPos(pos);
                bcs::FVEC3 hitNormal;
                float fHitTime;

                if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitPos, hitPos, PAINT_COLL_RADIUS, 5, 0)) {
                    if (DistSqXZ(pos, hitPos) <= 1.0f) {
                        pos = hitPos;
                    } else {
                        bThrow = 0;
                    }
                }
            }

            if (bThrow) {
                CC11_HOCKEY::CcKoopaJrPaint *pPaint =
                    new (0x100) CC11_HOCKEY::CcKoopaJrPaint(CGame::GetTempParent(), m_nTeamNo, bFirst);

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
        fRadius += PAINT_UNIT_RADIUS * 0.7f;
        if (fRadius > PAINT_UNIT_RADIUS * 3.0f) {
            break;
        }
    }
}

// After the shot: motions 0xA2 and 0xA3, one after the other. Over when the
// last has ended and the jump is over.
int msw::CChrCoreHockeyKoopaJr::SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork)
{
    int bJumpEnd = CChrCoreHockey::SubStateArtsShotDoJumpMove_(pWork);
    int bEnd = 0;

    switch (pWork->m_nShotStep) {
        case 0:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0xA2, 0, 0.0f, 0.0f);
                pWork->m_nShotStep++;
            }
            break;
        case 1:
            if (IsAnimEnd(3)) {
                AnimPlay(3, 0xA3, 0, 0.0f, 0.0f);
                pWork->m_nShotStep++;
            }
            break;
        case 2:
            if (IsAnimEnd(3)) {
                pWork->m_nShotStep++;
                bEnd = 1;
            }
            break;
        default:
            bEnd = 1;
            break;
    }
    return bJumpEnd && bEnd;
}
