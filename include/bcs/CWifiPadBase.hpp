#ifndef BCS_CWIFIPADBASE_HPP_
#define BCS_CWIFIPADBASE_HPP_

#include "bcs/dwc/CDwc.hpp"

namespace bcs {

    // Maps the pads of an online match to machines (AIDs) and local pads.
    // Only the static members seen so far.
    class CWifiPadBase {
        public:
            static long GetOnlinePadTotal();
            static long GetAIDFromOnlinePadID(long id);
            static long GetLocalPadIDFromOnlinePadID(long id);
            static long GetMyLocalPadID(long id);

        private:
            static bool IsInMatch()
            {
                dwc::CDwc *pDwc = dwc::CDwc::GetInstance();
                if (pDwc != 0 && pDwc->IsInMatch()) {
                    return true;
                }
                return false;
            }
    };
}

#endif /* !BCS_CWIFIPADBASE_HPP_ */
