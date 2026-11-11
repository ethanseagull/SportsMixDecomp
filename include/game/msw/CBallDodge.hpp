#ifndef MSW_CBALLDODGE_HPP_
#define MSW_CBALLDODGE_HPP_

#include "game/msw/CBall.hpp"

namespace msw {

    // The ball of the dodge game (DodgeF), size 0x4A8 (CBallManDodge
    // allocates it): CBall plus 0x190 bytes of its own.
    //
    // INCOMPLETE: only what game/dodge/CBallManDodge.cpp needs. The class
    // overrides most of CBall's virtual functions (not declared here) and
    // adds the ones below (slots 0x4F4 to 0x5B8 of __vt__Q23msw10CBallDodge;
    // return types guessed from the names except where a caller shows
    // them). The members are not mapped.
    class CBallDodge : public CBall {
        public:
            // (values not known) Kind of hit effect.
            enum EF_TYPE {
                EF_TYPE_0
            };

            CBallDodge(bcs::CTask *pParent, long nBallNo);
            virtual ~CBallDodge();

            virtual void SetFlag_DispOff();                                                         // 0x4F4
            virtual void ResetFlag_DispOff();                                                       // 0x4F8
            virtual void SetDodgeSideShootMode(long, long, long, float, unsigned long);             // 0x4FC
            virtual void SetDodgeCurveShootMode(long, long, long, float, unsigned long);            // 0x500
            virtual void SetDodgeShootModeEx(long, long, long, long, float, unsigned long);         // 0x504
            virtual void SetDodgeShootModeEx(long, const bcs::FVEC3 &, long, long, long, float, unsigned long);// 0x508
            virtual void SetDodgeShootModeEx(const bcs::FVEC3 &, const bcs::FVEC3 &, long, long, long, float, unsigned long);// 0x50C
            virtual void SetOutBallModeRaw(const bcs::FVEC3 &, long, long);                         // 0x510
            // A message from another ball (CBallManDodge::NotifyMessage_).
            // Zero when the ball takes it. (meaning of the result guessed)
            virtual int OnMessage(CBall *pFrom, long nMsg);                                     // 0x514
            virtual long GetDodgeDamage(long, long *, long *) const;                                // 0x518
            virtual float GetDmgEffectFrame() const;                                                // 0x51C
            virtual long GetTargetChrIdx() const;                                                   // 0x520
            virtual void DryTrgScore(long);                                                         // 0x524
            virtual long GetLastTouchChrIdx() const;                                                // 0x528
            virtual float GetShootFlyDist() const;                                                  // 0x52C
            virtual void SetGravity(float);                                                         // 0x530
            virtual long GetHitResult();                                                            // 0x534
            virtual long GetFlickResult();                                                          // 0x538
            virtual long GetCatchResult();                                                          // 0x53C
            virtual long GetOuterHitResult();                                                       // 0x540
            virtual long GetFriendHitResult();                                                      // 0x544
            virtual float GetFloorBoundDecRateXZ();                                                 // 0x548
            virtual float GetFloorBoundDecRateY();                                                  // 0x54C
            virtual float GetFloorRoundDecRateXZ();                                                 // 0x550
            virtual float GetWallBoundDecRateXZ();                                                  // 0x554
            virtual float GetWallBoundTime();                                                       // 0x558
            virtual long GetShootSe(long);                                                          // 0x55C
            virtual long GetArtsHitEffect(EF_TYPE) const;                                           // 0x560
            virtual void CreateArtsHitEffect(EF_TYPE, bcs::CMdlObj *, unsigned long);               // 0x564
            virtual void CreateArtsHitEffect(EF_TYPE, const bcs::FVEC3 &);                          // 0x568
            virtual void CreateArtsHitEffect(EF_TYPE, const bcs::FVEC3 &, const bcs::FVEC3 &);      // 0x56C
            virtual void CreateArtsExplosion();                                                     // 0x570
            virtual void CreateGimmickHitEffect(EF_TYPE, const bcs::FVEC3 &);                       // 0x574
            virtual void CreateGimmickHitEffect(EF_TYPE, const bcs::FVEC3 &, const bcs::FVEC3 &);   // 0x578
            virtual long GetCatchType() const;                                                      // 0x57C
            virtual float GetCatchPower() const;                                                    // 0x580
            virtual long GetItemType() const;                                                       // 0x584
            virtual float GetFlyCount() const;                                                      // 0x588
            virtual float GetShotFlyCount() const;                                                  // 0x58C
            virtual void SetShotFlyCount(float);                                                    // 0x590
            virtual float CalcStaggerRange_(float) const;                                           // 0x594
            virtual float CalcAtkRange_(float) const;                                               // 0x598
            virtual float CalcShootSpeed_(long, long) const;                                        // 0x59C
            virtual void AttachEffect_(unsigned long, long, float);                                 // 0x5A0
            virtual void DetachEffect_(unsigned long);                                              // 0x5A4
            virtual void DetachEffectAll_();                                                        // 0x5A8
            virtual void ReattachEffect_(unsigned long);                                            // 0x5AC
            virtual void LandingEffect_();                                                          // 0x5B0
            virtual void SetOutOfBounds();                                                          // 0x5B4
            virtual long GetWallHitEffect() const;                                                  // 0x5B8

            // (added for DodgeF game/dodge/CItemBallBombDodge.cpp; meaning
            // guessed) Turns the ball towards pChr by at most fMaxRot
            // (radians); zero when it could not.
            int HomingUpdate(const CChrCore *pChr, float fMaxRot, float, float);

            unsigned char m_Unk318[0x3C]; // 0x318
            // 0x354 (name guessed): cleared by
            // CItemBallBaseDodge::CancelOffensive_.
            long m_nOffensive;
            unsigned char m_Unk358[0x8];  // 0x358
            // 0x360 (meaning not known): the bomb item clears it when it is
            // shot.
            long m_nUnk360;
            unsigned char m_Unk364[0x144]; // 0x364
    };
}

#endif /* !MSW_CBALLDODGE_HPP_ */
