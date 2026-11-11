#ifndef MSW_CQPANELMANDODGE_HPP_
#define MSW_CQPANELMANDODGE_HPP_

#include "game/msw/CQPanelMan.hpp"

namespace msw {

    // The "?" panels of the dodge game (DodgeF game/dodge/CQPanelManDodge.cpp).
    // Size 0xC24 (msw::CGimmickManDodge::Initialize_ allocates that much).
    //
    // INCOMPLETE: the members are not mapped; the split of the size between
    // this class and CQPanelMan is a guess.
    class CQPanelManDodge : public CQPanelMan {
        public:
            CQPanelManDodge(bcs::CTask *pParent);
            virtual ~CQPanelManDodge();
            virtual void SetUp();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long) const;
            virtual const void *GetPopupGroupData_(long) const;

        private:
            unsigned char m_Unk9A8[0xC24 - 0x9A8]; // 0x9A8
    };

    // The "?" panels of the dodgeball party game ("Dodge or Die"). Same size
    // (msw::CGimmickManDodgeDodgeOrDie::Initialize_ allocates 0xC24); only
    // the panel check differs.
    class CQPanelManDodgeDodgeOrDie : public CQPanelManDodge {
        public:
            CQPanelManDodgeDodgeOrDie(bcs::CTask *pParent);
            virtual ~CQPanelManDodgeDodgeOrDie();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
    };
}

#endif /* !MSW_CQPANELMANDODGE_HPP_ */
