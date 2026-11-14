// CGimmickManBasket and the GIMMICK_BASKET coin effects
// (written from VolleyF game/volley/CGimmickManVolley.cpp and HockeyF
// game/hockey/CGimmickManHockey.cpp)
//
// The gimmick manager of the basketball game creates the coin, "?" panel and
// item managers and the two coin effects: CEffectCoinGet (a collected coin
// jumps up, hovers and fades) and CEffectCoinScore (scored coins spread out
// around the goal).

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCoinGetBase.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickManBasket.hpp"
#include "game/msw/CItemManBasket.hpp"
#include "game/msw/CQPanelManBasket.hpp"
#include "nw4r/math/triangular.h"

namespace msw {
    namespace GIMMICK_BASKET {

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
    return 2.0f * value;
}

static inline float PerFrame(float perSecond)
{
    return (1.0f / 60.0f) * perSecond;
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

inline msw::GIMMICK_BASKET::CEffectCoinGet::CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
                                                         DROP_COIN::CoinType type)
    : CEffectCoinGetBase(pParent)
{
    CreateMdl_(rPos, type, 1);
    m_fLife = 68.0f;
    m_fWait = 42.0f;
    m_fTopY = rPos.y + 2.3f;
    m_fRotSpeed = s_CoinGetRotSpeedStart;
    float fFrame = 20.0f;
    bcs::BCS_Phys_CalcYVG(Twice(fFrame), rPos.y, rPos.y, m_fTopY, m_Vel.y, m_fGravity);
    if (GetMdl() != 0) {
        m_hEffect = CEffectCtrl::CreateAttachEffect(0x4DD, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
    }
}

msw::CGimmickMan *msw::CGimmickManBasket::CreateInstance(bcs::CTask *pParent)
{
    CGimmickManBasket *pInstance = new (0) CGimmickManBasket(pParent);
    SetInstance_(pInstance);
    pInstance->Initialize_();
    return GetInstance_();
}

msw::CGimmickManBasket::~CGimmickManBasket()
{
}

void msw::CGimmickManBasket::Initialize_()
{
    m_pDropCoinMan = new (0) CDropCoinMan(this);
    m_pQPanelMan = new (0) CQPanelManBasket(this);
    m_pItemMan = new (0) CItemManBasket(this);
}

void msw::CGimmickManBasket::SetUp_()
{
    CGimmickMan::SetUp_();
}

void msw::CGimmickManBasket::AllReset_()
{
    CGimmickMan::AllReset_();
}

long msw::CGimmickManBasket::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, int a, int b, int c, long *pResult,
                                         CItem **ppItem)
{
    return CGimmickMan::QPanelCheck_(rPos, nTeam, a, b, c, pResult, ppItem);
}

long msw::CGimmickManBasket::CreateDropCoin_(const bcs::FVEC3 &rPos, long nNum)
{
    if (m_pDropCoinMan != 0) {
        return m_pDropCoinMan->CreateCoin(rPos, nNum, 5.0f, 1.0f);
    }
    return 0;
}

void msw::CGimmickManBasket::EffectCoinGetCreate_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
{
    new (0x100) GIMMICK_BASKET::CEffectCoinGet(CGame::GetTempParent(), rPos, type);
}

// nNum coins scored at rPos, the goal; rDir turns the score effect.
void msw::CGimmickManBasket::EffectCoinScoreCreate_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir, long nNum)
{
    if (nNum <= 0) {
        return;
    }

    bcs::FVEC3 pos = rPos;
    long nEffect = 0x4F1;
    if (nNum >= 5) {
        nEffect = 0x4F2;
    }
    if (nEffect >= 0) {
        unsigned long hEffect = CEffectCtrl::CreateAttachEffect(nEffect, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0);
        CEffectCluster *pEffect = CEffectCtrl::GetInstance()->FindList(hEffect);
        if (pEffect != 0) {
            pEffect->SetTranslate(&pos);
            bcs::FVEC3 rot(0.0f, 0.0f, 0.0f);
            rot.y = NormalizeRad(nw4r::math::Atan2Rad(rDir.x, rDir.z));
            pEffect->SetRotateRad(&rot);
        }
    }
    CGameSnd::Play(0xBFE, pos, 1, -1, 0.0f, 1.0f, 0.0f);

    if (nNum > 25) {
        nNum = 25;
    }
    for (long i = 0; i < nNum; i++) {
        bcs::FVEC3 coinPos = pos;
        float fRotY = NormalizeRad(bcs::BCS_RandMax(2.0f * 3.14159265f));

        coinPos.x += 0.1f * nw4r::math::SinRad(fRotY);
        coinPos.z += 0.1f * nw4r::math::CosRad(fRotY);
        new (0x100) GIMMICK_BASKET::CEffectCoinScore(CGame::GetTempParent(), coinPos, fRotY, i);
    }
}

