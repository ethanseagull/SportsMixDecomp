// msw::CComDrvBasketGame's own actions, started by ActCall* and run every
// frame by ActLoop*, and the action table g_ActFuncMap that lists them. A
// file of its own with its own float constants; the file name is not known.
#include <math.h>

#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/BCS_ShakerSort.hpp"
#include "bcs/CSimpleSortBase.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CBallBasket.hpp"
#include "game/msw/CBallMan.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CComDrvBasketGame.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CComManBasket.hpp"
#include "game/msw/CComManBasketGame.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CItem.hpp"
#include "game/msw/CNpcGhostBasketBase.hpp"
#include "game/msw/CStageMan.hpp"

namespace {
    // (names guessed) The work most of the actions share: a timer and the
    // place they run to.
    struct BK_WORK : msw::CComDrv::ACT_WORK {
        float m_fTimer;    // 0x1C
        bcs::FVEC3 m_Pos;  // 0x20
    };

    // (names guessed) The work of the actions that keep a second timer
    // before the place.
    struct BK_WORK2 : msw::CComDrv::ACT_WORK {
        float m_fTimer;    // 0x1C
        float m_fTimer2;   // 0x20
        bcs::FVEC3 m_Pos;  // 0x24
    };
}

namespace msw {

void CComDrvBasketGame::ActCallWaitRand(long nMin, long nMax)
{
    ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
}

void CComDrvBasketGame::ActCallJumpBall()
{
    EntryAct(ACT_JUMP_BALL);
}

// Waits a little, then jumps for the ball of the jump ball.
void CComDrvBasketGame::ActLoopJumpBall()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            float fLv = GetComLvF();
            ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0xDBF, fLv), CComMan::GetDatI(0xDC7, fLv)));
            pWork->m_nStep++;
            break;
        }
        case 1: {
            BK_WORK *pJump = static_cast<BK_WORK *>(EntryAct(ACT_JUMP2));
            pJump->m_fTimer = -1.0f;
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoOffense()
{
    EntryAct(ACT_GO_OFFENSE);
}

// With the ball: runs towards a team mate. Without: runs home and waits.
void CComDrvBasketGame::ActLoopGoOffense()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldBall()) {
        switch (pWork->m_nStep) {
            case 0: {
                static const long nArrayNum = 3;
                const CChrCore *apChr[nArrayNum];
                long nNum = CComManBasketGame::GetMemBer(m_nMyTeam, apChr, nArrayNum, -1, m_pChr);
                pWork->m_Pos = m_MyPos;
                if (nNum > 1) {
                    pWork->m_Pos = apChr[bcs::BCS_RandMax(nNum - 1)]->GetPos();
                } else if (nNum == 1) {
                    pWork->m_Pos = apChr[0]->GetPos();
                }
                pWork->m_Pos = CComMan::World2Local(pWork->m_Pos, -1);
                pWork->m_nStep++;
            }
            case 1:
                if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                    pWork->m_nStep++;
                }
                break;
            default:
                ReturnAct(1);
                break;
        }
    } else {
        switch (pWork->m_nStep) {
            case 0:
                GetHomePos(pWork->m_Pos);
                if (!CheckMoveToTargetDist(pWork->m_Pos)) {
                    ActCallLookBall();
                    pWork->m_nStep = 2;
                    break;
                }
                pWork->m_fTimer = 0.0f;
                pWork->m_nStep++;
            case 1:
                if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                    ActCallLookBall();
                    pWork->m_nStep = 2;
                }
                break;
            case 2: {
                float fLv = GetComLvF();
                ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0x888, fLv), CComMan::GetDatI(0x890, fLv)));
                pWork->m_nStep++;
                break;
            }
            default:
                ReturnAct(1);
                break;
        }
    }
}

void CComDrvBasketGame::ActCallGoOffense2()
{
    EntryAct(ACT_GO_OFFENSE2);
}

// Runs to a place at the side of the court in front of the goal it
// attacks; waits there unless it holds the ball.
void CComDrvBasketGame::ActLoopGoOffense2()
{
    static const long NUM = 2;
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            pWork->m_Pos.z = m_pChr->GetTargetGoalPos().z;
            pWork->m_Pos.z = pWork->m_Pos.z > 0.0f ? 8.0f : -8.0f;
            bcs::FVEC3 aPos[NUM] = {
                bcs::FVEC3(5.0f, 0.0f, pWork->m_Pos.z),
                bcs::FVEC3(-5.0f, 0.0f, pWork->m_Pos.z),
            };
            bcs::FVEC3 myPos = m_pChr->GetPos();
            bool bLeft = m_pChr->GetPos().x < 0.0f;
            float fRand = RandRangeF(0.0f, CComMan::GetDatF(0x8B8, GetComLvF()));
            if (bLeft) {
                pWork->m_Pos.x = fRand + (2.0f + CComMan::GetStageCourtLeft(0));
            } else {
                pWork->m_Pos.x = CComMan::GetStageCourtRight(0) - 2.0f - fRand;
            }
            pWork->m_Pos.z = aPos[0].z;
            float fMax = CComMan::GetDatF(0x8B0, GetComLvF());
            bcs::FVEC3Sub(&pWork->m_Pos, &pWork->m_Pos, &myPos);
            float fLen = bcs::BCS_Vec_GetLenXZ(pWork->m_Pos);
            if (fLen > fMax) {
                fLen = fMax;
            }
            bcs::FVEC3 dir(pWork->m_Pos.x, 0.0f, pWork->m_Pos.z);
            bcs::BCS_Vec_Normalize(pWork->m_Pos, dir);
            pWork->m_Pos.x = pWork->m_Pos.x * fLen + myPos.x;
            pWork->m_Pos.z = pWork->m_Pos.z * fLen + myPos.z;
            pWork->m_Pos = CComMan::World2Local(pWork->m_Pos, -1);
            pWork->m_nStep++;
            if (!CheckMoveToTargetDist(pWork->m_Pos)) {
                if (m_pChr->IsHoldBall()) {
                    ReturnAct(0);
                } else {
                    pWork->m_nStep = 2;
                    ActCallLookBall();
                }
                break;
            }
        }
        case 1:
            if (!MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                break;
            }
            pWork->m_nStep++;
            if (!m_pChr->IsHoldBall()) {
                ActCallLookBall();
                break;
            }
        case 2:
            if (m_pChr->IsHoldBall()) {
                ReturnAct(1);
                break;
            }
            ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0x8A0, GetComLvF()),
                                                     CComMan::GetDatI(0x8A8, GetComLvF())));
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoDefense()
{
    EntryAct(ACT_GO_DEFENSE);
}

// Runs home on defence, keeping to the character it marks, and waits.
void CComDrvBasketGame::ActLoopGoDefense()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            GetHomePos(pWork->m_Pos);
            if (!CheckMoveToTargetDist(pWork->m_Pos)) {
                if (!m_pChr->IsNowBlockWork()) {
                    ActCallLookBall();
                }
                pWork->m_nStep = 2;
                break;
            }
            pWork->m_fTimer = 0.0f;
            pWork->m_nStep++;
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall != 0 && pBall->IsPassMode() &&
                static_cast<CBallBasket *>(pBall)->GetPassTargetNo() == m_pMarkChr->GetPlayerNo()) {
                long nMin = CComMan::GetDatF(0x8C0, fLv);
                long nMax = CComMan::GetDatF(0x8C8, fLv);
                if (nMin > 0 || nMax > 0) {
                    ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
                    break;
                }
            }
        }
        case 1:
            if (m_pMarkChr != 0 && !m_pMarkChr->IsHoldBall()) {
                pWork->m_fTimer -= m_pChr->GetFrameStep();
                if (pWork->m_fTimer <= 0.0f) {
                    pWork->m_fTimer = CComMan::GetDatF(0x948, GetComLvF());
                    GetHomePos(pWork->m_Pos);
                }
            }
            if (CheckChangeMarkCore() || IsNoStopDefense()) {
                GetHomePos(pWork->m_Pos);
            }
            if (!MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                break;
            }
            pWork->m_nStep = 2;
            if (!m_pChr->IsNowBlockWork()) {
                ActCallLookBall();
                break;
            }
        case 2:
            if (m_pMarkChr != 0 && m_pMarkChr->IsHoldBall()) {
                ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0x8F8, fLv), CComMan::GetDatI(0x900, fLv)));
            } else {
                ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0x908, fLv), CComMan::GetDatI(0x910, fLv)));
            }
            pWork->m_nStep = 999;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoDefense2()
{
    EntryAct(ACT_GO_DEFENSE2);
}

// Keeps between the character it marks and the goal that character
// attacks, a little ahead of where it moves, for a while.
void CComDrvBasketGame::ActLoopGoDefense2()
{
    BK_WORK2 *pWork = static_cast<BK_WORK2 *>(GetCurrentActWork());
    if (!IsActCallDefenseExt()) {
        ReturnAct(0);
        return;
    }
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fTimer = CComMan::GetDatF(0x950, fLv);
            pWork->m_fTimer2 = 0.0f;
            pWork->m_nStep++;
        case 1: {
            static const float fDist = 0.0f;
            pWork->m_fTimer2 -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer2 <= 0.0f) {
                pWork->m_fTimer2 = CComMan::GetDatF(0x958, fLv);
                bcs::BCS_VecZ_RotY(pWork->m_Pos, 1.0f, m_pMarkChr->GetRotY_ToGoal());
                pWork->m_Pos.x *= fDist;
                pWork->m_Pos.z *= fDist;
                bcs::FVEC3 markPos = CComMan::GetChrPos(m_pMarkChr);
                bcs::FVEC3Add(&pWork->m_Pos, &pWork->m_Pos, &markPos);
                float fDir = static_cast<const CChrCoreBasket *>(m_pMarkChr)->GetNowMoveDir();
                if (fDir != -1.0f) {
                    bcs::FVEC3 dir;
                    bcs::BCS_VecZ_RotY(dir, 1.0f, fDir);
                    pWork->m_Pos.x += 2.0f * dir.x;
                }
                pWork->m_Pos.y = 0.0f;
            }
            MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer > 0.0f) {
                break;
            }
            pWork->m_nStep++;
            if (!IsNoStopDefense()) {
                long nWait = CComMan::GetDatF(0x960, fLv);
                if (nWait > 0) {
                    ActCallWait(nWait);
                    break;
                }
            }
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoDefense3()
{
    EntryAct(ACT_GO_DEFENSE3);
}

} // namespace msw

// (guessed helpers) An angle brought into [0, 2 pi), and into (-pi, pi].
namespace {
    const float PI = 3.1415927f;
    const float PI2 = 6.2831855f;

    inline float RoundRad(float fRad)
    {
        while (fRad > PI2) {
            fRad -= PI2;
        }
        while (fRad < 0.0f) {
            fRad += PI2;
        }
        return fRad;
    }

    inline float RoundRadPI(float fRad)
    {
        fRad = RoundRad(fRad);
        if (fRad > PI) {
            fRad -= PI2;
        }
        return fRad;
    }

    typedef bcs::CSimpleSortBase<32>::SORT SORT;
}

