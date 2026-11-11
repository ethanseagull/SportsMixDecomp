#ifndef MSW_CQPANELMANBOSS_HPP_
#define MSW_CQPANELMANBOSS_HPP_

#include "game/msw/CQPanelMan.hpp"

namespace msw {

    // The "?" panels of the boss game (BossF game/boss/CQPanelManBoss.cpp).
    // Size 0xC24 (msw::CGimmickManBoss::Initialize_ allocates that much).
    //
    // INCOMPLETE: the members are not mapped; the split of the size between
    // this class and CQPanelMan is a guess.
    class CQPanelManBoss : public CQPanelMan {
        public:
            CQPanelManBoss(bcs::CTask *pParent);
            virtual ~CQPanelManBoss();
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

#endif /* !MSW_CQPANELMANBOSS_HPP_ */
