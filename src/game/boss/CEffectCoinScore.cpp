// CGimmickManBoss and the GIMMICK_BOSS coin effects
//
// The gimmick manager of the boss game creates the coin managers and the two
// coin effects: CEffectCoinGet (a collected coin jumps up, hovers and fades)
// and CEffectCoinScore (scored coins spread out around a point).

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCoinGetBase.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGimmickManBoss.hpp"
#include "game/msw/CItemManBoss.hpp"
#include "game/msw/CQPanelManBoss.hpp"
#include "nw4r/math/triangular.h"

namespace msw {
    namespace GIMMICK_BOSS {

        // A collected coin: jumps up from rPos, hovers, falls back and fades.
        class CEffectCoinGet : public CEffectCoinGetBase {
            public:
                CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);
                virtual ~CEffectCoinGet();
                virtual void Main();
        };

        // A scored coin: waits m_fDelay frames hidden, then flies out from
        // the score point in direction fRotY and fades.
        class CEffectCoinScore : public CEffectCoinGetBase {
            public:
                CEffectCoinScore(bcs::CTask *pParent, const bcs::FVEC3 &rPos, float fRotY, long nIdx);
                virtual ~CEffectCoinScore();
                virtual void Main();

            private:
                float m_fDelay;      // 0x64, frames before the coin shows
                long m_nStep;        // 0x68, 0 while waiting
                float m_fScaleSpeed; // 0x6C, added to m_fScale per frame
        };
    }
}

// The turning speed of a collected coin goes from the first to the second.
static const float s_CoinGetRotSpeedStart = bcs::DEG_TO_RAD(30.0f);
static const float s_CoinGetRotSpeedEnd = bcs::DEG_TO_RAD(10.0f);

// (guessed helpers) The two multiplications below are not folded into one
// constant in the original, so the factor most likely came from an inline
// function; the names are guesses.
static inline float Twice(float value)
{
    return value * 2.0f;
}

static inline float PerFrame(float perSecond)
{
    return perSecond * (1.0f / 60.0f);
}

// (guessed helper) rad brought into [0, 2 pi].
static inline float NormalizeRad(float rad)
{
    while (rad > 2.0f * 3.14159265f) {
        rad -= 2.0f * 3.14159265f;
    }
    while (rad < 0.0f) {
        rad += 2.0f * 3.14159265f;
    }
    return rad;
}

inline msw::GIMMICK_BOSS::CEffectCoinGet::CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
                                                         DROP_COIN::CoinType type)
    : CEffectCoinGetBase(pParent)
{
    CreateMdl_(rPos, type, 1);
    m_fLife = 68.0f;
    m_fWait = 42.0f;
    m_fTopY = rPos.y + 2.3f;
    m_fRotSpeed = s_CoinGetRotSpeedStart;
    bcs::BCS_Phys_CalcYVG(Twice(20.0f), rPos.y, rPos.y, m_fTopY, m_Vel.y, m_fGravity);
    if (m_pMdl != 0) {
        m_hEffect = CEffectCtrl::CreateAttachEffect(0x56A, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
    }
}

msw::CGimmickMan *msw::CGimmickManBoss::CreateInstance(bcs::CTask *pParent)
{
    CGimmickManBoss *pInstance = new (0) CGimmickManBoss(pParent);
    SetInstance_(pInstance);
    pInstance->Initialize_();
    return GetInstance_();
}

msw::CGimmickManBoss::~CGimmickManBoss()
{
}

void msw::CGimmickManBoss::Initialize_()
{
    m_pDropCoinMan = new (0) CDropCoinMan(this);
    m_pQPanelMan = new (0) CQPanelManBoss(this);
    m_pItemMan = new (0) CItemManBoss(this);
}

void msw::CGimmickManBoss::SetUp_()
{
    CGimmickMan::SetUp_();
}

void msw::CGimmickManBoss::AllReset_()
{
    CGimmickMan::AllReset_();
}

long msw::CGimmickManBoss::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, int a, int b, int c, long *pResult,
                                        CItem **ppItem)
{
    return CGimmickMan::QPanelCheck_(rPos, nTeam, a, b, c, pResult, ppItem);
}