namespace msw {

// Keeps close to the character it marks, between it and the goal it
// attacks; while blocking, stands a random step to its side.
void CComDrvBasketGame::ActLoopGoDefense3()
{
    BK_WORK2 *pWork = static_cast<BK_WORK2 *>(GetCurrentActWork());
    if (!IsActCallDefenseExt()) {
        ReturnAct(0);
        return;
    }
    float fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fTimer = CComMan::GetDatF(0x968, fLv);
            pWork->m_fTimer2 = 0.0f;
            pWork->m_nStep++;
        case 1: {
            static const float fDist = 0.0f;
            pWork->m_fTimer2 -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer2 <= 0.0f) {
                pWork->m_fTimer2 = CComMan::GetDatF(0x970, fLv);
                bcs::BCS_VecZ_RotY(pWork->m_Pos, 1.0f, m_pMarkChr->GetRotY_ToGoal());
                pWork->m_Pos.x *= fDist;
                pWork->m_Pos.z *= fDist;
                bcs::FVEC3 markPos = CComMan::GetChrPos(m_pMarkChr);
                bcs::FVEC3Add(&pWork->m_Pos, &pWork->m_Pos, &markPos);
                pWork->m_Pos.y = 0.0f;
                if (m_pChr->IsNowBlockWork()) {
                    bcs::FVEC3 side;
                    bcs::BCS_VecZ_RotY(side, 1.0f, m_pMarkChr->GetRotY_ToGoal());
                    float fX = RandRangeF(CComMan::GetDatF(0x978, fLv), CComMan::GetDatF(0x980, fLv));
                    float fRot = RoundRadPI(m_pMarkChr->GetRotY_ToPosXZ(m_pChr->GetPos()) - m_pMarkChr->GetRotY_ToGoal());
                    if (fRot > 0.0f) {
                        fX = -fX;
                    }
                    float fZ = RandRangeF(CComMan::GetDatF(0x988, fLv), CComMan::GetDatF(0x990, fLv));
                    if (RandMaxI(1)) {
                        fZ = -fZ;
                    }
                    side.x = fX;
                    side.z = fZ;
                    bcs::BCS_Vec_RotY(side, side, m_pMarkChr->GetRotY_ToGoal());
                    pWork->m_Pos = m_MyPos;
                    pWork->m_Pos.y = 0.0f;
                    pWork->m_Pos.x += side.x;
                    pWork->m_Pos.z += side.z;
                } else {
                    float fDir = static_cast<const CChrCoreBasket *>(m_pMarkChr)->GetNowMoveDir();
                    if (fDir != -1.0f) {
                        bcs::FVEC3 dir;
                        bcs::BCS_VecZ_RotY(dir, 1.0f, fDir);
                        pWork->m_Pos.x += 2.0f * dir.x;
                    }
                }
                pWork->m_Pos.y = 0.0f;
            }
            MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer > 0.0f) {
                break;
            }
            pWork->m_nStep++;
            if (!IsNoStopDefense()) {
                long nWait = CComMan::GetDatF(0x998, fLv);
                if (nWait > 0) {
                    ActCallWait(nWait);
                    break;
                }
            }
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoDefPassCourse()
{
    EntryAct(ACT_GO_DEF_PASS_COURSE);
}

// Stands in the course of a pass from the ball holder to the character it
// marks, a little ahead of where that one moves.
void CComDrvBasketGame::ActLoopGoDefPassCourse()
{
    BK_WORK2 *pWork = static_cast<BK_WORK2 *>(GetCurrentActWork());
    float fLv = GetComLvF();
    int bCount = 0;
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fTimer = CComMan::GetDatF(0x9A0, fLv);
            pWork->m_fTimer2 = 0.0f;
            pWork->m_nStep++;
        case 1:
        case 2:
        case 3: {
            bCount = 1;
            const CChrCore *pHolder = CChrMan::Util_SearchHaveBallChr(0);
            if (pHolder == 0) {
                ReturnAct(0);
                return;
            }
            bcs::FVEC3 home;
            GetHomePos(home);
            if (m_pMarkChr != 0 && m_pMarkChr->IsHoldBall()) {
                ReturnAct(0);
                return;
            }
            pWork->m_fTimer2 -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer2 <= 0.0f) {
                static const float fDist = 3.5f;
                static const float fScale = 2.0f;
                pWork->m_fTimer2 = CComMan::GetDatF(0x9A8, fLv);
                bcs::FVEC3 markPos = m_pMarkChr->GetPos();
                float fDir = static_cast<const CChrCoreBasket *>(m_pMarkChr)->GetNowMoveDir();
                if (fDir != -1.0f) {
                    bcs::FVEC3 dir;
                    bcs::BCS_VecZ_RotY(dir, 1.0f, fDir);
                    markPos.x += fScale * dir.x;
                    markPos.z += fScale * dir.z;
                }
                bcs::FVEC3 diff;
                bcs::FVEC3Sub(&diff, &pHolder->GetPos(), &markPos);
                bcs::FVEC3 xz(diff.x, 0.0f, diff.z);
                bcs::FVEC3 dir;
                bcs::BCS_Vec_Normalize(dir, xz);
                pWork->m_Pos.x = fDist * dir.x + markPos.x;
                pWork->m_Pos.y = 0.0f;
                pWork->m_Pos.z = fDist * dir.z + markPos.z;
                pWork->m_Pos = CComMan::World2Local(pWork->m_Pos, -1);
                if (pWork->m_nStep == 1 || pWork->m_nStep == 2) {
                    if (!CheckMoveToTargetDist(pWork->m_Pos)) {
                        if (static_cast<CChrCoreBasket *>(m_pChr)->GetNowMoveDir() == -1.0f) {
                            pWork->m_nStep = 3;
                        } else {
                            pWork->m_nStep = 2;
                        }
                    } else {
                        pWork->m_nStep = 2;
                    }
                }
            }
            if (pWork->m_nStep == 2) {
                if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                    pWork->m_nStep = 3;
                }
            } else if (pWork->m_nStep == 3) {
                float fRot = m_pChr->GetRotY();
                if (!m_pChr->IsNowBlockWork()) {
                    CBall *pBall = CBallMan::GetBallPtr(0);
                    if (pBall != 0) {
                        fRot = m_pChr->GetRotY_ToPosXZ(pBall->GetPos());
                    }
                }
                if (MethdRotateY(fRot, 10)) {
                    pWork->m_nStep = 1;
                }
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
    if (bCount) {
        pWork->m_fTimer -= m_pChr->GetFrameStep();
        if (pWork->m_fTimer <= 0.0f) {
            ActCallWait(CComMan::GetDatI(0x9B0, fLv));
            ActCallLookBall();
            pWork->m_nStep = 999;
        }
    }
}

void CComDrvBasketGame::ActCallGoDefOnBaller()
{
    EntryAct(ACT_GO_DEF_ON_BALLER);
}

// Marks the ball holder: stands to one side of it, between it and the
// goal; of the team the one nearest that side takes it.
void CComDrvBasketGame::ActLoopGoDefOnBaller()
{
    BK_WORK2 *pWork = static_cast<BK_WORK2 *>(GetCurrentActWork());
    m_pMarkChr = CChrMan::Util_SearchHoldBallChr(0);
    if (m_pMarkChr == 0) {
        ReturnAct(0);
        return;
    }
    if (!m_pMarkChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }
    const float &fLv = GetComLvF();
    int bCount = 0;
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fTimer = CComMan::GetDatF(0x9B8, fLv);
            pWork->m_fTimer2 = 0.0f;
            pWork->m_nStep++;
        case 1:
        case 2:
        case 3:
            bCount = 1;
            pWork->m_fTimer2 -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer2 <= 0.0f) {
                static const float fScale = 2.0f;
                pWork->m_fTimer2 = CComMan::GetDatF(0x9C0, fLv);
                float fAngle = 0.43633232f;
                bcs::FVEC3 left;
                bcs::BCS_VecZ_RotY(left, 1.0f, fAngle + m_pMarkChr->GetRotY_ToGoal());
                bcs::FVEC3 right;
                bcs::BCS_VecZ_RotY(right, 1.0f, m_pMarkChr->GetRotY_ToGoal() - fAngle);
                bcs::FVEC3 markPos = m_pMarkChr->GetPos();
                float fDir = static_cast<const CChrCoreBasket *>(m_pMarkChr)->GetNowMoveDir();
                if (fDir != -1.0f) {
                    bcs::FVEC3 dir;
                    bcs::BCS_VecZ_RotY(dir, 1.0f, fDir);
                    markPos.x += fScale * dir.x;
                }
                float fLeft = markPos.x > 0.0 ? 1.5f : 3.5f;
                left.x *= fLeft;
                left.z *= fLeft;
                bcs::FVEC3Add(&left, &left, &markPos);
                float fRight = markPos.x < 0.0 ? 1.5f : 3.5f;
                right.x *= fRight;
                right.z *= fRight;
                bcs::FVEC3Add(&right, &right, &markPos);

                SORT aSort[32];
                SORT *pSort = aSort;
                long nNum = 0;
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr->GetState() != 6) {
                        float fDist = CComManBasket::GetDistXZ(pChr->GetPos(), left);
                        nNum++;
                        pSort->m_nId = pChr->GetPlayerNo();
                        pSort->m_fParam = fDist;
                        pSort++;
                    }
                }
                if (nNum != 1) {
                    bcs::BCS_ShakerSort(aSort, nNum, bcs::CSimpleSortBase<32>::SortCmpParamF);
                }
                const bcs::FVEC3 *pPos = &right;
                if (aSort[0].m_nId == m_pChr->GetPlayerNo()) {
                    pPos = &left;
                }
                pWork->m_Pos = *pPos;
                pWork->m_Pos = CComMan::World2Local(pWork->m_Pos, -1);
                pWork->m_Pos.y = 0.0f;
                pWork->m_Pos = CComMan::World2Local(pWork->m_Pos, -1);
                if (pWork->m_nStep == 1 || pWork->m_nStep == 2) {
                    if (!CheckMoveToTargetDist(pWork->m_Pos)) {
                        if (static_cast<CChrCoreBasket *>(m_pChr)->GetNowMoveDir() == -1.0f) {
                            pWork->m_nStep = 3;
                        } else {
                            pWork->m_nStep = 2;
                        }
                    } else {
                        pWork->m_nStep = 2;
                    }
                }
            }
            if (pWork->m_nStep == 2) {
                if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                    pWork->m_nStep = 3;
                }
            } else if (pWork->m_nStep == 3) {
                if (MethdRotateY(m_pChr->GetRotY_ToPosXZ(m_pMarkChr->GetPos()), 10)) {
                    pWork->m_nStep = 1;
                }
            }
            break;
        default:
            ReturnAct(1);
            break;
    }
    if (bCount) {
        pWork->m_fTimer -= m_pChr->GetFrameStep();
        if (pWork->m_fTimer <= 0.0f) {
            ActCallWait(CComMan::GetDatI(0x9C8, fLv));
            ActCallLookBall();
            pWork->m_nStep = 999;
        }
    }
}

} // namespace msw

namespace {
    // (name guessed) The work of the actions that only keep where they
    // run to.
    struct POS_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Pos;       // 0x1C
    };

    // (names guessed) The work of ActLoopGetHatenaPanel: the panel and
    // where it is.
    struct PANEL_WORK : msw::CComDrv::ACT_WORK {
        unsigned long m_nPanel; // 0x1C
        bcs::FVEC3 m_Pos;       // 0x20
    };

    // (names guessed) The work of ActLoopGoRun: ActCallGoRun's arguments.
    struct RUN_WORK : msw::CComDrv::ACT_WORK {
        unsigned long m_nFlag;  // 0x1C, bit 1: keep running on arrival
        float m_fMinDist;       // 0x20
        float m_fMaxFrame;      // 0x24, given up after this many frames
        bcs::FVEC3 m_Pos;       // 0x28
    };
}

namespace msw {

void CComDrvBasketGame::ActCallGoHome()
{
    EntryAct(ACT_GO_HOME);
}

// Goes on offence or defence, whichever the driver's team is on.
void CComDrvBasketGame::ActLoopGoHome()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        if (CGame::g_GameWork.m_nUserTeam == m_nMyTeam) {
            EntryAct(ACT_GO_OFFENSE);
        } else {
            EntryAct(ACT_GO_DEFENSE);
        }
        pWork->m_nStep++;
    } else {
        ReturnAct(1);
    }
}

// Runs to a panel of the free area, drawn by the panels' scores.
void CComDrvBasketGame::ActLoopGoFreeArea()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            static const long nMax = 128;
            long aRate[nMax];
            long nNum = 0;
            long nLeft;
            long nRight;
            long nTop;
            long nBottom;
            GetFreeAreaRect(nLeft, nRight, nTop, nBottom);
            long *pRate = aRate;
            for (long h = nTop; h <= nBottom; h++) {
                for (long w = nLeft; w <= nRight; w++) {
                    *pRate++ = GetPanelDate(w, h);
                    nNum++;
                }
            }
            long nIdx = LotProbability(aRate, nNum);
            long nWidth = nRight - nLeft + 1;
            float fLeft;
            float fRight;
            float fTop;
            float fBottom;
            GetPanelRect(nLeft + nIdx % nWidth, nTop + nIdx / nWidth, fLeft, fRight, fTop, fBottom);
            pWork->m_Pos.x = (fRight - fLeft) * 0.5f + fLeft;
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = (fBottom - fTop) * 0.5f + fTop;
            pWork->m_Pos = World2Local(pWork->m_Pos, -1);
            pWork->m_nStep++;
        }
        case 1:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ReturnAct(1);
            }
            break;
    }
}

void CComDrvBasketGame::ActCallGetBall()
{
    EntryAct(ACT_GET_BALL);
}

// Waits a little, then runs to the ball while it is in play.
void CComDrvBasketGame::ActLoopGetBall()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0:
            ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0xDAF, fLv), CComMan::GetDatI(0xDB7, fLv)));
            pWork->m_nStep++;
            break;
        case 1: {
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall == 0) {
                ReturnAct(0);
                break;
            }
            if (!pBall->IsActive()) {
                ReturnAct(0);
                break;
            }
            pWork->m_Pos = CComMan::GetBallPos(pBall);
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &pWork->m_Pos, &m_MyPos);
            bcs::FVEC3 d = diff;
            if (bcs::BCS_Vec_GetLenXZ(d) < 1.0f && pBall->GetPos().y > 3.0f) {
                break;
            }
            if (!MethdBKRunTest(pWork->m_Pos, 0.3f, 1.0f, 0)) {
                break;
            }
            pWork->m_nStep++;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGetHatenaPanel()
{
    EntryAct(ACT_GET_HATENA_PANEL);
}

// Runs to the nearest "?" panel in reach and waits on it.
void CComDrvBasketGame::ActLoopGetHatenaPanel()
{
    PANEL_WORK *pWork = static_cast<PANEL_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            unsigned long nPanel = CComMan::QPanel_GetNear(m_MyPos, 1);
            if (nPanel == 0) {
                ReturnAct(0);
                break;
            }
            pWork->m_Pos = CComMan::QPanel_GetPos(nPanel);
            float fArea = GetSearchHatenaPanelArea();
            if (CComManBasket::GetDistXZ(pWork->m_Pos, m_MyPos) > fArea) {
                ReturnAct(0);
                break;
            }
            pWork->m_nPanel = nPanel;
            pWork->m_nStep++;
        }
        case 1: {
            if (!CComManBasket::IsQPanelDisable(pWork->m_nPanel)) {
                pWork->m_Pos = CComMan::QPanel_GetPos(pWork->m_nPanel);
                if (!MethdBKRunTest(pWork->m_Pos, 0.3f, 1.0f, 0)) {
                    break;
                }
            }
            float fLv = GetComLvF();
            ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0xA4D, fLv), CComMan::GetDatI(0xA55, fLv)));
            pWork->m_nStep++;
            break;
        }
        case 2:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoRunRand()
{
    EntryAct(ACT_GO_RUN_RAND);
}

