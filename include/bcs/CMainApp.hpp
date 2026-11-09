#ifndef BCS_CMAINAPP_HPP_
#define BCS_CMAINAPP_HPP_

namespace bcs {

    // Only the members used so far. All are static: their bodies in the DOL
    // read globals and take no object. Return types the callers ignore are
    // guessed as void.
    class CMainApp {
        public:
            static float GetAppFrameRateF();
            static float GetUnivSpeed();
            static void StartMasterFade(long, float);
            static int IsPlayMasterFade(); // return type guessed
    };
}

#endif /* !BCS_CMAINAPP_HPP_ */
