#ifndef MSW_CSTAGEBASKET05_HPP_
#define MSW_CSTAGEBASKET05_HPP_

#include "game/msw/CStageManBasket.hpp"

namespace msw {

    class CStageNodeAnm;

    // Basket stage 05. Size 0x29C (from CStageFactoryBasket::Create).
    //
    // Each team's half of the court is a raft that floats back and forth
    // along Z: the team court offsets follow a sine wave with a stop at each
    // end, and whatever stands on a raft is carried with it (GetFloorFlow).
    // Between a raft and the end of the ground is a hole (GetHoleRect). Each
    // raft carries a turret; when the rafts swing out far enough in the
    // heavy phase, the turret fires and stays broken. A message (0x27, 3)
    // can fix the rafts towards one team's goal.
    //
    // The same stage is in DodgeF and VolleyF (CStageDodge05,
    // CStageVolley05) on the mode's own stage manager; what is basket's own
    // here is the base class, the three-point centres that move with the
    // rafts and the parameter names (BASKET_PRM_S05_*).
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw14CStageBasket05.
    class CStageBasket05 : public CStageManBasket {
        public:
            CStageBasket05(bcs::CTask *pParent);
            virtual ~CStageBasket05();

            virtual float GetCourtFront() const;
            virtual float GetCourtBack() const;
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
            virtual float GetOuterFieldFront() const;
            virtual float GetOuterFieldBack() const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual const bcs::FVEC3 &Get3PointAreaPos(long goal) const;
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnGameInit();
            virtual void OnPeriodInit();
            virtual void OnEntranceEventStart();
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
            // Whether the rafts move: the gimmick runs, nothing fixes them
            // and they have been started.
            virtual int IsReadyFloorFlow_() const;
            virtual void Turret_Update(float frame);
            // Puts each turret at the end of its raft.
            virtual void Turret_SetPos();
            // Puts both turrets back, whole or (bBroken) broken.
            virtual void Turret_Reset(int bBroken);
            // The shot of team's turret.
            virtual void Turret_Hit(long team);
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

            bcs::FVEC3 m_a3PointPos[RAFT_NUM];         // 0x218, each goal's three-point centre, moved with the raft
            bcs::FVEC3 m_aFloorFlow[RAFT_NUM];         // 0x230, how far each raft moved in the last frame
            CMdlPtrPool::Handle m_aRaftMdl[RAFT_NUM];  // 0x248, the "null_wavelog_<n>" models
            TURRET m_aTurret[RAFT_NUM];                // 0x258
            float m_fFloatTime;                        // 0x280, frames into the rafts' period
            float m_fFloatSpeed;                       // 0x284, 0 to 1: the rafts speed up at the start
            int m_bFloatReady;                         // 0x288, see IsReadyFloorFlow_
            int m_bHeavyShake;                         // 0x28C, the rafts swing in the heavy phase
            long m_nHeavyWait;                         // 0x290, turns of the rafts let pass before the heavy phase
            float m_fHeavyTime;                        // 0x294, frames before the heavy phase may start
            long m_nFixTeam;                           // 0x298, the team whose goal the rafts are fixed to, or -1
    };
}

#endif /* !MSW_CSTAGEBASKET05_HPP_ */
