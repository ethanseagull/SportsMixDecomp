// msw::CItemBanana: the banana peel. Dropped or thrown, it lies on the
// ground until something runs into it; then it flies off and fades out.
#include "game/msw/CItemKinds.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"

namespace msw {

    CItemBanana::CItemBanana(CItemMan *pMan) : CItem(pMan) {}

    void CItemBanana::Create(const bcs::FVEC3 &pos)
    {
        CItem::Create(pos);
        CreateMdlObj(5, 0, 0, 0x20);

        CollAtkCreate(1);
        CollAtkSetAtkType(0, 0x12104);
        CollAtkSetRadius(0, 0.5f);
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
        CollAtkSetHitCamVibIDAll(0x32);

        CollBodyCreate(1);
        CollBodySetTargetFlag(0, 0x10);
        CollBodySetPushEnableAll(1);
    }

    CItemBanana::~CItemBanana() {}

    float CItemBanana::GetBodyCollRadius() const
    {
        return 0.4f * GetParamScale();
    }

    float CItemBanana::GetParamHeight() const
    {
        return 0.2f * GetParamScale();
    }

    float CItemBanana::GetParamAttachHeight() const
    {
        return GetParamHeight();
    }

    float CItemBanana::GetAtkCollRadius() const
    {
        return m_AtkColl.GetRadius(0);
    }

    void CItemBanana::CtorStateDrop()
    {
        m_fStateTimer = GetParamDropLife();
        m_fGravity = GetParamGravity();
        m_nFlag &= ~0x4;
        m_nFlag &= ~0x2000;
        m_nFlag &= ~0x100;
        m_nFlag |= 0x400;
        CollBodySetEnableAll(1);
        m_nSubStep = 0;
    }

    // Falls until it lands (flag 0x100), then stops with a wobble.
    void CItemBanana::LoopStateDrop()
    {
        DoMove();
        if (m_nSubStep == 0 && (m_nFlag & 0x100)) {
            m_vVelocity.x = 0.0f;
            m_vVelocity.y = 0.0f;
            m_vVelocity.z = 0.0f;
            DoShakeInit(0.25f, bcs::DEG_TO_RAD(45.0f));
            m_nFlag |= 0x2004;
            m_nSubStep++;
        }
    }

    void CItemBanana::DoThrowInit(const bcs::FVEC3 &rPos, int)
    {
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
        SetVelParabola_(rPos, m_fParamB8, m_fParamBC, m_fParamC0);
    }

    void CItemBanana::CtorStateThrow()
    {
        m_fStateTimer = GetParamMainLife();
        m_nFlag &= ~0x4;
        m_nFlag |= 0x400;
        CollAtkSetPos(0, m_vPos);
        CollAtkSetRadius(0, 0.32f * GetParamScale());
        CollAtkSetEnable(0, 1);
        CollBodySetEnableAll(1);
        m_nSubStep = 0;
    }

    // Flies in a parabola; on landing it stops and lies there. Anything
    // its attack hits breaks it.
    void CItemBanana::LoopStateThrow()
    {
        DoMove();
        CollAtkSetPos(0, m_vPos);
        if (m_nSubStep == 0 && (m_nFlag & 0x100)) {
            m_vVelocity.x = 0.0f;
            m_vVelocity.y = 0.0f;
            m_vVelocity.z = 0.0f;
            CollBodySetEnableAll(0);
            m_nFlag |= 0x2004;
            DoTouchDown();
            DoShakeInit(0.25f, bcs::DEG_TO_RAD(45.0f));
            m_nSubStep++;
        }
        if (CollAtkIsAtkHit(0)) {
            ChangeStateBreak();
        }
    }

    // Hit: collisions off, it bounces away along the attack's direction.
    void CItemBanana::CtorStateBreak()
    {
        CollBodySetEnableAll(0);
        CollAtkSetEnable(0, 0);
        bcs::BCS_VecZ_RotY(m_vVelocity, -0.05f, CollAtkGetUnkA0(0));
        m_vVelocity.y = 0.125f;
        m_fGravity = -0.00625f;
        m_fUnk190 = 30.0f;
    }

    // Flies off and fades out over the 30 frames set above, then goes.
    void CItemBanana::LoopStateBreak()
    {
        float fRate = GetFrameRateF();
        float fGravity = m_fGravity * fRate;
        bcs::FVEC3 move;

        bcs::FVEC3Scale(&move, &m_vVelocity, fRate);
        move.y += 0.5f * (fGravity * fRate);
        m_vVelocity.y += fGravity;
        bcs::FVEC3Add(&m_vPos, &m_vPos, &move);

        if (m_fUnk190 > 0.0f) {
            m_fUnk190 -= fRate;
        }
        if (m_fUnk190 <= 0.0f) {
            DoDelete();
        } else {
            SetMdlAlpha((1.0f / 30.0f) * m_fUnk190);
        }
    }
}
