#ifndef MSW_CSTAGEBASKET07_HPP_
#define MSW_CSTAGEBASKET07_HPP_

#include "game/msw/CGoalObjBasket_BossPackun.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    class CNpcBossPackun;

    // Basket stage 07 (BasketF), the boss Packun stage, made by
    // CStageFactoryBasket. Size 0x238. Each of the two goals has its own
    // boss Packun standing at it; the gimmick hooks cancel the mud bombs in
    // flight. HockeyF's CStageHockey07 and DodgeF's CStageDodge07 are the
    // same stage on their mode's own stage manager.
    class CStageBasket07 : public CStageManBasket {
        public:
            CStageBasket07(bcs::CTask *pParent);
            virtual ~CStageBasket07();

            virtual CGoalObj *GetGoal(long goal);
            virtual const CGoalObj *GetGoal(long goal) const;
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                        nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual const void *GetPrmTbl() const;
            virtual void CreateGoal();
            virtual void OnSoundEvent(long event);

        private:
            CNpcBossPackun *m_apNpc[4];              // 0x218, CreateGoal makes the first two
            CGoalObjBasket_BossPackun *m_apGoal[4];  // 0x228, CreateGoal makes the first two
    };
}

#endif /* !MSW_CSTAGEBASKET07_HPP_ */
