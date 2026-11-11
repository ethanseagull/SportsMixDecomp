#ifndef MSW_CSTAGEBASKETHUNGRYBOSSPACKUN71_HPP_
#define MSW_CSTAGEBASKETHUNGRYBOSSPACKUN71_HPP_

#include "game/msw/CStageBasketHungryBossPackun.hpp"

namespace msw {

    // Hungry Boss Packun stage 71. Size 0x284 (from CStageFactoryBasket::
    // Create). The floors of the two teams' courts rise and sink in turn
    // (they move the courts' offsets along Z), carrying the players with
    // them; the "null_wavelog_<n>" models follow the two courts.
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual function follows.
    class CStageBasketHungryBossPackun71 : public CStageBasketHungryBossPackun {
        public:
            CStageBasketHungryBossPackun71(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun71();

            // The court of each team is limited by where the floors have
            // moved it.
            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            // How far the floor under rPos moved in the last frame.
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual const bcs::FVEC3 &Get3PointAreaPos(long goal) const;
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnGimmickReset();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);
            // Empty: the floors set the courts' offsets (Main_).
            virtual void UpdateCourtSize_();

            // Whether the floors move: once the gimmick runs and the game
            // has let them start (Main_).
            virtual int IsReadyFloorFlow_() const;

        private:
            enum {
                FLOOR_NUM = 2 // one floor per team
            };

            bcs::FVEC3 m_a3PointPos[FLOOR_NUM];            // 0x23C, each goal's three-point centre, moved with the floor
            bcs::FVEC3 m_aFloorFlow[FLOOR_NUM];            // 0x254, how far each floor moved in the last frame
            CStageMdlListNode *m_apFloorNode[FLOOR_NUM];   // 0x26C, the "null_wavelog_<n>" models
            float m_fFloorTime;                            // 0x274, frames into the floors' cycle
            float m_fFloorSpeed;                           // 0x278, 0 to 1: the floors speed up at the start
            int m_bFloorDir;                               // 0x27C, which way the floors move first
            int m_bFloorReady;                             // 0x280, see IsReadyFloorFlow_
    };
}

#endif /* !MSW_CSTAGEBASKETHUNGRYBOSSPACKUN71_HPP_ */
