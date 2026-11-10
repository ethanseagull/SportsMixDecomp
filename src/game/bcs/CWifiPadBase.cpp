#include "bcs/CWifiPadBase.hpp"

namespace bcs {

    long CWifiPadBase::GetOnlinePadTotal()
    {
        dwc::CDwc *pDwc = dwc::CDwc::GetInstance();
        return pDwc != 0 ? pDwc->GetOnlinePadTotal() : 0;
    }

    long CWifiPadBase::GetAIDFromOnlinePadID(long id)
    {
        dwc::CDwc *pDwc = dwc::CDwc::GetInstance();
        return pDwc != 0 ? pDwc->GetAIDFromOnlinePadID(id) : -1;
    }

    long CWifiPadBase::GetLocalPadIDFromOnlinePadID(long id)
    {
        dwc::CDwc *pDwc = dwc::CDwc::GetInstance();
        return pDwc != 0 ? pDwc->GetLocalPadIDFromOnlinePadID(id) : -1;
    }

    long CWifiPadBase::GetMyLocalPadID(long id)
    {
        if (IsInMatch()) {
            dwc::CDwc *pDwc = dwc::CDwc::GetInstance();
            if (pDwc != 0 && pDwc->GetMyAID() == pDwc->GetAIDFromOnlinePadID(id)) {
                return pDwc->GetLocalPadIDFromOnlinePadID(id);
            }
            return -1;
        }
        return id;
    }
}
