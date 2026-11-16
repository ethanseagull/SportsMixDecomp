#ifndef MSW_CBALLBASKET_HPP_
#define MSW_CBALLBASKET_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    namespace BALL_BASKET {
        // How a shot flies: filled in by the ball's item
        // (CItemBallBasket::GetShotParam_), passed to
        // CBallBasket::SetShootParamTest2_. Field meanings are not known.
        struct SHOT_PARAM {
            long m_nUnk00;
            long m_nUnk04;
            float m_fUnk08;
            float m_fUnk0C;
            float m_fUnk10;
            float m_fUnk14;
            float m_fUnk18;
            float m_fUnk1C;
            float m_fCurve; // 0x20, CItemBallBasket::GetCurveValue (name guessed)
            // 0x24 (name guessed): where the shot goes (SetShootParam_ puts
            // CChrCoreBasket::GetTargetShootPos here). Meanings of the
            // fields above, from CBallBasket::SetShootParamTest2_: 0x00
            // not zero for a straight shot; 0x04 not zero for a curving
            // one; 0x08 the chance of scoring; 0x0C the speed (per second);
            // 0x10, 0x14 the lowest and highest top of the arc; 0x18, 0x1C
            // the fewest and most frames of the flight; 0x20 how far a
            // curving shot bends.
            bcs::FVEC3 m_vTarget;
        };
    }

    // The ball of the basketball game (BasketF), size 0x468 (msw::
    // CBallBasketFruit's own members start there; nothing allocates this
    // class directly here).
    //
    // INCOMPLETE: written for game/basket/CBallBasketFruit.cpp, whose vtable
    // needs every slot. The overrides of CBall's virtual functions are the
    // CBallBasket entries of __vt__Q23msw11CBallBasket (BasketF); the new
    // virtual functions are its slots 0x4F4 to 0x540. Return types are read
    // from the bodies where they are one instruction, otherwise guessed.
    // Functions marked "inline" are inline in the original (CBallBasket.cpp
    // emits them as weak functions); their bodies are not written yet.
    // Member names are guesses.
    class CBallBasket : public CBall {
        public:
            CBallBasket(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallBasket();
            virtual void Main();

            virtual void Reset();
            virtual long AttachItemEffect(long);
            virtual void DetachItemEffect();
            virtual void SetPassMode(const bcs::FVEC3 &, long, float, long, long, int, int, unsigned long);
            virtual void SetShootMode(long, float, long, long, unsigned long);
            virtual bcs::FVEC3 GetShootStartPos() const;               // inline
            virtual long GetShootCalcType() const;                            // inline
            virtual float GetShootCurveValue() const;                         // inline: m_fShootCurveValue
            virtual float CalcJumpShootVel(const bcs::FVEC3 &, const bcs::FVEC3 &, bcs::FVEC3 &, float &);
            virtual void DoExplode(float, float);
            virtual int IsAirBall() const;
            virtual void OnBaPad0Shot();
            virtual void ResetBaPad0Shot();
            virtual int IsBaExplode() const;                                  // inline: 0
            virtual int IsBaPad0Shot() const;
            virtual void SetSleepMode();
            virtual void SetJumpBallMode();
            virtual void SetChrReflectMode(long, float);
            virtual int IsFreeMoveMode() const;
            virtual int IsPassMode() const;
            virtual int IsAlleyoopPassMode() const;
            virtual int IsShootMode() const;
            virtual int IsArtsShootMode() const;
            virtual int IsChrNeedBlockMode(long) const;
            virtual int IsCanbeTurnOver() const;
            virtual int IsNowRebound() const;
            virtual int IsInvincible() const;
            virtual int CanbeCatch() const;
            virtual float GetNoCatchCount() const;                            // inline: m_fNoCatchCount
            virtual float GetGoalInRate() const;                              // inline: m_fGoalInRate
            virtual float GetDefCounter() const;                              // inline: m_fDefCounter
            virtual long GetShotChrIdx() const;                               // inline: m_nShotChrIdx
            virtual long GetPassChrIdx() const;                               // inline: m_nPassChrIdx
            virtual long GetShotArea() const;                                 // inline: m_nShotArea
            virtual long GetTargetGoalIdx() const;                            // inline: m_nTargetGoalIdx
            virtual void SetChargePower(float);
            virtual float GetChargePower() const;                             // inline: m_fChargePower
            virtual void SetReboundDisable();                                 // inline: clears bit 0x40 of 0x338
            virtual void TrgBound(unsigned long);
            virtual void TrgGoalIn(long, long, long, long, long);
            virtual unsigned long SIDtoSeID_(long);
            virtual void PlayBoundSe();
            virtual float GetPassFlyFrame(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const;
            virtual float GetShootFlyFrame(const bcs::FVEC3 &, const bcs::FVEC3 &, long) const;
            virtual void GetPosAfterFrame(bcs::FVEC3 &, float) const;
            virtual void ResetParam_();
            virtual void ResetOnAttach_();
            virtual void ResetOnDetach_();
            virtual void SetHighlight_();
            virtual void UpdateCollAtk_();
            virtual void UpdateCollBody_();
            virtual void CheckCollision_();
            virtual void CreateCollBody_();
            virtual void CreateCollAttack_();
            virtual void CtorStateFreeMove();
            virtual void LoopStateFreeMove();
            virtual void CtorStateAttach();
            virtual void LoopStateAttach();
            virtual void DtorStateAttach();
            virtual void CtorStatePass();
            virtual void LoopStatePass();
            virtual void DtorStatePass();
            virtual void CtorStatePassmeet();
            virtual void LoopStatePassmeet();
            virtual void DtorStatePassmeet();
            virtual void CtorStateAlleyoop();
            virtual void LoopStateAlleyoop();
            virtual void DtorStateAlleyoop();
            virtual void CtorStateNormalShoot();
            virtual void LoopStateNormalShoot();
            virtual void DtorStateNormalShoot();
            virtual void CtorStateChargeShoot();
            virtual void LoopStateChargeShoot();
            virtual void DtorStateChargeShoot();
            virtual void CtorStateLayupShoot();
            virtual void LoopStateLayupShoot();
            virtual void DtorStateLayupShoot();
            virtual void CtorStateArtsShoot();
            virtual void LoopStateArtsShoot();
            virtual void DtorStateArtsShoot();
            virtual void CtorStateItemShoot();
            virtual void LoopStateItemShoot();
            virtual void DtorStateItemShoot();
            virtual void CtorStateJumpBall();
            virtual void LoopStateJumpBall();
            virtual void DtorStateJumpBall();
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &,
                                            unsigned long);
            virtual long CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &);
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual void SetBound_(const bcs::FVEC3 &);

            virtual void ReleaseBall(const bcs::FVEC3 &);                     // 0x4F4
            virtual void SetChrReflectMode(const bcs::FVEC3 &);               // 0x4F8
            virtual void SetChrReflectMode(long, float, float, float);        // 0x4FC
            // (return values: see CollSubCheckAll_)
            virtual long CheckCollision__();                                  // 0x500
            virtual long CollSubCheckBG_(bcs::FVEC3 &, bcs::FVEC3 &);         // 0x504
            virtual void TrgGoalIn_();                                        // 0x508
            virtual void RingHit();                                           // 0x50C
            virtual int IsDummy() const;                                      // 0x510
            // (meaning guessed) Frames of the "just shot" timing window.
            virtual long GetJustShotMinStartFrm() const;                      // 0x514 inline
            virtual long GetJustShotMidStartFrm() const;                      // 0x518 inline
            virtual long GetJustShotMaxStartFrm() const;                      // 0x51C inline
            virtual long GetJustShotMinEndFrm() const;                        // 0x520 inline
            virtual long GetJustShotMidEndFrm() const;                        // 0x524 inline
            virtual long GetJustShotMaxEndFrm() const;                        // 0x528 inline
            virtual long GetJustShotSignEffectID() const;                     // 0x52C
            virtual float GetFloorBoundRateY() const;                         // 0x530
            virtual float GetFloorBoundRateXZ() const;                        // 0x534
            virtual float GetWallBoundRateXZ() const;                         // 0x538
            // Gives a ball that bounces straight up a random sideways speed.
            virtual void CalcBoundRandXZBG();                                 // 0x53C
            virtual void SetShootResult(int);                                 // 0x540

            // Plays the effect of a ball going through the ring.
            void PlayGoalInEff();
            // (names from the symbols; meanings guessed)
            void PlaySeArtsOrbit_();
            void GetBasketPosAfterFrame(bcs::FVEC3 &rPos, float fFrame, int bCurve) const;
            // The collision checks of one step from rFrom to the ball's
            // position. They return what was hit: 0 nothing, 2 the floor,
            // 3 a wall, 4 the ring (rebound), 5 the backboard, 6 a block,
            // 8 and 10 the inside of the ring (8: through it).
            long CollSubCheckAll_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal);
            long CollSubCheckGoalIn_(long nGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                     const bcs::FVEC3 &rTo, int bRebound) const;
            long CollSubCheckGoalOut_(long nGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo);
            long CollSubCheckDef_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal);
            void SetHitDef_();
            int ReboundDemoCheck_(long nHit, int &rbBound);
            void ReboundDemoMakeParam_(long nHit);
            long ReboundDemoCalcNum_();
            void ReboundDemoSetParam_();
            int JudgeGoalIn_();
            int CheckGoalIn_();
            float SetPassModeParam__(long nType, const bcs::FVEC3 &rTarget, float fSpeed, float &rFrame, int, int);
            // Sets the flight of a shot; returns its XZ distance (0: none).
            float SetShootParamTest2_(const BALL_BASKET::SHOT_PARAM &rParam);
            void AddAirBallMiss_(bcs::FVEC3 &rTarget);
            // (name from the symbol of its constant fDist) The shot of a
            // ball with an item. (inline, defined in CBallBasket.cpp)
            void SetShootParamItem_();
            float CalcXZVel2(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel, float &rDecel,
                             float &rFrame, float fSpeed0, float fSpeed1, float fMinFrame, float fMaxFrame) const;
            float SetShootParam_(float fRate);
            void SetArtsShootParam_(float fRate);
            int IsArtsShootAtkEnbale() const;
            int IsLayUpShootMode() const;
            void PlayEffLayUp(const CChrCore *pChr);
            void PlayArtsEff();
            void SetJustShootFlg(int bJust);
            void StopJustSignEff();
            void CheckCurveStep_();
            // (inline, defined in CBallBasket.cpp) Tables of the passes and
            // shots by type and game level; the top of a lob.
            float GetPassFrameMin_(long nType) const;
            float GetPassFrameMax_(long nType) const;
            float GetPassRate_(long nType) const;
            long GetShotFrameMin_() const;
            float GetMidHeight(long nType, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo) const;
            // Moves rPos by rVel for one step under fGravity. (argument
            // names guessed)
            void FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate);
            // (name guessed) Sets bits of m_nBasketFlag; BasketF's ghost
            // sets 0x10000 on the ball it passes.
            void OnBasketFlag(unsigned long nBit) { m_nBasketFlag |= nBit; }
            // (name guessed; added for game/basket/CChrManBasket.cpp, which
            // tests bit 0x200000 of a passed ball)
            unsigned long GetBasketFlag() const { return m_nBasketFlag; }
            // Drops the item behaviour pItem from the ball (the bomb calls
            // it on itself when it goes off).
            void DetachItemEffect(CItemBall *pItem);
            // (name guessed) Waluigi's special shot sets it on each ball it
            // throws.
            void SetArtsShotUnk(float fValue) { m_fArtsShotUnk384 = fValue; }
            // (name from the weak copy in BasketF's CChrCoreBasketNinja.cpp)
            // The same member: the ninja's special shot sets 0.17 before
            // it shoots.
            void SetArtsShootSpeed(float fSpeed) { m_fArtsShotUnk384 = fSpeed; }
            // (name guessed; added for game/basket/unit_9AFD4.cpp)
            long GetPassTargetNo() const { return m_nPassTargetNo; }
            // (name guessed) Clears bits of m_nBasketFlag.
            void OffBasketFlag(unsigned long nBit) { m_nBasketFlag &= ~nBit; }

        protected:
            // Clears the curve of a shot (m_nCurveStep and its parameters).
            void ClearCurve_()
            {
                m_nCurveStep = 0;
                m_fTimer320 = 0.0f;
                m_fCurveDecel = 0.0f;
                m_fCurveTime = 0.0f;
                m_vCurveAccel.x = 0.0f;
                m_vCurveAccel.y = 0.0f;
                m_vCurveAccel.z = 0.0f;
            }
            // (name guessed) What every bounce does first: the curve stops,
            // the shot can no longer score.
            void ClearShot_()
            {
                ClearCurve_();
                m_fGoalInRate = 0.0f;
                m_nBasketFlag &= ~0x40;
            }
            // (name guessed) The ball is loose: a shot in the air has
            // missed, the game goes into loose ball mode, a ball that was
            // passed or shot goes back to free move. (inline, defined in
            // CBallBasket.cpp)
            void SetLooseBall_();
            // (name guessed) Sets the axis the ball rolls about for a
            // velocity rVel along the floor. (inline, defined in
            // CBallBasket.cpp)
            void SetRollAxis_(const bcs::FVEC3 &rVel);

            // 0x318 (name guessed): the step of the shot's curve: 0 and 1
            // none, 2 and 3 curving (CheckCurveStep_ ends it after
            // m_fCurveTime frames).
            long m_nCurveStep;
            long m_nCurveDir;           // 0x31C (name guessed): 2 or 4, the side a curving shot bends to
            float m_fTimer320;          // 0x320, counted down by m_fMoveRate (frames before the curve starts)
            float m_fCurveDecel;        // 0x324 (name guessed): the XZ speed lost per frame (FuncMove_)
            float m_fCurveTime;         // 0x328 (name guessed): frames the curve lasts
            bcs::FVEC3 m_vCurveAccel;   // 0x32C (name guessed): added to the velocity each frame of the curve
            unsigned long m_nBasketFlag; // 0x338
            unsigned long m_nAttachNode; // 0x33C (name guessed): the node of the model holding the ball
            long m_nPassType;           // 0x340 (name guessed): the type SetPassMode got
            long m_nShotChrIdx;         // 0x344
            long m_nPassChrIdx;         // 0x348
            // 0x34C (name guessed): the player number of the character a
            // pass goes to (BasketF's computer driver compares it with
            // CChrCore::GetPlayerNo).
            long m_nPassTargetNo;
            long m_nHitDefChrNo;        // 0x350 (name guessed): the character that blocked the ball, -1 for none
            long m_nShotArea;           // 0x354
            long m_nShotTeamIdx;        // 0x358 (passed to CGame::GetTargetGoalIdx)
            long m_nTargetGoalIdx;      // 0x35C
            long m_n360;                // 0x360 (meaning not known)
            long m_nArtsStep;           // 0x364 (name guessed): the step of a special shot
            long m_n368;                // 0x368 (meaning not known)
            long m_nTouchChrNo;         // 0x36C (name guessed): the player number of the last character to touch the ball
            float m_fChargePower;       // 0x370
            float m_fUnk374;            // 0x374
            float m_fNoCatchCount;      // 0x378, counted down by m_fFrameRate
            float m_fTimer37C;          // 0x37C, counted down by m_fFrameRate
            float m_fDefCounter;        // 0x380, counted down by m_fFrameRate
            // 0x384 (name guessed): set by Waluigi's special shot on each
            // ball it throws (SetArtsShotUnk).
            float m_fArtsShotUnk384;
            float m_fArtsTimer;         // 0x388 (name guessed): frames left in the step of a special shot
            float m_fArtsWarpNum;       // 0x38C (name guessed): warps left (Kamek's special shot)
            float m_fArtsWarpDist;      // 0x390 (name guessed): the distance from the goal that triggers a warp
            float m_fArtsWarpCount;     // 0x394 (name guessed): warps done
            float m_fGoalInRate;        // 0x398
            float m_fPassTime;          // 0x39C (name guessed): frames left of a pass
            float m_fShootCurveValue;   // 0x3A0
            float m_fRiverTimer;        // 0x3A4 (name guessed): frames until a ball fallen into the river comes back
            bcs::FVEC3 m_vShootStartPos; // 0x3A8 (name from GetShootStartPos)
            // 0x3B4-0x467 (names guessed): the bounces off the ring before a
            // rebound (ReboundDemo*_), cleared together.
            struct REBOUND {
                long m_bValid;          // 0x00
                float m_fRotY;          // 0x04, the direction of the bounce
                float m_fFrame;         // 0x08, frames to the next bounce
                float m_fDist;          // 0x0C, distance from the goal
                float m_fHeight;        // 0x10, height over the ring
            };
            long m_nReboundNum;         // 0x3B4
            long m_nReboundCount;       // 0x3B8
            REBOUND m_aRebound[8];      // 0x3BC
            // 0x45C: the bounce off the floor after the rim (CheckGoalIn_).
            float m_fBoundRotY;
            float m_fBoundAngle;        // 0x460
            float m_fBoundDist;         // 0x464
    };

    // A dummy ball of the basket game, size 0x468 (CBallManBasket's
    // CreateDummyBall_). (what it is used for is not known)
    class CBallBasketDummy : public CBallBasket {
        public:
            CBallBasketDummy(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallBasketDummy();

            virtual int CanbeCatch() const;
            virtual void CreateCollBody_();
            virtual void LoopStateFreeMove();
            virtual void DtorStateArtsShoot();
            virtual void TrgGoalIn_();
            virtual int IsDummy() const;
    };
}

#endif /* !MSW_CBALLBASKET_HPP_ */
