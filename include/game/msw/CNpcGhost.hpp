#ifndef MSW_CNPCGHOST_HPP_
#define MSW_CNPCGHOST_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CNpcBase.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {
    class CMultViewAlphaMdlObj;
}

namespace msw {

    class CBall;
    class CChrMan;
    class CCollAtkUnit;

    // The ghost (Boo) NPC. A DOL class; BasketF derives
    // CNpcGhostBasketBase from it (and VolleyF its own ghost). Its state
    // queue bcs::CFsmQueue<CNpcGhost> is at 0x188 (vptr at 0x1E4; the
    // destructor thunk of the derived vtables is "@392@").
    //
    // INCOMPLETE: the members up to 0x204 and the virtual functions are
    // from BasketF's __vt__Q23msw19CNpcGhostBasketBase and what
    // game/basket/CNpcGhostBasketBase.cpp reads; return types and member
    // names are guesses. The state functions marked inline are the ones
    // BasketF emits as weak copies (empty in the DOL); the others are DOL
    // functions.
    class CNpcGhost : public CNpcBase, public bcs::CFsmQueue<CNpcGhost> {
        public:
            // The states, in the order of the state functions in the vtable.
            enum STATE {
                STATE_SLEEP,
                STATE_START_WAIT,
                STATE_IDLE,
                STATE_MOVE,
                STATE_STEELE,
                STATE_GET_LOOSE_BALL,
                STATE_MOT_PLAY,
                STATE_PURSUIT,
                STATE_DISAPPEARS,
                STATE_APPEARANCE,
                STATE_PASS,
                STATE_PASS_CATCH,
                STATE_CATCH,
                STATE_HIDDEN,
                STATE_BLOCK,
                STATE_SPIKE
            };

            // (parameter names guessed) The type is what GetNpcType
            // returns (CChrMan::NpcSearch finds the ghost by it).
            CNpcGhost(CChrMan *pMan, long nType);
            virtual ~CNpcGhost();

            virtual void DispFunc() {}
            virtual long GetNpcType() const { return m_nNpcType; }
            virtual void Create();
            virtual void Reset();
            virtual long SendMessage(long, long, long, long, long);
            virtual void AttachBall(CBall *pBall, long nNode, float, unsigned long);
            virtual void DetachBall();
            virtual int IsHoldBall() const;

            // Own virtual functions, 0x80 on.
            virtual void CreateMdlObj_();
            virtual void InitFsm_();
            virtual long GetQueueWorkSize() const;
            virtual long GetQueueStackSize() const { return 4; }
            virtual bcs::CMultViewAlphaMdlObj *NewMdlObj_(nw4r::g3d::ResMdl mdl, unsigned long flag);
            virtual unsigned long CallbackBallDamage_(CBall *pBall, const CCollAtkUnit *pAtk, const bcs::FVEC3 &rPos);
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateStartWait() {}
            virtual void LoopStateStartWait() {}
            virtual void DtorStateStartWait() {}
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateMove();
            virtual void LoopStateMove();
            virtual void DtorStateMove();
            virtual void CtorStateSteele() {}
            virtual void LoopStateSteele() {}
            virtual void DtorStateSteele() {}
            virtual void CtorStateGetLooseBall();
            virtual void LoopStateGetLooseBall();
            virtual void DtorStateGetLooseBall();
            virtual void CtorStateMotPlay();
            virtual void LoopStateMotPlay();
            virtual void DtorStateMotPlay() {}
            virtual void CtorStatePursuit();
            virtual void LoopStatePursuit();
            virtual void DtorStatePursuit();
            virtual void CtorStateDisappears();
            virtual void LoopStateDisappears();
            virtual void DtorStateDisappears();
            virtual void CtorStateAppearance();
            virtual void LoopStateAppearance();
            virtual void DtorStateAppearance();
            virtual void CtorStatePass();
            virtual void LoopStatePass();
            virtual void DtorStatePass();
            virtual void CtorStatePassCatch();
            virtual void LoopStatePassCatch();
            virtual void DtorStatePassCatch();
            virtual void CtorStateCatch();
            virtual void LoopStateCatch();
            virtual void DtorStateCatch();
            virtual void CtorStateHidden() {}
            virtual void LoopStateHidden() {}
            virtual void DtorStateHidden() {}
            virtual void CtorStateBlock() {}
            virtual void LoopStateBlock() {}
            virtual void DtorStateBlock() {}
            virtual void CtorStateSpike() {}
            virtual void LoopStateSpike() {}
            virtual void DtorStateSpike() {}

            // (DOL) The goal the ghost's team attacks.
            long GetTargetGoalIdx() const;
            // (DOL) The position of node nNode of the model.
            void GetVNodePos(long nNode, bcs::FVEC3 &rPos) const;

        protected:
            // (DOL) Place the model at the ghost and decide whether it is
            // drawn.
            void CalcRoot_();
            void CheckDisp_();

            long m_nNpcType;                // 0x1E8, see GetNpcType
            long m_nTeamNo;                 // 0x1EC, the team the ghost plays for
            unsigned long m_Unk1F0;
            unsigned long m_nGhostFlag;     // 0x1F4, bit 0 set while asleep
            float m_fAlpha;                 // 0x1F8, how visible the ghost is
            unsigned short m_nBallNode;     // 0x1FC, the node the ball is held at
            unsigned short m_Unk1FE;
            unsigned short m_Unk200;
    };
}

#endif /* !MSW_CNPCGHOST_HPP_ */
