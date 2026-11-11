#ifndef MSW_COCKPIT_CEFFECTSET_HPP_
#define MSW_COCKPIT_CEFFECTSET_HPP_

namespace bcs {
    namespace G2D {
        class CPaneObj;
    }
}

namespace msw {
    namespace cockpit {

        // Effects attached to cockpit panes, two groups of eight handles.
        // Lives in the DOL (game/msw/cockpit/CMissionCount.cpp). Size 0x44,
        // vptr at 0x40.
        class CEffectSet {
            public:
                CEffectSet();

                void Initialize();
                void Finalize();
                // Starts effect nEffect on pObj in slot nIdx of group nGroup,
                // unless the slot is taken. A negative nIdx starts one that
                // is not kept.
                void SetEffect(long nGroup, long nIdx, long nEffect, bcs::G2D::CPaneObj *pObj);
                void RemoveEffect(long nGroup, long nIdx);
                // Shows or hides every kept effect.
                void SetDispFlag(int bDisp);
                // (inline; added for BasketF CCockpitBasketCoder::PlayAnimScore)
                // The handle kept in slot nIdx of group nGroup, 0 if none.
                unsigned long GetEffectHandle(long nGroup, long nIdx) const { return m_hEffect[nGroup][nIdx]; }

            private:
                enum {
                    GROUP_NUM = 2,
                    SLOT_NUM = 8
                };

                unsigned long m_hEffect[GROUP_NUM][SLOT_NUM]; // 0x00

            public:
                // vptr at 0x40: declared after the data.
                virtual ~CEffectSet();
        };
    }
}

#endif /* !MSW_COCKPIT_CEFFECTSET_HPP_ */