// Runs a random distance in a random direction, then looks at the ball
// (unless it holds it) and waits.
void CComDrvBasketGame::ActLoopGoRunRand()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            float fRot = bcs::DEG_TO_RAD(RandMaxI(360));
            float fDist = CComManBasket::GetRandValueF(CComMan::GetDatF(0x848, fLv), CComMan::GetDatF(0x850, fLv));
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, fRot);
            pWork->m_Pos.x = dir.x * fDist + m_MyPos.x;
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = dir.z * fDist + m_MyPos.z;
            pWork->m_Pos = World2Local(pWork->m_Pos, -1);
            NormalizeCanMovePos(pWork->m_Pos);
            if (CGame::g_GameWork.GetStage() == 7) {
                CComManBasket::NormalizeCanMovePosToBossPackun(pWork->m_Pos, m_pChr);
            }
            pWork->m_nStep++;
        }
        case 1:
            if (!MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                break;
            }
            if (m_pChr->IsHoldBall()) {
                pWork->m_nStep = 3;
                break;
            }
            pWork->m_nStep++;
        case 2:
            ActCallLookBall();
            pWork->m_nStep++;
            break;
        case 3: {
            long nMin = CComMan::GetDatF(0x858, fLv);
            long nMax = CComMan::GetDatF(0x860, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

// Runs to rPos (local); gives up after fMaxFrame frames.
void CComDrvBasketGame::ActCallGoRun(const bcs::FVEC3 &rPos, float fMinDist, float fMaxFrame, unsigned long nFlag)
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(EntryAct(ACT_GO_RUN));
    pWork->m_nFlag = nFlag;
    pWork->m_fMinDist = fMinDist;
    pWork->m_fMaxFrame = fMaxFrame;
    pWork->m_Pos = rPos;
    m_fUnk1B8 = m_pChr->GetRotY_ToPosXZ(rPos);
}

void CComDrvBasketGame::ActLoopGoRun()
{
    RUN_WORK *pWork = static_cast<RUN_WORK *>(GetCurrentActWork());
    if (MethdBKRunTest(pWork->m_Pos, pWork->m_fMinDist, 1.0f, (pWork->m_nFlag >> 1) & 1)) {
        ReturnAct(1);
    } else if (pWork->m_fActTime >= pWork->m_fMaxFrame) {
        ReturnAct(0);
    }
}

} // namespace msw

namespace {
    // (names guessed) The work of the passes. The pass itself runs its own
    // steps in m_nPassStep, so that ActLoopShootCancelPass and
    // ActLoopCancelPass can run their steps in m_nStep first.
    struct PASS_WORK : msw::CComDrv::ACT_WORK {
        long m_nPassStep;               // 0x1C, 1 when the receiver is given
        float m_fTime;                  // 0x20, frames since the shot feint
        float m_fWait;                  // 0x24, frames to wait before the pass
        const msw::CChrCore *m_pTarget; // 0x28, the receiver
    };

    // (names guessed) The work of ActLoopJumpShoot.
    struct SHOOT_WORK : msw::CComDrv::ACT_WORK {
        long m_nUnk1C;          // 0x1C, -1 from ActCallJumpShoot
        unsigned long m_nFlag;  // 0x20, bit 0: the slower release was drawn
        int m_bUnk24;           // 0x24, drawn for all-computer teams
        unsigned char m_Unk28[0x2C - 0x28];
        float m_fRelease;       // 0x2C, when to release, as a part of 1
    };
}

namespace msw {

void CComDrvBasketGame::ActCallPass()
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(EntryAct(ACT_PASS));
    pWork->m_nPassStep = 0;
    pWork->m_pTarget = 0;
}

void CComDrvBasketGame::ActCallPass(const CChrCore *pChr)
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(EntryAct(ACT_PASS));
    pWork->m_nPassStep = pChr != 0;
    pWork->m_pTarget = pChr;
}

// Picks the best placed team mate (free of markers, near a "?" panel and
// near the goal), may feint a jump shot, then passes to it.
void CComDrvBasketGame::ActLoopPass()
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nPassStep) {
        case 0: {
            MEMBER_INFO info;
            long nNum = CComManBasketGame::GetMemBer(m_nMyTeam, info, 3, m_pChr);
            if (nNum == 0) {
                ReturnAct(0);
                break;
            }
            for (long i = 0; i < nNum; i++) {
                const CChrCore *pChr = info.m_apChr[i];
                {
                    static const long nNoMarkPoint = 100;
                    CChrCore *pNear = CComManBasket::GetNearChrCore(CComMan::GetChrPos(pChr), 1 - m_nMyTeam, -1, -1, 1);
                    if (pNear != 0 && CComManBasket::GetDistXZ(pNear, pChr) <= 3.0f) {
                        info.m_aScore[i] += nNoMarkPoint;
                    }
                }
                {
                    static const long nPanelPoint = 100;
                    float fRate = 1.0f - CComManBasket::GetNearQPanelDistXZ(pChr) / 5.0f;
                    if (fRate > 1.0f) {
                        fRate = 1.0f;
                    } else if (fRate < 0.0f) {
                        fRate = 0.0f;
                    }
                    info.m_aScore[i] += nPanelPoint * fRate;
                }
                {
                    static const long nNearGoalPoint = 100;
                    float fRate = 1.0f - CComManBasket::GetDistXZ(m_TargetGoalPos, CComMan::GetChrPos(pChr)) / 10.0f;
                    if (fRate > 1.0f) {
                        fRate = 1.0f;
                    } else if (fRate < 0.0f) {
                        fRate = 0.0f;
                    }
                    info.m_aScore[i] += nNearGoalPoint * fRate;
                }
                if (pChr->GetComLevelWork() == 1) {
                    info.m_aScore[i] += 50;
                }
                if (pChr->GetState() == 6) {
                    info.m_aScore[i] -= 100;
                }
            }
            long nBest = -9999;
            for (long i = 0; i < nNum; i++) {
                if (info.m_aScore[i] > nBest) {
                    nBest = info.m_aScore[i];
                    pWork->m_pTarget = info.m_apChr[i];
                }
            }
            pWork->m_nPassStep++;
        }
        case 1:
            if (pWork->m_pTarget == 0) {
                ReturnAct(0);
                break;
            }
            if (m_pChr->GetState() != 0x22) {
                const float &fLv = GetComLvF();
                long nRate = CComMan::GetDatF(0xBC7, fLv);
                if (LotRate() < nRate) {
                    float fLen = CComMan::GetDatF(0xBD7, fLv);
                    float fWidth = CComMan::GetDatF(0xBCF, fLv) * 0.5f;
                    bcs::FVEC3 targetPos = CComMan::GetChrPos(pWork->m_pTarget);
                    if (!CComMan::IsPointInFaceXZ(targetPos, m_MyPos, targetPos, fWidth, fLen) &&
                        IsExistChrToTarget(targetPos, fWidth, fLen, GetEmTeamEnableBit()) &&
                        MethdJumpShoot(GetJumpShootJumpDir())) {
                        long nMin = CComMan::GetDatF(0xBDF, fLv);
                        long nMax = CComMan::GetDatF(0xBE7, fLv);
                        pWork->m_fWait = CComManBasket::GetRandValueI(nMin, nMax);
                    }
                }
            }
            pWork->m_nPassStep++;
        case 2:
            if (!m_pChr->IsHoldBall()) {
                ReturnAct(0);
                break;
            }
            pWork->m_fTime += m_pChr->GetFrameStep();
            if (pWork->m_fTime < pWork->m_fWait) {
                break;
            }
            if (!MethdPass(const_cast<CChrCore *>(pWork->m_pTarget))) {
                ReturnAct(0);
                break;
            }
            pWork->m_nPassStep++;
        default:
            switch (m_pChr->GetState()) {
                case 0x15:
                case 0x16:
                case 0x22:
                    break;
                default:
                    ReturnAct(1);
                    break;
            }
            break;
    }
}

void CComDrvBasketGame::ActCallPassGhost()
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(EntryAct(ACT_PASS_GHOST));
    pWork->m_nPassStep = 0;
}

// Passes to the team's ghost when it can catch.
void CComDrvBasketGame::ActLoopPassGhost()
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nPassStep) {
        case 0:
            pWork->m_nPassStep++;
        case 1: {
            if (!m_pChr->IsHoldBall()) {
                ReturnAct(0);
                break;
            }
            CNpcGhostBasketBase *pGhost = static_cast<CNpcGhostBasketBase *>(
                static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(m_pChr->GetTeamNo()));
            if (pGhost == 0) {
                ReturnAct(0);
                break;
            }
            if (!pGhost->CanbePassCatch()) {
                ReturnAct(0);
                break;
            }
            if (!MethdPassBK(m_pChr->GetRotY_ToPosXZ(pGhost->GetPos()), 0)) {
                ReturnAct(0);
                break;
            }
            pWork->m_nPassStep++;
        }
        default:
            switch (m_pChr->GetState()) {
                case 0x15:
                case 0x16:
                    break;
                default:
                    ReturnAct(1);
                    break;
            }
            break;
    }
}

void CComDrvBasketGame::ActCallShootCancelPass(const CChrCore *pChr)
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(EntryAct(ACT_SHOOT_CANCEL_PASS));
    pWork->m_nStep = 0;
    pWork->m_pTarget = pChr;
    pWork->m_nPassStep = pChr != 0;
}

// A jump shot broken off into a pass.
void CComDrvBasketGame::ActLoopShootCancelPass()
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        if (!MethdJumpShoot(GetJumpShootJumpDir())) {
            ReturnAct(0);
            return;
        }
        float fLv = GetComLvF();
        long nMin = CComMan::GetDatF(0xBDF, fLv);
        long nMax = CComMan::GetDatF(0xBE7, fLv);
        pWork->m_fWait = CComManBasket::GetRandValueI(nMin, nMax);
        pWork->m_nStep++;
    } else {
        ActLoopPass();
    }
}

void CComDrvBasketGame::ActCallCancelPass(const CChrCore *pChr)
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(EntryAct(ACT_CANCEL_PASS));
    pWork->m_pTarget = pChr;
    pWork->m_nPassStep = pChr != 0;
}

// The jump shot already running is broken off into a pass once it is far
// enough into the jump.
void CComDrvBasketGame::ActLoopCancelPass()
{
    PASS_WORK *pWork = static_cast<PASS_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fWait = CComMan::GetDatF(0xBEF, fLv);
            pWork->m_nStep++;
        case 1: {
            if (static_cast<CChrCoreBasket *>(m_pChr)->GetJumpShootLocalFrame() < pWork->m_fWait) {
                break;
            }
            long nMin = CComMan::GetDatI(0xBF7, fLv);
            long nMax = CComMan::GetDatI(0xBFF, fLv);
            pWork->m_fWait = 0.0f;
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        default:
            ActLoopPass();
            break;
    }
}

// Signs for a pass, waits half a second.
void CComDrvBasketGame::ActLoopPassSign()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (MethdPassSign()) {
                pWork->m_nStep++;
            } else {
                ReturnAct(0);
                return;
            }
        case 1:
            ActCallWait(30.0f);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            return;
    }
    ReturnAct(0);
}

void CComDrvBasketGame::ActCallJumpShoot()
{
    SHOOT_WORK *pWork = static_cast<SHOOT_WORK *>(EntryAct(ACT_JUMP_SHOOT));
    pWork->m_nUnk1C = -1;
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopReboundDunk.
    struct REBOUND_DUNK_WORK : msw::CComDrv::ACT_WORK {
        long m_nWait;           // 0x1C
        bcs::FVEC3 m_Pos;       // 0x20
    };
}

namespace msw {

// Waits a little, draws when to release and shoots; may follow up with a
// rebound dunk.
void CComDrvBasketGame::ActLoopJumpShoot()
{
    SHOOT_WORK *pWork = static_cast<SHOOT_WORK *>(GetCurrentActWork());
    float fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            pWork->m_nStep++;
            long nMin = CComMan::GetDatI(0xB57, fLv);
            long nMax = CComMan::GetDatI(0xB5F, fLv);
            if (nMin > 0 && nMax > 0) {
                ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
                break;
            }
        }
        case 1: {
            long nRate = CComMan::GetDatI(0xB67, fLv);
            pWork->m_fRelease = 100.0f;
            if (LotRate() < nRate) {
                pWork->m_fRelease = CComManBasket::GetRandValueI(CComMan::GetDatI(0xB6F, fLv), CComMan::GetDatI(0xB77, fLv));
            } else {
                pWork->m_nFlag |= 1;
                pWork->m_fRelease = CComManBasket::GetRandValueI(CComMan::GetDatI(0xB7F, fLv), CComMan::GetDatI(0xB87, fLv));
            }
            pWork->m_fRelease /= 100.0f;
            pWork->m_bUnk24 = 0;
            if (IsAllComTeam()) {
                long nBase = CComMan::GetDatI(0xD6, (long)m_pChr->GetChrTypeID());
                long nId = 0xF3;
                if (IsAllComTeam()) {
                    nId = 0xEB;
                }
                long nRate2 = nBase * CComMan::GetDatF(nId, GetComLvF());
                pWork->m_bUnk24 = LotRate() < nRate2;
            }
            MethdJumpShoot(GetJumpShootJumpDir());
            pWork->m_nStep++;
        }
        case 2: {
            if (m_pChr->GetState() == 0x22) {
                break;
            }
            pWork->m_nStep++;
            long nRate = CComMan::GetDatI(0x31A, GetComLvF());
            if (LotRate() < nRate) {
                EntryAct(ACT_GO_REBOUND_DUNK);
                break;
            }
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallDunk()
{
    EntryAct(ACT_DUNK);
}

// Waits for the dunk to end; may lay up instead.
void CComDrvBasketGame::ActLoopDunk()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    long nState = m_pChr->GetState();
    if (pWork->m_nStep == 0) {
        if (nState != 0x28) {
            ReturnAct(0);
            return;
        }
        pWork->m_nStep++;
        long nRate = CComMan::GetDatI(0xC1, GetChrTypeID());
        if (LotRate() < nRate) {
            EntryAct(ACT_LAY_UP);
            return;
        }
    }
    if (nState != 0x28) {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallGoReboundDunk()
{
    EntryAct(ACT_GO_REBOUND_DUNK);
}

// Runs under the goal for a rebound dunk while a shot is in the air.
void CComDrvBasketGame::ActLoopGoReboundDunk()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            if (In3PointArea()) {
                pWork->m_nStep = 2;
                break;
            }
            long nArg = CComMan::GetDatI(0xDA7, fLv);
            Get3PointAreaRandPos(pWork->m_Pos, CComMan::GetDatF(0xD97, fLv), CComMan::GetDatF(0xD9F, fLv), nArg);
            pWork->m_nStep++;
        }
        case 1:
        case 2:
            if (!m_pChr->CanbeRebound()) {
                CBall *pBall = CBallMan::GetBallPtr(0);
                if (pBall != 0 && !pBall->IsShootMode()) {
                    ReturnAct(0);
                    break;
                }
            }
            pWork->m_Pos = m_TargetGoalPos;
            MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// A rebound dunk after nWait frames (none when not above 0).
void CComDrvBasketGame::ActCallReboundDunk(long nWait)
{
    REBOUND_DUNK_WORK *pWork = static_cast<REBOUND_DUNK_WORK *>(EntryAct(ACT_REBOUND_DUNK));
    pWork->m_nStep = nWait <= 0;
    pWork->m_nWait = nWait;
}

void CComDrvBasketGame::ActLoopReboundDunk()
{
    REBOUND_DUNK_WORK *pWork = static_cast<REBOUND_DUNK_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldItem() || m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }
    switch (pWork->m_nStep) {
        case 0:
            ActCallWait(pWork->m_nWait);
            pWork->m_nStep++;
            break;
        case 1: {
            static const float fDist = 2.0f;
            bcs::FVEC3 diff;
            bcs::FVEC3Sub(&diff, &m_TargetGoalPos, &m_MyPos);
            pWork->m_Pos = diff;
            bcs::FVEC3 dir = pWork->m_Pos;
            bcs::BCS_Vec_Normalize(pWork->m_Pos, pWork->m_Pos);
            pWork->m_Pos.x = fDist * pWork->m_Pos.x + m_MyPos.x;
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = fDist * pWork->m_Pos.z + m_MyPos.z;
            pWork->m_nStep++;
        }
        case 2:
            if (!MethdReboundDunk()) {
                if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                    ReturnAct(0);
                }
                break;
            }
            pWork->m_nStep++;
        default:
            if (m_pChr->GetState() != 0x24) {
                ReturnAct(1);
            }
            break;
    }
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopAlleyoop.
    struct ALLEYOOP_WORK : msw::CComDrv::ACT_WORK {
        int m_bDone;            // 0x1C
        float m_fTimer;         // 0x20
        bcs::FVEC3 m_Pos;       // 0x24
    };

