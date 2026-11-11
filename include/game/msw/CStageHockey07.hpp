#ifndef MSW_CSTAGEHOCKEY07_HPP_
#define MSW_CSTAGEHOCKEY07_HPP_

#include "game/msw/CStageManHockey.hpp"

namespace msw {

    class CNpcBossPackun;

    // Hockey stage 07 (HockeyF), the boss Packun stage, made by
    // CStageFactoryHockey. Size 0x228. The boss stands at a fixed place
    // and throws mud bombs; the gimmick hooks cancel the bombs in flight.
    // The same stage is BasketF's CStageBasket07 and DodgeF's
    // CStageDodge07, on the mode's own stage manager.
    class CStageHockey07 : public CStageManHockey {
        public:
            CStageHockey07(bcs::CTask *pParent);
            virtual ~CStageHockey07();

            virtual float GetGoalX1Length(long goal) const;
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual const void *GetPrmTbl() const;
            virtual void CreateGoal();
            virtual void OnSoundEvent(long event);

        private:
            CNpcBossPackun *m_pPackun;   // 0x218, made by CreateGoal
            // 0x21C to 0x224: set to 0 by the constructor and not read here.
            unsigned long m_Unk21C;      // 0x21C
            unsigned long m_Unk220;      // 0x220
            unsigned long m_Unk224;      // 0x224
    };
}

#endif /* !MSW_CSTAGEHOCKEY07_HPP_ */
