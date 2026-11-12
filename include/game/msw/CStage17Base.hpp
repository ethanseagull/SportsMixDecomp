#ifndef MSW_CSTAGE17BASE_HPP_
#define MSW_CSTAGE17BASE_HPP_

#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "bcs/CScnObj.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageMdlListNode.hpp"
#include "game/msw/CStgSeSlot.hpp"

namespace msw {

    class CStgGmk_Trolley;

    // The gimmicks of stage 17 (turntables, trolley, signals, trains),
    // shared by the game modules' stage managers as a second base class
    // next to the sport's stage manager (DodgeF's msw::CStageDodge17 has it
    // at 0x244). A DOL class, size 0x58, constructor
    // __ct__Q23msw12CStage17BaseFv: own members from 0x00, vptr at 0x54.
    // Defined in game/msw/CStage17Base.cpp.
    //
    // It runs a cycle (Stage17_Main): wait, turn the tables, then either
    // wait again or warn with the signals and send the trolley over the
    // court, and wait until it has gone.
    //
    // The order of the virtual functions follows the CStage17Base part of
    // __vt__Q23msw13CStageDodge17 (DodgeF). The DOL vtable
    // (__vt__Q23msw12CStage17Base) has weak defaults for all the GetParam_*
    // tuning; NewTrolley, GetTurnTableNum and Stage17_IsGimmickReady are
    // pure (zero slots). Field names and the return types not visible in a
    // body are guesses.
    class CStage17Base {
        public:
            class CTurnTable;

            // (name guessed) Where a train's sound plays from: the world
            // position of its model. Two of them at 0x30, 8 bytes each,
            // vptr at 0x00; AddTrainMdl gives each its
            // model node.
            class CTrainPosFunc : public CStgSeSlot::CSESlot::CPosFunc {
                public:
                    CTrainPosFunc() : m_pNode(0) {}
                    virtual ~CTrainPosFunc() {}
                    virtual void GetPos(bcs::FVEC3 *pPos)
                    {
                        bcs::CScnObj *pObj = m_pNode != 0 ? m_pNode->GetObj() : 0;
                        if (pObj != 0) {
                            bcs::FMTX34 mtx;
                            pObj->GetNodeWorldMtx(0, mtx);
                            pPos->Set(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                        }
                    }

                    void SetNode(CStageMdlListNode *pNode) { m_pNode = pNode; }

                private:
                    CStageMdlListNode *m_pNode; // 0x04
            };

            // (name guessed) The sound slots of the stage: 0 and 1 the
            // trains' passing sounds, 2 and 3 their repeating ones, 4 the
            // signal. At 0x48 (vptr at 0x50).
            class CSeSlot : public CStgSeSlot {
                public:
                    virtual long SE_GetSlotNum() const { return SE_SLOT_NUM; }
            };

            enum {
                SIGNAL_MAX = 4,   // signal models
                SE_SLOT_SIGNAL = 4,
                SE_SLOT_NUM = 5
            };

            // The steps of the cycle (Stage17_Main; names guessed).
            enum STATE {
                STATE_WAIT,          // until the wait time is over, then turn
                STATE_TURN,          // until the tables stop
                STATE_SIGNAL,        // the signals warn of the trolley
                STATE_TROLLEY_WAIT,  // until the trolley sets off
                STATE_TROLLEY        // until the trolley has gone
            };

            // The arguments of GetParam_Effect and GetParam_SE (names
            // guessed from where they are played).
            enum {
                EFFECT_TURN_START,
                EFFECT_TURN_STOP
            };
            enum {
                SE_SIGNAL,
                SE_TRAIN,
                SE_TRAIN_LOOP,
                SE_TURN_STOP,
                SE_TURN
            };

        private:
            float m_fTimer;                       // 0x00, frames in the current step
            float m_fTime;                        // 0x04, frames the step lasts
            CTurnTable **m_ppTurnTable;           // 0x08, GetTurnTableNum() tables
            long m_nTurnTableNum;                 // 0x0C
            int m_bSignal;                        // 0x10
            CStageMdlListNode *m_apSignal[SIGNAL_MAX]; // 0x14
            long m_nSignalNum;                    // 0x24
            long m_nState;                        // 0x28, STATE
            CStgGmk_Trolley *m_pTrolley;          // 0x2C
            CTrainPosFunc m_aTrainPosFunc[2];     // 0x30
            long m_nTrainNum;                     // 0x40
            long m_nFixTurnTableTeam;             // 0x44, -1 after construction
            CSeSlot m_SeSlot;                     // 0x48

            // The sound slots, reached through a pointer, so the calls go
            // through the vtable.
            CStgSeSlot *GetSeSlot() { return &m_SeSlot; }

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

