// msw::CItemMushroom and msw::CItemToadstool: the mushroom and the poison
// mushroom. One file in the original: the two share one float pool, and
// their weak functions (GetItemType, the destructor thunks) come after the
// toadstool's code.
#include "game/msw/CItemKinds.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"

namespace {

    // (name guessed) v kept between lo and hi.
    template <typename T> static inline T Clamp(T v, T lo, T hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }

    // (name guessed) A factor of 1 on the toadstool's spin in flight. The
    // original multiplies by 1.0f at run time, loading the constant at
    // each use, so the factor came from somewhere that was not folded: a
    // guess at what that was.
    static inline float ThrowSpinScale()
    {
        return 1.0f;
    }
}

namespace msw {

    // --- mushroom ---

    CItemMushroom::CItemMushroom(CItemMan *pMan) : CItem(pMan)
    {
        m_fUnk1AC = 0.0f;
        m_fUnk1B0 = 0.0f;
    }

    CItemMushroom::~CItemMushroom() {}

    void CItemMushroom::Create(const bcs::FVEC3 &pos)
    {
        CItem::Create(pos);
        CreateMdlObj(6, 0, 0, 0x20);

        CollAtkCreate(1);
        CollAtkSetAtkType(0, 0x12104);
        CollAtkSetRadius(0, 0.5f);

        CollBodyCreate(1);
        CollBodySetPushEnableAll(1);
    }

    float CItemMushroom::GetBodyCollRadius() const
    {
        return 0.4f * GetParamScale();
    }

    float CItemMushroom::GetParamHeight() const
    {
        return 0.3f * GetParamScale();
    }

    float CItemMushroom::GetParamAttachHeight() const
    {
        return GetParamHeight();
    }

    float CItemMushroom::GetAtkCollRadius() const
    {
        return m_AtkColl.GetRadius(0);
    }

    void CItemMushroom::CtorStateDrop()
    {
        m_fStateTimer = GetParamDropLife();
        m_fGravity = GetParamGravity();
        m_nFlag &= ~0x4;
        m_nFlag &= ~0x2000;
        m_nFlag &= ~0x100;
        m_nFlag |= 0x400;
        CollBodySetEnableAll(1);
        m_nSubStep = 0;
        m_fUnk1AC = 0.0f;
        m_fUnk1B0 = 0.0f;
    }

    // Lands (flag 0x100) with a wobble, waits 10 frames, then spins up to
    // 3 degrees a frame. Wobbles again whenever it bounces off a wall
    // (flag 0x200).
    void CItemMushroom::LoopStateDrop()
    {
        DoMove();
        switch (m_nSubStep) {
        case 0:
            if (m_nFlag & 0x100) {
                m_vVelocity.x = m_vVelocity.z = 0.0f;
                DoShakeInit(0.25f, bcs::DEG_TO_RAD(45.0f));
                m_nFlag |= 0x2004;
                m_fUnk190 = 10.0f;
                m_nSubStep++;
            }
            break;
        case 1:
            if (m_fUnk190 > 0.0f) {
                m_fUnk190 -= GetFrameRateF();
            }
            if (m_fUnk190 <= 0.0f) {
                m_nSubStep++;
            }
            break;
        default:
            m_fUnk1AC += bcs::DEG_TO_RAD(0.1f) * GetFrameRateF();
            m_fUnk1AC = Clamp(m_fUnk1AC, 0.0f, bcs::DEG_TO_RAD(3.0f));
            break;
        }

        AddRotY(m_fUnk1AC * GetFrameRateF());
        if (IsFlag(0x200)) {
            m_nFlag &= ~0x200;
            DoShakeInit(0.2f, bcs::DEG_TO_RAD(225.0f));
        }
    }

    void CItemMushroom::DoThrowInit(const bcs::FVEC3 &rPos, int)
    {
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
        SetVelParabola_(rPos, m_fParamB8, m_fParamBC, m_fParamC0);
    }

    void CItemMushroom::CtorStateThrow()
    {
        m_fStateTimer = GetParamMainLife();
        m_nFlag |= 0x2000;
        m_nFlag &= ~0x4;
        m_nFlag |= 0x400;
        CollBodySetEnableAll(1);
        CollAtkSetRadius(0, 0.32f * GetParamScale());
        m_nSubStep = 0;
    }

