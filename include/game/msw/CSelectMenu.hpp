#ifndef CSELECTMENU_HPP_
#define CSELECTMENU_HPP_

namespace msw {

    // The task behind the SelectMenu screens (MenuF). Only the members used
    // so far; both are non-static and neither reads `this`.
    class CSelectMenu {
        public:
            const char *GetArchiveFileName(long nArchive);
            const char *GetArchiveRootFolderName(long nArchive);
    };
}

#endif /* !CSELECTMENU_HPP_ */
