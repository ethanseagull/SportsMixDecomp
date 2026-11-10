#ifndef MSW_CSTAGEBASKETHUNGRYBOSSPACKUN_HPP_
#define MSW_CSTAGEBASKETHUNGRYBOSSPACKUN_HPP_

#include "game/msw/CGoalObjBasket_BossPackun.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    class CNpcBossPackun;

    // Base of the Hungry Boss Packun stages of the basket game (70 to 72).
    // The boss carries goal 0 about the court: the stage has its own goals
    // (CGoalObjBasket_BossPackun), makes the boss for goal 0, and answers
    // every goal query with goal 0. Size 0x23C (stage 70 adds nothing).
    //
    // Overrides are declared in the order of their vtable slots. The bodies
    // given here are the ones the unit emits as weak functions (after their
    // first caller, or at the end of the unit in reverse order).
    class CStageBasketHungryBossPackun : public CStageManBasket {
        public:
            CStageBasketHungryBossPackun(bcs::CTask *pParent);
            virtual ~CStageBasketHungryBossPackun();

            // The court of team 0, with the front of team 1.
            virtual float GetCourtLeft() const;
            virtual float GetCourtRight() const;
            virtual float GetCourtFront() const;
            virtual float GetCourtBack() const;

            // Every goal query is answered for goal 0, the boss's goal.
            virtual CGoalObj *GetGoal(long goal) { return m_apGoal[0]; }
            virtual const CGoalObj *GetGoal(long goal) const { return m_apGoal[0]; }
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const { return CStageManBasket::GetGoalPos(0); }
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const { return CStageManBasket::GetGoalRot(0); }
            virtual const bcs::FVEC3 &GetBoardPos(long goal) const { return CStageManBasket::GetBoardPos(0); }
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const { return CStageManBasket::GetGoalVec(0); }
            // Plays sound event 0 on message 0x12 (meaning not known).
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3)
            {
                CStageManBasket::SendMessage(msg, arg1, arg2, arg3);
                if (msg == 0x12) {
                    OnSoundEvent(0);
                }
            }
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void CreateGoal();

            // Moves both teams' courts so that they keep their distance from
            // the centre line on this stage's court depth.
            virtual void UpdateCourtSize_();

        private:
            CNpcBossPackun *m_apNpc[4];              // 0x218, only [0] is made
            CGoalObjBasket_BossPackun *m_apGoal[4];  // 0x228, CreateGoal makes the first two
            CStageMdlListNode *m_pBgNode;            // 0x238, the "stage<n>_bg" model
    };
}

#endif /* !MSW_CSTAGEBASKETHUNGRYBOSSPACKUN_HPP_ */
