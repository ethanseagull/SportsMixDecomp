#ifndef MSW_CBALLINLINE_HPP_
#define MSW_CBALLINLINE_HPP_

#include "game/msw/CBall.hpp"
#include "nw4r/g3d/ResFile.hpp"

// The bodies of msw::CBall's inline virtual functions, read from the weak
// copies MenuF's game/menu/CTopMenuChrObj.cpp emits with msw::CTopMenuBall's
// vtable (BasketF's CBallBasket.cpp, DodgeF's CBallDodge.cpp and HockeyF's
// CBallHockey.cpp emit the same set).
//
// They are not in the class in CBall.hpp on purpose: a unit that includes
// them emits a weak copy of each for every CBall-derived vtable it emits,
// and the units already linked from source must not start doing that before
// their module's own copy is written. Include this header only from a unit
// whose original carries the copies. Most of them are the empty answers of
// another game's ball (the hockey getters of a basketball, and so on).

inline void msw::CBall::Create(nw4r::g3d::ResFile file)
{
    Create(file, 0x40);
}

inline void msw::CBall::Reset()
{
}

inline float msw::CBall::GetRadius() const
{
    return 0.25f;
}

inline float msw::CBall::GetBgCollRadius() const
{
    return 0.23f;
}

inline float msw::CBall::GetBgCollRadiusXZ() const
{
    return 0.23f;
}

inline float msw::CBall::GetCollPushRadiusDefault() const
{
    return 0.25f;
}

inline float msw::CBall::GetCollBodyRadiusDefault() const
{
    return 0.35f;
}

inline float msw::CBall::GetCollCatchRadiusDefault() const
{
    return 0.35f;
}

inline float msw::CBall::GetGravityDefault() const
{
    return -0.00625f;
}

inline bcs::FVEC3 msw::CBall::GetShootStartPos() const
{
    return bcs::FVEC3(0.0f, 0.0f, 0.0f);
}

inline long msw::CBall::GetShootCalcType() const
{
    return -1;
}

inline float msw::CBall::GetShootCurveValue() const
{
    return 0.0f;
}

inline float msw::CBall::CalcJumpShootVel(const bcs::FVEC3 &, const bcs::FVEC3 &, bcs::FVEC3 &, float &)
{
    return 0.0f;
}

inline int msw::CBall::IsAirBall() const
{
    return 0;
}

inline void msw::CBall::OnBaPad0Shot()
{
}

inline void msw::CBall::ResetBaPad0Shot()
{
}

inline int msw::CBall::IsBaExplode() const
{
    return 0;
}

inline int msw::CBall::IsBaPad0Shot() const
{
    return 0;
}

inline void msw::CBall::SetDodgeShootMode(long, long, const bcs::FVEC3 &, long, long)
{
}

inline float msw::CBall::SetDodgeShootMode(long, long, long, unsigned long)
{
    return 0.0f;
}

inline float msw::CBall::SetDodgePowerShootMode(long, long, long, float, unsigned long)
{
    return 0.0f;
}

inline void msw::CBall::SetOutBallMode(const bcs::FVEC3 &, long, long)
{
}

inline void msw::CBall::SetFlickBallMode(float, float, float, float, long, long)
{
}

inline void msw::CBall::SetDodgeReflectFreeMode(const bcs::FVEC3 &, float)
{
}

inline long msw::CBall::GetDodgeHitType(long) const
{
    return 0;
}

inline void msw::CBall::DodgeReattach(bcs::CMdlObj *, unsigned long)
{
}

inline void msw::CBall::DodgeOnPreTouch(long)
{
}

inline void msw::CBall::DodgeOnHit(long)
{
}

inline void msw::CBall::DodgeOnCatch(CChrCore *)
{
}

inline void msw::CBall::DodgeOnRelease(CChrCore *)
{
}

inline long msw::CBall::GetDodgeShootType() const
{
    return m_nShootType;
}

inline void msw::CBall::SetDodgeAddRange(float)
{
}

inline float msw::CBall::GetDodgeAddRange() const
{
    return 0.0f;
}

inline void msw::CBall::SetDodgeReceive(const bcs::FVEC3 &, float, float)
{
}

inline void msw::CBall::SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &)
{
}

inline int msw::CBall::SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &)
{
    return 0;
}

inline int msw::CBall::SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &)
{
    return 0;
}

inline void msw::CBall::SetVolleyPassShotMode(BALL::VOLLEY::PASS_SHOT_ARG &)
{
}

inline void msw::CBall::SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, float, long, float, unsigned long)
{
}

inline void msw::CBall::SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long)
{
}

inline void msw::CBall::SetVolleyArtsMode(const bcs::FVEC3 &, long, long, long, long, long, long, float, float, float, unsigned long)
{
}