    // (names guessed) The work of ActLoopAssistPlay.
    struct ASSIST_WORK : msw::CComDrv::ACT_WORK {
        int m_bSide;            // 0x1C, 0: under the goal, 1: to the side
        bcs::FVEC3 m_Pos;       // 0x20
    };
}

namespace msw {

void CComDrvBasketGame::ActCallAlleyoop()
{
    EntryAct(ACT_ALLEYOOP);
}

// Runs to the goal until the alley-oop can start, then waits for it.
void CComDrvBasketGame::ActLoopAlleyoop()
{
    ALLEYOOP_WORK *pWork = static_cast<ALLEYOOP_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldItem()) {
        ReturnAct(0);
        return;
    }
    switch (pWork->m_nStep) {
        case 0:
            if (!MethdAlleyoop()) {
                pWork->m_Pos = m_TargetGoalPos;
                if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                    ReturnAct(0);
                }
                break;
            }
            if (static_cast<CChrCoreBasket *>(m_pChr)->m_nBasketFlag & 0x800) {
                pWork->m_nStep = 999;
                break;
            }
            pWork->m_fTimer = 20.0f;
            pWork->m_nStep++;
        case 1:
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer <= 0.0f) {
                pWork->m_bDone = 1;
                pWork->m_nStep++;
            }
        default:
            if (m_pChr->GetState() != 0x26) {
                ReturnAct(1);
            }
            break;
    }
}

// Dashes about near where it stands.
void CComDrvBasketGame::ActLoopStickDash()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fTimer = 10.0f;
            pWork->m_Pos = m_MyPos;
            pWork->m_Pos.x += RandRangeF(-2.0f, 2.0f);
            pWork->m_Pos.z += RandRangeF(-2.0f, 2.0f);
            pWork->m_nStep++;
        case 1:
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer <= 0.0f) {
                pWork->m_nStep++;
            }
            break;
        case 2:
            if (!MethdDash()) {
                ReturnAct(0);
                return;
            }
            pWork->m_nStep++;
            break;
        case 3:
            if (m_pChr->GetState() != 0x34) {
                ReturnAct(1);
                return;
            }
            break;
    }
    if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
        pWork->m_Pos = m_MyPos;
        pWork->m_Pos.x += RandRangeF(-2.0f, 2.0f);
        pWork->m_Pos.z += RandRangeF(-2.0f, 2.0f);
    }
}

// Runs under the goal for an alley-oop when in the three-point area;
// otherwise signs for a pass and runs to the side.
void CComDrvBasketGame::ActCallAssistPlay()
{
    ASSIST_WORK *pWork = static_cast<ASSIST_WORK *>(EntryAct(ACT_ASSIST_PLAY));
    if (In3PointArea()) {
        pWork->m_bSide = 0;
        pWork->m_Pos = m_TargetGoalPos;
    } else {
        static const float fRate = 0.7f;
        pWork->m_bSide = 1;
        float fLeft = CComMan::GetStageCourtLeft(0);
        float fRight = CComMan::GetStageCourtRight(0);
        float fX = m_MyPos.x > 0.0f ? fRate * fLeft : fRate * fRight;
        pWork->m_Pos = m_MyPos;
        pWork->m_Pos.x = fX;
        MethdPassSign();
    }
}

void CComDrvBasketGame::ActLoopAssistPlay()
{
    ASSIST_WORK *pWork = static_cast<ASSIST_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }
    if (pWork->m_bSide == 0 && MethdAlleyoop()) {
        ReturnAct(1);
        return;
    }
    if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallBlockade()
{
    EntryAct(ACT_BLOCKADE);
}

// Draws one of the moves that shake off a marker.
void CComDrvBasketGame::ActLoopBlockade()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            long aRate[6] = {
                CComMan::GetDatI(0x830, fLv),
                CComMan::GetDatI(0x818, fLv),
                CComMan::GetDatI(0x820, fLv),
                CComMan::GetDatI(0x810, fLv),
                CComMan::GetDatI(0x828, fLv),
                CComMan::GetDatI(0x808, fLv),
            };
            switch (LotProbability(aRate, 6)) {
                case 0:
                    ActCallWait(CComManBasket::GetRandValueI(CComMan::GetDatI(0x838, fLv), CComMan::GetDatI(0x840, fLv)));
                    break;
                case 1:
                    EntryAct(ACT_GO_BACK);
                    break;
                case 2:
                    EntryAct(ACT_GO_ZIGUZAGU);
                    break;
                case 3:
                    EntryAct(ACT_GO_SIDE);
                    break;
                case 4:
                    EntryAct(ACT_SPIN_MOVE);
                    break;
                case 5:
                    EntryAct(ACT_GO_RUN_RAND);
                    break;
            }
            pWork->m_nStep++;
            break;
        }
        case 1:
            ReturnAct(1);
            break;
    }
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopGoZiguzagu.
    struct ZIGZAG_WORK : msw::CComDrv::ACT_WORK {
        long m_nCount;          // 0x1C, turns made
        long m_nSide;           // 0x20, 0 or 1: the side of the last turn
        bcs::FVEC3 m_Pos;       // 0x24
    };

    // (names guessed) The work of ActLoopSpinMove.
    struct SPIN_WORK : msw::CComDrv::ACT_WORK {
        int m_bRight;           // 0x1C
    };
}

namespace msw {

// Steps back from the goal it attacks, a little to one side.
void CComDrvBasketGame::ActLoopGoBack()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            static const long nMaxRang = 15;
            float fMin = CComMan::GetDatF(0xA72, (long)m_pChr->GetChrTypeID());
            float fDist = CComManBasket::GetRandValueF(fMin, CComMan::GetDatF(0xA87, (long)m_pChr->GetChrTypeID()));
            long nRang = CComMan::GetDatI(0xA5D, (long)m_pChr->GetChrTypeID());
            if (nRang > nMaxRang) {
                nRang = nMaxRang;
            }
            float fRot = bcs::DEG_TO_RAD(RandRangeI(-nRang, nRang)) + m_pChr->GetRotY_FromPosXZ(m_pChr->GetTargetGoalPos());
            fRot = RoundRad(fRot);
            bcs::BCS_VecZ_RotY(pWork->m_Pos, 1.0f, fRot);
            pWork->m_Pos.x = pWork->m_Pos.x * fDist + m_MyPos.x;
            pWork->m_Pos.z = pWork->m_Pos.z * fDist + m_MyPos.z;
            pWork->m_nStep++;
        }
        case 1:
            if (!MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 0.5f, 0)) {
                break;
            }
            pWork->m_nStep++;
        default:
            ReturnAct(1);
            break;
    }
}

// Runs a zigzag towards the goal it attacks, turning away from the side
// where opponents stand.
void CComDrvBasketGame::ActLoopGoZiguzagu()
{
    ZIGZAG_WORK *pWork = static_cast<ZIGZAG_WORK *>(GetCurrentActWork());
    if (m_pChr != 0 && !m_pChr->IsBattle()) {
        ReturnAct(0);
        return;
    }
    long nType = m_pChr->GetChrTypeID();
    switch (pWork->m_nStep) {
        case 0: {
            float fDist = CComManBasket::GetRandValueF(CComMan::GetDatF(0xAC6, nType), CComMan::GetDatF(0xADB, nType));
            long aRate[2] = {0, 0};
            float fGoalRot = m_pChr->GetRotY_ToPosXZ(m_pChr->GetTargetGoalPos());
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                const CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() != m_nMyTeam &&
                    CComManBasket::GetDistXZ(pChr, m_pChr) <= fDist) {
                    float fRot = RoundRadPI(m_pChr->GetRotY_ToPosXZ(pChr->GetPos()) - fGoalRot);
                    long nSide = !(fRot < 0.0f);
                    aRate[nSide] -= CComMan::GetDatI(0xAF0, nType);
                }
            }
            if (pWork->m_nCount > 0) {
                aRate[pWork->m_nSide] -= CComMan::GetDatI(0xB05, nType);
            }
            if (aRate[0] == 0 && aRate[1] == 0) {
                aRate[1] = 50;
                aRate[0] = 50;
            }
            pWork->m_nSide = 1 - LotProbability(aRate, 2);
            float fDeg = CComManBasket::GetRandValueI(CComMan::GetDatI(0xA9C, nType), CComMan::GetDatI(0xAB1, nType));
            if (fDeg > 75.0f) {
                fDeg = 75.0f;
            }
            if (RandMaxI(1)) {
                fDeg = -fDeg;
            }
            float fRot = RoundRad(fGoalRot + bcs::DEG_TO_RAD(fDeg));
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, fRot);
            pWork->m_Pos = m_MyPos;
            pWork->m_Pos.x = dir.x * fDist + pWork->m_Pos.x;
            pWork->m_Pos.z = dir.z * fDist + pWork->m_Pos.z;
            pWork->m_nStep++;
        }
        case 1:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                long nRate = CComMan::GetDatI(0xB1A, nType);
                if (LotRate() < nRate) {
                    pWork->m_nCount++;
                    pWork->m_nStep = 0;
                } else {
                    ReturnAct(1);
                }
            }
            break;
    }
}

// Runs to a random place at the side of the court and waits.
void CComDrvBasketGame::ActLoopGoSide()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    float fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            const bcs::FVEC3 &rMyPos = m_pChr->GetPos();
            float fWidth = CComMan::GetDatF(0x870, fLv);
            float fDepth = CComMan::GetDatF(0x868, fLv);
            CStageMan *pStage = CStageMan::GetInstance();
            if (rMyPos.x > 0.0f) {
                float fLeft = pStage->GetCourtLeft();
                pWork->m_Pos.x = CComManBasket::GetRandValueF(fLeft, fLeft + fWidth);
            } else {
                float fRight = pStage->GetCourtRight();
                pWork->m_Pos.x = CComManBasket::GetRandValueF(fRight - fWidth, fRight);
            }
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = rMyPos.z + CComManBasket::GetRandValueF(-fDepth, fDepth);
            pWork->m_Pos.z = pWork->m_Pos.z > pStage->GetCourtBack()   ? pStage->GetCourtBack()
                             : pWork->m_Pos.z < pStage->GetCourtFront() ? pStage->GetCourtFront()
                                                                         : pWork->m_Pos.z;
            pWork->m_Pos = CComMan::World2Local(pWork->m_Pos, -1);
            pWork->m_nStep++;
        }
        case 1: {
            if (!MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                break;
            }
            pWork->m_nStep++;
            long nMin = CComMan::GetDatI(0x878, fLv);
            long nMax = CComMan::GetDatI(0x880, fLv);
            if (nMin > 0 || nMax > 0) {
                ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            }
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallSpinMove()
{
    EntryAct(ACT_SPIN_MOVE);
}

// A spin move to the side the goal lies on.
void CComDrvBasketGame::ActLoopSpinMove()
{
    SPIN_WORK *pWork = static_cast<SPIN_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        float fRotY = m_pChr->GetRotY();
        float fRot = RoundRadPI(m_pChr->GetRotY_ToPosXZ(m_pChr->GetTargetGoalPos()) - fRotY);
        pWork->m_bRight = fRot > 0.0f;
        if (!MethdSpinMove(pWork->m_bRight)) {
            ReturnAct(0);
            return;
        }
        pWork->m_nStep++;
    }
    if (m_pChr->GetState() != 0x1E) {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallAirTrick()
{
    EntryAct(ACT_AIR_TRICK);
}

void CComDrvBasketGame::ActLoopAirTrick()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (!m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }
    if (pWork->m_nStep == 0) {
        if (!MethdAirTrick()) {
            ReturnAct(0);
            return;
        }
        pWork->m_nStep++;
    }
    if (m_pChr->GetState() != 0x27) {
        ReturnAct(1);
    }
}

// Runs to the place ActCallGoTargetGoal set, looks at the ball and waits
// a second.
void CComDrvBasketGame::ActLoopGoTargetGoal()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ActCallLookBall();
                pWork->m_nStep++;
            }
            break;
        case 1:
            ActCallWait(60.0f);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallBallKeep()
{
    EntryAct(ACT_BALL_KEEP);
}

