#ifndef MSW_CCHRCOREINLINE_HPP_
#define MSW_CCHRCOREINLINE_HPP_

#include "game/msw/CChrCore.hpp"

// The bodies of msw::CChrCore's inline virtual functions (vtable slot in the
// comment above each), read from the weak copies BossF's
// game/boss/CChrCoreBoss.cpp emits with msw::CChrCoreBoss's vtable (BasketF,
// DodgeF, HockeyF and VolleyF each have one file, their game/<mode>/
// CChrCore.cpp, that emits the same set). Most are the empty answers of
// another game's character (the volleyball spike queries of a boss-game
// character, and so on).
//
// They are not in the class in CChrCore.hpp on purpose, for the same reason
// as game/msw/CBallInline.hpp: with the bodies in the class every file that
// emits a CChrCore-derived vtable emits a weak copy of each, with the float
// constants they need. The original DodgeF character files
// (CChrCoreDodgeMario, Yoshi, Moogle) have neither, so the original kept the
// bodies out of the class header (tried 2026-11-12: in the class they broke
// the DodgeF hash). Include this header only from a mode's own CChrCore file.

namespace msw {

    // 0xC
    inline void CChrCore::DispFunc()
    {
    }

    // 0x14
    inline long CChrCore::GetQueueStackSize() const
    {
        return 4;
    }

    // 0x20
    inline unsigned long CChrCore::GetHoldBallHandle() const
    {
        return m_hBall;
    }

    // 0x24
    inline void CChrCore::SetConditionEffect_(long)
    {
    }

    // 0x2C
    inline void CChrCore::GetSpRootPos(bcs::FVEC3 &rPos) const
    {
        rPos = GetPos();
    }

    // 0x30
    inline int CChrCore::IsNowBlockWork() const
    {
        return 0;
    }

    // 0x3C
    inline int CChrCore::IsHoldItem() const
    {
        return (m_nStatusFlag >> 1) & 1;
    }

    // 0x40
    inline int CChrCore::IsHoldAny() const
    {
        return IsHoldBall() || IsHoldItem();
    }

    // 0x44
    inline int CChrCore::Is1on1() const
    {
        return 0;
    }

    // 0x48
    inline float CChrCore::Get1on1StartDist() const
    {
        return 0.0f;
    }

    // 0x54
    inline float CChrCore::GetAirTrickStatePassFrame() const
    {
        return m_fAirTrickStatePassFrame;
    }

