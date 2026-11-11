#ifndef BCS_CSIMPLESORTBASE_HPP_
#define BCS_CSIMPLESORTBASE_HPP_

namespace bcs {

    // (added for BossF game/boss/CComDrvBoss.cpp; names from the symbols
    // of its instances, the rest guessed) Entries to sort by a number with
    // bcs::BCS_ShakerSort, up to N of them. BossF's CComDrvBoss sorts the
    // characters by their distance with it (BasketF has the same instances).
    template <int N>
    class CSimpleSortBase {
        public:
            struct SORT {
                long m_nId;       // what the entry stands for
                float m_fParam;   // what it is sorted by

                // BCS_ShakerSortCompFunc's instance for SORT returns 0: the
                // entries have no order of their own.
                int operator>(const SORT &) const { return 0; }
            };

            // Ascending by m_fParam.
            static long SortCmpParamF(const SORT *pA, const SORT *pB)
            {
                return pA->m_fParam > pB->m_fParam;
            }
    };
}

#endif /* !BCS_CSIMPLESORTBASE_HPP_ */