    // Flies in a parabola; its attack is turned on only once it has
    // landed, and anything it then hits breaks it.
    void CItemMushroom::LoopStateThrow()
    {
        switch (m_nSubStep) {
        case 0:
            DoMove();
            if (m_nFlag & 0x100) {
                m_vVelocity.x = 0.0f;
                m_vVelocity.y = 0.0f;
                m_vVelocity.z = 0.0f;
                CollBodySetEnableAll(0);
                CollAtkSetEnable(0, 1);
                CollAtkSetPos(0, m_vPos);
                m_nFlag |= 0x4;
                DoShakeInit(0.25f, bcs::DEG_TO_RAD(45.0f));
                m_nSubStep++;
            }
            break;
        case 1:
            DoMove();
            CollAtkSetPos(0, m_vPos);
            if (CollAtkIsAtkHit(0)) {
                ChangeStateBreak();
            }
            break;
        }
    }

    // Pushed: it bounces off (its velocity is reflected about the push
    // direction in the XZ plane), unless already moving away.
    void CItemMushroom::CallbackBodyPush_(CCollBodyUnit *pMy, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        CItem::CallbackBodyPush_(pMy, pOther, pPush);
        if (pPush == 0) {
            return;
        }

        bcs::FVEC3 dir = *pPush;
        if (dir.x * dir.x + dir.z * dir.z > 0.0f) {
            bcs::FVEC3 pushXZ(dir.x, 0.0f, dir.z);
            bcs::BCS_Vec_Normalize(dir, pushXZ);

            // Already moving away: nothing to reflect.
            float fDot = 2.0f * bcs::FVEC3Dot(&m_vVelocity, &dir);
            if (fDot > 0.0f) {
                return;
            }
            bcs::FVEC3 reflect;
            bcs::FVEC3Scale(&reflect, &dir, fDot);
            bcs::FVEC3Sub(&m_vVelocity, &m_vVelocity, &reflect);
        }
    }

    // --- toadstool ---

    CItemToadstool::CItemToadstool(CItemMan *pMan) : CItem(pMan)
    {
        m_fUnk1AC = 0.0f;
        m_fUnk1B0 = 0.0f;
    }

    CItemToadstool::~CItemToadstool() {}

    void CItemToadstool::Create(const bcs::FVEC3 &pos)
    {
        CItem::Create(pos);
        CreateMdlObj(2, 0, 0, 0x20);

        CollAtkCreate(1);
        CollAtkSetAtkType(0, 0x11E0D);
        CollAtkSetRadius(0, 0.5f);
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);

