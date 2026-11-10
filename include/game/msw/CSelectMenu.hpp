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

            // Added for MenuF game/menu/CSelectMenuLayoutBase.cpp (return
            // type guessed).
            int IsBackButtonMove(long nPad);

            // Data, added for the same unit. The class is 0xAB8 bytes (the
            // size layout.py finds); only two members are named so far.
            // The second polymorphic part mentioned above (a `@68@` thunk)
            // lies somewhere in the padding and is not modelled yet.
        private:
            unsigned char m_Unk4C[0x8];      // 0x4C

        public:
            // 0x54. (name guessed) The current screen: read by
            // GetPrevMode, GoBackToMode and most screen steps.
            long m_nMode;

        private:
            unsigned char m_Unk58[0x46];     // 0x58

        public:
            // 0x9E. (name guessed) The selected button kind id per pad and
            // mode (0x66 bytes per pad: 51 modes). Saved by Backup().
            unsigned short m_nCurrentBtnKindId[4][51];

        private:
            unsigned char m_Unk236[0xAB8 - 0x236]; // 0x236
    };
}

#endif /* !CSELECTMENU_HPP_ */
