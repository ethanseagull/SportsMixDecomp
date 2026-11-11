// msw::CBallBasketFruit: the fruit balls of the Hungry Boss Packun court,
// and msw::CBombExplosionHB, the blast of the fruit bomb.
#include "game/msw/CBallBasketFruit.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/BasketPrm.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBallManBasketHungryBossPackun.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "game/msw/CNpcBase.hpp"
#include "nw4r/math/triangular.h"
#include "revolution/mtx.h"

// 70 degrees in radians. Not used here: nearly every BasketF file has this
// object and the static initialiser that fills it, so it most likely comes
// from a header they all include. (which one, and its name, are not known)
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

namespace msw {

    // The blast of the fruit bomb: CBombExplosion with the Hungry Boss
    // Packun court's effect.
    class CBombExplosionHB : public CBombExplosion {
        public:
            CBombExplosionHB(bcs::CTask *pParent, const bcs::FVEC3 &rPos, unsigned long arg, float f1, float f2,
                             float f3, long l1, long l2, unsigned long ul)
                : CBombExplosion(pParent, rPos, arg, f1, f2, f3, l1, l2, ul)
            {
            }
            virtual ~CBombExplosionHB() {}

            virtual void CreateEffect_(const bcs::FVEC3 &rPos);
    };
}

namespace {

    // (guessed helper) rad brought into [0, 2 pi].
    inline float NormalizeRad(float rad)
    {
        while (rad > 2.0f * 3.14159265f) {
            rad -= 2.0f * 3.14159265f;
        }
        while (rad < 0.0f) {
            rad += 2.0f * 3.14159265f;
        }
        return rad;
    }

    // (guessed helper) A random number between two bounds given in either
    // order.
    inline long RandRangeSorted(long a, long b)
    {
        long nMin = a;
        long nMax = b;

        if (a > b) {
            nMin = b;
            nMax = a;
        }
        return bcs::BCS_RandRange(nMin, nMax);
    }
}

inline float msw::CBallBasketFruit::RandLife() const
{
    long nMin = (m_nFruitFlag & FRUIT_FLAG_ON_CHR) ? BASKET_PRM_COMMON_HUNGRY_CHR_BALL_LIFE_MIN
                                                   : BASKET_PRM_COMMON_HUNGRY_STRING_BALL_LIFE_MIN;
    long nMax = (m_nFruitFlag & FRUIT_FLAG_ON_CHR) ? BASKET_PRM_COMMON_HUNGRY_CHR_BALL_LIFE_MAX
                                                   : BASKET_PRM_COMMON_HUNGRY_STRING_BALL_LIFE_MAX;

    return RandRangeSorted(nMin, nMax);
}

msw::CBallBasketFruit::CBallBasketFruit(bcs::CTask *pParent, long nBallNo, long nKind)
    : CBallBasket(pParent, nBallNo), m_nStringId(0), m_nFruitFlag(0), m_nKind(nKind)
{
}

msw::CBallBasketFruit::~CBallBasketFruit()
{
}

void msw::CBallBasketFruit::UpdateDisp_()
{
    CBallBasket::UpdateDisp_();
    // A fruit on a string does not show its second object.
    if (m_nStringId != 0) {
        m_pSubObj->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
    }
}

void msw::CBallBasketFruit::CtorStateNormalShoot()
{
    m_nFruitFlag &= ~FRUIT_FLAG_2;
    CBallBasket::CtorStateNormalShoot();
}

void msw::CBallBasketFruit::CtorStateAttach()
{
    CBallBasket::CtorStateAttach();
    if (m_nStringId == 0) {
        m_nFruitFlag |= FRUIT_FLAG_ON_CHR;
    } else {
        // Hung on a string: start at a random turn about Z.
        Quaternion q;
        float fSin;
        float fCos;

        m_vRotAxis.x = 0.0f;
        m_vRotAxis.y = 0.0f;
        m_vRotAxis.z = 1.0f;

        // A tilt and its side are drawn here but not used. (the bounds are
        // computed at run time in the original: plain float locals are not
        // folded by the compiler, so they are taken to be locals)
        float fTiltRange = 3.14159265f / 3.0f;
        float fTiltCenter = 3.14159265f / 2.0f;
        float fTilt = bcs::BCS_RandRange(fTiltCenter - fTiltRange, fTiltCenter + fTiltRange);
        if (bcs::BCS_RandMax(1L) != 0) {
            fTilt = -fTilt;
        }

        // A quaternion turning by fAngle about the axis.
        float fAngle = bcs::BCS_RandRange(0.0f, 2.0f * 3.14159265f);
        nw4r::math::SinCosFIdx(&fSin, &fCos, NW4R_MATH_RAD_TO_FIDX(0.5f * fAngle));
        q.x = m_vRotAxis.x * fSin;
        q.y = m_vRotAxis.y * fSin;
        q.z = m_vRotAxis.z * fSin;
        q.w = fCos;
        PSQUATMultiply(&q, &m_Rot, &m_Rot);
        PSQUATNormalize(&m_Rot, &m_Rot);
    }
}

