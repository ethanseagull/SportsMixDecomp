#ifndef MSW_CBALL_HPP_
#define MSW_CBALL_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "revolution/gx/GXEnum.h"
#include "revolution/mtx.h"

namespace nw4r {
    namespace g3d {
        class ResFile;
        class ResMdl;
    }
}

namespace bcs {
    class CMdlObj;
    class CAnmMotion;
    class CAnmMatClr;
    class CScnGroup;
    class CScnObj;
    struct BCS_Color;
}

namespace msw {

    class CChrCore;
    class CCollAtkUnit;
    class CItemBall;
    class CCollBodyUnit;
    class CAtkCollision;
    class CBodyCollision;

    // Arguments of the volleyball shots. (that BALL and VOLLEY are
    // namespaces and not classes is a guess, see notes/toolchain.md)
    namespace BALL {
        namespace VOLLEY {
            struct SERVE_TOSS_ARG;
            struct SERVE_SHOT_ARG;
            struct SPIKE_SHOT_ARG;
            struct PASS_SHOT_ARG;
            // (added for VolleyF game/volley/unit_63388.cpp; field names
            // and meanings guessed) What a special shot asks of the
            // character receiving it (CBall::GetVolleyEndureParam);
            // CChrCoreVolley::SetReceiveEndureWork_ copies it into the
            // receive state's work.
            // CBallVolley::GetVolleyEndureParam copies all 0x24 bytes.
            struct ENDURE_PRM {
                float m_fUnk00;
                float m_fUnk04;
                float m_fUnk08;
                float m_afUnk0C[3];
                long m_anUnk18[3];
            };
        }
        namespace BOSS {
            struct SPIKE_SHOT_ARG;
            struct PASS_SHOT_ARG;
        }
    }

    // The ball (or puck) of a game. A DOL class, size 0x318: bcs::CUnivTask
    // at 0x00, bcs::CFsmQueue<CBall> at 0x34 (vptr at 0x90), own members
    // from 0x94 (read from the constructor in the DOL and from
    // msw::CBallBoss's in BossF, whose own members start at 0x318). Each
    // game module derives its own ball from it.
    //
    // INCOMPLETE: only what game/boss/CBallManBoss.cpp needs is here.
    //  - All virtual functions up to slot 0x4F0 are declared. Their order
    //    and names are the slots of __vt__Q23msw9CBallBoss (BossF) and
    //    __vt__Q23msw10CBallDodge (DodgeF), read with the class name set
    //    back to CBall.
    //  - Of the functions without a body, the ones marked "DOL" are DOL
    //    functions; the others are inline in the original (the modules emit
    //    them) and their bodies are not written yet, so a unit that has to
    //    emit this vtable needs them first.
    //  - Return types are not part of the mangled names: all of them are
    //    guesses from the names, except those of the two functions with a
    //    body. The names of the parameters and members are guesses too.
    //  - The return types of the inline virtuals were checked against
    //    their bodies, which MenuF's game/menu/CTopMenuChrObj.cpp emits (the
    //    bodies are in game/msw/CBallInline.hpp): several that were guessed
    //    void return 0 or 0.0f, and GetShootStartPos returns a vector by
    //    value (BasketF's override too).
    class CBall : public bcs::CUnivTask, public bcs::CFsmQueue<CBall> {
        public:
            // Bits of m_nFlag.
            enum {
                // (name guessed) Read by CBallManDodge::IsBallScored_.
                FLAG_SCORED = 0x00000001,
                // (name guessed) Set by SetSleepMode; a ball with this bit
                // is skipped by everything that looks for a ball in play.
                FLAG_SLEEP = 0x00000002,
                // Set and cleared by SetDestroyOnReset.
                FLAG_DESTROY_ON_RESET = 0x00020000,
                // (name guessed) CBallMan::Main deletes a ball with this bit.
                FLAG_DELETE_REQUEST = 0x00040000
            };

            CBall(bcs::CTask *pParent, long nBallNo);
            // (DOL) The item behaviour attached to the ball (the task whose id
            // is at 0xD0), or null when there is none or it is being
            // deleted.
            CItemBall *GetItemEffect_() const;
            virtual ~CBall();                                                   // 0x08 DOL
            virtual void Main();                                                // 0x0C DOL
            virtual void Disp();                                                // 0x10 DOL

