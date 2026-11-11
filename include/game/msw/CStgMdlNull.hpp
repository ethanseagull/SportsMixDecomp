#ifndef MSW_CSTGMDLNULL_HPP_
#define MSW_CSTGMDLNULL_HPP_

#include "bcs/CScnGroup.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    // (purpose guessed from the name) A stage model with no model of its
    // own: an empty scene group stands in for it. Size 0x130. HockeyF's
    // game/hockey/CGoalObjHockey.cpp has its vtable and functions (the goal's
    // model 0); the constructor is inline (msw::CGoalObjHockey::NewMdl).
    class CStgMdlNull : public CStgMdl {
        public:
            CStgMdlNull(CStgObj *pParent) : CStgMdl(pParent) { m_pGroup = new (0) bcs::CScnGroup(0x10, 0); }
            virtual ~CStgMdlNull();
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual bcs::CMdlShadow *NewShadowMdl(long heap) const;
            virtual bcs::CMdlRef *NewRefMdl(long heap) const;

        private:
            bcs::CScnGroup *m_pGroup; // 0x12C
    };
}

#endif /* !MSW_CSTGMDLNULL_HPP_ */