msw::GIMMICK_BASKET::CEffectCoinGet::~CEffectCoinGet()
{
}

void msw::GIMMICK_BASKET::CEffectCoinGet::Main()
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
                m_Vel.y += dv;
                move.y += 0.5f * (dv * rate);
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
                m_Vel.y += dv;
                move.y += 0.5f * (dv * rate);
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
        float turn = m_fRotSpeed * rate;
        m_pMdl->m_fRotY = NormalizeRad(m_pMdl->m_fRotY + turn);
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

msw::GIMMICK_BASKET::CEffectCoinScore::CEffectCoinScore(bcs::CTask *pParent, const bcs::FVEC3 &rPos, float fRotY,
                                                      long nIdx)
    : CEffectCoinGetBase(pParent), m_fDelay(0.0f), m_nStep(0), m_fScaleSpeed(1.0f)
{
    CreateMdl_(rPos, DROP_COIN::COIN_TYPE_COIN, 0);
    m_fLife = 80.0f;
    m_fLife += bcs::BCS_OrgRandMax(1.0f);
    m_fScale = 0.0f;
    float fRotSpeed = 23.0f;
    m_fRotSpeed = bcs::DEG_TO_RAD(fRotSpeed);
    m_fScale = m_fScale > 1.0f ? 1.0f : (m_fScale < 0.0f ? 0.0f : m_fScale);
    m_fScaleSpeed = 0.0625f;
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        float fRandRotY = bcs::BCS_OrgRandMax(2.0f * 3.14159265f);
        bcs::CMdlObj *pMdl = m_pMdl;
        pMdl->m_fRotY = NormalizeRad(fRandRotY);
        float scale = 1.1f * m_fScale;
        m_pMdl->m_vScale.Set(scale, scale, scale);
    }
    // The coins leave one after another, 0.8 frames apart.
    m_fDelay = 0.8f * nIdx;
    float fSpeed = 3.1f;
    bcs::BCS_VecZ_RotY(m_Vel, PerFrame(fSpeed), NormalizeRad(fRotY));
    m_Vel.y = 8.0f / 60.0f;
    m_Vel.y += (1.0f / 60.0f) * bcs::BCS_OrgRandMax(4.0f);
    m_fGravity = -0.5f / 60.0f;
}

msw::GIMMICK_BASKET::CEffectCoinScore::~CEffectCoinScore()
{
}

void msw::GIMMICK_BASKET::CEffectCoinScore::Main()
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
    // The turning speed does not change (an acceleration of 0 degrees).
    float fRotAcc = 0.0f;
    m_fRotSpeed += bcs::DEG_TO_RAD(fRotAcc) * rate;

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
        float turn = m_fRotSpeed * rate;
        m_pMdl->m_fRotY = NormalizeRad(m_pMdl->m_fRotY + turn);
        float scale = 1.1f * m_fScale;
        m_pMdl->m_vScale.Set(scale, scale, scale);
        if (m_fLife < 5.0f) {
            m_pMdl->SetAlphaRateF(0.2f * m_fLife);
        }
    }
    if (m_fLife <= 0.0f) {
        Delete();
    }
}
