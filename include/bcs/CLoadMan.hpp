#ifndef BCS_CLOADMAN_HPP_
#define BCS_CLOADMAN_HPP_

namespace bcs {

    // Only the members used so far. All are static: their bodies in the DOL
    // read globals and take no object. Return types the callers ignore are
    // guessed as void.
    class CLoadMan {
        public:
            static void SetRawDataBufferMem(long);
            static int IsComplete(); // return type guessed
            // Data of a loaded file. What the second argument selects is
            // not known yet.
            static void *GetPtr(const char *pPath, unsigned long);
            static void UnLoad(const char *pPath, unsigned long);
    };
}

#endif /* !BCS_CLOADMAN_HPP_ */
