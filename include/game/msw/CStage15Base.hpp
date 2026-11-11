#ifndef MSW_CSTAGE15BASE_HPP_
#define MSW_CSTAGE15BASE_HPP_

#include "bcs/BCS_Color.hpp"
#include "game/msw/CStageRotBase.hpp"

namespace msw {

    class CStgGmk_Case;

    // (purpose guessed from the names) Stage 15, the ship stage: the ship
    // rolls (msw::CStageRotBase), cases (msw::CStgGmk_Case) slide over the
    // deck, rain comes and goes and lightning flashes. A second base class
    // next to the sport's stage manager (DodgeF's msw::CStageDodge15 has it
    // at 0x244). A DOL class, size 0x8C, constructor
    // __ct__Q23msw12CStage15BaseFv: CStageRotBase at 0x00 (vptr at 0x3C), own
    // members from 0x40.
    //
    // The order of the virtual functions follows __vt__Q23msw13CStageDodge15
    // (DodgeF). The modules emit weak copies of the functions with bodies
    // here; the GetParam_* functions and NewCase a module class overrides
    // have no DOL default with a name, so they are declared pure. Field
    // names and the return types not visible in a body are guesses.
    class CStage15Base : public CStageRotBase {
        public:
            CStage15Base();
            virtual ~CStage15Base();

            virtual bool IsRotReady_() const;

            virtual long GetParam_CVID() const { return -1; }             // 0x80
            virtual void Stage15_Main_(float frame);                      // 0x84
            // Adds one of the (up to four) barrel models.
            virtual bool AddBarrelMdlNode(CStageMdlListNode *pNode)        // 0x88
            {
                if (m_nBarrelMdlNum < 4) {
                    m_apBarrelMdl[m_nBarrelMdlNum++] = pNode;
                } else {
                    return false;
                }
                return true;
            }
            virtual void Rainy_Set(int bRainy);                           // 0x8C
            virtual void Rainy_Reset();                                   // 0x90
            virtual void Rainy_Update(float frame);                       // 0x94
            virtual int IsRainy() const { return m_bRainy; }              // 0x98
            virtual void Flash_Start();                                   // 0x9C
            virtual void Flash_Reset();                                   // 0xA0
            virtual void Flash_Update(float frame);                       // 0xA4
            virtual void Stage15_OnSceneEntry();                          // 0xA8
            virtual void Stage15_OnSceneReentry();                        // 0xAC
            virtual void Stage15_OnGimmickReset();                        // 0xB0
            virtual void Stage15_OnGimmickCancel();                       // 0xB4
            virtual void Stage15_OnPeriodInit();                          // 0xB8
            virtual void Stage15_OnEventStart();                          // 0xBC
            virtual void Stage15_OnEventEnd();                            // 0xC0

            // Tuning, given by the sport's stage. Times are in frames.
            virtual long GetParam_CaseNum() const { return 3; }           // 0xC4
            virtual float GetParam_CaseWidth() const = 0;                 // 0xC8
            virtual float GetParam_CaseOffsetZ() const = 0;               // 0xCC
            virtual float GetParam_CaseSpacingZ() const = 0;              // 0xD0
            virtual void GetParam_CaseLocateRect(float *pLeft, float *pRight, float *pFront,
                                                 float *pBack) = 0;       // 0xD4
            virtual long GetParam_CaseBreakDelayRange() const { return 10; } // 0xD8
            virtual bcs::BCS_Color GetParam_FlashColor() const = 0;       // 0xDC
            virtual float GetParam_FlashFrm() const = 0;                  // 0xE0
            virtual float GetParam_FlashGain() const = 0;                 // 0xE4
            virtual float GetParam_FlashDecay() const = 0;                // 0xE8
            virtual float GetParam_FlashNum() const { return 2.0f; }      // 0xEC
            virtual long GetParam_Effect(long type) const { return -1; }  // 0xF0
            virtual float GetParam_RainyInterval() const = 0;             // 0xF4
            virtual float GetParam_RainyTime() const = 0;                 // 0xF8

            // The cases.
            virtual CStgGmk_Case *NewCase() = 0;                          // 0xFC
            virtual CStgGmk_Case *Case_Get(long idx);                     // 0x100
            virtual const CStgGmk_Case *Case_Get(long idx) const;         // 0x104
            virtual void Case_Create();                                   // 0x108
            virtual void Case_DeleteAll();                                // 0x10C
            virtual void Case_RespawnAll();                               // 0x110
            virtual void Case_InactivateAll();                            // 0x114
            virtual void Case_SleepAll();                                 // 0x118
            virtual void Case_Relocate();                                 // 0x11C
            virtual void Case_ForceBreakAll();                            // 0x120
            // Cases are not placed on this team's side (name guessed).
            virtual void Case_SetNoLocateTeam(long team) { m_nNoLocateTeam = team; } // 0x124

        private:
            unsigned long m_Unk40[8];             // 0x40
            CStageMdlListNode *m_apBarrelMdl[4];  // 0x60
            long m_nBarrelMdlNum;                 // 0x70
            unsigned long m_Unk74;                // 0x74, flash state
            int m_bRainy;                         // 0x78
            float m_Unk7C;                        // 0x7C
            unsigned long m_Unk80;                // 0x80
            unsigned long m_Unk84;                // 0x84
            long m_nNoLocateTeam;                 // 0x88
    };
}

#endif /* !MSW_CSTAGE15BASE_HPP_ */
