#ifndef MSW_CQPANELMANBASKET_HPP_
#define MSW_CQPANELMANBASKET_HPP_

#include "game/msw/CQPanel.hpp"
#include "game/msw/CQPanelMan.hpp"

namespace msw {

    // The "?" panels of the basketball game (BasketF
    // game/basket/CQPanelBasket.cpp). Size 0xC24
    // (msw::CGimmickManBasket::Initialize_ allocates that much), like the
    // other modules' panel managers; the members past CQPanelMan are not
    // mapped.
    class CQPanelManBasket : public CQPanelMan {
        public:
            CQPanelManBasket(bcs::CTask *pParent);
            virtual ~CQPanelManBasket();
            virtual void SetUp();
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long nTeam) const;
            virtual const POPUP_GROUP *GetPopupGroupData_(long nStage) const;

        private:
            unsigned char m_Unk9A8[0xC24 - 0x9A8]; // 0x9A8
    };

    // A panel of the basketball game, size 0xF4 (nothing added). Built only
    // by CQPanelManBasket::CreatePanel_, which inlines the constructor.
    class CQPanelBasket : public CQPanel {
        public:
            CQPanelBasket(CQPanelMan *pMan, bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos,
                          const bcs::FVEC3 &rRot, CQPanelMan::PANEL_POP_INFO *pInfo, long nIdx)
                : CQPanel(pMan, pGroup, rPos, rRot, pInfo, nIdx)
            {
            }
            virtual ~CQPanelBasket();
            virtual float GetSize() const;
            virtual long DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem,
                                  CItem **ppItem);
            virtual float GetLifeTime() const;
            virtual float GetFirstPopupWaitTime() const;
            virtual float GetNextPopupWaitTime() const;
    };
}

#endif /* !MSW_CQPANELMANBASKET_HPP_ */