inline void msw::CBall::SetVolleyShootMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long)
{
}

inline void msw::CBall::SetVolleyDropMode(const bcs::FVEC3 &, long, long, long, unsigned long)
{
}

inline void msw::CBall::SetVolleyAnyHitMode(const bcs::FVEC3 *, long)
{
}

inline void msw::CBall::SetVolleyVsBossTossUpMode(const bcs::FVEC3 &, long, unsigned long)
{
}

inline void msw::CBall::SetVolleyMissionRRShotMode(const bcs::FVEC3 &, float, float)
{
}

inline void msw::CBall::GetVolleyPosAfterFrame(bcs::FVEC3 &, float, int) const
{
}

inline float msw::CBall::GetVolleyCatchableRate() const
{
    return 1.0f;
}

inline int msw::CBall::CanbeVolleyStraightSpike(const bcs::FVEC3 &)
{
    return 0;
}

inline int msw::CBall::CanbeVolleyReceiveVariation() const
{
    return 0;
}

inline int msw::CBall::CanbeVolleySpikeVariation() const
{
    return 0;
}

inline int msw::CBall::CanebVolleyTouch(long) const
{
    return 1;
}

inline long msw::CBall::GetVolleyReceiveReaction(unsigned long, long) const
{
    return 0;
}

inline long msw::CBall::GetVolleyBlockReaction(unsigned long, long) const
{
    return 0;
}

inline float msw::CBall::GetVolleyReceiveStaggerFrame(long) const
{
    return 0.0f;
}

inline long msw::CBall::GetVolleyReceiveDropCoinNum(long) const
{
    return 0;
}

inline float msw::CBall::GetVolleyFlickDownPower(long) const
{
    return 0.0f;
}

inline int msw::CBall::GetVolleyEndureParam(BALL::VOLLEY::ENDURE_PRM &)
{
    return 0;
}

inline float msw::CBall::GetVolleyPassCounter() const
{
    return 0.0f;
}

inline void msw::CBall::GetVolleyBallMarkerDispFrame(float &, float &) const
{
}

inline void msw::CBall::TrgVolleyReceiveReaction(long, unsigned long)
{
}

inline void msw::CBall::TrgVolleyBlockReaction(unsigned long)
{
}

inline long msw::CBall::GetVolleyBlockDropCoinNum(long) const
{
    return 0;
}

inline int msw::CBall::TrgVolleyPreparationTouch(long, long, long, long)
{
    return 1;
}

inline void msw::CBall::TrgVolleyArtsAttach(long, long, long, int)
{
}

inline void msw::CBall::TrgVolleyArtsReady(long, int)
{
}

inline int msw::CBall::TrgVolleyArtsOver(long, long, unsigned long)
{
    return 0;
}

inline int msw::CBall::IsVolleyArtsAttach() const
{
    return 0;
}

inline int msw::CBall::IsVolleyFakeBall() const
{
    return 0;
}

inline int msw::CBall::IsVolleyChanceBallMode() const
{
    return 0;
}

inline int msw::CBall::IsVolleyPowerSpike() const
{
    return 0;
}

inline int msw::CBall::IsVolleyCurveSpike() const
{
    return 0;
}

inline int msw::CBall::IsVolleyFeintSpike() const
{
    return 0;
}

inline int msw::CBall::IsVolleyTimingSpike() const
{
    return 0;
}

inline int msw::CBall::IsVolleyNetHit() const
{
    return 0;
}

inline int msw::CBall::IsVolleyEnemySpike() const
{
    return 0;
}

inline void msw::CBall::PlayVolleySpReceiveSuccessSe(unsigned long)
{
}

inline float msw::CBall::GetVolleyChargePower() const
{
    return 0.0f;
}

inline void msw::CBall::SetVolleyChargePower(float)
{
}

inline void msw::CBall::ClearVolleyChargePower()
{
}

inline long msw::CBall::GetVolleyTouchNum(long) const
{
    return 0;
}

inline long msw::CBall::GetVolleyTrueTouchNum(long) const
{
    return 0;
}

inline long msw::CBall::GetVolleyCanbeTouchKind(long, long)
{
    return 0;
}

inline long msw::CBall::GetVolleyBallTypeRR() const
{
    return 0;
}

inline long msw::CBall::GetVolleyBallColorTypeRR() const
{
    return 0;
}

inline int msw::CBall::IsVolleyComBallRR() const
{
    return 0;
}

inline int msw::CBall::IsVolleyChargeBall() const
{
    return 0;
}

inline int msw::CBall::IsVolleyTargetPosInnerCourt(long, float)
{
    return 0;
}

inline int msw::CBall::GetVolleySpikeJumpTargetPos(bcs::FVEC3 &) const
{
    return 0;
}

