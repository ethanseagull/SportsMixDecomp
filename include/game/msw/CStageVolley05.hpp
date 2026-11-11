#ifndef MSW_CSTAGEVOLLEY05_HPP_
#define MSW_CSTAGEVOLLEY05_HPP_

#include "game/msw/CStageManVolley.hpp"

namespace msw {

    // Volley stage 05, the rafts. Size 0x270 (from CStageFactoryVolley::Create).
    //
    // Each team's half of the court is a raft that sways on the water: from
    // side to side along X on a sine wave (the "H" motion) and back and
    // forth along Z on a cosine wave (the "V" motion), each with its own
    // cycle and its own heavy phase with wider swings. Whatever stands on
    // a raft is carried with it (GetFloorFlow). Each raft carries a turret
    // at its left end that fires as the raft reaches the end of a swing,
    // and breaks when the rafts swing out far enough in the heavy phase.
    //
    // DodgeF and BasketF have a stage 05 with rafts too (CStageDodge05,
    // CStageBasket05), but there the rafts only float along Z with stops at
    // the ends; this one is volley's own.
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw14CStageVolley05.
    class CStageVolley05 : public CStageManVolley {
        public:
            CStageVolley05(bcs::CTask *pParent);
            virtual ~CStageVolley05();

            virtual void Disp3D(long);
            virtual float GetGroundLeft() const;
            virtual float GetGroundRight() const;
            virtual float GetGroundFront() const;
            virtual float GetGroundBack() const;
            virtual float GetTeamGroundLeft(long team) const;
            virtual float GetTeamGroundRight(long team) const;
            virtual float GetTeamGroundFront(long team) const;
            virtual float GetTeamGroundBack(long team) const;
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseGroundWidth_() const;
            virtual float GetBaseGroundDepth_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
            virtual float GetBaseOuterFront_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                         CMdlPtrPool::Handle &rHandle, CArn &rArn, long n);
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnGameEnd();
            virtual void OnGameInit();
            virtual void OnPeriodInit();
            virtual void OnEntranceEventStart();
            virtual void OnVictoryEventStart();
            virtual void OnGimmickReset();
            virtual void OnGimmickCancel();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // Half the width of a raft along X.
            virtual float GetRaftWidth_() const;
            // The X of the left and right edges of team's raft.
            virtual float GetRaftEdgeLeft_(long team) const;
            virtual float GetRaftEdgeRight_(long team) const;
            // Where team's court will be `frame` frames from now.
            virtual void GetAssumedTeamCourtOffset_(float frame, long team, bcs::FVEC3 &rOffset) const;
            virtual void Turret_Update(float frame);
            // Puts each turret at the left end of its raft's swing.
            virtual void Turret_SetPos();
            // Puts both turrets back, whole or (bBroken) broken.
            virtual void Turret_Reset(int bBroken);
            // The shot of team's turret.
            virtual void Turret_Hit(long team);
            // Whether the rafts move: the gimmick runs and they have been
            // started.
            virtual int IsReadyFloorFlow_() const;
            virtual int IsNowHeavyShakeH() const;
            virtual int IsNowHeavyShakeV() const;
            // The tuning values of the phase the rafts are in, in world
            // units and frames.
            virtual float GetParam_FloatDepth() const;
            virtual float GetParam_DepthCycle() const;
            virtual float GetParam_FloatWidth() const;
            virtual float GetParam_WidthCycle() const;
            virtual float GetParam_TurretRadius() const;

            // GetAssumedTeamCourtOffset_ of the current stage, which must
            // be this one.
            static void GetAssumedTeamCourtOffset(float frame, long team, bcs::FVEC3 &rOffset);

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

            bcs::FVEC3 m_aFloorFlow[RAFT_NUM];        // 0x200, how far each raft moved in the last frame
            CMdlPtrPool::Handle m_aRaftMdl[RAFT_NUM]; // 0x218, the "null_wavelog_<n>" models
            TURRET m_aTurret[RAFT_NUM];               // 0x228
            float m_fFloatTimeH;                      // 0x250, frames into the side to side swing
            float m_fFloatTimeV;                      // 0x254, frames into the back and forth swing
            float m_fFloatSpeedH;                     // 0x258, 0 to 1: the rafts speed up at the start
            float m_fFloatSpeedV;                     // 0x25C
            int m_bFloatReady;                        // 0x260, see IsReadyFloorFlow_
            int m_bHeavyShakeH;                       // 0x264, the side to side swing is in its heavy phase
            int m_bHeavyShakeV;                       // 0x268, the back and forth swing is in its heavy phase
            float m_fHeavyTime;                       // 0x26C, frames before the heavy phase may start
    };
}

#endif /* !MSW_CSTAGEVOLLEY05_HPP_ */
