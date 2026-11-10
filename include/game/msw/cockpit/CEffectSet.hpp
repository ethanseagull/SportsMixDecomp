#ifndef MSW_COCKPIT_CEFFECTSET_HPP_
#define MSW_COCKPIT_CEFFECTSET_HPP_

namespace bcs {
    namespace G2D {
        class CPaneObj;
    }
}

namespace msw {
    namespace cockpit {

        // Size 0x44, vptr at 0x40. Lives in the DOL. Effects a cockpit
        // attaches to its panes: sixteen words, all cleared by the
        // constructor and by Initialize. Member and argument names and the
        // return types are guesses.
        class CEffectSet {
            public:
                CEffectSet();
                virtual ~CEffectSet();

                void Initialize();
                void Finalize();
                // Starts effect nEffect in slot (nGroup, nSlot) on pPaneObj.
                void SetEffect(long nGroup, long nSlot, long nEffect, bcs::G2D::CPaneObj *pPaneObj);
                void RemoveEffect(long nGroup, long nSlot);
                // Shows or hides every effect.
                void SetDispFlag(int bDisp);

            private:
                void *m_Unk00[16]; // 0x00
        };
    }
}

#endif /* !MSW_COCKPIT_CEFFECTSET_HPP_ */
