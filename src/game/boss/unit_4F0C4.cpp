// msw::CComDrvBoss: the driver's own actions (g_ActFuncMap from 10 on),
// started by ActCall* and run every frame by ActLoop*. A file of its own
// before game/boss/CComDrvBoss.cpp (its own float constants); the file name
// is not known.
#include "game/msw/CComDrvBoss.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CQPanel.hpp"
#include "bcs/BCS_Vec.hpp"

namespace {
    // (names guessed) The work of the actions that run to a place: where
    // to.
    struct RUN_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Target;   // 0x1C
    };

    // The work of the attacks: the place to run to, and how far from the
    // Behemoth to attack it from.
    struct ATTACK_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Target;   // 0x1C
        float m_fRange;        // 0x28
    };

    // The work of going for a "?" panel: where it is, and which.
    struct QPANEL_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Target;   // 0x1C
        unsigned long m_nPanel; // 0x28
    };

    // (guessed helper) The direction from rFrom to rTo in the XZ plane,
    // length 1 (zero when they are at one place).
    inline void GetDirXZ(bcs::FVEC3 &rDir, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 diff;
        diff.x = rTo.x - rFrom.x;
        diff.z = rTo.z - rFrom.z;
        bcs::BCS_Vec_Normalize(rDir, bcs::FVEC3(diff.x, 0.0f, diff.z));
    }

    // (guessed helper) rVec brought to length fLen in the XZ plane; its y
    // is kept.
    inline void SetLengthXZ(bcs::FVEC3 &rVec, float fLen)
    {
        float y = rVec.y;
        bcs::BCS_Vec_Normalize(rVec, bcs::FVEC3(rVec.x, 0.0f, rVec.z));
        rVec.x *= fLen;
        rVec.z *= fLen;
        rVec.y = y;
    }
}

