#ifndef CMENU_HPP_
#define CMENU_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // Root task of the menus. Lives in module MenuF. Size 0x88, laid out
    // like CMainStep: bcs::CTask at 0x00, bcs::CFsm<CMenu> at 0x34, own
    // members from 0x7C. Only what callers outside the class need so far;
    // the list of virtual functions is not complete.
    class CMenu : public bcs::CTask, public bcs::CFsm<CMenu> {
        public:
            CMenu(bcs::CTask *pParent);
            virtual ~CMenu();
            virtual void Main();
            virtual void Disp();

        private:
            unsigned char m_Unk7C[0x88 - 0x7C];
    };
}

#endif /* !CMENU_HPP_ */
