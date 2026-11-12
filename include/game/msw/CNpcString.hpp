#ifndef MSW_CNPCSTRING_HPP_
#define MSW_CNPCSTRING_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CNpcBase.hpp"

namespace msw {

    class CBall;
    class CChrMan;

    // A string hanging from above the court with a ball on its end (NPC type
    // 11). A DOL class; BasketF's party game derives CNpcStringBasket from
    // it to hang its fruit. Size 0x1DC: CNpcStringBasket's constructor
    // (inline in CBallManBasketHungryBossPackun::Create_) writes its own
    // members from 0x1DC on. Its state machine bcs::CFsm<CNpcString> is at
    // 0x188 (vptr at 0x1CC; the destructor thunk of the derived vtable is
    // "@392@").
    //
    // INCOMPLETE: the virtual functions are from BasketF's
    // __vt__Q23msw16CNpcStringBasket; the members 0x1D0-0x1D8 are not
    // mapped. The ones with a body are the ones BasketF emits as weak
    // copies; the others are DOL functions. Return types guessed.
    class CNpcString : public CNpcBase, public bcs::CFsm<CNpcString> {
        public:
            // The states, in the order of the state functions in the vtable.
            enum STATE {
                STATE_SLEEP,
                STATE_APPEAR,
                STATE_IDLE,
                STATE_MOVE,
                STATE_WITHDRAWS,
                STATE_DISAPPEAR
            };

            CNpcString(CChrMan *pMan);
            virtual ~CNpcString();

            virtual void Create();
            virtual long GetNpcType() const { return 11; }
            virtual long SendMessage(long nMsg, long, long, long, long);
            virtual void AttachBall(CBall *pBall);
            virtual void AttachBall(CBall *pBall, long nNode, float, unsigned long);
            virtual void MainFunc();

            // Own virtual functions, 0x88 on.
            virtual void AttachBall(CBall *pBall, const bcs::FVEC3 &rPos);
            virtual void DoAppear();
            virtual void DoDisappear();
            // The shadow under the ball: its colour, its alpha and its size.
            virtual void SetShadowColor(bcs::BCS_Color color);
            virtual void SetShadowRate(unsigned char nRate);
            virtual void SetShadowScale(float fScale);
            virtual void CtorStateSleep();
            virtual void LoopStateSleep() {}
            virtual void DtorStateSleep();
            virtual void CtorStateAppear();
            virtual void LoopStateAppear();
            virtual void DtorStateAppear();
            virtual void CtorStateIdle();
            virtual void LoopStateIdle();
            virtual void DtorStateIdle();
            virtual void CtorStateMove();
            virtual void LoopStateMove();
            virtual void DtorStateMove();
            virtual void CtorStateWithdraws();
            virtual void LoopStateWithdraws();
            virtual void DtorStateWithdraws();
            virtual void CtorStateDisappear();
            virtual void LoopStateDisappear();
            virtual void DtorStateDisappear();
            // Not zero when a ball can be hung on the string.
            virtual int CanbeAttachBall() const;

            // (name guessed) The state now running (CFsm's state number).
            long GetState() const { return m_nState; }

        protected:
            unsigned char m_Unk1D0[0x1DC - 0x1D0];
    };
}

#endif /* !MSW_CNPCSTRING_HPP_ */
