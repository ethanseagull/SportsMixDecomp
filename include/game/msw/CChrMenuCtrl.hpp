#ifndef MSW_CCHRMENUCTRL_HPP_
#define MSW_CCHRMENUCTRL_HPP_

#include "nw4r/ut/List.hpp"

namespace msw {

    class CChrMenuObj;

    // Owns the character models of the menus (MenuF). Size 0x14: a list of
    // msw::CChrMenuObj at 0x00, a word at 0x0C, vptr at 0x10. One static
    // instance. Only what is used so far is declared.
    class CChrMenuCtrl {
        public:
            // The object registered under a handle, or null.
            CChrMenuObj *GetChrMenuObj(unsigned long handle);
            // Removes and deletes the object registered under a handle.
            void DeleteChrMenuObj(unsigned long handle);

            static CChrMenuCtrl s_Instance;

        private:
            nw4r::ut::List m_List;  // 0x00 (name guessed)
            unsigned long m_Unk0C;  // 0x0C

        public:
            // Declared after the data: the vptr is at 0x10.
            virtual ~CChrMenuCtrl();
    };
}

#endif /* !MSW_CCHRMENUCTRL_HPP_ */
