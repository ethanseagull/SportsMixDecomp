#ifndef MSW_CGOALOBJBASKET_HPP_
#define MSW_CGOALOBJBASKET_HPP_

#include "game/CGoalObj.hpp"

namespace msw {

    // The goal of the basket game (BasketF, game/basket/CGoalObjBasket.cpp),
    // size 0x64A8. CStageManBasket::CreateGoal makes one per side.
    //
    // INCOMPLETE: only what CStageManBasket needs is here. The second base
    // class (msw::CBodyCollision, with its own part of the vtable) and the
    // class's own virtual functions (CollUpdate, CollUpdateSub,
    // FindCollInfo) are not declared, and the members are one block that
    // only gives the size, so the class must not be derived from and its
    // vtable must not be emitted from this declaration yet.
    class CGoalObjBasket : public CGoalObj {
        public:
            CGoalObjBasket(bcs::CTask *pParent, long idx);
            virtual ~CGoalObjBasket();
            virtual void Main();

            virtual CStgMdl *NewMdl(long heap, unsigned long idx);

            // The class's own virtual functions, at vtable offsets 0x12C to
            // 0x134 (__vt__Q23msw14CGoalObjBasket ends there, so a derived
            // class's new ones start at 0x138). Return types and the
            // contents of COLL_INFO are not known.
            struct COLL_INFO;
            virtual void CollUpdate();
            virtual void CollUpdateSub(long idx, const COLL_INFO &rInfo);
            virtual const COLL_INFO *FindCollInfo(long idx) const;

        private:
            // From the end of msw::CStgGmk (0x68); where CGoalObj's own
            // members end is not known.
            unsigned char m_Unk68[0x64A8 - 0x68];
    };
}

#endif /* !MSW_CGOALOBJBASKET_HPP_ */
