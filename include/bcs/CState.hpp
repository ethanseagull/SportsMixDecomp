#ifndef BCS_CSTATE_HPP_
#define BCS_CSTATE_HPP_

namespace bcs {

    // One state of a state machine (bcs::CFsm): three hooks run on entering
    // the state, on every frame in it and on leaving it. Size 4 (vptr only).
    // The DOL has no copy with a name; the layout and the vtable (destructor
    // then three null slots) come from the instances TourF's
    // game/tour/CCockpitTournament.cpp emits.
    class CState {
        public:
            virtual ~CState() {}
            virtual void CtorState() = 0;
            virtual void LoopState() = 0;
            virtual void DtorState() = 0;
    };

    // A state whose hooks are member functions of T, called on m_pOwner.
    // Size 0x2C. An unset hook (a null pointer to member) is skipped.
    template <class T>
    class CStateTemplate : public CState {
        public:
            typedef void (T::*StateFunc)();

            CStateTemplate()
                : m_pOwner(0)
                , m_pCtor(0)
                , m_pLoop(0)
                , m_pDtor(0)
            {
            }

            virtual ~CStateTemplate() {}

            virtual void CtorState()
            {
                if (m_pCtor) {
                    (m_pOwner->*m_pCtor)();
                }
            }

            virtual void LoopState()
            {
                if (m_pLoop) {
                    (m_pOwner->*m_pLoop)();
                }
            }

            virtual void DtorState()
            {
                if (m_pDtor) {
                    (m_pOwner->*m_pDtor)();
                }
            }

            // (name guessed)
            void Set(T *pOwner, StateFunc pCtor, StateFunc pLoop, StateFunc pDtor)
            {
                m_pOwner = pOwner;
                m_pCtor = pCtor;
                m_pLoop = pLoop;
                m_pDtor = pDtor;
            }

            T *m_pOwner;         // 0x04
            StateFunc m_pCtor;   // 0x08
            StateFunc m_pLoop;   // 0x14
            StateFunc m_pDtor;   // 0x20
    };
}

#endif /* !BCS_CSTATE_HPP_ */
