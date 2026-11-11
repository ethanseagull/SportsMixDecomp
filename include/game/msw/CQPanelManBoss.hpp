#ifndef MSW_CQPANELMANBOSS_HPP_
#define MSW_CQPANELMANBOSS_HPP_

#include "game/msw/CQPanel.hpp"
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

    // A panel of the boss game, size 0xF4 (nothing added). Built only by
    // CQPanelManBoss::CreatePanel_, which inlines the constructor.
    class CQPanelBoss : public CQPanel {
        public:
            CQPanelBoss(CQPanelMan *pMan, bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos,
                        const bcs::FVEC3 &rRot, CQPanelMan::PANEL_POP_INFO *pInfo, long nIdx)
                : CQPanel(pMan, pGroup, rPos, rRot, pInfo, nIdx)
            {
            }
            virtual ~CQPanelBoss();
            virtual float GetSize() const;
            virtual long DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem,
                                  CItem **ppItem);
            virtual float GetLifeTime() const;
            virtual float GetFirstPopupWaitTime() const;
            virtual float GetNextPopupWaitTime() const;
    };
}

#endif /* !MSW_CQPANELMANBOSS_HPP_ */
