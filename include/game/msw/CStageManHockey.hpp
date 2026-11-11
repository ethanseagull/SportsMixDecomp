#ifndef MSW_CSTAGEMANHOCKEY_HPP_
#define MSW_CSTAGEMANHOCKEY_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage manager of the hockey game (HockeyF), base of every hockey
    // stage. Size 0x218 (the default allocation in CStageFactoryHockey::Create):
    // CStageMan (0x1FC), then members not mapped yet (BasketF's CStageManBasket
    // has the same size).

    class CStageManHockey : public CStageMan {
        public:
            CStageManHockey(bcs::CTask *pParent);
            virtual ~CStageManHockey();

            // Makes the stage manager of stage stageNo (CStageFactoryHockey).
            static CStageManHockey *CreateInstance(bcs::CTask *pParent, long stageNo);

            // Overrides are declared in the order of their vtable slots; the
            // new virtual functions follow in the order of
            // __vt__Q23msw15CStageManHockey. The bodies given are the ones
            // HockeyF emits as weak functions (GetBaseOuterRadius_'s -1.0f
            // is in CStageHockey01's .rodata); the others are defined in
            // game/hockey/CStageManHockey.cpp.
            virtual float GetOuterFieldLeft() const;
            virtual float GetOuterFieldRight() const;
            virtual float GetOuterFieldFront() const;
            virtual float GetOuterFieldBack() const;
            virtual CGoalObj *GetGoal(long goal) { return m_pGoal[goal]; }
            virtual const CGoalObj *GetGoal(long goal) const { return m_pGoal[goal]; }
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const;
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const;
            virtual float GetGoalWidth(long goal) const;
            virtual float GetGoalX1Length(long goal) const;
            virtual float GetGoalRRRadius(long goal) const;
            virtual float GetGoalDepth(long goal) const;
            virtual void GoalCrush(long goal) {}
            virtual void GoalRepairAll() {}
            virtual void GoalIn(long goal);
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;
            virtual float GetBaseOuterRadius_() const { return -1.0f; }
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnPostInitialize();

            // (return types of the next two guessed)
            virtual long GetCurrentPanelType() const { return 0; }
            virtual long GetPanelScore(long score) const { return score; }
            virtual void CreateGoal();
            // (return type guessed)
            virtual CGoalObj *NewGoalObj(bcs::CTask *pParent, long goal);
            virtual long GetBallTeam() const;
            virtual void RegistRotNode(CStageMdlListNode *pNode, float rot)
            {
                m_pRotNode = pNode;
                m_fRot = rot;
            }
            virtual void UpdateRotNode();
            // Hooks for the derived stages.
            virtual void OnHockeyGoalResult(int) {}
            virtual void OnHockeyShoot(long) {}
            virtual void OnSoundEvent(long) {}

        private:
            // 0x1FC: the goals made by CreateGoal (two; GetGoal's callers
            // ask for up to four, and the constructor clears four).
            CGoalObj *m_pGoal[4];
            long m_nBallTeam;              // 0x20C, the team holding the ball (name guessed), -1 for none
            // 0x210 and 0x214 (names guessed): the node RegistRotNode
            // registers and its angle, for UpdateRotNode.
            CStageMdlListNode *m_pRotNode;
            float m_fRot;
    };
}

#endif /* !MSW_CSTAGEMANHOCKEY_HPP_ */
