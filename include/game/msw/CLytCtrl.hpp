#ifndef CLYTCTRL_HPP_
#define CLYTCTRL_HPP_

#include "bcs/G2D/CLytManager.hpp"

namespace nw4r {
    namespace ut {
        class Font;
    }
    namespace lyt {
        class ArcResourceAccessor;
        class ResourceAccessor;
    }
}

namespace bcs {
    class CTask;

    namespace G2D {
        class CLayoutBase;
    }
}

namespace msw {

    // Lives in the DOL (a task found through its handle). Only the members
    // used so far. Apart from GetInstance they are non-static, although
    // several never read `this`.
    class CLytCtrl {
        public:
            static CLytCtrl *GetInstance();
            // (return type guessed)
            static CLytCtrl *CreateInstance(bcs::CTask *pParent);
            // Static: its body reads nothing through r3. Bit 0 of the
            // layout manager's word at 0x44, or 0 without a manager (return
            // type guessed; the caller in CCameraMenuBG.cpp turns it into 0 or 1).
            static unsigned long IsLocationAdjust();

            void DeleteObject(unsigned long nHandle);
            nw4r::lyt::ArcResourceAccessor *NewArcResourceAccessor(const char *pFileName, const char *pRootFolderName);
            nw4r::ut::Font *GetArchiveFont(long nFont);   // return type guessed (some font pointer or null)
            const char *GetFontResourceName(long nFont);
            // Added for BossF game/boss/CCockpitBossCoder.cpp (the spelling
            // is the symbol's): the resources of layout pLayoutName, loaded
            // from archive nArc; null when not loaded. (argument meanings
            // and return type guessed)
            nw4r::lyt::ResourceAccessor *GetLayoutResourceAccesor(long nArc, const char *pLayoutName);
            // Added for MenuF game/menu/CSelectMenuLayoutBase.cpp. The second
            // argument is the word at 0x2C of pLayout's CLayoutDL; return
            // type guessed.
            void SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout, const char *pName);

            // Not in the DOL by name: the sequence appears inline in every
            // user (CLytManager::GetInstance, null check, GetObject). The
            // name and its being a member here are guesses.
            bcs::G2D::CBase *GetObject(unsigned long nHandle)
            {
                bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
                if (pManager == 0) {
                    return 0;
                }
                return pManager->GetObject(nHandle);
            }
    };
}

#endif /* !CLYTCTRL_HPP_ */
