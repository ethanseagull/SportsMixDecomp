// msw::CComDrvDodgeGame: the run check and the Methd*/Canbe* overrides
// (shot, pass, diving catch, quick turn, special shot and its target
// marker). The first half of what VolleyF has in
// game/volley/unit_AB1D8.cpp; the interrupt and think functions are the
// next file, game/dodge/unit_A5464.cpp (its float pool starts afresh after
// this file's padded .rodata). The file name is not known.
#include "game/msw/CComDrvDodge.hpp"
#include "game/msw/CBall.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CComMan.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStageDodge07.hpp"
#include "game/msw/CStgGmk_BossPackun_Dodge.hpp"
#include "bcs/BCS_Vec.hpp"
#include "nw4r/math/triangular.h"

namespace {
    inline float DegToRad(float fDeg)
    {
        return fDeg * (3.1415927f / 180.0f);
    }

    // Set by the file's static initialiser and used by nothing in the file
    // (as in game/dodge/unit_A40EC.cpp).
    float s_fAngle60 = DegToRad(60.0f);

    // (guessed helper, as in the volley version) The horizontal distance
    // between two points.
    inline float DistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo)
    {
        bcs::FVEC3 from(rFrom.x, 0.0f, rFrom.z);
        bcs::FVEC3 to(rTo.x, 0.0f, rTo.z);
        return bcs::FVEC3Dist(&from, &to);
    }
}

namespace msw {

// Whether a character stands in the way to rPos, other than the driver's
// team mates on the same side of the line (infield or outfield).
int CComDrvDodgeGame::IsExistChrToTargetWorldForRunEx(const bcs::FVEC3 &rPos)
{
    unsigned long nIgnore = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam == pChr->GetTeamNo() &&
            !pChr->IsDodgeInfield() == !m_pChr->IsDodgeInfield()) {
            CComMan::SetEnableBit(nIgnore, i);
        }
    }
    return CComMan::IsExistChrToTargetWorld(m_pChr, rPos, 0.2f, 0.5f + m_pChr->GetCollSize_BodyPush(), nIgnore, 0);
}

// Shoots, or plays the special shot instead when the level's rate says so.
int CComDrvDodgeGame::MethdShoot()
{
    long nRate = CComMan::GetDatI(0x4F6, GetComLvF());
    if (!CanbeSp()) {
        nRate = 0;
    }
    if (LotRate() < nRate) {
        return MethdSp();
    }
    return m_pChr->MethdShoot(-1.0f);
}

int CComDrvDodgeGame::MethdPass(CChrCore *pChr)
{
    return MethdPass(pChr->GetPlayerNo());
}

int CComDrvDodgeGame::MethdPass(long nChr)
{
    bcs::FVEC3 pos = CComMan::GetChrWorldPos(nChr);
    return m_pChr->MethdPass(m_pChr->GetRotY_ToPosXZ(pos), 1);
}

int CComDrvDodgeGame::MethdPass(const bcs::FVEC3 &rPos)
{
    bcs::FVEC3 pos = Local2World(rPos, -1);
    return m_pChr->MethdPass(m_pChr->GetRotY_ToPosXZ(pos), 1);
}

// Passing to the boss Packun of stage 07 (feeding it) from the infield,
// while it is hungry.
int CComDrvDodgeGame::CanbePassBossPackun()
{
    if (CGame::g_GameWork.GetStage() != 7) {
        return 0;
    }
    if (!m_pChr->IsDodgeInfield()) {
        return 0;
    }
    CStgGmk_BossPackun_Dodge *pBoss = CStageDodge07::GetBossPackun();
    if (pBoss == 0) {
        return 0;
    }
    return pBoss->IsHungry() != 0;
}

int CComDrvDodgeGame::MethdPassBossPackun()
{
    if (!CanbePassBossPackun()) {
        return 0;
    }
    CStgGmk_BossPackun_Dodge *pBoss = CStageDodge07::GetBossPackun();
    if (pBoss == 0) {
        return 0;
    }
    bcs::FVEC3 pos = pBoss->GetPos();
    return m_pChr->MethdPass(m_pChr->GetRotY_ToPosXZ(pos), 1);
}

