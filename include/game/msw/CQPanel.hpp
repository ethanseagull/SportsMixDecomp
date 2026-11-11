#ifndef MSW_CQPANEL_HPP_
#define MSW_CQPANEL_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CQPanelMan.hpp"

namespace bcs {
    class CScnGroup;
}

namespace msw {

    class CItem;

    // One "?" panel, made by a CQPanelMan. A DOL class; each game module
    // derives its own (CQPanelBasket, ...). Size 0xF4 (CQPanelManBasket
    // allocates CQPanelBasket, which adds nothing, at 0xF4).
    // bcs::CFsm<CQPanel> is at 0x34 (vptr at 0x78). The virtual functions
    // follow the slots of __vt__Q23msw13CQPanelBasket (BasketF); return
    // types are guessed unless marked.
    //
    // INCOMPLETE: only the members the module code uses are mapped.
    class CQPanel : public bcs::CUnivTask, public bcs::CFsm<CQPanel> {
        public:
            // Bits of the word at 0x8C (names from the getters below).
            enum {
                FLAG_REQ_POPUP = 0x1,
                FLAG_CAN_ACCESS = 0x2
            };

            CQPanel(CQPanelMan *pMan, bcs::CScnGroup *pGroup, const bcs::FVEC3 &, const bcs::FVEC3 &,
                    CQPanelMan::PANEL_POP_INFO *pInfo, long);
            virtual ~CQPanel();
            virtual void Main();

            virtual void Init();
            virtual void Reset();
            virtual float GetSize() const;
            virtual void ChangeDrawPriority(long);
            // The getters are inline: a module emits them, in reverse order
            // of these declarations, after its own functions.
            virtual int IsReqPopup() const { return m_nFlag & 1; }
            virtual int IsAccessed() const { return (m_nFlag >> 2) & 1; }
            virtual int IsDelete() const { return (m_nFlag >> 3) & 1; }
            virtual int IsInfinityLife() const { return (m_nFlag >> 4) & 1; }
            virtual int IsDisableViewRot() const { return (m_nFlag >> 5) & 1; }
            // Same bit as IsWrapType in the original.
            virtual int IsSleepOuterCourt() const { return (m_nFlag >> 6) & 1; }
            virtual int IsWrapType() const { return (m_nFlag >> 6) & 1; }
            virtual int IsInnerOnly() const { return (m_nFlag >> 8) & 1; }
            virtual int IsNoFlowMove() const { return (m_nFlag >> 7) & 1; }
            virtual int IsNowPopup() const;
            virtual int CanAccess() const { return (m_nFlag >> 1) & 1; }
            virtual int CanReqPopup() const;
            virtual void ReqSleep(float);
            // Called when a character touches the panel; returns 0, 1 (no
            // item) or 2 (item), with the item kind in rItem.
            virtual long DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem,
                                  CItem **ppItem);
            virtual void ReqPopup(float, const bcs::FVEC3 *, CQPanelMan::PANEL_POP_INFO *);
            virtual void AdjustPos();
            virtual void UpdateCount_();
            virtual void UpdatePos_();
            virtual void UpdateVib_();
            virtual void UpdateColor_();
            virtual void DoCmnSendMsg_(long, long);
            // Times in frames.
            virtual float GetLifeTime() const;
            virtual float GetFirstPopupWaitTime() const;
            virtual float GetNextPopupWaitTime() const;

            // (DOL) Marks the panel as a miss ("suka"): DodgeF's managers
            // call it with 1 when a touched panel gives nothing.
            void SetSuka(int);
            // (name guessed) The panel's position at 0xA0.
            const bcs::FVEC3 &GetPos() const { return m_Pos; }

        protected:
            unsigned char m_Unk7C[0x84 - 0x7C]; // 0x7C
            CQPanelMan *m_pMan;                 // 0x84
            long m_nUnk88;                      // 0x88, no item while not 0
            unsigned long m_nFlag;              // 0x8C
            unsigned char m_Unk90[0xA0 - 0x90]; // 0x90
            bcs::FVEC3 m_Pos;                   // 0xA0
            unsigned char m_UnkAC[0xF4 - 0xAC]; // 0xAC
    };
}

#endif /* !MSW_CQPANEL_HPP_ */
