#ifndef MSW_CCOLLMAN_HPP_
#define MSW_CCOLLMAN_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/FVEC4.hpp"

namespace msw {

    // Collision of the ball and the characters with the court. A DOL class,
    // size 0x7C: bcs::CTask at 0x00, own members from 0x34 (read from the
    // constructor and the small virtual functions in the DOL). Each game
    // module derives its own manager from it.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw12CCollManBoss (BossF). Bodies given here are the ones the
    // modules emit as weak functions (the DOL's copies have no name); the
    // functions without a body are DOL functions. Return types are not part
    // of the mangled names: "int" stands for a body that returns zero or a
    // word, "void" for an empty body; all are guesses, and so are the names
    // of the parameters and members.
    class CCollMan : public bcs::CTask {
        public:
            CCollMan(bcs::CTask *pParent);
            virtual ~CCollMan();
            virtual void Main();
            virtual void Disp();

            virtual int IsEnableViewColl_() const;
            virtual long GetViewCollTargetDir_() const;
            virtual float GetViewCollPosZ_() const;
            virtual void SetArtsExColl_(long n) { m_nArtsExColl = n; }
            virtual long GetArtsExColl_() const { return m_nArtsExColl; }
            // Keeps a sphere of radius `fRadius` that moved from rFrom to rTo
            // inside the court: rPos gets the corrected position and rNormal
            // the direction it was pushed back in. Returns zero when nothing
            // was hit.
            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long, unsigned long);
            virtual int DetectBallGoalIn_(long &, bcs::FVEC3 &, bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &,
                                          float)
            {
                return 0;
            }
            virtual int DetectBallGoalOut_(long &, bcs::FVEC3 &, bcs::FVEC3 &, const bcs::FVEC3 &, const bcs::FVEC3 &,
                                           float)
            {
                return 0;
            }
            virtual int CheckMapInner_(const bcs::FVEC3 &rPos, float fRadius, unsigned long);
            virtual void EnableGoalGuardian_(long, int) {}
            virtual void SetGoalGuardianPos_(long, const bcs::FVEC3 &) {}
            virtual void SetGoalGuardianRotY_(long, float) {}
            virtual void SetGoalGuardianSize_(long, const bcs::FVEC3 &) {}
            // The next two are called by the constructor.
            virtual void ResetViewColl_();
            virtual void UpdateViewColl_();
            virtual void ResetExColl_();
            virtual void EnableExColl_(long n, int bEnable);
            virtual void SetExCollPos_(long n, const bcs::FVEC3 &rPos);
            virtual void SetExCollRadius_(long n, float fRadius);

            // These call the function of the same name with a trailing
            // underscore on m_pInstance and do nothing without one.
            static int DetectMapHit(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                    const bcs::FVEC3 &rTo, float fRadius, long, unsigned long);
            static int CheckMapInner(const bcs::FVEC3 &rPos, float fRadius, unsigned long);
            static void SetArtsExColl(long n);
            // (DOL) The ground attribute under rPos (the stage's
            // GetFloorAttribute) into rAttr and the floor's flow there (its
            // GetFloorFlow) into rFlow. 8 is a hole (msw::CDropCoin).
            static void GetGrdAttr(const bcs::FVEC3 &rPos, unsigned long &rAttr, bcs::FVEC3 &rFlow);
            // (DOL) Bit 1 of a ground attribute: the floor moves things.
            static int IsGrdAttrFlow(unsigned long attr);

            static CCollMan *m_pInstance;

        protected:
            // DOL functions the modules' DetectMapHit_ build on. rRect is a
            // rectangle on the ground (left, right, front, back): the Inner
            // one keeps the sphere inside it, the Outer ones keep it out of
            // it (OuterRgnZ_ only along z). Bit 31 of the flags is passed
            // through. What the float after the rectangle is for is not known
            // (BasketF passes 0).
            int DetectMapHitInnerRgn_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC4 &rRect,
                                      float, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius,
                                      unsigned long flags);
            int DetectMapHitOuterRgn_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC4 &rRect,
                                      float, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius,
                                      unsigned long flags);
            int DetectMapHitOuterRgnZ_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC4 &rRect,
                                       float, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius,
                                       unsigned long flags);
            // The same for a circle around rCenter of radius fCircleRadius
            // (DodgeF passes one of the m_ExCollPos/m_fExCollRadius pairs).
            int DetectMapHitInnerCircle_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                         const bcs::FVEC3 &rCenter, float fCircleRadius, const bcs::FVEC3 &rFrom,
                                         const bcs::FVEC3 &rTo, float fRadius, unsigned long flags);
            int DetectMapHitOuterCircle_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime,
                                         const bcs::FVEC3 &rCenter, float fCircleRadius, const bcs::FVEC3 &rFrom,
                                         const bcs::FVEC3 &rTo, float fRadius, unsigned long flags);

            int m_bEnableViewColl;       // 0x34
            long m_Unk38;                // 0x38, zero from ResetViewColl_
            long m_nViewCollTargetDir;   // 0x3C, -1 from ResetViewColl_
            float m_Unk40;               // 0x40, added to or taken from m_Unk44.z by GetViewCollPosZ_
            bcs::FVEC3 m_Unk44;          // 0x44 (that the three floats are one vector is a guess)
            int m_bExCollEnable[2];      // 0x50
            float m_fExCollRadius[2];    // 0x58
            bcs::FVEC3 m_ExCollPos[2];   // 0x60
            long m_nArtsExColl;          // 0x78, -1 from the constructor
    };
}

#endif /* !MSW_CCOLLMAN_HPP_ */
