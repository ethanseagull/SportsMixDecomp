#ifndef MSW_CSTAGE17BASE_HPP_
#define MSW_CSTAGE17BASE_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStageMan.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    class CStageMdlListNode;
    class CStgGmk_Trolley;

    // The gimmicks of stage 17 (turntables, trolley, signals, trains),
    // shared by the game modules' stage managers as a second base class
    // next to the sport's stage manager (DodgeF's msw::CStageDodge17 has it
    // at 0x244). A DOL class, size 0x58, constructor
    // __ct__Q23msw12CStage17BaseFv: own members from 0x00, vptr at 0x54.
    //
    // The order of the virtual functions follows the CStage17Base part of
    // __vt__Q23msw13CStageDodge17 (DodgeF). The modules emit weak copies of
    // the functions with bodies here; the ones a module class overrides with
    // no DOL default (NewTrolley, GetTurnTableNum, Stage17_IsGimmickReady
    // and the GetParam_* tuning) are declared pure. Field names and the
    // return types not visible in a body are guesses.
    class CStage17Base {
        public:
            class CTurnTable;

        private:
            float m_Unk00;                       // 0x00
            float m_Unk04;                       // 0x04
            CTurnTable *m_apTurnTable[2];        // 0x08
            int m_bSignal;                       // 0x10
            unsigned long m_Unk14;               // 0x14
            unsigned long m_Unk18;               // 0x18
            unsigned long m_Unk1C;               // 0x1C
            unsigned long m_Unk20;               // 0x20
            unsigned long m_Unk24;               // 0x24, signal model
            unsigned long m_Unk28;               // 0x28
            CStgGmk_Trolley *m_pTrolley;         // 0x2C
            unsigned char m_Unk30[0x10];         // 0x30, two 8-byte objects with a vptr
            unsigned long m_Unk40;               // 0x40
            long m_nFixTurnTableTeam;            // 0x44, -1 after construction
            unsigned char m_Unk48[0xC];          // 0x48, a CStgSeSlot-derived member

        public:
            CStage17Base();
            virtual ~CStage17Base();                                                         // 0x08

            virtual long Stage17_GetFloorAttribute(const bcs::FVEC3 &rPos,
                                                   CStageMan::FLOW_TYPE type) const;        // 0x0C
            virtual void Stage17_GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                              CStageMan::FLOW_TYPE type) const;             // 0x10
            virtual float Stage17_GetFloorFlowRot(const bcs::FVEC3 &rPos,
                                                  CStageMan::FLOW_TYPE type) const;         // 0x14
            virtual void Stage17_CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos,
                                                  CStageMan::FLOW_TYPE type) const;         // 0x18
            virtual float Stage17_CalcFloorFlowRot(const bcs::FVEC3 &rPos,
                                                   CStageMan::FLOW_TYPE type) const;        // 0x1C
            // The turntables do not turn towards this team (-1: none).
            virtual void SetFixTurnTableTeam(long team) { m_nFixTurnTableTeam = team; }       // 0x20
            virtual void Stage17_Create();                                                   // 0x24
            virtual void Stage17_Delete();                                                   // 0x28
            virtual void Stage17_Main(float frame);                                          // 0x2C
            virtual void Stage17_Reset();                                                    // 0x30

            // The trolley.
            virtual CStgGmk_Trolley *NewTrolley() = 0;                                       // 0x34
            virtual bool IsTrolleyActive() const;                                            // 0x38
            virtual CStgGmk_Trolley *GetTrolley_() { return m_pTrolley; }                    // 0x3C
            virtual const CStgGmk_Trolley *GetTrolley_() const { return m_pTrolley; }        // 0x40
            virtual void TrolleyActivate();                                                  // 0x44
            virtual void TrolleyReset();                                                     // 0x48

            // The turntables.
            virtual long GetTurnTableNum() const = 0;                                        // 0x4C
            virtual CTurnTable *NewTurnTable(CStage17Base *pOwner, long heap, long align) const; // 0x50
            virtual CTurnTable *GetTurnTable(long idx);                                      // 0x54
            virtual const CTurnTable *GetTurnTable(long idx) const;                          // 0x58
            virtual bool IsTurnTableOpen() const;                                            // 0x5C

            // The signals and trains.
            virtual void AddSignalMdl(CStageMdlListNode *pNode);                             // 0x60
            virtual void SignalStart();                                                      // 0x64
            virtual void SignalStop();                                                       // 0x68
            virtual void SignalReset();                                                      // 0x6C
            virtual int IsSignal() const { return m_bSignal; }                               // 0x70
            virtual void AddTrainMdl(CStageMdlListNode *pNode);                              // 0x74
            virtual void ResetTrainAll();                                                    // 0x78

            // Hooks, called by the sport's stage manager.
            virtual bool Stage17_IsGimmickReady() const = 0;                                 // 0x7C
            virtual void Stage17_OnGimmickReset();                                           // 0x80
            virtual void Stage17_OnGimmickStart();                                           // 0x84
            virtual void Stage17_OnGimmickStop();                                            // 0x88
            virtual void Stage17_OnGimmickCancel();                                          // 0x8C
            virtual void Stage17_OnGimmickPause();                                           // 0x90
            virtual void Stage17_OnGimmickResume();                                          // 0x94
            virtual void Stage17_OnSceneEntry();                                             // 0x98
            virtual void Stage17_OnTurnInit();                                               // 0x9C

            // Tuning, given by the sport's stage. Times are in frames.
            virtual float GetParam_TTInitialWait() const = 0;                                // 0xA0
            virtual float GetParam_TTWait() const = 0;                                       // 0xA4
            virtual float GetParam_TTWaitRnd() const = 0;                                    // 0xA8
            virtual float GetParam_TTRadius() const = 0;                                     // 0xAC
            virtual float GetParam_TTOffset() const = 0;                                     // 0xB0
            virtual float GetParam_TTRotTime() const = 0;                                    // 0xB4
            virtual float GetParam_TTRotTimeRnd() const = 0;                                 // 0xB8
            virtual float GetParam_TTRotAccTime() const = 0;                                 // 0xBC
            virtual float GetParam_TTRotSpeed() const = 0;                                   // 0xC0
            virtual long GetParam_TTRotDirType() const = 0;                                  // 0xC4
            virtual float GetParam_TTOpenRate() const = 0;                                   // 0xC8
            virtual float GetParam_TTOpenAngle() const = 0;                                  // 0xCC
            virtual float GetParam_SignalTime() const = 0;                                   // 0xD0
            virtual float GetParam_SignalAfterTime() const = 0;                              // 0xD4
            virtual float GetParam_SignalSEStopTime() const = 0;                             // 0xD8
            virtual float GetParam_TrolleyPreWait() const = 0;                               // 0xDC
            virtual float GetParam_TrolleyRate() const = 0;                                  // 0xE0
            virtual float GetParam_TrolleyInitRate() const = 0;                              // 0xE4
            virtual long GetParam_TrolleyDirType() const = 0;                                // 0xE8
            virtual float GetParam_TrolleyLocOffset() const = 0;                             // 0xEC
            virtual long GetParam_Effect(long type) const = 0;                               // 0xF0
            virtual long GetParam_SE(long type) const;                                       // 0xF4
    };

    // One turntable of stage 17. A DOL class, size 0x54: owner at 0x00,
    // own members, vptr at 0x50 (the vtable has only the destructor, which
    // is empty and inline). Members read from the constructor and Assign;
    // names are guesses.
    class CStage17Base::CTurnTable {
        public:
            CTurnTable(CStage17Base *pOwner);
            void Assign(long nIdx, bcs::CScnObj *pObj);

        private:
            CStage17Base *m_pOwner; // 0x00
            long m_nIdx;            // 0x04, set by Assign
            float m_Unk08;          // 0x08
            float m_Unk0C;          // 0x0C
            float m_Unk10;          // 0x10
            float m_Unk14;          // 0x14
            float m_Unk18;          // 0x18
            float m_Unk1C;          // 0x1C
            float m_Unk20;          // 0x20
            float m_Unk24;          // 0x24
            float m_Unk28;          // 0x28
            float m_Unk2C;          // 0x2C
            float m_Unk30;          // 0x30
            unsigned long m_Unk34;  // 0x34
            long m_Unk38;           // 0x38
            unsigned long m_Unk3C;  // 0x3C
            unsigned long m_Unk40;  // 0x40
            float m_Unk44;          // 0x44
            bcs::CScnObj *m_pObj;   // 0x48, set by Assign
            long m_Unk4C;           // 0x4C

        public:
            virtual ~CTurnTable() {}
    };

    inline CStage17Base::CTurnTable *CStage17Base::NewTurnTable(CStage17Base *pOwner, long heap, long align) const
    {
        return new (heap, align) CTurnTable(pOwner);
    }
}

#endif /* !MSW_CSTAGE17BASE_HPP_ */
