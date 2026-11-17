#ifndef MSW_CBALLVOLLEY_HPP_
#define MSW_CBALLVOLLEY_HPP_

#include "game/msw/CBall.hpp"
#include "bcs/BCS_Color.hpp"
#include "bcs/FVEC4.hpp"

namespace msw {

    // The ball of the volley game (VolleyF), size 0x48C (the size
    // CBallManVolley allocates): CBall plus 0x174 bytes of its own.
    //
    // INCOMPLETE: the virtual functions are declared (see below), but few
    // members are mapped.
    class CBallVolley : public CBall {
        public:
            struct ARTS_EFCT_TBL;

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
            virtual long AttachItemEffect(long);                                                     // 0x5C
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
            virtual float GetVolleyPassCounter() const;                    // 0x140
            virtual void GetVolleyBallMarkerDispFrame(float &, float &) const;                       // 0x144
            virtual void TrgVolleyReceiveReaction(long, unsigned long);                              // 0x148
            virtual void TrgVolleyBlockReaction(unsigned long);                                      // 0x14C
            virtual long GetVolleyBlockDropCoinNum(long) const;                                      // 0x150
            virtual int TrgVolleyPreparationTouch(long, long, long, long);                           // 0x154
            virtual void TrgVolleyArtsAttach(long, long, long, int);                                 // 0x158
            virtual void TrgVolleyArtsReady(long, int);                                              // 0x15C
            virtual int TrgVolleyArtsOver(long, long, unsigned long);                                // 0x160
            virtual int IsVolleyArtsAttach() const;                         // 0x164
            virtual int IsVolleyReqDelete() const;                                                  // 0x16C
            virtual int IsVolleyVanish() const;                                                     // 0x170
            virtual int IsVolleyChanceBallMode() const;                                              // 0x174
            virtual int IsVolleyPowerSpike() const;                                                  // 0x178
            virtual int IsVolleyCurveSpike() const;                                                  // 0x17C
            virtual int IsVolleyFeintSpike() const;                                                  // 0x180
            virtual int IsVolleyTimingSpike() const;                     // 0x184
            virtual int IsVolleyNetHit() const;                                                      // 0x188
            virtual int IsVolleyEnemySpike() const;                                                  // 0x18C
            virtual void PlayVolleySpReceiveSuccessSe(unsigned long);                                // 0x190
            virtual float GetVolleyChargePower() const;                                              // 0x194
            virtual void SetVolleyChargePower(float);                                                // 0x198
            virtual void ClearVolleyChargePower();                                                   // 0x19C
            virtual long GetVolleyTouchNum(long) const;                                              // 0x1A0
            virtual long GetVolleyTrueTouchNum(long nTeam) const; // 0x1A4
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
            virtual void SetShotChrIdx(long nChr);                          // 0x304
            virtual long GetShotChrIdx() const;                             // 0x308
            virtual void SetPassChrIdx(long nChr);                          // 0x30C
            virtual long GetPassChrIdx() const;                             // 0x310
            virtual void SetShotTeamIdx(long nTeam);                      // 0x314
            virtual long GetShotTeamIdx() const;                           // 0x318
            virtual long GetTouchNum() const;                       // 0x334
            virtual long GetTargetCourtIdx() const;                                                  // 0x338
            virtual int IsTargetCourt(long) const;                                                   // 0x33C
            virtual void TrgGoalIn(long, long, long, long, long);                                    // 0x348
            virtual unsigned long SIDtoSeID_(long nSe);                               // 0x360
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
            virtual void CollCheckPassHitNet_(bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &) const; // 0x500
            virtual void AttachTailEffect_();                                                        // 0x504
            virtual void TrgHitFloor_(int);                                                          // 0x508
            virtual void CmnShotInit_();                                                             // 0x50C
            virtual void SetTouch_(long, long, int);                                                 // 0x510
            virtual int CanbeTouchDown_();                                                           // 0x514
            virtual void SetSpikeTimingWork_(float, float, float);                                   // 0x518
            virtual int IsTimingSpike_() const;                                                      // 0x51C
            virtual void SetSpikeJumpTgtWork_(float);                                                // 0x520
            virtual int GetColorReg0Color_(bcs::BCS_Color &);                                        // 0x524
            virtual int GetColorAmbientColor_(bcs::BCS_Color &);                                     // 0x528
            virtual const ARTS_EFCT_TBL *GetArtsEffectTbl_(long, long) const;                                 // 0x52C
            virtual int IsScoreBookCountChr_(long);                                                  // 0x530

