#ifndef CSELECTMENULAYOUTBASE_HPP_
#define CSELECTMENULAYOUTBASE_HPP_

#include "bcs/G2D/CPaneCtrl.hpp"
#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/CMenuLytObj.hpp"
#include "game/msw/CMenuPaneAccessor.hpp"
#include "game/msw/CMenuPartList.hpp"
#include "game/msw/CSelectMenu.hpp"

namespace bcs {
    struct FVEC3;
}

namespace nw4r {
    namespace ut {
        class ArchiveFont;
    }
}

namespace msw {

    // Size 0x5C. Base of the SelectMenu screens. CMenuLinkBase is at 0 (vptr
    // at 0x18), CMenuLytObj at 0x1C (vptr at 0x28), own members from 0x2C.
    // Member names and the return types of the virtuals are guesses unless
    // a comment says otherwise.
    class CSelectMenuLayoutBase : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CPaneCtrl, 1> {
        public:
            // Values of m_Unk30, from CButton0::Main and CloseMenu. The
            // names are guesses.
            enum {
                STATE_NONE = 0,
                STATE_SET = 1,     // set by Set()
                STATE_IN = 2,      // waiting for the opening animation
                STATE_ACTIVE = 3,
                STATE_CLOSE = 4,   // set by CloseMenu()
                STATE_OUT = 5      // waiting for the closing animation
            };

            CSelectMenuLayoutBase(long nId);
            virtual ~CSelectMenuLayoutBase();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual void SetVisible(int bVisible);

            virtual void CloseMenu();
            virtual void CallbackButtonAnimIn(long);
            virtual void CallbackButtonAnimOut(long);
            virtual void CallbackButtonAnimPush(long);
            virtual void CallbackButtonAnimCancel(long);
            virtual void CallbackButtonAnimMultiIn(long, long);
            virtual void CallbackButtonAnimMultiOut(long, long);
            virtual void HitCheck(bcs::FVEC3 *);
            virtual void SetCursorMarkerPosition(long, long);
            virtual void ClearCursorMarkerPosition(long, long);
            virtual void SetCursorMarkerVisible(long, int);
            virtual void StickMoveCheck(long, long);
            virtual void SetDefaultMoveCursor(long, long);
            virtual void GetOutAnimFrameSize();
            virtual void PushButton(long, long);
            virtual void IsMarkerOnPointer(long, long);
            virtual void IsButtonKindOfStickMode(long);

            // Non-virtual helpers defined in CSelectMenuLayoutBase.cpp; the
            // argument lists are the game's.
            // Creates m_pPaneAccessor; -1 when the allocation fails, else 0.
            long SetPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum);
            void DeletePaneAccessor();
            void DeleteArcFont();
            void ClearPushButtonId();
            // Creates a CPaneCtrl, registers it and appends its handle; null
            // on failure. The two longs go to CLytManager::AddObject /
            // CLayoutBase::CreateLayoutObject and CLayoutDL::SetGDLMemArea.
            bcs::G2D::CPaneCtrl *CreateLytObj(nw4r::lyt::ArcResourceAccessor *pAccessor, long nUnk, long nGDLMemArea);
            void SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout);

        protected:
            // The owner as a CSelectMenu: the base's helpers call
            // CSelectMenu members on CMenuLinkBase::m_Unk10. No such
            // function has a name in the game.
            CSelectMenu *GetSelectMenu() const
            {
                return (CSelectMenu *)m_Unk10;
            }

            unsigned long m_Unk2C;               // 0x2C
            long m_Unk30;                        // 0x30, the state: STATE_* (name kept for CFade.cpp)
            unsigned long m_Unk34;               // 0x34
            nw4r::ut::ArchiveFont *m_pArcFont;   // 0x38, released by DeleteArcFont
            CMenuPaneAccessor *m_pPaneAccessor;  // 0x3C
            unsigned short m_Unk40;              // 0x40
            unsigned short m_nPushButtonId[4];   // 0x42, cleared by ClearPushButtonId
            void *m_Unk4C;                       // 0x4C, an array freed with delete[]
            int m_bSeEnable;                     // 0x50, tested by SE_Play and Voice_Play
            int m_bVibEnable;                    // 0x54, tested by Vib_Start
            CMenuPartList *m_pMenuPartList;      // 0x58
    };
}

#endif /* !CSELECTMENULAYOUTBASE_HPP_ */
