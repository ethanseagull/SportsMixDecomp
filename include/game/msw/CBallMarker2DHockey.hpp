#ifndef CBALLMARKER2DHOCKEY_HPP_
#define CBALLMARKER2DHOCKEY_HPP_

#include "game/msw/CMenuLinkBase.hpp"

namespace msw {

    // Size 0x34 (CCockpitHockey::CreateObjImpl allocates it): the 2D marker
    // of the puck. Its constructor builds a bcs::G2D::CPaneCtrl; the members
    // after CMenuLinkBase are not mapped yet.
    class CBallMarker2DHockey : public CMenuLinkBase {
        public:
            CBallMarker2DHockey(long nId);
            virtual ~CBallMarker2DHockey();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();

        private:
            unsigned char m_Unk1C[0x34 - 0x1C];
    };
}

#endif /* !CBALLMARKER2DHOCKEY_HPP_ */
