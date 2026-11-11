#ifndef MSW_CQPANELMANVOLLEY_HPP_
#define MSW_CQPANELMANVOLLEY_HPP_

#include "game/msw/CQPanel.hpp"
#include "game/msw/CQPanelMan.hpp"

namespace msw {

    // The "?" panels of the volley game (VolleyF game/volley/CQPanelVolley.cpp).
    // Size 0xC24 (msw::CGimmickManVolley::Initialize_ allocates that much).
    //
    // INCOMPLETE: the members are not mapped; the split of the size between
    // this class and CQPanelMan is a guess.
    class CQPanelManVolley : public CQPanelMan {
        public:
            CQPanelManVolley(bcs::CTask *pParent);
            virtual ~CQPanelManVolley();
            virtual void SetUp();
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long nTeam) const;
            virtual const POPUP_GROUP *GetPopupGroupData_(long nStage) const;

        private:
            unsigned char m_Unk9A8[0xC24 - 0x9A8]; // 0x9A8
    };

    // A panel of the volley game, size 0xF4 (nothing added). Built only by
    // CQPanelManVolley::CreatePanel_, which inlines the constructor.
    class CQPanelVolley : public CQPanel {
        public:
            CQPanelVolley(CQPanelMan *pMan, bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos,
                          const bcs::FVEC3 &rRot, CQPanelMan::PANEL_POP_INFO *pInfo, long nIdx)
                : CQPanel(pMan, pGroup, rPos, rRot, pInfo, nIdx)
            {
            }
            virtual ~CQPanelVolley();
            virtual float GetSize() const;
            virtual long DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem,
                                  CItem **ppItem);
            virtual float GetLifeTime() const;
            virtual float GetFirstPopupWaitTime() const;
            virtual float GetNextPopupWaitTime() const;
    };
}

#endif /* !MSW_CQPANELMANVOLLEY_HPP_ */
