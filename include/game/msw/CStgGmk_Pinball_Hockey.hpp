#ifndef MSW_CSTGGMK_PINBALL_HOCKEY_HPP_
#define MSW_CSTGGMK_PINBALL_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Pinball.hpp"
#include "game/msw/CStgGmk_Pinball_Act.hpp"

namespace msw {

    // The balls of stage 12 (pinball) in the hockey mode.

    // The actor, size 0x224 (NewAct allocates it; one float added to
    // msw::CStgGmk_Pinball_Act), with the HOCKEY_PRM_S12_BALL_* parameters.
    // Unlike the other modules' balls it takes hits from the puck (its
    // defence volume is on) and bounces off the goals. Everything is in
    // game/hockey/CStgGmk_Pinball_Act_Hockey.cpp; the constructor is used by
    // msw::CStgGmk_Pinball_Hockey::NewAct.
    class CStgGmk_Pinball_Act_Hockey : public CStgGmk_Pinball_Act {
        public:
            CStgGmk_Pinball_Act_Hockey(CStgGmk *pParent) : CStgGmk_Pinball_Act(pParent), m_fReflectWait(0.0f) {}
            virtual ~CStgGmk_Pinball_Act_Hockey();

            virtual void OnUpdate(float rate);
            virtual void UpdateColl();
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual int WallHitCheck(bcs::FVEC3 *pPos, bcs::FVEC3 *pVel, const bcs::FVEC3 &rPrev,
                                     const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel);
            virtual bool IsEnableDefColl() const { return true; }
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

        private:
            float m_fReflectWait; // 0x220, frames until the puck can bounce the ball again (name guessed)
    };

    // One ball, size 0x78 (nothing added to msw::CStgGmk_Pinball). Made by
    // msw::CPinballManHockey::NewGmk. All inline: the unit above emits the
    // vtable and NewDrv, NewAct and the destructor (reverse order of the
    // declarations here).
    class CStgGmk_Pinball_Hockey : public CStgGmk_Pinball {
        public:
            CStgGmk_Pinball_Hockey(bcs::CTask *pParent, int arg) : CStgGmk_Pinball(pParent, arg) {}
            virtual ~CStgGmk_Pinball_Hockey() {}

            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Pinball_Act_Hockey(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Pinball_Drv(this); }
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_HOCKEY_HPP_ */