        CollBodyCreate(1);
        CollBodySetTargetFlag(0, 0x10);
        CollBodySetPushEnableAll(1);
    }

    float CItemToadstool::GetBodyCollRadius() const
    {
        return 0.4f * GetParamScale();
    }

    float CItemToadstool::GetParamHeight() const
    {
        return 0.3f * GetParamScale();
    }

    float CItemToadstool::GetParamAttachHeight() const
    {
        return GetParamHeight();
    }

    float CItemToadstool::GetAtkCollRadius() const
    {
        return m_AtkColl.GetRadius(0);
    }

    void CItemToadstool::CtorStateDrop()
    {
        m_fStateTimer = GetParamDropLife();
        m_fGravity = GetParamGravity();
        m_nFlag &= ~0x4;
        m_nFlag &= ~0x2000;
        m_nFlag &= ~0x100;
        m_nFlag |= 0x400;
        CollBodySetEnableAll(1);
        m_nSubStep = 0;
        m_fUnk1AC = 0.0f;
        m_fUnk1B0 = 0.0f;
    }

    // As the mushroom's, but it spins the other way, up to 5 degrees a
    // frame.
    void CItemToadstool::LoopStateDrop()
    {
        DoMove();
        switch (m_nSubStep) {
        case 0:
            if (m_nFlag & 0x100) {
                m_vVelocity.x = m_vVelocity.z = 0.0f;
                DoShakeInit(0.25f, bcs::DEG_TO_RAD(45.0f));
                m_nFlag |= 0x2004;
                m_fUnk190 = 10.0f;
                m_nSubStep++;
            }
            break;
        case 1:
            if (m_fUnk190 > 0.0f) {
                m_fUnk190 -= GetFrameRateF();
            }
            if (m_fUnk190 <= 0.0f) {
                m_nSubStep++;
            }
            break;
        default:
            m_fUnk1AC += bcs::DEG_TO_RAD(1.0f / 6.0f) * GetFrameRateF();
            m_fUnk1AC = Clamp(m_fUnk1AC, 0.0f, bcs::DEG_TO_RAD(5.0f));
            break;
        }

        AddRotY(-1.0f * m_fUnk1AC * GetFrameRateF());
        if (IsFlag(0x200)) {
            m_nFlag &= ~0x200;
            DoShakeInit(0.2f, bcs::DEG_TO_RAD(225.0f));
        }
    }

    void CItemToadstool::DoThrowInit(const bcs::FVEC3 &rPos, int)
    {
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
        SetVelParabola_(rPos, m_fParamB8, m_fParamBC, m_fParamC0);
    }

    void CItemToadstool::CtorStateThrow()
    {
        m_nFlag &= ~0x100;
        m_fStateTimer = GetParamMainLife();
        m_nFlag &= ~0x4;
        m_nFlag &= ~0x2000;
        m_nFlag |= 0x400;
        CollAtkSetPos(0, m_vPos);
        CollAtkSetEnable(0, 1);
        CollAtkSetRadius(0, 0.32f * GetParamScale());
        CollBodySetEnableAll(1);
        m_nSubStep = 0;
        m_fUnk1AC = 0.0f;
        m_fUnk1B0 = 0.0f;
    }

    // Unlike the mushroom its attack is on in flight; it lands and spins as
    // in the drop state, and goes as soon as its attack hits.
    void CItemToadstool::LoopStateThrow()
    {
        DoMove();
        CollAtkSetPos(0, m_vPos);
        switch (m_nSubStep) {
        case 0:
            if (m_nFlag & 0x100) {
                m_vVelocity.x = m_vVelocity.z = 0.0f;
                DoTouchDown();
                DoShakeInit(0.25f, bcs::DEG_TO_RAD(45.0f));
                CollBodySetEnableAll(0);
                m_nFlag |= 0x2004;
                m_fUnk190 = 10.0f;
                m_nSubStep++;
            }
            break;
        case 1:
            if (m_fUnk190 > 0.0f) {
                m_fUnk190 -= GetFrameRateF();
            }
            if (m_fUnk190 <= 0.0f) {
                m_nSubStep++;
            }
            break;
        default:
            m_fUnk1AC += ThrowSpinScale() * (bcs::DEG_TO_RAD(1.0f / 6.0f) * GetFrameRateF());
            m_fUnk1AC = Clamp(m_fUnk1AC, 0.0f, bcs::DEG_TO_RAD(5.0f));
            CollAtkSetPos(0, m_vPos);
            break;
        }

        AddRotY(-1.0f * m_fUnk1AC * GetFrameRateF() * ThrowSpinScale());
        if (IsFlag(0x200)) {
            m_nFlag &= ~0x200;
            DoShakeInit(0.2f, bcs::DEG_TO_RAD(225.0f));
        }
        if (CollAtkIsAtkHit(0)) {
            DoDelete();
        }
    }

    void CItemToadstool::CallbackBodyPush_(CCollBodyUnit *pMy, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        CItem::CallbackBodyPush_(pMy, pOther, pPush);
        if (pPush == 0) {
            return;
        }

        bcs::FVEC3 dir = *pPush;
        if (dir.x * dir.x + dir.z * dir.z > 0.0f) {
            bcs::FVEC3 pushXZ(dir.x, 0.0f, dir.z);
            bcs::BCS_Vec_Normalize(dir, pushXZ);

            // Already moving away: nothing to reflect.
            float fDot = 2.0f * bcs::FVEC3Dot(&m_vVelocity, &dir);
            if (fDot > 0.0f) {
                return;
            }
            bcs::FVEC3 reflect;
            bcs::FVEC3Scale(&reflect, &dir, fDot);
            bcs::FVEC3Sub(&m_vVelocity, &m_vVelocity, &reflect);
        }
    }
}
