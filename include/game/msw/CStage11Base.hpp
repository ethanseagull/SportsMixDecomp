#ifndef MSW_CSTAGE11BASE_HPP_
#define MSW_CSTAGE11BASE_HPP_

#include "bcs/BCS_Rand.hpp"
#include "bcs/CPool.hpp"
#include "bcs/CTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStageMan.hpp"

namespace msw {

    class CStageMdlListNode;

    namespace STAGE11 {

        // (purpose guessed) A task that plays a stage model node's
        // animation at random intervals (the fire pillars of stage 11). A
        // DOL class, size 0x50, constructor
        // __ct__Q33msw7STAGE1113CStgRndAnmObjFPQ23bcs5CTaskPQ23msw17CStageMdlListNodellff:
        // the node, two numbers (DodgeF passes a voice id, 0x35 or 0x36, and
        // the sound 0xD62) and the interval range in frames. Only what the
        // modules use is declared.
        class CStgRndAnmObj : public bcs::CTask {
            public:
                CStgRndAnmObj(bcs::CTask *pParent, CStageMdlListNode *pNode, long n0, long n1, float fMin,
                              float fMax);

                virtual void Main();
                // 0x18 (name guessed) Starts over; CBgFirePillarCluster
                // calls it for every pillar when the scene is re-entered.
                virtual void Reset();

            private:
                CStageMdlListNode *m_pNode; // 0x34
                long m_Unk38;               // 0x38
                long m_Unk3C;               // 0x3C
                long m_Unk40;               // 0x40
                float m_Unk44;              // 0x44
                float m_fMin;               // 0x48
                float m_fMax;               // 0x4C
        };

        // (purpose guessed from the names) The fire pillars of stage 11,
        // size 0x3B8: a fixed pool of 32 task ids (the pillars, made by the
        // sport's stage and handed over with PushBack) and a pool of
        // CStgRndAnmObj nodes at 0x35C that nothing here fills. Every
        // module emits all of it weak (DodgeF CStageDodge11).
        class CBgFirePillarCluster : public bcs::CFixedPool<unsigned long, 32, bcs::CPoolNode<unsigned long, 0> > {
            private:
                typedef bcs::CPoolNode<unsigned long, 0> CNode;

            public:
                virtual ~CBgFirePillarCluster() {}

                // Keeps the task id of pObj.
                //
                // The original tests the address of Handle::IsValid instead
                // of calling it (__ptmf_test of the pointer-to-member
                // constant {0, -1, IsValid} in .data, which is also why
                // IsValid is emitted out of line). That address is never
                // null, so the id is stored even when no node was free
                // (through a null pointer). Kept as in the original.
                virtual void PushBack(STAGE11::CStgRndAnmObj *pObj)
                {
                    if (pObj != 0) {
                        Handle handle = Create();
                        if (&Handle::IsValid) {
                            *handle.Get() = pObj->GetId();
                        }
                    }
                }

                // Starts every pillar over.
                virtual void ResetAll()
                {
                    for (bcs::CListNodeBase *pIt = m_UsedList.CListBase::GetFirst(); pIt != m_UsedList.GetEnd();
                         bcs::CListNodeBase::Step(pIt)) {
                        CStgRndAnmObj *pObj =
                            static_cast<CStgRndAnmObj *>(bcs::CTaskMan::GetTask(static_cast<CNode *>(pIt)->m_Data));
                        if (pObj != 0) {
                            pObj->Reset();
                        }
                    }
                }

                // Forgets the pillars whose task is gone or being deleted.
                virtual void CleanUp()
                {
                    bcs::CListNodeBase *pIt = m_UsedList.CListBase::GetFirst();
                    while (pIt != m_UsedList.GetEnd()) {
                        bcs::CListNodeBase *pNext = pIt;
                        bcs::CListNodeBase::Step(pNext);

                        CNode *pNode = static_cast<CNode *>(pIt);
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(pNode->m_Data);
                        if (pTask == 0 || pTask->IsDelete()) {
                            Release(pNode->GetHandleId());
                        }
                        pIt = pNext;
                    }
                }

            private:
                bcs::CPool<CStgRndAnmObj, bcs::CPoolNode<CStgRndAnmObj, 0>, bcs::pool::NoLock> m_RndAnmPool; // 0x35C
        };
    }

