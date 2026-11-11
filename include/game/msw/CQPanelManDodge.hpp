#ifndef MSW_CQPANELMANDODGE_HPP_
#define MSW_CQPANELMANDODGE_HPP_

#include "game/msw/CQPanel.hpp"
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
            virtual const POPUP_GROUP *GetPopupGroupData_(long) const;

        private:
            unsigned char m_Unk9A8[0xC24 - 0x9A8]; // 0x9A8
    };

    // A panel of the dodgeball game, size 0xF4 (nothing added). Built only
    // by CQPanelManDodge::CreatePanel_, which inlines the constructor.
    class CQPanelDodge : public CQPanel {
        public:
            CQPanelDodge(CQPanelMan *pMan, bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos,
                         const bcs::FVEC3 &rRot, CQPanelMan::PANEL_POP_INFO *pInfo, long nIdx)
                : CQPanel(pMan, pGroup, rPos, rRot, pInfo, nIdx)
            {
            }
            virtual ~CQPanelDodge();
            virtual float GetSize() const;
            virtual long DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem,
                                  CItem **ppItem);
            virtual float GetLifeTime() const;
            virtual float GetFirstPopupWaitTime() const;
            virtual float GetNextPopupWaitTime() const;
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