void msw::CBallBasketFruit::CtorStateFreeMove()
{
    CBallBasket::CtorStateFreeMove();
    m_fLife = RandLife();
}

void msw::CBallBasketFruit::LoopStateFreeMove()
{
    if (m_fNoCatchCount > 0.0f) {
        m_fNoCatchCount -= m_fFrameRate;
        if (m_fNoCatchCount < 0.0f) {
            m_fNoCatchCount = 0.0f;
        }
    }
    if (m_fDefCounter > 0.0f) {
        m_fDefCounter -= m_fFrameRate;
        if (m_fDefCounter < 0.0f) {
            m_fDefCounter = 0.0f;
        }
    }
    if (m_fTimer37C > 0.0f) {
        m_fTimer37C -= m_fFrameRate;
        if (m_fTimer37C < 0.0f) {
            m_fTimer37C = 0.0f;
        }
    }
    if (m_fTimer320 > 0.0f) {
        m_fTimer320 -= m_fMoveRate;
        if (m_fTimer320 < 0.0f) {
            m_fTimer320 = 0.0f;
        }
    }

    CheckCurveStep_();
    FuncMove_(m_Pos, m_Velocity, m_fGravity, m_fMoveRate);
    CheckCollision_();
    RotNormal_();

    if (GetState() == 1) {
        m_fLife -= GetFrameRateF();
        if (m_fLife <= 0.0f) {
            m_fLife = 0.0f;
            m_nFruitFlag |= FRUIT_FLAG_DEAD;
        }
    }
    if ((m_nFruitFlag & FRUIT_FLAG_2) || (m_nFruitFlag & FRUIT_FLAG_DEAD)) {
        AdjustGravity_();
    }

    // A dead fruit fades out, faster once it has scored, and goes away.
    if (m_nKind != KIND_BOMB && (m_nFruitFlag & FRUIT_FLAG_DEAD)) {
        float fFade = (m_nFlag & FLAG_SCORED) ? 0.1f : 1.0f / 30.0f;
        float fAlpha = m_fAlphaRate;
        float fNewAlpha = fAlpha - fFade * GetFrameRateF();

        if (fNewAlpha < 0.0f) {
            CBallMan::DelBall(m_nBallNo);
        } else {
            SetAlphaRate(fNewAlpha);
        }
    }
}

void msw::CBallBasketFruit::DtorStateNormalShoot()
{
    CBallBasket::DtorStateNormalShoot();
}

void msw::CBallBasketFruit::DtorStateLayupShoot()
{
    CBallBasket::DtorStateLayupShoot();
}

void msw::CBallBasketFruit::TrgGoalIn_()
{
    TrgItemEffect_(3);
    m_Velocity.y = 0.0f;
    m_fGravity = 0.0f;
    if (m_nFlag & FLAG_SCORED) {
        return;
    }

    PlayGoalInEff();
    switch (m_nShootType) {
    case 0:
    case 1:
    case 6:
    case 7:
        break;
    case 3:
        // Thrown off in a random direction.
        bcs::BCS_VecZ_RotY(m_Velocity, 0.1f, NormalizeRad(bcs::BCS_RandMax(2.0f * 3.14159265f)));
        m_Velocity.y = -0.3f;
        break;
    default:
        return;
    }

    SetShootResult(1);
    long nKind = m_nKind;
    m_nFruitFlag |= FRUIT_FLAG_DEAD;
    CGame::TrgGoalIn(m_nShotChrIdx, m_nShotTeamIdx, m_nShootType, nKind, 0.0f,
                     CGame::GetGoalPos(CGame::GetTargetGoalIdx(m_nShotTeamIdx)));
    m_nFlag |= 0x3001;
}

void msw::CBallBasketFruit::Reset()
{
    m_nFruitFlag = 0;
    CBallBasket::Reset();

    float aScale[] = {
        BASKET_PRM_S70_SACLE_ORANGE, BASKET_PRM_S70_SACLE_BLUE_APPLE, BASKET_PRM_S70_SACLE_RED_APPLE,
        BASKET_PRM_S70_SACLE_SUICA, 1.0f, BASKET_PRM_S70_SACLE_BOMB,
    };
    float fScale = aScale[m_nKind];

    SetCollPushRadius(fScale * GetCollPushRadiusDefault());
    SetCollBodyRadius(fScale * GetCollBodyRadiusDefault());
    SetCollCatchRadius(fScale * GetCollCatchRadiusDefault());
    SetScale(fScale);
    if (m_nKind == KIND_SUICA) {
        Eff_Stop(0xC);
        Eff_CreateAttach(0xC, 0x49D, 0, 0, 3);
    }
    m_fUnk374 = 0.0f;

    m_fLife = RandLife();
}

