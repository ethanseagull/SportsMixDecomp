#include "game/msw/CItemBallDodgeKinds.hpp"
#include "game/msw/CBallDodge.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "game/msw/CC_PARAM_DODGE.hpp"
#include "game/msw/CChrCoreDodge.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollAtkUnit.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameBGM.hpp"
#include "game/msw/CGameSnd.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

#include <math.h>

// The item behaviours of the dodgeball game's balls (see CItemBallDodge.hpp
// and CItemBallDodgeKinds.hpp): the interface CBallDodge asks, the plain
// ball's, and the six item kinds. One file in the original: the classes
// share one table of constants and one float pool.

// 60 degrees in radians; unused here, as in the other DodgeF files that
// have it (most likely from a header they all include).
static const float s_Unk60Deg = bcs::DEG_TO_RAD(60.0f);

namespace {

    // Tuning values of the item behaviours (names guessed from the
    // functions that read them). Written with integer initialisers: the
    // compiler then keeps each as its own object at the start of .rodata
    // and reads it from there, as the original does, instead of folding it
    // into a float literal (two of them have the same value as a literal
    // used elsewhere, and three are 30).
    const float WALL_BOUND_TIME = 7;
    const float MAX_SHOT_FLY_COUNT = 3;     // the shells and the banana, after a wall
    const float GSHELL_DMG_EFFECT_FRAME = 30;
    const float RSHELL_DMG_EFFECT_FRAME = 30;
    const float RSHELL_HOMING_FRAME = 20;   // the homing turn is narrowest after this many frames
    const float BANANA_DMG_EFFECT_FRAME = 12;
    const float BANANA_HOMING_FRAME = 20;
    const float SMALL_BOUND_FRAME = 120;    // the time of one bounce of the mushroom
    const float SMALL_DMG_EFFECT_FRAME = 0;
    const float BOMB_BOUND_FRAME = 90;
    const float BOMB_DMG_EFFECT_FRAME = 80; // also the down time of the bomb's blast
    const float STAR_DMG_EFFECT_FRAME = 30;
    const float STAR_WALL_BOUND_TIME = 0;
    const float BASE_DMG_EFFECT_FRAME = 20;

    // Model scale of each kind once constructed.
    const float SHELL_SCALE = 1.35f;
    const float BOMB_SCALE = 1.7f;
    const float SMALL_SCALE = 0.5f;

    const long EFFECT_BOMB_FUSE = 5;
    const long SE_BOMB_FUSE = 0xC4F;
    const long SE_SHELL_SHOOT = 0xC3F;
    const long SE_ITEM_CHANGE = 0xC41;

    inline void SetMdlScale(bcs::CMdlObj *pMdl, float scale)
    {
        if (pMdl != 0) {
            pMdl->m_vScale.x = scale;
            pMdl->m_vScale.y = scale;
            pMdl->m_vScale.z = scale;
        }
    }

    // The team of character nChr.
    inline long GetTeam(long nChr)
    {
        return msw::CChrMan::GetChrCore(nChr)->GetTeamNo();
    }
}

// The blast of the dodgeball bomb. It does not hit the team of the
// character that threw the bomb.
class CBombExplosionDodge : public msw::CBombExplosion {
    public:
        CBombExplosionDodge(msw::CBallDodge *pBall, const bcs::FVEC3 &rPos, unsigned long hMdl, float fSize,
                            unsigned long nNoHitFlag)
            : msw::CBombExplosion(pBall, rPos, hMdl, 0.0f, fSize, 0.0f, 0, 0, 0), m_nNoHitFlag(nNoHitFlag),
              m_nShotChr(-1), m_nShotTeam(-1)
        {
            if (pBall != 0) {
                m_nShotChr = pBall->GetShotChrIdx();
                if (m_nShotChr >= 0) {
                    m_nShotTeam = m_nShotChr & 1;
                }
            }
        }

        virtual ~CBombExplosionDodge();

        virtual unsigned long GetAtkType() const;
        virtual unsigned long GetNoHitFlag() const;
        virtual void Create_(const bcs::FVEC3 &rPos, unsigned long hMdl);

    private:
        unsigned long m_nNoHitFlag; // 0x7C, one bit per character
        long m_nShotChr;            // 0x80
        long m_nShotTeam;           // 0x84
};

namespace msw {

    // Interface --------------------------------------------------------------

    void CItemBallBaseDodge::OnCtorNormalShoot()
    {
    }

    void CItemBallBaseDodge::OnPreNormalShoot()
    {
    }

    void CItemBallBaseDodge::DoNormalShootPhysMove(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fAccel, float fFrame)
    {
        float fAdd = fAccel * fFrame;
        bcs::FVEC3 move;

        bcs::FVEC3Scale(&move, &rVel, fFrame);
        rVel.y += fAdd;
        move.y += 0.5f * (fAdd * fFrame);
        bcs::FVEC3Add(&rPos, &rPos, &move);
    }

    void CItemBallBaseDodge::OnPostNormalShoot()
    {
    }

    void CItemBallBaseDodge::OnDtorNormalShoot()
    {
    }

    void CItemBallBaseDodge::OnCtorFreeMove()
    {
    }

    void CItemBallBaseDodge::OnDtorFreeMove()
    {
    }

    void CItemBallBaseDodge::OnReflectFloor()
    {
    }

    void CItemBallBaseDodge::OnTrgTouch(long)
    {
    }

    void CItemBallBaseDodge::OnCatch(long)
    {
    }

    long CItemBallBaseDodge::GetFriendHitResult()
    {
        return 1;
    }

    float CItemBallBaseDodge::GetFloorBoundDecRateXZ()
    {
        return CBallMan::GetFloorBoundDecRateXZ();
    }

    float CItemBallBaseDodge::GetFloorBoundDecRateY()
    {
        return CBallMan::GetFloorBoundDecRateY();
    }