// Keeps the ball: draws one of the moves that keep it (run about, escape,
// the free area, the dunk area, an item or a wait).
void CComDrvBasketGame::ActLoopBallKeep()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        float fLv = GetComLvF();
        long aRate[6] = {0, 0, 0, 0, 0, 0};
        aRate[0] = CComMan::GetDatI(0x7E0, fLv);
        aRate[1] = CComMan::GetDatI(0x7E8, fLv);
        aRate[2] = CComMan::GetDatI(0x7C8, fLv);
        aRate[3] = CComMan::GetDatI(0x7D0, fLv);
        aRate[5] = CComMan::GetDatI(0x7F0, fLv);
        if (CGame::g_GameWork.GetStage() == 2) {
            if (CanbeActGetGShell()) {
                aRate[4] = CComMan::GetDatI(0x7D8, fLv);
            }
        } else if (CanbeActGetHatenaPanel()) {
            aRate[4] = CComMan::GetDatI(0x7D8, fLv) * GetHatenaPanelStageRate_();
        }
        switch (LotProbability(aRate, 6)) {
            case 0:
                EntryAct(ACT_GO_RUN_RAND);
                break;
            case 1:
                EntryAct(ACT_GO_SIDE);
                break;
            case 2:
                EntryAct(ACT_GO_FREE_AREA);
                break;
            case 3:
                EntryAct(ACT_GO_DUNK_AREA);
                break;
            case 4:
                if (CGame::g_GameWork.GetStage() == 2) {
                    EntryAct(ACT_GET_G_SHELL);
                } else {
                    EntryAct(ACT_GET_HATENA_PANEL);
                }
                break;
            case 5: {
                long nMin = CComMan::GetDatI(0x7F8, fLv);
                long nMax = CComMan::GetDatI(0x800, fLv);
                ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
                break;
            }
        }
        pWork->m_nStep++;
    } else {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallGoDunkArea()
{
    EntryAct(ACT_GO_DUNK_AREA);
}

// Runs to a random place of the three-point area.
void CComDrvBasketGame::ActLoopGoDunkArea()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            float fLv = GetComLvF();
            long nArg = CComMan::GetDatI(0x9E0, fLv);
            float fMin = CComMan::GetDatF(0x9D0, fLv);
            float fMax = CComMan::GetDatF(0x9D8, fLv);
            Get3PointAreaRandPos(pWork->m_Pos, fMin, fMax, nArg);
            pWork->m_nStep++;
        }
        case 1:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ReturnAct(1);
            }
            break;
    }
}

// Runs into the three-point area until near enough to the goal, then
// dunks or lays up.
void CComDrvBasketGame::ActLoopGoDunk()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            float fLv = GetComLvF();
            float fMin = CComMan::GetDatF(0x9E8, fLv);
            float fMax = CComMan::GetDatF(0x9F0, fLv);
            pWork->m_fTimer = RandRangeF(fMin, fMax);
            pWork->m_Pos = CComManBasket::Get3PointAreaRundPos(m_nUnk8C);
            pWork->m_nStep++;
        }
        case 1:
            if (CComManBasket::GetDistXZ(m_TargetGoalPos, m_MyPos) > pWork->m_fTimer &&
                !MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                break;
            }
            {
                long nRate = CComMan::GetDatI(0xC1, GetChrTypeID());
                if (LotRate() < nRate) {
                    EntryAct(ACT_LAY_UP);
                } else {
                    EntryAct(ACT_DUNK);
                }
            }
            pWork->m_nStep++;
            break;
        case 2:
            ReturnAct(1);
            break;
    }
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopLayUp and ActLoopCancelLayUp.
    struct LAYUP_WORK : msw::CComDrv::ACT_WORK {
        int m_bRelease;         // 0x1C, set when the lay-up is to be let go
        float m_fFrame;         // 0x20, the dunk frame to break off at
        float m_fTime;          // 0x24
        float m_fWait;          // 0x28, frames before the release
    };
}

namespace msw {

// Waits for the lay-up out, releasing it after a drawn time.
void CComDrvBasketGame::ActLoopLayUp()
{
    LAYUP_WORK *pWork = static_cast<LAYUP_WORK *>(GetCurrentActWork());
    long nState = m_pChr->GetState();
    if (pWork->m_nStep == 0) {
        if (nState != 0x28) {
            ReturnAct(0);
            return;
        }
        long nMin = CComMan::GetDatI(0xC07, GetComLvF());
        long nMax = CComMan::GetDatI(0xC0F, GetComLvF());
        pWork->m_fWait = CComManBasket::GetRandValueI(nMin, nMax);
        pWork->m_bRelease = 0;
        pWork->m_nStep++;
    }
    switch (m_pChr->GetState()) {
        case 0x28:
            pWork->m_fTime += m_pChr->GetFrameStep();
            if (pWork->m_fTime >= pWork->m_fWait) {
                pWork->m_bRelease = 1;
            }
            break;
        case 0x25:
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallCancelLayUp()
{
    EntryAct(ACT_CANCEL_LAY_UP);
}

// The dunk already running is broken off into a lay-up once far enough
// into it.
void CComDrvBasketGame::ActLoopCancelLayUp()
{
    LAYUP_WORK *pWork = static_cast<LAYUP_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_fFrame = CComMan::GetDatF(0xC17, fLv);
            pWork->m_nStep++;
        case 1:
            if (!m_pChr->CanbeLayUp()) {
                ReturnAct(0);
                return;
            }
            if (static_cast<CChrCoreBasket *>(m_pChr)->GetDunkLocalFrame() < pWork->m_fFrame) {
                break;
            }
            pWork->m_nStep++;
        case 2: {
            long nMin = CComMan::GetDatI(0xC1F, fLv);
            long nMax = CComMan::GetDatI(0xC27, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        default: {
            long nState = m_pChr->GetState();
            if (nState == 0x28 || nState == 0x25) {
                pWork->m_bRelease = 1;
                return;
            }
            ReturnAct(1);
            break;
        }
    }
}

void CComDrvBasketGame::ActCallItemShoot(long nArg)
{
    SHOOT_WORK *pWork = static_cast<SHOOT_WORK *>(EntryAct(ACT_ITEM_SHOOT));
    pWork->m_nUnk1C = nArg;
}

// Waits a little, draws when to release and shoots with the item.
void CComDrvBasketGame::ActLoopItemShoot()
{
    SHOOT_WORK *pWork = static_cast<SHOOT_WORK *>(GetCurrentActWork());
    float fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            pWork->m_nStep++;
            long nMin = CComMan::GetDatI(0xB8F, fLv);
            long nMax = CComMan::GetDatI(0xB97, fLv);
            if (nMin > 0 && nMax > 0) {
                ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
                break;
            }
        }
        case 1: {
            long nRate = CComMan::GetDatI(0xB9F, fLv);
            int bFast = LotRate() < nRate;
            pWork->m_fRelease = 100.0f;
            if (bFast) {
                long nMin = CComMan::GetDatI(0xBA7, fLv);
                long nMax = CComMan::GetDatI(0xBAF, fLv);
                pWork->m_fRelease = CComManBasket::GetRandValueI(nMin, nMax);
            } else {
                pWork->m_nFlag |= 1;
                long nMin = CComMan::GetDatI(0xBB7, fLv);
                long nMax = CComMan::GetDatI(0xBBF, fLv);
                pWork->m_fRelease = CComManBasket::GetRandValueI(nMin, nMax);
            }
            pWork->m_fRelease /= 100.0f;
            pWork->m_bUnk24 = 1;
            MethdJumpShoot(GetJumpShootJumpDir());
            pWork->m_nStep++;
            break;
        }
        default:
            if (m_pChr->GetState() != 0x22) {
                ReturnAct(1);
            }
            break;
    }
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopCombination.
    struct COMBO_WORK : msw::CComDrv::ACT_WORK {
        int m_bPartner;         // 0x1C, a team mate is in the combination state
        int m_bFar;             // 0x20, run to the goal while waiting
        float m_fTimer;         // 0x24
        bcs::FVEC3 m_Pos;       // 0x28
    };

    // (names guessed) The work of ActLoopGoEnemy.
    struct ENEMY_WORK : msw::CComDrv::ACT_WORK {
        float m_fTimer;                 // 0x1C
        const msw::CChrCore *m_pTarget; // 0x20
    };

    // (names guessed) The work of ActLoopGoBall.
    struct BALL_WORK : msw::CComDrv::ACT_WORK {
        float m_fTimer;         // 0x1C
        msw::CBall *m_pBall;    // 0x20
    };
}

namespace msw {

// The combination play: runs to the goal when far from it.
void CComDrvBasketGame::ActCallCombination()
{
    static const float fCheckDist = 3.0f;
    COMBO_WORK *pWork = static_cast<COMBO_WORK *>(EntryAct(ACT_COMBINATION));
    bcs::FVEC3 diff;
    const bcs::FVEC3 &rGoal = m_pChr->GetTargetGoalPos();
    bcs::FVEC3Sub(&diff, &m_pChr->GetPos(), &rGoal);
    bcs::FVEC3 d = diff;
    pWork->m_bFar = fCheckDist < bcs::BCS_Vec_GetLenXZ(d);
}

// With the ball, waits for a team mate to join the combination and passes
// to it (or shoots); otherwise passes to the nearest team mate when the
// time is up.
void CComDrvBasketGame::ActLoopCombination()
{
    if (!IsCombination()) {
        ReturnAct(1);
        return;
    }
    COMBO_WORK *pWork = static_cast<COMBO_WORK *>(GetCurrentActWork());
    float fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            if (!m_pChr->IsHoldBall()) {
                break;
            }
            if (!IsAllComTeam()) {
                break;
            }
            float fMin = CComMan::GetDatF(0xDE7, fLv);
            float fMax = CComMan::GetDatF(0xDEF, fLv);
            pWork->m_fTimer = CComManBasket::GetRandValueF(fMin, fMax);
            pWork->m_nStep++;
        }
        case 1:
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer <= 0.0f) {
                pWork->m_nStep = 2;
                break;
            }
            if (pWork->m_bPartner == 0) {
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && pChr != m_pChr && pChr->GetState() == 0x26 &&
                        pChr->GetTeamNo() == m_pChr->GetTeamNo()) {
                        pWork->m_bPartner = 1;
                        break;
                    }
                }
                if (pWork->m_bPartner != 0) {
                    long nRate = CComMan::GetDatF(0xDCF, fLv);
                    if (LotRate() < nRate) {
                        long nMin = CComMan::GetDatF(0xDD7, fLv);
                        long nMax = CComMan::GetDatF(0xDDF, fLv);
                        long nWait = CComManBasket::GetRandValueF(nMin, nMax);
                        ActCallWait(nWait);
                        pWork->m_nStep = 2;
                        break;
                    }
                }
            }
            if (CanbeActCallJumpShoot()) {
                long nRate = CComMan::GetDatF(0xDF7, fLv);
                if (LotRate() < nRate) {
                    ActCallJumpShoot();
                    pWork->m_nStep = 3;
                }
            }
            break;
        case 2: {
            const CChrCore *pTarget = 0;
            float fMinDist = 9999.9f;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                const CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && pChr != m_pChr && pChr->GetState() == 0x26 &&
                    pChr->GetTeamNo() == m_pChr->GetTeamNo()) {
                    bcs::FVEC3 diff;
                    const bcs::FVEC3 &rMyPos = m_pChr->GetPos();
                    bcs::FVEC3Sub(&diff, &pChr->GetPos(), &rMyPos);
                    bcs::FVEC3 d = diff;
                    float fDist = bcs::BCS_Vec_GetLenXZ(d);
                    if (fMinDist < fDist) {
                        continue;
                    }
                    fMinDist = fDist;
                    pTarget = pChr;
                }
            }
            if (pTarget == 0) {
                float fMinDist = 9999.9f;
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && pChr != m_pChr &&
                        pChr->GetTeamNo() == m_pChr->GetTeamNo()) {
                        bcs::FVEC3 diff;
                        const bcs::FVEC3 &rMyPos = m_pChr->GetPos();
                        bcs::FVEC3Sub(&diff, &pChr->GetPos(), &rMyPos);
                        bcs::FVEC3 d = diff;
                        float fDist = bcs::BCS_Vec_GetLenXZ(d);
                        if (fMinDist < fDist) {
                            continue;
                        }
                        fMinDist = fDist;
                        pTarget = pChr;
                    }
                }
            }
            ActCallPass(pTarget);
            break;
        }
    }
    if (pWork->m_bFar) {
        pWork->m_Pos = m_TargetGoalPos;
        MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
    }
}

void CComDrvBasketGame::ActCallGoEnemy()
{
    EntryAct(ACT_GO_ENEMY);
}

