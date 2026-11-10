#ifndef BCS_CACCINPUTMAN_HPP_
#define BCS_CACCINPUTMAN_HPP_

namespace bcs {

    // (name guessed; no symbol) Keeps four pointers (one per pad) to objects
    // of 0x2FE8 bytes, in an array in the DOL's .bss. Lives in the DOL. Only
    // the member used so far.
    class CAccInputMan {
        public:
            // (name guessed) Creates the object of every pad that has none.
            // The body is the DOL's fn_800AE9C8.
            static void CreateInstances();
    };
}

#endif /* !BCS_CACCINPUTMAN_HPP_ */
