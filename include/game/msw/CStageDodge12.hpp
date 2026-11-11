#ifndef MSW_CSTAGEDODGE12_HPP_
#define MSW_CSTAGEDODGE12_HPP_

#include "game/msw/CStageManDodge.hpp"

namespace msw {

    class CPinballManDodge;
    class CStgGmk_Pinball;

    // Dodge stage 12, the pinball stage (DodgeF), made by
    // CStageFactoryDodge. Size 0x260: CStageManDodge (0x244), then the
    // pinball manager and the bumper and exit-hole models. A launcher model
    // ("s<n>_wl_meka") shoots balls (CPinballManDodge) across the court;
    // the bumpers ("s<n>_banper_0<k>") and the holes ("s<n>_ana_0<k>")
    // play an animation when a ball hits them (SendMessage 0x25). VolleyF
    // and HockeyF have the same stage (CStageVolley12, CStageHockey12).
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw13CStageDodge12.
    class CStageDodge12 : public CStageManDodge {
        public:
            CStageDodge12(bcs::CTask *pParent);
            virtual ~CStageDodge12();

            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
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

            CPinballManDodge *m_pPinball;             // 0x244
            CStageMdlListNode *m_apBumper[BUMPER_NUM]; // 0x248, "banper_01" to "_04"
            CStageMdlListNode *m_apExit[EXIT_NUM];     // 0x258, "ana_01" and "_02"
    };
}

#endif /* !MSW_CSTAGEDODGE12_HPP_ */