// Runs at the nearest opponent for a while.
void CComDrvBasketGame::ActLoopGoEnemy()
{
    ENEMY_WORK *pWork = static_cast<ENEMY_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            float fMinDist = 9999.9f;
            for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                const CChrCore *pChr = CChrMan::GetChrCore(i);
                if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() != m_nMyTeam &&
                    !(pChr->GetStatusFlag() & 0x20000) && pChr->GetState() != 6) {
                    bcs::FVEC3 diff;
                    const bcs::FVEC3 &rMyPos = m_pChr->GetPos();
                    bcs::FVEC3Sub(&diff, &pChr->GetPos(), &rMyPos);
                    bcs::FVEC3 d = diff;
                    float fDist = bcs::BCS_Vec_GetLenXZ(d);
                    if (fDist > fMinDist) {
                        continue;
                    }
                    fMinDist = fDist;
                    pWork->m_pTarget = pChr;
                }
            }
            if (pWork->m_pTarget == 0) {
                ReturnAct(0);
                return;
            }
            float fLv = GetComLvF();
            float fMin = CComMan::GetDatF(0xDFF, fLv);
            float fMax = CComMan::GetDatF(0xE07, fLv);
            pWork->m_fTimer = CComManBasket::GetRandValueF(fMin, fMax);
            pWork->m_nStep++;
        }
        case 1: {
            const CChrCore *pTarget = pWork->m_pTarget;
            if ((pTarget->GetStatusFlag() & 0x20000) || pTarget->GetState() == 6) {
                ReturnAct(1);
                return;
            }
            bcs::FVEC3 pos = CComMan::GetChrPos(pTarget);
            MethdBKRunTest(pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer > 0.0f) {
                break;
            }
            pWork->m_nStep++;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoBall()
{
    EntryAct(ACT_GO_BALL);
}

// Runs after the ball in play for a while, then waits.
void CComDrvBasketGame::ActLoopGoBall()
{
    BALL_WORK *pWork = static_cast<BALL_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall == 0) {
                ReturnAct(0);
                return;
            }
            if (!pBall->IsActive()) {
                ReturnAct(0);
                return;
            }
            pWork->m_pBall = pBall;
            float fMin = CComMan::GetDatF(0xE0F, fLv);
            float fMax = CComMan::GetDatF(0xE17, fLv);
            pWork->m_fTimer = CComManBasket::GetRandValueF(fMin, fMax);
            pWork->m_nStep++;
        }
        case 1: {
            if (!pWork->m_pBall->IsActive()) {
                ReturnAct(0);
                return;
            }
            if (m_pChr->IsHoldBall()) {
                ReturnAct(1);
                return;
            }
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer < 0.0f) {
                ReturnAct(0);
                return;
            }
            bcs::FVEC3 pos = CComMan::GetBallPos(pWork->m_pBall);
            if (MethdBKRunTest(pos, 0.3f, 1.0f, 0)) {
                float fMin = CComMan::GetDatF(0xE1F, fLv);
                float fMax = CComMan::GetDatF(0xE27, fLv);
                ActCallWait(CComManBasket::GetRandValueF(fMin, fMax));
                pWork->m_nStep++;
            }
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallRebound()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(EntryAct(ACT_REBOUND));
    pWork->m_fTimer = 0.0f;
}

void CComDrvBasketGame::ActCallRebound(float fArg)
{
    BK_WORK *pWork = static_cast<BK_WORK *>(EntryAct(ACT_REBOUND));
    pWork->m_fTimer = fArg;
}

// Runs under the ball; once it is low and near enough, waits a little
// and jumps for it.
void CComDrvBasketGame::ActLoopRebound()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            if (!m_pChr->CanbeRebound()) {
                ReturnAct(0);
                break;
            }
            CBall *pBall = CBallMan::GetBallPtr(0);
            pWork->m_Pos = CComMan::World2Local(pBall->GetPos(), -1);
            MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            if (pBall->GetPos().y <= CComMan::GetDatF(0xD77, fLv)) {
                break;
            }
            static const float fBaseDist = 3.0f;
            float fDist = CComMan::GetDatF(0xD7F, fLv);
            if (CComManBasket::GetDistXZ(pWork->m_Pos, m_MyPos) > fBaseDist + fDist) {
                break;
            }
            long nMin = CComMan::GetDatI(0xD87, fLv);
            long nMax = CComMan::GetDatI(0xD8F, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        case 1: {
            BK_WORK *pJump = static_cast<BK_WORK *>(EntryAct(ACT_JUMP2));
            pJump->m_fTimer = -1.0f;
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

// Cuts at the ball (or straight ahead when there is none in play) and
// waits for the cut to end.
void CComDrvBasketGame::ActLoopCut()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        float fRot = m_pChr->GetRotY();
        CBall *pBall = CBallMan::GetBallPtr(0);
        if (pBall != 0 && pBall->IsActive()) {
            fRot = m_pChr->GetRotY_ToPosXZ(pBall->GetPos());
        }
        MethdCut(fRot);
        pWork->m_nStep++;
    } else if (m_pChr->GetState() != 9) {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallMissCut()
{
    EntryAct(ACT_MISS_CUT);
}

// A cut that is meant to miss.
void CComDrvBasketGame::ActLoopMissCut()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        ACT_WORK *pCut = static_cast<ACT_WORK *>(EntryAct(ACT_CUT));
        pCut->m_fTime = 0.0f;
        pWork->m_nStep++;
    } else {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallJump()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(EntryAct(ACT_JUMP2));
    pWork->m_fTimer = -1.0f;
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopJump and ActLoopDefJump: the
    // direction to jump in (-1 for none) and the wait before it.
    struct JUMP_WORK : msw::CComDrv::ACT_WORK {
        float m_fDir;           // 0x1C
    };

    struct DEF_JUMP_WORK : msw::CComDrv::ACT_WORK {
        float m_fWait;          // 0x1C
        float m_fDir;           // 0x20
    };

    // (names guessed) The work of ActLoopDefAlleyoop.
    struct DEF_ALLEYOOP_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Pos;               // 0x1C
        float m_fDist;                  // 0x28
        const msw::CChrCore *m_pTarget; // 0x2C, the one going for the alley-oop
    };
}

namespace msw {

// A block jump; the trick flags are cleared when it ends.
void CComDrvBasketGame::ActLoopJump()
{
    JUMP_WORK *pWork = static_cast<JUMP_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            MethdJumpBlock(pWork->m_fDir);
            pWork->m_nStep++;
        case 1:
            if (m_pChr->GetState() == 0x30) {
                break;
            }
            m_nFlag17C &= ~0x1C;
            pWork->m_nStep++;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallDefJump(float fWait, float fDir)
{
    DEF_JUMP_WORK *pWork = static_cast<DEF_JUMP_WORK *>(EntryAct(ACT_DEF_JUMP));
    pWork->m_fWait = fWait;
    pWork->m_fDir = fDir;
}

// A defensive jump after a drawn wait.
void CComDrvBasketGame::ActCallDefJumpRandWait(float fDir)
{
    float fMin = CComMan::GetDatF(0xC57, GetComLvF());
    float fMax = CComMan::GetDatF(0xC5F, GetComLvF());
    ActCallDefJump(CComManBasket::GetRandValueF(fMin, fMax), fDir);
}

void CComDrvBasketGame::ActLoopDefJump()
{
    DEF_JUMP_WORK *pWork = static_cast<DEF_JUMP_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_nStep++;
            if (pWork->m_fWait > 0.0f) {
                ActCallWait(pWork->m_fWait);
                break;
            }
        case 1: {
            float fDir = pWork->m_fDir;
            JUMP_WORK *pJump = static_cast<JUMP_WORK *>(EntryAct(ACT_JUMP2));
            pJump->m_fDir = fDir;
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

// Waits for the jump cut to end.
void CComDrvBasketGame::ActLoopJumpCut()
{
    GetCurrentActWork();
    switch (m_pChr->GetState()) {
        case 0x30:
        case 0x31:
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// An upper cut; waits for it to end.
void CComDrvBasketGame::ActLoopJumpUpper()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        if (!MethdUpperCut()) {
            ReturnAct(0);
            return;
        }
        pWork->m_nStep++;
    }
    if (m_pChr->GetState() != 0xC) {
        ReturnAct(1);
    }
}

// Defends against pChr's alley-oop.
void CComDrvBasketGame::ActCallDefAlleyoop(const CChrCore *pChr)
{
    DEF_ALLEYOOP_WORK *pWork = static_cast<DEF_ALLEYOOP_WORK *>(EntryAct(ACT_DEF_ALLEYOOP));
    pWork->m_pTarget = pChr;
}

// Stands between the alley-oop's goal and the one going for it, and jumps
// when the ball comes near it.
void CComDrvBasketGame::ActLoopDefAlleyoop()
{
    DEF_ALLEYOOP_WORK *pWork = static_cast<DEF_ALLEYOOP_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            float fDist = CComMan::GetDatI(0xC6F, GetComLvF());
            bcs::FVEC3 dir;
            bcs::BCS_VecZ_RotY(dir, 1.0f, pWork->m_pTarget->GetRotY_FromGoal());
            dir.x *= fDist;
            dir.z *= fDist;
            pWork->m_Pos.x = dir.x + pWork->m_pTarget->GetTargetGoalPos().x;
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = dir.z + pWork->m_pTarget->GetTargetGoalPos().z;
            pWork->m_fDist = CComMan::GetDatI(0xC67, GetComLvF());
            pWork->m_nStep++;
        }
        case 1: {
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall == 0) {
                ReturnAct(0);
                return;
            }
            if (pWork->m_pTarget->GetState() != 0x26) {
                ReturnAct(0);
                return;
            }
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                m_pChr->RotateYRate_(m_pChr->GetRotY_ToPosXZ(pWork->m_pTarget->GetPos()), 0.2f);
            }
            bcs::FVEC3 diff;
            const bcs::FVEC3 &rTargetPos = pWork->m_pTarget->GetPos();
            bcs::FVEC3Sub(&diff, &pBall->GetPos(), &rTargetPos);
            bcs::FVEC3 d = diff;
            if (bcs::BCS_Vec_GetLen(d) > 1.5f + pWork->m_fDist) {
                break;
            }
            ActCallDefJumpRandWait(GetRotY_P1ToP2XZ(m_pChr, pWork->m_pTarget));
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallItemEscape()
{
    EntryAct(ACT_ITEM_ESCAPE);
}

// Does nothing (the escape from items is ActLoopGoEscape).
void CComDrvBasketGame::ActLoopItemEscape()
{
    ReturnAct(1);
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopGoEscape: the item to get away
    // from and where to run.
    struct ESCAPE_WORK : msw::CComDrv::ACT_WORK {
        unsigned long m_nItem;  // 0x1C, the item's task id
        bcs::FVEC3 m_Pos;       // 0x20
    };
}

namespace msw {

// Gets away from item nItem.
void CComDrvBasketGame::ActCallGoEscape(unsigned long nItem)
{
    ESCAPE_WORK *pWork = static_cast<ESCAPE_WORK *>(EntryAct(ACT_GO_ESCAPE));
    pWork->m_nItem = nItem;
}

// Runs aside from a flying item: at least 50 degrees off its course and
// not straight back along it; out of the blast of a bomb; and turned back
// when a wall is in the way.
void CComDrvBasketGame::ActLoopGoEscape()
{
    ESCAPE_WORK *pWork = static_cast<ESCAPE_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            CItem *pItem = static_cast<CItem *>(bcs::CTaskMan::GetTask(pWork->m_nItem));
            if (pItem == 0) {
                ReturnAct(0);
                return;
            }
            static const long nDegRange = 50;
            bcs::FVEC3 zero(0.0f, 0.0f, 0.0f);
            int bStop = 0;
            if (pItem->GetVelocity().x == zero.x && pItem->GetVelocity().y == zero.y &&
                pItem->GetVelocity().z == zero.z) {
                bStop = 1;
            }
            float fItemRot;
            if (bStop) {
                fItemRot = 0.0f;
            } else {
                fItemRot = CComManBasket::GetRotY_P1ToP2XZ(bcs::FVEC3(0.0f, 0.0f, 0.0f), pItem->GetVelocity());
            }
            float fRot = RoundRadPI(m_pChr->GetRotY() - fItemRot);
            float fAbs = fabsf(fRot);
            if (fAbs < bcs::DEG_TO_RAD(nDegRange)) {
                fRot = fRot > 0.0f ? bcs::DEG_TO_RAD(nDegRange) : -bcs::DEG_TO_RAD(nDegRange);
            } else if (fAbs > bcs::DEG_TO_RAD(180 - nDegRange)) {
                fRot = fRot > 0.0f ? bcs::DEG_TO_RAD(180 - nDegRange) : -bcs::DEG_TO_RAD(180 - nDegRange);
            }
            fRot = RoundRad(fRot + fItemRot);
            bcs::FVEC3 pos;
            bcs::BCS_VecZ_RotY(pos, 1.0f, fRot);
            float fDist = 2.0f + pItem->GetAtkCollRadius();
            pos.x = pos.x * fDist + m_pChr->GetPos().x;
            pos.z = pos.z * fDist + m_pChr->GetPos().z;
            bcs::FVEC3 target;
            if (pItem->GetItemType() == CItem::KIND_BOMB && pItem->GetThrowTargetPos(target)) {
                bcs::FVEC3 diff;
                bcs::FVEC3Sub(&diff, &pos, &target);
                bcs::FVEC3 d = diff;
                float fLen = bcs::BCS_Vec_GetLenXZ(d);
                float fBlast = 0.5f + pItem->GetExplosionCollSize();
                if (fLen < fBlast) {
                    bcs::FVEC3 xz(d.x, 0.0f, d.z);
                    bcs::BCS_Vec_Normalize(d, xz);
                    pos.x = d.x * fBlast + target.x;
                    pos.z = d.z * fBlast + target.z;
                }
            }
            bcs::FVEC3 hit;
            if (m_pChr->CheckMapHit(hit, m_MyWorldPos, pos)) {
                bcs::FVEC3Sub(&pos, &pos, &m_pChr->GetPos());
                pos.y = 0.0f;
                bcs::BCS_Vec_RotY(pos, pos, PI);
                bcs::FVEC3Add(&pos, &pos, &m_pChr->GetPos());
                pos.y = 0.0f;
            }
            pWork->m_Pos = CComMan::World2Local(pos, -1);
            pWork->m_nStep++;
        }
        case 1:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ReturnAct(1);
            }
            break;
        default:
            ReturnAct(0);
            break;
    }
}

// Runs to rPos (local), looks at the ball and waits a little.
void CComDrvBasketGame::ActCallGoEscape2(const bcs::FVEC3 &rPos)
{
    POS_WORK *pWork = static_cast<POS_WORK *>(EntryAct(ACT_GO_ESCAPE2));
    pWork->m_Pos = rPos;
}

void CComDrvBasketGame::ActLoopGoEscape2()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ActCallLookBall();
                pWork->m_nStep++;
            }
            break;
        case 1:
            ActCallWait(CComManBasket::GetRandValueI(5, 10));
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Throws the held item at rPos.
void CComDrvBasketGame::ActCallItemThrow(const bcs::FVEC3 &rPos)
{
    POS_WORK *pWork = static_cast<POS_WORK *>(EntryAct(ACT_ITEM_THROW));
    pWork->m_Pos = rPos;
}

