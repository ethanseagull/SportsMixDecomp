#ifndef MSW_CCHRMENUOBJ_HPP_
#define MSW_CCHRMENUOBJ_HPP_

namespace msw {

    // A character model shown in the menus, kept in msw::CChrMenuCtrl's
    // list. Size 0x48, vptr at 0x44 (MenuF). Only what is used so far is
    // declared, so the class must not be created from a module source yet.
    class CChrMenuObj {
        public:
            void SetAlpha(float alpha);
    };
}

#endif /* !MSW_CCHRMENUOBJ_HPP_ */