int CComDrvDodgeGame::CanbeDivingCatch() const
{
    return m_pChr->CanbeDivingCatch();
}

int CComDrvDodgeGame::MethdDivingCatch(const bcs::FVEC3 &rPos)
{
    bcs::FVEC3 pos = Local2World(rPos, -1);
    return m_pChr->MethdDivingCatch(m_pChr->GetRotY_ToPosXZ(pos));
}

int CComDrvDodgeGame::MethdSwitchAutoAimTarget()
{
    m_pChr->SwitchAutoAimTarget();
    return 1;
}

int CComDrvDodgeGame::CanbeQuickTurn()
{
    return m_pChr->CanbeQuickTurn(-1.0f);
}

// Turns round to the ball, unless there is none or the character already
// faces it.
int CComDrvDodgeGame::MethdQuickTurn()
{
    if (m_pBall == 0) {
        return 0;
    }
    if (IsDirTarget(m_BallPos, 40)) {
        return 1;
    }
    return m_pChr->MethdQuickTurn(-1.0f);
}

int CComDrvDodgeGame::MethdBallCatch()
{
    return m_pChr->MethdBallCatch(-1.0f);
}

// The special shot: only from the infield, for a computer team, while
// someone of the other team stands in its infield to be hit.
int CComDrvDodgeGame::CanbeSp() const
{
    if (!m_pChr->IsDodgeInfield()) {
        return 0;
    }
    if (IsExistPadDrv()) {
        return 0;
    }
    long nTarget = 0;
    for (long i = 0; i < CChrMan::GetChrCoreNum(); i++) {
        CChrCore *pChr = CChrMan::GetChrCore(i);
        if (pChr != 0 && !pChr->IsDisable() && m_nMyTeam != pChr->GetTeamNo() && pChr->IsDodgeInfield() &&
            pChr->GetUnk17B() != 1) {
            nTarget = 1;
            break;
        }
    }
    if (nTarget > 0) {
        return m_pChr->CanbeAirTrick();
    }
    return 0;
}

int CComDrvDodgeGame::MethdSp()
{
    if (CanbeSp() == 0) {
        return 0;
    }
    return m_pChr->MethdAirTrick();
}

// No turning while holding the ball.
int CComDrvDodgeGame::MethdRotateY(const bcs::FVEC3 &rPos, long nArg)
{
    if (m_pChr->IsHoldBall()) {
        return 1;
    }
    return CComDrv::MethdRotateY(rPos, nArg);
}

int CComDrvDodgeGame::MethdRotateY(float fRot, long nArg)
{
    if (m_pChr->IsHoldBall()) {
        return 1;
    }
    return CComDrv::MethdRotateY(fRot, nArg);
}

int CComDrvDodgeGame::CanbeMoveSpTargetMarkerPos() const
{
    return m_pChr->CanbeMoveDodgeArtsTargetMarkerPos();
}

// Moves the special shot's target marker towards rPos; returns 1 while it
// is not there (closer than fRange) yet.
int CComDrvDodgeGame::MethdMoveSpTargetMarkerPos(const bcs::FVEC3 &rPos, float fRange) const
{
    bcs::FVEC3 marker;
    if (!m_pChr->GetDodgeArtsTargetMarkerPos(marker, 0)) {
        return 1;
    }

    bcs::FVEC3 pos = Local2World(rPos, -1);
    float fDist = DistXZ(pos, marker);
    if (fDist < fRange) {
        return 1;
    }

    // The direction from the marker to rPos, from 0 to 2 pi.
    bcs::FVEC3 diff;
    diff.x = pos.x - marker.x;
    diff.z = pos.z - marker.z;
    float fRot = nw4r::math::Atan2Rad(diff.x, diff.z);
    while (fRot > 6.2831855f) {
        fRot -= 6.2831855f;
    }
    while (fRot < 0.0f) {
        fRot += 6.2831855f;
    }

    if (fDist > 0.2f && !m_pChr->MoveDodgeArtsTargetMarkerPos(fRot)) {
        return 1;
    }
    return 0;
}

} // namespace msw