long msw::CGimmickManBoss::QPanelChoicePopupCoin_(long nPlayer)
{
    if (m_pQPanelMan != 0) {
        return m_pQPanelMan->ChoicePopupCoin(nPlayer);
    }
    return -1;
}

msw::CDropCoin *msw::CGimmickManBoss::CreateDropCoin_(const bcs::FVEC3 &rPos, long nNum)
{
    if (m_pDropCoinMan != 0) {
        return m_pDropCoinMan->CreateCoin(rPos, nNum, 3.0f, 1.0f);
    }
    return 0;
}

void msw::CGimmickManBoss::EffectCoinGetCreate_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
{
    new (0x100) GIMMICK_BOSS::CEffectCoinGet(CGame::GetTempParent(), rPos, type);
}

void msw::CGimmickManBoss::EffectCoinScoreCreate_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &, long nNum)
{
    if (nNum > 0) {
        bcs::FVEC3 center(rPos.x, 0.0f, rPos.z);

        if (nNum > 16) {
            nNum = 16;
        }
        for (long i = 0; i < nNum; i++) {
            bcs::FVEC3 pos = center;
            float fRotY = NormalizeRad(bcs::BCS_RandMax(2.0f * 3.14159265f));

            pos.x += 0.1f * nw4r::math::SinRad(fRotY);
            pos.z += 0.1f * nw4r::math::CosRad(fRotY);
            new (0x100) GIMMICK_BOSS::CEffectCoinScore(CGame::GetTempParent(), pos, fRotY, i);
        }
    }
}

msw::GIMMICK_BOSS::CEffectCoinGet::~CEffectCoinGet()
{
}

void msw::GIMMICK_BOSS::CEffectCoinGet::Main()
{
    float rate = GetFrameRateF();

    if (m_fLife > 0.0f) {
        m_fLife -= rate;
        m_fLife = m_fLife < 0.0f ? 0.0f : m_fLife;
    }
    if (m_fScale < 1.0f) {
        m_fScale += 0.0625f * rate;
        m_fScale = m_fScale > 1.0f ? 1.0f : (m_fScale < 0.0f ? 0.0f : m_fScale);
    }
    m_fRotSpeed += rate * ((1.0f / 34.0f) * (s_CoinGetRotSpeedEnd - s_CoinGetRotSpeedStart));
    m_fRotSpeed = m_fRotSpeed > s_CoinGetRotSpeedStart
                      ? s_CoinGetRotSpeedStart
                      : (m_fRotSpeed < s_CoinGetRotSpeedEnd ? s_CoinGetRotSpeedEnd : m_fRotSpeed);

    if (m_pMdl != 0) {
        bcs::FVEC3 pos = m_pMdl->m_vTrans;

        switch (m_nState) {
            case 0: {
                // Rise until the speed turns.
                float dv = m_fGravity * rate;
                bcs::FVEC3 move;

                bcs::FVEC3Scale(&move, &m_Vel, rate);
                move.y += 0.5f * (dv * rate);
                m_Vel.y += dv;
                bcs::FVEC3Add(&pos, &pos, &move);
                if (m_Vel.y < 0.0f) {
                    m_Vel.y = 0.0f;
                    m_nState++;
                }
                break;
            }
            case 1:
                // Hover.
                m_fWait -= rate;
                if (m_fWait <= 0.0f) {
                    m_nState++;
                }
                break;
            default: {
                // Fall.
                float dv = m_fGravity * rate;
                bcs::FVEC3 move;

                bcs::FVEC3Scale(&move, &m_Vel, rate);
                move.y += 0.5f * (dv * rate);
                m_Vel.y += dv;
                bcs::FVEC3Add(&pos, &pos, &move);
                break;
            }
        }

        float floorY = 0.25678f * m_fScale;
        if (pos.y < floorY && m_Vel.y < 0.0f) {
            pos.y = floorY;
            m_Vel.y *= -0.5f;
        }
        m_pMdl->m_vTrans = pos;
        m_pMdl->m_fRotY = NormalizeRad(m_pMdl->m_fRotY + m_fRotSpeed * rate);
        float scale = 1.9f * m_fScale;
        m_pMdl->m_vScale.Set(scale, scale, scale);
        if (m_fLife < 4.0f) {
            m_pMdl->SetAlphaRateF(0.25f * m_fLife);
        }
    }
    if (m_fLife <= 0.0f) {
        Delete();
    }
}

