#ifndef MSW_CGOALOBJBASKET_HPP_
#define MSW_CGOALOBJBASKET_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/CGoalObj.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CGoalMdlBasket.hpp"
#include "game/msw/CStgMdlNull.hpp"

namespace msw {

    // The goal of the basket game (BasketF, game/basket/CGoalObjBasket.cpp),
    // size 0x64A8. CStageManBasket::CreateGoal makes one per side. Besides
    // CGoalObj it has body collision (two units) that keeps players out of
    // the goal's stand; the shape of the stand depends on the stage.
    //
    // __vt__Q23msw14CGoalObjBasket ends at 0x138 with the class's own
    // CollUpdate, CollUpdateSub and FindCollInfo (0x12C to 0x134), so a
    // derived class's new virtual functions start at 0x138.
    class CGoalObjBasket : public CGoalObj {
        public:
            // The body collision of one unit: in the goal's frame, rotated by
            // the goal's Y rotation and moved to its board position.
            struct COLL_INFO {
                int bEnable;       // 0x00
                bcs::FVEC3 pos;    // 0x04
                bcs::FVEC3 size;   // 0x10
            };

            // The units' shapes of one stage (a goal sub-type, GameWork's
            // stage number); a table of them ends with type -1.
            struct COLL_TABLE {
                long nType;           // 0x00
                COLL_INFO aInfo[2];   // 0x04
            };

            CGoalObjBasket(bcs::CTask *pParent, long idx);
            virtual ~CGoalObjBasket();
            virtual void Main();

            // Model 0 is an empty stand-in, the others the goal's models.
            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                if (idx == 0) {
                    return new (heap) CStgMdlNull(this);
                }
                return new (heap) CGoalMdlBasket(this);
            }

            virtual void OnPostCreate();
            virtual void SetPosAuto();

            virtual void CollUpdate();                                                // 0x12C
            virtual int CollUpdateSub(long idx, const COLL_INFO &rInfo);              // 0x130
            virtual const COLL_INFO *FindCollInfo(long type) const;                   // 0x134

        private:
            CBodyCollision m_BodyColl;     // 0x6498
            const COLL_INFO *m_pCollInfo;  // 0x64A4, this stage's two units (none: no collision)
    };
}

#endif /* !MSW_CGOALOBJBASKET_HPP_ */
