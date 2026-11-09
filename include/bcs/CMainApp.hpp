#ifndef BCS_CMAINAPP_HPP_
#define BCS_CMAINAPP_HPP_

namespace bcs {

    // Only the members used so far. All are called without an object, so they
    // are static. Return types the callers ignore are guessed as void.
    class CMainApp {
        public:
            static float GetAppFrameRateF();
            static void StartMasterFade(long, float);
            static int IsPlayMasterFade(); // return type guessed
    };
}

#endif /* !BCS_CMAINAPP_HPP_ */
