#ifndef MSW_CSTAGEDODGE07_HPP_
#define MSW_CSTAGEDODGE07_HPP_

#include "game/msw/CStageManDodge.hpp"

namespace msw {

    class CStgGmk_BossPackun_Dodge;

    // Dodge stage 07 (DodgeF), the boss Packun stage, made by
    // CStageFactoryDodge. Size 0x248: CStageManDodge (0x244), then the boss
    // at 0x244. The boss is a stage gimmick here; it throws mud bombs, which
    // the gimmick hooks cancel. HockeyF's CStageHockey07 and BasketF's
    // CStageBasket07 are the same stage in the other modes.
    class CStageDodge07 : public CStageManDodge {
        public:
            CStageDodge07(bcs::CTask *pParent);
            virtual ~CStageDodge07();

            virtual void Disp();
            virtual bool GetPassToPos(bcs::FVEC3 *pPos, long nChr, long type) const;
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostInitialize();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // The boss of the running stage, which must be this one.
            static CStgGmk_BossPackun_Dodge *GetBossPackun();

        private:
            CStgGmk_BossPackun_Dodge *m_pBoss; // 0x244, made by OnPostInitialize
    };
}

#endif /* !MSW_CSTAGEDODGE07_HPP_ */
