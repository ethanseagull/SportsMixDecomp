#ifndef MSW_CQPANELMAN_HPP_
#define MSW_CQPANELMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CScnGroup;
}

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

            // One row of item popup rates, one weight per item kind (read
            // from the tables of BasketF game/basket/CQPanelBasket.cpp).
            struct ITEM_POPUP_RATE_TBL {
                long anRate[16];
            };

            // The rows of one rate table, picked by ChoiceItemPopRateTblIdx_
            // (0-4). Size 0x140.
            struct ITEM_POPUP_RATE_TBL_TBL {
                ITEM_POPUP_RATE_TBL aTbl[5];
            };

            // One panel place of a popup group, 0x14 bytes. Field use from
            // the DOL's group copy (fn_8012F700) and ChoicePanelInfo_: bit 0
            // of the flags enables the place, bit 1 is tested too; the panel
            // stands at (x, 0, z). The word at 0x10 is not copied; the two
            // bytes after it are.
            struct POPUP_PANEL {
                long nFlag;             // 0x00
                float fX;               // 0x04
                float fZ;               // 0x08
                float fRotY;            // 0x0C, (guess) degrees: 0, 90 or 270 in BasketF
                short nUnk10;           // 0x10
                unsigned char nUnk12;   // 0x12
                unsigned char nUnk13;   // 0x13
            };

            // A group of panel places, 0x184 bytes; GetPopupGroupData_
            // returns an array of six. nNum is indexed by the level at 0x44.
            struct POPUP_GROUP {
                long nUnk00;            // 0x00
                long nUnk04;            // 0x04
                long anNum[5];          // 0x08
                POPUP_PANEL aPanel[18]; // 0x1C
            };

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
            virtual int IsItemCreate() const { return 0; }
            virtual long GetPopupItemTypeNum(long);
            virtual long GetSubstituteItem(long);
            virtual void WakeupNext(CQPanel *, PANEL_POP_INFO *);
            virtual void AdjustPos_(CQPanel *, PANEL_POP_INFO *, bcs::FVEC3 &);
            virtual long QPanelCheck_(const bcs::FVEC3 &, long, unsigned long, long *, CItem **);
            virtual CQPanel *CreatePanel_(const bcs::FVEC3 &, const bcs::FVEC3 &, PANEL_POP_INFO *, long);
            virtual void SetupItemPopRate_();
            virtual long ChoiceItemPopRateTblIdx_(long) const;
            virtual const POPUP_GROUP *GetPopupGroupData_(long) const;
            virtual PANEL_POP_INFO *ChoicePanelInfo_(long);

            // (name guessed) The level at 0x44.
            long GetLevel() const { return m_nLevel; }

        protected:
            // In the DOL.
            void SetItemPopRate_(long nIdx, const ITEM_POPUP_RATE_TBL_TBL &rTbl);
            // In the DOL. The panel touched at rPos, or null; DodgeF passes
            // 1 when bit 1 of the check flags is clear (meaning not known).
            CQPanel *QPanelHitCheck_(const bcs::FVEC3 &rPos, long);

            unsigned char m_Unk34[0x44 - 0x34]; // 0x34
            // (name guessed) GameWork's word at 0x14 clamped to 0-4, forced
            // to 0 or 4 by two GameWork flags (DOL constructor). The panel
            // timings and group sizes are chosen by it.
            long m_nLevel;                      // 0x44
            bcs::CScnGroup *m_pScnGroup;        // 0x48, made by the constructor
            unsigned char m_Unk4C[0x9A8 - 0x4C]; // 0x4C
    };
}

#endif /* !MSW_CQPANELMAN_HPP_ */
