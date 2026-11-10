#ifndef CCOCKPITFRAMEOUTBASE_HPP_
#define CCOCKPITFRAMEOUTBASE_HPP_

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {

    // Size 0xC, vptr at 0x08. Lives in the DOL; a third base of several
    // cockpit telops (at 0x60 in CCockpitTelopSwitchColor and
    // CCockpitModelTelopHold). Its constructor is inline: the derived
    // constructors store -1 and a float at 0x00 and 0x04 (DodgeF
    // CCockpitDodgeTelopInOutChange). The destructor is inline (DodgeF emits
    // it). Member names are guesses.
    class CCockpitFrameOutBase {
        public:
            // (inline; added for BasketF game/basket/CCockpitBasketBonusTime.cpp,
            // whose constructor stores -1 and 1.0f here before its own
            // vptrs) The float differs per derived class.
            CCockpitFrameOutBase(float fUnk04)
                : m_nWaitFrameOut(-1)
                , m_fUnk04(fUnk04)
            {
            }

            // 0 when pPaneCtrl is null or animation nAnim has ended,
            // otherwise whether its frame count passes a limit.
            int IsWaitAnim(bcs::G2D::CPaneCtrl *pPaneCtrl, long nAnim);

            // (names guessed; no symbols) What the telops read and write
            // inline.
            int IsWaitFrameOut() const { return m_nWaitFrameOut != 0; }
            void SetWaitFrameOut(int bWait) { m_nWaitFrameOut = bWait != 0; }

            long m_nWaitFrameOut; // 0x00, -1 until decided, then 0 or 1
            float m_fUnk04;       // 0x04

            // vptr at 0x08
            virtual ~CCockpitFrameOutBase() {}
    };
}

#endif /* !CCOCKPITFRAMEOUTBASE_HPP_ */