inline void msw::CBall::GetVolleyTouchDownPos(bcs::FVEC3 &, int) const
{
}

inline int msw::CBall::IsVolleyObstacleBallRR() const
{
    return 0;
}

inline int msw::CBall::IsVolleyInCatchableArea(long) const
{
    return 1;
}

inline int msw::CBall::IsVolleyThroughUnderNet() const
{
    return 0;
}

inline int msw::CBall::DoVolleyItemBallDirectSpike(long, long)
{
    return 0;
}

inline void msw::CBall::SetHockeyChargeShootMode(const bcs::FVEC3 &, float, long, float, long, float, float, float, int, int, int)
{
}

inline void msw::CBall::SetHockeyShootMode(const bcs::FVEC3 &, float, long, float, long, int, long)
{
}

inline void msw::CBall::SetHockeyArtsMode(const bcs::FVEC3 &, float, long, long, int, long)
{
}

inline void msw::CBall::SetHockeyItemShootMode(const bcs::FVEC3 &, float, long, float, int, long)
{
}

inline void msw::CBall::SetHockeyBodyGoalMode(long, long, long)
{
}

inline int msw::CBall::SetHockeySnatch(const CCollAtkUnit *, const bcs::FVEC3 &)
{
    return 0;
}

inline void msw::CBall::TrgGoalIn(const bcs::FVEC3 &, long, long, long, long, long)
{
}

inline void msw::CBall::SetHockeyDropMode(const bcs::FVEC3 &, float, long, long)
{
}

inline void msw::CBall::SetHockeyArtsDropMode(const bcs::FVEC3 &, float, long, long)
{
}

inline long msw::CBall::GetArtsType() const
{
    return 0;
}

inline int msw::CBall::TrgHockeyArtsEnd(long)
{
    return 0;
}

inline int msw::CBall::IsHockeyArtsEnd() const
{
    return 0;
}

inline void msw::CBall::SetHockeyArtsStart()
{
}

inline void msw::CBall::SetHockeyShootTargetPos(const bcs::FVEC3 &)
{
}

inline void msw::CBall::SetHockeyShootTargetMove(float, float)
{
}

inline float msw::CBall::GetHockeyNoChgLeaderCount()
{
    return 0.0f;
}

inline long msw::CBall::GetHockeyPassTgtChrIdx()
{
    return 0;
}

inline int msw::CBall::GetPassCatchPos(bcs::FVEC3 &, long)
{
    return 0;
}

inline long msw::CBall::GetHockeyHaveTeamIdx()
{
    return 0;
}

inline float msw::CBall::GetHockeySnatchFrame()
{
    return 0.0f;
}

inline void msw::CBall::SetHockeySnatchFrame(float)
{
}

inline long msw::CBall::GetHockeyFaceOffMode()
{
    return 0;
}

inline long msw::CBall::GetHockeyCatchChrIdx()
{
    return 0;
}

inline int msw::CBall::IsHockeyShootGoalDir()
{
    return 0;
}

inline void msw::CBall::SetHockeyScaleSpeed(float)
{
}

inline long msw::CBall::GetHockeyReBoundType() const
{
    return 0;
}

inline long msw::CBall::GetHockeyItemBallType() const
{
    return -1;
}

inline float msw::CBall::GetHokeyBombExplodeRadius() const
{
    return 0.0f;
}

inline float msw::CBall::GetHokeyBombShootDist() const
{
    return 0.0f;
}

inline int msw::CBall::IsHockeyAttackMode() const
{
    return 0;
}

inline int msw::CBall::GetHokeyCurveShootData(float &, bcs::FVEC3 &)
{
    return 0;
}

inline float msw::CBall::GetFaceOffGuideDispFrm()
{
    return 0.0f;
}

inline void msw::CBall::SettHockeyReflectMode(float)
{
}

inline int msw::CBall::IsShootExceptionalStart()
{
    return 0;
}

inline int msw::CBall::IsDummyShootBall()
{
    return 0;
}

inline float msw::CBall::GetHockeyPassSpeed()
{
    return 0.0f;
}

inline long msw::CBall::GetVsBossBallMode() const
{
    return 0;
}

inline void msw::CBall::SetVsBossTossUpMode(const bcs::FVEC3 &, long, unsigned long)
{
}

inline int msw::CBall::SetVsBossSpikeShotMode(BALL::BOSS::SPIKE_SHOT_ARG &)
{
    return 0;
}

inline void msw::CBall::SetVsBossPassShotMode(BALL::BOSS::PASS_SHOT_ARG &)
{
}

inline void msw::CBall::SetVsBossPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long)
{
}

inline void msw::CBall::SetVsBossShootMode(const bcs::FVEC3 &, long, float)
{
}

inline void msw::CBall::SetVsBossDunkShotMode(const bcs::FVEC3 &, long)
{
}