    // 0x58
    inline int CChrCore::GetTargetMarkerPos(bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x5C
    inline int CChrCore::MoveTargetMarkerPos(float)
    {
        return 0;
    }

    // 0x60
    inline int CChrCore::AutoMoveTargetMarkerPos()
    {
        return 0;
    }

    // 0x64
    inline long CChrCore::CheckGuideTelopDisp()
    {
        return -1;
    }

    // 0x68
    inline int CChrCore::IsBattle() const
    {
        return 0;
    }

    // 0x6C
    inline int CChrCore::IsDisp1On1Maker() const
    {
        return 0;
    }

    // 0x70
    inline int CChrCore::IsDunkBlockEnable() const
    {
        return 0;
    }

    // 0x74
    inline int CChrCore::CanbePassRequest()
    {
        return 0;
    }

    // 0x78
    inline int CChrCore::MethdPassRequest()
    {
        return 0;
    }

    // 0x7C
    inline float CChrCore::GetBlockWorkDir() const
    {
        return 0.0f;
    }

    // 0x80
    inline float CChrCore::GetJumpBlockLocalFrame() const
    {
        return -1.0f;
    }

    // 0x84
    inline int CChrCore::GetStarRemainderRate() const
    {
        return 0;
    }

    // 0x88
    inline int CChrCore::IsAirTrickDunkMode() const
    {
        return 0;
    }

    // 0x8C
    inline int CChrCore::IsVolleyRightBallTouch(long) const
    {
        return 1;
    }

    // 0x90
    inline int CChrCore::IsVolleyServeMode() const
    {
        return 0;
    }

    // 0x94
    inline int CChrCore::IsVolleyJumpServeMode() const
    {
        return 0;
    }

    // 0x98
    inline int CChrCore::IsVolleyTargetMarkerActive() const
    {
        return 0;
    }

    // 0x9C
    inline long CChrCore::GetVolleyTargetMarkerNum() const
    {
        return 0;
    }

    // 0xA0
    inline void CChrCore::GetVolleyTargetMarkerDefaultPos(long, bcs::FVEC3 &, int)
    {
    }

    // 0xA4
    inline long CChrCore::GetVolleyTargetMarkerType() const
    {
        return 0;
    }

    // 0xA8
    inline long CChrCore::GetVolleyTargetMarkerFixedType() const
    {
        return 0;
    }

    // 0xAC
    inline int CChrCore::GetVolleyTargetMarkerPos(bcs::FVEC3 &, long)
    {
        return 0;
    }

    // 0xB0
    inline int CChrCore::GetVolleyTargetMarkerMoved(bcs::FVEC3 &)
    {
        return 0;
    }

    // 0xB4
    inline int CChrCore::MoveVolleyTargetMarkerPos(float)
    {
        return 0;
    }

    // 0xB8
    inline int CChrCore::CanbeMoveVolleyTargetMarkerPos()
    {
        return 0;
    }

    // 0xBC
    inline int CChrCore::IsVolleyTargetMarkerStealthMode()
    {
        return 0;
    }

    // 0xC8
    inline int CChrCore::IsVolleySpikeJumpStart() const
    {
        return 0;
    }

    // 0xCC
    inline int CChrCore::IsVolleySuperSpikeJump() const
    {
        return 0;
    }

    // 0xD0
    inline int CChrCore::IsVolleySpikeSwingEnd() const
    {
        return 0;
    }

    // 0xD4
    inline int CChrCore::IsVolleySpikeFall() const
    {
        return 0;
    }

    // 0xD8
    inline int CChrCore::IsVolleyAirTrickSwingEnd() const
    {
        return 0;
    }

    // 0xDC
    inline int CChrCore::IsVolleyAirTrickFall() const
    {
        return 0;
    }

    // 0xE0
    inline int CChrCore::IsVolleyBlockJumpStart() const
    {
        return 0;
    }

    // 0xE4
    inline int CChrCore::IsVolleyBlockFall() const
    {
        return 0;
    }

    // 0xE8
    inline int CChrCore::IsVolleyBlockSuccess() const
    {
        return 0;
    }

    // 0xEC
    inline int CChrCore::IsVolleyDispSpikeJumpMarker() const
    {
        return 0;
    }

    // 0xF0
    inline int CChrCore::GetVolleySpikeFeintTargetPos(float, bcs::FVEC3 &, bcs::FVEC3 &, bcs::FVEC3 &)
    {
        return 0;
    }

    // 0xF4
    inline int CChrCore::GetVolleyBallShotCollPos(long, bcs::FVEC3 &, float &) const
    {
        return 0;
    }

    // 0xFC
    inline CBall * CChrCore::GetVolleyTargetBall() const
    {
        return 0;
    }

    // 0x100
    inline int CChrCore::IsVolleySuperSpikeCurveType() const
    {
        return 0;
    }

    // 0x104
    inline float CChrCore::GetVolleySuperSpikeCurveRate() const
    {
        return 0.0f;
    }

    // 0x108
    inline float CChrCore::GetVolleySuperSpikeCurveMaxDist() const
    {
        return 0.0f;
    }

    // 0x10C
    inline float CChrCore::GetVolleySuperSpikeCurveMinDist() const
    {
        return 0.0f;
    }

    // 0x110
    inline float CChrCore::GetVolleyBlockJumpDelayFrame() const
    {
        return 0.0f;
    }

    // 0x114
    inline int CChrCore::IsVolleyEnableDiveReceive() const
    {
        return 0;
    }

    // 0x118
    inline int CChrCore::IsVolleyCombinationMode() const
    {
        return 0;
    }

    // 0x11C
    inline long CChrCore::GetVolleyCombinationRole() const
    {
        return 0;
    }

    // 0x120
    inline long CChrCore::GetVolleyCombinationStep() const
    {
        return 0;
    }

    // 0x124
    inline void CChrCore::SetVolleyCombinationRoleDown()
    {
    }

    // 0x128
    inline int CChrCore::IsVolleyDispExIcon(long) const
    {
        return 0;
    }

    // 0x12C
    inline long CChrCore::CheckVolleyLineMsgDisp()
    {
        return -1;
    }

    // 0x130
    inline int CChrCore::IsVolleyCombinationReceiver() const
    {
        return 0;
    }

    // 0x134
    inline int CChrCore::IsVolleyCombinationAttacker() const
    {
        return 0;
    }

    // 0x138
    inline int CChrCore::IsVolleyArtsKoopaShotFire() const
    {
        return 0;
    }

    // 0x13C
    inline int CChrCore::IsVolleyArtsKoopaFireEnd() const
    {
        return 1;
    }

    // 0x140
    inline long CChrCore::GetVolleyArtsExInput()
    {
        return 0;
    }

    // 0x144
    inline int CChrCore::IsHockeyChgArrowMarker()
    {
        return 0;
    }

    // 0x148
    inline float CChrCore::GetHockeyArrowMarkerRotY() const
    {
        return 0.0f;
    }

    // 0x14C
    inline int CChrCore::IsHockeyTackleMarker() const
    {
        return 0;
    }

    // 0x150
    inline int CChrCore::IsHockeyTgtArrowMarker() const
    {
        return 0;
    }

    // 0x154
    inline void CChrCore::GetHockeyShootGoalTgtPos(bcs::FVEC3 &, float &)
    {
    }

    // 0x158
    inline int CChrCore::IsHockeyDispShootMarker()
    {
        return 0;
    }

    // 0x15C
    inline void CChrCore::SetHockeyNoChargeCutFrame(float)
    {
    }

    // 0x160
    inline float CChrCore::GetHockeyNoChargeCutFrame()
    {
        return 0.0f;
    }

    // 0x164
    inline float CChrCore::GetShootChargeFrameMax() const
    {
        return 0.0f;
    }

    // 0x168
    inline int CChrCore::GetHockeyKeeperTgtPos(bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x16C
    inline int CChrCore::IsKeeper() const
    {
        return 0;
    }

    // 0x170
    inline void CChrCore::SetKeeper(int)
    {
    }

    // 0x174
    inline void CChrCore::SetMaxSkateSpeed(float)
    {
    }

    // 0x178
    inline float CChrCore::GetMaxSkateSpeed() const
    {
        return 0.0f;
    }

    // 0x17C
    inline float CChrCore::GetHockeyVelXZ() const
    {
        return 0.0f;
    }

    // 0x180
    inline float CChrCore::GetBallScramblePoint() const
    {
        return -1.0f;
    }

    // 0x184
    inline float CChrCore::GetBallScrambleNowPoint() const
    {
        return -1.0f;
    }

    // 0x188
    inline float CChrCore::GetBallScrambleWinPoint() const
    {
        return -1.0f;
    }

    // 0x18C
    inline float CChrCore::GetBallScrambleMaxPoint() const
    {
        return -1.0f;
    }

    // 0x190
    inline int CChrCore::GetBodyContactSkate() const
    {
        return 0;
    }

    // 0x194
    inline void CChrCore::SetBodyContactSkate(int)
    {
    }

    // 0x198
    inline void CChrCore::SetPackNotCatchFrame(float)
    {
    }

    // 0x19C
    inline int CChrCore::IsHockeyBodyNoPush() const
    {
        return 0;
    }

    // 0x1A0
    inline long CChrCore::GetHockeyMaxShootCourseDirPercent()
    {
        return 0;
    }

    // 0x1A4
    inline int CChrCore::GetHockeyShootGoalTgtPos(bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x1A8
    inline int CChrCore::MethdHockeyKeeperQuickMove(const bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x1AC
    inline int CChrCore::IsHockeyDamage() const
    {
        return 0;
    }

    // 0x1B0
    inline int CChrCore::IsHockeyBallHit()
    {
        return 0;
    }

    // 0x1B4
    inline int CChrCore::IsHockeyAttack() const
    {
        return 0;
    }

    // 0x1B8
    inline int CChrCore::IsHockeyFighting() const
    {
        return 0;
    }

    // 0x1BC
    inline void CChrCore::HockeyChrReflect(bcs::FVEC3 &, CChrCore *)
    {
    }

    // 0x1C0
    inline long CChrCore::Get1on1AttackHitType() const
    {
        return -1;
    }

    // 0x1C4
    inline long CChrCore::GetNow1on1AttackNum() const
    {
        return 0;
    }

    // 0x1C8
    inline float CChrCore::Get1on1AttackWaitTimeMax() const
    {
        return 0.0f;
    }

    // 0x1CC
    inline void CChrCore::SetUseShootMarker(int)
    {
    }

    // 0x1D0
    inline int CChrCore::IsUseShootMarker() const
    {
        return 0;
    }

    // 0x1D4
    inline int CChrCore::MethdHockeySetMoveRevise(float, float)
    {
        return 0;
    }

    // 0x1D8
    inline void CChrCore::HockeyArtsEnd()
    {
    }

    // 0x1DC
    inline int CChrCore::CanbeKeeperDivingCatch()
    {
        return 0;
    }

    // 0x1E0
    inline int CChrCore::MethdKeeperDivingCatch()
    {
        return 0;
    }

    // 0x1E4
    inline float CChrCore::GetAttackPower(int)
    {
        return 0.0f;
    }

    // 0x1E8
    inline long CChrCore::GetDamageCollKind() const
    {
        return -1;
    }

    // 0x1EC
    inline float CChrCore::GetHockeyBodyCollRadiusNow() const
    {
        return 0.0f;
    }

    // 0x1F0
    inline float CChrCore::GetHockeyMass() const
    {
        return 0.0f;
    }

    // 0x1F4
    inline float CChrCore::GetFaceOffChargeMaxFrame() const
    {
        return 0.0f;
    }

    // 0x1F8
    inline long CChrCore::GetTackleCount(long) const
    {
        return 0;
    }

    // 0x1FC
    inline void CChrCore::SetAddTackleCount(long)
    {
    }

    // 0x200
    inline void CChrCore::ReSetTackleCount(long)
    {
    }

    // 0x204
    inline int CChrCore::CanbeHockeyComBlockAttack()
    {
        return 0;
    }

    // 0x208
    inline int CChrCore::IsHockeyInvisible() const
    {
        return 0;
    }

    // 0x20C
    inline void CChrCore::SetHockeyAttackObjNull(long)
    {
    }

    // 0x210
    inline int CChrCore::GetHockeyAttackObjInfo(long, bcs::FVEC3 &, float &, bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x214
    inline long CChrCore::GetHockeyAttackObjMax()
    {
        return 0;
    }

    // 0x218
    inline void CChrCore::SetKeeperCatchMode()
    {
    }

    // 0x21C
    inline float CChrCore::GetHockeyPassSpeed()
    {
        return 0.0f;
    }

    // 0x220
    inline float CChrCore::GetHockeyKeeperPassSpeed()
    {
        return 0.0f;
    }

    // 0x224
    inline int CChrCore::IsHockeyStatePaintTumble() const
    {
        return 0;
    }

    // 0x228
    inline void CChrCore::SetDodgeArrangeArea(long)
    {
    }

    // 0x22C
    inline long CChrCore::GetDdogeArrangeArea() const
    {
        return 0;
    }

    // 0x230
    inline int CChrCore::IsDodgeInfield() const
    {
        return 1;
    }

    // 0x234
    inline int CChrCore::IsDodgeCloseRange() const
    {
        return 0;
    }

    // 0x238
    inline float CChrCore::GetDodgeCatchCollRadius() const
    {
        return 0.0f;
    }

    // 0x23C
    inline long CChrCore::GetDodgeLockTargetChrIdx() const
    {
        return 0;
    }

    // 0x240
    inline void CChrCore::SwitchAutoAimTarget()
    {
    }

    // 0x244
    inline int CChrCore::IsDodgeTargetMarkerActive()
    {
        return 0;
    }

    // 0x248
    inline long CChrCore::GetDodgeTargetMarkerType() const
    {
        return 0;
    }

    // 0x24C
    inline long CChrCore::GetDodgeTargetMarkerFixedType() const
    {
        return 0;
    }

    // 0x250
    inline int CChrCore::GetDodgeTargetMarkerPos(bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x254
    inline void CChrCore::DodgeForceUpdateAreaCode()
    {
    }

    // 0x258
    inline int CChrCore::IsDodgeOnBallJump() const
    {
        return 0;
    }

    // 0x25C
    inline int CChrCore::IsDodgeOnBallJumpFall() const
    {
        return 0;
    }

    // 0x260
    inline int CChrCore::IsDodgeJumpBallChr() const
    {
        return 0;
    }

    // 0x264
    inline void CChrCore::SetDodgeJumpBallChr(int)
    {
    }

    // 0x268
    inline float CChrCore::GetDodgeDivingCatchDist() const
    {
        return 0.0f;
    }

    // 0x26C
    inline int CChrCore::IsDodgeArtsTargetMarkerActive() const
    {
        return 0;
    }

    // 0x270
    inline long CChrCore::GetDodgeArtsTargetMarkerType() const
    {
        return 0;
    }

    // 0x274
    inline long CChrCore::GetDodgeArtsTargetMarkerPos(bcs::FVEC3 &, long)
    {
        return 0;
    }

    // 0x278
    inline long CChrCore::GetDodgeArtsTargetMarkerNum() const
    {
        return 0;
    }

    // 0x27C
    inline void CChrCore::GetDodgeArtsTargetMarkerDefaultPos(bcs::FVEC3 &, long)
    {
    }

    // 0x280
    inline long CChrCore::GetDodgeArtsTargetMarkerMoved(bcs::FVEC3 &)
    {
        return 0;
    }

    // 0x284
    inline void CChrCore::SetDodgeArtsTargetMarkerPosClip(bcs::FVEC3 &)
    {
    }

    // 0x288
    inline int CChrCore::MoveDodgeArtsTargetMarkerPos(float)
    {
        return 0;
    }

    // 0x28C
    inline int CChrCore::CanbeMoveDodgeArtsTargetMarkerPos()
    {
        return 0;
    }

    // 0x290
    inline void CChrCore::SetDodgeArtsTargetMarkerDisable()
    {
    }

    // 0x294
    inline void CChrCore::GetVel(bcs::FVEC3 &) const
    {
    }

    // 0x298
    inline void CChrCore::SetVel(bcs::FVEC3 &, float)
    {
    }

    // 0x29C
    inline void CChrCore::SetSwitch(int)
    {
    }

    // 0x2A0
    inline int CChrCore::IsSwitch() const
    {
        return 0;
    }

    // 0x2A4
    inline void CChrCore::SetNoReflectFrame(float)
    {
    }

    // 0x2A8
    inline float CChrCore::GetNoReflectFrame() const
    {
        return 0.0f;
    }

    // 0x2AC
    inline void CChrCore::SetReflectNoMoveFrame(float)
    {
    }

    // 0x2B0
    inline float CChrCore::GetReflectNoMoveFrame() const
    {
        return 0.0f;
    }

    // 0x2B4
    inline void CChrCore::SetAutoBrakeStartTime(float)
    {
    }

    // 0x2B8
    inline float CChrCore::GetAutoBrakeStartTime() const
    {
        return 0.0f;
    }

    // 0x2BC
    inline float CChrCore::GetKeeperDivingCatchSpeed(int) const
    {
        return 0.0f;
    }

    // 0x2C0
    inline float CChrCore::GetHockeyKeeperDivingSteeleDist(int) const
    {
        return 0.0f;
    }

    // 0x2C4
    inline float CChrCore::GetHockeyKeeperDivingSteeleGoalFrame(int) const
    {
        return 0.0f;
    }

    // 0x2C8
    inline void CChrCore::SetHockeySkateWork(float, float, float, float)
    {
    }

    // 0x2CC
    inline int CChrCore::IsHockeyNowCombinationMode() const
    {
        return 0;
    }

    // 0x2D0
    inline int CChrCore::IsCombinationShootStance() const
    {
        return 0;
    }

    // 0x2D4
    inline void CChrCore::GetForceInfo(bcs::FVEC3 &, float &)
    {
    }

    // 0x2D8
    inline void CChrCore::CrashSkateMissionSetReady(int)
    {
    }

    // 0x2DC
    inline int CChrCore::IsHockeyNowKeeperMoveMode() const
    {
        return 0;
    }

    // 0x2E0
    inline int CChrCore::IsHockeyTackleSteal() const
    {
        return 0;
    }

    // 0x328
    inline float CChrCore::GetTargetGoalXLength() const
    {
        return 0.0f;
    }

    // 0x32C
    inline float CChrCore::GetTargetGoalZLength() const
    {
        return 0.0f;
    }

    // 0x330
    inline float CChrCore::GetGuardGoalXLength() const
    {
        return 0.0f;
    }

    // 0x334
    inline float CChrCore::GetGuardGoalZLength() const
    {
        return 0.0f;
    }

    // 0x340
    inline float CChrCore::GetCollSize_BodyPush() const
    {
        return 0.5f;
    }

    // 0x344
    inline void CChrCore::GetAlleyoopPassTargetPos(bcs::FVEC3 &, bcs::FVEC3 &, bcs::FVEC3 &) const
    {
    }

    // 0x378
    inline void CChrCore::Create_()
    {
    }

    // 0x3B4
    inline void CChrCore::LoopStateDefault()
    {
    }

    // 0x3B8
    inline void CChrCore::DtorStateDefault()
    {
    }

    // 0x3C8
    inline void CChrCore::CtorStateBlockWork()
    {
    }

    // 0x3CC
    inline void CChrCore::LoopStateBlockWork()
    {
    }

    // 0x3D0
    inline void CChrCore::DtorStateBlockWork()
    {
    }

    // 0x3D4
    inline void CChrCore::CtorStateAutoBlockWork()
    {
    }

    // 0x3D8
    inline void CChrCore::LoopStateAutoBlockWork()
    {
    }

    // 0x3DC
    inline void CChrCore::DtorStateAutoBlockWork()
    {
    }

    // 0x3EC
    inline void CChrCore::CtorStateJump()
    {
    }

    // 0x3F0
    inline void CChrCore::LoopStateJump()
    {
    }

    // 0x3F4
    inline void CChrCore::DtorStateJump()
    {
    }

    // 0x41C
    inline void CChrCore::CtorStateBlockCut()
    {
    }

    // 0x420
    inline void CChrCore::LoopStateBlockCut()
    {
    }

    // 0x424
    inline void CChrCore::DtorStateBlockCut()
    {
    }

    // 0x428
    inline void CChrCore::CtorStateChargeCut()
    {
    }

    // 0x42C
    inline void CChrCore::LoopStateChargeCut()
    {
    }

    // 0x430
    inline void CChrCore::DtorStateChargeCut()
    {
    }

    // 0x434
    inline void CChrCore::CtorStateJumpUpper()
    {
    }

    // 0x438
    inline void CChrCore::LoopStateJumpUpper()
    {
    }

    // 0x43C
    inline void CChrCore::DtorStateJumpUpper()
    {
    }

    // 0x440
    inline void CChrCore::CtorStateDropCut()
    {
    }

    // 0x444
    inline void CChrCore::LoopStateDropCut()
    {
    }

    // 0x448
    inline void CChrCore::DtorStateDropCut()
    {
    }

    // 0x458
    inline void CChrCore::CtorStateSizeChange()
    {
    }

    // 0x45C
    inline void CChrCore::LoopStateSizeChange()
    {
    }

    // 0x460
    inline void CChrCore::DtorStateSizeChange()
    {
    }

    // 0x464
    inline void CChrCore::CtorStateFreeze()
    {
    }

    // 0x468
    inline void CChrCore::LoopStateFreeze()
    {
    }

    // 0x46C
    inline void CChrCore::DtorStateFreeze()
    {
    }

    // 0x4B8
    inline void CChrCore::CtorStateAerialPass()
    {
    }

    // 0x4BC
    inline void CChrCore::LoopStateAerialPass()
    {
    }

    // 0x4C0
    inline void CChrCore::DtorStateAerialPass()
    {
    }

    // 0x4C4
    inline void CChrCore::CtorStatePassCatch()
    {
    }

    // 0x4C8
    inline void CChrCore::LoopStatePassCatch()
    {
    }

    // 0x4CC
    inline void CChrCore::DtorStatePassCatch()
    {
    }

    // 0x4D0
    inline void CChrCore::CtorStateArtsPass()
    {
    }

    // 0x4D4
    inline void CChrCore::LoopStateArtsPass()
    {
    }

    // 0x4D8
    inline void CChrCore::DtorStateArtsPass()
    {
    }

    // 0x4DC
    inline void CChrCore::CtorStateSDB()
    {
    }

    // 0x4E0
    inline void CChrCore::LoopStateSDB()
    {
    }

    // 0x4E4
    inline void CChrCore::DtorStateSDB()
    {
    }

    // 0x4E8
    inline void CChrCore::CtorStateSDB_Chain()
    {
    }

    // 0x4EC
    inline void CChrCore::LoopStateSDB_Chain()
    {
    }

    // 0x4F0
    inline void CChrCore::DtorStateSDB_Chain()
    {
    }

    // 0x4F4
    inline void CChrCore::CtorStateRDB()
    {
    }

    // 0x4F8
    inline void CChrCore::LoopStateRDB()
    {
    }

    // 0x4FC
    inline void CChrCore::DtorStateRDB()
    {
    }

    // 0x500
    inline void CChrCore::CtorStateRDB_Chain()
    {
    }

    // 0x504
    inline void CChrCore::LoopStateRDB_Chain()
    {
    }

    // 0x508
    inline void CChrCore::DtorStateRDB_Chain()
    {
    }

    // 0x50C
    inline void CChrCore::CtorStateQuickTurn()
    {
    }

    // 0x510
    inline void CChrCore::LoopStateQuickTurn()
    {
    }

    // 0x514
    inline void CChrCore::DtorStateQuickTurn()
    {
    }

    // 0x518
    inline void CChrCore::CtorStateTrickMove()
    {
    }

    // 0x51C
    inline void CChrCore::LoopStateTrickMove()
    {
    }

    // 0x520
    inline void CChrCore::DtorStateTrickMove()
    {
    }

    // 0x524
    inline void CChrCore::CtorStateTrickDance()
    {
    }

    // 0x528
    inline void CChrCore::LoopStateTrickDance()
    {
    }

    // 0x52C
    inline void CChrCore::DtorStateTrickDance()
    {
    }

    // 0x530
    inline void CChrCore::CtorStateCatch()
    {
    }

    // 0x534
    inline void CChrCore::LoopStateCatch()
    {
    }

    // 0x538
    inline void CChrCore::DtorStateCatch()
    {
    }

    // 0x53C
    inline void CChrCore::CtorStateSDB_Catch()
    {
    }

    // 0x540
    inline void CChrCore::LoopStateSDB_Catch()
    {
    }

    // 0x544
    inline void CChrCore::DtorStateSDB_Catch()
    {
    }

    // 0x554
    inline void CChrCore::CtorStateAerialShoot()
    {
    }

    // 0x558
    inline void CChrCore::LoopStateAerialShoot()
    {
    }

    // 0x55C
    inline void CChrCore::DtorStateAerialShoot()
    {
    }

    // 0x560
    inline void CChrCore::CtorStateReboundDunk()
    {
    }

    // 0x564
    inline void CChrCore::LoopStateReboundDunk()
    {
    }

    // 0x568
    inline void CChrCore::DtorStateReboundDunk()
    {
    }

    // 0x56C
    inline void CChrCore::CtorStateLayUp()
    {
    }

    // 0x570
    inline void CChrCore::LoopStateLayUp()
    {
    }

    // 0x574
    inline void CChrCore::DtorStateLayUp()
    {
    }

    // 0x578
    inline void CChrCore::CtorStateAlleyoop()
    {
    }

    // 0x57C
    inline void CChrCore::LoopStateAlleyoop()
    {
    }

    // 0x580
    inline void CChrCore::DtorStateAlleyoop()
    {
    }

    // 0x59C
    inline void CChrCore::CtorStateItemBallAttack()
    {
    }

    // 0x5A0
    inline void CChrCore::LoopStateItemBallAttack()
    {
    }

    // 0x5A4
    inline void CChrCore::DtorStateItemBallAttack()
    {
    }

    // 0x5A8
    inline void CChrCore::CtorStateBigDunk()
    {
    }

    // 0x5AC
    inline void CChrCore::LoopStateBigDunk()
    {
    }

    // 0x5B0
    inline void CChrCore::DtorStateBigDunk()
    {
    }

    // 0x5B4
    inline void CChrCore::CtorStateServe()
    {
    }

    // 0x5B8
    inline void CChrCore::LoopStateServe()
    {
    }

    // 0x5BC
    inline void CChrCore::DtorStateServe()
    {
    }

    // 0x5D8
    inline void CChrCore::CtorStateBlockWorkLose()
    {
    }

    // 0x5DC
    inline void CChrCore::LoopStateBlockWorkLose()
    {
    }

    // 0x5E0
    inline void CChrCore::DtorStateBlockWorkLose()
    {
    }

    // 0x5E4
    inline void CChrCore::CtorStateAutoDribblePosture()
    {
    }

    // 0x5E8
    inline void CChrCore::LoopStateAutoDribblePosture()
    {
    }

    // 0x5EC
    inline void CChrCore::DtorStateAutoDribblePosture()
    {
    }

    // 0x5F0
    inline void CChrCore::CtorStateJumpBlock()
    {
    }

    // 0x5F4
    inline void CChrCore::LoopStateJumpBlock()
    {
    }

    // 0x5F8
    inline void CChrCore::DtorStateJumpBlock()
    {
    }

    // 0x5FC
    inline void CChrCore::CtorStateSwatIt()
    {
    }

    // 0x600
    inline void CChrCore::LoopStateSwatIt()
    {
    }

    // 0x604
    inline void CChrCore::DtorStateSwatIt()
    {
    }

    // 0x608
    inline void CChrCore::CtorStateSmashCut()
    {
    }

    // 0x60C
    inline void CChrCore::LoopStateSmashCut()
    {
    }

    // 0x610
    inline void CChrCore::DtorStateSmashCut()
    {
    }

    // 0x614
    inline void CChrCore::CtorStateArtsShotCatch()
    {
    }

    // 0x618
    inline void CChrCore::LoopStateArtsShotCatch()
    {
    }

    // 0x61C
    inline void CChrCore::DtorStateArtsShotCatch()
    {
    }

    // 0x620
    inline void CChrCore::CtorStateDash()
    {
    }

    // 0x624
    inline void CChrCore::LoopStateDash()
    {
    }

    // 0x628
    inline void CChrCore::DtorStateDash()
    {
    }

    // 0x62C
    inline void CChrCore::CtorStateJumpPass()
    {
    }

    // 0x630
    inline void CChrCore::LoopStateJumpPass()
    {
    }

    // 0x634
    inline void CChrCore::DtorStateJumpPass()
    {
    }

    // 0x638
    inline void CChrCore::CtorStateJumpCatch()
    {
    }

    // 0x63C
    inline void CChrCore::LoopStateJumpCatch()
    {
    }

    // 0x640
    inline void CChrCore::DtorStateJumpCatch()
    {
    }

    // 0x644
    inline void CChrCore::CtorStateDribbleMiss()
    {
    }

    // 0x648
    inline void CChrCore::LoopStateDribbleMiss()
    {
    }

    // 0x64C
    inline void CChrCore::DtorStateDribbleMiss()
    {
    }

    // 0x650
    inline void CChrCore::CtorStateBallCatch()
    {
    }

    // 0x654
    inline void CChrCore::LoopStateBallCatch()
    {
    }

    // 0x658
    inline void CChrCore::DtorStateBallCatch()
    {
    }

    // 0x65C
    inline void CChrCore::CtorStateBallCatchBack()
    {
    }

    // 0x660
    inline void CChrCore::LoopStateBallCatchBack()
    {
    }

    // 0x664
    inline void CChrCore::DtorStateBallCatchBack()
    {
    }

    // 0x668
    inline void CChrCore::CtorStateStagger()
    {
    }

    // 0x66C
    inline void CChrCore::LoopStateStagger()
    {
    }

    // 0x670
    inline void CChrCore::DtorStateStagger()
    {
    }

    // 0x674
    inline void CChrCore::CtorStateImpact()
    {
    }

    // 0x678
    inline void CChrCore::LoopStateImpact()
    {
    }

    // 0x67C
    inline void CChrCore::DtorStateImpact()
    {
    }

    // 0x68C
    inline void CChrCore::CtorStateSpinStagger()
    {
    }

    // 0x690
    inline void CChrCore::LoopStateSpinStagger()
    {
    }

    // 0x694
    inline void CChrCore::DtorStateSpinStagger()
    {
    }

    // 0x698
    inline void CChrCore::CtorStateArtsOut()
    {
    }

    // 0x69C
    inline void CChrCore::LoopStateArtsOut()
    {
    }

    // 0x6A0
    inline void CChrCore::DtorStateArtsOut()
    {
    }

    // 0x6A4
    inline void CChrCore::CtorStateLineOver()
    {
    }

    // 0x6A8
    inline void CChrCore::LoopStateLineOver()
    {
    }

    // 0x6AC
    inline void CChrCore::DtorStateLineOver()
    {
    }

    // 0x6B0
    inline void CChrCore::CtorStateCrouch()
    {
    }

    // 0x6B4
    inline void CChrCore::LoopStateCrouch()
    {
    }

    // 0x6B8
    inline void CChrCore::DtorStateCrouch()
    {
    }

    // 0x6BC
    inline void CChrCore::CtorStateDivingCatch()
    {
    }

    // 0x6C0
    inline void CChrCore::LoopStateDivingCatch()
    {
    }

    // 0x6C4
    inline void CChrCore::DtorStateDivingCatch()
    {
    }

    // 0x6C8
    inline void CChrCore::CtorStateSpinAttack()
    {
    }

    // 0x6CC
    inline void CChrCore::LoopStateSpinAttack()
    {
    }

    // 0x6D0
    inline void CChrCore::DtorStateSpinAttack()
    {
    }

    // 0x6D4
    inline void CChrCore::CtorStateSkate()
    {
    }

    // 0x6D8
    inline void CChrCore::LoopStateSkate()
    {
    }

    // 0x6DC
    inline void CChrCore::DtorStateSkate()
    {
    }

    // 0x6E0
    inline void CChrCore::CtorStateStealStance()
    {
    }

    // 0x6E4
    inline void CChrCore::LoopStateStealStance()
    {
    }

    // 0x6E8
    inline void CChrCore::DtorStateStealStance()
    {
    }

    // 0x6F8
    inline void CChrCore::CtorStateBrake()
    {
    }

    // 0x6FC
    inline void CChrCore::LoopStateBrake()
    {
    }

    // 0x700
    inline void CChrCore::DtorStateBrake()
    {
    }

    // 0x704
    inline void CChrCore::CtorStateSwitch()
    {
    }

    // 0x708
    inline void CChrCore::LoopStateSwitch()
    {
    }

    // 0x70C
    inline void CChrCore::DtorStateSwitch()
    {
    }

    // 0x710
    inline void CChrCore::CtorStateBlockStance()
    {
    }

    // 0x714
    inline void CChrCore::LoopStateBlockStance()
    {
    }

    // 0x718
    inline void CChrCore::DtorStateBlockStance()
    {
    }

    // 0x71C
    inline void CChrCore::CtorStateChangeStance()
    {
    }

    // 0x720
    inline void CChrCore::LoopStateChangeStance()
    {
    }

    // 0x724
    inline void CChrCore::DtorStateChangeStance()
    {
    }

    // 0x728
    inline void CChrCore::CtorStateAutoChangeStance()
    {
    }

    // 0x72C
    inline void CChrCore::LoopStateAutoChangeStance()
    {
    }

    // 0x730
    inline void CChrCore::DtorStateAutoChangeStance()
    {
    }

    // 0x734
    inline void CChrCore::CtorStateDirectShootStance()
    {
    }

    // 0x738
    inline void CChrCore::LoopStateDirectShootStance()
    {
    }

    // 0x73C
    inline void CChrCore::DtorStateDirectShootStance()
    {
    }

    // 0x740
    inline void CChrCore::CtorStateDirectPassStance()
    {
    }

    // 0x744
    inline void CChrCore::LoopStateDirectPassStance()
    {
    }

    // 0x748
    inline void CChrCore::DtorStateDirectPassStance()
    {
    }

    // 0x74C
    inline void CChrCore::CtorStateTackleGuard()
    {
    }

    // 0x750
    inline void CChrCore::LoopStateTackleGuard()
    {
    }

    // 0x754
    inline void CChrCore::DtorStateTackleGuard()
    {
    }

    // 0x758
    inline void CChrCore::CtorStateFallOut()
    {
    }

    // 0x75C
    inline void CChrCore::LoopStateFallOut()
    {
    }

    // 0x760
    inline void CChrCore::DtorStateFallOut()
    {
    }

    // 0x764
    inline void CChrCore::CtorStateFallSafe()
    {
    }

    // 0x768
    inline void CChrCore::LoopStateFallSafe()
    {
    }

    // 0x76C
    inline void CChrCore::DtorStateFallSafe()
    {
    }

    // 0x770
    inline void CChrCore::CtorStateSmashAttack()
    {
    }

    // 0x774
    inline void CChrCore::LoopStateSmashAttack()
    {
    }

    // 0x778
    inline void CChrCore::DtorStateSmashAttack()
    {
    }

    // 0x77C
    inline void CChrCore::CtorStateBallScramble()
    {
    }

    // 0x780
    inline void CChrCore::LoopStateBallScramble()
    {
    }

    // 0x784
    inline void CChrCore::DtorStateBallScramble()
    {
    }

    // 0x788
    inline void CChrCore::CtorStateFaceOff()
    {
    }

    // 0x78C
    inline void CChrCore::LoopStateFaceOff()
    {
    }

    // 0x790
    inline void CChrCore::DtorStateFaceOff()
    {
    }

    // 0x794
    inline void CChrCore::CtorStateDivingSteal()
    {
    }

    // 0x798
    inline void CChrCore::LoopStateDivingSteal()
    {
    }

    // 0x79C
    inline void CChrCore::DtorStateDivingSteal()
    {
    }

    // 0x7A0
    inline void CChrCore::CtorStateBodyContact()
    {
    }

    // 0x7A4
    inline void CChrCore::LoopStateBodyContact()
    {
    }

    // 0x7A8
    inline void CChrCore::DtorStateBodyContact()
    {
    }

    // 0x7AC
    inline void CChrCore::CtorStateOffBallStance()
    {
    }

    // 0x7B0
    inline void CChrCore::LoopStateOffBallStance()
    {
    }

    // 0x7B4
    inline void CChrCore::DtorStateOffBallStance()
    {
    }

    // 0x7B8
    inline void CChrCore::CtorStateAttackFight()
    {
    }

    // 0x7BC
    inline void CChrCore::LoopStateAttackFight()
    {
    }

    // 0x7C0
    inline void CChrCore::DtorStateAttackFight()
    {
    }

    // 0x7C4
    inline void CChrCore::CtorStateKeeperCatchAction()
    {
    }

    // 0x7C8
    inline void CChrCore::LoopStateKeeperCatchAction()
    {
    }

    // 0x7CC
    inline void CChrCore::DtorStateKeeperCatchAction()
    {
    }

    // 0x7D0
    inline void CChrCore::CtorStateFall()
    {
    }

    // 0x7D4
    inline void CChrCore::LoopStateFall()
    {
    }

    // 0x7D8
    inline void CChrCore::DtorStateFall()
    {
    }

    // 0x7DC
    inline void CChrCore::CtorStateRebirth()
    {
    }

    // 0x7E0
    inline void CChrCore::LoopStateRebirth()
    {
    }

    // 0x7E4
    inline void CChrCore::DtorStateRebirth()
    {
    }

    // 0x7F4
    inline int CChrCore::CanbeBlockWork()
    {
        return 0;
    }

    // 0x7F8
    inline int CChrCore::MethdBlockWork(float)
    {
        return 0;
    }

    // 0x7FC
    inline int CChrCore::MethdBlockWork(float, float)
    {
        return 0;
    }

    // 0x800
    inline int CChrCore::CanbeJump()
    {
        return 0;
    }

    // 0x804
    inline int CChrCore::MethdJump()
    {
        return 0;
    }

    // 0x824
    inline int CChrCore::CanbeChgSpeedUpState()
    {
        return 0;
    }

    // 0x82C
    inline int CChrCore::CanbeChgSpeedDownState()
    {
        return 0;
    }

    // 0x834
    inline int CChrCore::CanbeJumpUpper() const
    {
        return 0;
    }

    // 0x838
    inline int CChrCore::CanbeNowJumpUpper() const
    {
        return 0;
    }

    // 0x83C
    inline int CChrCore::MethdJumpUpper()
    {
        return 0;
    }

    // 0x848
    inline int CChrCore::CanbeCatch()
    {
        return 0;
    }

    // 0x84C
    inline int CChrCore::MethdCatch()
    {
        return 0;
    }

    // 0x850
    inline int CChrCore::CanbeCatchToSDB()
    {
        return 0;
    }

    // 0x854
    inline int CChrCore::MethdCatchToSDB()
    {
        return 0;
    }

    // 0x858
    inline int CChrCore::CanbeSDBCatch()
    {
        return 0;
    }

    // 0x85C
    inline int CChrCore::MethdSDBCatch()
    {
        return 0;
    }

    // 0x860
    inline int CChrCore::CanbeDribble()
    {
        return 0;
    }

    // 0x864
    inline int CChrCore::CanbeSDB()
    {
        return 0;
    }

    // 0x868
    inline int CChrCore::MethdSDB(float)
    {
        return 0;
    }

    // 0x86C
    inline int CChrCore::CanbeRDB()
    {
        return 0;
    }

    // 0x870
    inline int CChrCore::MethdRDB(INPUT_WORK *, float)
    {
        return 0;
    }

    // 0x874
    inline int CChrCore::MethdRDB(INPUT_WORK *, float, float)
    {
        return 0;
    }

    // 0x878
    inline int CChrCore::CanbeQuickTurnState()
    {
        return 0;
    }

    // 0x87C
    inline int CChrCore::CanbeQuickTurn(float)
    {
        return 0;
    }

    // 0x880
    inline int CChrCore::MethdQuickTurn(float)
    {
        return 0;
    }

    // 0x884
    inline int CChrCore::CanbeTrickMove()
    {
        return 0;
    }

    // 0x888
    inline int CChrCore::MethdTrickMove(float)
    {
        return 0;
    }

    // 0x88C
    inline int CChrCore::CanbeTrickDance()
    {
        return 0;
    }

    // 0x890
    inline int CChrCore::MethdTrickDance()
    {
        return 0;
    }

    // 0x894
    inline int CChrCore::CanbeBlockCut(int)
    {
        return 0;
    }

    // 0x898
    inline int CChrCore::CanbeNowBlockCut()
    {
        return 0;
    }

    // 0x89C
    inline int CChrCore::MethdBlockCut(float)
    {
        return 0;
    }

    // 0x8A0
    inline int CChrCore::CanbeChargeCut()
    {
        return 0;
    }

    // 0x8A4
    inline int CChrCore::MethdChargeCut(float)
    {
        return 0;
    }

    // 0x8A8
    inline int CChrCore::CanbeDropCut()
    {
        return 0;
    }

    // 0x8AC
    inline int CChrCore::CanbeNowDropCut()
    {
        return 0;
    }

    // 0x8B0
    inline int CChrCore::MethdDropCut()
    {
        return 0;
    }

    // 0x8BC
    inline int CChrCore::CanbeJumpShoot()
    {
        return 0;
    }

    // 0x8C0
    inline int CChrCore::MethdJumpShoot()
    {
        return 0;
    }

    // 0x8C4
    inline int CChrCore::MethdJumpShoot(float, int)
    {
        return 0;
    }

    // 0x8C8
    inline int CChrCore::CanbeAerialShoot()
    {
        return 0;
    }

    // 0x8CC
    inline int CChrCore::MethdAerialShoot()
    {
        return 0;
    }

    // 0x8D0
    inline int CChrCore::CanbeRebound()
    {
        return 0;
    }

    // 0x8D4
    inline int CChrCore::CanbeReboundDunk()
    {
        return 0;
    }

    // 0x8D8
    inline int CChrCore::MethdReboundDunk()
    {
        return 0;
    }

    // 0x8DC
    inline int CChrCore::CanbeLayUp()
    {
        return 0;
    }

    // 0x8E0
    inline int CChrCore::MethdLayUp()
    {
        return 0;
    }

    // 0x8E4
    inline int CChrCore::MethdLayUp(int)
    {
        return 0;
    }

    // 0x900
    inline int CChrCore::CanbeItemBallAttack()
    {
        return 0;
    }

    // 0x904
    inline int CChrCore::MethdItemBallAttack()
    {
        return 0;
    }

    // 0x90C
    inline int CChrCore::MethdPass(float, int)
    {
        return 0;
    }

    // 0x914
    inline int CChrCore::CanbePassIdx(long)
    {
        return 0;
    }

    // 0x918
    inline int CChrCore::MethdPassIdx(long)
    {
        return 0;
    }

    // 0x91C
    inline int CChrCore::CanbeDash() const
    {
        return 0;
    }

    // 0x920
    inline int CChrCore::MethdDash()
    {
        return 0;
    }

    // 0x924
    inline int CChrCore::CanbeJumpPass()
    {
        return 0;
    }

    // 0x928
    inline int CChrCore::MethdJumpPass(long)
    {
        return 0;
    }

    // 0x92C
    inline int CChrCore::CanbeJumpCatch()
    {
        return 0;
    }

    // 0x930
    inline int CChrCore::MethdJumpCatch(float)
    {
        return 0;
    }

    // 0x934
    inline int CChrCore::CanbeDribbleMiss()
    {
        return 0;
    }

    // 0x938
    inline int CChrCore::MethdDribbleMiss(float)
    {
        return 0;
    }

    // 0x93C
    inline int CChrCore::CanbeAerialPass(CChrCore *)
    {
        return 0;
    }

    // 0x940
    inline int CChrCore::MethdAerialPass(float, int)
    {
        return 0;
    }

    // 0x944
    inline int CChrCore::CanbeAerialPassIdx(long)
    {
        return 0;
    }

    // 0x948
    inline int CChrCore::MethdAerialPassIdx(long)
    {
        return 0;
    }

    // 0x964
    inline int CChrCore::MethdDamage(unsigned long, float, float, float, long, float, long, long, long, long)
    {
        return 0;
    }

    // 0x968
    inline int CChrCore::CanbeUseItem()
    {
        return 0;
    }

    // 0x96C
    inline int CChrCore::MethdUseItem()
    {
        return 0;
    }

    // 0x98C
    inline int CChrCore::MethdChgLeader(float, long, long, int)
    {
        return 0;
    }

    // 0x998
    inline int CChrCore::CanbeServe(long)
    {
        return 0;
    }

    // 0x99C
    inline int CChrCore::MethdServe(long)
    {
        return 0;
    }

    // 0x9B8
    inline int CChrCore::CanbeAutoSpikeJump() const
    {
        return 0;
    }

    // 0x9BC
    inline int CChrCore::Canbe1on1()
    {
        return 0;
    }

    // 0x9C0
    inline int CChrCore::Methd1on1(int)
    {
        return 0;
    }

    // 0x9C4
    inline int CChrCore::CanbeAutoDPM()
    {
        return 0;
    }

    // 0x9C8
    inline int CChrCore::MethdAutoDPM(float, float)
    {
        return 0;
    }

    // 0x9CC
    inline int CChrCore::CanbeBlockWorkLose()
    {
        return 0;
    }

    // 0x9D0
    inline int CChrCore::MethdBlockWorkLose()
    {
        return 0;
    }

    // 0x9D4
    inline int CChrCore::CanbeJumpBlock()
    {
        return 0;
    }

    // 0x9D8
    inline int CChrCore::MethdJumpBlock(float)
    {
        return 0;
    }

    // 0x9DC
    inline int CChrCore::CanbeSwatIt() const
    {
        return 0;
    }

    // 0x9E0
    inline int CChrCore::MethdSwatIt()
    {
        return 0;
    }

    // 0x9E4
    inline int CChrCore::CanbeSmashCut()
    {
        return 0;
    }

    // 0x9E8
    inline int CChrCore::MethdSmashCut(float)
    {
        return 0;
    }

    // 0x9EC
    inline int CChrCore::MethdAirTrickDownSpeed(float)
    {
        return 0;
    }

    // 0x9F0
    inline int CChrCore::CanbeSpecialBattle()
    {
        return 0;
    }

    // 0x9F4
    inline int CChrCore::MethdSpecialBattle()
    {
        return 0;
    }

    // 0x9F8
    inline int CChrCore::CanbeArtsShotCatch()
    {
        return 0;
    }

    // 0x9FC
    inline int CChrCore::MethdArtsShotCatch()
    {
        return 0;
    }

    // 0xA00
    inline int CChrCore::CanbeBallCatch()
    {
        return 0;
    }

    // 0xA04
    inline int CChrCore::MethdBallCatch(float)
    {
        return 0;
    }

    // 0xA08
    inline int CChrCore::CanbeLineOver()
    {
        return 0;
    }

    // 0xA0C
    inline int CChrCore::MethdLineOver(float)
    {
        return 0;
    }

    // 0xA10
    inline int CChrCore::CanbeCrouch()
    {
        return 0;
    }

    // 0xA14
    inline int CChrCore::MethdCrouch()
    {
        return 0;
    }

    // 0xA18
    inline int CChrCore::CanbeSkate(int)
    {
        return 0;
    }

    // 0xA1C
    inline int CChrCore::MethdSkate(float, int, float, int)
    {
        return 0;
    }

    // 0xA20
    inline int CChrCore::CanbeStealStance()
    {
        return 0;
    }

    // 0xA24
    inline int CChrCore::MethdStealStance(float)
    {
        return 0;
    }

    // 0xA30
    inline int CChrCore::CanbeBrake()
    {
        return 0;
    }

    // 0xA34
    inline int CChrCore::MethdBrake(float, long)
    {
        return 0;
    }

    // 0xA38
    inline int CChrCore::CanbeSwitchBrake(float, int)
    {
        return 0;
    }

    // 0xA3C
    inline int CChrCore::MethdSwitchBrake(float, float)
    {
        return 0;
    }

    // 0xA40
    inline int CChrCore::CanbeSwitch()
    {
        return 0;
    }

    // 0xA44
    inline int CChrCore::MethdSwitch()
    {
        return 0;
    }

    // 0xA48
    inline int CChrCore::CanbeBlockStance()
    {
        return 0;
    }

    // 0xA4C
    inline int CChrCore::MethdBlockStance(float, float, long)
    {
        return 0;
    }

    // 0xA50
    inline int CChrCore::CanbeAutoBlockStance()
    {
        return 0;
    }

    // 0xA54
    inline int CChrCore::MethdAutoBlockStance(float, float, long)
    {
        return 0;
    }

    // 0xA58
    inline int CChrCore::CanbeAutoChangeStance()
    {
        return 0;
    }

    // 0xA5C
    inline int CChrCore::MethdAutoChangeStance(float, float, long, float)
    {
        return 0;
    }

    // 0xA60
    inline int CChrCore::MethdArtsDamage(unsigned long, bcs::FVEC3)
    {
        return 0;
    }

    // 0xA64
    inline int CChrCore::CanbeChangeStance()
    {
        return 0;
    }

    // 0xA68
    inline int CChrCore::MethdChangeStance(float, float, long, float)
    {
        return 0;
    }

    // 0xA6C
    inline int CChrCore::CanbePassCatchState()
    {
        return 0;
    }

    // 0xA70
    inline int CChrCore::CanbeAlleyoop()
    {
        return 0;
    }

    // 0xA74
    inline int CChrCore::MethdSetPassCatchState(CBall *, int, const bcs::FVEC3 &, long)
    {
        return 0;
    }

    // 0xA78
    inline int CChrCore::MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &, float)
    {
        return 0;
    }

    // 0xA7C
    inline int CChrCore::MethdAlleyoop()
    {
        return 0;
    }

    // 0xA80
    inline int CChrCore::MethdSetAlleyoopCatchState(float, float, const bcs::FVEC3 &, const bcs::FVEC3 &)
    {
        return 0;
    }

    // 0xA84
    inline void CChrCore::GetTpInputDribble(INPUT_WORK &, float, float)
    {
    }

    // 0xA90
    inline int CChrCore::CanbeSubShoot()
    {
        return 0;
    }

    // 0xA94
    inline int CChrCore::MethdSubShoot(float)
    {
        return 0;
    }

    // 0xA98
    inline int CChrCore::CanbeNormalShoot()
    {
        return 0;
    }

    // 0xA9C
    inline int CChrCore::MethdNormalShoot(float)
    {
        return 0;
    }

    // 0xAA0
    inline int CChrCore::CanbePowerShot()
    {
        return 0;
    }

    // 0xAA4
    inline int CChrCore::MethdPowerShot(float)
    {
        return 0;
    }

    // 0xAA8
    inline int CChrCore::CanbeSideShot()
    {
        return 0;
    }

    // 0xAAC
    inline int CChrCore::MethdSideShot(float)
    {
        return 0;
    }

    // 0xAB0
    inline int CChrCore::CanbeDivingCatch()
    {
        return 0;
    }

    // 0xAB4
    inline int CChrCore::MethdDivingCatch(float)
    {
        return 0;
    }

    // 0xAB8
    inline int CChrCore::CanbeSpinAttack()
    {
        return 0;
    }

    // 0xABC
    inline int CChrCore::MethdSpinAttack()
    {
        return 0;
    }

    // 0xAC8
    inline int CChrCore::CanbePassStance()
    {
        return 0;
    }

    // 0xACC
    inline int CChrCore::MethdPassStance(float, long)
    {
        return 0;
    }

    // 0xAD0
    inline int CChrCore::CanbeDirectShootStance()
    {
        return 0;
    }

    // 0xAD4
    inline int CChrCore::MethdDirectShootStance(float, long)
    {
        return 0;
    }

    // 0xAD8
    inline int CChrCore::CanbeDirectPassStance()
    {
        return 0;
    }

    // 0xADC
    inline int CChrCore::MethdDirectPassStance(float, long)
    {
        return 0;
    }

    // 0xAE0
    inline int CChrCore::CanbePassChgLeader()
    {
        return 0;
    }

    // 0xAE4
    inline int CChrCore::MethdPassChgLeader()
    {
        return 0;
    }

    // 0xAE8
    inline int CChrCore::CanbeHockeyTrickMove()
    {
        return 0;
    }

    // 0xAEC
    inline int CChrCore::MethdHockeyTrickMove(float, long)
    {
        return 0;
    }

    // 0xAF0
    inline int CChrCore::CanbeTackleGuard()
    {
        return 0;
    }

    // 0xAF4
    inline int CChrCore::MethdTackleGuard(float, int, int)
    {
        return 0;
    }

    // 0xAF8
    inline int CChrCore::CanbeSmashAttack()
    {
        return 0;
    }

    // 0xAFC
    inline int CChrCore::MethdSmashAttack(float)
    {
        return 0;
    }

    // 0xB00
    inline int CChrCore::CanbeBallScramble(CBall *)
    {
        return 0;
    }

    // 0xB04
    inline int CChrCore::MethdBallScramble(CBall *, long, long)
    {
        return 0;
    }

    // 0xB08
    inline int CChrCore::CanbeDivingSteal(int)
    {
        return 0;
    }

    // 0xB0C
    inline int CChrCore::MethdDivingSteal(float, int, int)
    {
        return 0;
    }

    // 0xB10
    inline int CChrCore::CanbeBodyContact()
    {
        return 0;
    }

    // 0xB14
    inline int CChrCore::MethdBodyContact(float, long, int)
    {
        return 0;
    }

    // 0xB18
    inline int CChrCore::CanbeOffBallStance()
    {
        return 0;
    }

    // 0xB1C
    inline int CChrCore::MethdOffBallStance(float, int)
    {
        return 0;
    }

    // 0xB20
    inline int CChrCore::CanbeBlockAttack()
    {
        return 0;
    }

    // 0xB24
    inline int CChrCore::MethdBlockAttack(int)
    {
        return 0;
    }

    // 0xB28
    inline int CChrCore::CanbeTackleAttack()
    {
        return 0;
    }

    // 0xB2C
    inline int CChrCore::MethdTackleAttack(float)
    {
        return 0;
    }

    // 0xB30
    inline int CChrCore::CanbeAttackFight()
    {
        return 0;
    }

    // 0xB34
    inline int CChrCore::MethdAttackFight(long, int)
    {
        return 0;
    }

    // 0xB38
    inline int CChrCore::CanbePassCatch()
    {
        return 0;
    }

    // 0xB3C
    inline int CChrCore::MethdPassCatch(long, const bcs::FVEC3 &, float)
    {
        return 0;
    }

    // 0xB40
    inline int CChrCore::CanbeFreeShoot()
    {
        return 0;
    }

    // 0xB44
    inline int CChrCore::MethdbFreeShoot(float)
    {
        return 0;
    }

    // 0xB48
    inline int CChrCore::CanbeFeintShoot()
    {
        return 0;
    }

    // 0xB4C
    inline int CChrCore::MethdFeintShoot(float)
    {
        return 0;
    }

    // 0xB50
    inline int CChrCore::CanbeTreadOnQPanel(const CQPanel *)
    {
        return 1;
    }

    // 0xB54
    inline int CChrCore::CanbeItemBallShoot()
    {
        return 1;
    }

    // 0xB58
    inline int CChrCore::MethdItemBallShoot(float)
    {
        return 1;
    }

    // 0xB5C
    inline int CChrCore::CanbeCombination() const
    {
        return 0;
    }

    // 0xB60
    inline int CChrCore::MethdCombination()
    {
        return 0;
    }

    // 0xB64
    inline int CChrCore::CanbeLanding() const
    {
        return 1;
    }

    // 0xB68
    inline int CChrCore::CanbeFall() const
    {
        return 0;
    }

    // 0xB6C
    inline int CChrCore::MethdFall()
    {
        return 0;
    }

    // 0xB70
    inline int CChrCore::CanbeRebirth() const
    {
        return 0;
    }

    // 0xB74
    inline int CChrCore::MethdRebirth()
    {
        return 0;
    }

    // 0xC60
    inline void CChrCore::CallbackBallSnatched_(CBall *)
    {
    }

    // 0xC80
    inline void CChrCore::CallbackMotTrg_AtkOn_(long, long)
    {
    }

    // 0xC84
    inline void CChrCore::CallbackMotTrg_AtkOff_(long, long)
    {
    }

}

#endif