    float CItemBallBaseDodge::GetFloorRoundDecRateXZ()
    {
        return CBallMan::GetFloorRoundDecRateXZ();
    }

    float CItemBallBaseDodge::GetWallBoundDecRateXZ()
    {
        float fRate = 0.85f;
        if (CGame::g_GameWork.m_nUnk40 & 0x20) {
            fRate = 0.0f;
        }
        return fRate;
    }

    float CItemBallBaseDodge::GetWallBoundTime()
    {
        return WALL_BOUND_TIME;
    }

    float CItemBallBaseDodge::GetGravityDefault() const
    {
        return -0.00625f;
    }

    long CItemBallBaseDodge::GetShootSe(long nSe) const
    {
        return nSe;
    }

    void CItemBallBaseDodge::SetOutBallMode(const bcs::FVEC3 &, long nChr, long nArg)
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = CChrCoreDodge::GetCcParamByChrIdx(nChr);
            pBall->SetFlickBallMode(pParam->m_Unk550, pParam->m_Unk554, pParam->m_Unk558, pParam->m_Unk55C, nChr,
                                    nArg);
        }
    }

    long CItemBallBaseDodge::GetHitType() const
    {
        return 0;
    }

    long CItemBallBaseDodge::GetFriendHitType() const
    {
        return 1;
    }

    float CItemBallBaseDodge::GetDmgEffectFrame() const
    {
        return BASE_DMG_EFFECT_FRAME;
    }

    const bcs::FVEC3 &CItemBallBaseDodge::GetVel() const
    {
        return GetBallDodge()->GetVelocity();
    }

    void CItemBallBaseDodge::SetVel(const bcs::FVEC3 &rVel)
    {
        GetBallDodge()->SetVelocity(rVel);
    }

    void CItemBallBaseDodge::SetTargetPos(const bcs::FVEC3 &rPos)
    {
        GetBallDodge()->SetTargetPos(rPos);
    }

    void CItemBallBaseDodge::CancelOffensive_()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            pBall->m_nOffensive = 0;
        }
    }

    // Plain ball -------------------------------------------------------------

    // Zero for the shot types that pass through the thrower's team.
    long CItemBallNormalDodge::GetFriendHitResult()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            switch (pBall->GetShootType()) {
                case 2:
                case 6:
                case 7:
                case 8:
                case 11:
                    return 0;
            }
        }
        return 1;
    }

    CBallDodge *CItemBallNormalDodge::GetBallDodge() const
    {
        return m_pBall;
    }

    void CItemBallNormalDodge::OnReflectWall()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0 && pBall->IsArtsShootMode() == 0) {
            CancelOffensive_();
        }
    }

    // Item base --------------------------------------------------------------

    CBallDodge *CItemBallDodge::GetBallDodge() const
    {
        return static_cast<CBallDodge *>(GetBall());
    }

    float CItemBallDodge::GetWallBoundDecRateXZ()
    {
        return 0.0f;
    }

    // Bomb -------------------------------------------------------------------

    inline void CItemBallBombDodge::StartFuse_()
    {
        if (m_pMdl == 0) {
            return;
        }
        if (m_hFuseEffect == 0) {
            m_hFuseEffect = CEffectCtrl::CreateAttachEffect(EFFECT_BOMB_FUSE, m_pMdl, 0, 0, 1, 1, 0, 3, 0, 0, 0);
        }
        if (m_hFuseSe == 0) {
            m_hFuseSe = CGameSnd::Play(SE_BOMB_FUSE, &m_vPos, 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    inline void CItemBallBombDodge::StopFuse_()
    {
        if (m_hFuseEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hFuseEffect);
            m_hFuseEffect = 0;
        }
        if (m_hFuseSe != 0) {
            bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hFuseSe));
            if (pSe != 0) {
                pSe->Stop(0.0f);
            }
            m_hFuseSe = 0;
        }
    }

    CItemBallBombDodge::CItemBallBombDodge(CBall *pBall)
        : CItemBallDodge(pBall, 3), m_bExploded(0), m_fFuseTime(60.0f), m_fUnk74(0.0f), m_hFuseEffect(0),
          m_hFuseSe(0), m_fUnk80(0.0f), m_hTailEffect(0), m_nShotTeam(-1)
    {
    }

    CItemBallBombDodge::~CItemBallBombDodge()
    {
        StopFuse_();
        DetachTailEffect_();
    }

    void CItemBallBombDodge::MainFunc_()
    {
    }

    long CItemBallBombDodge::TrgConstruct_()
    {
        // A random fuse: 60 to 540 frames, then 60 to 240 more.
        float fDelayMin = 60.0f;
        m_fFuseDelay = bcs::BCS_RandRange(fDelayMin, 480 + fDelayMin);
        float fTimeMin = 60.0f;
        m_fFuseTime = m_fFuseDelay + bcs::BCS_RandRange(fTimeMin, 180 + fTimeMin);
        SetMdlScale(m_pMdl, BOMB_SCALE);
        StartFuse_();
        return 0;
    }

    long CItemBallBombDodge::TrgShootStart_()
    {
        return CItemBall::TrgShootStart_();
    }

    long CItemBallBombDodge::TrgRebound_()
    {
        return 0;
    }

    long CItemBallBombDodge::TrgReboundToFree_()
    {
        return 0;
    }

    long CItemBallBombDodge::TrgGoalIn_()
    {
        return 0;
    }

    long CItemBallBombDodge::TrgFreeMode_()
    {
        if (IsShot()) {
            m_nTrgFlag |= 1 << (TRG_REBOUND_TO_FREE + 1);
        }
        return 0;
    }

    long CItemBallBombDodge::TrgChrHit_()
    {
        return 0;
    }

    long CItemBallBombDodge::TrgAttach_()
    {
        return 0;
    }

    // Returns 4 when the ball has not been shot.
    long CItemBallBombDodge::TrgDamage_()
    {
        if (IsShot()) {
            Explode_(0);
            return 1;
        }
        return 4;
    }

    void CItemBallBombDodge::Explode_(int bAttach)
    {
        if (m_bExploded != 0) {
            return;
        }
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            unsigned long hMdl = bAttach ? pBall->GetMdlObj()->GetHandle() : 0;
            bcs::FVEC3 pos = pBall->GetPos();
            float fSize = GetExplosionCollSize();
            // The thrower's team is the characters 0, 2, 4 and 6 or 1, 3, 5
            // and 7.
            unsigned long nNoHit = m_nShotTeam >= 0 ? 0x55 << m_nShotTeam : 0;
            new (0) CBombExplosionDodge(pBall, pos, hMdl, fSize, nNoHit);
        }
        m_bExploded = 1;
        if (m_hFuseEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hFuseEffect);
            m_hFuseEffect = 0;
        }
        if (m_hFuseSe != 0) {
            bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hFuseSe));
            if (pSe != 0) {
                pSe->Stop(0.0f);
            }
            m_hFuseSe = 0;
        }
    }

    void CItemBallBombDodge::AttachTailEffect_()
    {
        if (m_hTailEffect == 0) {
            m_hTailEffect = CEffectCtrl::CreateAttachEffect(0x4EC, m_pMdl, 0, 0, 1, 1, 0, 3, 0, 0, 0);
        }
    }

    void CItemBallBombDodge::DetachTailEffect_()
    {
        if (m_hTailEffect != 0) {
            CEffectCtrl::StopAttachEffect(m_hTailEffect);
            m_hTailEffect = 0;
        }
    }

    void CItemBallBombDodge::OnCtorNormalShoot()
    {
        // The charge power is read and not used.
        GetBall()->GetChargePower();
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            m_nShotTeam = pBall->GetShotTeamIdx();
        }
        SetVelocity();
        pBall->m_nUnk360 = 0;
    }

    void CItemBallBombDodge::OnCtorFreeMove()
    {
        AttachTailEffect_();
    }

    void CItemBallBombDodge::OnDtorFreeMove()
    {
        DetachTailEffect_();
    }

    void CItemBallBombDodge::OnReflectWall()
    {
        Explode_(0);
        TrgDestroy_();
    }

    void CItemBallBombDodge::OnReflectFloor()
    {
        Explode_(0);
        TrgDestroy_();
    }

    long CItemBallBombDodge::GetHitType() const
    {
        return 1;
    }

    void CItemBallBombDodge::OnTrgTouch(long)
    {
        Explode_(0);
        TrgDestroy_();
    }

    void CItemBallBombDodge::OnCatch(long)
    {
        Explode_(0);
        TrgDestroy_();
    }

    void CItemBallBombDodge::SetVelocity()
    {
        CBallDodge *pBall = GetBallDodge();
        float fSpeed = 0.1f;
        if (0.001f > fSpeed) {
            fSpeed = 0.001f;
        }
        float fGroundY = 0.0f;
        float fTopY = 3.0f;
        float fVelY;
        float fGravity;
        bcs::BCS_Phys_CalcYVG(6.5f / fSpeed, pBall->GetPos().y, fGroundY, fTopY, fVelY, fGravity);

        bcs::FVEC3 vel(GetVel());
        bcs::FVEC3 dir;
        bcs::BCS_Vec_Normalize(dir, vel);
        bcs::FVEC3Scale(&vel, &dir, fSpeed);
        vel.y = fVelY;
        SetVel(vel);

        bcs::FVEC3 target;
        bcs::FVEC3Scale(&target, &dir, 6.5f);
        bcs::FVEC3Add(&target, &target, &pBall->GetPos());
        target.y = fGroundY;
        SetTargetPos(target);
        pBall->SetGravity(fGravity);
    }

    void CItemBallBombDodge::SetFreeMove()
    {
        CBallDodge *pBall = GetBallDodge();
        pBall->SetGravity(GetGravityDefault());
        pBall->TrgScore(-1);
        pBall->ChangeState(1);
    }

    float CItemBallBombDodge::GetFloorBoundDecRateXZ()
    {
        bcs::FVEC3 vel(GetBallDodge()->GetVelocity());
        vel.y = 0.0f;
        float fSpeed = bcs::FVEC3Len(&vel);
        float fRate = 1.0f;
        float fWant = GetMyVelXZ();
        if (fSpeed > 0.0f) {
            fRate = fWant / fSpeed;
        }
        return fRate;
    }

    float CItemBallBombDodge::GetFloorRoundDecRateXZ()
    {
        bcs::FVEC3 vel(GetBallDodge()->GetVelocity());
        vel.y = 0.0f;
        float fSpeed = bcs::FVEC3Len(&vel);
        float fRate = 1.0f;
        float fWant = GetMyVelXZ();
        if (fSpeed > 0.0f) {
            fRate = fWant / fSpeed;
        }
        return fRate;
    }

    float CItemBallBombDodge::GetFloorBoundDecRateY()
    {
        float fSpeed = fabsf(GetBallDodge()->GetVelocity().y);
        float fRate = 1.0f;
        float fWant = GetMyVelY();
        if (fSpeed > 0.0f) {
            fRate = fWant / fSpeed;
        }
        SetBoundVel();
        return fRate;
    }

    // The gravity that makes a bounce rise 2 units and come down again in
    // the bomb's bound time.
    float CItemBallBombDodge::GetGravityDefault() const
    {
        float fHeight = 2.0f;
        float fFrame = 0.5f * BOMB_BOUND_FRAME;
        float fUp = nw4r::math::FSqrt(fHeight);
        float fDown = nw4r::math::FSqrt(fHeight);
        float fSum = fUp + fDown;
        return -2.0f * (fSum * fSum / (fFrame * fFrame));
    }

    float CItemBallBombDodge::GetMyVelXZ() const
    {
        return 0.1f;
    }

    float CItemBallBombDodge::GetMyVelY() const
    {
        float fHeight = 2.0f;
        float fFrame = 0.5f * BOMB_BOUND_FRAME;
        float fUp = nw4r::math::FSqrt(fHeight);
        float fDown = nw4r::math::FSqrt(fHeight);
        return 2.0f * fUp * (fUp + fDown) / fFrame;
    }

    void CItemBallBombDodge::SetBoundVel()
    {
        CBall *pBall = GetBall();
        bcs::FVEC3 pos(pBall->GetPos());
        CChrCore *pChr = CChrMan::Util_SearchNearPosChar(pos, -1);
        if (pChr != 0 && bcs::BCS_RandRange(0.0f, 1.0f) < 0.4f) {
            // Turn towards the nearest character, keeping the speed along
            // the floor.
            bcs::FVEC3 vel(GetVel());
            bcs::FVEC3 dir(vel);
            dir.y = 0.0f;
            float fSpeed = bcs::FVEC3Len(&dir);
            bcs::FVEC3Sub(&dir, &pChr->GetPos(), &pos);
            dir.y = 0.0f;
            bcs::BCS_Vec_Normalize(dir, dir);
            bcs::FVEC3Scale(&dir, &dir, fSpeed);
            vel.x = dir.x;
            vel.z = dir.z;
            SetVel(vel);
        } else {
            bcs::FVEC3 vel(GetVel());
            float fRange = 2.7925267f;
            bcs::BCS_Vec_RotY(vel, vel, bcs::BCS_RandRange(-fRange, fRange));
            SetVel(vel);
        }
    }

    float CItemBallBombDodge::GetExplosionCollSize()
    {
        return 3.3f;
    }

    float CItemBallBombDodge::GetDmgEffectFrame() const
    {
        return BOMB_DMG_EFFECT_FRAME;
    }

    // Green shell ------------------------------------------------------------

    CItemBallGShellDodge::CItemBallGShellDodge(CBall *pBall)
        : CItemBallDodge(pBall, 0), m_nReflectNum(1), m_fTimer(0.0f), m_fCatchTime(300.0f)
    {
    }

    CItemBallGShellDodge::~CItemBallGShellDodge()
    {
    }

    long CItemBallGShellDodge::TrgConstruct_()
    {
        SetMdlScale(m_pMdl, SHELL_SCALE);
        return 0;
    }

    void CItemBallGShellDodge::MainFunc_()
    {
        if (m_fCatchTime > 0.0f && m_fTimer < m_fCatchTime) {
            m_fTimer += GetFrameRateF();
        }
    }

    long CItemBallGShellDodge::TrgGoalIn_()
    {
        return 1;
    }

    long CItemBallGShellDodge::TrgChrHit_()
    {
        return 1;
    }

    long CItemBallGShellDodge::TrgAttach_()
    {
        return 0;
    }

    // Caught by the other team, or by the thrower's team once the catch
    // time is over, the shell is gone.
    void CItemBallGShellDodge::OnCatch(long nChr)
    {
        long nTeam = GetTeam(GetBallDodge()->GetShotChrIdx());
        if (nTeam != GetTeam(nChr)) {
            TrgDestroy_();
        } else if (m_fCatchTime > 0.0f && m_fTimer >= m_fCatchTime) {
            TrgDestroy_();
        }
    }

    long CItemBallGShellDodge::TrgDamage_()
    {
        return 0;
    }

    long CItemBallGShellDodge::GetTailEffect(long) const
    {
        return 0x4E9;
    }

    long CItemBallGShellDodge::GetShootSe(long) const
    {
        return SE_SHELL_SHOOT;
    }

    void CItemBallGShellDodge::OnCtorNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall->IsNowRebound()) {
            return;
        }
        if (pBall->GetShotFlyCount() <= 0.0f) {
            return;
        }
        pBall->GetChargePower();
        m_nReflectNum = 1;
        bcs::FVEC3 vel(GetVel());
        bcs::BCS_Vec_Normalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, 0.5f);
        SetVel(vel);
        pBall->SetCollCatchRadius(0.5f);
    }

    // The first bounces off a wall turn the shell towards the nearest
    // character of the other team.
    void CItemBallGShellDodge::OnReflectWall()
    {
        if (m_nReflectNum-- != 0) {
            CBallDodge *pBall = GetBallDodge();
            pBall->HomingUpdate(CChrMan::Util_SearchNearPosChar(pBall->GetPos(), pBall->GetShotTeamIdx()),
                                31.415928f, -1.0f, -1.0f);
        } else {
            CBallDodge *pBall = GetBallDodge();
            float fCount = pBall->GetShotFlyCount();
            if (MAX_SHOT_FLY_COUNT < fCount) {
                fCount = MAX_SHOT_FLY_COUNT;
            }
            pBall->SetShotFlyCount(fCount);
        }
    }

    float CItemBallGShellDodge::GetWallBoundDecRateXZ()
    {
        if (m_nReflectNum > 0) {
            return 0.0f;
        }
        return 0.85f;
    }

    long CItemBallGShellDodge::TranslateCatchMethod(long nMethod, long nKind) const
    {
        switch (nKind) {
            case 0:
                nMethod = 0;
                break;
            case 1:
                nMethod = 0;
                break;
            case 2:
                nMethod = 0;
                break;
            case 3:
                nMethod = 0;
                break;
            case 4:
                nMethod = 0;
                break;
            case 5:
                nMethod = 0;
                break;
        }
        return nMethod;
    }

    long CItemBallGShellDodge::TranslateCatchParamType(long, int nArg) const
    {
        return (nArg != 0) + 1;
    }

    float CItemBallGShellDodge::GetDmgEffectFrame() const
    {
        return GSHELL_DMG_EFFECT_FRAME;
    }

    // Red shell --------------------------------------------------------------

    CItemBallRShellDodge::CItemBallRShellDodge(CBall *pBall)
        : CItemBallDodge(pBall, 1), m_bHoming(1), m_fTimer(0.0f), m_fCatchTime(300.0f)
    {
    }

    CItemBallRShellDodge::~CItemBallRShellDodge()
    {
    }

    long CItemBallRShellDodge::TrgConstruct_()
    {
        SetMdlScale(m_pMdl, SHELL_SCALE);
        return 0;
    }

    void CItemBallRShellDodge::MainFunc_()
    {
        if (m_fCatchTime > 0.0f && m_fTimer < m_fCatchTime) {
            m_fTimer += GetFrameRateF();
        }
    }

    long CItemBallRShellDodge::TrgGoalIn_()
    {
        return 1;
    }

    long CItemBallRShellDodge::TrgChrHit_()
    {
        return 1;
    }

    long CItemBallRShellDodge::TrgAttach_()
    {
        return 0;
    }

    void CItemBallRShellDodge::OnCatch(long nChr)
    {
        long nTeam = GetTeam(GetBallDodge()->GetShotChrIdx());
        if (nTeam != GetTeam(nChr)) {
            TrgDestroy_();
        } else if (m_fCatchTime > 0.0f && m_fTimer >= m_fCatchTime) {
            TrgDestroy_();
        }
    }

    long CItemBallRShellDodge::TrgDamage_()
    {
        return 0;
    }

    long CItemBallRShellDodge::GetTailEffect(long) const
    {
        return 0x4EA;
    }

    long CItemBallRShellDodge::GetShootSe(long) const
    {
        return SE_SHELL_SHOOT;
    }

    // The shot leaves at an angle (32 of 256 steps, 45 degrees) to the
    // throw and homes in from there.
    void CItemBallRShellDodge::OnCtorNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall->IsNowRebound()) {
            return;
        }
        if (pBall->GetShotFlyCount() <= 0.0f) {
            return;
        }
        m_nUnk68 = 0;
        m_bHoming = 1;
        bcs::FVEC3 vel(GetVel());
        bcs::BCS_Vec_Normalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, 0.25f);
        nw4r::math::MTX34 mtx;
        nw4r::math::MTX34RotXYZFIdx(&mtx, 0.0f, 32.0f, 0.0f);
        PSMTXMultVec(reinterpret_cast<const float(*)[4]>(&mtx), reinterpret_cast<Vec *>(&vel), reinterpret_cast<Vec *>(&vel));
        SetVel(vel);
        pBall->SetCollCatchRadius(0.5f);
    }

    // While the shot flies, turn it towards its target; the turn allowed
    // per step narrows from 360 to 60 degrees over the homing frames. Stops
    // homing for good when it cannot turn or has flown far.
    void CItemBallRShellDodge::OnPreNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        long nTarget = pBall->GetTargetChrIdx();
        if (pBall->IsOffensive() && m_bHoming) {
            // The thrower's team is read and not used.
            pBall->GetShotTeamIdx();
            CChrCore *pChr = nTarget >= 0 ? CChrMan::GetChrCore(nTarget) : 0;
            int bTurned;
            if (pChr != 0) {
                float fRot = 360.0f;
                float fMinRot = 60.0f;
                float fFrame = RSHELL_HOMING_FRAME;
                float fFly = pBall->GetFlyCount();
                float fTime;
                if (fFly > fFrame) {
                    fTime = fFrame;
                } else if (fFly < 0.0f) {
                    fTime = 0.0f;
                } else {
                    fTime = fFly;
                }
                if (fFrame > 0.0f) {
                    fRot = fTime / fFrame * (fRot - fMinRot) + fMinRot;
                }
                bTurned = pBall->HomingUpdate(pChr, bcs::DEG_TO_RAD(fRot), 0.0f, 3.1415927f);
            } else {
                bTurned = 0;
            }
            if (pBall->GetShootFlyDist() > 12.0f) {
                bTurned = 0;
            }
            if (!bTurned) {
                m_bHoming = 0;
            }
        }
    }

    int CItemBallRShellDodge::CanbeWallReflect() const
    {
        return m_bHoming == 0;
    }

    void CItemBallRShellDodge::OnReflectWall()
    {
        if (m_bHoming == 0) {
            CBallDodge *pBall = GetBallDodge();
            float fCount = pBall->GetShotFlyCount();
            if (MAX_SHOT_FLY_COUNT < fCount) {
                fCount = MAX_SHOT_FLY_COUNT;
            }
            pBall->SetShotFlyCount(fCount);
            TrgDestroy_();
        }
    }

    float CItemBallRShellDodge::GetWallBoundDecRateXZ()
    {
        return 0.85f;
    }

    long CItemBallRShellDodge::TranslateCatchMethod(long nMethod, long nKind) const
    {
        switch (nKind) {
            case 0:
                nMethod = 0;
                break;
            case 1:
                nMethod = 0;
                break;
            case 2:
                nMethod = 0;
                break;
            case 3:
                nMethod = 0;
                break;
            case 4:
                nMethod = 0;
                break;
            case 5:
                nMethod = 0;
                break;
        }
        return nMethod;
    }

    long CItemBallRShellDodge::TranslateCatchParamType(long, int nArg) const
    {
        return (nArg != 0) + 1;
    }

    float CItemBallRShellDodge::GetDmgEffectFrame() const
    {
        return RSHELL_DMG_EFFECT_FRAME;
    }

    // Banana -----------------------------------------------------------------

    CItemBallBananaDodge::CItemBallBananaDodge(CBall *pBall)
        : CItemBallDodge(pBall, 5), m_bHoming(1), m_fTimer(0.0f), m_fCatchTime(300.0f)
    {
    }

    CItemBallBananaDodge::~CItemBallBananaDodge()
    {
    }

    long CItemBallBananaDodge::TrgConstruct_()
    {
        SetMdlScale(m_pMdl, SHELL_SCALE);
        return 0;
    }

    void CItemBallBananaDodge::MainFunc_()
    {
        if (m_fCatchTime > 0.0f && m_fTimer < m_fCatchTime) {
            m_fTimer += GetFrameRateF();
        }
    }

    long CItemBallBananaDodge::TrgGoalIn_()
    {
        return 1;
    }

    long CItemBallBananaDodge::TrgChrHit_()
    {
        return 1;
    }

    long CItemBallBananaDodge::TrgAttach_()
    {
        return 0;
    }

    void CItemBallBananaDodge::OnCatch(long nChr)
    {
        long nTeam = GetTeam(GetBallDodge()->GetShotChrIdx());
        if (nTeam != GetTeam(nChr)) {
            TrgDestroy_();
        } else if (m_fCatchTime > 0.0f && m_fTimer >= m_fCatchTime) {
            TrgDestroy_();
        }
    }

    long CItemBallBananaDodge::TrgDamage_()
    {
        return 0;
    }

    long CItemBallBananaDodge::GetTailEffect(long) const
    {
        return 0x4EB;
    }

    long CItemBallBananaDodge::GetShootSe(long) const
    {
        return SE_SHELL_SHOOT;
    }

    void CItemBallBananaDodge::OnCtorNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall->IsNowRebound()) {
            return;
        }
        if (pBall->GetShotFlyCount() <= 0.0f) {
            return;
        }
        m_nUnk68 = 0;
        m_bHoming = 1;
        bcs::FVEC3 vel(GetVel());
        bcs::BCS_Vec_Normalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, 0.25f);
        nw4r::math::MTX34 mtx;
        nw4r::math::MTX34RotXYZFIdx(&mtx, 0.0f, 32.0f, 0.0f);
        PSMTXMultVec(reinterpret_cast<const float(*)[4]>(&mtx), reinterpret_cast<Vec *>(&vel), reinterpret_cast<Vec *>(&vel));
        SetVel(vel);
        pBall->SetCollCatchRadius(0.5f);
    }

    void CItemBallBananaDodge::OnPreNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        long nTarget = pBall->GetTargetChrIdx();
        if (pBall->IsOffensive() && m_bHoming) {
            // The thrower's team is read and not used.
            pBall->GetShotTeamIdx();
            CChrCore *pChr = nTarget >= 0 ? CChrMan::GetChrCore(nTarget) : 0;
            int bTurned;
            if (pChr != 0) {
                float fRot = 360.0f;
                float fMinRot = 60.0f;
                float fFrame = BANANA_HOMING_FRAME;
                float fFly = pBall->GetFlyCount();
                float fTime;
                if (fFly > fFrame) {
                    fTime = fFrame;
                } else if (fFly < 0.0f) {
                    fTime = 0.0f;
                } else {
                    fTime = fFly;
                }
                if (fFrame > 0.0f) {
                    fRot = fTime / fFrame * (fRot - fMinRot) + fMinRot;
                }
                bTurned = pBall->HomingUpdate(pChr, bcs::DEG_TO_RAD(fRot), 0.0f, 3.1415927f);
            } else {
                bTurned = 0;
            }
            if (pBall->GetShootFlyDist() > 12.0f) {
                bTurned = 0;
            }
            if (!bTurned) {
                m_bHoming = 0;
            }
        }
    }

    int CItemBallBananaDodge::CanbeWallReflect() const
    {
        return m_bHoming == 0;
    }

    void CItemBallBananaDodge::OnReflectWall()
    {
        if (m_bHoming == 0) {
            CBallDodge *pBall = GetBallDodge();
            float fCount = pBall->GetShotFlyCount();
            if (MAX_SHOT_FLY_COUNT < fCount) {
                fCount = MAX_SHOT_FLY_COUNT;
            }
            pBall->SetShotFlyCount(fCount);
            TrgDestroy_();
        }
    }

    float CItemBallBananaDodge::GetWallBoundDecRateXZ()
    {
        return 0.85f;
    }

    long CItemBallBananaDodge::TranslateCatchMethod(long nMethod, long nKind) const
    {
        switch (nKind) {
            case 0:
                nMethod = 2;
                break;
            case 1:
                nMethod = 2;
                break;
            case 2:
                nMethod = 2;
                break;
            case 3:
                nMethod = 2;
                break;
            case 4:
                nMethod = 2;
                break;
            case 5:
                nMethod = 2;
                break;
        }
        return nMethod;
    }

    long CItemBallBananaDodge::TranslateCatchParamType(long, int nArg) const
    {
        return nArg != 0;
    }

    // Out of the court the banana flies off like a flicked ball and is gone.
    void CItemBallBananaDodge::SetOutBallMode(const bcs::FVEC3 &, long nChr, long nArg)
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = CChrCoreDodge::GetCcParamByChrIdx(nChr);
            pBall->SetFlickBallMode(pParam->m_Unk528, pParam->m_Unk52C, pParam->m_Unk530, pParam->m_Unk534, nChr,
                                    nArg);
        }
        TrgDestroy_();
    }

    float CItemBallBananaDodge::GetDmgEffectFrame() const
    {
        return BANANA_DMG_EFFECT_FRAME;
    }

    // Mushroom ---------------------------------------------------------------

    CItemBallSmallDodge::CItemBallSmallDodge(CBall *pBall)
        : CItemBallDodge(pBall, 2), m_fTimer(0.0f), m_fCatchTime(300.0f)
    {
    }

    CItemBallSmallDodge::~CItemBallSmallDodge()
    {
    }

    void CItemBallSmallDodge::MainFunc_()
    {
        if (m_fCatchTime > 0.0f && m_fTimer < m_fCatchTime) {
            m_fTimer += GetFrameRateF();
        }
    }

    long CItemBallSmallDodge::TrgConstruct_()
    {
        SetMdlScale(m_pMdl, SMALL_SCALE);
        CBallDodge *pBall = GetBallDodge();
        pBall->SetGravity(GetGravityDefault());
        return 0;
    }

    long CItemBallSmallDodge::TrgDestroy_()
    {
        return CItemBall::TrgDestroy_();
    }

    long CItemBallSmallDodge::TrgGoalIn_()
    {
        return 1;
    }

    long CItemBallSmallDodge::TrgFreeMode_()
    {
        return 1;
    }

    long CItemBallSmallDodge::TrgChrHit_()
    {
        return 0;
    }

    long CItemBallSmallDodge::TrgRebound_()
    {
        return 1;
    }

    long CItemBallSmallDodge::TrgAttach_()
    {
        return 0;
    }

    void CItemBallSmallDodge::OnCatch(long nChr)
    {
        CBallDodge *pBall = GetBallDodge();
        long nTeam = GetTeam(pBall->GetShotChrIdx());
        CChrCore *pChr = CChrMan::GetChrCore(nChr);
        if (pBall->GetTouchNum() > 0) {
            TrgDestroy_();
        } else if (nTeam != pChr->GetTeamNo()) {
            TrgDestroy_();
        } else if (m_fCatchTime > 0.0f && m_fTimer >= m_fCatchTime) {
            TrgDestroy_();
        }
    }

    long CItemBallSmallDodge::TrgDamage_()
    {
        return 0;
    }

    long CItemBallSmallDodge::GetTailEffect(long) const
    {
        return 0x4ED;
    }

    void CItemBallSmallDodge::OnCtorNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall->IsNowRebound()) {
            return;
        }
        if (pBall->GetShotFlyCount() <= 0.0f) {
            return;
        }
        bcs::FVEC3 vel(GetVel());
        bcs::BCS_Vec_Normalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, 0.3f);
        SetVel(vel);
        pBall->SetCollCatchRadius(0.5f);
    }

    void CItemBallSmallDodge::OnPreNormalShoot()
    {
        GetBallDodge();
    }

    void CItemBallSmallDodge::OnCtorFreeMove()
    {
        CBallDodge *pBall = GetBallDodge();
        pBall->SetGravity(GetGravityDefault());
    }

    void CItemBallSmallDodge::OnReflectWall()
    {
    }

    void CItemBallSmallDodge::OnReflectFloor()
    {
        CBallDodge *pBall = GetBallDodge();
        pBall->SetGravity(GetGravityDefault());
    }

    float CItemBallSmallDodge::GetFloorBoundDecRateY()
    {
        float fSpeed = fabsf(GetBallDodge()->GetVelocity().y);
        float fRate = 1.0f;
        float fWant = GetMyVelY();
        if (fSpeed > 0.0f) {
            fRate = fWant / fSpeed;
        }
        return fRate;
    }

    // The gravity that makes a bounce rise 3 units and come down again in
    // the mushroom's bound time.
    float CItemBallSmallDodge::GetGravityDefault() const
    {
        float fHeight = 3.0f;
        float fFrame = 0.5f * SMALL_BOUND_FRAME;
        float fUp = nw4r::math::FSqrt(fHeight);
        float fDown = nw4r::math::FSqrt(fHeight);
        float fSum = fUp + fDown;
        return -2.0f * (fSum * fSum / (fFrame * fFrame));
    }

    float CItemBallSmallDodge::GetMyVelY() const
    {
        float fHeight = 3.0f;
        float fFrame = 0.5f * SMALL_BOUND_FRAME;
        float fUp = nw4r::math::FSqrt(fHeight);
        float fDown = nw4r::math::FSqrt(fHeight);
        return 2.0f * fUp * (fUp + fDown) / fFrame;
    }

    long CItemBallSmallDodge::TranslateCatchMethod(long nMethod, long nKind) const
    {
        switch (nKind) {
            case 0:
                nMethod = 0;
                break;
            case 1:
                nMethod = 0;
                break;
            case 2:
                nMethod = 0;
                break;
            case 3:
                nMethod = 0;
                break;
            case 4:
                nMethod = 2;
                break;
            case 5:
                nMethod = 2;
                break;
        }
        return nMethod;
    }

    long CItemBallSmallDodge::TranslateCatchParamType(long, int nArg) const
    {
        return nArg != 0;
    }

    void CItemBallSmallDodge::SetOutBallMode(const bcs::FVEC3 &, long nChr, long nArg)
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            const CHR_CORE_DODGE::CC_PARAM_DODGE *pParam = CChrCoreDodge::GetCcParamByChrIdx(nChr);
            pBall->SetFlickBallMode(pParam->m_Unk528, pParam->m_Unk52C, pParam->m_Unk530, pParam->m_Unk534, nChr,
                                    nArg);
        }
        TrgDestroy_();
    }

    float CItemBallSmallDodge::GetDmgEffectFrame() const
    {
        return SMALL_DMG_EFFECT_FRAME;
    }

    // Star -------------------------------------------------------------------

    CItemBallStarDodge::CItemBallStarDodge(CBall *pBall)
        : CItemBallDodge(pBall, 4), m_nUnk68(0), m_fTimer(270.0f)
    {
    }

    CItemBallStarDodge::~CItemBallStarDodge()
    {
    }

    // When the time is up the star ends, unless the ball is still flying as
    // a shot: then it only falls normally again.
    void CItemBallStarDodge::MainFunc_()
    {
        if (m_nTrgFlag & 1) {
            return;
        }
        CBallDodge *pBall = GetBallDodge();
        m_fTimer -= GetFrameRateF();
        if (m_fTimer <= 0.0f) {
            if (pBall->IsShootMode() && pBall->IsOffensive()) {
                pBall->SetGravity(GetGravityDefault());
            } else {
                TrgDestroy_();
            }
        }
    }

    long CItemBallStarDodge::TrgConstruct_()
    {
        SetMdlScale(m_pMdl, SHELL_SCALE);
        CGameBGM::SetBgmState(4);
        return 0;
    }

    long CItemBallStarDodge::TrgGoalIn_()
    {
        return 0;
    }

    long CItemBallStarDodge::TrgChrHit_()
    {
        return 0;
    }

    long CItemBallStarDodge::TrgAttach_()
    {
        return 0;
    }

    long CItemBallStarDodge::TrgDamage_()
    {
        return 0;
    }

    long CItemBallStarDodge::GetTailEffect(long) const
    {
        return 0x4EE;
    }

    void CItemBallStarDodge::OnCtorNormalShoot()
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall->IsNowRebound()) {
            return;
        }
        if (pBall->GetShotFlyCount() <= 0.0f) {
            return;
        }
        pBall->SetShotFlyCount(m_fTimer);
        pBall->GetChargePower();
        m_nUnk68 = 0;
        bcs::FVEC3 vel(GetVel());
        bcs::BCS_Vec_Normalize(vel, vel);
        bcs::FVEC3Scale(&vel, &vel, 0.5f);
        SetVel(vel);
        pBall->SetCollCatchRadius(0.5f);
    }

    // The star shot does not come down: it is held at a height while it
    // flies as a shot.
    void CItemBallStarDodge::DoNormalShootPhysMove(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fAccel, float fFrame)
    {
        float fAdd = fAccel * fFrame;
        bcs::FVEC3 move;

        bcs::FVEC3Scale(&move, &rVel, fFrame);
        rVel.y += fAdd;
        move.y += 0.5f * (fAdd * fFrame);
        bcs::FVEC3Add(&rPos, &rPos, &move);

        CBallDodge *pBall = GetBallDodge();
        if (m_fTimer > 0.0f && pBall->IsShootMode() && pBall->IsOffensive()) {
            if (rPos.y < 0.6f) {
                rPos.y = 0.6f;
                if (rVel.y < 0.0f) {
                    float fSpeed = bcs::FVEC3Len(&rVel);
                    rVel.y = 0.0f;
                    bcs::BCS_Vec_Normalize(rVel, rVel);
                    bcs::FVEC3Scale(&rVel, &rVel, fSpeed);
                }
            }
        }
    }

    void CItemBallStarDodge::OnDtorNormalShoot()
    {
        TrgDestroy_();
    }

    // Off a wall the star turns by a random angle of 20 to 45 degrees
    // either way.
    void CItemBallStarDodge::OnReflectWall()
    {
        bcs::FVEC3 vel(GetBallDodge()->GetVelocity());
        // A random turn of 20 to 45 degrees either way.
        float fMax = 0.7853982f;
        float fMin = 0.34906584f;
        if (fMin > fMax) {
            fMin = 0.7853982f;
            fMax = 0.34906584f;
        }
        float fRot = bcs::BCS_RandRange(-fMax, fMax);
        if (fabsf(fRot) < fMin) {
            if (fRot < 0.0f) {
                fMin = -fMin;
            }
        } else {
            fMin = fRot;
        }
        bcs::BCS_Vec_RotY(vel, vel, fMin);
        SetVel(vel);
    }

    float CItemBallStarDodge::GetWallBoundDecRateXZ()
    {
        return 0.0f;
    }

    float CItemBallStarDodge::GetWallBoundTime()
    {
        return STAR_WALL_BOUND_TIME;
    }

    long CItemBallStarDodge::GetHitType() const
    {
        return 1;
    }

    long CItemBallStarDodge::TranslateCatchMethod(long nMethod, long nKind) const
    {
        switch (nKind) {
            case 0:
                nMethod = 0;
                break;
            case 1:
                nMethod = 0;
                break;
            case 2:
                nMethod = 0;
                break;
            case 3:
                nMethod = 0;
                break;
            case 4:
                nMethod = 0;
                break;
            case 5:
                nMethod = 0;
                break;
        }
        return nMethod;
    }

    long CItemBallStarDodge::TranslateCatchParamType(long, int nArg) const
    {
        return nArg != 0;
    }

    long CItemBallStarDodge::GetMapCollDetectParam() const
    {
        // The shot mode is asked and not used.
        GetBall()->IsShootMode();
        return 0;
    }

    void CItemBallStarDodge::OnTrgTouch(long)
    {
        CBallDodge *pBall = GetBallDodge();
        if (pBall != 0) {
            pBall->DryTrgScore(-1);
        }
    }

    float CItemBallStarDodge::GetDmgEffectFrame() const
    {
        return STAR_DMG_EFFECT_FRAME;
    }

    // Change effect ----------------------------------------------------------

    CItemBallChangeEffectDodge::CItemBallChangeEffectDodge(bcs::CTask *pParent, unsigned long nBallId)
        : CItemBallChangeEffect(pParent, nBallId)
    {
    }

    void CItemBallChangeEffectDodge::SetStartEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(0x4EF, pBall->GetPos(), 3, 0, 0);
            CGameSnd::Play(SE_ITEM_CHANGE, pBall->GetPos(), 1, -1, 0.0f, 1.0f, 0.0f);
        }
    }

    void CItemBallChangeEffectDodge::SetEndEffect()
    {
        CBall *pBall = static_cast<CBall *>(bcs::CTaskMan::GetTask(m_nBallId));
        if (pBall != 0) {
            CEffectCtrl::SimpleEffect(0x4F0, pBall->GetPos(), 3, 0, 0);
        }
    }
}

// Bomb blast ---------------------------------------------------------------

void CBombExplosionDodge::Create_(const bcs::FVEC3 &rPos, unsigned long hMdl)
{
    msw::CBombExplosion::Create_(rPos, hMdl);
    if (m_pAtkUnit != 0) {
        m_pAtkUnit->m_nTeamNo = m_nShotTeam;
        m_pAtkUnit->m_nChrNo = m_nShotChr;
        m_pAtkUnit->m_fDownFrame = BOMB_DMG_EFFECT_FRAME;
    }
}

unsigned long CBombExplosionDodge::GetNoHitFlag() const
{
    return m_nNoHitFlag;
}

unsigned long CBombExplosionDodge::GetAtkType() const
{
    return 0x202501;
}

CBombExplosionDodge::~CBombExplosionDodge()
{
}

msw::CItemBallChangeEffectDodge::~CItemBallChangeEffectDodge()
{
}
