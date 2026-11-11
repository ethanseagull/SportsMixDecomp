#ifndef MSW_CQPANELMANHOCKEY_HPP_
#define MSW_CQPANELMANHOCKEY_HPP_

#include "game/msw/CGame.hpp"
#include "game/msw/CQPanel.hpp"
#include "game/msw/CQPanelMan.hpp"

namespace msw {

    // The "?" panels of the hockey game (HockeyF game/hockey/CQPanelHockey.cpp).
    // Size 0xC24 (msw::CGimmickManHockey::Initialize_ allocates that much).
    //
    // INCOMPLETE: the members are not mapped; the split of the size between
    // this class and CQPanelMan is a guess.
    class CQPanelManHockey : public CQPanelMan {
        public:
            CQPanelManHockey(bcs::CTask *pParent);
            virtual ~CQPanelManHockey();
            virtual void SetUp();
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long) const;
            virtual const POPUP_GROUP *GetPopupGroupData_(long) const;

        private:
            unsigned char m_Unk9A8[0xC24 - 0x9A8]; // 0x9A8
    };

    // A panel of the hockey game, size 0xF8. Built only by
    // CQPanelManHockey::CreatePanel_, which inlines the constructor.
    class CQPanelHockey : public CQPanel {
        public:
            // The panels are bigger on stage 10.
            CQPanelHockey(CQPanelMan *pMan, bcs::CScnGroup *pGroup, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot,
                          CQPanelMan::PANEL_POP_INFO *pInfo, long nIdx)
                : CQPanel(pMan, pGroup, rPos, rRot, pInfo, nIdx)
            {
                m_fSizeScale = 1.0f;
                if (CGame::g_GameWork.GetStage() == 10) {
                    m_fSizeScale = 1.2385321f;
                }
            }
            virtual ~CQPanelHockey();
            virtual float GetSize() const;
            virtual long DoAccess(const bcs::FVEC3 &rPos, long nChr, unsigned long nFlag, long &rItem,
                                  CItem **ppItem);
            virtual float GetLifeTime() const;
            virtual float GetFirstPopupWaitTime() const;
            virtual float GetNextPopupWaitTime() const;

        private:
            float m_fSizeScale; // 0xF4 (name guessed)
    };
}

#endif /* !MSW_CQPANELMANHOCKEY_HPP_ */
