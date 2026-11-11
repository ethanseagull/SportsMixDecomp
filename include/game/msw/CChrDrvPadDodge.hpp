#ifndef MSW_CCHRDRVPADDODGE_HPP_
#define MSW_CCHRDRVPADDODGE_HPP_

#include "game/msw/CChrDrv.hpp"

namespace msw {

    // INCOMPLETE: the pad driver of the dodgeball game (DodgeF), size 0x1A8
    // (what CChrManDodge::CreateDriver_ allocates). Its base in the original
    // is the DOL's CChrDrvPad, which is not declared yet; only the
    // constructor and destructor are here, so the vtable is not emitted from
    // this declaration.
    class CChrDrvPadDodge : public CChrDrv {
        public:
            CChrDrvPadDodge();
            virtual ~CChrDrvPadDodge();

        protected:
            unsigned char m_Unk04[0x1A8 - 0x04];
    };
}

#endif /* !MSW_CCHRDRVPADDODGE_HPP_ */
