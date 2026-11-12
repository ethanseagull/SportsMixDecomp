#ifndef MSW_CBALLBOSS_HPP_
#define MSW_CBALLBOSS_HPP_

#include "game/msw/CBall.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"

namespace msw {

    // The ball of the boss game (BossF), size 0x448: CBall plus 0x130 bytes
    // of its own (read from the constructor). Mapped from
    // game/boss/CBallBoss.cpp; the names of most members are guesses, and
    // where the meaning is not known the name is the offset.
    class CBallBoss : public CBall {
        public:
            // (name and layout guessed) The flight CBallBoss::SetPassModeParam_
            // and SetBadPassParam_ work out for a pass.
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

            // Bits of m_nBossFlag (names guessed where noted).
            enum {
                // The ball rides on the object m_nRideObjHandle.
                BOSSFLAG_RIDE = 0x1,
                // Set when the item effect asks for it (TrgItemEffect_ bit 0x10).
                BOSSFLAG_ITEM_HIT = 0x2
            };

            CBallBoss(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallBoss();
            virtual void Disp();

            virtual void Reset();
            virtual float GetRadius() const;
            virtual float GetBgCollRadius() const;
            virtual float GetBgCollRadiusXZ() const;
            virtual float GetCollCatchRadiusDefault() const { return 0.1f; }
            virtual int GetTargetPos(bcs::FVEC3 &rPos) const;
            virtual long AttachItemEffect(long nKind);
            virtual void GetVolleyPosAfterFrame(bcs::FVEC3 &rPos, float fFrame, int) const;
            virtual long GetVsBossBallMode() const { return m_nVsBossBallMode; }
            virtual void SetVsBossTossUpMode(const bcs::FVEC3 &rPos, long, unsigned long);
            virtual int SetVsBossSpikeShotMode(BALL::BOSS::SPIKE_SHOT_ARG &rArg);
            virtual void SetVsBossPassMode(const bcs::FVEC3 &rPos, long, long, long, long, long, unsigned long);
            virtual void SetVsBossShootMode(const bcs::FVEC3 &rPos, long, float);
            virtual void SetVsBossDunkShotMode(const bcs::FVEC3 &rPos, long);
            virtual void SetVsBossArtsDunkMode(const bcs::FVEC3 &rPos, long, int);
            virtual int CanbeVsBossReceive() const;
            virtual int CanbeVsBossReflect() const;
            virtual int CanbeVsBossAirTrick() const;
            virtual void SetVsBossReflectCheckEnd();
            virtual void SetVsBossCoinPower(long nPower);
            virtual void AddVsBossCoinPower(long nPower);
            virtual long GetVsBossCoinPower() const;
            virtual void ClearVsBossCoinPower();
            virtual void TrgVsBossArtsAttach(long, long, long, int);
            virtual void SetJumpBallMode();
            virtual void SetChrReflectMode(long, float);
            virtual int IsActive() const;
            virtual int IsSleepMode() const;
            virtual int IsFreeMoveMode() const;
            virtual int IsPassMode() const;
            virtual int IsAlleyoopPassMode() const;
            virtual int IsShootMode() const;
            virtual int IsArtsShootMode() const;
            virtual int IsCanbeTurnOver() const;
            virtual int IsInvincible() const;
            virtual int CanbeCatch() const;
            virtual long GetShotChrIdx() const { return m_nShotChrIdx; }
            virtual long GetPassChrIdx() const { return m_nPassChrIdx; }
            virtual long GetShotTeamIdx() const { return m_nShotTeamIdx; }
            virtual long GetTouchNum() const { return GetVolleyTouchNum(-1); }
            virtual void SetShotChrIdx(long nChr) { m_nShotChrIdx = nChr; }
            virtual void SetPassChrIdx(long nChr) { m_nPassChrIdx = nChr; }
            virtual void SetShotTeamIdx(long nTeam) { m_nShotTeamIdx = nTeam; }
            virtual unsigned long SIDtoSeID_(long nSe) { return nSe; }
            virtual int CanbeVolleyPickup(long) const { return m_n410; }
            virtual long GetTargetCourtIdx() const;
            virtual int IsTargetCourt(long nCourt) const;
            virtual void TrgGoalIn(long, long, long, long, long);
            virtual void PlayBoundSe();
            virtual void PlayFloorHitSe(unsigned long);
            virtual float GetTouchDownFrame(float) const;
            virtual float GetTouchDownFrame2(float, float) const;
            virtual void GetPosAfterFrame(bcs::FVEC3 &rPos, float fFrame) const;
            virtual void GetVelAfterFrame(bcs::FVEC3 &rVel, float fFrame) const;
            virtual float GetHeightPassFrame(float, int) const;
            virtual void ResetParam_();
            virtual void ResetOnAttach_();
            virtual void ResetOnDetach_();
            virtual void SetHighlight_();
            virtual void UpdateRainbow_();
            virtual void UpdateCollAtk_();
            virtual void UpdateCollBody_();
            virtual void CheckCollision_();
            virtual void UpdateDisp_();
            virtual void CreateMdlObj_(nw4r::g3d::ResFile file, nw4r::g3d::ResMdl mdl, nw4r::g3d::ResMdl shadowMdl,
                                       nw4r::g3d::ResMdl refMdl, long nShadow);
            virtual void CreateCollBody_();
            virtual void CtorStateFreeMove();
            virtual void LoopStateFreeMove();
            virtual void DtorStateFreeMove();
            virtual void CtorStateAttach();
            virtual void LoopStateAttach();
            virtual void DtorStateAttach();
            virtual void CtorStatePass();
            virtual void LoopStatePass();
            virtual void DtorStatePass();
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
            virtual void CtorStateJumpBall();
            virtual void LoopStateJumpBall();
            virtual void DtorStateJumpBall();
            virtual void CtorStateTouchDown();
            virtual void LoopStateTouchDown();
            virtual void DtorStateTouchDown();
            virtual long CallbackBodyDamage_(const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual void SetBound_(const bcs::FVEC3 &rNormal);

            // New virtual functions (slots 0x4F4 to 0x514).
            virtual void GetVolleyVelAfterFrame(bcs::FVEC3 &rVel, float fFrame, int) const;  // 0x4F4
            virtual void CollSubCheckTgtPos_();                                             // 0x4F8
            virtual int CollSubCheckNet_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel);               // 0x4FC
            virtual void AttachTailEffect_();                                               // 0x500
            virtual void TrgHitFloor_();                                                    // 0x504
            virtual void CmnShotInit_();                                                    // 0x508
            virtual void SetTouch_(long, long);                                             // 0x50C
            virtual int CanbeTouchDown_();                                                  // 0x510
            virtual void SetSpikeJumpTgtWork_(float);                                       // 0x514

            void InitArtsShotStep_();
            void UpdateCountWork_();
            void FuncMove_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float fGravity, float fRate);
            int CollSubCheckBG_(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel);
            void SetPassModeParam_(const bcs::FVEC3 &rPos, float, float, float, float);
            void SetBadPassParam_(long, long, PASS_PARAM_ &rParam);
            int SetSpikeParam_(BALL::BOSS::SPIKE_SHOT_ARG &rArg);
            float CalcXZVel2(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, bcs::FVEC3 &rVel, float &, float &rFrame,
                             float, float, float, float) const;

