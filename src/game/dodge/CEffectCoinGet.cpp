// CGimmickManDodge, CGimmickManDodgeDodgeOrDie and GIMMICK_DODGE::CEffectCoinGet
// (ported from BossF game/boss/CEffectCoinScore.cpp)
//
// The gimmick managers of the dodgeball game create the coin, "?" panel and
// item managers; the plain one also makes the coin effect CEffectCoinGet (a
// collected coin jumps up, hovers and fades).

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "game/msw/CEffectCoinGetBase.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGimmickManDodge.hpp"
#include "game/msw/CItemDodgeDD.hpp"
#include "game/msw/CItemManDodge.hpp"
#include "game/msw/CQPanelManDodge.hpp"

namespace msw {
    namespace GIMMICK_DODGE {

        // A collected coin: jumps up from rPos, hovers, falls back and fades.
        class CEffectCoinGet : public CEffectCoinGetBase {
            public:
                CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);
                virtual ~CEffectCoinGet();
                virtual void Main();
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

inline msw::GIMMICK_DODGE::CEffectCoinGet::CEffectCoinGet(bcs::CTask *pParent, const bcs::FVEC3 &rPos,
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
        m_hEffect = CEffectCtrl::CreateAttachEffect(0x4E6, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
    }
}

msw::CGimmickMan *msw::CGimmickManDodge::CreateInstance(bcs::CTask *pParent)
{
    CGimmickManDodge *pInstance = new (0) CGimmickManDodge(pParent);
    SetInstance_(pInstance);
    pInstance->Initialize_();
    return GetInstance_();
}

msw::CGimmickManDodge::~CGimmickManDodge()
{
}

void msw::CGimmickManDodge::Initialize_()
{
    m_pDropCoinMan = new (0) CDropCoinMan(this);
    m_pQPanelMan = new (0) CQPanelManDodge(this);
    if (CGame::g_GameWork.GetGameType() == 2) {
        m_pItemMan = new (0) CItemManDodgeDD(this);
    } else {
        m_pItemMan = new (0) CItemManDodge(this);
    }
}

void msw::CGimmickManDodge::SetUp_()
{
    CGimmickMan::SetUp_();
}

void msw::CGimmickManDodge::AllReset_()
{
    CGimmickMan::AllReset_();
}

long msw::CGimmickManDodge::QPanelCheck_(const bcs::FVEC3 &rPos, long nTeam, int a, int b, int c, long *pResult,
                                        CItem **ppItem)
{
    return CGimmickMan::QPanelCheck_(rPos, nTeam, a, b, c, pResult, ppItem);
}

long msw::CGimmickManDodge::QPanelChoicePopupItem_(long nPlayer, int n)
{
    if (m_pQPanelMan != 0) {
        return m_pQPanelMan->ChoicePopupItem(nPlayer, n);
    }
    return -1;
}

long msw::CGimmickManDodge::QPanelChoicePopupCoin_(long nPlayer)
{
    if (m_pQPanelMan != 0) {
        return m_pQPanelMan->ChoicePopupCoin(nPlayer);
    }
    return -1;
}

void msw::CGimmickManDodge::EffectCoinGetCreate_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type)
{
    new (0x100) GIMMICK_DODGE::CEffectCoinGet(CGame::GetTempParent(), rPos, type);
}

msw::GIMMICK_DODGE::CEffectCoinGet::~CEffectCoinGet()
{
}

void msw::GIMMICK_DODGE::CEffectCoinGet::Main()
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

msw::CGimmickManDodgeDodgeOrDie::~CGimmickManDodgeDodgeOrDie()
{
}

msw::CGimmickMan *msw::CGimmickManDodgeDodgeOrDie::CreateInstance(bcs::CTask *pParent)
{
    CGimmickManDodgeDodgeOrDie *pInstance = new (0) CGimmickManDodgeDodgeOrDie(pParent);
    SetInstance_(pInstance);
    pInstance->Initialize_();
    return GetInstance_();
}

void msw::CGimmickManDodgeDodgeOrDie::Initialize_()
{
    m_pDropCoinMan = new (0) CDropCoinMan(this);
    m_pQPanelMan = new (0) CQPanelManDodgeDodgeOrDie(this);
    m_pItemMan = new (0) CItemManDodgeDD(this);
}