void CComDrvBasketGame::ActLoopItemThrow()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        // The original passes a local it never sets, not the place
        // ActCallItemThrow stored (0x1C of the work); kept as it is.
        bcs::FVEC3 pos;
        if (!MethdItemShoot(pos)) {
            ReturnAct(0);
            return;
        }
        pWork->m_nStep++;
    }
    if (m_pChr->GetState() != 8) {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallDefGhost()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(EntryAct(ACT_DEF_GHOST));
    pWork->m_nStep = 0;
}

// Defends against the other team's ghost while it holds the ball: runs to
// the ball and, when near and in front of it, jumps for it or cuts.
void CComDrvBasketGame::ActLoopDefGhost()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            long nEmTeam = 1 - m_pChr->GetTeamNo();
            CNpcBase *pGhost = static_cast<CChrManBasket *>(CChrMan::GetInstance())->GetGhost_(nEmTeam);
            if (pGhost == 0) {
                ReturnAct(0);
                return;
            }
            if (!pGhost->IsHoldBall()) {
                ReturnAct(0);
                return;
            }
            const bcs::FVEC3 &rBallPos = pGhost->GetAttachBall()->GetPos();
            long nRate = CComMan::GetDatI(0x102B, fLv);
            if (LotRate() < nRate) {
                bcs::FVEC3 diff;
                bcs::FVEC3Sub(&diff, &rBallPos, &m_pChr->GetPos());
                bcs::FVEC3 d = diff;
                if (bcs::BCS_Vec_GetLenXZ(d) < 2.0f) {
                    float fGhostRot = pGhost->GetRot().y;
                    float fRot = RoundRadPI(m_pChr->GetRotY_FromPosXZ(rBallPos) - fGhostRot);
                    if (fabsf(fRot) <= bcs::DEG_TO_RAD(135.0f)) {
                        if (rBallPos.y > 2.8f) {
                            MethdJumpBlock(-1.0f);
                            pWork->m_nStep = 1;
                            return;
                        }
                        if (rBallPos.y > 2.2f) {
                            MethdJumpBlock(-1.0f);
                            pWork->m_nStep = 2;
                            return;
                        }
                        MethdCut(m_pChr->GetRotY_ToPosXZ(rBallPos));
                        pWork->m_nStep = 99;
                        return;
                    }
                }
            }
            bcs::FVEC3 pos = CComMan::World2Local(rBallPos, -1);
            if (MethdBKRunTest(pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ActCallLookBall();
                ActCallWait(10.0f);
            }
            break;
        }
        case 1:
            if (m_pChr->GetPos().y > 2.0f) {
                JUMP_WORK *pJump = static_cast<JUMP_WORK *>(EntryAct(ACT_JUMP_UPPER));
                pJump->m_fDir = 0.0f;
                pWork->m_nStep = 99;
            }
            break;
        case 2:
            if (m_pChr->GetPos().y > 2.0f) {
                EntryAct(ACT_JUMP_CUT);
                pWork->m_nStep = 99;
            }
            break;
        default: {
            long nState = m_pChr->GetState();
            if (nState != 0x31 && nState != 0xC && nState != 9) {
                pWork->m_nStep = 0;
            }
            break;
        }
    }
}

void CComDrvBasketGame::ActCallCutChance()
{
    EntryAct(ACT_CUT_CHANCE);
}

// Waits a drawn time, then cuts (or, when the draw fails, only waits).
void CComDrvBasketGame::ActLoopCutChance()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            const float &fLv = GetComLvF();
            long nRate = CComMan::GetDatI(0xCBF, fLv);
            long nMin;
            long nMax;
            if (LotRate() < nRate) {
                nMin = CComMan::GetDatI(0xCC7, fLv);
                nMax = CComMan::GetDatI(0xCCF, fLv);
                pWork->m_nStep++;
            } else {
                nMin = CComMan::GetDatI(0xCD7, fLv);
                nMax = CComMan::GetDatI(0xCDF, fLv);
                pWork->m_nStep = 2;
            }
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            break;
        }
        case 1: {
            ACT_WORK *pCut = static_cast<ACT_WORK *>(EntryAct(ACT_CUT));
            pCut->m_fTime = 0.0f;
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoSmash()
{
    EntryAct(ACT_GO_SMASH);
}

// Runs at the ball holder while the smash charges; once it is full and
// the holder is near, cuts at the ball.
void CComDrvBasketGame::ActLoopGoSmash()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
            if (pHolder == 0) {
                ReturnAct(0);
                return;
            }
            pWork->m_Pos = CComMan::World2Local(static_cast<const CChrCore *>(pHolder)->GetPos(), -1);
            const bcs::FVEC3 &rBase = m_Pos148;
            float fRot = GetRotY_P1ToP2XZ(rBase, pWork->m_Pos);
            float fDist = 0.93f * CComManBasket::GetDistXZ(rBase, pWork->m_Pos);
            bcs::BCS_VecZ_RotY(pWork->m_Pos, 1.0f, fRot);
            pWork->m_Pos.x = pWork->m_Pos.x * fDist + rBase.x;
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = pWork->m_Pos.z * fDist + rBase.z;
        }
        case 1: {
            MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            if (static_cast<CChrCoreBasket *>(m_pChr)->GetSmashChargeRate() != 100) {
                ReturnAct(0);
                return;
            }
            const CChrCore *pHolder = CComMan::GetHoldBallChrCore(0);
            if (pHolder == 0) {
                ReturnAct(0);
                return;
            }
            bcs::FVEC3 diff;
            const bcs::FVEC3 &rMyPos = m_pChr->GetPos();
            bcs::FVEC3Sub(&diff, &pHolder->GetPos(), &rMyPos);
            bcs::FVEC3 d = diff;
            if (bcs::BCS_Vec_GetLenXZ(d) > 3.0f) {
                break;
            }
            float fRot = m_pChr->GetRotY();
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall != 0) {
                fRot = m_pChr->GetRotY_ToPosXZ(pBall->GetPos());
            }
            MethdCut(fRot);
            pWork->m_nStep = 999;
            break;
        }
        default:
            if (m_pChr->GetState() != 0x32) {
                ReturnAct(1);
            }
            break;
    }
}

void CComDrvBasketGame::ActCallMoveBlock()
{
    EntryAct(ACT_MOVE_BLOCK);
}