long msw::CBallBasketFruit::CallbackBodyDamage_(const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos)
{
    // Neither a fruit on a string nor the bomb takes damage.
    if (m_nStringId != 0 || IsBaExplode()) {
        return 0;
    }
    return CBallBasket::CallbackBodyDamage_(pAtk, rPos);
}

void msw::CBallBasketFruit::CallbackBodyPush_(CCollBodyUnit *pSelf, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
{
    if (!IsShootMode() && !(m_nFruitFlag & FRUIT_FLAG_ON_CHR)) {
        CBallBasket::CallbackBodyPush_(pSelf, pOther, pPush);
    }
}

int msw::CBallBasketFruit::IsActive() const
{
    if (CBallBasket::IsActive() && !(m_nFruitFlag & FRUIT_FLAG_DEAD)) {
        return 1;
    }
    return 0;
}

int msw::CBallBasketFruit::CanbeCatch() const
{
    if (!IsActive()) {
        return 0;
    }
    if (!IsShootMode() && (m_nFruitFlag & FRUIT_FLAG_ON_CHR)) {
        return 0;
    }
    // A fruit on a string can be caught when the string allows it.
    if (m_nStringId != 0) {
        CNpcBase *pString = static_cast<CNpcBase *>(bcs::CTaskMan::GetTask(m_nStringId));

        if (pString != 0) {
            return pString->CanbeCatch();
        }
    }
    return CBallBasket::CanbeCatch();
}

void msw::CBallBasketFruit::ResetOnDetach_()
{
    m_nStringId = 0;
}

float msw::CBallBasketFruit::GetGravityDefault() const
{
    if (m_nKind == KIND_BOMB) {
        return -BASKET_PRM_S70_GRAV_VALUE_BOMB;
    }
    return -BASKET_PRM_S70_GRAV_VALUE_EXT;
}

float msw::CBallBasketFruit::GetFloorBoundRateY() const
{
    float aRate[] = {
        BASKET_PRM_S70_BOUND_RATE_Y_OR, BASKET_PRM_S70_BOUND_RATE_Y_BA, BASKET_PRM_S70_BOUND_RATE_Y_RA,
        BASKET_PRM_S70_BOUND_RATE_Y_SU, BASKET_PRM_S70_BOUND_RATE_Y_ME, 0.0f,
    };
    return aRate[m_nKind];
}

float msw::CBallBasketFruit::GetFloorBoundRateXZ() const
{
    float aRate[] = {
        BASKET_PRM_S70_BOUND_RATE_XZ_OR, BASKET_PRM_S70_BOUND_RATE_XZ_BA, BASKET_PRM_S70_BOUND_RATE_XZ_RA,
        BASKET_PRM_S70_BOUND_RATE_XZ_SU, BASKET_PRM_S70_BOUND_RATE_XZ_ME, 0.0f,
    };
    return aRate[m_nKind];
}

float msw::CBallBasketFruit::GetWallBoundRateXZ() const
{
    return GetFloorBoundRateXZ();
}

void msw::CBallBasketFruit::CalcBoundRandXZBG()
{
    if (m_Velocity.z == 0.0f && m_Velocity.x == 0.0f) {
        float fVelX = bcs::BCS_RandRange(BASKET_PRM_S70_FIRST_BOUND_MIN_VEL_X, BASKET_PRM_S70_FIRST_BOUND_MAX_VEL_X);

        if (bcs::BCS_RandRange(0L, 1L) != 0) {
            fVelX = -fVelX;
        }
        m_Velocity.x = fVelX;
    }
}

long msw::CBallBasketFruit::GetJustShotMinStartFrm() const
{
    long nKind = m_nKind;
    long aFrm[] = {
        BASKET_PRM_S70_JS_MIN_ST_FRAME_OR, BASKET_PRM_S70_JS_MIN_ST_FRAME_BA, BASKET_PRM_S70_JS_MIN_ST_FRAME_RA,
        BASKET_PRM_S70_JS_MIN_ST_FRAME_SU, 0, 0,
    };
    return aFrm[nKind];
}

long msw::CBallBasketFruit::GetJustShotMidStartFrm() const
{
    long nKind = m_nKind;
    long aFrm[] = {
        BASKET_PRM_S70_JS_MID_ST_FRAME_OR, BASKET_PRM_S70_JS_MID_ST_FRAME_BA, BASKET_PRM_S70_JS_MID_ST_FRAME_RA,
        BASKET_PRM_S70_JS_MID_ST_FRAME_SU, 0, 0,
    };
    return aFrm[nKind];
}

long msw::CBallBasketFruit::GetJustShotMaxStartFrm() const
{
    long nKind = m_nKind;
    long aFrm[] = {
        BASKET_PRM_S70_JS_MAX_ST_FRAME_OR, BASKET_PRM_S70_JS_MAX_ST_FRAME_BA, BASKET_PRM_S70_JS_MAX_ST_FRAME_RA,
        BASKET_PRM_S70_JS_MAX_ST_FRAME_SU, 0, 0,
    };
    return aFrm[nKind];
}

long msw::CBallBasketFruit::GetJustShotMinEndFrm() const
{
    long nKind = m_nKind;
    long aFrm[] = {
        BASKET_PRM_S70_JS_MIN_ED_FRAME_OR, BASKET_PRM_S70_JS_MIN_ED_FRAME_BA, BASKET_PRM_S70_JS_MIN_ED_FRAME_RA,
        BASKET_PRM_S70_JS_MIN_ED_FRAME_SU, 0, 0,
    };
    return aFrm[nKind];
}

long msw::CBallBasketFruit::GetJustShotMidEndFrm() const
{
    long nKind = m_nKind;
    long aFrm[] = {
        BASKET_PRM_S70_JS_MID_ED_FRAME_OR, BASKET_PRM_S70_JS_MID_ED_FRAME_BA, BASKET_PRM_S70_JS_MID_ED_FRAME_RA,
        BASKET_PRM_S70_JS_MID_ED_FRAME_SU, 0, 0,
    };
    return aFrm[nKind];
}

long msw::CBallBasketFruit::GetJustShotMaxEndFrm() const
{
    long nKind = m_nKind;
    long aFrm[] = {
        BASKET_PRM_S70_JS_MAX_ED_FRAME_OR, BASKET_PRM_S70_JS_MAX_ED_FRAME_BA, BASKET_PRM_S70_JS_MAX_ED_FRAME_RA,
        BASKET_PRM_S70_JS_MAX_ED_FRAME_SU, 0, 0,
    };
    return aFrm[nKind];
}

long msw::CBallBasketFruit::GetJustShotSignEffectID() const
{
    long nEffectID = 0x4A0;

    if (m_nKind == KIND_SUICA) {
        nEffectID = 0x49F;
    }
    return nEffectID;
}

void msw::CBallBasketFruit::DoExplode(float, float)
{
    if (IsBaExplode()) {
        new (0x100) CBombExplosionHB(CGame::GetTempParent(), m_Pos, 0, BASKET_PRM_HB_BOMB_DOWN_FRM,
                                     CBallManBasketHungryBossPackun::GetBombExplodeR(), 0.0f, 0, 0, 0);
        ChangeState(0);
        m_nFlag |= 0x40000;
        if (m_nStringId != 0) {
            CChrMan::NpcSendMessage(0xB, 0, -1, m_nBallNo, 0);
        }
    }
}

int msw::CBallBasketFruit::IsBaExplode() const
{
    return m_nKind == KIND_BOMB;
}

float msw::CBallBasketFruit::GetBgCollRadius() const
{
    float aScale[] = {
        BASKET_PRM_S70_SACLE_ORANGE, BASKET_PRM_S70_SACLE_BLUE_APPLE, BASKET_PRM_S70_SACLE_RED_APPLE,
        BASKET_PRM_S70_SACLE_SUICA, 1.0f, BASKET_PRM_S70_SACLE_BOMB,
    };
    return 0.23f * aScale[m_nKind];
}

float msw::CBallBasketFruit::GetBgCollRadiusXZ() const
{
    float aScale[] = {
        BASKET_PRM_S70_SACLE_ORANGE, BASKET_PRM_S70_SACLE_BLUE_APPLE, BASKET_PRM_S70_SACLE_RED_APPLE,
        BASKET_PRM_S70_SACLE_SUICA, 1.0f, BASKET_PRM_S70_SACLE_BOMB,
    };
    return 0.23f * aScale[m_nKind];
}

void msw::CBombExplosionHB::CreateEffect_(const bcs::FVEC3 &rPos)
{
    CGimmickMan::EffectBombExplosionCreate(rPos, 0, BASKET_PRM_HB_BOMB_EXPLODE_EFF_SCALE, 0);
    CGameSnd::Play(0xC51, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
}
