#ifndef CLYTCTRL_HPP_
#define CLYTCTRL_HPP_

#include "bcs/CTask.hpp"
#include "bcs/G2D/CLytManager.hpp"

namespace nw4r {
    namespace ut {
        class Font;
    }
    namespace lyt {
        class ArcResourceAccessor;
        class FontRefLink;
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

    // Lives in the DOL (game/msw/CLytCtrl.cpp). A task of size 0x3C
    // (CreateInstance allocates it), found through its handle; vtable
    // lbl_804715E8 holds the destructor, Main, Disp and CTask's
    // GetFrameRateF. It sets up the layout manager, the layout resources
    // and the archive fonts, and hides the layouts while the game pauses.
    // Apart from the static ones the members are non-static, although
    // several never read `this`.
    class CLytCtrl : public bcs::CTask {
        public:
            // (name guessed) An archive already in memory, for the second
            // CreateInstance: 0x10 bytes, built on the stack by the DOL's
            // caller at 0x8021CC0C (field names guessed).
            struct ArcInfo {
                long m_nId;            // 0x00, a CLoadData entry
                const char *m_pPath;   // 0x04
                void *m_pData;         // 0x08
                unsigned long m_nSize; // 0x0C
            };

            static CLytCtrl *GetInstance();
            // (return type guessed)
            static CLytCtrl *CreateInstance(bcs::CTask *pParent);
            // (the DOL's fn_8013EBC8; signature guessed from its one caller)
            // The same, taking the nNum archives of pArc that are layout
            // archives instead of the ones CLoadData holds.
            static CLytCtrl *CreateInstance(bcs::CTask *pParent, const ArcInfo *pArc, long nNum);

            // (the DOL's fn_8013E894)
            virtual ~CLytCtrl();
            // (the DOL's fn_8013F308)
            virtual void Main();
            // (the DOL's fn_8013F664, after LayoutTask's two functions)
            virtual void Disp() {}
            // (added for BossF game/boss/GameBoss.cpp, whose Finalize_ calls
            // the three without an object: static) Frees the layouts, lets
            // go of their resources and closes the archives.
            static void Dispose();
            static void DetachResource();
            static void Close();
            // Static: its body reads nothing through r3. Bit 0 of the
            // layout manager's word at 0x44, or 0 without a manager (return
            // type guessed; the caller in CCameraMenuBG.cpp turns it into 0 or 1).
            static unsigned long IsLocationAdjust();

            void DeleteObject(unsigned long nHandle);
            nw4r::lyt::ArcResourceAccessor *NewArcResourceAccessor(const char *pFileName, const char *pRootFolderName);
            // Added for MenuF game/menu/CBgDisplay.cpp (return types guessed).
            void DeleteArcResourceAccessor(nw4r::lyt::ArcResourceAccessor *pAccessor);
            void UnregistFont(nw4r::lyt::ArcResourceAccessor *pAccessor, nw4r::lyt::FontRefLink *pLink);
            nw4r::ut::Font *GetArchiveFont(long nFont);   // return type guessed (some font pointer or null)
            const char *GetFontResourceName(long nFont);
            // Added for BossF game/boss/CCockpitBossCoder.cpp (the spelling
            // is the symbol's): an accessor for the first of the three
            // layout archives that has resource pLayoutName of type nType
            // (0: any); the caller owns it. Null when none has it.
            nw4r::lyt::ResourceAccessor *GetLayoutResourceAccesor(long nType, const char *pLayoutName);
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

            // (added for TourF game/tour/CTournamentStep.cpp; static)
            // Stop and restart the layouts' animations for a pause.
            // -1 without a layout manager, else 0.
            static long SetPauseStop();
            static long ClearPauseStop();

        private:
            // (inline in CLytCtrl.cpp) The first hands the layout archives
            // CLoadData holds to bcs::G2D::CLytResource itself.
            inline CLytCtrl(bcs::CTask *pParent);
            inline CLytCtrl(bcs::CTask *pParent, const ArcInfo *pArc, long nNum);

            // (the DOL's fn_8013EC84; name guessed) Hands the archives of
            // pArc that are layout archives to bcs::G2D::CLytResource.
            static void AttachResource(const ArcInfo *pArc, long nNum);
            // (the DOL's fn_8013E988; name guessed) Builds the archive
            // fonts and sets the layouts' 16:9 location adjust.
            void Initialize();
            // (the DOL's fn_8013F30C; name guessed) Hides the layouts
            // while the game's pause bit or the application pause is on.
            void UpdatePause();

            // (name guessed) Bit 0: hidden for the game's pause bit; bit 3:
            // hidden for the application pause. In CTask's tail padding.
            unsigned char m_nPauseFlag;    // 0x32
            unsigned long m_Unk34[2];      // 0x34, not touched here

            // (lbl_805C0C38; name guessed) The instance's task handle.
            static unsigned long s_InstanceHandle;
    };
}

#endif /* !CLYTCTRL_HPP_ */
