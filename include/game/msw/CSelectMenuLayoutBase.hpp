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
            // Pure again here: the class's vtable (MenuF .data 0x17A0) has
            // null entries for the two, where CMenuLinkBase has inline
            // bodies.
            virtual int Set() = 0;
            virtual void Remove() = 0;
            virtual void SetVisible(int bVisible);

            virtual void CloseMenu();
            virtual void CallbackButtonAnimIn(long);
            virtual void CallbackButtonAnimOut(long);
            virtual void CallbackButtonAnimPush(long);
            virtual void CallbackButtonAnimCancel(long);
            virtual void CallbackButtonAnimMultiIn(long, long);
            virtual void CallbackButtonAnimMultiOut(long, long);
            // The return types from here to IsButtonKindOfStickMode are
            // read from the base bodies (all return a constant), from
            // CButton1's overrides and from how CSelectMenu uses the
            // results; `long` against `int` is a guess. The argument names
            // are CButton1's.
            // The kind id of the button under *pPos, or an event result.
            virtual long HitCheck(bcs::FVEC3 *pPos);
            virtual long SetCursorMarkerPosition(long, long);
            virtual long ClearCursorMarkerPosition(long, long);
            virtual long SetCursorMarkerVisible(long, int);
            // The kind id the stick of pad nPad moves to from nBtnKindId.
            virtual long StickMoveCheck(long nPad, long nBtnKindId);
            virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
            virtual float GetOutAnimFrameSize();
            virtual long PushButton(long nPad, long nBtnKindId);
            virtual int IsMarkerOnPointer(long, long);
            virtual int IsButtonKindOfStickMode(long nBtnKindId);

            // Non-virtual helpers defined in CSelectMenuLayoutBase.cpp; the
            // argument lists are the game's.
            // Creates m_pPaneAccessor; -1 when the allocation fails, else 0.
            long SetPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum);
            void DeletePaneAccessor();
            void DeleteArcFont();
            void ClearPushButtonId();
            void DeleteMenuPartList();
            // Creates a CPaneCtrl, registers it and appends its handle; null
            // on failure. The two longs go to CLytManager::AddObject /
            // CLayoutBase::CreateLayoutObject and CLayoutDL::SetGDLMemArea.
            bcs::G2D::CPaneCtrl *CreateLytObj(nw4r::lyt::ArcResourceAccessor *pAccessor, long nUnk, long nGDLMemArea);
            void SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout);
            // The kind id of the button pad nPad has selected on the owner's
            // current screen; 0 without an owner.
            unsigned short GetCurrentButtonKindId(long nPad);
            // Sound and rumble, each only when enabled by m_bSeEnable /
            // m_bVibEnable. The argument names are guesses.
            unsigned long SE_Play(long nSe, long nUnk);
            unsigned long Voice_Play(long nVoice, long nUnk);
            void Voice_Stop(long nVoice);
            void Vib_Start(long nPad, long nPattern, unsigned long nUnk);
            // Whether the owner's back button is moving in / is the
            // selected button.
            int IsBackFrameIn(long nPad);
            int IsBackFrameOut(long nPad);

        protected:
            // CSelectMenu's screen steps read m_Unk30 and write
            // m_nPushButtonId of their menu objects (game/menu/CHowTo.cpp).
            friend class CSelectMenu;

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
            unsigned char *m_Unk4C;              // 0x4C, an array freed with delete[]
            int m_bSeEnable;                     // 0x50, tested by SE_Play and Voice_Play
            int m_bVibEnable;                    // 0x54, tested by Vib_Start
            CMenuPartList *m_pMenuPartList;      // 0x58
    };
}

#endif /* !CSELECTMENULAYOUTBASE_HPP_ */
