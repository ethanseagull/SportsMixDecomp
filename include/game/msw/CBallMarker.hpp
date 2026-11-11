#ifndef MSW_CBALLMARKER_HPP_
#define MSW_CBALLMARKER_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace bcs {
    struct BCS_Color;
    class CScnObj;
}

namespace msw {

    // The marker of a ball. A DOL class, size 0x90 (CBallManDodge allocates
    // a CBallMarkerDodge of that size, which adds no members): bcs::CTask at
    // 0x00, bcs::CFsm<CBallMarker> at 0x34 (vptr at 0x78), own members from
    // 0x7C. Each game module derives its own marker from it.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw16CBallMarkerDodge (DodgeF). MainFunc is inline (DodgeF
    // emits it, a single blr). The float return types are read from the DOL
    // bodies, the others are guesses; so are the names of the parameters and
    // members.
    class CBallMarker : public bcs::CTask, public bcs::CFsm<CBallMarker> {
        public:
            // `nBallHandle` is the task id of the ball the marker follows.
            // (what the last argument selects is not known; CBallManDodge
            // passes 0)
            CBallMarker(bcs::CTask *pParent, unsigned long nBallHandle, long);
            virtual ~CBallMarker();
            virtual void Main();
            virtual void Disp();

            // Per-game work, empty here.
            virtual void MainFunc() {}
            // Asked in the Sleep state: 2 when the marker should come up
            // (LoopStateSleep then changes to state 1). The DOL body (unnamed,
            // fn_800CE074) returns 1 without a ball or for a sleeping one,
            // otherwise 2 or 3.
            virtual int AppearCheck();
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            virtual void GetColor_(bcs::BCS_Color &, bcs::BCS_Color &, long);
            virtual void UpdateColor_();

            // One Ctor/Loop/Dtor triple per state, in state order.
            virtual void CtorStateSleep();
            virtual void LoopStateSleep();
            virtual void DtorStateSleep();
            virtual void CtorStateAppear();
            virtual void LoopStateAppear();
            virtual void DtorStateAppear();
            virtual void CtorStateMain();
            virtual void LoopStateMain();
            virtual void DtorStateMain();
            virtual void CtorStateDisappear();
            virtual void LoopStateDisappear();
            virtual void DtorStateDisappear();

            // DOL. (what the index selects is not known; the basket marker
            // passes 0 on stage 6)
            void SetFogIdx(long nIdx);

        protected:
            unsigned char m_Unk7C[0x4];  // 0x7C
            // 0x80 (name guessed): the marker's model; the basket and hockey
            // markers attach it to the ball or to what holds the ball.
            bcs::CScnObj *m_pMdlObj;
            unsigned long m_nBallHandle; // 0x84, the constructor's nBallHandle
            unsigned char m_Unk88[0x8];  // 0x88
    };
}

#endif /* !MSW_CBALLMARKER_HPP_ */
