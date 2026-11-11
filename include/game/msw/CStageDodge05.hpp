#ifndef MSW_CSTAGEDODGE05_HPP_
#define MSW_CSTAGEDODGE05_HPP_

#include "game/msw/CStageManDodge.hpp"

namespace msw {

    class CStageNodeAnm;

    // Dodge stage 05. Size 0x2B4 (from CStageFactoryDodge::Create).
    //
    // Each team's half of the court is a raft that floats back and forth
    // along Z: the team court offsets follow a sine wave with a stop at each
    // end, and whatever stands on a raft is carried with it (GetFloorFlow).
    // Between a raft and the end zone behind it is a hole (GetHoleRect). Each
    // raft carries a turret; when the rafts swing out far enough in the
    // heavy phase, the turret fires and stays broken. A message (0x27, 3)
    // can fix the rafts towards one team's goal.
    //
    // The same stage is in BasketF and VolleyF (CStageBasket05,
    // CStageVolley05) on the mode's own stage manager. What is dodge's own
    // here: the base class, holes and floor flow measured against the outer
    // fields, the outer field limits themselves, a 13-unit raft, a stage
    // sound on a random timer (m_fEnvSeTime) and the parameter names
    // (DODGE_PRM_S05_*).
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw13CStageDodge05.
    class CStageDodge05 : public CStageManDodge {
        public:
            CStageDodge05(bcs::CTask *pParent);
            virtual ~CStageDodge05();

            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual float GetCourtInnerAreaFront() const;
            virtual float GetCourtInnerAreaBack() const;
            virtual float GetTeamCourtInnerAreaFront(long team) const;
            virtual float GetTeamCourtInnerAreaBack(long team) const;
            virtual float GetGroundFront() const;
            virtual float GetGroundBack() const;
            virtual float GetTeamGroundFront(long team) const;
            virtual float GetTeamGroundBack(long team) const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnGameInit();
            virtual void OnPeriodInit();
            virtual void OnEntranceEventStart();
            virtual void OnVictoryEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickCancel();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // Half the length of a raft along Z.
            virtual float GetRaftDepth_() const;
            // The Z of the front edge of team 1's raft and of the back edge
            // of team 0's.
            virtual float GetRaftEdgeFront_() const;
            virtual float GetRaftEdgeBack_() const;
            // The hole between team's raft and the end of the ground; false
            // when there is none.
            virtual bool GetHoleRect(long team, float *pLeft, float *pRight, float *pFront, float *pBack) const;
            virtual void Turret_Update(float frame);
            // Puts each turret at the end of its raft.
            virtual void Turret_SetPos();
            // Puts both turrets back, whole or (bBroken) broken.
            virtual void Turret_Reset(int bBroken);
            // The shot of team's turret.
            virtual void Turret_Hit(long team);
            // Whether the rafts move: the gimmick runs, nothing fixes them
            // and they have been started.
            virtual int IsReadyFloorFlow_() const;
            virtual int IsNowHeavyShake() const;
            // The tuning values of the phase the rafts are in, in world
            // units and frames.
            virtual float GetParam_FloatDepth() const;
            virtual float GetParam_DepthCycle() const;
            virtual float GetParam_FloatStopTime() const;
            virtual float GetParam_TurretRadius() const;

        private:
            enum {
                RAFT_NUM = 2 // one per team
            };

            // (member names guessed) A turret on a raft, size 0x14.
            struct TURRET {
                CStageMdlListNode *m_pNode; // 0x00, the "yagura_<n>" model
                bcs::FVEC3 m_vPos;          // 0x04, where its sounds play
                int m_bBroken;              // 0x10
            };

            void ResetHeavyShake_();

            bcs::FVEC3 m_aFloorFlow[RAFT_NUM];         // 0x244, how far each raft moved in the last frame
            CMdlPtrPool::Handle m_aRaftMdl[RAFT_NUM];  // 0x25C, the "null_wavelog_<n>" models
            TURRET m_aTurret[RAFT_NUM];                // 0x26C
            float m_fFloatTime;                        // 0x294, frames into the rafts' period
            float m_fFloatSpeed;                       // 0x298, 0 to 1: the rafts speed up at the start
            int m_bFloatReady;                         // 0x29C, see IsReadyFloorFlow_
            float m_fEnvSeTime;                        // 0x2A0, frames before the next stage sound (set, not yet seen read)
            int m_bHeavyShake;                         // 0x2A4, the rafts swing in the heavy phase
            long m_nHeavyWait;                         // 0x2A8, turns of the rafts let pass before the heavy phase
            float m_fHeavyTime;                        // 0x2AC, frames before the heavy phase may start
            long m_nFixTeam;                           // 0x2B0, the team whose goal the rafts are fixed to, or -1
    };
}

#endif /* !MSW_CSTAGEDODGE05_HPP_ */
