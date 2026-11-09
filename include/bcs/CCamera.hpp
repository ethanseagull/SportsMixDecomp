#ifndef BCS_CCAMERA_HPP_
#define BCS_CCAMERA_HPP_

namespace bcs {

    // Only the member used so far.
    class CCamera {
        public:
            // Static: the body in the DOL reads the data block through r3.
            static void LoadQuakeData(const void *pData);
    };
}

#endif /* !BCS_CCAMERA_HPP_ */
