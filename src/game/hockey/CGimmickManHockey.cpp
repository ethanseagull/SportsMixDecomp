// CGimmickManHockey and the GIMMICK_HOCKEY coin effects
//
// The gimmick manager of the hockey game creates the coin, "?" panel and
// item managers and the two coin effects: CEffectCoinGet (a collected coin
// jumps up, hovers and fades) and CEffectCoinScore (scored coins fly out
// from the goal and fade).

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
#include "game/msw/CGimmickManHockey.hpp"
#include "game/msw/CItemManHockey.hpp"
#include "game/msw/CQPanelManHockey.hpp"
#include "nw4r/math/triangular.h"

namespace msw {
    namespace GIMMICK_HOCKEY {

        // A collected coin: jumps up from rPos, hovers, falls back and fades.
        class CEffectCoinGet : public CEffectCoinGetBase {
            public:
                CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);
                virtual ~CEffectCoinGet();
                virtual void Main();
        };

        // A scored coin: waits m_fDelay frames hidden, then flies out from
        // the goal in direction fRotY, pushed along rDir, and fades.
        class CEffectCoinScore : public CEffectCoinGetBase {
            public:
                CEffectCoinScore(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir, float fRotY,
                                 long nIdx);
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

// (guessed helper) The multiplication below is not folded into one constant
// in the original, so the factor most likely came from an inline function
// applied to a local (see notes/toolchain.md); the name is a guess.
static inline float Twice(float value)
{
    return 2.0f * value;
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

inline msw::GIMMICK_HOCKEY::CEffectCoinGet::CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
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
        m_hEffect = CEffectCtrl::CreateAttachEffect(0x4DE, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
    }
}

msw::CGimmickMan *msw::CGimmickManHockey::CreateInstance(bcs::CTask *pParent)
{
    CGimmickManHockey *pInstance = new (0) CGimmickManHockey(pParent);
    SetInstance_(pInstance);
    pInstance->Initialize_();
    return GetInstance_();
}

msw::CGimmickManHockey::~CGimmickManHockey()
{
}

void msw::CGimmickManHockey::Initialize_()
{
    m_pDropCoinMan = new (0) CDropCoinMan(this);
    m_pQPanelMan = new (0) CQPanelManHockey(this);
    m_pItemMan = new (0) CItemManHockey(this);
}

void msw::CGimmickManHockey::SetUp_()
{
    CGimmickMan::SetUp_();
}

void msw::CGimmickManHockey::AllReset_()
{
    CGimmickMan::AllReset_();
}

long msw::CGimmickManHockey::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, int a, int b, int c, long *pResult,
                                         CItem **ppItem)
{
    return CGimmickMan::QPanelCheck_(rPos, nTeam, a, b, c, pResult, ppItem);
}

long msw::CGimmickManHockey::QPanelChoicePopupItem_(long nPlayer, int n)
{
    if (m_pQPanelMan != 0) {
        return m_pQPanelMan->ChoicePopupItem(nPlayer, n);
    }
    return -1;
}

long msw::CGimmickManHockey::QPanelChoicePopupCoin_(long nPlayer)
{
    if (m_pQPanelMan != 0) {
        return m_pQPanelMan->ChoicePopupCoin(nPlayer);
    }
    return -1;
}

long msw::CGimmickManHockey::CreateDropCoin_(const bcs::FVEC3 &rPos, long nNum)
{
    if (m_pDropCoinMan != 0) {
        return m_pDropCoinMan->CreateCoin(rPos, nNum, 10.0f, 1.0f);
    }
    return 0;
}

void msw::CGimmickManHockey::EffectCoinGetCreate_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
{
    new (0x100) GIMMICK_HOCKEY::CEffectCoinGet(CGame::GetTempParent(), rPos, type);
}

// The scored coins (at most 16) fly out of the goal at rPos; rDir is the
// direction of the shot. A burst effect (a bigger one from five coins on)
// turned along rDir and a sound go with them.
void msw::CGimmickManHockey::EffectCoinScoreCreate_(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rDir, long nNum)
{
    if (nNum <= 0) {
        return;
    }

    bcs::FVEC3 pos = rPos;
    long nEffect = 0x4EF;
    if (nNum >= 5) {
        nEffect = 0x4F0;
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

    if (nNum > 16) {
        nNum = 16;
    }
    for (long i = 0; i < nNum; i++) {
        bcs::FVEC3 coinPos = pos;
        float fRotY = NormalizeRad(bcs::BCS_RandMax(2.0f * 3.14159265f));

        coinPos.x += 0.1f * nw4r::math::SinRad(fRotY);
        coinPos.z += 0.1f * nw4r::math::CosRad(fRotY);
        new (0x100) GIMMICK_HOCKEY::CEffectCoinScore(CGame::GetTempParent(), coinPos, rDir, fRotY, i);
    }
}

msw::GIMMICK_HOCKEY::CEffectCoinGet::~CEffectCoinGet()
{
}

void msw::GIMMICK_HOCKEY::CEffectCoinGet::Main()
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

msw::GIMMICK_HOCKEY::CEffectCoinScore::CEffectCoinScore(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
                                                        const bcs::FVEC3 &rDir, float fRotY, long nIdx)
    : CEffectCoinGetBase(pParent), m_fDelay(0.0f), m_nStep(0), m_fScaleSpeed(1.0f)
{
    CreateMdl_(rPos, DROP_COIN::COIN_TYPE_COIN, 0);
    m_fLife = 30.0f;
    m_fLife += bcs::BCS_OrgRandMax(20.0f);
    m_fScale = 0.0f;
    float fRotSpeed = 20.0f;
    m_fRotSpeed = bcs::DEG_TO_RAD(fRotSpeed);
    m_fScale = m_fScale > 1.0f ? 1.0f : (m_fScale < 0.0f ? 0.0f : m_fScale);
    m_fScaleSpeed = 0.0625f;
    if (m_pMdl != 0) {
        m_pMdl->OnFlag(bcs::CScnObj::FLAG_HIDE_40 | bcs::CScnObj::FLAG_HIDE_80);
        float fRotY = bcs::BCS_OrgRandMax(2.0f * 3.14159265f);
        bcs::CMdlObj *pMdl = m_pMdl;
        pMdl->m_fRotY = NormalizeRad(fRotY);
        m_pMdl->m_vScale.Set(m_fScale, m_fScale, m_fScale);
    }
    m_fDelay = 0.7f * nIdx;

    // Speeds are given per second.
    float fSpeed = 3.0f;
    fSpeed += bcs::BCS_OrgRandMax(1.0f);
    bcs::BCS_VecZ_RotY(m_Vel, (1.0f / 60.0f) * fSpeed, NormalizeRad(fRotY));
    m_Vel.y = 20.0f / 60.0f;
    m_Vel.y += (1.0f / 60.0f) * bcs::BCS_OrgRandMax(10.0f);
    m_fGravity = -1.0f / 60.0f;

    // The shot pushes the coin along.
    float fPush = 0.8f * m_Vel.y;
    bcs::FVEC3 vPush;
    vPush.x = rDir.x * fPush;
    vPush.z = rDir.z * fPush;
    m_Vel.x += vPush.x;
    m_Vel.z += vPush.z;
}

msw::GIMMICK_HOCKEY::CEffectCoinScore::~CEffectCoinScore()
{
}

void msw::GIMMICK_HOCKEY::CEffectCoinScore::Main()
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
    float fRotAccel = -0.3f;
    m_fRotSpeed += bcs::DEG_TO_RAD(fRotAccel) * rate;

    if (m_pMdl != 0) {
        bcs::FVEC3 pos = m_pMdl->m_vTrans;
        float dv = m_fGravity * rate;
        bcs::FVEC3 move;

        bcs::FVEC3Scale(&move, &m_Vel, rate);
        m_Vel.y += dv;
        move.y += 0.5f * (dv * rate);
        bcs::FVEC3Add(&pos, &pos, &move);

        float floorY = 0.25678f * m_fScale;
        if (pos.y < floorY && m_Vel.y < 0.0f) {
            pos.y = floorY;
            m_Vel.y *= -0.5f;
        }
        m_pMdl->m_vTrans = pos;
        float turn = m_fRotSpeed * rate;
        m_pMdl->m_fRotY = NormalizeRad(m_pMdl->m_fRotY + turn);
        m_pMdl->m_vScale.Set(m_fScale, m_fScale, m_fScale);
        if (m_fLife < 8.0f) {
            m_pMdl->SetAlphaRateF(0.125f * m_fLife);
        }
    }
    if (m_fLife <= 0.0f) {
        Delete();
    }
}
