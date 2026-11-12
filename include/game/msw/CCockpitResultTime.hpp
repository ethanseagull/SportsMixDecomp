#ifndef MSW_CCOCKPITRESULTTIME_HPP_
#define MSW_CCOCKPITRESULTTIME_HPP_

namespace msw {

    // The two floats the class starts with. A base class is a guess: the
    // vptr comes after them (at 0x08), which a base with data and no virtual
    // function explains.
    struct CResultTimeData {
        float m_fUnk00; // 0x00, 75.0f for kind 1, else 30.0f (meaning not known)
        float m_fUnk04; // 0x04, 15.0f (meaning not known)
    };

    // (purpose guessed from the name) The timing of a result cockpit, a part
    // of it kept at 0x60 (CCockpitLayoutBase's constructor builds it with
    // 0 or 1). Lives in the DOL. Size 0x0C: two floats (CResultTimeData), then the vptr (the
    // vtable holds only the destructor).
    class CCockpitResultTime : public CResultTimeData {
        public:
            // nKind is 1 or 0 (a comparison result in the caller).
            CCockpitResultTime(long nKind);
            virtual ~CCockpitResultTime();

            // (name guessed; added for DOL game/msw/CCockpitResult.cpp) When
            // item n of a list appears: m_fUnk00 + n * m_fUnk04 frames, as
            // BasketF's CCockpitBasketResult computes it for its scores.
            float GetDispTime(long n) const
            {
                float fDelay = m_fUnk04 * n;
                return m_fUnk00 + fDelay;
            }
    };
}

#endif /* !MSW_CCOCKPITRESULTTIME_HPP_ */
