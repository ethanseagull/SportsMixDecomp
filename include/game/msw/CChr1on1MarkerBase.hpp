#ifndef MSW_CCHR1ON1MARKERBASE_HPP_
#define MSW_CCHR1ON1MARKERBASE_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CUnivTask.hpp"

namespace bcs {
    class CMdlObj;
    struct BCS_Color;
}

namespace msw {

    // A marker over one character (the one-on-one games). A DOL class; the
    // modules derive their own (HockeyF CChr1on1MarkerHockey, BasketF
    // CChrMoveBlockMarkerBasket). The state machine is a second base at
    // 0x34 (its vptr at 0x78, as in CChrMarker). The virtual functions
    // follow __vt__Q23msw20CChr1on1MarkerHockey; return types are guesses
    // unless a body shows them. Size 0x9C (the hockey marker adds nothing).
    //
    // The first base is bcs::CUnivTaskEx: the DOL constructor stores its
    // vtable and the destructor tests `this` twice before calling
    // bcs::CTask's (one test per inline level).
    class CChr1on1MarkerBase : public bcs::CUnivTaskEx, public bcs::CFsm<CChr1on1MarkerBase> {
        public:
            // The constructor builds the four states (hidden, appearing,
            // shown, disappearing) and enters the first. The derived class
            // calls Create.
            CChr1on1MarkerBase(bcs::CTask *pParent, long nChr);
            virtual ~CChr1on1MarkerBase();
            virtual void Main();
            virtual void Disp();

            // Loads the marker model.
            virtual void Create();
            // (meanings guessed from the states) 1 hides the marker at once,
            // 2 brings it up, 3 lets it go away; the base returns 3.
            virtual int CheckAppear();
            // Frames the marker takes to come up and to go away, and its
            // size (the base's values are the ones BasketF repeats).
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            // The two colours of the model's materials for stage nIdx.
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx);
            virtual void UpdateColor_();

        private:
            // (name guessed; not in the DOL) Builds the state table and
            // enters the first state; the constructor inlines it.
            void InitState();

            // The four states, in state order (names guessed after
            // CChrMarker's; the DOL has no names for them).
            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateMain();
            void LoopStateMain();
            void DtorStateMain();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

        protected:
            // 0x7C to 0x88 named from BasketF CChrMoveBlockMarkerBasket::
            // CheckAppear (it places the marker; names guessed). Main copies
            // them to the model.
            float m_fPosX;          // 0x7C
            float m_fPosY;          // 0x80
            float m_fPosZ;          // 0x84
            float m_fRotY;          // 0x88
            bcs::CMdlObj *m_pMdl;   // 0x8C, made by Create
            long m_nChr;            // 0x90, passed to CChrMan::GetChrCore
            // (names guessed) How far the marker has come up, 0 to 1, and
            // the phase of its pulsing while shown (radians).
            float m_fDispRate;      // 0x94
            float m_fWave;          // 0x98
    };
}

#endif /* !MSW_CCHR1ON1MARKERBASE_HPP_ */
