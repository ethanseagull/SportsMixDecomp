#ifndef MSW_CSTAGEHOCKEY12_HPP_
#define MSW_CSTAGEHOCKEY12_HPP_

#include "game/msw/CStageManHockey.hpp"

namespace msw {

    class CPinballManHockey;
    class CStgGmk_Pinball;

    // Hockey stage 12, the pinball stage (HockeyF), made by
    // CStageFactoryHockey. Size 0x234: CStageManHockey (0x218), then the
    // pinball manager and the bumper and exit-hole models. A launcher model
    // ("s<n>_wl_meka") shoots balls (CPinballManHockey) across the court;
    // the bumpers ("s<n>_banper_0<k>") and the holes ("s<n>_ana_0<k>")
    // play an animation when a ball hits them (SendMessage 0x25). VolleyF
    // and HockeyF have the same stage (CStageVolley12, CStageHockey12).
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw13CStageHockey12.
    class CStageHockey12 : public CStageManHockey {
        public:
            CStageHockey12(bcs::CTask *pParent);
            virtual ~CStageHockey12();

            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Disp();
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnSceneReentry();
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnEntranceEventGmkStart();
            virtual void OnEntranceEventGmkEnd();
            virtual void OnSoundEvent(long event);

            // Puts every bumper, or every hole, back to its rest animation.
            virtual void BumperReset();
            virtual void ExitReset();

            // The balls of the running stage, which must be this one: how
            // many there can be, and ball idx if it is in play (else null).
            static long GetPinballMaxNum();
            static CStgGmk_Pinball *GetPinball(long idx);

        private:
            enum {
                BUMPER_NUM = 4,
                EXIT_NUM = 2
            };

            CPinballManHockey *m_pPinball;             // 0x218
            CStageMdlListNode *m_apBumper[BUMPER_NUM]; // 0x21C, "banper_01" to "_04"
            CStageMdlListNode *m_apExit[EXIT_NUM];     // 0x22C, "ana_01" and "_02"
    };
}

#endif /* !MSW_CSTAGEHOCKEY12_HPP_ */