            // Tuning, given by the sport's stage, with the DOL's defaults.
            // Times are in frames, angles in radians.
            virtual float GetParam_TTInitialWait() const { return 180.0f; }                  // 0xA0
            virtual float GetParam_TTWait() const { return 300.0f; }                         // 0xA4
            virtual float GetParam_TTWaitRnd() const { return 0.0f; }                        // 0xA8
            virtual float GetParam_TTRadius() const { return 4.0f; }                         // 0xAC
            virtual float GetParam_TTOffset() const { return 0.0f; }                         // 0xB0
            virtual float GetParam_TTRotTime() const { return 300.0f; }                      // 0xB4
            virtual float GetParam_TTRotTimeRnd() const { return 0.0f; }                     // 0xB8
            virtual float GetParam_TTRotAccTime() const { return 60.0f; }                    // 0xBC
            virtual float GetParam_TTRotSpeed() const { return 0.008726646f; }               // 0xC0, 0.5 degrees
            virtual long GetParam_TTRotDirType() const { return 0; }                         // 0xC4
            virtual float GetParam_TTOpenRate() const { return 0.5f; }                       // 0xC8
            virtual float GetParam_TTOpenAngle() const { return 0.0f; }                      // 0xCC
            virtual float GetParam_SignalTime() const { return 60.0f; }                      // 0xD0
            virtual float GetParam_SignalAfterTime() const { return 120.0f; }                // 0xD4
            virtual float GetParam_SignalSEStopTime() const { return 120.0f; }               // 0xD8
            virtual float GetParam_TrolleyPreWait() const { return 60.0f; }                  // 0xDC
            virtual float GetParam_TrolleyRate() const { return 1.0f; }                      // 0xE0
            virtual float GetParam_TrolleyInitRate() const { return 0.0f; }                  // 0xE4
            virtual long GetParam_TrolleyDirType() const { return 2; }                       // 0xE8
            virtual float GetParam_TrolleyLocOffset() const { return 10.0f; }                // 0xEC
            virtual long GetParam_Effect(long type) const { return -1; }                     // 0xF0
            virtual long GetParam_SE(long type) const;                                       // 0xF4
    };

    // One turntable of stage 17. A DOL class, size 0x54: owner at 0x00,
    // own members, vptr at 0x50 (the vtable has only the destructor, which
    // is empty; it follows the constructor in the DOL, so it is not
    // inline). Angles are in radians; names are guesses.
    class CStage17Base::CTurnTable {
        public:
            CTurnTable(CStage17Base *pOwner);
            void Assign(long nIdx, bcs::CScnObj *pObj);

            // Per frame: speeds up, turns, slows down near the target angle
            // and stops on it.
            void Main(float frame);
            // Back to the base angle, standing.
            void Reset();
            // Starts a turn by rot in direction dir (its sign); 0 when it is
            // already turning, 2 when the turn time is not positive, else 1.
            long Start(float rot, float dir);

            // (inline) The distance of rPos from the centre.
            float GetDist(const bcs::FVEC3 &rPos) const;
            // (inline) Whether rPos is on the table.
            bool IsOn(const bcs::FVEC3 &rPos) const;
            // The angle the table turned this frame, without and with the
            // snap onto the target at a stop.
            float GetFlowRot() const { return m_fFlowRot; }
            float GetFrameRot() const { return m_fFrameRot; }
            // (inline) The centre of the table and its radius.
            void GetCenter(bcs::FVEC3 &rPos) const;
            float GetRadius() const;
            // (inline) The angle of the table, in [0, 2 pi].
            float GetRot() const;
            // (inline) Whether the table stands open (a multiple of pi
            // from its base angle, within 10 degrees).
            bool IsOpen() const;
            // (inline) Whether it is turning.
            int IsTurn() const { return m_bTurn; }
            // (inline) Stops at once on the target angle.
            void Stop();

        private:
            void StopSE();
            void UpdateMdl();

            CStage17Base *m_pOwner; // 0x00
            long m_nIdx;            // 0x04, set by Assign; table 1 is the mirror of table 0
            float m_fBaseRot;       // 0x08
            float m_Unk0C;          // 0x0C
            float m_fRot;           // 0x10, m_fBaseRot + m_fTurnRot
            float m_Unk14;          // 0x14
            float m_fTurnRot;       // 0x18
            float m_fTargetRot;     // 0x1C
            float m_fSpeed;         // 0x20
            float m_fMaxSpeed;      // 0x24
            float m_fFlowRot;       // 0x28, turned this frame
            float m_fFrameRot;      // 0x2C, m_fFlowRot + m_fSnapRot
            float m_fSnapRot;       // 0x30, the jump to the target at a stop
            float m_fDir;           // 0x34, 1 or -1
            long m_bTurn;           // 0x38
            long m_bTarget;         // 0x3C, heading for the target angle
            long m_bSlow;           // 0x40, slowing down near the target
            float m_fTurnTime;      // 0x44, frames before heading for the target
            bcs::CScnObj *m_pObj;   // 0x48, set by Assign
            unsigned long m_nSe;    // 0x4C, the turning sound (table 0 only)

        public:
            virtual ~CTurnTable();
    };

    inline CStage17Base::CTurnTable *CStage17Base::NewTurnTable(CStage17Base *pOwner, long heap, long align) const
    {
        return new (heap, align) CTurnTable(pOwner);
    }
}

#endif /* !MSW_CSTAGE17BASE_HPP_ */