namespace msw {

void CComDrvBoss::ActCallGoHome()
{
    EntryAct(ACT_GO_HOME);
}

// Faces the Behemoth, waits a little, runs to the home position and faces
// the Behemoth again; gives up after five to seven seconds.
void CComDrvBoss::ActLoopGoHome()
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(GetCurrentActWork());
    if (pWork->m_fActTime > 60.0f * RandRangeF(5.0f, 7.0f)) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            ActCallLookTarget(m_BossPos);
            pWork->m_nStep++;
            break;
        case 1:
            ActCallWait(RandRangeF(15.0f, 30.0f));
            pWork->m_nStep++;
            break;
        case 2:
            GetHomePos(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3:
            if (MethdRelayRun(pWork->m_Target, 1.0f)) {
                pWork->m_nStep++;
            }
            break;
        case 4:
            ActCallLookTarget(m_BossPos);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBoss::ActCallGoRand()
{
    EntryAct(ACT_GO_RAND);
}

// The same to a random place near the home position.
void CComDrvBoss::ActLoopGoRand()
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(GetCurrentActWork());
    if (pWork->m_fActTime > 60.0f * RandRangeF(5.0f, 7.0f)) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            ActCallLookTarget(m_BossPos);
            pWork->m_nStep++;
            break;
        case 1:
            ActCallWait(RandRangeF(15.0f, 30.0f));
            pWork->m_nStep++;
            break;
        case 2:
            GetHomeRandPos(pWork->m_Target);
            pWork->m_nStep++;
            break;
        case 3:
            if (MethdRelayRun(pWork->m_Target, 1.0f)) {
                pWork->m_nStep++;
            }
            break;
        case 4:
            ActCallLookTarget(m_BossPos);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Runs to the ball (m_pBall's position) until the character holds it.
void CComDrvBoss::ActLoopGetBall()
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldBall()) {
        ReturnAct(1);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2:
            pWork->m_Target = m_BallPos;
            if (MethdRelayRun(pWork->m_Target, 0.3f)) {
                pWork->m_nStep++;
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBoss::ActCallReceiveGo()
{
    EntryAct(ACT_RECEIVE_GO);
}

// Runs under the falling ball; dives for it when it is about to land close
// by.
void CComDrvBoss::ActLoopReceiveGo()
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(GetCurrentActWork());
    if (m_pBall == 0) {
        ReturnAct(0);
        return;
    }
    if (m_pChr->IsHoldBall()) {
        ReturnAct(1);
        return;
    }
    if (!GetBallTargetPos(pWork->m_Target)) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2: {
            float fDist = CComMan::GetDisXZ(m_MyPos, pWork->m_Target);
            float fFrame = m_pBall->GetTouchDownFrame(0.3f);
            if (m_pChr->IsVolleyEnableDiveReceive() && CanbeDiveReceive() && fFrame <= 20.0f && fDist < 4.0f) {
                MethdDiveReceive();
                ReturnAct(1);
                break;
            }
            MethdRelayRun(pWork->m_Target, 0.3f);
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

// Runs under the falling ball and spikes it when it is low enough.
void CComDrvBoss::ActLoopSpikeGo()
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(GetCurrentActWork());
    if (m_pBall == 0) {
        ReturnAct(0);
        return;
    }
    if (m_pChr->IsHoldBall()) {
        ReturnAct(1);
        return;
    }
    if (!GetBallTargetPos(pWork->m_Target)) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2: {
            float fDist = CComMan::GetDisXZ(m_MyPos, pWork->m_Target);
            if (m_pBall->GetTouchDownFrame(2.5f) <= 60.0f && fDist < 10.0f) {
                if (m_pChr->MethdSpike(0)) {
                    ReturnAct(1);
                    break;
                }
            }
            MethdRelayRun(pWork->m_Target, 0.3f);
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBoss::ActCallStickAttack()
{
    ATTACK_WORK *pWork = static_cast<ATTACK_WORK *>(EntryAct(ACT_STICK_ATTACK));
    pWork->m_fRange = RandRange(4.0f, 5.5f);
    pWork->m_Target = m_MyPos;
    m_nStickAttackStep = 0;
}

// Runs to within m_fRange of the Behemoth and swings the stick, then waits
// one to two seconds.
void CComDrvBoss::ActLoopStickAttack()
{
    ATTACK_WORK *pWork = static_cast<ATTACK_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2:
            if (GetDisXZ(m_BossPos, m_MyPos) > 1.0f + pWork->m_fRange) {
                bcs::FVEC3 dir;
                GetDirXZ(dir, m_BossPos, m_MyPos);
                SetLengthXZ(dir, pWork->m_fRange);
                bcs::FVEC3Add(&pWork->m_Target, &dir, &m_BossPos);
                if (MethdRelayRun(pWork->m_Target, 0.5f)) {
                    pWork->m_nStep++;
                }
                break;
            }
            pWork->m_nStep++;
            break;
        case 3:
            m_nStickAttackStep = 1;
            if (MethdShootStance()) {
                pWork->m_fTime = -RandRange(60.0f, 120.0f);
                pWork->m_nStep++;
                break;
            }
            ReturnAct(0);
            break;
        case 4:
            if (pWork->m_fTime >= 0.0f) {
                m_nStickAttackStep = 2;
                pWork->m_nStep++;
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBoss::ActCallShoot()
{
    ATTACK_WORK *pWork = static_cast<ATTACK_WORK *>(EntryAct(ACT_SHOOT));
    pWork->m_fRange = RandRange(7.0f, 9.0f);
    pWork->m_Target = m_MyPos;
}

// Runs to within m_fRange of the Behemoth and shoots the ball at it.
void CComDrvBoss::ActLoopShoot()
{
    ATTACK_WORK *pWork = static_cast<ATTACK_WORK *>(GetCurrentActWork());
    if (!m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2:
            if (MethdDunk()) {
                ReturnAct(1);
                break;
            }
            if (GetDisXZ(m_BossPos, m_MyPos) > 1.0f + pWork->m_fRange) {
                bcs::FVEC3 dir;
                GetDirXZ(dir, m_BossPos, m_MyPos);
                SetLengthXZ(dir, pWork->m_fRange);
                bcs::FVEC3Add(&pWork->m_Target, &dir, &m_BossPos);
                if (MethdRelayRun(pWork->m_Target, 0.5f)) {
                    pWork->m_nStep++;
                }
                break;
            }
            pWork->m_nStep++;
            break;
        case 3:
            MethdShoot(m_BossPos);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBoss::ActCallThrowItem()
{
    ATTACK_WORK *pWork = static_cast<ATTACK_WORK *>(EntryAct(ACT_THROW_ITEM));
    pWork->m_fRange = RandRange(9.0f, 11.0f);
    pWork->m_Target = m_MyPos;
}

// Runs to within m_fRange of the Behemoth and throws the item at it.
void CComDrvBoss::ActLoopThrowItem()
{
    ATTACK_WORK *pWork = static_cast<ATTACK_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }
    if (!IsHaveMyStockItem()) {
        ReturnAct(0);
        return;
    }

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            pWork->m_nStep++;
            break;
        case 1:
            pWork->m_nStep++;
            break;
        case 2:
            if (GetDisXZ(m_BossPos, m_MyPos) > 1.0f + pWork->m_fRange) {
                bcs::FVEC3 dir;
                GetDirXZ(dir, m_BossPos, m_MyPos);
                SetLengthXZ(dir, pWork->m_fRange);
                bcs::FVEC3Add(&pWork->m_Target, &dir, &m_BossPos);
                if (MethdRelayRun(pWork->m_Target, 0.5f)) {
                    pWork->m_nStep++;
                }
                break;
            }
            pWork->m_nStep++;
            break;
        case 3:
            MethdItemShoot(m_BossPos);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBoss::ActCallGetQpanel()
{
    EntryAct(ACT_GET_QPANEL);
}

// Faces the Behemoth, waits, then runs to the nearest "?" panel while it
// can still be taken.
void CComDrvBoss::ActLoopGetQpanel()
{
    QPANEL_WORK *pWork = static_cast<QPANEL_WORK *>(GetCurrentActWork());

    switch (pWork->m_nStep) {
        case 0:
            ResetRelayRun();
            ActCallLookTarget(m_BossPos);
            pWork->m_nStep++;
            break;
        case 1:
            ActCallWait(RandRangeF(15.0f, 30.0f));
            pWork->m_nStep++;
            break;
        case 2:
            pWork->m_nPanel = CComMan::QPanel_GetNear(m_MyPos, 1);
            if (pWork->m_nPanel == 0) {
                ReturnAct(0);
                break;
            }
            pWork->m_nStep++;
            break;
        case 3: {
            CQPanel *pPanel = CComMan::QPanel_GetPtr(pWork->m_nPanel);
            if (pPanel == 0) {
                ReturnAct(0);
                break;
            }
            if (!pPanel->CanAccess()) {
                ReturnAct(0);
                break;
            }
            pWork->m_Target = CComMan::QPanel_GetPos(pWork->m_nPanel);
            if (MethdRelayRun(pWork->m_Target, 0.3f)) {
                pWork->m_nStep++;
            }
            break;
        }
        case 4:
            ActCallLookTarget(m_BossPos);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

} // namespace msw
