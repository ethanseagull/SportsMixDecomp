#ifndef BCS_BCS_SHAKERSORT_HPP_
#define BCS_BCS_SHAKERSORT_HPP_

namespace bcs {

    // The default order of BCS_ShakerSort: ascending by operator>.
    // (Instances in TourF game/tour/CTournamentStep, the module stage units
    // and game/bcs/dwc/CDwcFriend; names from their symbols.)
    template <typename T>
    long BCS_ShakerSortCompFunc(const T *pA, const T *pB)
    {
        return *pA > *pB;
    }

    // Sorts the nNum elements of pArray in place with a cocktail shaker
    // sort. pCompFunc returns 1 or more when its first argument belongs
    // after the second; null means BCS_ShakerSortCompFunc.
    template <typename T>
    void BCS_ShakerSort(T *pArray, long nNum, long (*pCompFunc)(const T *, const T *))
    {
        if (nNum <= 1) {
            return;
        }
        if (pCompFunc == 0) {
            pCompFunc = BCS_ShakerSortCompFunc<T>;
        }

        long nLeft = 0;
        long nRight = nNum - 1;
        while (nLeft < nRight) {
            long nLast = nLeft;
            for (long i = nLeft; i < nRight; i++) {
                if (pCompFunc(&pArray[i], &pArray[i + 1]) >= 1) {
                    T tmp = pArray[i];
                    pArray[i] = pArray[i + 1];
                    pArray[i + 1] = tmp;
                    nLast = i;
                }
            }
            nRight = nLast;
            if (nLeft >= nRight) {
                break;
            }

            for (long i = nRight; i > nLeft; i--) {
                if (pCompFunc(&pArray[i - 1], &pArray[i]) >= 1) {
                    T tmp = pArray[i];
                    pArray[i] = pArray[i - 1];
                    pArray[i - 1] = tmp;
                    nLast = i;
                }
            }
            nLeft = nLast;
        }
    }
}

#endif /* !BCS_BCS_SHAKERSORT_HPP_ */
