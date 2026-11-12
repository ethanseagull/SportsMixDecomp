// Written from src/game/hockey/CChrCoreHockeyKoopaJr.cpp (Hockey -> Basket).
// msw::CChrCoreBasketKoopaJr: Bowser Jr. in the basket game. In his special
// shot he throws a spray of paint balls (CC11_BASKET::CcKoopaJrPaint) on
// rings that widen outwards around the goal his team attacks.
//
// msw::CC11_BASKET::CcKoopaJrPaint: one paint ball. The base class
// (msw::CC_CMN::KOOPAJR::CcPaintBall, in main.dol) does the work; this class
// only gives its tuning values.
#include "game/msw/CChrCoreBasket.hpp"

#include <math.h>
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CcPaintBall.hpp"

namespace msw {
    namespace CC11_BASKET {

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
                            return 0x587;
                        case 1:
                            return 0x586;
                        case 2:
                            return 0x585;
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

    // (guessed helpers) An angle brought into [0, 2 pi]; degrees to
    // radians; the square of the distance between two points in the XZ
    // plane.
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


// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

msw::CChrCoreBasketKoopaJr::CChrCoreBasketKoopaJr()
{
}

msw::CChrCoreBasketKoopaJr::~CChrCoreBasketKoopaJr()
{
}

// The paint is thrown at frame 53 of the trick motion (or at its end); the
// shot is ready when the motion is over. The loop motion starts the first
// time a motion ends.
int msw::CChrCoreBasketKoopaJr::SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork)
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

    if (pWork->m_nUnkA0 < 1 && IsAnimEnd(3)) {
        SubStateArtsShotPlayLoopMotion_(pWork);
        pWork->m_nUnkA0 = 1;
    }
    return bReady;
}

void msw::CChrCoreBasketKoopaJr::SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(pWork);

    Eff_CreateAttachAuto(0x13, 0x99, 3);
    PlaySePos(0x456, 0, 0, -1, 0.0f, 1.0f, 0.0f);
    m_nBasketFlag &= ~0x40;
    // The paint brush he holds (the special-shot model).
    ArtsOptDestroy();
    ArtsOptCreate(1, 0, 1);
    ArtsOptFadeIn(5.0f, 0.0f, 0);
    ArtsOptCreateAttachEffectAuto(0, 0x98, 3);
}

void msw::CChrCoreBasketKoopaJr::SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork)
{
    CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(pWork);

    Eff_CreateAttachAuto(0x14, 0x9A, 3);
    ArtsOptSetFadeOutSE(0x459);
    ArtsOptFadeOut(5.0f, 65.0f, 1);
}

// Throws up to 64 paint balls from the brush's tip (node SP_item_02, or his
// position without the model) to points on rings around the goal his team
// attacks: the first ring 0.3 out, then every 1.92 out to 6.5, with a ball
// about every 2.24 along each ring, a little scattered. Points the court's
// edge moves by more than one unit are left out.
void msw::CChrCoreBasketKoopaJr::SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long)
{
    // static-order swap (notes/decisions.md): the game numbers
    // PAINT_UNIT_RADIUS first and stores it second.
    static const float PAINT_COLL_RADIUS = 1.2f;
    static const float PAINT_UNIT_RADIUS = 3.2f;

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

    const bcs::FVEC3 &rGoal = GetTargetGoalPos();
    bcs::FVEC3 goal(rGoal.x, 0.0f, rGoal.z);
    unsigned long nHitFlag = (unsigned char)GetTargetCourtIdx() | 0x2000000;
    // (A second XZ copy of the goal: the original keeps one too, its
    // members stored to the stack and the rings read from it.)
    bcs::FVEC3 center(goal.x, 0.0f, goal.z);
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

            if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, hitPos, hitPos, PAINT_COLL_RADIUS, 5, nHitFlag)) {
                if (DistSqXZ(pos, hitPos) <= 1.0f) {
                    pos = hitPos;
                } else {
                    bThrow = 0;
                }
            }

            if (bThrow) {
                CC11_BASKET::CcKoopaJrPaint *pPaint =
                    new (0x100) CC11_BASKET::CcKoopaJrPaint(CGame::GetTempParent(), m_nTeamNo, bFirst);

                if (pPaint != 0) {
                    pPaint->Create(this, from, pos, nThrown & 3);
                    pPaint->SetAutoVanish(1);
                    bFirst = 0;
                }
                nThrown++;
            }
            fRot = WrapRad(fRot + fStep);
        }

        // Each ring starts half a step further round.
        fStartRot = WrapRad(fStartRot + 0.5f * fStep);
        fRadius += PAINT_UNIT_RADIUS * 0.6f;
        if (fRadius > 6.5f) {
            break;
        }
    }
}
