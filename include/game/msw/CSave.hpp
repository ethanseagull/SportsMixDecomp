#ifndef CSAVE_HPP_
#define CSAVE_HPP_

namespace msw {

    // Only the members used so far. All are static: their bodies in the DOL
    // work on a global instance pointer and take no object. Return types the
    // callers ignore are guessed as void.
    class CSave {
        public:
            static void Initialize();
            static int CanbeWrite(); // return type guessed
            static void SaveDataInit();
    };
}

#endif /* !CSAVE_HPP_ */
