#ifndef MSW_CBALLBASKET_HPP_
#define MSW_CBALLBASKET_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

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
            virtual void AttachItemEffect(long);
            virtual void DetachItemEffect();
            virtual void SetPassMode(const bcs::FVEC3 &, long, float, long, long, int, int, unsigned long);
            virtual void SetShootMode(long, float, long, long, unsigned long);
            virtual const bcs::FVEC3 &GetShootStartPos() const;               // inline
            virtual long GetShootCalcType() const;                            // inline
            virtual float GetShootCurveValue() const;                         // inline: m_fShootCurveValue
            virtual void CalcJumpShootVel(const bcs::FVEC3 &, const bcs::FVEC3 &, bcs::FVEC3 &, float &);
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
            virtual void CheckCollision__();                                  // 0x500
            virtual void CollSubCheckBG_(bcs::FVEC3 &, bcs::FVEC3 &);         // 0x504
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
            void CheckCurveStep_();
            // Moves rPos by rVel for one step under fGravity. (argument
            // names guessed)
            void FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate);

        protected:
            unsigned char m_Unk318[0x320 - 0x318];
            float m_fTimer320;          // 0x320, counted down by m_fMoveRate
            unsigned char m_Unk324[0x338 - 0x324];
            unsigned long m_nBasketFlag; // 0x338
            unsigned char m_Unk33C[0x344 - 0x33C];
            long m_nShotChrIdx;         // 0x344
            long m_nPassChrIdx;         // 0x348
            unsigned char m_Unk34C[0x354 - 0x34C];
            long m_nShotArea;           // 0x354
            long m_nShotTeamIdx;        // 0x358 (passed to CGame::GetTargetGoalIdx)
            long m_nTargetGoalIdx;      // 0x35C
            unsigned char m_Unk360[0x370 - 0x360];
            float m_fChargePower;       // 0x370
            float m_fUnk374;            // 0x374
            float m_fNoCatchCount;      // 0x378, counted down by m_fFrameRate
            float m_fTimer37C;          // 0x37C, counted down by m_fFrameRate
            float m_fDefCounter;        // 0x380, counted down by m_fFrameRate
            unsigned char m_Unk384[0x398 - 0x384];
            float m_fGoalInRate;        // 0x398
            unsigned char m_Unk39C[0x3A0 - 0x39C];
            float m_fShootCurveValue;   // 0x3A0
            unsigned char m_Unk3A4[0x468 - 0x3A4];
    };
}

#endif /* !MSW_CBALLBASKET_HPP_ */
