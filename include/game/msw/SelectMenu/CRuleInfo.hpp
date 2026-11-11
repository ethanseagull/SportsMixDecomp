#ifndef SELECTMENU_CRULEINFO_HPP_
#define SELECTMENU_CRULEINFO_HPP_

#include "game/msw/CButtonEntry.hpp"
#include "game/msw/CSelectMenuLayoutBase.hpp"

namespace bcs {
    class CXmlReader;
}

namespace msw {
    namespace SelectMenu {

        class CRuleHelpTex;

        // Size 0x5F4. The rules screen ("rule_operation"): a row of tabs
        // (five or six, from "RuleExplanation.xml" for the current game
        // kind), each with pages of text and pictures shown in turn. Its
        // buttons are read from "rule_operation.btn" by the CButtonEntry
        // base at 0x5C. Menu object 0x4E of CSelectMenu;
        // CSelectMenu::RuleInfo_* drive it. Member names are guesses.
        class CRuleInfo : public CSelectMenuLayoutBase, public CButtonEntry {
            public:
                enum {
                    TAB_MAX = 7
                };

                CRuleInfo(long nId);
                virtual ~CRuleInfo();
                virtual void Main();
                virtual long SendEvent(long nEvent, void *pParam);
                virtual void Disp();
                virtual int Set();
                virtual void Remove();

                virtual long HitCheck(bcs::FVEC3 *pPos);
                virtual long SetCursorMarkerPosition(long, long);
                virtual long SetCursorMarkerVisible(long, int);
                virtual long StickMoveCheck(long nPad, long nBtnKindId);
                virtual long SetDefaultMoveCursor(long nPad, long nBtnKindId);
                virtual long PushButton(long nPad, long nBtnKindId);
                virtual int IsButtonKindOfStickMode(long nBtnKindId);

                // New in this class (as in CButton1); both return 0 here.
                // The return type is a guess.
                virtual long SetCursorAnim(long, long);
                virtual long PushCursorAnim(long, long);

                // Moves from one picture of the page to the next.
                void ChangePict();
                // Selects tab nTab (ignored when it is already selected or
                // out of range) and starts its first page.
                void SetTabSelect(long nTab);
                // Shows page nTab + 1 ("PAGE%d") of the tab element pNode.
                void SetPageNode(void *pNode, long nTab);

            private:
                // Inlined in the game; only its local static has a symbol.
                void CheckButtonAnim();

                friend class msw::CSelectMenu;

                unsigned long m_Unk5A0;          // 0x5A0, not touched by this unit
                long m_nTabSelect;               // 0x5A4, -1 before the first
                long m_bTabHit[TAB_MAX];         // 0x5A8, the pointer is on tab i
                long m_nItem;                    // 0x5C4, the item of the page shown next
                float m_fTime;                   // 0x5C8, frames left until the next item
                bcs::CXmlReader *m_pXml;         // 0x5CC, "RuleExplanation.xml"
                bcs::CXmlReader *m_pTabNode;     // 0x5D0, the game kind's element
                bcs::CXmlReader *m_pPage;        // 0x5D4, the page shown
                long m_nPict;                    // 0x5D8, the picture pane in front (0 or 1)
                CRuleHelpTex *m_pHelpTex;        // 0x5DC
                long m_nPictStep;                // 0x5E0, the step of ChangePict
                long m_Unk5E4;                   // 0x5E4, cleared by Set
                long m_Unk5E8;                   // 0x5E8, cleared by Set
                long m_nTabNum;                  // 0x5EC, 5 or 6
                const long *m_pTabPanes;         // 0x5F0, the tab panes
        };
    }
}

#endif /* !SELECTMENU_CRULEINFO_HPP_ */
