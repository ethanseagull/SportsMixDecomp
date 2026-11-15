// msw::CItemBomb and msw::CBombExplosion: the bomb item and its blast. One
// file in the original: the two share one float pool, the blast's vtable
// sits just before the bomb's, and the bomb's weak tail (GetItemType, the
// destructor thunk) comes after the blast's code.
#include "game/msw/CItemKinds.hpp"
#include "game/msw/CBombExplosion.hpp"

#include "FastCast.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollUnit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CGimmickMan.hpp"
#include "nw4r/g3d/ResNode.hpp"
#include "nw4r/math/triangular.h"

#include <math.h>

namespace {

    // The blast's sound (name guessed).
    const long SE_BOMB_EXPLOSION = 0xC51;

    // (name guessed) The radius the blast effect is made for.
    const float BOMB_EFFECT_BASE_SIZE = 3.0f;

    // (names guessed) The attack unit starts at this fraction of the full
    // radius and grows to it in m_fGrowTime frames.
    const float BOMB_START_SCALE = 0.125f;
    const float BOMB_GROW_SCALE = 1.0f - BOMB_START_SCALE;

    // WrapRad and Clamp as the flash in LoopStateThrow uses them: the
    // original rounds both results to single again (an frsp after each), so
    // they are guessed to return double there. Same values as the float
    // versions below.
    inline double WrapRadD(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    inline double ClampD(float v, float lo, float hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }

    // (name guessed) v kept between lo and hi.
    template <typename T> static inline T Clamp(T v, T lo, T hi)
    {
        return v > hi ? hi : (v < lo ? lo : v);
    }

    // Brings an angle in radians into [0, 2 pi].
    inline float WrapRad(float rad)
    {
        while (rad > 6.2831855f) {
            rad -= 6.2831855f;
        }
        while (rad < 0.0f) {
            rad += 6.2831855f;
        }
        return rad;
    }

    // rVel reflected on the plane with normal rNormal, unless it already
    // moves away from it.
    inline void Reflect(bcs::FVEC3 &rVel, const bcs::FVEC3 &rNormal)
    {
        float dot = 2.0f * bcs::FVEC3Dot(&rVel, &rNormal);
        if (dot > 0.0f) {
            return; // already moving away
        }
        bcs::FVEC3 work;
        bcs::FVEC3Scale(&work, &rNormal, dot);
        bcs::FVEC3Sub(&rVel, &rVel, &work);
    }

    // The fuse sound (name guessed).
    const long SE_BOMB_FUSE = 0xC4F;

    // (name guessed) Frames from the throw to the blast.
    const float BOMB_FUSE_FRAME = 60.0f;

    void AttackHitCallback(const msw::CCollAtkUnit *pAtk, const msw::CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                           unsigned long flag, void *pArg);
}

namespace msw {

    // --- the bomb ---

    CItemBomb::CItemBomb(CItemMan *pMan) : CItem(pMan)
    {
        m_fExplodeRestFrame = 0.0f;
        m_fFlashPhase = 0.0f;
        m_bEffect = 0;
        m_hEffect = 0;
        m_bExplode = 0;
    }

    CItemBomb::~CItemBomb()
    {
        StopEffect_();
    }

    void CItemBomb::Create(const bcs::FVEC3 &pos)
    {
        CItem::Create(pos);
        CreateMdlObj(3, 0, 1, 0x20);

        CollBodyCreate(1);
        CollBodySetPushEnableAll(1);
    }

    // The fuse: a spark on the model's "body" node and its sound.
    void CItemBomb::StartEffect_()
    {
        if (m_pMdl != 0) {
            if (m_bEffect == 0) {
                unsigned long nNode = 0;
                nw4r::g3d::ResNode node = m_pMdl->GetResMdl().GetResNode("body");
                if (node.IsValid()) {
                    nNode = node.GetID();
                }
                bcs::FVEC3 ofs(0.0f, 0.1f, 0.0f);
                m_bEffect = CEffectCtrl::CreateAttachEffect(4, m_pMdl, &ofs, nNode, 1, 1, 0, 3, 0, 0, 0);
            }
            if (m_hEffect == 0) {
                m_hEffect = PlaySe(SE_BOMB_FUSE, 0);
            }
        }
    }