        private:
            // (name guessed) Counts the spin work at 0x36C down (several
            // states share it).
            inline void UpdateSpin_();
            // (name guessed) Switches the model's colour animation to the
            // one of power level n.
            inline void ChangePowerColor_(long n);
            // (names guessed) Stops the spin; tells the cameras where the
            // ball is heading (never in the first mode while m_bCollOff is
            // set); lets the ball go loose after a hit.
            inline void ClearSpin_();
            inline void UpdateCameraTarget_(int bFlag);
            inline void SetLooseBall_();
            // (names guessed) Keeps the shooter's own body from hitting the
            // ball; starts a flight towards rTarget from m_Pos; the
            // acceleration of fTime frames of flight.
            inline void SetShotChrNoHit_();
            inline void InitFlight_(const bcs::FVEC3 &rTarget);
            inline void CalcFlightAccel_(bcs::FVEC3 &rAccel, const bcs::FVEC3 &rDir, float fDecel, float fGravity,
                                         float fTime, int bSpin) const;

        protected:
            long m_Unk318;                  // 0x318
            unsigned long m_nBossFlag;      // 0x31C (name guessed)
            float m_f320;                   // 0x320
            long m_nShotChrIdx;             // 0x324
            long m_n328;                    // 0x328
            long m_nPassChrIdx;             // 0x32C
            long m_nShotTeamIdx;            // 0x330
            long m_n334;                    // 0x334
            long m_n338;                    // 0x338
            long m_n33C;                    // 0x33C
            long m_n340;                    // 0x340
            float m_fFadeCount;             // 0x344 (name guessed: counts the touch-down fade out)
            float m_f348;                   // 0x348
            long m_n34C;                    // 0x34C
            float m_f350;                   // 0x350, counted down by m_fMoveRate
            float m_f354;                   // 0x354, counted down by m_fMoveRate
            float m_f358;                   // 0x358, counted down by m_fMoveRate
            long m_n35C;                    // 0x35C
            long m_n360;                    // 0x360
            float m_f364;                   // 0x364
            float m_f368;                   // 0x368
            // 0x36C-0x37C (names guessed): a spin given to the ball (kind
            // 1 to 3) and its timer and parameters.
            long m_nSpinType;
            float m_fSpinCount;             // 0x370
            bcs::FVEC3 m_vSpinAccel;         // 0x374
            float m_f380;                   // 0x380, counted down by m_fMoveRate
            long m_n384;                    // 0x384
            long m_n388;                    // 0x388
            float m_f38C;                   // 0x38C
            long m_n390;                    // 0x390
            long m_n394;                    // 0x394
            float m_f398;                   // 0x398
            float m_f39C;                   // 0x39C
            float m_f3A0;                   // 0x3A0
            long m_n3A4;                    // 0x3A4
            float m_f3A8;                   // 0x3A8
            float m_f3AC;                   // 0x3AC
            float m_f3B0;                   // 0x3B0
            float m_f3B4;                   // 0x3B4
            float m_f3B8;                   // 0x3B8
            float m_f3BC;                   // 0x3BC
            long m_n3C0;                    // 0x3C0
            long m_n3C4;                    // 0x3C4
            // 0x3C8 (name guessed): not zero while the ball is out of play
            // for its own collision (attack and body units disabled).
            long m_bCollOff;
            bcs::FVEC3 m_v3CC;              // 0x3CC, (0, 1, 0) after reset
            long m_n3D8;                    // 0x3D8
            long m_n3DC;                    // 0x3DC
            // 0x3E0 (name guessed): a floor hit sound to play once instead
            // of CBall's, or -1.
            long m_nFloorHitSe;
            long m_n3E4;                    // 0x3E4
            float m_f3E8;                   // 0x3E8
            float m_f3EC;                   // 0x3EC
            float m_f3F0;                   // 0x3F0
            float m_f3F4;                   // 0x3F4
            float m_f3F8;                   // 0x3F8
            unsigned long m_nRideObjHandle; // 0x3FC (name guessed)
            // 0x400 (name guessed): where the object the ball rides on
            // (handle m_n3FC, while BOSSFLAG_1 is set) was last frame.
            bcs::FVEC3 m_vRideObjPos;
            long m_n40C;                    // 0x40C
            long m_n410;                    // 0x410
            float m_f414;                   // 0x414
            long m_nVsBossBallMode;         // 0x418
            long m_bReflectCheckEnd;        // 0x41C (name guessed)
            long m_nCoinPower;              // 0x420, 0 to 100
            long m_nPrevCoinPower;          // 0x424 (name guessed)
            long m_n428;                    // 0x428
            long m_n42C;                    // 0x42C
            // 0x430: the colour animations "power00" to "power05" of the
            // ball's file (names guessed).
            nw4r::g3d::ResAnmClr m_aPowerAnmClr[6];
    };
}

#endif /* !MSW_CBALLBOSS_HPP_ */
