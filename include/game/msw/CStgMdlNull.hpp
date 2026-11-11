#ifndef MSW_CSTGMDLNULL_HPP_
#define MSW_CSTGMDLNULL_HPP_

#include "bcs/CScnGroup.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    // (purpose guessed from the name) A stage model with no model of its
    // own: an empty scene group stands in for it. Size 0x130. Everything is
    // inline: HockeyF's game/hockey/CGoalObjHockey.cpp and BasketF's
    // game/basket/CGoalObjBasket.cpp (the goal's model 0) both have weak
    // copies of the vtable and the functions, in reverse declaration order
    // with the destructor last.
    class CStgMdlNull : public CStgMdl {
        public:
            CStgMdlNull(CStgObj *pParent) : CStgMdl(pParent) { m_pGroup = new (0) bcs::CScnGroup(0x10, 0); }
            virtual ~CStgMdlNull()
            {
                if (m_pGroup != 0) {
                    bcs::CScnObj::AppendDelayDeleteObject(m_pGroup);
                }
                m_pGroup = 0;
            }
            virtual bcs::CScnObj *GetScnObj() { return m_pGroup; }
            virtual const bcs::CScnObj *GetScnObj() const { return m_pGroup; }
            virtual bcs::CMdlObj *NewMdl(long heap) const { return 0; }
            virtual bcs::CMdlShadow *NewShadowMdl(long heap) const { return 0; }
            virtual bcs::CMdlRef *NewRefMdl(long heap) const { return 0; }

        private:
            bcs::CScnGroup *m_pGroup; // 0x12C
    };
}

#endif /* !MSW_CSTGMDLNULL_HPP_ */