    void CItemBomb::StopEffect_()
    {
        if (m_bEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_bEffect);
            m_bEffect = 0;
        }
        if (m_hEffect != 0) {
            bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hEffect));
            if (pSe != 0) {
                pSe->Stop(0.0f);
            }
            m_hEffect = 0;
        }
    }

    float CItemBomb::GetBodyCollRadius() const
    {
        return 0.4f * GetParamScale();
    }

    float CItemBomb::GetParamHeight() const
    {
        return 0.4f * GetParamScale();
    }

    float CItemBomb::GetParamAttachHeight() const
    {
        return GetParamHeight();
    }

    float CItemBomb::GetAtkCollRadius() const
    {
        return GetBodyCollRadius();
    }

    void CItemBomb::CtorStateDrop()
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

    // Lands (flag 0x100) with a wobble; afterwards it turns to face the
    // way it rolls.
    void CItemBomb::LoopStateDrop()
    {
        DoMove();
        switch (m_nSubStep) {
            case 0:
                if (m_nFlag & 0x100) {
                    DoShakeInit(0.1f, bcs::DEG_TO_RAD(45.0f));
                    m_vVelocity.y = 0.0f;
                    m_vVelocity.z = 0.0f;
                    m_vVelocity.x = 0.0f;
                    m_nFlag |= 0x2004;
                    m_fUnk190 = 16.0f;
                    m_nSubStep++;
                }
                break;
            case 1:
                break;
            default:
                RotateYRate(WrapRad(nw4r::math::Atan2Rad(m_vVelocity.x, m_vVelocity.z)), 0.1f * GetFrameRateF());
                break;
        }

        if (IsFlag(0x200)) {
            m_nFlag &= ~0x200;
        }
    }

    void CItemBomb::CtorStateAttach()
    {
        CollBodySetEnableAll(0);
        AnimPlay(0, 1, 0.0f, 0.0f);
    }

    void CItemBomb::DoThrowInit(const bcs::FVEC3 &rPos, int)
    {
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);
        SetVelParabola_(rPos, m_fParamB8, m_fParamBC, m_fParamC0);
    }

    void CItemBomb::CtorStateThrow()
    {
        m_fStateTimer = GetParamMainLife();
        m_nFlag &= ~0x2000;
        m_nFlag &= ~0x4;
        m_nFlag |= 0x400;
        CollBodySetEnableAll(1);
        m_fExplodeRestFrame = BOMB_FUSE_FRAME;
        m_fFlashPhase = 0.0f;
        StartEffect_();
        m_fUnk160 = -0.5f;
        m_nSubStep = 0;
    }

    // Flies until it hits a wall or the ground (flag 0x80) or something
    // pushes it, flashing red faster as the fuse burns down; then blows up.
    void CItemBomb::LoopStateThrow()
    {
        switch (m_nSubStep) {
            case 0:
                DoMove();
                if (m_nFlag & 0x80) {
                    DoShakeInit(0.1f, bcs::DEG_TO_RAD(45.0f));
                    m_vVelocity.x *= 0.5f;
                    m_vVelocity.z *= 0.5f;
                    m_bExplode = 1;
                }

                if (m_fExplodeRestFrame > 0.0f) {
                    m_fExplodeRestFrame -= GetFrameRateF();
                    if (m_fExplodeRestFrame < 0.0f) {
                        m_fExplodeRestFrame = 0.0f;
                    }
                }

                {
                    float step = 0.4f * GetFrameRateF();
                    m_fFlashPhase += (1.0f - (1.0f / BOMB_FUSE_FRAME) * m_fExplodeRestFrame) * step;
                    m_fFlashPhase = fmod(m_fFlashPhase, 1.0);

                    float flash = ClampD(fabs(nw4r::math::SinRad(WrapRadD(3.1415927f * m_fFlashPhase))), 0.0f, 1.0f);

                    bcs::BCS_Color base;
                    unsigned char level;
                    F32ToU8(255.0f - 200.0f * flash, &level);
                    base.r = base.g = base.b = level;
                    base.a = 0xFF;
                    if (m_pMdl != 0) {
                        m_pMdl->SetSilhouetteColor(bcs::BCS_Color(0xFF2020FF));
                        m_pMdl->SetBaseColor(base);
                    }
                }

                if (m_bExplode) {
                    StopEffect_();
                    m_vVelocity.x = 0.0f;
                    m_vVelocity.y = 0.0f;
                    m_vVelocity.z = 0.0f;
                    m_nFlag &= ~0x4;
                    DoExplode_();
                    DoDelete();
                    m_nSubStep++;
                }
                break;
            case 1:
                DoMove();
                break;
        }
    }

    // (argument names guessed) What CBombExplosion's constructor takes.
    void CItemBomb::GetExplosionParam(float &rDownFrame, float &rRadius, float &rUnk98, long &rUnk9C, long &rUnk9E,
                                      unsigned long &rNoHitFlag)
    {
        rDownFrame = m_fParamHitEffectFrame;
        rRadius = m_fExplosionCollSize;
        rUnk98 = m_fParamHitDropCoinRate;
        rUnk9C = m_nParamHitDropCoinNumMin;
        rUnk9E = m_nParamHitDropCoinNumMax;
        rNoHitFlag = 0;
    }

    void CItemBomb::DoExplode_()
    {
        float fDownFrame;
        float fRadius;
        float fUnk98;
        long nUnk9C;
        long nUnk9E;
        unsigned long nNoHitFlag;

        GetExplosionParam(fDownFrame, fRadius, fUnk98, nUnk9C, nUnk9E, nNoHitFlag);
        CBombExplosion *pExp = new (0x100) CBombExplosion(CGame::GetTempParent(), m_vPos, 0, fDownFrame, fRadius,
                                                          fUnk98, nUnk9C, nUnk9E, nNoHitFlag);
        if (pExp != 0) {
            // The thrower, when a character threw it.
            long nChr = m_nUnk1A8;
            if (nChr >= 0) {
                pExp->m_bReportHit = (m_nFlag >> 16) & 1;
                pExp->m_nOwnerChr = nChr;
            }
        }
        CollBodySetEnableAll(0);
    }

    // Pushed: it bounces off and blows up.
    void CItemBomb::CallbackBodyPush_(CCollBodyUnit *pMy, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        CItem::CallbackBodyPush_(pMy, pOther, pPush);
        if (pPush != 0) {
            bcs::FVEC3 normal(*pPush);
            if (normal.x * normal.x + normal.z * normal.z > 0.0f) {
                bcs::FVEC3 flat(normal.x, 0.0f, normal.z);
                bcs::BCS_Vec_Normalize(normal, flat);
                Reflect(m_vVelocity, normal);
            }
        }
        m_bExplode = 1;
        m_fExplodeRestFrame = 0.0f;
    }

    // --- the blast ---

    CBombExplosion::CBombExplosion(bcs::CTask *pParent, const bcs::FVEC3 &rPos, unsigned long hMdl, float fDownFrame,
                                   float fRadius, float fUnk98, long nUnk9C, long nUnk9E, unsigned long nNoHitFlag)
        : bcs::CUnivTaskEx(pParent, 0x17), m_pAtkUnit(0), m_fTimer(0.0f), m_Pos(rPos), m_hMdl(hMdl),
          m_nState(STATE_CREATE), m_fRadius(fRadius), m_fDownFrame(fDownFrame), m_fUnk98(fUnk98),
          m_nUnk9C(nUnk9C), m_nUnk9E(nUnk9E), m_nNoHitFlag(nNoHitFlag), m_fUnk68(0.0f), m_fGrowTime(8.0f),
          m_fUnk70(0.0f), m_nOwnerChr(-1), m_bReportHit(0)
    {
    }

    CBombExplosion::~CBombExplosion()
    {
        if (m_pAtkUnit != 0) {
            delete m_pAtkUnit;
        }
        m_pAtkUnit = 0;
    }

    void CBombExplosion::CreateEffect_(const bcs::FVEC3 &rPos)
    {
        CGimmickMan::EffectBombExplosionCreate(rPos, 0, m_fRadius / BOMB_EFFECT_BASE_SIZE, 0);
        CGameSnd::Play(SE_BOMB_EXPLOSION, rPos, 1, -1, 0.0f, 1.0f, 0.0f);
    }

    void CBombExplosion::Create_(const bcs::FVEC3 &rPos, unsigned long hMdl)
    {
        CreateEffect_(rPos);

        m_pAtkUnit = new (0x100) CCollAtkUnit(4, 0, hMdl);
        m_pAtkUnit->m_nFlag = 0x2A;
        m_pAtkUnit->GetCollUnit()->SetPos(rPos);
        m_pAtkUnit->GetCollUnit()->SetSize(BOMB_START_SCALE * m_fRadius, 0.0f, 0.0f);
        unsigned long nAtkType = GetAtkType();
        m_pAtkUnit->GetCollUnit()->SetAtkType(nAtkType);
        m_pAtkUnit->m_nNoHitFlag = m_nNoHitFlag | GetNoHitFlag();
        m_pAtkUnit->m_fDownFrame = m_fDownFrame;
        m_pAtkUnit->m_fUnk98 = m_fUnk98;
        m_pAtkUnit->SetUnk9C(m_nUnk9C, m_nUnk9E);
        m_pAtkUnit->m_nOwnerNo = GetAtkIndex();
        m_pAtkUnit->m_fVanishPower = m_fUnk68;
        m_pAtkUnit->SetAttackHitCallback(AttackHitCallback, this);
    }

    void CBombExplosion::Main()
    {
        switch (m_nState) {
            case STATE_CREATE:
                Create_(m_Pos, m_hMdl);
                m_nState++;
                // fall through
            case STATE_GROW:
                if (m_fGrowTime > 0.0f) {
                    m_fTimer += GetFrameRateF();
                    if (m_fTimer < m_fGrowTime) {
                        float fRate = Clamp(m_fTimer / m_fGrowTime, 0.0f, 1.0f);
                        float fScale = fRate * BOMB_GROW_SCALE + BOMB_START_SCALE;
                        if (m_pAtkUnit != 0) {
                            m_pAtkUnit->GetCollUnit()->SetSize(fScale * m_fRadius, 0.0f, 0.0f);
                            m_pAtkUnit->GetCollUnit()->OnFlag10(1);
                        }
                        break;
                    }
                }
                if (m_pAtkUnit != 0) {
                    m_pAtkUnit->GetCollUnit()->SetSize(m_fRadius, 0.0f, 0.0f);
                    m_pAtkUnit->GetCollUnit()->OnFlag10(1);
                }
                if (m_fUnk70 > 0.0f) {
                    m_nState++;
                } else {
                    m_nState = STATE_END;
                }
                break;
            case STATE_HOLD:
                m_fUnk70 -= GetFrameRateF();
                if (m_fUnk70 <= 0.0f) {
                    m_nState++;
                    Delete();
                }
                break;
            default:
                Delete();
                break;
        }
    }

    unsigned long CBombExplosion::GetAtkType() const
    {
        return 0x11F01;
    }

    unsigned long CBombExplosion::GetNoHitFlag() const
    {
        return 0;
    }

    void CBombExplosion::SetAttackPower(float power)
    {
        m_fUnk68 = power;
        if (m_pAtkUnit != 0) {
            m_pAtkUnit->m_fVanishPower = m_fUnk68;
        }
    }
}

namespace {

    // Called by the attack unit when the blast hits a body. Tells the game
    // about every character the blast's owner hits and, once, that another
    // character was hit (message meanings not known).
    void AttackHitCallback(const msw::CCollAtkUnit *, const msw::CCollBodyUnit *pBody, const bcs::FVEC3 &,
                           unsigned long, void *pArg)
    {
        msw::CBombExplosion *pThis = static_cast<msw::CBombExplosion *>(pArg);

        if (pThis != 0 && pBody->GetOwnerKind() == 1 && pThis->m_nOwnerChr >= 0) {
            long nChr = pBody->m_nUnk17;
            if (nChr >= 0) {
                msw::CGame::SendMessage(0x2F, pThis->m_nOwnerChr, nChr, 3);
                if (pThis->m_bReportHit && pThis->m_nOwnerChr != nChr && pThis->m_nOwnerChr >= 0) {
                    msw::CGame::SendMessage(0x2E, 2, 0, 0);
                    pThis->m_bReportHit = 0;
                }
            }
        }
    }
}