inline void msw::CBall::SetVsBossArtsDunkMode(const bcs::FVEC3 &, long, int)
{
}

inline int msw::CBall::CanbeVsBossReceive() const
{
    return 1;
}

inline int msw::CBall::CanbeVsBossReflect() const
{
    return 1;
}

inline int msw::CBall::CanbeVsBossAirTrick() const
{
    return 1;
}

inline void msw::CBall::SetVsBossReflectCheckEnd()
{
}

inline void msw::CBall::SetVsBossCoinPower(long)
{
}

inline void msw::CBall::AddVsBossCoinPower(long)
{
}

inline long msw::CBall::GetVsBossCoinPower() const
{
    return 0;
}

inline void msw::CBall::ClearVsBossCoinPower()
{
}

inline void msw::CBall::TrgVsBossArtsAttach(long, long, long, int)
{
}

inline void msw::CBall::SetJumpBallMode()
{
}

inline void msw::CBall::SetChrReflectMode(long, float)
{
}

inline float msw::CBall::JumpBallFrameSet(int)
{
    return 0.0f;
}

inline int msw::CBall::IsChrNeedBlockMode(long) const
{
    return 0;
}

inline int msw::CBall::IsCanbeTurnOver() const
{
    return 1;
}

inline int msw::CBall::IsNowRebound() const
{
    return 0;
}

inline int msw::CBall::IsInvincible() const
{
    return 0;
}

inline int msw::CBall::IsOffensive() const
{
    return 0;
}

inline void msw::CBall::SetNoCatchCount(long)
{
}

inline float msw::CBall::GetNoCatchCount() const
{
    return 0.0f;
}

inline float msw::CBall::GetGoalInRate() const
{
    return 0.0f;
}

inline float msw::CBall::GetDefCounter() const
{
    return 0.0f;
}

inline void msw::CBall::SetShotChrIdx(long)
{
}

inline long msw::CBall::GetShotChrIdx() const
{
    return 0;
}

inline void msw::CBall::SetPassChrIdx(long)
{
}

inline long msw::CBall::GetPassChrIdx() const
{
    return 0;
}

inline void msw::CBall::SetShotTeamIdx(long)
{
}

inline long msw::CBall::GetShotTeamIdx() const
{
    return 0;
}

inline void msw::CBall::SetShotArea(long)
{
}

inline long msw::CBall::GetShotArea() const
{
    return 0;
}

inline void msw::CBall::SetTargetGoalIdx(long)
{
}

inline long msw::CBall::GetTargetGoalIdx() const
{
    return 0;
}

inline void msw::CBall::SetChargePower(float)
{
}

inline float msw::CBall::GetChargePower() const
{
    return 0.0f;
}

inline long msw::CBall::GetTouchNum() const
{
    return 0;
}

inline long msw::CBall::GetTargetCourtIdx() const
{
    return 0;
}

inline int msw::CBall::IsTargetCourt(long) const
{
    return 0;
}

inline void msw::CBall::SetReboundDisable()
{
}

inline void msw::CBall::TrgGoalIn(long, long, long, long, long)
{
}

inline void msw::CBall::TrgTouch(long)
{
}

inline void msw::CBall::TrgScore(long)
{
}

inline void msw::CBall::ClearTouch()
{
}

inline long msw::CBall::GetRallyNum() const
{
    return 0;
}

inline void msw::CBall::AddRallyNum(long)
{
}

inline float msw::CBall::GetPassFlyFrame(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const
{
    return 0.0f;
}

inline float msw::CBall::GetShootFlyFrame(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const
{
    return 0.0f;
}

inline float msw::CBall::GetTouchDownFrame(float) const
{
    return 0.0f;
}

inline float msw::CBall::GetTouchDownFrame2(float, float) const
{
    return 0.0f;
}

inline void msw::CBall::GetPosAfterFrame(bcs::FVEC3 &, float) const
{
}

inline void msw::CBall::GetVelAfterFrame(bcs::FVEC3 &, float) const
{
}

inline float msw::CBall::GetHeightPassFrame(float, int) const
{
    return 0.0f;
}

inline void msw::CBall::ResetOnAttach_()
{
}

inline void msw::CBall::ResetOnDetach_()
{
}

inline void msw::CBall::UpdateRainbow_()
{
}

inline void msw::CBall::DispFunc_()
{
}

inline void msw::CBall::CreateCollBody_()
{
}

inline void msw::CBall::CreateCollAttack_()
{
}

inline unsigned long msw::CBall::GetQueueWorkSize() const
{
    return 4;
}

inline unsigned long msw::CBall::GetQueueStackSize() const
{
    return 4;
}

#endif /* !MSW_CBALLINLINE_HPP_ */
