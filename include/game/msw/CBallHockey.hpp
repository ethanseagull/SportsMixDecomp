#ifndef MSW_CBALLHOCKEY_HPP_
#define MSW_CBALLHOCKEY_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the hockey game (HockeyF), size 0x484 (the allocation in
    // CBallManHockey::Create_): CBall plus 0x16C bytes of its own.
    //
    // INCOMPLETE: only what game/hockey/CBallManHockey.cpp and
    // game/hockey/CChrCoreHockeyWaluigi.cpp need. The overrides of CBall's
    // virtual functions are the CBallHockey slots of
    // __vt__Q33msw11CC06_HOCKEY18CBallHockeyWaluigi (declarations as in
    // CBall); the new ones (0x4F4 on) follow it too (names from the
    // symbols, return types guessed).
    class CBallHockey : public CBall {
        public:
            CBallHockey(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallHockey();

            virtual void Main();                                                                 // 0xC
            virtual void Reset();                                                                // 0x2C
            virtual float GetBgCollRadius() const;                                               // 0x34
            virtual void *GetRadarIconTexObj() const;                                            // 0x50
            virtual void DetachItemEffect();                                                     // 0x60
            virtual void SetPassMode(const bcs::FVEC3 &, long, float, long, long, int, int, unsigned long); // 0x64
            virtual void SetHockeyChargeShootMode(const bcs::FVEC3 &, float, long, float, long, float, float, float, int, int, int); // 0x1D8
            virtual void SetHockeyShootMode(const bcs::FVEC3 &, float, long, float, long, int, long); // 0x1DC
            virtual void SetHockeyArtsMode(const bcs::FVEC3 &, float, long, long, int, long);    // 0x1E0
            virtual void SetHockeyItemShootMode(const bcs::FVEC3 &, float, long, float, int, long); // 0x1E4
            virtual void SetHockeyBodyGoalMode(long, long, long);                                // 0x1E8
            virtual void SetHockeySnatch(const CCollAtkUnit *, const bcs::FVEC3 &);              // 0x1EC
            virtual void TrgGoalIn(const bcs::FVEC3 &, long, long, long, long, long);            // 0x1F0
            virtual void SetHockeyDropMode(const bcs::FVEC3 &, float, long, long);               // 0x1F4
            virtual void SetHockeyArtsDropMode(const bcs::FVEC3 &, float, long, long);           // 0x1F8
            virtual long GetArtsType() const;                                                    // 0x1FC
            virtual void TrgHockeyArtsEnd(long);                                                 // 0x200
            virtual int IsHockeyArtsEnd() const;                                                 // 0x204
            virtual void SetHockeyArtsStart();                                                   // 0x208
            virtual void SetHockeyShootTargetPos(const bcs::FVEC3 &);                            // 0x20C
            virtual void SetHockeyShootTargetMove(float, float);                                 // 0x210
            virtual long GetHockeyNoChgLeaderCount();                                            // 0x214
            virtual long GetHockeyPassTgtChrIdx();                                               // 0x218
            virtual void GetPassCatchPos(bcs::FVEC3 &, long);                                    // 0x21C
            virtual long GetHockeyHaveTeamIdx();                                                 // 0x220
            virtual float GetHockeySnatchFrame();                                                // 0x224
            virtual void SetHockeySnatchFrame(float);                                            // 0x228
            virtual long GetHockeyFaceOffMode();                                                 // 0x22C
            virtual long GetHockeyCatchChrIdx();                                                 // 0x230
            virtual int IsHockeyShootGoalDir();                                                  // 0x234
            virtual void SetHockeyScaleSpeed(float);                                             // 0x238
            virtual long GetHockeyReBoundType() const;                                           // 0x23C
            virtual long GetHockeyItemBallType() const;                                          // 0x240
            virtual float GetHokeyBombExplodeRadius() const;                                     // 0x244
            virtual float GetHokeyBombShootDist() const;                                         // 0x248
            virtual int IsHockeyAttackMode() const;                                              // 0x24C
            virtual void GetHokeyCurveShootData(float &, bcs::FVEC3 &);                          // 0x250
            virtual float GetFaceOffGuideDispFrm();                                              // 0x254
            virtual void SettHockeyReflectMode(float);                                           // 0x258
            virtual int IsShootExceptionalStart();                                               // 0x25C
            virtual int IsDummyShootBall();                                                      // 0x260
            virtual float GetHockeyPassSpeed();                                                  // 0x264
            virtual void SetSleepMode();                                                         // 0x2AC
            virtual void SetJumpBallAttach();                                                    // 0x2B0
            virtual void SetJumpBallMode();                                                      // 0x2B4
            virtual void SetChrReflectMode(long, float);                                         // 0x2B8
            virtual void JumpBallFrameSet(int);                                                  // 0x2BC
            virtual int IsShootMode() const;                                                     // 0x2D4
            virtual int IsArtsShootMode() const;                                                 // 0x2D8
            virtual int IsCanbeTurnOver() const;                                                 // 0x2E0
            virtual int IsInvincible() const;                                                    // 0x2E8
            virtual void SetNoCatchCount(long);                                                  // 0x2F4
            virtual float GetDefCounter() const;                                                 // 0x300
            virtual long GetShotChrIdx() const;                                                  // 0x308
            virtual long GetPassChrIdx() const;                                                  // 0x310
            virtual long GetShotTeamIdx() const;                                                 // 0x318
            virtual long GetShotArea() const;                                                    // 0x320
            virtual long GetTargetGoalIdx() const;                                               // 0x328
            virtual void SetChargePower(float);                                                  // 0x32C
            virtual float GetChargePower() const;                                                // 0x330
            virtual void TrgBound(unsigned long);                                                // 0x344
            virtual unsigned long SIDtoSeID_(long);                                              // 0x360
            virtual void PlayBoundSe();                                                          // 0x364
            virtual void ResetParam_();                                                          // 0x3E8
            virtual void ResetOnAttach_();                                                       // 0x3EC
            virtual void ResetOnDetach_();                                                       // 0x3F0
            virtual void UpdateCollBody_();                                                      // 0x404
            virtual void CheckCollision_();                                                      // 0x408
            virtual void RotNormal_();                                                           // 0x414
            virtual void CreateRadarIcon_(nw4r::g3d::ResFile);                                   // 0x424
            virtual void CreateCollBody_();                                                      // 0x42C
            virtual void CreateCollAttack_();                                                    // 0x430
            virtual void LoopStateFreeMove();                                                    // 0x44C
            virtual void CtorStateAttach();                                                      // 0x454
            virtual void LoopStateAttach();                                                      // 0x458
            virtual void DtorStateAttach();                                                      // 0x45C
            virtual void CtorStatePass();                                                        // 0x460
            virtual void LoopStatePass();                                                        // 0x464
            virtual void DtorStatePass();                                                        // 0x468
            virtual void CtorStateNormalShoot();                                                 // 0x484
            virtual void LoopStateNormalShoot();                                                 // 0x488
            virtual void DtorStateNormalShoot();                                                 // 0x48C
            virtual void CtorStateChargeShoot();                                                 // 0x490
            virtual void LoopStateChargeShoot();                                                 // 0x494
            virtual void DtorStateChargeShoot();                                                 // 0x498
            virtual void CtorStateArtsShoot();                                                   // 0x4A8
            virtual void LoopStateArtsShoot();                                                   // 0x4AC
            virtual void DtorStateArtsShoot();                                                   // 0x4B0
            virtual void CtorStateItemShoot();                                                   // 0x4B4
            virtual void LoopStateItemShoot();                                                   // 0x4B8
            virtual void DtorStateItemShoot();                                                   // 0x4BC
            virtual void CtorStateJumpBall();                                                    // 0x4C0
            virtual void LoopStateJumpBall();                                                    // 0x4C4
            virtual void DtorStateJumpBall();                                                    // 0x4C8
            virtual void CtorStateGoal();                                                        // 0x4D8
            virtual void LoopStateGoal();                                                        // 0x4DC
            virtual void DtorStateGoal();                                                        // 0x4E0
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long); // 0x4E4
            virtual long CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &);          // 0x4E8
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);      // 0x4EC
            virtual void SetBound_(const bcs::FVEC3 &);                                          // 0x4F0

            virtual void SetFlag_DispOff();                                             // 0x4F4
            virtual void ResetFlag_DispOff();                                           // 0x4F8
            virtual void SetArtsType(long);                                             // 0x4FC
            virtual void SetReStartBallMode();                                          // 0x500
            virtual void CollSubCheckBG_(bcs::FVEC3 &, bcs::FVEC3 &);                   // 0x504
            // (meaning guessed) The puck's special shot is over; the
            // hockey ball has no body for it, Waluigi's copies do.
            virtual int TrgHockeyArtsOver(long);                                        // 0x508

        private:
            unsigned char m_Unk318[0x354 - 0x318]; // 0x318
        protected:
            // 0x354 (name guessed): what GetNoCatchCount gives (the hockey
            // ball's own override is not declared here).
            float m_fNoCatchCount;
        private:
            unsigned char m_Unk358[0x434 - 0x358]; // 0x358
        protected:
            // 0x434 (name guessed): flags; Waluigi's copies of the puck set
            // bits 0 and 1 when they are built.
            unsigned long m_nHockeyFlag;
        private:
            unsigned char m_Unk438[0x484 - 0x438]; // 0x438
    };
}

#endif /* !MSW_CBALLHOCKEY_HPP_ */
