#ifndef MSW_CSTAGEMANBASKET_HPP_
#define MSW_CSTAGEMANBASKET_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage manager of the basket game (BasketF), base of every basket
    // stage (CStageBasket01 and the others; CStageFactoryBasket makes the
    // one for a stage number). Size 0x218: CStageMan (0x1FC), then the goals,
    // the team with the ball and an optional model that turns towards the
    // goal being attacked.
    //
    // Overrides are declared in the order of their vtable slots; the new
    // virtual functions follow in the order of __vt__Q23msw15CStageManBasket.
    // The bodies given here are the ones the units emit as weak functions.
    class CStageManBasket : public CStageMan {
        public:
            CStageManBasket(bcs::CTask *pParent);
            virtual ~CStageManBasket();

            virtual CGoalObj *GetGoal(long goal) { return m_apGoal[goal]; }
            virtual const CGoalObj *GetGoal(long goal) const { return m_apGoal[goal]; }
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const;
            virtual const bcs::FVEC3 &GetBoardPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const;
            virtual long GetGoalSE(GOAL_SE_TYPE type) const;
            // Whether rPos is inside the three-point line of `goal`: within
            // `radius` of Get3PointAreaPos(goal) (height ignored), or in the
            // band of that half-width that runs from there to the end of
            // the court.
            virtual bool Is3PointArea(long goal, const bcs::FVEC3 &rPos, float radius) const;
            virtual void GoalCrush(long goal);
            virtual void GoalRepairAll();
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnPostInitialize();

            // (return types of the next two guessed)
            virtual long GetCurrentPanelType() const { return 0; }
            virtual long GetPanelScore(long score, long) const { return score; }
            // The offset of each goal from its default place.
            virtual const bcs::FVEC3 &GetGoalOffsetPos() const;
            virtual void CreateGoal();
            // The team of the first character in the game that has the
            // ball (bit 0 of its status flags), or -1.
            virtual long GetBallTeam() const;
            // Makes pNode's model turn with the attacked goal; rot is added
            // to the turn.
            virtual void RegistRotNode(CStageMdlListNode *pNode, float rot)
            {
                m_pRotNode = pNode;
                m_fRotNodeOffset = rot;
            }
            virtual void UpdateRotNode();
            // Hooks for the derived stages: the messages of SendMessage
            // (0x32, 0x34), and the change of the team with the ball.
            virtual void OnBasketGoalResult(int) {}
            virtual void OnBasketShoot(long, long) {}
            virtual void OnBasketSwitchOffense(long) {}
            // Sounds of the stage for events 0 to 3.
            virtual void OnSoundEvent(long) {}

            // Creates the stage of number `stageNo` (through
            // CStageFactoryBasket) and gives it its number.
            static CStageManBasket *CreateInstance(bcs::CTask *pParent, long stageNo);

        private:
            CGoalObj *m_apGoal[4];         // 0x1FC, CreateGoal makes the first two
            long m_nBallTeam;              // 0x20C, -1 for none
            CStageMdlListNode *m_pRotNode; // 0x210
            float m_fRotNodeOffset;        // 0x214
    };
}

#endif /* !MSW_CSTAGEMANBASKET_HPP_ */
