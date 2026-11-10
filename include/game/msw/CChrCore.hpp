#ifndef MSW_CCHRCORE_HPP_
#define MSW_CCHRCORE_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"

struct _GXTexObj;

namespace msw {

    class CChrDrv;

    // One character on the court. A DOL class; each game module derives its
    // own (CChrCoreBoss, CChrCoreBasket, ...). bcs::CFsmQueue<CChrCore> is at
    // 0x00 and the two share the vptr at 0x5C; own members start at 0x60.
    //
    // INCOMPLETE: only what game/boss/CChrMarkerBoss.cpp needs is here.
    //  - The members stop at 0x179. The constructor in the DOL writes up to
    //    0x570, so the class must not be instantiated, embedded or derived
    //    from until the rest is added.
    //  - The virtual functions stop at slot 0x250 of about 800. Their order
    //    and names are the slots of __vt__Q23msw12CChrCoreBoss (BossF), read
    //    with the class name set back to CChrCore. All of them except the
    //    destructor, GetMainOrderValue and IsNowFallOut are unnamed inline
    //    functions in the DOL; they are declared without bodies here, so a
    //    unit that has to emit this vtable needs the bodies first.
    //  - Return types are not part of the mangled names: "int" stands for a
    //    DOL body that returns a word, "float" for one that loads f1, "void"
    //    for an empty one. All are guesses, as are the member names.
    class CChrCore : public bcs::CFsmQueue<CChrCore> {
        public:
            CChrCore();
            virtual ~CChrCore();                                     // 0x08
            virtual void DispFunc();                                 // 0x0C
            virtual long GetQueueWorkSize() const;                   // 0x10
            virtual long GetQueueStackSize() const;                  // 0x14
            virtual long GetMainOrderValue(long);                    // 0x18
            virtual void *GetHoldBall() const;                       // 0x1C
            virtual unsigned long GetHoldBallHandle() const;         // 0x20
            virtual void SetConditionEffect_(long);                  // 0x24
            virtual void SetConditionEffect_(long, unsigned long);   // 0x28
            virtual void GetSpRootPos(bcs::FVEC3 &) const;           // 0x2C
            virtual int IsNowBlockWork() const;                      // 0x30
            virtual int IsNowFallOut() const;                        // 0x34
            virtual int IsHoldBall() const;                          // 0x38
            virtual int IsHoldItem() const;                          // 0x3C
            virtual int IsHoldAny() const;                           // 0x40
            virtual int Is1on1() const;                              // 0x44
            virtual float Get1on1StartDist() const;                  // 0x48
            virtual int IsAirTrickSwingEnd() const;                  // 0x4C
            virtual int IsAirTrickFall() const;                      // 0x50
            virtual float GetAirTrickStatePassFrame() const;         // 0x54
            virtual void GetTargetMarkerPos(bcs::FVEC3 &);           // 0x58
            virtual int MoveTargetMarkerPos(float);                  // 0x5C
            virtual int AutoMoveTargetMarkerPos();                   // 0x60
            virtual long CheckGuideTelopDisp();                      // 0x64
            virtual int IsBattle() const;                            // 0x68
            virtual int IsDisp1On1Maker() const;                     // 0x6C
            virtual int IsDunkBlockEnable() const;                   // 0x70
            virtual int CanbePassRequest();                          // 0x74
            virtual int MethdPassRequest();                          // 0x78
            virtual float GetBlockWorkDir() const;                   // 0x7C
            virtual float GetJumpBlockLocalFrame() const;            // 0x80
            virtual int GetStarRemainderRate() const;                // 0x84
            virtual int IsAirTrickDunkMode() const;                  // 0x88
            virtual int IsVolleyRightBallTouch(long) const;          // 0x8C
            virtual int IsVolleyServeMode() const;                   // 0x90
            virtual int IsVolleyJumpServeMode() const;               // 0x94
            virtual int IsVolleyTargetMarkerActive() const;          // 0x98
            virtual long GetVolleyTargetMarkerNum() const;           // 0x9C
            virtual void GetVolleyTargetMarkerDefaultPos(long, bcs::FVEC3 &, int); // 0xA0
            virtual long GetVolleyTargetMarkerType() const;          // 0xA4
            virtual long GetVolleyTargetMarkerFixedType() const;     // 0xA8
            virtual int GetVolleyTargetMarkerPos(bcs::FVEC3 &, long); // 0xAC
            virtual int GetVolleyTargetMarkerMoved(bcs::FVEC3 &);    // 0xB0
            virtual int MoveVolleyTargetMarkerPos(float);            // 0xB4
            virtual int CanbeMoveVolleyTargetMarkerPos();            // 0xB8
            virtual int IsVolleyTargetMarkerStealthMode();           // 0xBC
            virtual float GetVolleySpikeAdsorbRangeXZ(int) const;    // 0xC0
            virtual float GetVolleySpikeAdsorbRangeY(int) const;     // 0xC4
            virtual int IsVolleySpikeJumpStart() const;              // 0xC8
            virtual int IsVolleySuperSpikeJump() const;              // 0xCC
            virtual int IsVolleySpikeSwingEnd() const;               // 0xD0
            virtual int IsVolleySpikeFall() const;                   // 0xD4
            virtual int IsVolleyAirTrickSwingEnd() const;            // 0xD8
            virtual int IsVolleyAirTrickFall() const;                // 0xDC
            virtual int IsVolleyBlockJumpStart() const;              // 0xE0
            virtual int IsVolleyBlockFall() const;                   // 0xE4
            virtual int IsVolleyBlockSuccess() const;                // 0xE8
            virtual int IsVolleyDispSpikeJumpMarker() const;         // 0xEC
            // 0xF0-0x250: names from __vt__Q23msw13CChrCoreDodge (DodgeF);
            // return types from the weak CChrCore bodies the modules emit.
            virtual int GetVolleySpikeFeintTargetPos(float, bcs::FVEC3 &, bcs::FVEC3 &, bcs::FVEC3 &); // 0xF0
            virtual int GetVolleyBallShotCollPos(long, bcs::FVEC3 &, float &) const; // 0xF4
            virtual void GetVolleySpikeOffsetPos(long, bcs::FVEC3 &) const; // 0xF8
            virtual void *GetVolleyTargetBall() const;               // 0xFC
            virtual int IsVolleySuperSpikeCurveType() const;         // 0x100
            virtual float GetVolleySuperSpikeCurveRate() const;      // 0x104
            virtual float GetVolleySuperSpikeCurveMaxDist() const;   // 0x108
            virtual float GetVolleySuperSpikeCurveMinDist() const;   // 0x10C
            virtual float GetVolleyBlockJumpDelayFrame() const;      // 0x110
            virtual int IsVolleyEnableDiveReceive() const;           // 0x114
            virtual int IsVolleyCombinationMode() const;             // 0x118
            virtual long GetVolleyCombinationRole() const;           // 0x11C
            virtual long GetVolleyCombinationStep() const;           // 0x120
            virtual void SetVolleyCombinationRoleDown();             // 0x124
            virtual int IsVolleyDispExIcon(long) const;              // 0x128
            virtual long CheckVolleyLineMsgDisp();                   // 0x12C
            virtual int IsVolleyCombinationReceiver() const;         // 0x130
            virtual int IsVolleyCombinationAttacker() const;         // 0x134
            virtual int IsVolleyArtsKoopaShotFire() const;           // 0x138
            virtual int IsVolleyArtsKoopaFireEnd() const;            // 0x13C
            virtual long GetVolleyArtsExInput();                     // 0x140
            virtual int IsHockeyChgArrowMarker();                    // 0x144
            virtual float GetHockeyArrowMarkerRotY() const;          // 0x148
            virtual int IsHockeyTackleMarker() const;                // 0x14C
            virtual int IsHockeyTgtArrowMarker() const;              // 0x150
            virtual void GetHockeyShootGoalTgtPos(bcs::FVEC3 &, float &); // 0x154
            virtual int IsHockeyDispShootMarker();                   // 0x158
            virtual void SetHockeyNoChargeCutFrame(float);           // 0x15C
            virtual float GetHockeyNoChargeCutFrame();               // 0x160
            virtual float GetShootChargeFrameMax() const;            // 0x164
            virtual int GetHockeyKeeperTgtPos(bcs::FVEC3 &);         // 0x168
            virtual int IsKeeper() const;                            // 0x16C
            virtual void SetKeeper(int);                             // 0x170
            virtual void SetMaxSkateSpeed(float);                    // 0x174
            virtual float GetMaxSkateSpeed() const;                  // 0x178
            virtual float GetHockeyVelXZ() const;                    // 0x17C
            virtual float GetBallScramblePoint() const;              // 0x180
            virtual float GetBallScrambleNowPoint() const;           // 0x184
            virtual float GetBallScrambleWinPoint() const;           // 0x188
            virtual float GetBallScrambleMaxPoint() const;           // 0x18C
            virtual int GetBodyContactSkate() const;                 // 0x190
            virtual void SetBodyContactSkate(int);                   // 0x194
            virtual void SetPackNotCatchFrame(float);                // 0x198
            virtual int IsHockeyBodyNoPush() const;                  // 0x19C
            virtual long GetHockeyMaxShootCourseDirPercent();        // 0x1A0
            virtual int GetHockeyShootGoalTgtPos(bcs::FVEC3 &);      // 0x1A4
            virtual int MethdHockeyKeeperQuickMove(const bcs::FVEC3 &); // 0x1A8
            virtual int IsHockeyDamage() const;                      // 0x1AC
            virtual int IsHockeyBallHit();                           // 0x1B0
            virtual int IsHockeyAttack() const;                      // 0x1B4
            virtual int IsHockeyFighting() const;                    // 0x1B8
            virtual void HockeyChrReflect(bcs::FVEC3 &, CChrCore *); // 0x1BC
            virtual long Get1on1AttackHitType() const;               // 0x1C0
            virtual long GetNow1on1AttackNum() const;                // 0x1C4
            virtual float Get1on1AttackWaitTimeMax() const;          // 0x1C8
            virtual void SetUseShootMarker(int);                     // 0x1CC
            virtual int IsUseShootMarker() const;                    // 0x1D0
            virtual int MethdHockeySetMoveRevise(float, float);      // 0x1D4
            virtual void HockeyArtsEnd();                            // 0x1D8
            virtual int CanbeKeeperDivingCatch();                    // 0x1DC
            virtual int MethdKeeperDivingCatch();                    // 0x1E0
            virtual float GetAttackPower(int);                       // 0x1E4
            virtual long GetDamageCollKind() const;                  // 0x1E8
            virtual float GetHockeyBodyCollRadiusNow() const;        // 0x1EC
            virtual float GetHockeyMass() const;                     // 0x1F0
            virtual float GetFaceOffChargeMaxFrame() const;          // 0x1F4
            virtual long GetTackleCount(long) const;                 // 0x1F8
            virtual void SetAddTackleCount(long);                    // 0x1FC
            virtual void ReSetTackleCount(long);                     // 0x200
            virtual int CanbeHockeyComBlockAttack();                 // 0x204
            virtual int IsHockeyInvisible() const;                   // 0x208
            virtual void SetHockeyAttackObjNull(long);               // 0x20C
            virtual int GetHockeyAttackObjInfo(long, bcs::FVEC3 &, float &, bcs::FVEC3 &); // 0x210
            virtual long GetHockeyAttackObjMax();                    // 0x214
            virtual void SetKeeperCatchMode();                       // 0x218
            virtual float GetHockeyPassSpeed();                      // 0x21C
            virtual float GetHockeyKeeperPassSpeed();                // 0x220
            virtual int IsHockeyStatePaintTumble() const;            // 0x224
            virtual void SetDodgeArrangeArea(long);                  // 0x228
            virtual long GetDdogeArrangeArea() const;                // 0x22C (sic)
            virtual int IsDodgeInfield() const;                      // 0x230
            virtual int IsDodgeCloseRange() const;                   // 0x234
            virtual float GetDodgeCatchCollRadius() const;           // 0x238
            virtual long GetDodgeLockTargetChrIdx() const;           // 0x23C
            virtual void SwitchAutoAimTarget();                      // 0x240
            virtual int IsDodgeTargetMarkerActive();                 // 0x244
            virtual long GetDodgeTargetMarkerType() const;           // 0x248
            virtual long GetDodgeTargetMarkerFixedType() const;      // 0x24C
            // Not zero when it wrote the marker's position to the vector.
            virtual int GetDodgeTargetMarkerPos(bcs::FVEC3 &);       // 0x250
            // ... the vtable goes on from here (see above).

