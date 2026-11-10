#ifndef MSW_CSTAGEMANDODGE_HPP_
#define MSW_CSTAGEMANDODGE_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage of the dodge game (DodgeF), base of every dodge stage
    // (CStageDodge01 and the others made by CStageFactoryDodge). Size 0x244:
    // CStageMan (0x1FC), then a goal position, rotation and direction per
    // team.
    //
    // The inline functions are declared in the order that makes
    // CStageDodge01.cpp emit them as the original does (reverse declaration
    // order, after CStageMan's).
    class CStageManDodge : public CStageMan {
        public:
            // (name guessed) Team of a character or player index: -1 for
            // none, else the low bit.
            static long GetTeamOf(long n) { return n < 0 ? -1 : (n & 1); }

            // (name guessed) The kinds of hit in a SendMessage 0x42 that
            // make the crowd react when they come from the other team:
            // 0-2 and 6-11.
            static bool IsTeamHitType(unsigned long type)
            {
                if (type - 6 <= 5 || type <= 2) {
                    return true;
                }
                return false;
            }

            CStageManDodge(bcs::CTask *pParent);
            virtual ~CStageManDodge();

            virtual void Disp3D(long);
            virtual float GetCourtCenter() const
            {
                float back = GetTeamCourtBack(1);
                float front = GetTeamCourtFront(0);
                return front + (back - front) * 0.5f;
            }
            virtual bool IsNoBorder() const { return false; }
            virtual bool CanbeComeback() const { return true; }
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const { return m_aGoalPos[goal]; }
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const { return m_aGoalRot[goal]; }
            virtual const bcs::FVEC3 &GetBoardPos(long) const { return m_vZero; }
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const { return m_aGoalVec[goal]; }
            virtual void GoalCrush(long) {}
            virtual void GoalRepairAll() {}
            virtual float GetTeamOuterFieldLeft(long team, long n) const;
            virtual float GetTeamOuterFieldRight(long team, long n) const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
            virtual long GetTeamIdxByPos(const bcs::FVEC3 &rPos) const;
            virtual void SendMessage(long msg, long, long, long);
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseGroundWidth_() const;
            virtual float GetBaseGroundDepth_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
            virtual float GetBaseOuterFront_() const;
            virtual long GetQPanelEnabled_() const;
            virtual void OnPostInitialize();

            // (return type guessed: one)
            virtual bool IsEnemyZoneLimited() const { return true; }
            // Plays the stage's sound for event `event` (the derived stages
            // fill it in).
            virtual void OnSoundEvent(long event) {}

            // Makes stage `stageNo` through CStageFactoryDodge.
            static CStageManDodge *CreateInstance(bcs::CTask *pParent, long stageNo);

        private:
            bcs::FVEC3 m_aGoalPos[2]; // 0x1FC, only z is set
            bcs::FVEC3 m_aGoalRot[2]; // 0x214
            bcs::FVEC3 m_aGoalVec[2]; // 0x22C, unit vector along the goal's rotation
    };
}

#endif /* !MSW_CSTAGEMANDODGE_HPP_ */
