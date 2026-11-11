#ifndef MSW_CCHRMENUCTRL_HPP_
#define MSW_CCHRMENUCTRL_HPP_

#include "nw4r/ut/List.hpp"
#include "revolution/rfl.h"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CChrMdlObj;
    class CChrMenuObj;

    // Owns the character models of the menus (MenuF,
    // game/menu/CChrMenuCtrl.cpp). Size 0x14: a list of msw::CChrMenuObj at
    // 0x00, a counter at 0x0C, vptr at 0x10. One static instance.
    class CChrMenuCtrl {
        public:
            void Initialize();
            // Deletes every object in the list.
            void Finalize();
            // A new object with a model of character nChr. Null if the model
            // could not be made. (meanings of nSub and the word after the
            // source not known)
            CChrMenuObj *CreateChrMenuObj(long nChr, long nSub, RFLDataSource source, long, long nLayer);
            // A new object for a model made elsewhere; null without one.
            CChrMenuObj *CreateChrMenuObj(long nChr, long nSub, bcs::CMdlObj *pMdl);
            // The object registered under a handle, or null.
            CChrMenuObj *GetChrMenuObj(unsigned long handle);
            // Removes and deletes the object registered under a handle.
            void DeleteChrMenuObj(unsigned long handle);

            static CChrMenuCtrl s_Instance;

        private:
            CChrMdlObj *CreateMdlObj(long nChr, long nSub, RFLDataSource source, long, long nLayer);
            // (name guessed) Inline: the lookup GetChrMenuObj and
            // DeleteChrMenuObj share.
            CChrMenuObj *Find(unsigned long handle);

            nw4r::ut::List m_List;  // 0x00 (name guessed)
            long m_nCount;          // 0x0C, serial number of the next object

        public:
            // Declared after the data: the vptr is at 0x10.
            virtual ~CChrMenuCtrl();
    };
}

#endif /* !MSW_CCHRMENUCTRL_HPP_ */
