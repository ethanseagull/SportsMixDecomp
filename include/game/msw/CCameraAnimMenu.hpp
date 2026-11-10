#ifndef MSW_CCAMERAANIMMENU_HPP_
#define MSW_CCAMERAANIMMENU_HPP_

#include "bcs/CCameraAnm.hpp"

namespace msw {

    // The menu background's camera (size 0x1A4, same as bcs::CCameraAnm).
    // It overrides nothing but the destructor. The constructor is inline in
    // msw::CCameraMenuBG::Initialize.
    class CCameraAnimMenu : public bcs::CCameraAnm {
        public:
            CCameraAnimMenu(long view, short priority, bcs::CTask *pParent)
                : CCameraAnm(view, priority, pParent)
            {
            }

            virtual ~CCameraAnimMenu();
    };
}

#endif /* !MSW_CCAMERAANIMMENU_HPP_ */
