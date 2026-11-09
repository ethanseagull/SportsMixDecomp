#ifndef MENU_CDISPTASKLIST_HPP_
#define MENU_CDISPTASKLIST_HPP_

namespace msw {
    namespace menu {

        // Size 0x10, vptr at 0x0C. Lives in the DOL. Nothing of it is used
        // yet; it is here for the layout of the classes that hold one.
        class CDispTaskList {
            private:
                unsigned char m_Unk00[0xC]; // 0x00: a nw4r::ut::List

            public:
                CDispTaskList();
                virtual ~CDispTaskList();
        };
    }
}

#endif /* !MENU_CDISPTASKLIST_HPP_ */
