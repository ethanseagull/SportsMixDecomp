#ifndef SELECTMENU_CCUPCHR_HPP_
#define SELECTMENU_CCUPCHR_HPP_

#include "game/msw/CSelectMenuLayoutBase.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CMenuMdlTableSet;
    class CTrophyObj;

    namespace SelectMenu {

        class CCourtCharaObj;

        // The cup on its stand shown on the cup selection screen (MenuF,
        // game/menu/CCupChr.cpp). Size 0x84 (layout.py). The stand is a
        // model of "chara_table.brres" played through a CMenuMdlTableSet;
        // the cup is a msw::CTrophyObj registered in msw::CChrMenuCtrl and
        // faded in and out by a CCourtCharaObj. Member names are guessed.
        class CCupChr : public CSelectMenuLayoutBase {
            public:
                // Events of SendEvent (names guessed from what they do).
                enum {
                    EVENT_SHOW = 0x3F,       // pParam null: hide the cup and reset the stand
                    EVENT_PLAY_ANIM = 0x40,  // play animation (long)pParam unless it is playing
                    EVENT_PLAY_ANIM_FORCE = 0x41,
                    EVENT_PUSH_ANIM = 0x42,  // queue animation (long)pParam
                    EVENT_CHANGE_ANIM = 0x43 // drop the queue, then queue (long)pParam
                };

                CCupChr(long nId);
                virtual ~CCupChr();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual int Set();
                virtual void Remove();

                void Initialize();
                // A model of pName in file, drawn in layer 2 with motion
                // slot 0 attached; null if there is none.
                bcs::CMdlObj *CreateMdlObj(const char *pName, nw4r::g3d::ResFile file);
                // Makes cup nType (1 to 4) of kind nKind (0 to 5) and
                // registers it with id nId in msw::CChrMenuCtrl.
                CTrophyObj *CreateChrMenuObj(long nId, long nType, long nKind);
                // Turns the cup models.
                void StepControl();
                // Whether the stand's animation has ended (1 without one).
                int IsAnimEnd();
                // Shows cup nType of kind nKind on the stand nIndex; 0 if
                // that cup is already shown, else 1.
                int SetCharaModel(long nIndex, long nType, long nKind, long nSlot);

            private:
                // 0x5C: an array, constructed in a loop.
                nw4r::g3d::ResFile m_ResFile[1];    // 0x5C
                CMenuMdlTableSet *m_pTableSet;      // 0x60, the stand
                unsigned long m_hMdl[2];            // 0x64, CChrMenuCtrl handles; [1] is the cup
                long m_Unk6C;                       // 0x6C
                unsigned char m_nShown[4];          // 0x70, index into m_hMdl
                int m_bClose;                       // 0x74, StepControl closes the menu when set
                float m_Unk78;                      // 0x78
                CCourtCharaObj *m_pCharaObj[1];     // 0x7C
                long m_nAnim;                       // 0x80, the stand's animation, -1 for none
        };
    }
}

#endif /* !SELECTMENU_CCUPCHR_HPP_ */