            // Not zero when the character takes no part in the game: it has
            // no model yet (the word at 0x90 is zero) or bit 30 of the word
            // at 0xEC is set.
            int IsDisable() const;
            // The type the current driver reports; zero without a driver.
            // CChrMarkerBoss only shows markers for type 1 (most likely the
            // human players; not checked).
            long DriverGetType() const;
            // DOL. The yaw (radians) from the character towards a point,
            // in the XZ plane.
            float GetRotY_ToPosXZ(const bcs::FVEC3 &pos) const;

            // (names guessed) Inline in the original; the module reads the
            // members directly (CChrMarkerBoss::AppearCheck loads the byte
            // unsigned).
            long GetChrId() const { return m_nChrId; }
            unsigned long GetStatusFlag() const { return m_nStatusFlag; }
            unsigned long GetDispFlag() const { return m_nDispFlag; }

            // The texture of the character's radar icon; null when
            // IsDisable() is true. In the DOL. (that the pointer is to a
            // GXTexObj is read from the caller, which makes a
            // bcs::G2D::CTexMap from it; const guessed)
            const _GXTexObj *GetRadarIconTexObj() const;
            // (names guessed) Inline in the original; the modules read the
            // bytes directly, as signed values.
            long GetPlayerNo() const { return m_nPlayerNo; }
            long GetTeamNo() const { return m_nTeamNo; }

        protected:
            unsigned long m_Unk60;
            CChrDrv *m_pDriver[2];       // 0x64 (count guessed from the index's place)
            long m_nDriver;              // 0x6C, index of the driver in use, negative for none
            unsigned char m_Unk70[0x78];
            unsigned long m_nStatusFlag; // 0xE8, meaning of the bits not known
            unsigned long m_UnkEC;
            // 0xF0 (name guessed): CChrCore::UpdateDisp_ draws nothing while
            // bit 2 is set, and CChrMarkerDodge shows no marker then.
            unsigned long m_nDispFlag;
            unsigned char m_UnkF4[0x80];
            unsigned char m_nChrId;      // 0x174, a character number (meaning guessed)
            unsigned char m_Unk175;
            // (names guessed) What CRadarIcon2D<Game>::CreateCharaIcon passes
            // to CGame::GetCharType and to CGame::GetTeamColorIdx.
            signed char m_nPlayerNo;     // 0x176
            unsigned char m_Unk177;
            signed char m_nTeamNo;       // 0x178
    };
}

#endif /* !MSW_CCHRCORE_HPP_ */
