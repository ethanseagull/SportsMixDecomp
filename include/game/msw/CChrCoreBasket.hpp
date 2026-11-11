#ifndef MSW_CCHRCOREBASKET_HPP_
#define MSW_CCHRCOREBASKET_HPP_

#include "game/msw/CChrCore.hpp"

namespace msw {

    class CBall;

    // The basketball character core (BasketF).
    //
    // INCOMPLETE: only what game/basket/CComDrvBasket.cpp calls is
    // declared. CChrCore is incomplete too, so the class must not be
    // instantiated or have its vtable emitted yet.
    class CChrCoreBasket : public CChrCore {
        public:
            virtual ~CChrCoreBasket();

            // Not zero when pChr (holding the ball) is in reach of a cut
            // (steal) by this character. (meaning guessed from the name)
            int CutHitCheck(const CChrCore *pChr) const;

            // Added for game/basket/CChrDrvPadBasket.cpp (BasketF, all in
            // this module; return types guessed). Not zero when the
            // character takes a pass from a partner automatically.
            int IsAutoChrChangeToPassCatch() const;
            // Starts the combination pass in direction fRotY; zero when it
            // cannot.
            int MethdCombinationPass(float fRotY);

            // Slots 0x978 to 0xA7C of __vt__Q23msw14CChrCoreBasket. They are
            // CChrCore's virtual functions (the DOL names many of them
            // CChrCore::), declared here rather than in CChrCore.hpp so that
            // the placeholder slots of other modules' derived classes
            // (CChrCoreDodge, CChrCoreBoss) do not move. CChrCoreBasket adds
            // nothing in front of them, so the offsets are the same.
            virtual int MethdGameSet();                                // 0x978
            virtual int MethdGameResult(int);                          // 0x97C
            virtual int CanbeChgLeaderState();                         // 0x980
            virtual int CanbeChgNewLeaderState();                      // 0x984
            virtual int CanbeChgLeader(CChrCore *);                    // 0x988
            virtual int MethdChgLeader(float, long, long, int);        // 0x98C
            virtual int MethdChgLeader(long, long, long);              // 0x990
            virtual int MethdChgLeader();                              // 0x994
            virtual int CanbeServe(long);                              // 0x998
            virtual int MethdServe(long);                              // 0x99C
            virtual int CanbeReceive(int);                             // 0x9A0
            virtual int CanbeDiveReceive();                            // 0x9A4
            virtual int MethdReceive(int, int);                        // 0x9A8
            virtual int MethdReceive(int, int, float);                 // 0x9AC
            virtual int CanbeSpike(int) const;                         // 0x9B0
            virtual int MethdSpike(int);                               // 0x9B4
            virtual int CanbeAutoSpikeJump() const;                    // 0x9B8
            virtual int Canbe1on1();                                   // 0x9BC
            virtual int Methd1on1(int);                                // 0x9C0
            virtual int CanbeAutoDPM();                                // 0x9C4
            virtual int MethdAutoDPM(float, float);                    // 0x9C8
            virtual int CanbeBlockWorkLose();                          // 0x9CC
            virtual int MethdBlockWorkLose();                          // 0x9D0
            virtual int CanbeJumpBlock();                              // 0x9D4
            virtual int MethdJumpBlock(float);                         // 0x9D8
            virtual int CanbeSwatIt() const;                           // 0x9DC
            virtual int MethdSwatIt();                                 // 0x9E0
            virtual int CanbeSmashCut();                               // 0x9E4
            virtual int MethdSmashCut(float);                          // 0x9E8
            virtual int MethdAirTrickDownSpeed(float);                 // 0x9EC
            virtual int CanbeSpecialBattle();                          // 0x9F0
            virtual int MethdSpecialBattle();                          // 0x9F4
            virtual int CanbeArtsShotCatch();                          // 0x9F8
            virtual int MethdArtsShotCatch();                          // 0x9FC
            virtual int CanbeBallCatch();                              // 0xA00
            virtual int MethdBallCatch(float);                         // 0xA04
            virtual int CanbeLineOver();                               // 0xA08
            virtual int MethdLineOver(float);                          // 0xA0C
            virtual int CanbeCrouch();                                 // 0xA10
            virtual int MethdCrouch();                                 // 0xA14
            virtual int CanbeSkate(int);                               // 0xA18
            virtual int MethdSkate(float, int, float, int);            // 0xA1C
            virtual int CanbeStealStance();                            // 0xA20
            virtual int MethdStealStance(float);                       // 0xA24
            virtual int CanbeShootStance();                            // 0xA28
            virtual int MethdShootStance(float, int);                  // 0xA2C
            virtual int CanbeBrake();                                  // 0xA30
            virtual int MethdBrake(float, long);                       // 0xA34
            virtual int CanbeSwitchBrake(float, int);                  // 0xA38
            virtual int MethdSwitchBrake(float, float);                // 0xA3C
            virtual int CanbeSwitch();                                 // 0xA40
            virtual int MethdSwitch();                                 // 0xA44
            virtual int CanbeBlockStance();                            // 0xA48
            virtual int MethdBlockStance(float, float, long);          // 0xA4C
            virtual int CanbeAutoBlockStance();                        // 0xA50
            virtual int MethdAutoBlockStance(float, float, long);      // 0xA54
            virtual int CanbeAutoChangeStance();                       // 0xA58
            virtual int MethdAutoChangeStance(float, float, long, float); // 0xA5C
            virtual int MethdArtsDamage(unsigned long, bcs::FVEC3);    // 0xA60
            virtual int CanbeChangeStance();                           // 0xA64
            virtual int MethdChangeStance(float, float, long, float);  // 0xA68
            virtual int CanbePassCatchState();                         // 0xA6C
            virtual int CanbeAlleyoop();                               // 0xA70
            virtual int MethdSetPassCatchState(CBall *, int, const bcs::FVEC3 &, long); // 0xA74
            virtual int MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &, float); // 0xA78
            virtual int MethdAlleyoop();                               // 0xA7C
            // Slots 0xA80 to 0xD48 are not mapped yet; placeholders (the
            // comment names each slot's function) keep GetFuturePos at 0xD4C.
            virtual void UnkA80(); // 0xA80 CChrCore::MethdSetAlleyoopCatchState(float, float, const bcs::FVEC3&, const bcs::FVEC3&)
            virtual void UnkA84(); // 0xA84 CChrCore::GetTpInputDribble(msw::CChrCore::INPUT_WORK&, float, float)
            virtual void UnkA88(); // 0xA88 CChrCoreBasket::GetArtsEventRot()
            virtual void UnkA8C(); // 0xA8C CChrCoreBasket::DoPlayArtsEvent()
            virtual void UnkA90(); // 0xA90 CChrCore::CanbeSubShoot()
            virtual void UnkA94(); // 0xA94 CChrCore::MethdSubShoot(float)
            virtual void UnkA98(); // 0xA98 CChrCore::CanbeNormalShoot()
            virtual void UnkA9C(); // 0xA9C CChrCore::MethdNormalShoot(float)
            virtual void UnkAA0(); // 0xAA0 CChrCore::CanbePowerShot()
            virtual void UnkAA4(); // 0xAA4 CChrCore::MethdPowerShot(float)
            virtual void UnkAA8(); // 0xAA8 CChrCore::CanbeSideShot()
            virtual void UnkAAC(); // 0xAAC CChrCore::MethdSideShot(float)
            virtual void UnkAB0(); // 0xAB0 CChrCore::CanbeDivingCatch()
            virtual void UnkAB4(); // 0xAB4 CChrCore::MethdDivingCatch(float)
            virtual void UnkAB8(); // 0xAB8 CChrCore::CanbeSpinAttack()
            virtual void UnkABC(); // 0xABC CChrCore::MethdSpinAttack()
            virtual void UnkAC0(); // 0xAC0 CChrCore::CanbeEventDoll()
            virtual void UnkAC4(); // 0xAC4 CChrCore::MethdEventDoll()
            virtual void UnkAC8(); // 0xAC8 CChrCore::CanbePassStance()
            virtual void UnkACC(); // 0xACC CChrCore::MethdPassStance(float, long)
            virtual void UnkAD0(); // 0xAD0 CChrCore::CanbeDirectShootStance()
            virtual void UnkAD4(); // 0xAD4 CChrCore::MethdDirectShootStance(float, long)
            virtual void UnkAD8(); // 0xAD8 CChrCore::CanbeDirectPassStance()
            virtual void UnkADC(); // 0xADC CChrCore::MethdDirectPassStance(float, long)
            virtual void UnkAE0(); // 0xAE0 CChrCore::CanbePassChgLeader()
            virtual void UnkAE4(); // 0xAE4 CChrCore::MethdPassChgLeader()
            virtual void UnkAE8(); // 0xAE8 CChrCore::CanbeHockeyTrickMove()
            virtual void UnkAEC(); // 0xAEC CChrCore::MethdHockeyTrickMove(float, long)
            virtual void UnkAF0(); // 0xAF0 CChrCore::CanbeTackleGuard()
            virtual void UnkAF4(); // 0xAF4 CChrCore::MethdTackleGuard(float, int, int)
            virtual void UnkAF8(); // 0xAF8 CChrCore::CanbeSmashAttack()
            virtual void UnkAFC(); // 0xAFC CChrCore::MethdSmashAttack(float)
            virtual void UnkB00(); // 0xB00 CChrCore::CanbeBallScramble(msw::CBall*)
            virtual void UnkB04(); // 0xB04 CChrCore::MethdBallScramble(msw::CBall*, long, long)
            virtual void UnkB08(); // 0xB08 CChrCore::CanbeDivingSteal(int)
            virtual void UnkB0C(); // 0xB0C CChrCore::MethdDivingSteal(float, int, int)
            virtual void UnkB10(); // 0xB10 CChrCore::CanbeBodyContact()
            virtual void UnkB14(); // 0xB14 CChrCore::MethdBodyContact(float, long, int)
            virtual void UnkB18(); // 0xB18 CChrCore::CanbeOffBallStance()
            virtual void UnkB1C(); // 0xB1C CChrCore::MethdOffBallStance(float, int)
            virtual void UnkB20(); // 0xB20 CChrCore::CanbeBlockAttack()
            virtual void UnkB24(); // 0xB24 CChrCore::MethdBlockAttack(int)
            virtual void UnkB28(); // 0xB28 CChrCore::CanbeTackleAttack()
            virtual void UnkB2C(); // 0xB2C CChrCore::MethdTackleAttack(float)
            virtual void UnkB30(); // 0xB30 CChrCore::CanbeAttackFight()
            virtual void UnkB34(); // 0xB34 CChrCore::MethdAttackFight(long, int)
            virtual void UnkB38(); // 0xB38 CChrCore::CanbePassCatch()
            virtual void UnkB3C(); // 0xB3C CChrCore::MethdPassCatch(long, const bcs::FVEC3&, float)
            virtual void UnkB40(); // 0xB40 CChrCore::CanbeFreeShoot()
            virtual void UnkB44(); // 0xB44 CChrCore::MethdbFreeShoot(float)
            virtual void UnkB48(); // 0xB48 CChrCore::CanbeFeintShoot()
            virtual void UnkB4C(); // 0xB4C CChrCore::MethdFeintShoot(float)
            virtual void UnkB50(); // 0xB50 CChrCore::CanbeTreadOnQPanel(const msw::CQPanel*)
            virtual void UnkB54(); // 0xB54 CChrCore::CanbeItemBallShoot()
            virtual void UnkB58(); // 0xB58 CChrCore::MethdItemBallShoot(float)
            virtual void UnkB5C(); // 0xB5C CChrCoreBasket::CanbeCombination() const
            virtual void UnkB60(); // 0xB60 CChrCoreBasket::MethdCombination()
            virtual void UnkB64(); // 0xB64 CChrCoreBasket::CanbeLanding() const
            virtual void UnkB68(); // 0xB68 CChrCoreBasket::CanbeFall() const
            virtual void UnkB6C(); // 0xB6C CChrCoreBasket::MethdFall()
            virtual void UnkB70(); // 0xB70 CChrCoreBasket::CanbeRebirth() const
            virtual void UnkB74(); // 0xB74 CChrCoreBasket::MethdRebirth()
            virtual void UnkB78(); // 0xB78 CChrCore::ArtsOptAliveCheck_()
            virtual void UnkB7C(); // 0xB7C CChrCore::GetArtsOpt() const
            virtual void UnkB80(); // 0xB80 CChrCore::GetArtsOptMdl() const
            virtual void UnkB84(); // 0xB84 CChrCore::ArtsOptCreate(int, int, int)
            virtual void UnkB88(); // 0xB88 CChrCore::ArtsOptDestroy()
            virtual void UnkB8C(); // 0xB8C CChrCore::ArtsOptSetDisp(int)
            virtual void UnkB90(); // 0xB90 CChrCore::ArtsOptFadeIn(float, float, int)
            virtual void UnkB94(); // 0xB94 CChrCore::ArtsOptFadeOut(float, float, int)
            virtual void UnkB98(); // 0xB98 CChrCore::ArtsOptSetFadeInSE(long)
            virtual void UnkB9C(); // 0xB9C CChrCore::ArtsOptSetFadeOutSE(long)
            virtual void UnkBA0(); // 0xBA0 CChrCore::ArtsOptSetAliveSE(long)
            virtual void UnkBA4(); // 0xBA4 CChrCore::ArtsOptGetNodeIdx(const char*) const
            virtual void UnkBA8(); // 0xBA8 CChrCore::ArtsOptAnimPlay(long, int, float, float)
            virtual void UnkBAC(); // 0xBAC CChrCore::ArtsOptAnimStop()
            virtual void UnkBB0(); // 0xBB0 CChrCore::ArtsOptAnimSetSpeed(float)
            virtual void UnkBB4(); // 0xBB4 CChrCore::ArtsOptAnimGetFrame() const
            virtual void UnkBB8(); // 0xBB8 CChrCore::ArtsOptAnimGetFrameNum() const
            virtual void UnkBBC(); // 0xBBC CChrCore::ArtsOptAnimGetResFrameNum(long) const
            virtual void UnkBC0(); // 0xBC0 CChrCore::ArtsOptAnimGetMotID() const
            virtual void UnkBC4(); // 0xBC4 CChrCore::ArtsOptAnimSetFrame(float)
            virtual void UnkBC8(); // 0xBC8 CChrCore::ArtsOptAnimResetLoopedFlag()
            virtual void UnkBCC(); // 0xBCC CChrCore::ArtsOptIsAnimLoop() const
            virtual void UnkBD0(); // 0xBD0 CChrCore::ArtsOptIsAnimLooped() const
            virtual void UnkBD4(); // 0xBD4 CChrCore::ArtsOptIsAnimEnd() const
            virtual void UnkBD8(); // 0xBD8 CChrCore::ArtsOptIsAnimInter() const
            virtual void UnkBDC(); // 0xBDC CChrCore::ArtsOptIsAnimResLoopType(long) const
            virtual void UnkBE0(); // 0xBE0 CChrCore::ArtsOptCreateAttachEffect(long, long, unsigned long, const bcs::FVEC3*, unsigned long, unsigned long)
            virtual void UnkBE4(); // 0xBE4 CChrCore::ArtsOptCreateAttachEffect(long, long, const char*, const bcs::FVEC3*, unsigned long, unsigned long)
            virtual void UnkBE8(); // 0xBE8 CChrCore::ArtsOptCreateAttachEffectAuto(long, long, unsigned long)
            virtual void UnkBEC(); // 0xBEC CChrCore::ArtsOptGetEffectHandle(long) const
            virtual void UnkBF0(); // 0xBF0 CChrCore::ArtsOptSetEffectSpeed(long, float)
            virtual void UnkBF4(); // 0xBF4 CChrCore::ArtsOptIsEffectAlive(long) const
            virtual void UnkBF8(); // 0xBF8 CChrCore::ArtsOptStopEffect(long)
            virtual void UnkBFC(); // 0xBFC CChrCore::ArtsOptEraseEffect(long)
            virtual void UnkC00(); // 0xC00 CChrCore::ArtsOptStopEffectAll()
            virtual void UnkC04(); // 0xC04 CChrCore::ArtsOptEraseEffectAll()
            virtual void UnkC08(); // 0xC08 CChrCore::ArtsOptCreate(long, long, long, long)
            virtual void UnkC0C(); // 0xC0C CChrCore::Util_DropCoin(float, long, long)
            virtual void UnkC10(); // 0xC10 CChrCore::CheckCountWork__()
            virtual void UnkC14(); // 0xC14 CChrCore::CheckWater_(int)
            virtual void UnkC18(); // 0xC18 CChrCore::CalcMoveSpeedRate_(float, long)
            virtual void UnkC1C(); // 0xC1C CChrCore::Util_SetEffect(msw::CChrCore::SET_EFFECT_TYPE)
            virtual void UnkC20(); // 0xC20 CChrCore::Util_AttachEffect(msw::CChrCore::ATTACH_EFFECT_TYPE, long)
            virtual void UnkC24(); // 0xC24 CChrCore::Util_DetachEffectAll()
            virtual void UnkC28(); // 0xC28 CChrCore::Util_DetachEffect(msw::CChrCore::ATTACH_EFFECT_TYPE)
            virtual void UnkC2C(); // 0xC2C CChrCore::Util_DetachEffect_(unsigned long&)
            virtual void UnkC30(); // 0xC30 CChrCore::Util_EraseEffectAll()
            virtual void UnkC34(); // 0xC34 CChrCore::Util_EraseEffect(msw::CChrCore::ATTACH_EFFECT_TYPE)
            virtual void UnkC38(); // 0xC38 CChrCore::Util_EraseEffect_(unsigned long&)
            virtual void UnkC3C(); // 0xC3C CChrCore::CallbackDropCoinPickup_(const msw::CDropCoin*)
            virtual void UnkC40(); // 0xC40 CChrCore::CallbackItemPickup_(const msw::CItem*)
            virtual void UnkC44(); // 0xC44 CChrCoreBasket::CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG*)
            virtual void UnkC48(); // 0xC48 CChrCore::CallbackDribbleTap_()
            virtual void UnkC4C(); // 0xC4C CChrCoreBasket::CallbackBodyDamage_(const msw::CCollAtkUnit*, const bcs::FVEC3&)
            virtual void UnkC50(); // 0xC50 CChrCoreBasket::CallbackBallDamage_(msw::CBall*, const msw::CCollAtkUnit*, const bcs::FVEC3&)
            virtual void UnkC54(); // 0xC54 CChrCoreBasket::CallbackBodyPush_(msw::CCollBodyUnit*, msw::CCollBodyUnit*, bcs::FVEC3*)
            virtual void UnkC58(); // 0xC58 CChrCoreBasket::CallbackBallCatch_(msw::CBall*)
            virtual void UnkC5C(); // 0xC5C CChrCoreBasket::CallbackAttackHit_(const msw::CCollAtkUnit*, const msw::CCollBodyUnit*, const bcs::FVEC3&, unsigned long)
            virtual void UnkC60(); // 0xC60 CChrCoreBasket::CallbackBallSnatched_(msw::CBall*)
            virtual void UnkC64(); // 0xC64 CChrCore::CallbackDefReflectHit_(const msw::CCollDefUnit*, const msw::CBall*, bcs::FVEC3&, bcs::FVEC3&)
            virtual void UnkC68(); // 0xC68 CChrCore::CallbackDefObstructHit_(const msw::CCollDefUnit*, const msw::CBall*, bcs::FVEC3&, bcs::FVEC3&)
            virtual void UnkC6C(); // 0xC6C CChrCoreBasket::CallbackDefReflectHit_(const msw::CCollDefUnit*, const msw::CBall*, const bcs::FVEC3&)
            virtual void UnkC70(); // 0xC70 CChrCoreBasket::CallbackMotTrg_SeFoot_(long, long)
            virtual void UnkC74(); // 0xC74 CChrCore::CallbackMotTrg_SeLand_(long, long)
            virtual void UnkC78(); // 0xC78 CChrCore::CallbackMotTrg_SeJump_(long, long)
            virtual void UnkC7C(); // 0xC7C CChrCore::CallbackMotTrg_BodyVisible_(long, long)
            virtual void UnkC80(); // 0xC80 CChrCore::CallbackMotTrg_AtkOn_(long, long)
            virtual void UnkC84(); // 0xC84 CChrCore::CallbackMotTrg_AtkOff_(long, long)
            virtual void UnkC88(); // 0xC88 CChrCore::SubStateDamageCtorMame_(msw::CChrCore::STATE_WORK_DAMAGE*)
            virtual void UnkC8C(); // 0xC8C CChrCore::SubStateDamageLoopMame_(msw::CChrCore::STATE_WORK_DAMAGE*)
            virtual void UnkC90(); // 0xC90 CChrCore::SubStateParalyzeStateSet_(unsigned long, float)
            virtual void UnkC94(); // 0xC94 CChrCoreBasket::SubStateParalyzeSetEffect_()
            virtual void UnkC98(); // 0xC98 CChrCoreBasket::SubStateParalyzeEndEffect_()
            virtual void UnkC9C(); // 0xC9C CChrCoreBasket::GetCcParam_() const
            virtual void UnkCA0(); // 0xCA0 CChrCoreBasket::NewCcParam2_(long) const
            virtual void UnkCA4(); // 0xCA4 CChrCoreBasket::GetCcParam2_() const
            virtual void UnkCA8(); // 0xCA8 CChrCoreBasket::GetSpSetBackPos(bcs::FVEC3&) const
            virtual void UnkCAC(); // 0xCAC CChrCoreBasket::UpDateCollBody3_()
            virtual void UnkCB0(); // 0xCB0 CChrCoreBasket::UpdateCollAtk_()
            virtual void UnkCB4(); // 0xCB4 CChrCoreBasket::SubStateArtsShotStartTrick_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCB8(); // 0xCB8 CChrCoreBasket::SubStateArtsShotSetTrickFrameParam_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*, long)
            virtual void UnkCBC(); // 0xCBC CChrCoreBasket::SubStateArtsShotSetTrickJumpParam_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*, long)
            virtual void UnkCC0(); // 0xCC0 CChrCoreBasket::SubStateArtsShotSetTrickMoveParam_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*, long)
            virtual void UnkCC4(); // 0xCC4 CChrCoreBasket::SubStateArtsShotGetTrickMotFrame_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*, long)
            virtual void UnkCC8(); // 0xCC8 CChrCoreBasket::SubStateArtsShotGetTrickDoFrame_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*, long)
            virtual void UnkCCC(); // 0xCCC CChrCoreBasket::SubStateArtsShotGetTrickRotFrame_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCD0(); // 0xCD0 CChrCoreBasket::SubStateArtsShotIsReadyArtsShot_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCD4(); // 0xCD4 CChrCoreBasket::SubStateArtsShotIsStartArtsShot_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCD8(); // 0xCD8 CChrCoreBasket::SubStateArtsShotCalcRotParam_(msw::CChrCore::ROT_LERP1&, float, float)
            virtual void UnkCDC(); // 0xCDC CChrCoreBasket::SubStateArtsShotClearMoveParamWork_(msw::CC_DATA::CHR_PARAM::MOVE_PARAM&)
            virtual void UnkCE0(); // 0xCE0 CChrCoreBasket::SubStateArtsShotDoJumpMove_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCE4(); // 0xCE4 CChrCoreBasket::SubStateArtsShotFireBullet_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*, long)
            virtual void UnkCE8(); // 0xCE8 CChrCoreBasket::SubStateArtsShotPlayTrickMotion_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCEC(); // 0xCEC CChrCoreBasket::SubStateArtsShotReadyArtsShot_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCF0(); // 0xCF0 CChrCoreBasket::SubStateArtsShotPlayLoopMotion_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCF4(); // 0xCF4 CChrCoreBasket::SubStateArtsShotStartArtsShot_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCF8(); // 0xCF8 CChrCoreBasket::SubStateArtsShotPlayArtsShotMotion_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkCFC(); // 0xCFC CChrCoreBasket::SubStateArtsShotAskTargetMarkerMove_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD00(); // 0xD00 CChrCoreBasket::SubStateArtsShotIsEndArtsShot_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD04(); // 0xD04 CChrCoreBasket::SubStateArtsShotShootBall_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD08(); // 0xD08 CChrCoreBasket::SubStateArtsShotStartArtsHitback_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD0C(); // 0xD0C CChrCoreBasket::SubStateArtsShotIsEndArtsShotAfter_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD10(); // 0xD10 CChrCoreBasket::SubStateArtsShotDoLandStart_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD14(); // 0xD14 CChrCoreBasket::SubStateArtsShotPlayLandMotion_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD18(); // 0xD18 CChrCoreBasket::SubStateArtsShotPlayLandEffect_(msw::CChrCoreBasket::STATE_WORK_AIR_TRICK*)
            virtual void UnkD1C(); // 0xD1C CChrCoreBasket::CanbePassSign()
            virtual void UnkD20(); // 0xD20 CChrCoreBasket::MethdPassSign()
            virtual void UnkD24(); // 0xD24 CChrCoreBasket::CallbackBodyPush3_(msw::CCollBodyUnit*, msw::CCollBodyUnit*, bcs::FVEC3*)
            virtual void UnkD28(); // 0xD28 CChrCoreBasket::StartDunkBattle(msw::CChrCore*)
            virtual void UnkD2C(); // 0xD2C CChrCoreBasket::EndDunkBattle()
            virtual void UnkD30(); // 0xD30 CChrCoreBasket::CheckDunkBattle()
            virtual void UnkD34(); // 0xD34 CChrCoreBasket::GetColBodyRadius() const
            virtual void UnkD38(); // 0xD38 CChrCoreBasket::GetPush1Radius() const
            virtual void UnkD3C(); // 0xD3C CChrCoreBasket::ResultAirTrickBattle(int)
            virtual void UnkD40(); // 0xD40 CChrCoreBasket::IsDunkBattle()
            virtual void UnkD44(); // 0xD44 CChrCoreBasket::GetArtsShootType() const
            virtual void UnkD48(); // 0xD48 CChrCoreBasket::GetArtsFuturePos(float&, float&)
            // Added for BasketF game/basket/CNpcGhostBasketBase.cpp (return
            // type guessed): where the character will be after fFrame frames.
            virtual void GetFuturePos(bcs::FVEC3 &rPos, float fFrame); // 0xD4C

            // 0x57C (name guessed): flag bits; with 0x800 set a catch can be
            // followed by an aerial pass (CChrDrvPadBasket::MethdPass).
            unsigned char m_Unk574[0x57C - 0x574];
            unsigned long m_nBasketFlag;
    };
}

#endif /* !MSW_CCHRCOREBASKET_HPP_ */
