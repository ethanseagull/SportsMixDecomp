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

            // Overrides are declared in the order of their vtable slots; the
            // new virtual functions follow in the order of
            // __vt__Q23msw15CStageManHockey. Only the declarations are known
            // so far.
            virtual float GetOuterFieldLeft() const;
            virtual float GetOuterFieldRight() const;
            virtual float GetOuterFieldFront() const;
            virtual float GetOuterFieldBack() const;
            virtual CGoalObj *GetGoal(long goal);
            virtual const CGoalObj *GetGoal(long goal) const;
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const;
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const;
            virtual float GetGoalWidth(long goal) const;
            virtual float GetGoalX1Length(long goal) const;
            virtual float GetGoalRRRadius(long goal) const;
            virtual float GetGoalDepth(long goal) const;
            virtual void GoalCrush(long goal);
            virtual void GoalRepairAll();
            virtual void GoalIn(long goal);
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtRadius_() const;
            virtual float GetBaseOuterRadius_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnPostInitialize();

            // (return types of the next two guessed)
            virtual long GetCurrentPanelType() const;
            virtual long GetPanelScore(long score) const;
            virtual void CreateGoal();
            // (return type guessed)
            virtual CGoalObj *NewGoalObj(bcs::CTask *pParent, long goal);
            virtual long GetBallTeam() const;
            virtual void RegistRotNode(CStageMdlListNode *pNode, float rot);
            virtual void UpdateRotNode();
            // Hooks for the derived stages.
            virtual void OnHockeyGoalResult(int);
            virtual void OnHockeyShoot(long);
            virtual void OnSoundEvent(long);

        private:
            unsigned char m_Unk1FC[0x1C]; // size 0x218
    };
}

#endif /* !MSW_CSTAGEMANHOCKEY_HPP_ */
