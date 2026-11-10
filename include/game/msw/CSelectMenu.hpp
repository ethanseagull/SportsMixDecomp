#ifndef CSELECTMENU_HPP_
#define CSELECTMENU_HPP_

#include "game/msw/CMenuTask.hpp"

namespace msw {

    // The task behind the SelectMenu screens (MenuF). Its constructor calls
    // CMenuTask's. Only the members used so far; the two name getters are
    // non-static and neither reads `this`. The class has a second
    // polymorphic part (its destructor has a `@68@` thunk) that is not
    // written here, so do not create or destroy one through this header.
    class CSelectMenu : public CMenuTask {
        public:
            CSelectMenu(bcs::CTask *pParent);
            virtual ~CSelectMenu();

            const char *GetArchiveFileName(long nArchive);
            const char *GetArchiveRootFolderName(long nArchive);
    };
}

#endif /* !CSELECTMENU_HPP_ */
