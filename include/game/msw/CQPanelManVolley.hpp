#ifndef MSW_CQPANELMANVOLLEY_HPP_
#define MSW_CQPANELMANVOLLEY_HPP_

#include "game/msw/CQPanelMan.hpp"

namespace msw {

    // The "?" panels of the volley game (BossF game/volley/CQPanelManVolley.cpp).
    // Size 0xC24 (msw::CGimmickManVolley::Initialize_ allocates that much).
    //
    // INCOMPLETE: the members are not mapped; the split of the size between
    // this class and CQPanelMan is a guess.
    class CQPanelManVolley : public CQPanelMan {
        public:
            CQPanelManVolley(bcs::CTask *pParent);
            virtual ~CQPanelManVolley();
            virtual void SetUp();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long) const;
            virtual const POPUP_GROUP *GetPopupGroupData_(long) const;

        private:
            unsigned char m_Unk9A8[0xC24 - 0x9A8]; // 0x9A8
    };
}

#endif /* !MSW_CQPANELMANVOLLEY_HPP_ */