        public:
            // (name guessed) The flight SetPassModeParam2_, SetBadPassParam_,
            // SetBadReturnParam_ and SetChanceBallParam_ work out for a
            // pass. INCOMPLETE: only the fields read so far.
            struct PASS_PARAM_ {
                float m_f00;     // 0x00
                float m_f04;     // 0x04
                float m_f08;     // 0x08
                float m_f0C;     // 0x0C
                float m_f10;     // 0x10
                float m_f14;     // 0x14
                float m_f18;     // 0x18
                float m_f1C;     // 0x1C
                float m_f20;     // 0x20
                float m_f24;     // 0x24
                float m_f28;     // 0x28
                float m_f2C;     // 0x2C
                long m_n30;      // 0x30
            };

            // The effects and reactions of a special shot (one per kind,
            // GetArtsEffectTbl_), size 0x74. Field meanings guessed.
            struct ARTS_EFCT_TBL {
                // The colours to give an effect: a .data object built by
                // the static initialiser (size 0x20).
                struct EFCT_COL_SET {
                    ~EFCT_COL_SET() {}

                    // Bits: 1 the point light colours (0, 1), 2 and 4
                    // the TEV colours 0 and 1 (2, 3), 8, 0x10 and 0x20 the
                    // K colours 0 to 2 (4 to 6).
                    unsigned long m_nFlag;
                    // Plain colours (the destructor has nothing to do).
                    GXColor m_aColor[7];
                };
                // One effect: its id and colours.
                struct EFCT {
                    long m_nEffect;
                    // Bits 1, 2, 4: passed to Eff_CreateAttach; 8: a
                    // floor mark, 0x10: a model effect (touch down).
                    unsigned long m_nFlag;
                    EFCT_COL_SET *m_pColor;
                };
                // A list of effects played together.
                struct EFCT_TBL {
                    long m_nNum;
                    const EFCT *m_pEfct;
                };

                EFCT_TBL m_aEfct[7];          // 0x00
                long m_nReceiveReaction;      // 0x38
                long m_nReceiveReaction3;     // 0x3C
                long m_nBlockReaction;        // 0x40
                long m_nUnk44;                // 0x44
                long m_nUnk48;                // 0x48
                float m_fUnk4C;               // 0x4C
                long m_anUnk50[6];            // 0x50
                long m_anUnk68[3];            // 0x68
            };

