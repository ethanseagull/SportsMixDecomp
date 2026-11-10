#ifndef BCS_CSND_HPP_
#define BCS_CSND_HPP_

namespace bcs {

    // The sound system (DOL). Only the members used so far.
    class CSnd {
        public:
            // Static: the DOL function takes no object. Returns 1 when the
            // sound system is ready (the body ends in `x == 1`).
            static int IsReady();
    };
}

#endif /* !BCS_CSND_HPP_ */