            virtual void RestoreColorReg();                                     // 0x24 DOL
            virtual void Create(nw4r::g3d::ResFile);                            // 0x28
            virtual void Reset();                                               // 0x2C
            virtual float GetRadius() const;                                    // 0x30
            virtual float GetBgCollRadius() const;                              // 0x34
            virtual float GetBgCollRadiusXZ() const;                            // 0x38
            virtual float GetCollPushRadiusDefault() const;                     // 0x3C
            virtual float GetCollBodyRadiusDefault() const;                     // 0x40
            virtual float GetCollCatchRadiusDefault() const;                    // 0x44
            virtual float GetGravityDefault() const;                            // 0x48
            virtual void ChangeState(long);                                     // 0x4C DOL
            virtual void *GetRadarIconTexObj() const;                           // 0x50 DOL
            virtual void *GetMinimapIconTexObj() const;                         // 0x54 DOL
            virtual int GetTargetPos(bcs::FVEC3 &) const;                       // 0x58 DOL (int: CComManVolley tests it)
            // (return type from HockeyF's CC06_HOCKEY::CBallHockeyWaluigi,
            // whose override returns 0)
            virtual long AttachItemEffect(long);                                // 0x5C DOL
            virtual void DetachItemEffect();                                    // 0x60 DOL
            virtual void SetPassMode(const bcs::FVEC3 &, long, float, long, long, int, int, unsigned long); // 0x64 DOL
            virtual void SetShootMode(long, float, long, long, unsigned long);  // 0x68 DOL
            virtual void SetShootMode(const bcs::FVEC3 &, long, long, unsigned long); // 0x6C DOL
            virtual void SetReShootMode(int);                                   // 0x70 DOL
            virtual bcs::FVEC3 GetShootStartPos() const;                 // 0x74
            virtual long GetShootCalcType() const;                              // 0x78
            virtual float GetShootCurveValue() const;                           // 0x7C
            virtual float CalcJumpShootVel(const bcs::FVEC3 &, const bcs::FVEC3 &, bcs::FVEC3 &, float &); // 0x80
            virtual void DoExplode(float, float);                               // 0x84 DOL
            virtual int IsAirBall() const;                                      // 0x88
            virtual void OnBaPad0Shot();                                        // 0x8C
            virtual void ResetBaPad0Shot();                                     // 0x90
            virtual int IsBaExplode() const;                                    // 0x94
            virtual int IsBaPad0Shot() const;                                   // 0x98
            virtual void SetDodgeShootMode(long, long, const bcs::FVEC3 &, long, long); // 0x9C
            virtual float SetDodgeShootMode(long, long, long, unsigned long);    // 0xA0
            virtual float SetDodgePowerShootMode(long, long, long, float, unsigned long); // 0xA4
            virtual void SetOutBallMode(const bcs::FVEC3 &, long, long);        // 0xA8
            virtual void SetFlickBallMode(float, float, float, float, long, long); // 0xAC
            virtual void SetDodgeReflectFreeMode(const bcs::FVEC3 &, float);    // 0xB0
            virtual long GetDodgeHitType(long) const;                           // 0xB4
            virtual void DodgeReattach(bcs::CMdlObj *, unsigned long);          // 0xB8
            virtual void DodgeOnPreTouch(long);                                 // 0xBC
            virtual void DodgeOnHit(long);                                      // 0xC0
            virtual void DodgeOnCatch(CChrCore *);                              // 0xC4
            virtual void DodgeOnRelease(CChrCore *);                            // 0xC8
            virtual long GetDodgeShootType() const;                             // 0xCC
            virtual void SetDodgeAddRange(float);                               // 0xD0
            virtual float GetDodgeAddRange() const;                             // 0xD4
            virtual void SetDodgeReceive(const bcs::FVEC3 &, float, float);     // 0xD8
            virtual void SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &); // 0xDC
            virtual int SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &); // 0xE0
            virtual int SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &); // 0xE4
            virtual void SetVolleyPassShotMode(BALL::VOLLEY::PASS_SHOT_ARG &);  // 0xE8
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, float, long, float,
                                           unsigned long);                      // 0xEC
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long); // 0xF0
            virtual void SetVolleyArtsMode(const bcs::FVEC3 &, long, long, long, long, long, long, float, float, float,
                                           unsigned long);                      // 0xF4
            virtual void SetVolleyShootMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long); // 0xF8
            virtual void SetVolleyDropMode(const bcs::FVEC3 &, long, long, long, unsigned long); // 0xFC
            virtual void SetVolleyAnyHitMode(const bcs::FVEC3 *, long);         // 0x100
            virtual void SetVolleyVsBossTossUpMode(const bcs::FVEC3 &, long, unsigned long); // 0x104
            virtual void SetVolleyMissionRRShotMode(const bcs::FVEC3 &, float, float); // 0x108
            virtual void GetVolleyPosAfterFrame(bcs::FVEC3 &, float, int) const; // 0x10C
            virtual float GetVolleyCatchableRate() const;                       // 0x110
            virtual int CanbeVolleyStraightSpike(const bcs::FVEC3 &);           // 0x114
            virtual int CanbeVolleyReceiveVariation() const;                    // 0x118
            virtual int CanbeVolleySpikeVariation() const;                      // 0x11C
            virtual int CanebVolleyTouch(long) const;                           // 0x120 (spelt so in the symbol)
            // Inline: a weak copy (li r3, 0) is in five modules.
            virtual int CanbeVolleyPickup(long) const { return 0; }             // 0x124
            virtual long GetVolleyReceiveReaction(unsigned long, long) const;   // 0x128
            virtual long GetVolleyBlockReaction(unsigned long, long) const;     // 0x12C
            virtual float GetVolleyReceiveStaggerFrame(long) const;             // 0x130
            virtual long GetVolleyReceiveDropCoinNum(long) const;               // 0x134
            virtual float GetVolleyFlickDownPower(long) const;                  // 0x138
            virtual int GetVolleyEndureParam(BALL::VOLLEY::ENDURE_PRM &);       // 0x13C
            virtual float GetVolleyPassCounter() const;                         // 0x140
            virtual void GetVolleyBallMarkerDispFrame(float &, float &) const;  // 0x144
            virtual void TrgVolleyReceiveReaction(long, unsigned long);         // 0x148
            virtual void TrgVolleyBlockReaction(unsigned long);                 // 0x14C
            virtual long GetVolleyBlockDropCoinNum(long) const;                 // 0x150
            virtual int TrgVolleyPreparationTouch(long, long, long, long);      // 0x154
            virtual void TrgVolleyArtsAttach(long, long, long, int);            // 0x158
            virtual void TrgVolleyArtsReady(long, int);                         // 0x15C
            virtual int TrgVolleyArtsOver(long, long, unsigned long);           // 0x160
            virtual int IsVolleyArtsAttach() const;                             // 0x164
            virtual int IsVolleyFakeBall() const;                               // 0x168
            virtual int IsVolleyReqDelete() const { return 0; }                 // 0x16C
            virtual int IsVolleyVanish() const { return 0; }                    // 0x170
            // Slots 0x174 to 0x4F0 added for game/dodge/CBallManDodge.cpp from the
            // slots of __vt__Q23msw10CBallDodge (DodgeF); return types guessed
            // from the names except where a caller shows them.
            virtual int IsVolleyChanceBallMode() const;                                             // 0x174
            virtual int IsVolleyPowerSpike() const;                                                 // 0x178
            virtual int IsVolleyCurveSpike() const;                                                 // 0x17C
            virtual int IsVolleyFeintSpike() const;                                                 // 0x180
            virtual int IsVolleyTimingSpike() const;                                                // 0x184
            virtual int IsVolleyNetHit() const;                                                     // 0x188
            virtual int IsVolleyEnemySpike() const;                                                 // 0x18C
            virtual void PlayVolleySpReceiveSuccessSe(unsigned long);                               // 0x190
            virtual float GetVolleyChargePower() const;                                             // 0x194
            virtual void SetVolleyChargePower(float);                                               // 0x198
            virtual void ClearVolleyChargePower();                                                  // 0x19C
            virtual long GetVolleyTouchNum(long) const;                                             // 0x1A0
            virtual long GetVolleyTrueTouchNum(long) const;                                         // 0x1A4
            virtual long GetVolleyCanbeTouchKind(long, long);                                       // 0x1A8
            virtual long GetVolleyBallTypeRR() const;                                               // 0x1AC
            virtual long GetVolleyBallColorTypeRR() const;                                          // 0x1B0
            virtual int IsVolleyComBallRR() const;                                                  // 0x1B4
            virtual int IsVolleyChargeBall() const;                                                 // 0x1B8
            virtual int IsVolleyTargetPosInnerCourt(long, float);                                   // 0x1BC
            virtual int GetVolleySpikeJumpTargetPos(bcs::FVEC3 &) const;                            // 0x1C0 (int: CComManVolley tests it)
            virtual void GetVolleyTouchDownPos(bcs::FVEC3 &, int) const;                            // 0x1C4
            virtual int IsVolleyObstacleBallRR() const;                                             // 0x1C8
            virtual int IsVolleyInCatchableArea(long) const;                                        // 0x1CC
            virtual int IsVolleyThroughUnderNet() const;                                            // 0x1D0
            virtual int DoVolleyItemBallDirectSpike(long, long);                                    // 0x1D4
            virtual void SetHockeyChargeShootMode(const bcs::FVEC3 &, float, long, float, long, float, float, float, int, int, int);// 0x1D8
            virtual void SetHockeyShootMode(const bcs::FVEC3 &, float, long, float, long, int, long);// 0x1DC
            virtual void SetHockeyArtsMode(const bcs::FVEC3 &, float, long, long, int, long);       // 0x1E0
            virtual void SetHockeyItemShootMode(const bcs::FVEC3 &, float, long, float, int, long); // 0x1E4
            virtual void SetHockeyBodyGoalMode(long, long, long);                                   // 0x1E8
            virtual int SetHockeySnatch(const CCollAtkUnit *, const bcs::FVEC3 &);                  // 0x1EC
            virtual void TrgGoalIn(const bcs::FVEC3 &, long, long, long, long, long);               // 0x1F0
            virtual void SetHockeyDropMode(const bcs::FVEC3 &, float, long, long);                  // 0x1F4
            virtual void SetHockeyArtsDropMode(const bcs::FVEC3 &, float, long, long);              // 0x1F8
            virtual long GetArtsType() const;                                                       // 0x1FC
            virtual int TrgHockeyArtsEnd(long);                                                     // 0x200
            virtual int IsHockeyArtsEnd() const;                                                    // 0x204
            virtual void SetHockeyArtsStart();                                                      // 0x208
            virtual void SetHockeyShootTargetPos(const bcs::FVEC3 &);                               // 0x20C
            virtual void SetHockeyShootTargetMove(float, float);                                    // 0x210
            virtual float GetHockeyNoChgLeaderCount();                                              // 0x214
            virtual long GetHockeyPassTgtChrIdx();                                                  // 0x218
            virtual int GetPassCatchPos(bcs::FVEC3 &, long);                                        // 0x21C
            virtual long GetHockeyHaveTeamIdx();                                                    // 0x220
            virtual float GetHockeySnatchFrame();                                                   // 0x224
            virtual void SetHockeySnatchFrame(float);                                               // 0x228
            virtual long GetHockeyFaceOffMode();                                                    // 0x22C
            virtual long GetHockeyCatchChrIdx();                                                    // 0x230
            virtual int IsHockeyShootGoalDir();                                                     // 0x234
            virtual void SetHockeyScaleSpeed(float);                                                // 0x238
            virtual long GetHockeyReBoundType() const;                                              // 0x23C
            virtual long GetHockeyItemBallType() const;                                             // 0x240
            virtual float GetHokeyBombExplodeRadius() const;                                        // 0x244
            virtual float GetHokeyBombShootDist() const;                                            // 0x248
            virtual int IsHockeyAttackMode() const;                                                 // 0x24C
            virtual int GetHokeyCurveShootData(float &, bcs::FVEC3 &);                              // 0x250
            virtual float GetFaceOffGuideDispFrm();                                                 // 0x254
            virtual void SettHockeyReflectMode(float);                                              // 0x258
            virtual int IsShootExceptionalStart();                                                  // 0x25C
            virtual int IsDummyShootBall();                                                         // 0x260
            virtual float GetHockeyPassSpeed();                                                     // 0x264
            virtual long GetVsBossBallMode() const;                                                 // 0x268
            virtual void SetVsBossTossUpMode(const bcs::FVEC3 &, long, unsigned long);              // 0x26C
            virtual int SetVsBossSpikeShotMode(BALL::BOSS::SPIKE_SHOT_ARG &);                       // 0x270
            virtual void SetVsBossPassShotMode(BALL::BOSS::PASS_SHOT_ARG &);                        // 0x274
            virtual void SetVsBossPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long);// 0x278
            virtual void SetVsBossShootMode(const bcs::FVEC3 &, long, float);                       // 0x27C
            virtual void SetVsBossDunkShotMode(const bcs::FVEC3 &, long);                           // 0x280
            virtual void SetVsBossArtsDunkMode(const bcs::FVEC3 &, long, int);                      // 0x284
            virtual int CanbeVsBossReceive() const;                                                 // 0x288
            virtual int CanbeVsBossReflect() const;                                                 // 0x28C
            virtual int CanbeVsBossAirTrick() const;                                                // 0x290
            virtual void SetVsBossReflectCheckEnd();                                                // 0x294
            virtual void SetVsBossCoinPower(long);                                                  // 0x298
            virtual void AddVsBossCoinPower(long);                                                  // 0x29C
            virtual long GetVsBossCoinPower() const;                                                // 0x2A0
            virtual void ClearVsBossCoinPower();                                                    // 0x2A4
            virtual void TrgVsBossArtsAttach(long, long, long, int);                                // 0x2A8
            virtual void SetSleepMode();                                                            // 0x2AC
            virtual void SetJumpBallAttach();                                                       // 0x2B0
            virtual void SetJumpBallMode();                                                         // 0x2B4
            virtual void SetChrReflectMode(long, float);                                            // 0x2B8
            virtual float JumpBallFrameSet(int);                                                    // 0x2BC
            virtual int IsActive() const;                                                           // 0x2C0
            virtual int IsSleepMode() const;                                                        // 0x2C4
            virtual int IsFreeMoveMode() const;                                                     // 0x2C8
            virtual int IsPassMode() const;                                                         // 0x2CC
            virtual int IsAlleyoopPassMode() const;                                                 // 0x2D0
            virtual int IsShootMode() const;                                                        // 0x2D4
            virtual int IsArtsShootMode() const;                                                    // 0x2D8
            virtual int IsChrNeedBlockMode(long) const;                                             // 0x2DC
            virtual int IsCanbeTurnOver() const;                                                    // 0x2E0
            virtual int IsNowRebound() const;                                                       // 0x2E4
            virtual int IsInvincible() const;                                                       // 0x2E8
            virtual int IsOffensive() const;                                                        // 0x2EC
            virtual int CanbeCatch() const;                                                         // 0x2F0
            virtual void SetNoCatchCount(long);                                                     // 0x2F4
            virtual float GetNoCatchCount() const;                                                  // 0x2F8 (float: CBallBasket's is an lfs)
            virtual float GetGoalInRate() const;                                                    // 0x2FC
            virtual float GetDefCounter() const;                                                    // 0x300 (float: CBallBasket's is an lfs)
            virtual void SetShotChrIdx(long);                                                       // 0x304
            virtual long GetShotChrIdx() const;                                                     // 0x308
            virtual void SetPassChrIdx(long);                                                       // 0x30C
            virtual long GetPassChrIdx() const;                                                     // 0x310
            virtual void SetShotTeamIdx(long);                                                      // 0x314
            virtual long GetShotTeamIdx() const;                                                    // 0x318
            virtual void SetShotArea(long);                                                         // 0x31C
            virtual long GetShotArea() const;                                                       // 0x320
            virtual void SetTargetGoalIdx(long);                                                    // 0x324
            virtual long GetTargetGoalIdx() const;                                                  // 0x328
            virtual void SetChargePower(float);                                                     // 0x32C
            virtual float GetChargePower() const;                                                   // 0x330
            virtual long GetTouchNum() const;                                                       // 0x334
            virtual long GetTargetCourtIdx() const;                                                 // 0x338
            virtual int IsTargetCourt(long) const;                                                  // 0x33C
            virtual void SetReboundDisable();                                                       // 0x340
            virtual void TrgBound(unsigned long);                                                   // 0x344
            virtual void TrgGoalIn(long, long, long, long, long);                                   // 0x348
            virtual void TrgTouch(long);                                                            // 0x34C
            virtual void TrgScore(long);                                                            // 0x350
            virtual void ClearTouch();                                                              // 0x354
            virtual long GetRallyNum() const;                                                       // 0x358
            virtual void AddRallyNum(long);                                                         // 0x35C
            virtual unsigned long SIDtoSeID_(long);                                                 // 0x360
            virtual void PlayBoundSe();                                                             // 0x364
            virtual void PlayFloorHitSe(unsigned long);                                             // 0x368
            virtual void PlayWallHitSe(unsigned long);                                              // 0x36C
            virtual float GetPassFlyFrame(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const;      // 0x370
            virtual float GetShootFlyFrame(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const;     // 0x374
            virtual float GetTouchDownFrame(float) const;                                           // 0x378
            virtual float GetTouchDownFrame2(float, float) const;                                   // 0x37C
            virtual void GetPosAfterFrame(bcs::FVEC3 &, float) const;                               // 0x380
            virtual void GetVelAfterFrame(bcs::FVEC3 &, float) const;                               // 0x384
            virtual float GetHeightPassFrame(float, int) const;                                     // 0x388
            virtual void Eff_CreateAttach(long, long, const bcs::FVEC3 *, unsigned long, unsigned long);// 0x38C
            virtual void Eff_SetSpeed(long, float);                                                 // 0x390
            virtual void Eff_SetDefaultSpeed(long);                                                 // 0x394
            virtual void Eff_SetColor(long, bcs::BCS_Color);                                        // 0x398
            virtual void Eff_SetAmbientLightColor(long, bcs::BCS_Color);                            // 0x39C
            virtual void Eff_SetPointLightColor(long, bcs::BCS_Color, bcs::BCS_Color);              // 0x3A0
            virtual void Eff_SetMdlKColorReg(long, _GXTevKColorID, bcs::BCS_Color);                 // 0x3A4
            virtual void Eff_SetMdlKColorReg(long, const char *, _GXTevKColorID, bcs::BCS_Color);   // 0x3A8
            virtual void Eff_SetMdlColorReg(long, _GXTevRegID, bcs::BCS_Color);                     // 0x3AC
            virtual void Eff_SetMdlColorReg(long, const char *, _GXTevRegID, bcs::BCS_Color);       // 0x3B0
            virtual void Eff_SetMiiColor(long, long);                                               // 0x3B4
            virtual unsigned long Eff_GetHandle(long) const;                                        // 0x3B8
            virtual void * Eff_GetCluster(long);                                                    // 0x3BC
            virtual long Eff_GetEffectID(long) const;                                               // 0x3C0
            virtual int Eff_IsAlive(long) const;                                                    // 0x3C4
            virtual void Eff_Stop(long);                                                            // 0x3C8
            virtual void Eff_Erase(long);                                                           // 0x3CC
            virtual void Eff_StopGroup(long);                                                       // 0x3D0
            virtual void Eff_EraseGroup(long);                                                      // 0x3D4
            virtual void Eff_StopAll();                                                             // 0x3D8
            virtual void Eff_EraseAll();                                                            // 0x3DC
            virtual void Eff_AliveCheckAll_();                                                      // 0x3E0
            virtual void Eff_GetGroupSlotMinMax_(long, long &, long &);                             // 0x3E4
            virtual void ResetParam_();                                                             // 0x3E8
            virtual void ResetOnAttach_();                                                          // 0x3EC
            virtual void ResetOnDetach_();                                                          // 0x3F0
            virtual void UpdateColor_();                                                            // 0x3F4
            virtual void SetHighlight_();                                                           // 0x3F8
            virtual void UpdateRainbow_();                                                          // 0x3FC
            virtual void UpdateCollAtk_();                                                          // 0x400
            virtual void UpdateCollBody_();                                                         // 0x404
            virtual void CheckCollision_();                                                         // 0x408
            virtual void UpdateFrame_();                                                            // 0x40C
            virtual void UpdateDisp_();                                                             // 0x410
            virtual void RotNormal_();                                                              // 0x414
            virtual void DispFunc_();                                                               // 0x418
            virtual void UpdateColorReg_();                                                         // 0x41C
            virtual void CreateMdlObj_(nw4r::g3d::ResFile, nw4r::g3d::ResMdl, nw4r::g3d::ResMdl, nw4r::g3d::ResMdl, long);// 0x420
            virtual void CreateRadarIcon_(nw4r::g3d::ResFile);                                      // 0x424
            virtual void DestroyMdlObj();                                                           // 0x428
            virtual void CreateCollBody_();                                                         // 0x42C
            virtual void CreateCollAttack_();                                                       // 0x430
            virtual unsigned long GetQueueWorkSize() const;                                         // 0x434
            virtual unsigned long GetQueueStackSize() const;                                        // 0x438
            virtual void CtorStateSleep();                                                          // 0x43C
            virtual void LoopStateSleep();                                                          // 0x440
            virtual void DtorStateSleep();                                                          // 0x444
            virtual void CtorStateFreeMove();                                                       // 0x448
            virtual void LoopStateFreeMove();                                                       // 0x44C
            virtual void DtorStateFreeMove();                                                       // 0x450
            virtual void CtorStateAttach();                                                         // 0x454
            virtual void LoopStateAttach();                                                         // 0x458
            virtual void DtorStateAttach();                                                         // 0x45C
            virtual void CtorStatePass();                                                           // 0x460
            virtual void LoopStatePass();                                                           // 0x464
            virtual void DtorStatePass();                                                           // 0x468
            virtual void CtorStatePassmeet();                                                       // 0x46C
            virtual void LoopStatePassmeet();                                                       // 0x470
            virtual void DtorStatePassmeet();                                                       // 0x474
            virtual void CtorStateAlleyoop();                                                       // 0x478
            virtual void LoopStateAlleyoop();                                                       // 0x47C
            virtual void DtorStateAlleyoop();                                                       // 0x480
            virtual void CtorStateNormalShoot();                                                    // 0x484
            virtual void LoopStateNormalShoot();                                                    // 0x488
            virtual void DtorStateNormalShoot();                                                    // 0x48C
            virtual void CtorStateChargeShoot();                                                    // 0x490
            virtual void LoopStateChargeShoot();                                                    // 0x494
            virtual void DtorStateChargeShoot();                                                    // 0x498
            virtual void CtorStateLayupShoot();                                                     // 0x49C
            virtual void LoopStateLayupShoot();                                                     // 0x4A0
            virtual void DtorStateLayupShoot();                                                     // 0x4A4
            virtual void CtorStateArtsShoot();                                                      // 0x4A8
            virtual void LoopStateArtsShoot();                                                      // 0x4AC
            virtual void DtorStateArtsShoot();                                                      // 0x4B0
            virtual void CtorStateItemShoot();                                                      // 0x4B4
            virtual void LoopStateItemShoot();                                                      // 0x4B8
            virtual void DtorStateItemShoot();                                                      // 0x4BC
            virtual void CtorStateJumpBall();                                                       // 0x4C0
            virtual void LoopStateJumpBall();                                                       // 0x4C4
            virtual void DtorStateJumpBall();                                                       // 0x4C8
            virtual void CtorStateTouchDown();                                                      // 0x4CC
            virtual void LoopStateTouchDown();                                                      // 0x4D0
            virtual void DtorStateTouchDown();                                                      // 0x4D4
            virtual void CtorStateGoal();                                                           // 0x4D8
            virtual void LoopStateGoal();                                                           // 0x4DC
            virtual void DtorStateGoal();                                                           // 0x4E0
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long);// 0x4E4
            virtual long CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &);             // 0x4E8 (long: the DOL's returns 2)
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);         // 0x4EC
            virtual void SetBound_(const bcs::FVEC3 &);                                             // 0x4F0

            // Loads the ball's model and builds the ball. A negative
            // `nModel` takes the ball's own number. (what the second
            // argument selects is not known; the boss game passes 0x40)
            void Create(long nModel, long);
            // (DOL) Builds the ball from `file`; the inline Create(ResFile)
            // passes 0x40 for the second argument.
            void Create(nw4r::g3d::ResFile file, long);
            // (DOL) Moves the ball to rPos. (return type guessed)
            void SetPos(const bcs::FVEC3 &rPos);
            // DOL. Not zero while the ball is attached to something (most
            // likely held by a character; not checked).
            int IsAttach() const;

            // (name guessed) Not zero for a ball that is out of play.
            int IsSleep() const { return m_nFlag & FLAG_SLEEP; }
            // (name guessed) Not zero after SetDestroyOnReset(1).
            int IsDestroyOnReset() const { return m_nFlag & FLAG_DESTROY_ON_RESET; }
            // (name guessed) Not zero once the ball asked CBallMan to delete it.
            int IsDeleteRequest() const { return m_nFlag & FLAG_DELETE_REQUEST; }
            // DOL (added for main game/msw/CBallMan.cpp; return type guessed).
            // Not zero for a star ball.
            int IsStarBall() const;
            // (name guessed)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }
            const bcs::FVEC3 &GetTargetPos() const { return m_vTargetPos; }
            // (name guessed) See m_PosD4.
            const bcs::FVEC3 &GetPosD4() const { return m_PosD4; }
            // (name guessed)
            const bcs::FVEC3 &GetOffsetPos() const { return m_OffsetPos; }
            // (name guessed)
            int IsScored() const { return m_nFlag & FLAG_SCORED; }
            // (names guessed) The ball's place in CBallMan's table.
            long GetBallNo() const { return m_nBallNo; }
            // (DOL; added for DodgeF CComDrvDodgeGame) The item kind of an
            // item ball (CItem::KIND_*; 3, a bomb, is what DodgeF tests).
            long GetItemBallType() const;
            void SetBallNo(long nBallNo) { m_nBallNo = nBallNo; }
            // DOL.
            void SetPos(const bcs::FVEC3 *pPos);
            // DOL. The model the ball is attached to, or null.
            bcs::CScnObj *GetAttachMdlObj() const;
            // DOL. The ball's offset from the hand of the character holding
            // it (VolleyF's CChrCoreVolleyLuigi lifts it while it comes in).
            void SetAttachPos(const bcs::FVEC3 &rOfs, int);
            // (DOL; added for DodgeF CChrCoreDodgeWaluigi, return types
            // guessed) Puts the ball at rPos at once; attaches it to node
            // nNode of pMdl; frees it again.
            void WarpPos(const bcs::FVEC3 &rPos);
            void Attach(bcs::CMdlObj *pMdl, unsigned long nNode, float, unsigned long, unsigned long, bcs::CScnGroup *);
            void Detach();
            // (DOL; added for BasketF CChrCoreBasketWaluigi) With 1 the
            // ball is destroyed when the game resets (FLAG_DESTROY_ON_RESET).
            void SetDestroyOnReset(int bOn);
            // (name guessed; added for HockeyF CChrCoreHockeyWaluigi) The
            // resource file the ball was built from. The caller needs the
            // definition of nw4r::g3d::ResFile to copy it.
            const nw4r::g3d::ResFile &GetResFile() const
            {
                return *reinterpret_cast<const nw4r::g3d::ResFile *>(m_ResFile);
            }
            // DOL (added for HockeyF CChrCoreHockeyWaluigi; meaning guessed
            // from the name): with 1 no marker is shown for the ball.
            void SetDisableMarker(int bDisable);
            // (names guessed) The ball's turn.
            const Quaternion &GetRot() const { return m_Rot; }
            void SetRot(const Quaternion &rRot)
            {
                m_Rot.x = rRot.x;
                m_Rot.y = rRot.y;
                m_Rot.z = rRot.z;
                m_Rot.w = rRot.w;
            }
            // DOL (name from the symbol). Stops the trail effect behind the
            // ball (VolleyF's CChrCoreVolleySiroma, once it is pulled in).
            void StopTailEffect();
            // (names guessed) Two more bits of m_nFlag that the basket and
            // hockey ball markers read; what they stand for is not known.
            int IsFlag1() const { return m_nFlag & 0x1; }
            int IsFlag4() const { return m_nFlag & 0x4; }
            // (added for BossF CChrCoreBoss::CanbeSpike, which then spikes
            // the ball whatever its height)
            int IsFlag2() const { return m_nFlag & 0x2; }
            int IsFlag100000() const { return m_nFlag & 0x100000; }
            // (names guessed; added for BasketF's CChrCoreBasketNinja, which
            // sets and clears bit 4 directly while the ninja hides) Sets or
            // clears bits of m_nFlag.
            void OnFlag(unsigned long nBit) { m_nFlag |= nBit; }
            void OffFlag(unsigned long nBit) { m_nFlag &= ~nBit; }
            // (names guessed) msw::CItemBall attaches its model to the
            // ball's; msw::CItemBallShellWreck sends the wreck off along the
            // velocity.
            bcs::CMdlObj *GetMdlObj() const { return m_pMdlObj; }
            const bcs::FVEC3 &GetVelocity() const { return m_Velocity; }
            // (names guessed; added for DodgeF game/dodge/CItemBallBombDodge.cpp)
            // Direct access for the dodgeball item behaviours, which read
            // and write these members of the ball without a call.
            void SetVelocity(const bcs::FVEC3 &rVel) { m_Velocity = rVel; }
            void SetTargetPos(const bcs::FVEC3 &rPos) { m_vTargetPos = rPos; }
            long GetShootType() const { return m_nShootType; }
            // (fn_800C7660, name guessed) Calls UpdateDisp_; msw::CItemBall
            // calls it when the item shows or hides its demo display.
            void UpdateDisp();
            // (DOL; return type guessed) Plays sound nSe at the ball, or at
            // *pPos when it is not null. The item balls pass
            // (nSe, 0, 1, -1, 0.0f, 1.0f, 0.0f), as for CGameSnd::Play.
            // (DOL; added for MenuF's msw::CTopMenuChrObj, which plays the
            // sound of the ball's sport with it; arguments as PlaySePos)
            void PlayMoveSe(long nSe, const bcs::FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                            float fVolume, float fArg6);
            void PlaySePos(long nSe, const bcs::FVEC3 *pPos, unsigned long nArg2, long nArg3, float fArg4,
                           float fVolume, float fArg6);
            // (DOL; return types guessed) Used by BasketF's
            // msw::CBallBasketFruit.
            // (BossF's CBallBoss tests bit 0x10 of the result.)
            unsigned long TrgItemEffect_(long);
            void AdjustGravity_();
            void SetAlphaRate(float fRate);
            void SetCollPushRadius(float fRadius);
            void SetCollBodyRadius(float fRadius);
            void SetCollCatchRadius(float fRadius);
            void SetScale(float fScale);

            // (DOL; added for BossF game/boss/CBallBoss.cpp, return types and
            // parameter meanings guessed from the names and the callers)
            // The body collision callbacks CreateCollBody_ registers; pArg
            // is the ball.
            static unsigned long CallbackBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                    const bcs::FVEC3 &rPos, void *pArg);
            static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                         bcs::FVEC3 *pPush);
            void EffectStopTail_();
            void SetBlinkDisable();
            void Detach_();
            void SetSquashParam(const bcs::FVEC3 &rDir, float, float, float, float);
            void RotReduce_(float fRate);
            void VelReduce_(float fRate);
            void ResetVelGravity_();
            void Eff_CreateSimple(long nEffect, const bcs::FVEC3 &rPos, unsigned long, unsigned long);
            void Eff_CreateSimple(long nEffect, const bcs::FVEC3 &rPos, float, unsigned long, unsigned long);
            // Solves the vertical motion: the start velocity and gravity of a
            // flight of fFrame frames from fStartY over fTopY to fEndY.
            void CalcYVG(float fStartY, float fTopY, float fEndY, float fFrame, float &rVel, float &rGravity) const;
            void SE_Stop(long nSlot, float fFade);
            int IsItemEffect() const;
            void LinkItemEffect_(unsigned long nId);
            void ClearOffsetPos();

        protected:
            unsigned long m_nFlag;         // 0x94
            long m_nBallNo;                // 0x98, the ball's place in CBallMan's table
            // 0x9C (name guessed): the step of the running state (BossF's
            // CBallBoss counts its touch-down steps with it).
            long m_nStateStep;
            unsigned char m_UnkA0[0x4];    // 0xA0
            // (names guessed) Per-frame steps: the DOL sets both to the same
            // value; CBallBasket counts its timers down by them and passes
            // m_fMoveRate to FuncMove_.
            float m_fFrameRate;            // 0xA4
            float m_fMoveRate;             // 0xA8
            // 0xAC: the ball's nw4r::g3d::ResFile (kept as bytes so that this
            // header need not include nw4r/g3d; GetResFile gives it typed).
            unsigned char m_ResFile[0x4];
            bcs::CMdlObj *m_pMdlObj;       // 0xB0, the ball's model (name guessed)
            // 0xB4, a second scene object, deleted with the model (name
            // guessed; CBallBasketFruit sets bits 0xC0 of its flags)
            bcs::CScnObj *m_pSubObj;
            // 0xB8 (name guessed): a third scene object of the ball;
            // MenuF's msw::CTopMenuBall colours it with m_pMdlObj.
            bcs::CMdlObj *m_pSubObj2;
            // 0xBC, 0xC4 (names guessed): the animations CreateMdlObj_
            // attaches to m_pMdlObj (BossF CBallBoss).
            bcs::CAnmMotion *m_pAnmMotion;
            unsigned char m_UnkC0[0x4];    // 0xC0
            bcs::CAnmMatClr *m_pAnmMatClr;
            // 0xC8, 0xCC (names guessed): the ball's attack and body
            // collision (BossF CBallBoss::CreateCollBody_ builds the body).
            CAtkCollision *m_pAtkColl;
            CBodyCollision *m_pBodyColl;
            unsigned char m_UnkD0[0x4];    // 0xD0
            // 0xD4 (name not known; added for BossF game/boss/unit_18E08.cpp):
            // a point of the ball's flight; the boss characters face it in
            // their receive poses and draw the ball's line from it to
            // m_vTargetPos.
            bcs::FVEC3 m_PosD4;
            // 0xE0 (name guessed): where the ball will come down / is heading;
            // VolleyF's CBallTargetMarker puts its marker there, DodgeF's item
            // balls set it (CItemBallBaseDodge::SetTargetPos).
            bcs::FVEC3 m_vTargetPos;
            // 0xEC (name guessed): the position of the frame before (BossF's
            // CBallBoss checks the map from here to m_Pos and sets both on a
            // bounce).
            bcs::FVEC3 m_PrevPos;
            bcs::FVEC3 m_Pos;              // 0xF8
            // 0x104 (name guessed): the velocity of the frame before.
            bcs::FVEC3 m_PrevVel;
            bcs::FVEC3 m_Velocity;         // 0x110 (name guessed)
            unsigned char m_Unk11C[0x18];  // 0x11C
            // 0x134 (name guessed from CBall::ClearOffsetPos, which writes it)
            bcs::FVEC3 m_OffsetPos;
            Quaternion m_Rot;              // 0x140, the ball's turn (name guessed)
            bcs::FVEC3 m_vRotAxis;         // 0x150 (name guessed)
            float m_fRotSpeed;             // 0x15C (name guessed; BossF sets 15 degrees on a bounce)
            float m_fGravity;              // 0x160 (name guessed; passed to CBallBasket::FuncMove_)
            unsigned char m_Unk164[0x4];   // 0x164
            // 0x168 (name guessed): the ball's scale; BossF's CBallBoss grows
            // it with the coin power and multiplies its radii by it.
            float m_fScale;
            unsigned char m_Unk16C[0x10];  // 0x16C
            long m_Unk17C;                 // 0x17C (meaning not known)
            unsigned char m_Unk180[0xC];   // 0x180
            // 0x18C (meaning not known): BossF's CBallBoss copies its attach
            // lift here.
            float m_f18C;
            unsigned char m_Unk190[0x30];  // 0x190
            long m_nShootType;             // 0x1C0, returned by GetDodgeShootType (name guessed)
            unsigned char m_Unk1C4[0xC];   // 0x1C4
            float m_fAlphaRate;            // 0x1D0 (name guessed; SetAlphaRate's value)
            unsigned char m_Unk1D4[0x8];   // 0x1D4
            // 0x1DC-0x1E4 (meanings not known): a squash of the model that
            // BossF's CBallBoss reads while the ball fades out.
            float m_f1DC;
            long m_n1E0;
            float m_f1E4;
            unsigned char m_Unk1E8[0x130]; // 0x1E8
    };
}

#endif /* !MSW_CBALL_HPP_ */