        protected:
            void InitArtsShotStep_();
            void UpdateCountWork_();
            void FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate);
            int CollSubCheckBG_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel);
            void CollSubCheckDef_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal);
            void DoBlockReflect_(long nKind, long nChrId, long nChr, long nTeam, const bcs::FVEC3 &rNormal, long nSe,
                                 long nEffect, int bReflect, unsigned long nOutline, int bChr);
            void DoBlockAcuteReflect_(const bcs::FVEC3 &rNormal, long nGoal, float, float, float, float, float);
            void SetParabolaParam_(float, float, float, float, float, float, float);
            void TrgGoalIn_(int bFall, int bOut, int bWater);
            void Util_GetCourtRect_(long nCourt, bcs::FVEC4 &rRect, int bMargin);
            void Util_GetModifyTouchDownPos_(const bcs::FVEC3 &rPos, bcs::FVEC3 &rOut);
            void Util_GetPassParam_(long nKind, float &rSpeed, float &rRate, float &rFrame);
            float CalcXZVel2(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel, float &, float &,
                             float &rFrame, float, float, float, float, float) const;
            int CanbeStraightSpike_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rCross);
            float SetPassModeParam_(const bcs::FVEC3 &rTarget, float fSpeed, float fRate, float fFrame, float fHeight);
            void SetBadPassParam_(long nBad, long nType, PASS_PARAM_ &rParam);
            float SetPassModeParam2_(const bcs::FVEC3 &rTarget, float fTop, float fSpeed, float fEndSpeed, float fFrame,
                                     float fHeight);
            void CorruptTargetPos_(bcs::FVEC3 &rTarget, float fAngleMin, float fAngleMax, float fDistMin, float fDistMax,
                                   float &rRate, int bAbsolute);
            int SetSpikeParam_(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg);
            void SetSpikeFeintParam_(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg);
            float CalcSpikeTargetPosRate_(const bcs::FVEC3 &rTarget, long nCourt, float &rMin, float &rMid, float &rMax);
            void SetArtsSpikeParam_(long nArts, const bcs::FVEC3 &rTarget, float, float, float);
            void SetBadReturnParam_(long nBad, PASS_PARAM_ &rParam);
            float SetChanceBallParam_(const bcs::FVEC3 &rTarget, PASS_PARAM_ &rParam);
            void ArtsBallBeforeMoveFunc_();
            void ArtsBallAfterMoveFunc_();
            // The small getters and setters among them (SIDtoSeID_, the
            // shot and pass indices, GetTouchNum, IsVolleyArtsAttach,
            // GetVolleyPassCounter, GetVolleyTrueTouchNum,
            // IsVolleyTimingSpike, CanebVolleyTouch) are inline in
            // CBallVolley.cpp only: other units refer to them.

            // (added for CC06_VOLLEY::CBallVolleyWaluigi) What a special
            // shot that is over does (non-virtual; return type guessed).
            int TrgVolleyArtsOver_(long, long, unsigned long);
            void SetArtsTailEffect__(const ARTS_EFCT_TBL::EFCT_TBL &rTbl);
            int SetArtsTouchdownEffect_(const ARTS_EFCT_TBL::EFCT_TBL &rTbl, const bcs::FVEC3 &rPos);

        protected:
            // (names guessed) Inline helpers of CBallVolley.cpp: clear the
            // per touch tables, the spin, the spike timing and target, the
            // item hit flag; tell the cameras where the ball is heading;
            // the effect table of the special shot the ball carries (null
            // when there is none).
            inline void ClearTouchReaction_();
            inline void ClearSpin_();
            inline void ClearSpikeWork_();
            inline void ClearRide_();
            inline void UpdateCameraTarget_(int bFlag);
            inline const ARTS_EFCT_TBL *GetArtsEffectTbl_() const;
            inline void UpdateSpin_();
            inline void Reflect_(bcs::FVEC3 &rOut, const bcs::FVEC3 &rNormal) const;
            inline void SetLooseBall_();
            inline void SetBlockNormal_(long nKind, long nChrId, long nChr, long nTeam, const bcs::FVEC3 &rNormal,
                                        int bChr, long &rSe, long &rEffect, long &rVoice, int &rbRot);
            inline void ClearReceiveReaction200000_();
            inline void SetShotChrNoHit_(long nChr);
            inline void InitFlight_(const bcs::FVEC3 &rTarget);
            inline void SetReceiveStagger_();
            inline void SetReceiveReaction3_();
            inline void CheckServeSide_();
            inline void SetReceiveStaggerHard_();
            inline CChrCore *InitJumpBall_();
            inline void GetFlightRot_(bcs::FVEC3 &rRot) const;
            inline void CalcFlightAccel_(bcs::FVEC3 &rAccel, const bcs::FVEC3 &rDir, float fDecel, float fGravity,
                                         float fTime, int bSpin) const;

        protected:

            unsigned long m_nVolleyFlag318;  // 0x318
            unsigned long m_nVolleyFlag;     // 0x31C
            float m_f320;                    // 0x320
            float m_f324;                    // 0x324
            long m_nShotChrIdx;              // 0x328
            long m_n32C;                     // 0x32C
            long m_n330;                     // 0x330
            long m_n334;                     // 0x334
            long m_nPassChrIdx;              // 0x338
            long m_nShotTeamIdx;             // 0x33C
            long m_nServeCourt;                     // 0x340
            long m_nTouchNum;                     // 0x344
            long m_nTrueTouchNum;                     // 0x348
            long m_n34C;                     // 0x34C
            float m_f350;                    // 0x350
            float m_fChargePower;            // 0x354
            long m_n358;                     // 0x358
            float m_f35C;                    // 0x35C, counted down by m_fMoveRate
            float m_f360;                    // 0x360, counted down by m_fMoveRate
            float m_fPassCounter;            // 0x364, counted down by m_fMoveRate
            // 0x368 (name guessed): the kind of spike, 1 power, 3 curve,
            // 5 feint, 6 none.
            long m_nSpikeType;
            long m_n36C;                     // 0x36C
            float m_fMarkerDispFrame;        // 0x370 (name guessed)
            float m_fMarkerDispFrame2;       // 0x374 (name guessed)
            // 0x378-0x3C7 (names guessed): per touch count (0 to 4), the
            // reaction of a receive (the second table for kind 3), of a
            // block, and the frames a receiver staggers.
            long m_anReceiveReaction[5];     // 0x378
            long m_anReceiveReaction3[5];    // 0x38C
            long m_anBlockReaction[5];       // 0x3A0
            float m_afStaggerFrame[5];       // 0x3B4
            // 0x3C8-0x3DB (names guessed): a spin given to the ball (kind
            // 1 to 3), its timer and its acceleration.
            long m_nSpinType;
            float m_fSpinCount;              // 0x3CC
            bcs::FVEC3 m_vSpinAccel;         // 0x3D0
            float m_f3DC;                    // 0x3DC, counted down by m_fMoveRate
            long m_n3E0;                     // 0x3E0
            long m_n3E4;                     // 0x3E4
            long m_bArtsAttach;              // 0x3E8
            long m_n3EC;                     // 0x3EC
            float m_f3F0;                    // 0x3F0
            long m_nArtsKind;                // 0x3F4 (name guessed), -1 for none
            long m_n3F8;                     // 0x3F8
            long m_n3FC;                     // 0x3FC
            float m_f400;                    // 0x400
            float m_f404;                    // 0x404
            float m_f408;                    // 0x408
            long m_bEndure;                  // 0x40C (name guessed): m_EndurePrm is valid
            BALL::VOLLEY::ENDURE_PRM m_EndurePrm; // 0x410
            // 0x434: set once the special shot is over (the split balls
            // of Waluigi's shot cannot be caught after that); the ball
            // then fades out (m_fFadeWait, m_fAlpha) and asks to be
            // deleted.
            int m_bArtsOver;
            float m_fFadeWait;               // 0x438 (name guessed)
            float m_fAlpha;                  // 0x43C (name guessed)
            float m_fArtsOverRate;           // 0x440, the alpha lost per frame
            unsigned long m_nVolleyFlag444;  // 0x444
            long m_n448;                     // 0x448
            // 0x44C: a floor hit sound to play once instead of CBall's,
            // or -1.
            long m_nFloorHitSe;
            long m_n450;                     // 0x450
            // 0x454-0x463 (names guessed): the timing window of a spike.
            float m_fTimingCount;            // 0x454
            float m_fTimingStart;            // 0x458
            float m_fTimingMid;              // 0x45C
            float m_fTimingEnd;              // 0x460
            // 0x464-0x477 (names guessed): where and when a spiker can
            // meet the ball (SetSpikeJumpTgtWork_).
            bcs::FVEC3 m_vSpikeJumpTgt;      // 0x464
            float m_fSpikeJumpHeight;        // 0x470
            float m_fSpikeJumpFrame;         // 0x474
            long m_nRideObjHandle;                     // 0x478
            bcs::FVEC3 m_vRideObjPos;               // 0x47C
            unsigned long m_nSpikeFlag;      // 0x488
    };

    // The ball of the volley game against a boss (VolleyF), size 0x494 (the
    // size BALL_MAN_VOLLEY::CBallManVolleyVsBoss allocates): CBallVolley
    // plus 8 bytes. Its constructor has CBallVolley's inline.
    class CBallVolleyVsBoss : public CBallVolley {
        public:
            CBallVolleyVsBoss(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallVolleyVsBoss();

            virtual void Disp();
            virtual void SetVolleyVsBossTossUpMode(const bcs::FVEC3 &rTarget, long nTeam, unsigned long);
            virtual int CanebVolleyTouch(long) const;
            virtual int CanbeVolleyPickup(long) const { return m_bPickup; }
            virtual long GetVolleyTouchNum(long nTeam) const;
            virtual long GetVolleyTrueTouchNum(long nTeam) const;
            virtual long GetVolleyCanbeTouchKind(long nTeam, long nChr);
            virtual void SetJumpBallMode();
            virtual float GetNoCatchCount() const;
            virtual void ResetParam_();
            virtual void ResetOnAttach_();
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual void CollSubCheckTgtPos_();
            virtual int CollSubCheckNet_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal);
            virtual void CollCheckPassHitNet_(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo) const;
            virtual void TrgHitFloor_(int bWater);
            virtual void SetTouch_(long nTeam, long nChr, int);
            virtual int CanbeTouchDown_();
            virtual void SetSpikeTimingWork_(float, float, float) {}
            virtual int IsScoreBookCountChr_(long) { return 0; }

        private:
            // 0x48C (name guessed): the ball lies on the floor after a
            // point and can be picked up.
            long m_bPickup;
            // 0x490 (name guessed): how far the ball is tinted (a half per
            // touch of the attacking team).
            float m_fTint;
    };

    // A ball of the rhythm receive mission (VolleyF), size 0x4B4 (the size
    // CRhythmReceiveMan allocates). Field names guessed.
    class CBallVolleyRhythmReceive : public CBallVolley {
        public:
            CBallVolleyRhythmReceive(bcs::CTask *pParent, long nSe, long nSe2, long nType, long nColor, int bCom);
            virtual ~CBallVolleyRhythmReceive();

            virtual void Disp() {}
            virtual float GetCollCatchRadiusDefault() const;
            virtual long AttachItemEffect(long) { return 0; }
            virtual void SetVolleyServeTossMode(BALL::VOLLEY::SERVE_TOSS_ARG &rArg);
            virtual int SetVolleyServeShotMode(BALL::VOLLEY::SERVE_SHOT_ARG &rArg);
            virtual int SetVolleySpikeShotMode(BALL::VOLLEY::SPIKE_SHOT_ARG &rArg);
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, float, long, float, unsigned long);
            virtual void SetVolleyPassMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long);
            virtual void SetVolleyArtsMode(const bcs::FVEC3 &, long, long, long, long, long, long, float, float, float,
                                           unsigned long);
            virtual void SetVolleyShootMode(const bcs::FVEC3 &, long, long, long, long, long, unsigned long);
            virtual void SetVolleyDropMode(const bcs::FVEC3 &, long, long, long, unsigned long);
            virtual void SetVolleyMissionRRShotMode(const bcs::FVEC3 &rTarget, float fFrame, float fBeat);
            virtual int CanebVolleyTouch(long nChr) const;
            virtual int TrgVolleyArtsOver(long nKind, long nChr, unsigned long nArg);
            virtual int IsVolleyChanceBallMode() const { return 0; }
            virtual int IsVolleyPowerSpike() const { return 0; }
            virtual int IsVolleyCurveSpike() const { return 0; }
            virtual int IsVolleyTimingSpike() const { return 0; }
            virtual long GetVolleyBallTypeRR() const { return m_nBallType; }
            virtual long GetVolleyBallColorTypeRR() const { return m_nColorType; }
            virtual int IsVolleyComBallRR() const;
            virtual int IsVolleyObstacleBallRR() const;
            virtual int IsVolleyInCatchableArea(long) const { return 1; }
            virtual float GetNoCatchCount() const;
            virtual void PlayFloorHitSe(unsigned long nArg);
            virtual void CreateMdlObj_(nw4r::g3d::ResFile file, nw4r::g3d::ResMdl mdl, nw4r::g3d::ResMdl shadowMdl,
                                       nw4r::g3d::ResMdl refMdl, long nShadow);
            virtual void CtorStateAlleyoop();
            virtual void LoopStateAlleyoop();
            virtual void DtorStateAlleyoop();
            virtual int CollSubCheckNet_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal);
            virtual void CollCheckPassHitNet_(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo) const;
            virtual void AttachTailEffect_();
            virtual void TrgHitFloor_(int bWater);
            virtual void CmnShotInit_();
            virtual void SetSpikeTimingWork_(float, float, float) {}
            virtual int IsScoreBookCountChr_(long) { return 0; }

        private:
            void SetReceive_(long nChr);

            long m_nBallType;      // 0x48C: 0 to 3 a colour, 4 rainbow, 5 and 6 obstacles
            long m_nColorType;     // 0x490
            long m_nTouchMask;     // 0x494: the sides (bits) that may touch the ball
            float m_fBeat;         // 0x498: the music frame the ball is due on
            long m_nSe;            // 0x49C: the receive sound
            long m_nSeOrg;         // 0x4A0
            long m_nSe2;           // 0x4A4: a second sound after the receive
            int m_bCom;            // 0x4A8
            long m_nBounce;        // 0x4AC: bounces left before the miss
            float m_fCatchRadius;  // 0x4B0
    };
}

#endif /* !MSW_CBALLVOLLEY_HPP_ */
