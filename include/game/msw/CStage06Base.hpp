#ifndef MSW_CSTAGE06BASE_HPP_
#define MSW_CSTAGE06BASE_HPP_

#include "bcs/BCS_Color.hpp"

namespace bcs {
    class CRegex;
    class CScnObj;
}

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace msw {

    class CArn;
    class CStageMan;
    class CStageMdlListNode;

    namespace STAGE {
        struct MDL_GEN_RULE;
    }

    // (purpose guessed from the names) The lights of a sport's stage 06: a
    // second base class next to the sport's stage manager (BasketF's
    // msw::CStageBasket06 has it at 0x218). A DOL class, size 0x30: data
    // first, vptr at 0x2C, like CStage01Base. The sport's stage hands its
    // hooks on (Stage06_*) and gives the tuning through the GetParam_*
    // functions, which it overrides. Field names and types are guesses from
    // the DOL's accesses.
    class CStage06Base {
        public:
            CStage06Base(CStageMan *pStage);

        private:
            CStageMan *m_pStage;      // 0x00
            long m_Unk04;             // 0x04
            float m_fUnk08;           // 0x08
            float m_fUnk0C;           // 0x0C
            long m_nIllumiState;      // 0x10
            float m_fUnk14;           // 0x14
            float m_fUnk18;           // 0x18
            float m_fUnk1C;           // 0x1C
            long m_Unk20;             // 0x20
            bcs::CScnObj *m_pUnkObj;  // 0x24, given to AppendDelayDeleteObject by the destructor
            long m_Unk28;             // 0x28, -1 at first

        public:
            virtual ~CStage06Base();                          // 0x08
            virtual void Illumi_On(long goal, float time);    // 0x0C
            virtual void Illumi_Inc(long goal, float time);   // 0x10
            virtual void Illumi_Off(long goal);               // 0x14
            virtual int Illumi_IsOn(long goal) const;         // 0x18
            virtual void Illumi_StateFix(long goal);          // 0x1C
            virtual void Stage06_Main_(float frame);          // 0x20
            virtual void Illumi_Init();                       // 0x24
            virtual void Illumi_Reset(long goal, int);        // 0x28
            virtual void Illumi_SetRateAll();                 // 0x2C
            virtual void Illumi_SetAnimAll(long);             // 0x30
            virtual void Illumi_Update(float frame);          // 0x34
            virtual void Illumi_FogCheck(int);                // 0x38
            virtual void Stage06_OnPostInitialize();          // 0x3C
            virtual void Stage06_OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                  CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn,
                                                  long n);    // 0x40
            virtual void Stage06_OnPeriodInit();              // 0x44
            virtual void Stage06_OnGimmickReset();            // 0x48
            virtual void Stage06_OnEventStart();              // 0x4C

            // Tuning, given by the sport's stage.
            virtual long GetParam_SeSlot_Illumi() const = 0;       // 0x50
            virtual bcs::BCS_Color GetParam_IllumiOnColor() const = 0;  // 0x54
            virtual bcs::BCS_Color GetParam_IllumiOffColor() const = 0; // 0x58
            virtual long GetParam_IllumiOnGoalCnt() const = 0;     // 0x5C
            virtual float GetParam_IllumiOnTime() const = 0;       // 0x60, frames
            virtual bcs::BCS_Color GetParam_FlashColor() const = 0;     // 0x64
            virtual float GetParam_FlashTime() const = 0;          // 0x68
            virtual float GetParam_FlashGain() const = 0;          // 0x6C
            virtual float GetParam_FlashDecay() const = 0;         // 0x70
            // Emitted weak by the modules (BasketF CStageBasket06).
            virtual float GetParam_FlashNum() const { return 2.0f; } // 0x74
    };
}

#endif /* !MSW_CSTAGE06BASE_HPP_ */
