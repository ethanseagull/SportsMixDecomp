#ifndef MSW_CCOLLMAN_HPP_
#define MSW_CCOLLMAN_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/FVEC4.hpp"

namespace msw {

    // DOL collision routines outside CCollMan (Q23msw4COLL: a namespace or a
    // class of statics). Added for HockeyF game/hockey/CCollManHockey.cpp.
    namespace COLL {
        // Non-zero when a sphere of radius fRadius moving from rFrom to rTo
        // hits the box at rCenter with half-sizes rSize turned by rotY about
        // Y; rHit gets where. (meaning of the arguments read from the caller)
        int DetectHitPos_CapsuleCuboid(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius,
                                       const bcs::FVEC3 &rCenter, const bcs::FVEC3 &rSize, float rotY,
                                       bcs::FVEC3 &rHit);
    }

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
            // HockeyF's CStgGmk_Pinball_Act_Hockey::WallHitCheck compares the
            // result unsigned with 0x20 and 0x40 (a goal post hit).
            static unsigned long DetectBallGoalIn(long &rGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal,
                                                  const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius);
            static void SetArtsExColl(long n);
            // (added for DodgeF game/dodge/CcDaisyFlowerGarden.cpp) The same
            // for the extra collisions (Daisy's fence uses the one of its
            // court).
            static void EnableExColl(long n, int bEnable);
            static void SetExCollPos(long n, const bcs::FVEC3 &rPos);
            static void SetExCollRadius(long n, float fRadius);
            // (DOL) The ground attribute under rPos (the stage's
            // GetFloorAttribute) into rAttr and the floor's flow there (its
            // GetFloorFlow) into rFlow. 8 is a hole (msw::CDropCoin).
            static void GetGrdAttr(const bcs::FVEC3 &rPos, unsigned long &rAttr, bcs::FVEC3 &rFlow);
            // (DOL) Bit 1 of a ground attribute: the floor moves things.
            static int IsGrdAttrFlow(unsigned long attr);

            // DOL geometry helpers (static: they never read `this`).
            // Added for HockeyF game/hockey/CCollManHockey.cpp; parameter
            // names and the meaning of the returns are read from its use.
            // The closest points of segments rA0-rA1 and rB0-rB1 (rClosestA,
            // rClosestB, at parameters rT and rS); returns the square of
            // their distance.
            static float GetClosestP_SegmentSegment(const bcs::FVEC3 &rA0, const bcs::FVEC3 &rA1,
                                                    const bcs::FVEC3 &rB0, const bcs::FVEC3 &rB1, float &rT,
                                                    float &rS, bcs::FVEC3 &rClosestA, bcs::FVEC3 &rClosestB);
            // (added for BossF game/boss/CComDrvBoss.cpp; meaning read from its
            // use) Where segment rFrom-rTo first touches the capsule of radius
            // fRadius around rCap0-rCap1, in rHit; not zero when it does.
            static int DetectHitPos_SegmentCapsule(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo,
                                                   const bcs::FVEC3 &rCap0, const bcs::FVEC3 &rCap1, float fRadius,
                                                   bcs::FVEC3 &rHit);
            // The point of segment r0-r1 closest to rPoint; returns the
            // square of the distance.
            static float GetClosestP_PointSegment(const bcs::FVEC3 &rPoint, const bcs::FVEC3 &r0,
                                                  const bcs::FVEC3 &r1, float &rT, bcs::FVEC3 &rClosest);
            // Where segment rFrom-rTo crosses the plane through rPoint with
            // normal rNormal from its front side (rHit), as a fraction of the
            // segment.
            static float GetIntersectionTimeOfFrontPlaneAndLine(bcs::FVEC3 &rHit, const bcs::FVEC3 &rNormal,
                                                                const bcs::FVEC3 &rPoint, const bcs::FVEC3 &rFrom,
                                                                const bcs::FVEC3 &rTo);
            // Where segment rFrom-rTo first touches the sphere around
            // rCenter (rHit), as a fraction of the segment.
            static float GetIntersectionTimeOfSegmentSphere(bcs::FVEC3 &rHit, const bcs::FVEC3 &rFrom,
                                                            const bcs::FVEC3 &rTo, const bcs::FVEC3 &rCenter,
                                                            float fRadius);

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
