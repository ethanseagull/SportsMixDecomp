#ifndef MSW_CSTGGMK_PINBALL_DODGE_HPP_
#define MSW_CSTGGMK_PINBALL_DODGE_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/CStgGmk_Pinball_Act.hpp"

namespace msw {

    // The balls of stage 12 (pinball) in the dodgeball mode.

    // The actor, size 0x220 (nothing added to msw::CStgGmk_Pinball_Act),
    // with the DODGE_PRM_S12_BALL_* parameters. Everything is in
    // game/dodge/CStgGmk_Pinball_Act_Dodge.cpp; the constructor is used by
    // msw::CStgGmk_Pinball_Dodge::NewAct.
    class CStgGmk_Pinball_Act_Dodge : public CStgGmk_Pinball_Act {
        public:
            CStgGmk_Pinball_Act_Dodge(CStgGmk *pParent) : CStgGmk_Pinball_Act(pParent) {}
            virtual ~CStgGmk_Pinball_Act_Dodge();

            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual float GetShotAngle() const;
            virtual float GetParam_Radius() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_CollRadius() const;
            virtual float GetParam_CollHeight() const;
            virtual float GetParam_AtkCollRadius() const;
            virtual float GetParam_Interval() const;
            virtual void GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const;
            virtual float GetParam_Speed() const;
            virtual float GetParam_VelLimit() const;
            virtual float GetParam_Dec() const;
            virtual float GetParam_ForceAcc() const;
            virtual long GetParam_EndLineEnabled() const;
            virtual float GetParam_GuideWidth() const;
            virtual float GetParam_GuideDepth() const;
            virtual float GetParam_GuideDecay() const;
            virtual float GetParam_KickPower() const;
            virtual float GetParam_BallKickPower() const;
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_AtkHitEffectFrame() const;
    };

    // One ball, size 0x78 (nothing added to msw::CStgGmk_Pinball). Made by
    // msw::CPinballManDodge::NewGmk. All inline: the unit above emits the
    // vtable and NewDrv, NewAct and the destructor (reverse order of the
    // declarations here).
    class CStgGmk_Pinball_Dodge : public CStgGmk_Pinball {
        public:
            CStgGmk_Pinball_Dodge(bcs::CTask *pParent, int arg) : CStgGmk_Pinball(pParent, arg) {}
            virtual ~CStgGmk_Pinball_Dodge() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Pinball_Act_Dodge(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Pinball_Drv(this); }
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_DODGE_HPP_ */