    // (purpose guessed from the names) Stage 11, the volcano stage: the
    // court tilts and moves its centre line towards the team with more
    // players in its infield (court status and levels), bubbles rise on the
    // court and fire pillars burn around it. A second base class next to
    // the sport's stage manager (DodgeF's msw::CStageDodge11 has it at
    // 0x244). A DOL class, size 0x70, constructor __ct__Q23msw12CStage11BaseFv:
    // data first, vptr at 0x6C.
    //
    // The order of the virtual functions follows the DOL vtable (and
    // __vt__Q23msw13CStageDodge11 from 0x448). The modules emit weak copies
    // of the functions with bodies here; the DOL defaults of the ones a
    // module class overrides have no name, so they are declared pure. Field
    // names, return types not visible in a body and the meaning of the
    // court status values are guesses.
    class CStage11Base {
        public:
            enum COURT_STATUS {
                COURT_STATUS_0,
                COURT_STATUS_1,
                COURT_STATUS_2,
                COURT_STATUS_3
            };

            CStage11Base();

        private:
            COURT_STATUS m_nCourtStatus;              // 0x00
            float m_fStepCount;                       // 0x04
            unsigned long m_Unk08;                    // 0x08
            float m_fCurCourtRotX;                    // 0x0C
            float m_Unk10;                            // 0x10
            float m_Unk14;                            // 0x14
            float m_Unk18;                            // 0x18
            float m_fCourtRotLevel;                   // 0x1C
            float m_fCurCourtCenter;                  // 0x20
            float m_Unk24;                            // 0x24
            long m_nCourtMoveLevel;                   // 0x28
            float m_Unk2C;                            // 0x2C
            long m_Unk30;                             // 0x30
            float m_Unk34;                            // 0x34
            long m_nBubbleCourt;                      // 0x38, the team whose court gets the next bubble
            CStageMdlListNode *m_apChainMdl[4];       // 0x3C, the "kusari" (chain) models
            CStageMdlListNode *m_pRotCenterMdl;       // 0x4C
            unsigned char m_SeSlot[0xC];              // 0x50, a msw::CStgSeSlot subclass (vptr at 0x58)
            long m_Unk5C;                             // 0x5C
            float m_Unk60;                            // 0x60
            long m_nCenterFixTeam;                    // 0x64
            long m_nRotFixTeam;                       // 0x68

        public:
            virtual ~CStage11Base();                                                        // 0x08
            virtual void CtorCourtStatusChange_(COURT_STATUS status);                       // 0x0C
            virtual void LoopCourtStatusChange_(COURT_STATUS status, float frame);          // 0x10
            virtual void DtorCourtStatusChange_(COURT_STATUS status);                       // 0x14
            virtual void Stage11_Main_(float frame);                                        // 0x18
            virtual void Stage11_OnGameInit();                                              // 0x1C
            virtual void Stage11_OnTurnInit();                                              // 0x20
            virtual void SetCenterFixTeam(long team) { m_nCenterFixTeam = team; }           // 0x24
            virtual long GetCenterFixTeam() const { return m_nCenterFixTeam; }              // 0x28
            virtual void SetRotFixTeam(long team) { m_nRotFixTeam = team; }                 // 0x2C
            virtual long GetRotFixTeam() const { return m_nRotFixTeam; }                    // 0x30
            virtual int IsRotReady_() const = 0;                                            // 0x34
            virtual void SetCourtStatus(COURT_STATUS status);                               // 0x38
            virtual void ResetCourtStatus(COURT_STATUS status);                             // 0x3C
            virtual COURT_STATUS GetCourtStatus() const { return m_nCourtStatus; }          // 0x40
            virtual long GetCourtMoveLevel() const { return m_nCourtMoveLevel; }            // 0x44
            virtual long GetCourtRotLevel() const { return m_fCourtRotLevel; }              // 0x48
            virtual void SetCourtRotLevel_(long level) { m_fCourtRotLevel = level; }        // 0x4C
            virtual void AddCourtRotLevel_(long n) = 0;                                     // 0x50
            virtual void ResetCourtRotLevel_() { m_fCourtRotLevel = 0.0f; }                 // 0x54
            virtual float GetCurCourtCenter_() const { return m_fCurCourtCenter; }          // 0x58
            virtual float GetCurCourtRotX_() const { return m_fCurCourtRotX; }              // 0x5C
            virtual float GetStepCount_() const { return m_fStepCount; }                    // 0x60
            virtual long GetFloorAttribute__(const bcs::FVEC3 &rPos) const { return 2; }    // 0x64
            virtual void GetFloorFlow__(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                        CStageMan::FLOW_TYPE type) const;                   // 0x68
            virtual void SetChainMdl(long idx, CStageMdlListNode *pNode) { m_apChainMdl[idx] = pNode; } // 0x6C
            virtual void SetRotCenterMdl(CStageMdlListNode *pNode) { m_pRotCenterMdl = pNode; }         // 0x70

