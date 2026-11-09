#ifndef BCS_CLOADMAN_HPP_
#define BCS_CLOADMAN_HPP_

namespace bcs {

    // Only the members used so far. All are called without an object, so they
    // are static. Return types the callers ignore are guessed as void.
    class CLoadMan {
        public:
            static void SetRawDataBufferMem(long);
            static int IsComplete(); // return type guessed
    };
}

#endif /* !BCS_CLOADMAN_HPP_ */
