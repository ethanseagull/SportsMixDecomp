#ifndef CCOCKPITLAYOUTBASE_HPP_
#define CCOCKPITLAYOUTBASE_HPP_

#include "bcs/G2D/CLayoutDL.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/New.hpp"
#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/CMenuLytObj.hpp"
#include "game/msw/menu/CDispTaskList.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {
    class CCockpit;
    class CMenuPaneAccessor;
    class CMenuPartList;

    // Size 0x60. Base of the in-game displays that are made of one layout.
    // Lives in the DOL. CMenuLinkBase is at 0 (vptr at 0x18), CMenuLytObj at
    // 0x1C (vptr at 0x28), own members from 0x2C. The layout is read from
    // the constructors and the destructor; member names are guesses, and so
    // are the return types of the virtual functions.
    class CCockpitLayoutBase : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CPaneCtrl, 1> {
        public:
            CCockpitLayoutBase(long nId);
            CCockpitLayoutBase(long nId, float fSortZ);
            virtual ~CCockpitLayoutBase();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            // Pure again here: the vtables of this family have null entries
            // for the two, where CMenuLinkBase has inline bodies.
            virtual int Set() = 0;
            virtual void Remove() = 0;
            virtual void SetVisible(int bVisible);

            virtual void CancelMenuImpl();
            virtual void HitCheck(bcs::FVEC3 &);
            virtual void OnCursorCheck(long, unsigned short);
            virtual void StickMoveCheck(long, unsigned short);
            virtual void IsButtonKindOfStickMode(unsigned short);
            virtual long CancelMenu(); // returns 0 (the original sets r3)
            virtual void PushButton(long);
            // The next four are inline: each module that derives a class
            // emits them. They return 0.
            virtual int SetCursorAnim(long, unsigned short) { return 0; }
            virtual int PushCursorAnim(long, unsigned short) { return 0; }
            virtual int SetCursorMarkerPosition(long, unsigned short) { return 0; }
            virtual int SetCursorMarkerVisible(long, int) { return 0; }

            // Creates m_pPaneAccessor; -1 when the allocation fails, else 0.
            long SetPaneAccessor(bcs::G2D::CPaneCtrl *pPaneCtrl, const char **ppPaneNames, long nNum);
            // pBtnKindId has four entries.
            void SetDefaultBtnKindId(const unsigned short *pBtnKindId);
            long CheckButtonAnim(); // returns 0
            // Added for TourF game/tour/CCockpitTournamentTelopLogo.cpp. Fills
            // the layout's text boxes from the message data (through
            // CLytCtrl). Return type guessed.
            void SetTextBoxFromMessage(bcs::G2D::CLayoutBase *pLayout);
            // Deletes m_pPaneAccessor, if any (return type guessed).
            void DeletePaneAccessor();

        private:
            // (added for game/msw/CCockpitButtonInfo.cpp) Its
            // ButtonParameterInitialize sets m_nDefaultBtnKindId.
            friend class CCockpitButtonInfo;

        protected:
            // The owner as a CCockpit (CMenuLinkBase::m_Unk10), as
            // CSelectMenuLayoutBase::GetSelectMenu does for the menus. No such
            // function has a name in the game.
            CCockpit *GetCockpit() const
            {
                return (CCockpit *)m_Unk10;
            }

            // (name guessed; no symbol) Makes a CPaneCtrl on archive
            // pAccessor and hands it to the layout manager. Null when either
            // is missing.
            static bcs::G2D::CPaneCtrl *NewPaneCtrl(nw4r::lyt::ArcResourceAccessor *pAccessor, long nUnk)
            {
                if (pAccessor == 0) {
                    return 0;
                }

                bcs::G2D::CLytManager *pManager = bcs::G2D::CLytManager::GetInstance();
                bcs::G2D::CPaneCtrl *pPaneCtrl = new (0) bcs::G2D::CPaneCtrl;

                if (pPaneCtrl == 0) {
                    return 0;
                }
                pManager->AddObject(pPaneCtrl, nUnk);
                pPaneCtrl->CreateLayoutObject(nUnk);
                pPaneCtrl->AttachArchiveResource(pAccessor);
                return pPaneCtrl;
            }

            // (no symbol; named after CSelectMenuLayoutBase::CreateLytObj,
            // which does the same out of line) Creates the layout object and
            // appends its handle. Inline in every Set function.
            bcs::G2D::CPaneCtrl *CreateLytObj(nw4r::lyt::ArcResourceAccessor *pAccessor, long nUnk, long nGDLMemArea)
            {
                if (CLytCtrl::GetInstance() == 0) {
                    return 0;
                }

                bcs::G2D::CPaneCtrl *pPaneCtrl = NewPaneCtrl(pAccessor, nUnk);

                if (pPaneCtrl != 0) {
                    m_nHandle[m_nNum] = pPaneCtrl->m_nId;
                    pPaneCtrl->GetLayout()->SetGDLMemArea(nGDLMemArea);
                    m_nNum++;
                }
                return pPaneCtrl;
            }

            long m_nStep;                        // 0x2C, 1 to 3: which Step function runs
            long m_nState;                       // 0x30, what StateControl switches on
            CMenuPaneAccessor *m_pPaneAccessor;  // 0x34
            CMenuPartList *m_pMenuPartList;      // 0x38
            menu::CDispTaskList m_DispTaskList;  // 0x3C
            unsigned short m_nDefaultBtnKindId[4]; // 0x4C
            unsigned short m_nBtnKindId[4];      // 0x54
            float m_fSortZ;                      // 0x5C
    };
}

#endif /* !CCOCKPITLAYOUTBASE_HPP_ */
