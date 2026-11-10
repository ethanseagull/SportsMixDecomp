#ifndef MSW_CQPANELMAN_HPP_
#define MSW_CQPANELMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CItem;
    class CQPanel;

    // The "?" panels of a game. A DOL class; each game module derives its own
    // (CQPanelManBoss, ...). The virtual functions are in the order of the
    // slots of __vt__Q23msw14CQPanelManBoss (BossF). Return types are
    // guessed unless marked.
    //
    // INCOMPLETE: layout.py also finds a bcs::CScnGroup base (constructed
    // after bcs::CTask) that is not declared here; the members are not
    // mapped. The constructor writes up to 0x9A4.
    class CQPanelMan : public bcs::CUnivTask {
        public:
            struct ITEM_POP_SET_ARG;
            struct PANEL_POP_INFO;

            CQPanelMan(bcs::CTask *pParent, long);
            virtual ~CQPanelMan();
            virtual void Main();
            virtual void AllReset();
            virtual void SetUp();
            virtual long SendMessage(long, long, long, long);
            virtual long QPanelCheck(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
            virtual long ChoicePopupItem(long, int);
            // (return type from msw::CGimmickManBoss::QPanelChoicePopupCoin_,
            // which returns it and -1 without a manager)
            virtual long ChoicePopupCoin(long);
            virtual int IsItemCreate() const;
            virtual long GetPopupItemTypeNum(long);
            virtual long GetSubstituteItem(long);
            virtual void WakeupNext(CQPanel *, PANEL_POP_INFO *);
            virtual void AdjustPos_(CQPanel *, PANEL_POP_INFO *, bcs::FVEC3 &);
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long) const;
            virtual const void *GetPopupGroupData_(long) const;
            virtual PANEL_POP_INFO *ChoicePanelInfo_(long);

        private:
            unsigned char m_Unk34[0x9A8 - 0x34]; // 0x34
    };
}

#endif /* !MSW_CQPANELMAN_HPP_ */
