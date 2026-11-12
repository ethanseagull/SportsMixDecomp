#ifndef MSW_CBALLVOLLEY_HPP_
#define MSW_CBALLVOLLEY_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the volley game (VolleyF), size 0x48C (the size
    // CBallManVolley allocates): CBall plus 0x174 bytes of its own.
    //
    // INCOMPLETE: the virtual functions are declared (see below), but few
    // members are mapped.
    class CBallVolley : public CBall {
        public:
            CBallVolley(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallVolley();

            // (added for game/volley/CChrCoreVolleyWaluigi.cpp, which emits
            // the vtable of a CBallVolley-derived class) The overrides of
            // CBall's virtual functions: the CBallVolley slots of
            // __vt__Q33msw11CC06_VOLLEY18CBallVolleyWaluigi, declared as in
            // CBall.
            virtual void Disp();                                                                     // 0x10
            virtual void Reset();                                                                    // 0x2C
            virtual float GetRadius() const;                                                         // 0x30
            virtual float GetBgCollRadius() const;                                                   // 0x34
            virtual float GetBgCollRadiusXZ() const;                                                 // 0x38
            virtual float GetCollCatchRadiusDefault() const;                                         // 0x44
            virtual int GetTargetPos(bcs::FVEC3 &) const;                                            // 0x58
            virtual void SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &);                     // 0xDC
            virtual int SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &);                      // 0xE0
            virtual int SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &);                      // 0xE4
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, float, long, float, unsigned long); // 0xEC
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long); // 0xF0
            virtual void SetVolleyArtsMode(const bcs::FVEC3 &, long, long, long, long, long, long, float, float, float, unsigned long); // 0xF4
            virtual void SetVolleyShootMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long); // 0xF8
            virtual void SetVolleyDropMode(const bcs::FVEC3 &, long, long, long, unsigned long);     // 0xFC
            virtual void SetVolleyAnyHitMode(const bcs::FVEC3 *, long);                              // 0x100
            virtual void GetVolleyPosAfterFrame(bcs::FVEC3 &, float, int) const;                     // 0x10C
            virtual float GetVolleyCatchableRate() const;                                            // 0x110
            virtual int CanbeVolleyStraightSpike(const bcs::FVEC3 &);                                // 0x114
            virtual int CanbeVolleySpikeVariation() const;                                           // 0x11C
            virtual int CanebVolleyTouch(long) const;                                                // 0x120
            virtual long GetVolleyReceiveReaction(unsigned long, long) const;                        // 0x128
            virtual long GetVolleyBlockReaction(unsigned long, long) const;                          // 0x12C
            virtual float GetVolleyReceiveStaggerFrame(long) const;                                  // 0x130
            virtual long GetVolleyReceiveDropCoinNum(long) const;                                    // 0x134
            virtual float GetVolleyFlickDownPower(long) const;                                       // 0x138
            virtual int GetVolleyEndureParam(BALL::VOLLEY::ENDURE_PRM &);                            // 0x13C
            virtual float GetVolleyPassCounter() const;                                              // 0x140
            virtual void GetVolleyBallMarkerDispFrame(float &, float &) const;                       // 0x144
            virtual void TrgVolleyReceiveReaction(long, unsigned long);                              // 0x148
            virtual void TrgVolleyBlockReaction(unsigned long);                                      // 0x14C
            virtual long GetVolleyBlockDropCoinNum(long) const;                                      // 0x150
            virtual int TrgVolleyPreparationTouch(long, long, long, long);                           // 0x154
            virtual void TrgVolleyArtsAttach(long, long, long, int);                                 // 0x158
            virtual void TrgVolleyArtsReady(long, int);                                              // 0x15C
            virtual int IsVolleyArtsAttach() const;                                                  // 0x164
            virtual int IsVolleyReqDelete() const;                                                  // 0x16C
            virtual int IsVolleyVanish() const;                                                     // 0x170
            virtual int IsVolleyChanceBallMode() const;                                              // 0x174
            virtual int IsVolleyPowerSpike() const;                                                  // 0x178
            virtual int IsVolleyCurveSpike() const;                                                  // 0x17C
            virtual int IsVolleyFeintSpike() const;                                                  // 0x180
            virtual int IsVolleyTimingSpike() const;                                                 // 0x184
            virtual int IsVolleyNetHit() const;                                                      // 0x188
            virtual int IsVolleyEnemySpike() const;                                                  // 0x18C
            virtual float GetVolleyChargePower() const;                                              // 0x194
            virtual void SetVolleyChargePower(float);                                                // 0x198
            virtual void ClearVolleyChargePower();                                                   // 0x19C
            virtual long GetVolleyTouchNum(long) const;                                              // 0x1A0
            virtual long GetVolleyTrueTouchNum(long) const;                                          // 0x1A4
            virtual long GetVolleyCanbeTouchKind(long, long);                                        // 0x1A8
            virtual int IsVolleyChargeBall() const;                                                  // 0x1B8
            virtual int IsVolleyTargetPosInnerCourt(long, float);                                    // 0x1BC
            virtual int GetVolleySpikeJumpTargetPos(bcs::FVEC3 &) const;                             // 0x1C0
            virtual void GetVolleyTouchDownPos(bcs::FVEC3 &, int) const;                             // 0x1C4
            virtual int IsVolleyInCatchableArea(long) const;                                         // 0x1CC
            virtual int IsVolleyThroughUnderNet() const;                                             // 0x1D0
            virtual int DoVolleyItemBallDirectSpike(long, long);                                     // 0x1D4
            virtual void SetSleepMode();                                                             // 0x2AC
            virtual void SetJumpBallMode();                                                          // 0x2B4
            virtual void SetChrReflectMode(long, float);                                             // 0x2B8
            virtual int IsActive() const;                                                            // 0x2C0
            virtual int IsSleepMode() const;                                                         // 0x2C4
            virtual int IsFreeMoveMode() const;                                                      // 0x2C8
            virtual int IsPassMode() const;                                                          // 0x2CC
            virtual int IsAlleyoopPassMode() const;                                                  // 0x2D0
            virtual int IsShootMode() const;                                                         // 0x2D4
            virtual int IsArtsShootMode() const;                                                     // 0x2D8
            virtual int IsCanbeTurnOver() const;                                                     // 0x2E0
            virtual int IsInvincible() const;                                                        // 0x2E8
            virtual int CanbeCatch() const;                                                          // 0x2F0
            virtual void SetShotChrIdx(long);                                                        // 0x304
            virtual long GetShotChrIdx() const;                                                      // 0x308
            virtual void SetPassChrIdx(long);                                                        // 0x30C
            virtual long GetPassChrIdx() const;                                                      // 0x310
            virtual void SetShotTeamIdx(long);                                                       // 0x314
            virtual long GetShotTeamIdx() const;                                                     // 0x318
            virtual long GetTouchNum() const;                                                        // 0x334
            virtual long GetTargetCourtIdx() const;                                                  // 0x338
            virtual int IsTargetCourt(long) const;                                                   // 0x33C
            virtual void TrgGoalIn(long, long, long, long, long);                                    // 0x348
            virtual unsigned long SIDtoSeID_(long);                                                  // 0x360
            virtual void PlayBoundSe();                                                              // 0x364
            virtual void PlayFloorHitSe(unsigned long);                                              // 0x368
            virtual float GetTouchDownFrame(float) const;                                            // 0x378
            virtual float GetTouchDownFrame2(float, float) const;                                    // 0x37C
            virtual void GetPosAfterFrame(bcs::FVEC3 &, float) const;                                // 0x380
            virtual void GetVelAfterFrame(bcs::FVEC3 &, float) const;                                // 0x384
            virtual float GetHeightPassFrame(float, int) const;                                      // 0x388
            virtual void ResetParam_();                                                              // 0x3E8
            virtual void ResetOnAttach_();                                                           // 0x3EC
            virtual void ResetOnDetach_();                                                           // 0x3F0
            virtual void SetHighlight_();                                                            // 0x3F8
            virtual void UpdateRainbow_();                                                           // 0x3FC
            virtual void UpdateCollAtk_();                                                           // 0x400
            virtual void UpdateCollBody_();                                                          // 0x404
            virtual void CheckCollision_();                                                          // 0x408
            virtual void UpdateDisp_();                                                              // 0x410
            virtual void CreateCollBody_();                                                          // 0x42C
            virtual void CtorStateFreeMove();                                                        // 0x448
            virtual void LoopStateFreeMove();                                                        // 0x44C
            virtual void DtorStateFreeMove();                                                        // 0x450
            virtual void CtorStateAttach();                                                          // 0x454
            virtual void LoopStateAttach();                                                          // 0x458
            virtual void DtorStateAttach();                                                          // 0x45C
            virtual void CtorStatePass();                                                            // 0x460
            virtual void LoopStatePass();                                                            // 0x464
            virtual void DtorStatePass();                                                            // 0x468
            virtual void CtorStateAlleyoop();                                                        // 0x478
            virtual void LoopStateAlleyoop();                                                        // 0x47C
            virtual void DtorStateAlleyoop();                                                        // 0x480
            virtual void CtorStateNormalShoot();                                                     // 0x484
            virtual void LoopStateNormalShoot();                                                     // 0x488
            virtual void DtorStateNormalShoot();                                                     // 0x48C
            virtual void CtorStateChargeShoot();                                                     // 0x490
            virtual void LoopStateChargeShoot();                                                     // 0x494
            virtual void DtorStateChargeShoot();                                                     // 0x498
            virtual void CtorStateArtsShoot();                                                       // 0x4A8
            virtual void LoopStateArtsShoot();                                                       // 0x4AC
            virtual void DtorStateArtsShoot();                                                       // 0x4B0
            virtual void CtorStateJumpBall();                                                        // 0x4C0
            virtual void LoopStateJumpBall();                                                        // 0x4C4
            virtual void DtorStateJumpBall();                                                        // 0x4C8
            virtual void CtorStateTouchDown();                                                       // 0x4CC
            virtual void LoopStateTouchDown();                                                       // 0x4D0
            virtual void DtorStateTouchDown();                                                       // 0x4D4
            virtual long CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &);              // 0x4E8
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);          // 0x4EC
            virtual void SetBound_(const bcs::FVEC3 &);                                              // 0x4F0

            // The virtual functions CBallVolley adds, in vtable order (names
            // from the symbols; return types guessed).
            virtual void GetVolleyVelAfterFrame(bcs::FVEC3 &, float, int) const;                    // 0x4F4
            virtual void CollSubCheckTgtPos_();                                                      // 0x4F8
            virtual int CollSubCheckNet_(bcs::FVEC3 &, bcs::FVEC3 &);                                // 0x4FC
            virtual int CollCheckPassHitNet_(bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &) const; // 0x500
            virtual void AttachTailEffect_();                                                        // 0x504
            virtual void TrgHitFloor_(int);                                                          // 0x508
            virtual void CmnShotInit_();                                                             // 0x50C
            virtual void SetTouch_(long, long, int);                                                 // 0x510
            virtual int CanbeTouchDown_();                                                           // 0x514
            virtual void SetSpikeTimingWork_(float, float, float);                                   // 0x518
            virtual int IsTimingSpike_() const;                                                      // 0x51C
            virtual void SetSpikeJumpTgtWork_(float);                                                // 0x520
            virtual void GetColorReg0Color_(bcs::BCS_Color &);                                       // 0x524
            virtual void GetColorAmbientColor_(bcs::BCS_Color &);                                    // 0x528
            virtual const void *GetArtsEffectTbl_(long, long) const;                                 // 0x52C
            virtual int IsScoreBookCountChr_(long);                                                  // 0x530

        protected:
            // (added for CC06_VOLLEY::CBallVolleyWaluigi) What a special
            // shot that is over does (non-virtual; return type guessed).
            int TrgVolleyArtsOver_(long, long, unsigned long);

            // Mostly not mapped. Names guessed (CChrCoreVolleyWaluigi.cpp):
            // 0x318 and 0x444 are flag words only the constructors set bits
            // of; 0x434 is set once the special shot is over (the split
            // balls of Waluigi's shot cannot be caught after that); 0x440
            // is a rate per frame (Waluigi's balls set 1/5).
            unsigned long m_nVolleyFlag318;  // 0x318
            unsigned char m_Unk31C[0x434 - 0x31C];
            int m_bArtsOver;                 // 0x434
            unsigned char m_Unk438[0x440 - 0x438];
            float m_fArtsOverRate;           // 0x440
            unsigned long m_nVolleyFlag444;  // 0x444
            unsigned char m_Unk448[0x48C - 0x448];
    };

    // The ball of the volley game against a boss (VolleyF), size 0x494 (the
    // size BALL_MAN_VOLLEY::CBallManVolleyVsBoss allocates): CBallVolley
    // plus 8 bytes. Its constructor has CBallVolley's inline.
    //
    // INCOMPLETE: only what game/volley/CBallManVolley.cpp needs to create
    // one.
    class CBallVolleyVsBoss : public CBallVolley {
        public:
            CBallVolleyVsBoss(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallVolleyVsBoss();

        private:
            unsigned char m_Unk48C[0x8]; // 0x48C
    };
}

#endif /* !MSW_CBALLVOLLEY_HPP_ */
