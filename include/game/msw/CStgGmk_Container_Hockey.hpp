#ifndef MSW_CSTGGMK_CONTAINER_HOCKEY_HPP_
#define MSW_CSTGGMK_CONTAINER_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Case.hpp"
#include "game/msw/CStgGmk_Case_Act.hpp"
#include "bcs/BCS_Rand.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    // (purpose guessed from the names) The containers of the conveyor belt
    // stage (stage 09): crates the belts carry along, holding an item, a bomb
    // or treasure (the type given to the constructor). A case of the ship
    // stage with its own actor.

    // The actor of a container. A DOL class, size 0x214 (nothing added to
    // msw::CStgGmk_Case_Act); its vtable __vt__Q23msw21CStgGmk_Container_Act
    // is in the DOL (key function GetParam_HitCameraQuake). Its constructor
    // and destructor are inline (msw::CStgGmk_Container::NewAct; the hockey
    // destructor calls CStgGmk_Case_Act's after one more test of `this`).
    class CStgGmk_Container_Act : public CStgGmk_Case_Act {
        public:
            CStgGmk_Container_Act(CStgGmk *pParent) : CStgGmk_Case_Act(pParent) {}
            virtual ~CStgGmk_Container_Act() {}

            virtual long GetParam_HitCameraQuake() const;
            virtual long GetParam_ItemHitMethod() const { return 2; }
    };

    // The hockey model of a container, size 0x134 (nothing added to
    // msw::CStgGmk_Case_Mdl). Declared before the actor: the vtables come out
    // in .data in reverse order of declaration (actor first). Create and Main are in
    // game/hockey/Gmk_Container_Hockey.cpp; the constructor is inline
    // (msw::CStgGmk_Container_Hockey::NewMdl).
    class CStgGmk_Container_Mdl_Hockey : public CStgGmk_Case_Mdl {
        public:
            CStgGmk_Container_Mdl_Hockey(CStgGmk *pParent, long type) : CStgGmk_Case_Mdl(pParent, type) {}
            virtual void Create();
            virtual void Main(float rate);
    };

    // The hockey version of the actor, size 0x218, with the
    // HOCKEY_PRM_S09_CONTAINER_* (plain), _BOMB_* and _TREASURE_*
    // parameters: the gimmick's sub type is 0 for a plain container, 1 for
    // a bomb, 2 for a treasure box. Its functions are in
    // game/hockey/Gmk_Container_Hockey.cpp; the constructor is
    // inline (msw::CStgGmk_Container_Hockey::NewAct). The inline functions
    // not called in that file are emitted at its end in reverse order of
    // the declarations here.
    class CStgGmk_Container_Act_Hockey : public CStgGmk_Container_Act {
        public:
            CStgGmk_Container_Act_Hockey(CStgGmk *pParent) : CStgGmk_Container_Act(pParent) { m_fReflectWait = 0.0f; }
            virtual ~CStgGmk_Container_Act_Hockey();

            virtual float GetParam_Dec() const { return HOCKEY_PRM_S09_CONTAINER_DEC + bcs::BCS_RandMax(HOCKEY_PRM_S09_CONTAINER_DEC_RND); }
            virtual float GetParam_VelLimit() const { return HOCKEY_PRM_S09_CONTAINER_VEL; }
            virtual long GetParam_MdlIdx() const { return HOCKEY_PRM_S09_CONTAINER_BOX_TYPE; }
            virtual float GetParam_MaxLife() const { return HOCKEY_PRM_S09_CONTAINER_LIFE; }
            // The container reflects the puck (OnReflectHit).
            virtual bool IsEnableDefColl() const { return true; }
            // The gimmick's sub type (0 plain, 1 bomb, 2 treasure).
            virtual long GetSubType_() const
            {
                const CStgGmk_Case *pParent = static_cast<const CStgGmk_Case *>(GetParent());
                return pParent != 0 ? pParent->GetSubType() : 0;
            }
            // Seconds to frames.
            virtual float GetParam_Interval() const { return 60.0f * HOCKEY_PRM_S09_CONTAINER_INTERVAL; }

            virtual float GetParam_Width() const;
            virtual float GetParam_Depth() const;
            virtual float GetParam_Height() const;
            virtual float GetParam_MdlWidth() const;
            virtual float GetParam_MdlDepth() const;
            virtual float GetParam_MdlHeight() const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual float GetParam_BreakTime() const;
            virtual void OnUpdate(float rate);
            virtual void UpdateColl();
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual void OnBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush);
            virtual void CtorState_INACTIVE();
            virtual void LoopState_INACTIVE();
            virtual void DtorState_INACTIVE();
            virtual void CtorState_MOVE();
            virtual void LoopState_MOVE();
            virtual void DtorState_MOVE();
            virtual void CtorState_OPEN();
            virtual void LoopState_OPEN();
            virtual void DtorState_OPEN();

        private:
            float m_fReflectWait; // 0x214, frames until the container reflects the puck again (name guessed)
    };

    // One container, size 0x78 (nothing added to msw::CStgGmk_Case). All
    // inline: HockeyF's game/hockey/CStageHockey09.cpp emits the vtable and the
    // functions. Its gimmick type is 6 (what CStgGmkMan's lookups ask for).
    class CStgGmk_Container : public CStgGmk_Case {
        public:
            CStgGmk_Container(bcs::CTask *pParent, int type) : CStgGmk_Case(pParent, type) { SetType(6); }
            virtual ~CStgGmk_Container() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                return new (heap) CStgGmk_Case_Mdl(this, GetCaseType());
            }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Container_Act(this); }
    };

    // The hockey container: the hockey actor and model. Made by
    // msw::CStageHockey09::ConveyorUpdate. NewAct, NewMdl and the destructor
    // are emitted in reverse order of the declarations here.
    class CStgGmk_Container_Hockey : public CStgGmk_Container {
        public:
            CStgGmk_Container_Hockey(bcs::CTask *pParent, int type) : CStgGmk_Container(pParent, type) {}
            virtual ~CStgGmk_Container_Hockey() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                return new (heap) CStgGmk_Container_Mdl_Hockey(this, GetCaseType());
            }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Container_Act_Hockey(this); }
    };
}

#endif /* !MSW_CSTGGMK_CONTAINER_HOCKEY_HPP_ */