msw::GIMMICK_BOSS::CEffectCoinScore::CEffectCoinScore(bcs::CTask *pParent, const bcs::FVEC3 &rPos, float fRotY,
                                                      long nIdx)
    : CEffectCoinGetBase(pParent), m_fDelay(0.0f), m_nStep(0), m_fScaleSpeed(1.0f)
{
    CreateMdl_(rPos, DROP_COIN::COIN_TYPE_COIN, 1);
    m_fLife = 70.0f;
    m_fLife += bcs::BCS_OrgRandMax(1.0f);
    m_fScale = 0.0f;
    m_fRotSpeed = bcs::DEG_TO_RAD(23.0f);
    m_fScale = m_fScale > 1.0f ? 1.0f : (m_fScale < 0.0f ? 0.0f : m_fScale);
    m_fScaleSpeed = 0.0625f;
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        m_pMdl->m_fRotY = NormalizeRad(bcs::BCS_OrgRandMax(2.0f * 3.14159265f));
        float scale = 1.1f * m_fScale;
        m_pMdl->m_vScale.Set(scale, scale, scale);
    }
    m_fDelay = nIdx;
    bcs::BCS_VecZ_RotY(m_Vel, PerFrame(2.0f), NormalizeRad(fRotY));
    m_Vel.y = 8.0f / 60.0f;
    m_Vel.y += PerFrame(bcs::BCS_OrgRandMax(4.0f));
    m_fGravity = -0.3f / 60.0f;
}

msw::GIMMICK_BOSS::CEffectCoinScore::~CEffectCoinScore()
{
}

void msw::GIMMICK_BOSS::CEffectCoinScore::Main()
{
    if (m_nStep == 0) {
        m_fDelay -= GetFrameRateF();
        if (m_fDelay <= 0.0f) {
            m_nStep++;
            if (m_pMdl != 0) {
                m_pMdl->OffFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
            }
        }
        return;
    }

    float rate = GetFrameRateF();

    if (m_fLife > 0.0f) {
        m_fLife -= rate;
        m_fLife = m_fLife < 0.0f ? 0.0f : m_fLife;
    }
    if (m_fScale < 1.0f) {
        m_fScale += m_fScaleSpeed * rate;
        m_fScale = m_fScale > 1.0f ? 1.0f : (m_fScale < 0.0f ? 0.0f : m_fScale);
    }
    m_fRotSpeed += bcs::DEG_TO_RAD(0.0f) * rate;

    if (m_pMdl != 0) {
        bcs::FVEC3 pos = m_pMdl->m_vTrans;
        float dv = m_fGravity * rate;
        bcs::FVEC3 move;

        bcs::FVEC3Scale(&move, &m_Vel, rate);
        move.y += 0.5f * (dv * rate);
        m_Vel.y += dv;
        bcs::FVEC3Add(&pos, &pos, &move);

        float floorY = 0.25678f * m_fScale;
        if (pos.y < floorY && m_Vel.y < 0.0f) {
            pos.y = floorY;
            m_Vel.y *= -0.5f;
        }
        m_pMdl->m_vTrans = pos;
        m_pMdl->m_fRotY = NormalizeRad(m_pMdl->m_fRotY + m_fRotSpeed * rate);
        float scale = 1.1f * m_fScale;
        m_pMdl->m_vScale.Set(scale, scale, scale);
        if (m_fLife < 8.0f) {
            m_pMdl->SetAlphaRateF(0.125f * m_fLife);
        }
    }
    if (m_fLife <= 0.0f) {
        Delete();
    }
}
