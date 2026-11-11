// msw::CItemShellCmn, msw::CItemGShell, msw::CItemRShell: the green and the
// red shell and what they share. One file in the original: the three
// classes share one float pool, and the red shell's search ranges are the
// file's two statics.
#include "game/msw/CItemKinds.hpp"

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CCollBodyUnit.hpp"
#include "game/msw/CCollMan.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "nw4r/math/triangular.h"
#include "revolution/mtx.h"

#include <math.h>

namespace {

    // (names guessed) How far off the throw direction the red shell looks
    // for a target: across (yaw) and, for a target in the air, up (pitch).
    static const float s_fSearchRangeH = bcs::DEG_TO_RAD(45.0f);
    static const float s_fSearchRangeV = bcs::DEG_TO_RAD(45.0f);
}

namespace msw {

    CItemShellCmn::~CItemShellCmn()
    {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
    }

    // What both shells do in Create: model nMdl, one attack collision and
    // one body collision.
    void CItemShellCmn::Create_(long nMdl, const bcs::FVEC3 &pos)
    {
        CItem::Create(pos);
        CreateMdlObj(nMdl, 0, 1, 0x20);
        m_fUnk160 = -0.35f;

        CollAtkCreate(1);
        CollAtkSetRadius(0, 0.5f * GetParamScale());
        CollAtkSetHitEffectID(0, 0xC);
        CollAtkSetHitCamVibIDAll(1);
        CollAtkSetHitEffectFrameAll(m_fParamHitEffectFrame);
        CollAtkSetHitDropCoinRateAll(m_fParamHitDropCoinRate);
        CollAtkSetHitDropCoinNumAll(m_nParamHitDropCoinNumMin, m_nParamHitDropCoinNumMax);

        CollBodyCreate(1);
        CollBodySetPushEnableAll(1);
    }

    float CItemShellCmn::GetBodyCollRadius() const
    {
        return 0.2f * GetParamScale();
    }

    float CItemShellCmn::GetParamHeight() const
    {
        return 0.35f * GetParamScale();
    }

    float CItemShellCmn::GetParamAttachHeight() const
    {
        return GetParamHeight();
    }

    float CItemShellCmn::GetAtkCollRadius() const
    {
        return m_AtkColl.GetRadius(0);
    }

    // The shell breaks: collisions off, it turns round and bounces away.
    void CItemShellCmn::CtorStateBreak()
    {
        m_nFlag &= ~0x2000;
        CollBodySetEnableAll(0);
        CollAtkSetEnable(0, 0);
        SetRotYWrapped(3.1415927f);
        bcs::BCS_VecZ_RotY(m_vVelocity, -0.05f, CollAtkGetUnkA0(0));
        m_vVelocity.y = 0.125f;
        m_fGravity = -0.00625f;
        m_fUnk190 = 30.0f;
        PlaySePos(0xC4C);
    }

    // Flies off and fades out over the 30 frames set above, then goes.
    void CItemShellCmn::LoopStateBreak()
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

    // Pushed by a character: the shell hits it and will break away from
    // it. Pushed by anything else: it bounces off (its velocity is
    // reflected about the push direction in the XZ plane).
    void CItemShellCmn::CallbackBodyPush_(CCollBodyUnit *pMy, CCollBodyUnit *pOther, bcs::FVEC3 *pPush)
    {
        CItem::CallbackBodyPush_(pMy, pOther, pPush);
        if (pPush == 0) {
            return;
        }

        bcs::FVEC3 dir = *pPush;
        if (pOther != 0 && pOther->GetOwnerKind() == 1) {
            SendHitChrMessage(pOther->m_nUnk17);
            float fDir = 3.1415927f + nw4r::math::Atan2Rad(dir.x, dir.z);
            while (fDir > 6.2831855f) {
                fDir -= 6.2831855f;
            }
            while (fDir < 0.0f) {
                fDir += 6.2831855f;
            }
            CollAtkSetUnkA0(0, fDir);
            return;
        }

        if (pPush->x * pPush->x + pPush->z * pPush->z > 0.0f) {
            bcs::FVEC3 pushXZ(pPush->x, 0.0f, pPush->z);
            bcs::BCS_Vec_Normalize(dir, pushXZ);

            // Reflect the velocity off the push direction unless already moving away.
            float fDot = 2.0f * bcs::FVEC3Dot(&m_vVelocity, &dir);
            if (fDot <= 0.0f) {
                bcs::FVEC3 reflect;
                bcs::FVEC3Scale(&reflect, &dir, fDot);
                bcs::FVEC3Sub(&m_vVelocity, &m_vVelocity, &reflect);
            }
        }
    }

