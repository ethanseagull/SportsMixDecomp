#ifndef MSW_CSTAGEVOLLEY12_HPP_
#define MSW_CSTAGEVOLLEY12_HPP_

#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {

    class CPinballManVolley;
    class CStgGmk_Pinball;

    // Volley stage 12, the pinball stage, made by
    // CStageFactoryVolley. Size 0x228: CStageManVolley (0x200), then the
    // pinball manager and the bumper and exit-hole models. A launcher model
    // ("s<n>_wl_meka") shoots balls (CPinballManVolley) across the court;
    // the bumpers ("s<n>_banper_0<k>") and the holes ("s<n>_ana_0<k>")
    // play an animation when a ball hits them (SendMessage 0x25). HockeyF
    // and DodgeF have the same stage (CStageHockey12, CStageDodge12).
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw14CStageVolley12.
    class CStageVolley12 : public CStageManVolley {
        public:
            CStageVolley12(bcs::CTask *pParent);
            virtual ~CStageVolley12();

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

            CPinballManVolley *m_pPinball;             // 0x200
            CStageMdlListNode *m_apBumper[BUMPER_NUM]; // 0x204, "banper_01" to "_04"
            CStageMdlListNode *m_apExit[EXIT_NUM];     // 0x214, "ana_01" and "_02"
            // Only in VolleyF's version of the stage; not used by this
            // unit (meaning not known).
            CBodyCollision m_Unk21C;                   // 0x21C
    };
}

#endif /* !MSW_CSTAGEVOLLEY12_HPP_ */
