#ifndef MSW_CSTAGEMANVOLLEY_HPP_
#define MSW_CSTAGEMANVOLLEY_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage of the volley game (VolleyF), size 0x200: CStageMan (0x1FC)
    // and the net. The stages of the volley courts derive from it
    // (CStageFactoryVolley makes them). The inline overrides are declared in
    // the reverse of the order the module emits them.
    class CStageManVolley : public CStageMan {
        public:
            CStageManVolley(bcs::CTask *pParent);
            virtual ~CStageManVolley();

            virtual bool IsNoBorder() const { return false; }
            virtual bool CanbeComeback() const { return false; }
            virtual const bcs::FVEC3 &GetGoalPos(long) const { return m_vZero; }
            virtual const bcs::FVEC3 &GetGoalRot(long) const { return m_vZero; }
            virtual const bcs::FVEC3 &GetBoardPos(long) const { return m_vZero; }
            virtual const bcs::FVEC3 &GetGoalVec(long) const { return m_vZero; }
            virtual void GoalCrush(long) {}
            virtual void GoalRepairAll() {}
            virtual const CStageNet *GetNet() const { return m_pNet; }
            virtual CStageNet *GetNet() { return m_pNet; }

            virtual long GetGoalSE(GOAL_SE_TYPE type) const;
            virtual void SendMessage(long msg, long, long arg, long);

            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtFrontZoneDepth_() const;
            virtual float GetBaseGroundWidth_() const;
            virtual float GetBaseGroundDepth_() const;
            virtual float GetBaseOuterWidth_() const;
            virtual float GetBaseOuterDepth_() const;
            virtual float GetBaseOuterFront_() const;
            virtual float GetBaseNetWidth_() const;
            virtual float GetBaseNetHeightTop_() const;
            virtual float GetBaseNetHeightBottom_() const;

            virtual void OnPostInitialize();

            // Makes the net; OnPostInitialize creates it and places it at
            // the origin.
            virtual CStageNet *NewNet(bcs::CTask *pParent);
            // (meaning of the argument guessed from SendMessage: which
            // crowd reaction, 0 to 5) Empty here.
            virtual void OnSoundEvent(long);

            // Creates the stage manager of stage `stageNo`.
            static CStageManVolley *CreateInstance(bcs::CTask *pParent, long stageNo);

        private:
            CStageNet *m_pNet; // 0x1FC
    };
}

#endif /* !MSW_CSTAGEMANVOLLEY_HPP_ */