            // The bubbles.
            virtual void Bubble_Init();                                                     // 0x74
            virtual void Bubble_Reset();                                                    // 0x78
            virtual void Bubble_Update(float frame);                                        // 0x7C
            virtual void Bubble_CalcCreatePos();                                            // 0x80
            virtual void Bubble_Create();                                                   // 0x84
            virtual bool Bubble_IsReady() const;                                            // 0x88
            virtual void Bubble_InitCourt() { m_nBubbleCourt = bcs::BCS_RandMax(1L); }      // 0x8C
            virtual long Bubble_GetCourt() const { return m_nBubbleCourt; }                 // 0x90
            virtual void Bubble_SetNextCourt() { m_nBubbleCourt ^= 1; }                     // 0x94

            virtual void Volcano_Reset();                                                   // 0x98
            virtual void Volcano_Stop();                                                    // 0x9C
            virtual void Volcano_Update(float frame);                                       // 0xA0

            // Hooks around a change of the court status.
            virtual void OnCtorCourtStatusChange(COURT_STATUS status) {}                    // 0xA4
            virtual void OnLoopCourtStatusChange(COURT_STATUS status) {}                    // 0xA8
            virtual void OnDtorCourtStatusChange(COURT_STATUS status) {}                    // 0xAC
            virtual void OnCenterMoveBegin(long team) = 0;                                  // 0xB0
            virtual void OnCenterMoveEnd(long team) = 0;                                    // 0xB4

            // Tuning, given by the sport's stage. Angles in radians, speeds
            // per frame.
            virtual float GetParam_CourtAngle() const = 0;                                  // 0xB8
            virtual float GetParam_CourtRotSpeed() const = 0;                               // 0xBC
            virtual float GetParam_CourtRotAcc() const = 0;                                 // 0xC0
            virtual long GetParam_CourtRotDiv() const = 0;                                  // 0xC4
            virtual float GetParam_ChrFlowSpeed() const = 0;                                // 0xC8
            virtual float GetParam_CenterMoveSpeed() const = 0;                             // 0xCC
            virtual float GetParam_CenterMoveAcc() const = 0;                               // 0xD0
            virtual float GetParam_CourtCenterRange() const = 0;                            // 0xD4
            virtual long GetParam_CourtCenterDiv() const = 0;                               // 0xD8
            virtual float GetParam_CourtRotLimit() const { return 0.2617994f; }             // 0xDC, 15 degrees
            virtual float GetParam_CourtCenterFixRate() const { return 0.5f; }              // 0xE0
            virtual float GetParam_BubbleGroundHeight() const { return 5.0f; }              // 0xE4
            virtual float GetParam_BubbleZoneZ() const = 0;                                 // 0xE8
            virtual float GetParam_BubbleZoneR() const { return 4.0f; }                     // 0xEC
            virtual float GetParam_BubbleZoneAngle() const { return 1.2217305f; }           // 0xF0, 70 degrees
            virtual float GetParam_BubbleInitInterval() const = 0;                          // 0xF4
            virtual float GetParam_BubbleInterval() const = 0;                              // 0xF8
            virtual float GetParam_BubbleScale() const = 0;                                 // 0xFC
            virtual float GetParam_BubbleDownFrm() const = 0;                               // 0x100
            virtual float GetParam_BubblePreStartFrm() const = 0;                           // 0x104
            virtual float GetParam_BubbleJumpHeight() = 0;                                  // 0x108
            virtual float GetParam_BubbleJumpFrm() = 0;                                     // 0x10C
            virtual float GetParam_BubbleJumpDOF() = 0;                                     // 0x110
            // The sound of kind `type`.
            virtual long GetParam_SE(long type) const;                                      // 0x114
            // The voice of kind `type`, or -1.
            virtual long GetParam_CVID(long type) const = 0;                                // 0x118
    };
}

#endif /* !MSW_CSTAGE11BASE_HPP_ */
