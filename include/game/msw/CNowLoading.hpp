#ifndef CNOWLOADING_HPP_
#define CNOWLOADING_HPP_

namespace msw {

    // Only the member used so far.
    class CNowLoading {
        public:
            // Static: the body in the DOL stores into a global instance.
            static void SetType(long);
    };
}

#endif /* !CNOWLOADING_HPP_ */
