#ifndef MSW_CSTGGMK_PINBALL_ACT_HPP_
#define MSW_CSTGGMK_PINBALL_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CDefCollision.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    class CBall;
    class CCollAtkUnit;
    class CCollBodyUnit;
    class CStgGmk;

    // (purpose guessed from the names) The actor of a ball of stage 12
    // (msw::CStgGmk_Pinball). A DOL class, size 0x220: CStgGmkAct at 0x000
    // (vptrs at 0x008 and 0x014), its state queue bcs::CFsmQueue at 0x174
    // (vptr at 0x1D0), own members from 0x1D4. Constructor
    // __ct__Q23msw19CStgGmk_Pinball_ActFPQ23msw7CStgGmk. Each game module
    // derives its own class from it to supply the parameters.
    //
    // The ball (DOL game/msw/CStgGmk_Pinball.cpp): the launcher shoots it
    // into the field, where it rolls at GetParam_Speed and grows from
    // GetParam_InitRadius to GetParam_Radius. It bounces off the walls
    // round the field (WallHitCheck); an attack of a character kicks it
    // away. Over a goal line it scores for the team behind it and sleeps
    // until it is shot again.
    //
    // The order of the new virtual functions (IsAlive to DtorState_INACTIVE)
    // follows __vt__Q23msw26CStgGmk_Pinball_Act_Volley (VolleyF) and
    // __vt__Q23msw25CStgGmk_Pinball_Act_Dodge (DodgeF), which
    // emits weak copies of IsAlive, IsInvincible, SetInvincible,
    // SE_GetSlotNum, IsEnableDefColl and GetParam_LocOffsetZ: those are
    // inline here. The DOL emits weak copies of GetParam_AtkCollRadius,
    // GetParam_AtkHitEffectFrame and GetShotAngle as well. The DOL's own
    // vtable (lbl_804851D8, 0x2AC bytes) ends at DtorState_INACTIVE:
    // GetParam_BallKickPower is declared by the VolleyF and DodgeF classes
    // (both put it last). Return types not visible in a
    // body are guesses.
    class CStgGmk_Pinball_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Pinball_Act> {
        public:
            enum STATE {
                STATE_DEFAULT,
                STATE_STANDBY,
                STATE_MOVE,
                STATE_BREAK,
                STATE_SLEEP,
                STATE_RESPAWN,
                STATE_INACTIVE,
                STATE_NUM
            };

            CStgGmk_Pinball_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Pinball_Act();

            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);
            virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody, const bcs::FVEC3 &rPos,
                                     unsigned long flag);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);

            virtual int IsAlive() const { return m_bAlive; }              // 0x1A4
            virtual void Inactivate();                                    // 0x1A8
            virtual void Sleep(float frame);                              // 0x1AC
            virtual void Respawn(float angle);                            // 0x1B0
            virtual void Standby();                                       // 0x1B4
            virtual long SE_GetSlotNum() const { return 1; }             // 0x1B8
            virtual void SE_PlayByParam(long type);                       // 0x1BC
            virtual void SE_PlayByParam(long type, const bcs::FVEC3 &rPos); // 0x1C0
            virtual float GetCurRadius() const;                           // 0x1C4
            virtual float GetParam_InitRadius() const;                    // 0x1C8
            virtual float GetParam_Radius() const;                        // 0x1CC
            virtual float GetParam_Height() const;                        // 0x1D0
            virtual float GetParam_CollRadius() const;                    // 0x1D4
            virtual float GetParam_CollHeight() const;                    // 0x1D8
            virtual float GetParam_AtkCollRadius() const { return GetParam_CollRadius(); } // 0x1DC
            virtual float GetParam_Dec() const;                           // 0x1E0
            virtual float GetParam_KickPower() const;                     // 0x1E4
            virtual float GetParam_Speed() const;                         // 0x1E8
            virtual float GetParam_VelLimit() const;                      // 0x1EC
            virtual float GetParam_Gravity() const;                       // 0x1F0
            virtual float GetParam_Interval() const;                      // 0x1F4
            virtual void GetParam_CourtRect(float *pLeft, float *pRight, float *pFront, float *pBack) const; // 0x1F8
            virtual void GetParam_OuterRect(float *pLeft, float *pRight, float *pFront, float *pBack) const; // 0x1FC
            virtual long GetParam_MdlIdx() const;                         // 0x200
            virtual long GetParam_Effect(long type) const;                // 0x204
            virtual long GetParam_SE(long type) const;                    // 0x208
            virtual float GetParam_MaxLife() const;                       // 0x20C
            virtual float GetParam_LocOffsetZ() const { return 0.0f; }   // 0x210
            virtual float GetParam_ForceAcc() const;                      // 0x214
            virtual long GetParam_EndLineEnabled() const;                 // 0x218
            virtual float GetParam_GuideWidth() const;                    // 0x21C
            virtual float GetParam_GuideDepth() const;                    // 0x220
            virtual long GetParam_GuideBoundEnabled() const;              // 0x224
            virtual float GetParam_GuideDecay() const;                    // 0x228
            virtual float GetParam_AtkHitEffectFrame() const { return 0.0f; } // 0x22C
            virtual long GetParam_HitCameraQuake() const;                 // 0x230
            virtual void UpdateColl();                                    // 0x234
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);    // 0x238
            virtual bool IsInvincible() const { return m_fInvincibleTime > 0.0f; } // 0x23C
            virtual void SetInvincible(float time) { m_fInvincibleTime = time; } // 0x240
            virtual bool IsEnableDefColl() const { return false; }       // 0x244
            virtual float GetShotAngle() const { return m_fShotAngle; }   // 0x248
            // (argument names guessed) Moves the ball from rPrev to rPos at
            // rVel against the walls round the field; the position and the
            // velocity after any bounce go to pPos and pVel. True when the
            // ball is out of the field (and scores).
            virtual int WallHitCheck(bcs::FVEC3 *pPos, bcs::FVEC3 *pVel, const bcs::FVEC3 &rPrev,
                                     const bcs::FVEC3 &rPos, const bcs::FVEC3 &rVel); // 0x24C
            // (argument names guessed) The same against the one wall from
            // rWall0 to rWall1 (in the ground plane), for a ball of the given
            // radius; a bounce keeps (1 - decay) of the speed. True on a hit.
            virtual int WallHitCheckSub(bcs::FVEC3 *pPos, bcs::FVEC3 *pVel, const bcs::FVEC3 &rPrev,
                                        const bcs::FVEC3 &rPos, float radius, const bcs::FVEC3 &rVel,
                                        const bcs::FVEC3 &rWall0, const bcs::FVEC3 &rWall1, float decay) const; // 0x250
            virtual void KickHitEffect(const bcs::FVEC3 &rPos);           // 0x254
            virtual void CtorState_DEFAULT();                             // 0x258
            virtual void LoopState_DEFAULT();
            virtual void DtorState_DEFAULT();
            virtual void CtorState_STANDBY();
            virtual void LoopState_STANDBY();
            virtual void DtorState_STANDBY();
            virtual void CtorState_MOVE();
            virtual void LoopState_MOVE();
            virtual void DtorState_MOVE();
            virtual void CtorState_BREAK();
            virtual void LoopState_BREAK();
            virtual void DtorState_BREAK();
            virtual void CtorState_SLEEP();
            virtual void LoopState_SLEEP();
            virtual void DtorState_SLEEP();
            virtual void CtorState_RESPAWN();
            virtual void LoopState_RESPAWN();
            virtual void DtorState_RESPAWN();
            virtual void CtorState_INACTIVE();
            virtual void LoopState_INACTIVE();
            virtual void DtorState_INACTIVE();                            // 0x2A8

        protected:
            // (protected: HockeyF's CStgGmk_Pinball_Act_Hockey::UpdateColl
            // changes m_BodyColl)
            CBodyCollision m_BodyColl;     // 0x1D4: 0 the ball, 1 its push volume
            CAtkCollision m_AtkColl;       // 0x1E0
            CDefCollision m_DefColl;       // 0x1EC
            float m_fStateTime;            // 0x1F8, frames left in BREAK, frames slept in SLEEP
            int m_bAlive;                  // 0x1FC (IsAlive)
            int m_bInField;                // 0x200, set once the ball is inside the field (names guessed)
            unsigned long m_hEffect;       // 0x204, an effect stopped when MOVE ends
            float m_fLife;                 // 0x208
            float m_fMaxLife;              // 0x20C, GetParam_MaxLife at the spawn
            float m_fInvincibleTime;       // 0x210 (IsInvincible while above zero; name guessed)
            float m_fGrowTime;             // 0x214, frames grown (GetCurRadius)
            float m_fGrowTimeMax;          // 0x218, frames to grow to full size
            float m_fShotAngle;            // 0x21C (name guessed; read by VolleyF's GetShotAngle)
    };
}

#endif /* !MSW_CSTGGMK_PINBALL_ACT_HPP_ */