    // --- green shell ---

    CItemGShell::CItemGShell(CItemMan *pMan) : CItemShellCmn(pMan) {}

    CItemGShell::~CItemGShell() {}

    void CItemGShell::Create(const bcs::FVEC3 &pos)
    {
        CItemShellCmn::Create_(0, pos);
        CollAtkSetAtkType(0, 0x11C00);
    }

    float CItemGShell::GetThrowDir_(const bcs::FVEC3 &rPos, float fDir, int arg, long nTeam) const
    {
        return CItem::GetThrowDir(rPos, fDir, arg, nTeam);
    }

    // Thrown along rDir. With `arg` set it flies level for 7 units first.
    void CItemGShell::DoThrowInit(const bcs::FVEC3 &rDir, int arg)
    {
        float fSpeed = (1.0f / 60.0f) * m_fParamThrowSpeed;

        bcs::FVEC3Scale(&m_vVelocity, &rDir, fSpeed);
        m_vMove = m_vVelocity;
        if (arg != 0) {
            m_fGravity = 0.0f;
            m_fThrowTime = (fSpeed > 0.0f) ? 7.0f / fSpeed : 0.0f;
        } else {
            m_fGravity = -0.0125f;
            m_fThrowTime = 0.0f;
        }
    }

    void CItemGShell::CtorStateThrow()
    {
        m_fStateTimer = GetParamMainLife();
        m_nFlag |= 0x2004;
        m_hEffect = CEffectCtrl::CreateAttachEffect(9, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
        CollAtkSetEnable(0, 1);
        CollBodySetEnableAll(1);
        m_nFlag |= 0x400;
    }

    // Spins as it slides. Flag 0x200 is a bounce off a wall; after
    // GetParamReboundNum of them the attack takes the direction of the
    // throw. Breaks when its attack hits.
    void CItemGShell::LoopStateThrow()
    {
        float fRate = GetFrameRateF();

        DoMove();
        long nRebound = GetParamReboundNum();
        if (m_nFlag & 0x200) {
            m_nFlag &= ~0x200;
            if (++m_nReboundCount > nRebound) {
                CollAtkSetUnkA0(0, nw4r::math::Atan2Rad(m_vMove.x, m_vMove.z));
            } else {
                PlaySePos(0xC4D);
            }
        }

        AddRotY(0.5235988f * fRate); // 30 degrees a frame
        CollAtkSetPos(0, m_vPos);
        if (CollAtkIsAtkHit(0)) {
            CollAtkSetEnable(0, 0);
            CollBodySetEnableAll(0);
            ChangeStateBreak();
        }

        if (m_fThrowTime > 0.0f) {
            m_fThrowTime -= fRate;
            if (m_fThrowTime <= 0.0f) {
                m_fThrowTime = 0.0f;
                m_fGravity = -0.0125f;
            }
        }
    }

    void CItemGShell::DtorStateThrow()
    {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
    }

    // --- red shell ---

    CItemRShell::CItemRShell(CItemMan *pMan) : CItemShellCmn(pMan)
    {
        m_nHomingStep = 0;
        m_fUnk1BC = 4.0f;
        m_nTarget = -1;
        m_bHomingAir = 0;
        m_nUnk1C8 = 1;
    }

    CItemRShell::~CItemRShell() {}

    void CItemRShell::Create(const bcs::FVEC3 &pos)
    {
        m_fParamA0 = 6.0f;
        CItemShellCmn::Create_(1, pos);
        CollAtkSetAtkType(0, 0x11D00);
    }

    // Thrown along rDir; picks the character it will home in on.
    void CItemRShell::DoThrowInit(const bcs::FVEC3 &rDir, int arg)
    {
        // The direction of the throw as an angle in [0, 2 pi].
        float fDir = nw4r::math::Atan2Rad(rDir.x, rDir.z);
        while (fDir > 6.2831855f) {
            fDir -= 6.2831855f;
        }
        while (fDir < 0.0f) {
            fDir += 6.2831855f;
        }
        m_nTarget = SearchTarget_(m_vPos, fDir, arg, m_nUnk1AA);

        float fSpeed = (1.0f / 60.0f) * m_fParamThrowSpeed;
        bcs::FVEC3Scale(&m_vVelocity, &rDir, fSpeed);
        m_vMove = m_vVelocity;
        if (arg != 0 && m_nUnk1C8 != 0) {
            m_fGravity = 0.0f;
            m_fThrowTime = (fSpeed > 0.0f) ? 7.0f / fSpeed : 0.0f;
        } else {
            m_fGravity = -0.0125f;
            m_fThrowTime = 0.0f;
        }
    }

    void CItemRShell::CtorStateThrow()
    {
        m_fStateTimer = GetParamMainLife();
        m_nFlag |= 0x2004;
        m_hEffect = CEffectCtrl::CreateAttachEffect(10, m_pMdl, 0, 0, 0, 0, 0, 3, 0, 0, 0);
        CollBodySetEnableAll(1);
        CollAtkSetEnable(0, 1);
        m_nFlag |= 0x400;
        m_nHomingStep = 0;
        m_fUnk1BC = 4.0f;
    }

    // As the green shell's, steered by DoHoming_ first.
    void CItemRShell::LoopStateThrow()
    {
        float fRate = GetFrameRateF();

        DoHoming_();
        DoMove();
        long nRebound = GetParamReboundNum();
        if (m_nFlag & 0x200) {
            m_nFlag &= ~0x200;
            if (++m_nReboundCount > nRebound) {
                CollAtkSetUnkA0(0, nw4r::math::Atan2Rad(m_vMove.x, m_vMove.z));
            } else {
                PlaySePos(0xC4D);
            }
        }

        AddRotY(0.5235988f * fRate); // 30 degrees a frame
        CollAtkSetPos(0, m_vPos);
        if (CollAtkIsAtkHit(0)) {
            CollAtkSetEnable(0, 0);
            CollBodySetEnableAll(0);
            ChangeStateBreak();
        }

        if (m_fThrowTime > 0.0f) {
            m_fThrowTime -= fRate;
            if (m_fThrowTime <= 0.0f) {
                m_fThrowTime = 0.0f;
                m_fGravity = -0.0125f;
            }
        }
    }

    void CItemRShell::DtorStateThrow()
    {
        CEffectCtrl::StopAttachEffect(m_hEffect);
        m_hEffect = 0;
    }

    // Turns the velocity towards the target by at most m_fParamA0 degrees
    // a frame: in the XZ plane, or in 3D for a target in the air. Homing
    // stops for good (step 99) when there is no target, the shell is
    // close to it, or (in 3D) it is too far off to the side.
    void CItemRShell::DoHoming_()
    {
        bcs::FVEC3 targetPos;
        int bTarget = 0;

        switch (m_nHomingStep) {
        case 0:
            m_nHomingStep++;
            break;
        case 1:
            bTarget = GetHomingTargetPos(targetPos);
            if (!bTarget) {
                m_nHomingStep = 99;
            }
            break;
        }
        if (!bTarget) {
            return;
        }

        float fRate = GetFrameRateF();

        if (m_bHomingAir) {
            if (0.0f >= bcs::FVEC3Len(&m_vVelocity)) {
                if (m_nUnk1C8 == 0) {
                    m_bHomingAir = 0;
                    m_fThrowTime = 0.0f;
                    m_fGravity = -0.0125f;
                }
                m_nHomingStep = 99;
                return;
            }

            bcs::FVEC3 toTarget;
            bcs::FVEC3Sub(&toTarget, &targetPos, &m_vPos);
            if (bcs::FVEC3SquareMag(&toTarget) <= 4.0f) {
                return;
            }

            bcs::FVEC3 dir;
            bcs::FVEC3 dirTarget;
            bcs::BCS_Vec_Normalize(dir, m_vVelocity);
            bcs::BCS_Vec_Normalize(dirTarget, toTarget);
            float fTurn = fRate * bcs::DEG_TO_RAD(m_fParamA0);
            float fCos = bcs::FVEC3Dot(&dir, &dirTarget);

            if (fCos >= nw4r::math::CosRad(fTurn)) {
                dir = dirTarget;
            } else {
                // Turn dir by fTurn about dir x dirTarget (q * dir * q^-1),
                // unless the target is too far round to reach. (135 is in
                // radians, as the original has it.) The axis goes into
                // toTarget, which is not needed any more.
                if (fCos > nw4r::math::CosRad(135.0f)) {
                    PSVECCrossProduct((const Vec *)&dir, (const Vec *)&dirTarget, (Vec *)&toTarget);
                } else {
                    m_nHomingStep = 99;
                    if (m_nUnk1C8 == 0) {
                        m_bHomingAir = 0;
                        m_fThrowTime = 0.0f;
                        m_fGravity = -0.0125f;
                    }
                    return;
                }

                float fSin;
                float fHalfCos;
                Quaternion q;
                Quaternion v;
                Quaternion qInv;

                nw4r::math::SinCosRad(&fSin, &fHalfCos, 0.5f * fTurn);
                q.x = toTarget.x * fSin;
                q.y = toTarget.y * fSin;
                q.z = toTarget.z * fSin;
                q.w = fHalfCos;
                v.x = dir.x;
                v.y = dir.y;
                v.z = dir.z;
                v.w = 0.0f;
                qInv.x = -toTarget.x * fSin;
                qInv.y = -toTarget.y * fSin;
                qInv.z = -toTarget.z * fSin;
                qInv.w = fHalfCos;
                PSQUATMultiply(&q, &v, &v);
                PSQUATMultiply(&v, &qInv, &v);
                dir.x = v.x;
                dir.y = v.y;
                dir.z = v.z;
                bcs::BCS_Vec_Normalize(dir, dir);
            }
            bcs::FVEC3Scale(&m_vVelocity, &dir, (1.0f / 60.0f) * m_fParamThrowSpeed);
            return;
        }

        bcs::FVEC3 toTarget;
        bcs::FVEC3 velTarget;
        bcs::FVEC3 velR;
        bcs::FVEC3 velL;

        float fSpeed = bcs::FVEC3Len(&bcs::FVEC3(m_vVelocity.x, 0.0f, m_vVelocity.z));
        if (0.0f >= fSpeed) {
            if (m_nUnk1C8 == 0) {
                m_bHomingAir = 0;
                m_fThrowTime = 0.0f;
                m_fGravity = -0.0125f;
            }
            m_nHomingStep = 99;
            return;
        }

        toTarget.x = targetPos.x - m_vPos.x;
        toTarget.z = targetPos.z - m_vPos.z;
        if (bcs::FVEC3Len(&bcs::FVEC3(toTarget.x, 0.0f, toTarget.z)) <= 2.0f) {
            m_nHomingStep = 99;
            if (m_nUnk1C8 == 0) {
                m_fThrowTime = 0.0f;
                m_fGravity = -0.0125f;
            }
            return;
        }

        bcs::BCS_Vec_Normalize(velTarget, bcs::FVEC3(toTarget.x, 0.0f, toTarget.z));
        velTarget.x *= fSpeed;
        velTarget.z *= fSpeed;

        // Straight at the target if that is within the turn; otherwise the
        // full turn to whichever side gets closer.
        float fTurn = fRate * bcs::DEG_TO_RAD(m_fParamA0);
        bcs::BCS_Vec_RotY(velL, m_vVelocity, fTurn);
        float fDotL = m_vVelocity.x * velL.x + m_vVelocity.z * velL.z;
        float fDotTarget = m_vVelocity.x * velTarget.x + m_vVelocity.z * velTarget.z;
        if (fDotTarget >= fDotL) {
            m_vVelocity.x = velTarget.x;
            m_vVelocity.z = velTarget.z;
            return;
        }

        float fTurnR = -fTurn;
        while (fTurnR > 6.2831855f) {
            fTurnR -= 6.2831855f;
        }
        while (fTurnR < 0.0f) {
            fTurnR += 6.2831855f;
        }
        bcs::BCS_Vec_RotY(velR, m_vVelocity, fTurnR);
        fDotL = velL.x * toTarget.x + velL.z * toTarget.z;
        float fDotR = velR.x * toTarget.x + velR.z * toTarget.z;
        if (fDotR >= fDotL) {
            m_vVelocity.x = velR.x;
            m_vVelocity.z = velR.z;
        } else {
            m_vVelocity.x = velL.x;
            m_vVelocity.z = velL.z;
        }
    }

    // The target's position (on the ground under it when the map is hit
    // on the way), raised to the shell's height.
    int CItemRShell::GetHomingTargetPos(bcs::FVEC3 &rPos)
    {
        if (!CChrMan::IsActive()) {
            return 0;
        }
        if (m_nTarget < 0) {
            return 0;
        }
        CChrCore *pChr = CChrMan::GetChrCore(m_nTarget);
        if (pChr == 0) {
            return 0;
        }
        if (pChr->IsDisable()) {
            return 0;
        }

        pChr->GetTransNodePos(rPos);

        bcs::FVEC3 hitPos;
        bcs::FVEC3 hitNormal;
        float fHitTime;
        if (CCollMan::DetectMapHit(hitPos, hitNormal, fHitTime, rPos, rPos, GetBodyCollRadius(), 4,
                                   (m_nUnk1A6 & 0xFF) | ((m_nUnk1A4 & 0x7F) << 24))) {
            rPos.x = hitPos.x;
            rPos.z = hitPos.z;
        }
        rPos.y += GetParamHeight();
        return 1;
    }

    // The nearest character of another team than nTeam within
    // s_fSearchRangeH of fDir as seen from rPos, or -1. With `arg` set a
    // character in the air (status bit 0x400) within 12.5 units and
    // s_fSearchRangeV above comes first, and homing on it is done in 3D.
    long CItemRShell::SearchTarget_(const bcs::FVEC3 &rPos, float fDir, int arg, long nTeam)
    {
        m_bHomingAir = 0;
        m_nUnk1C8 = 1;
        if (!CChrMan::IsActive()) {
            return -1;
        }

        float fBestDist = -1.0f;
        long nTarget = -1;
        long nChrNum = CChrMan::GetChrCoreNum();

        if (arg == 0) {
            for (long i = 0; i < nChrNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr == 0 || pChr->IsDisable() || nTeam == pChr->GetTeamNo()) {
                    continue;
                }
                float fDist = pChr->GetDist_ToPosSq(rPos);
                float fDiff = pChr->GetRotY_FromPosXZ(rPos) - fDir;
                while (fDiff > 6.2831855f) {
                    fDiff -= 6.2831855f;
                }
                while (fDiff < 0.0f) {
                    fDiff += 6.2831855f;
                }
                if (fDiff > 3.1415927f) {
                    fDiff -= 6.2831855f;
                }
                if (fabsf(fDiff) <= s_fSearchRangeH && (fDist < fBestDist || fBestDist < 0.0f)) {
                    fBestDist = fDist;
                    nTarget = pChr->GetPlayerNo();
                }
            }
        } else {
            for (long i = 0; i < nChrNum; i++) {
                CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr == 0 || pChr->IsDisable() || nTeam == pChr->GetTeamNo() || !(pChr->GetStatusFlag() & 0x400)) {
                    continue;
                }
                float fDist = pChr->GetDist_ToPosSq(rPos);
                float fRotY = pChr->GetRotY_FromPosXZ(rPos);
                float fRotX = pChr->GetRotX_FromPos(rPos);
                float fDiff = fRotY - fDir;
                while (fDiff > 6.2831855f) {
                    fDiff -= 6.2831855f;
                }
                while (fDiff < 0.0f) {
                    fDiff += 6.2831855f;
                }
                if (fDiff > 3.1415927f) {
                    fDiff -= 6.2831855f;
                }
                float fAbsDiff = fabsf(fDiff);
                if (fRotX > 3.1415927f) {
                    fRotX -= 6.2831855f;
                }
                float fAbsRotX = fabsf(fRotX);
                if (fAbsDiff <= s_fSearchRangeH && fAbsRotX <= s_fSearchRangeV && fDist <= 156.25f
                    && (fDist < fBestDist || fBestDist < 0.0f)) {
                    fBestDist = fDist;
                    nTarget = pChr->GetPlayerNo();
                }
            }

            // Nobody in the air: the nearest one further than 12.5 units.
            if (nTarget < 0) {
                for (long i = 0; i < nChrNum; i++) {
                    CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr == 0 || pChr->IsDisable() || nTeam == pChr->GetTeamNo()) {
                        continue;
                    }
                    float fDist = pChr->GetDist_ToPosSq(rPos);
                    float fDiff = pChr->GetRotY_FromPosXZ(rPos) - fDir;
                    while (fDiff > 6.2831855f) {
                        fDiff -= 6.2831855f;
                    }
                    while (fDiff < 0.0f) {
                        fDiff += 6.2831855f;
                    }
                    if (fDiff > 3.1415927f) {
                        fDiff -= 6.2831855f;
                    }
                    if (fabsf(fDiff) <= s_fSearchRangeH && fDist > 156.25f
                        && (fDist < fBestDist || fBestDist < 0.0f)) {
                        fBestDist = fDist;
                        nTarget = pChr->GetPlayerNo();
                    }
                }
            }
        }

        if (m_nTarget >= 0 && arg != 0 && (CChrMan::GetChrCore(m_nTarget)->GetStatusFlag() & 0x400)) {
            m_bHomingAir = 1;
            m_nUnk1C8 = 0;
        }
        return nTarget;
    }
}