// Waits a drawn time, then cuts (or only waits).
void CComDrvBasketGame::ActLoopMoveBlock()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            const float &fLv = GetComLvF();
            long nRate = CComMan::GetDatI(0xC2F, fLv);
            if (LotRate() > nRate) {
                long nMin = CComMan::GetDatI(0xC47, fLv);
                long nMax = CComMan::GetDatI(0xC4F, fLv);
                ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
                pWork->m_nStep = 999;
                break;
            }
            long nMin = CComMan::GetDatI(0xC37, fLv);
            long nMax = CComMan::GetDatI(0xC3F, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        case 1: {
            ACT_WORK *pCut = static_cast<ACT_WORK *>(EntryAct(ACT_CUT));
            pCut->m_fTime = 0.0f;
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallGoDefenseGoal()
{
    EntryAct(ACT_GO_DEFENSE_GOAL);
}

// Waits a little, then runs back to defend the goal: from 0x148 towards
// where the ball will be (the holder, or the middle of a curved shot's
// course), the further the fewer team mates are nearer the goal; looks
// at the ball, waits and goes for it.
void CComDrvBasketGame::ActLoopGoDefenseGoal()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    if (m_pChr->IsHoldBall()) {
        ReturnAct(0);
        return;
    }
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            long nMin = CComMan::GetDatI(0x9F8, fLv);
            long nMax = CComMan::GetDatI(0xA00, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        case 1: {
            pWork->m_Pos = m_Pos148;
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall != 0 && pBall->IsActive()) {
                long aRate[3] = {
                    CComMan::GetDatI(0xA08, fLv),
                    CComMan::GetDatI(0xA10, fLv),
                    CComMan::GetDatI(0xA18, fLv),
                };
                long nNear = 0;
                const bcs::FVEC3 &rGoal = m_pChr->GetGuardGoalPos();
                float fMyDist = CComManBasket::GetDistXZ(m_pChr->GetPos(), rGoal);
                for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
                    const CChrCore *pChr = CChrMan::GetChrCore(i);
                    if (pChr != 0 && !pChr->IsDisable() && pChr->GetTeamNo() == m_nMyTeam && pChr != m_pChr &&
                        fMyDist > CComManBasket::GetDistXZ(pChr->GetPos(), rGoal)) {
                        nNear++;
                    }
                }
                bcs::FVEC3 ballPos = pBall->GetPos();
                const CChrCore *pHolder = CChrMan::Util_SearchHoldBallChr(pBall->GetBallNo());
                if (pHolder != 0) {
                    ballPos = pHolder->GetPos();
                } else if (pBall->IsShootMode()) {
                    long nCalc = pBall->GetShootCalcType();
                    if (nCalc == 2 || nCalc == 3) {
                        float fCurve = pBall->GetShootCurveValue();
                        ballPos = pBall->GetShootStartPos();
                        bcs::FVEC3 goal = CGame::GetGoalPos(pBall->GetTargetGoalIdx());
                        bcs::FVEC3 diff;
                        bcs::FVEC3Sub(&diff, &goal, &ballPos);
                        bcs::FVEC3 dir = diff;
                        float fHalf = bcs::BCS_Vec_GetLenXZ(dir) * 0.5f;
                        bcs::FVEC3 xz(dir.x, 0.0f, dir.z);
                        bcs::BCS_Vec_Normalize(dir, xz);
                        bcs::FVEC3 side;
                        bcs::BCS_Vec_RotY(side, dir, pBall->GetShootCalcType() == 2 ? 1.5707964f : -1.5707964f);
                        side.x *= fCurve;
                        side.z *= fCurve;
                        ballPos.x += fHalf * dir.x + side.x;
                        ballPos.y = 0.0f;
                        ballPos.z += fHalf * dir.z + side.z;
                    }
                }
                ballPos = CComMan::World2Local(ballPos, -1);
                bcs::FVEC3 diff;
                bcs::FVEC3Sub(&diff, &ballPos, &pWork->m_Pos);
                bcs::FVEC3 dir = diff;
                float fDist = CComManBasket::GetDistXZ(dir);
                CComManBasket::NormalVecXZ(dir, dir);
                float fScale = fDist * aRate[nNear] / 10.0f;
                dir.x *= fScale;
                dir.z *= fScale;
                bcs::FVEC3Add(&pWork->m_Pos, &pWork->m_Pos, &dir);
            }
            pWork->m_nStep++;
        }
        case 2:
            if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
                ActCallLookBall();
                pWork->m_nStep++;
            }
            break;
        case 3: {
            long nMin = CComMan::GetDatI(0xA20, fLv);
            long nMax = CComMan::GetDatI(0xA28, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        case 4:
            EntryAct(ACT_GET_BALL);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallShootBlock()
{
    EntryAct(ACT_SHOOT_BLOCK);
}

// Against a shot: waits and jumps, gives up, or runs in front of the ball
// (a little off at random) for a while and then jumps.
void CComDrvBasketGame::ActLoopShootBlock()
{
    BK_WORK *pWork = static_cast<BK_WORK *>(GetCurrentActWork());
    const float &fLv = GetComLvF();
    switch (pWork->m_nStep) {
        case 0: {
            long aRate[3] = {
                CComMan::GetDatI(0xC77, fLv),
                CComMan::GetDatI(0xC7F, fLv),
                CComMan::GetDatI(0xC87, fLv),
            };
            switch (LotProbability(aRate, 3)) {
                case 0: {
                    long nMin = CComMan::GetDatI(0xC8F, fLv);
                    long nMax = CComMan::GetDatI(0xC97, fLv);
                    ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
                    pWork->m_nStep = 2;
                    return;
                }
                case 2:
                    ReturnAct(0);
                    return;
            }
            pWork->m_Pos = m_MyPos;
            CBall *pBall = CBallMan::GetBallPtr(0);
            if (pBall != 0) {
                bcs::FVEC3 ballPos = CComMan::World2Local(pBall->GetPos(), -1);
                float fDist = CComMan::GetDatF(0xC9F, fLv);
                bcs::FVEC3 diff;
                bcs::FVEC3Sub(&diff, &m_Pos148, &ballPos);
                bcs::FVEC3 dir = diff;
                bcs::FVEC3 xz(dir.x, 0.0f, dir.z);
                bcs::BCS_Vec_Normalize(dir, xz);
                pWork->m_Pos.x = dir.x * fDist + ballPos.x;
                pWork->m_Pos.y = 0.0f;
                pWork->m_Pos.z = dir.z * fDist + ballPos.z;
                float fRand = RandMaxF(CComMan::GetDatF(0xCA7, fLv));
                bcs::BCS_VecZ_RotY(dir, 1.0f, bcs::DEG_TO_RAD(RandMaxI(360)));
                pWork->m_Pos.x = dir.x * fRand + pWork->m_Pos.x;
                pWork->m_Pos.z = dir.z * fRand + pWork->m_Pos.z;
            }
            long nMin = CComMan::GetDatI(0xCAF, fLv);
            long nMax = CComMan::GetDatI(0xCB7, fLv);
            pWork->m_fTimer = CComManBasket::GetRandValueI(nMin, nMax);
            pWork->m_Pos.y = 0.0f;
            pWork->m_nStep++;
        }
        case 1:
            pWork->m_fTimer -= m_pChr->GetFrameStep();
            if (pWork->m_fTimer <= 0.0f) {
                pWork->m_nStep = 2;
                break;
            }
            MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0);
            break;
        case 2: {
            JUMP_WORK *pJump = static_cast<JUMP_WORK *>(EntryAct(ACT_JUMP2));
            pJump->m_fDir = -1.0f;
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallJumpUpperChance()
{
    EntryAct(ACT_JUMP_UPPER_CHANCE);
}

// Waits a drawn time, then an upper cut, a jump cut or nothing.
void CComDrvBasketGame::ActLoopJumpUpperChance()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            const float &fLv = GetComLvF();
            long aRate[3] = {
                CComMan::GetDatI(0xCE7, fLv),
                CComMan::GetDatI(0xCEF, fLv),
                CComMan::GetDatI(0xCF7, fLv),
            };
            long nMin;
            long nMax;
            switch (LotProbability(aRate, 3)) {
                case 1:
                    nMin = CComMan::GetDatI(0xD0F, fLv);
                    nMax = CComMan::GetDatI(0xD17, fLv);
                    pWork->m_nStep = 1;
                    break;
                case 2:
                    nMin = CComMan::GetDatI(0xD1F, fLv);
                    nMax = CComMan::GetDatI(0xD27, fLv);
                    pWork->m_nStep = 2;
                    break;
                default:
                    nMin = CComMan::GetDatI(0xCFF, fLv);
                    nMax = CComMan::GetDatI(0xD07, fLv);
                    pWork->m_nStep = 999;
                    break;
            }
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            break;
        }
        case 1: {
            JUMP_WORK *pJump = static_cast<JUMP_WORK *>(EntryAct(ACT_JUMP_UPPER));
            pJump->m_fDir = 0.0f;
            pWork->m_nStep = 999;
            break;
        }
        case 2:
            EntryAct(ACT_JUMP_CUT);
            pWork->m_nStep = 999;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

void CComDrvBasketGame::ActCallJumpCutChance()
{
    EntryAct(ACT_JUMP_CUT_CHANCE);
}

// As ActLoopJumpUpperChance, with the jump cut's rates.
void CComDrvBasketGame::ActLoopJumpCutChance()
{
    ACT_WORK *pWork = static_cast<ACT_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            const float &fLv = GetComLvF();
            long aRate[3] = {
                CComMan::GetDatI(0xD2F, fLv),
                CComMan::GetDatI(0xD37, fLv),
                CComMan::GetDatI(0xD3F, fLv),
            };
            long nMin;
            long nMax;
            switch (LotProbability(aRate, 3)) {
                case 1:
                    nMin = CComMan::GetDatI(0xD57, fLv);
                    nMax = CComMan::GetDatI(0xD5F, fLv);
                    pWork->m_nStep = 1;
                    break;
                case 2:
                    nMin = CComMan::GetDatI(0xD67, fLv);
                    nMax = CComMan::GetDatI(0xD6F, fLv);
                    pWork->m_nStep = 2;
                    break;
                default:
                    nMin = CComMan::GetDatI(0xD47, fLv);
                    nMax = CComMan::GetDatI(0xD4F, fLv);
                    pWork->m_nStep = 999;
                    break;
            }
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            break;
        }
        case 1: {
            JUMP_WORK *pJump = static_cast<JUMP_WORK *>(EntryAct(ACT_JUMP_UPPER));
            pJump->m_fDir = 0.0f;
            pWork->m_nStep = 999;
            break;
        }
        case 2:
            EntryAct(ACT_JUMP_CUT);
            pWork->m_nStep = 999;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

// Runs to the place ActCallGoPointPanel set and waits a little.
void CComDrvBasketGame::ActLoopGoPointPanel()
{
    POS_WORK *pWork = static_cast<POS_WORK *>(GetCurrentActWork());
    if (pWork->m_nStep == 0) {
        if (MethdBKRunTest(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0)) {
            ActCallWait(10.0f);
            pWork->m_nStep++;
        }
    } else {
        ReturnAct(1);
    }
}

void CComDrvBasketGame::ActCallGetGShell()
{
    EntryAct(ACT_GET_G_SHELL);
}

// Runs to the nearest green shell fallen on the beach stage and waits on
// it.
void CComDrvBasketGame::ActLoopGetGShell()
{
    PANEL_WORK *pWork = static_cast<PANEL_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0:
            pWork->m_nPanel = CComManBasketGame::GetNBeachNearFallGShell(m_MyPos);
            if (pWork->m_nPanel == 0) {
                ReturnAct(0);
                return;
            }
            pWork->m_nStep++;
        case 1:
            if (!CComManBasketGame::IsNBeachFallGSellDisable(pWork->m_nPanel)) {
                pWork->m_Pos = CComManBasketGame::GetNBeachFallGShellPos(pWork->m_nPanel);
                if (!MethdBKRunTest(pWork->m_Pos, 0.3f, 1.0f, 0)) {
                    break;
                }
            }
            pWork->m_nStep++;
            if (!m_pChr->IsHoldBall()) {
                ActCallLookBall();
                break;
            }
        case 2: {
            float fLv = GetComLvF();
            long nMin = CComMan::GetDatI(0xA4D, fLv);
            long nMax = CComMan::GetDatI(0xA55, fLv);
            ActCallWait(CComManBasket::GetRandValueI(nMin, nMax));
            pWork->m_nStep++;
            break;
        }
        default:
            ReturnAct(1);
            break;
    }
}

} // namespace msw

namespace {
    // (names guessed) The work of ActLoopGoEspBubble.
    struct BUBBLE_WORK : msw::CComDrv::ACT_WORK {
        bcs::FVEC3 m_Pos;           // 0x1C
        unsigned long m_nObstacle;  // 0x28, the bubble's task id
    };
}

namespace msw {

// Steps aside from the obstacle nObstacle (a bubble of the ESP stage).
void CComDrvBasketGame::ActCallGoEspBubble(unsigned long nObstacle)
{
    BUBBLE_WORK *pWork = static_cast<BUBBLE_WORK *>(EntryAct(ACT_GO_ESP_BUBBLE));
    pWork->m_nObstacle = nObstacle;
}

void CComDrvBasketGame::ActLoopGoEspBubble()
{
    static const long nWait = 10;
    BUBBLE_WORK *pWork = static_cast<BUBBLE_WORK *>(GetCurrentActWork());
    switch (pWork->m_nStep) {
        case 0: {
            if (bcs::CTaskMan::GetTask(pWork->m_nObstacle) == 0) {
                ReturnAct(0);
                return;
            }
            static const float fDist = 2.5f;
            float fX = RandMaxI(1) ? -fDist : fDist;
            pWork->m_Pos.x = fX + m_MyPos.x;
            pWork->m_Pos.y = 0.0f;
            pWork->m_Pos.z = m_MyPos.z;
            pWork->m_nStep++;
        }
        case 1:
            if (!MethdBKRun2(pWork->m_Pos, GetAttainmentMoveMinDist(), 1.0f, 0, 0)) {
                break;
            }
            pWork->m_nStep++;
        case 2:
            pWork->m_nStep++;
            if (!m_pChr->Is1on1()) {
                ActCallLookBall();
                break;
            }
        case 3:
            ActCallWait(nWait);
            pWork->m_nStep++;
            break;
        default:
            ReturnAct(1);
            break;
    }
}

} // namespace msw

// 70 degrees in radians, unused (as in CComManBasket.cpp).
static const float s_Unk70Deg = bcs::DEG_TO_RAD(70.0f);

// uncalled stand-in: the original pool has 70 degrees (the static
// initialiser's constant) after the last function's constants and before
// g_ActFuncMap, where the static initialiser, compiled at the end of the
// file, cannot create it. Nothing calls this; the module link strips it.
namespace unit_9AFD4_cpp {
    float Unused70Deg()
    {
        return bcs::DEG_TO_RAD(70.0f);
    }
}

namespace msw {

// The actions by number (ACT_TYPE); OnMainSub_ runs the one on top every
// frame.
const CComDrvBasketGame::ActFunc CComDrvBasketGame::g_ActFuncMap[] = {
    &CComDrv::ActLoopNop,
    &CComDrv::ActLoopWait,
    &CComDrv::ActLoopWaitNeutral,
    &CComDrv::ActLoopLookTarget,
    &CComDrv::ActLoopLookFront,
    &CComDrv::ActLoopLookBall,
    &CComDrv::ActLoopRun,
    &CComDrv::ActLoopRunTurn,
    &CComDrvBasketGame::ActLoopJump,
    &CComDrv::ActLoopJumpBlock,
    &CComDrvBasketGame::ActLoopJumpBall,
    &CComDrvBasketGame::ActLoopGoOffense,
    &CComDrvBasketGame::ActLoopGoOffense2,
    &CComDrvBasketGame::ActLoopGoDefense,
    &CComDrvBasketGame::ActLoopGoDefense2,
    &CComDrvBasketGame::ActLoopGoDefense3,
    &CComDrvBasketGame::ActLoopGoDefPassCourse,
    &CComDrvBasketGame::ActLoopGoDefOnBaller,
    &CComDrvBasketGame::ActLoopGoHome,
    &CComDrvBasketGame::ActLoopGoFreeArea,
    &CComDrvBasketGame::ActLoopGetBall,
    &CComDrvBasketGame::ActLoopGetHatenaPanel,
    &CComDrvBasketGame::ActLoopGoRunRand,
    &CComDrvBasketGame::ActLoopGoRun,
    &CComDrvBasketGame::ActLoopPass,
    &CComDrvBasketGame::ActLoopPassGhost,
    &CComDrvBasketGame::ActLoopShootCancelPass,
    &CComDrvBasketGame::ActLoopCancelPass,
    &CComDrvBasketGame::ActLoopPassSign,
    &CComDrvBasketGame::ActLoopJumpShoot,
    &CComDrvBasketGame::ActLoopDunk,
    &CComDrvBasketGame::ActLoopGoReboundDunk,
    &CComDrvBasketGame::ActLoopReboundDunk,
    &CComDrvBasketGame::ActLoopAlleyoop,
    &CComDrvBasketGame::ActLoopAssistPlay,
    &CComDrvBasketGame::ActLoopBlockade,
    &CComDrvBasketGame::ActLoopGoBack,
    &CComDrvBasketGame::ActLoopGoZiguzagu,
    &CComDrvBasketGame::ActLoopGoSide,
    &CComDrvBasketGame::ActLoopSpinMove,
    &CComDrvBasketGame::ActLoopAirTrick,
    &CComDrvBasketGame::ActLoopStickDash,
    &CComDrvBasketGame::ActLoopGoTargetGoal,
    &CComDrvBasketGame::ActLoopBallKeep,
    &CComDrvBasketGame::ActLoopGoDunkArea,
    &CComDrvBasketGame::ActLoopGoDunk,
    &CComDrvBasketGame::ActLoopLayUp,
    &CComDrvBasketGame::ActLoopCancelLayUp,
    &CComDrvBasketGame::ActLoopItemShoot,
    &CComDrvBasketGame::ActLoopCombination,
    &CComDrvBasketGame::ActLoopGoEnemy,
    &CComDrvBasketGame::ActLoopGoBall,
    &CComDrvBasketGame::ActLoopRebound,
    &CComDrvBasketGame::ActLoopCut,
    &CComDrvBasketGame::ActLoopMissCut,
    &CComDrvBasketGame::ActLoopJump,
    &CComDrvBasketGame::ActLoopDefJump,
    &CComDrvBasketGame::ActLoopJumpCut,
    &CComDrvBasketGame::ActLoopJumpUpper,
    &CComDrvBasketGame::ActLoopDefAlleyoop,
    &CComDrvBasketGame::ActLoopItemEscape,
    &CComDrvBasketGame::ActLoopGoEscape,
    &CComDrvBasketGame::ActLoopGoEscape2,
    &CComDrvBasketGame::ActLoopItemThrow,
    &CComDrvBasketGame::ActLoopDefGhost,
    &CComDrvBasketGame::ActLoopCutChance,
    &CComDrvBasketGame::ActLoopGoSmash,
    &CComDrvBasketGame::ActLoopMoveBlock,
    &CComDrvBasketGame::ActLoopGoDefenseGoal,
    &CComDrvBasketGame::ActLoopShootBlock,
    &CComDrvBasketGame::ActLoopJumpUpperChance,
    &CComDrvBasketGame::ActLoopJumpCutChance,
    &CComDrvBasketGame::ActLoopGoPointPanel,
    &CComDrvBasketGame::ActLoopGetGShell,
    &CComDrvBasketGame::ActLoopGoEspBubble,
    &CComDrvBasketGame::ActLoopScript,
};

} // namespace msw
