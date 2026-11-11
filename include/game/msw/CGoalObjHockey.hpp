#ifndef MSW_CGOALOBJHOCKEY_HPP_
#define MSW_CGOALOBJHOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/CGoalMdlHockey.hpp"
#include "game/msw/CStgMdlNull.hpp"

namespace msw {

    // The goal of the hockey game (HockeyF, game/hockey/CGoalObjHockey.cpp),
    // size 0x6498 (CStageManHockey::NewGoalObj). CStageManHockey::CreateGoal makes one per side.
    //
    // __vt__Q23msw14CGoalObjHockey is 0x12C bytes: the class adds no
    // virtual functions of its own (an earlier version of this header
    // declared basketball's CollUpdate, CollUpdateSub and FindCollInfo here;
    // HockeyF has none of them) and keeps CGoalObj::Main. The overrides
    // below are the ones in that vtable. NewMdl is inline (HockeyF's
    // CStageHockey17::CGoalObj17::NewMdl inlines it); GetRingPos,
    // GetBoardPos, GetBoardVec and DunkRelease are weak copies at the end of
    // CGoalObjHockey.cpp too, but their bodies are not written yet. The
    // members are one block that only gives the size.
    class CGoalObjHockey : public CGoalObj {
        public:
            CGoalObjHockey(bcs::CTask *pParent, long idx);
            virtual ~CGoalObjHockey();

            // Model 0 is an empty stand-in, the others the goal's model of
            // the goal's sub-type.
            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                CStgMdl *pMdl = 0;
                if (idx == 0) {
                    pMdl = new (heap) CStgMdlNull(this);
                } else {
                    CGoalMdlHockey *pGoalMdl = new (heap) CGoalMdlHockey(this);
                    if (pGoalMdl != 0) {
                        pGoalMdl->SetSubType(GetSubType());
                    }
                    pMdl = pGoalMdl;
                }
                return pMdl;
            }

            virtual void SetPosAuto();
            virtual const bcs::FVEC3 &GetRingPos() const;
            virtual const bcs::FVEC3 &GetBoardPos() const;
            virtual const bcs::FVEC3 &GetBoardVec() const;
            virtual void GoalIn();
            virtual void DunkStart(DUNK_POW pow, const bcs::FVEC3 &rPos);
            virtual void DunkRelease();
            virtual void DunkCancel();

        private:
            // From the end of CGoalObj's mapped members (0x74); where
            // CGoalObj's own members end is not known.
            unsigned char m_Unk74[0x6498 - 0x74];
    };
}

#endif /* !MSW_CGOALOBJHOCKEY_HPP_ */
