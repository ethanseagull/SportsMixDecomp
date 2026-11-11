#ifndef MSW_CGOALMDLBASKET_HPP_
#define MSW_CGOALMDLBASKET_HPP_

#include "game/msw/CGoalMdl.hpp"

namespace msw {

    // A model of the basket game's goal (BasketF,
    // game/basket/CGoalMdlBasket.cpp), size 0x340 (msw::CGoalObjBasket's
    // NewMdl allocates it). The model index (CStgMdl::GetIndex) picks the
    // part: 1 and 5 fade by view, 4 and 5 are view-fading figures, 1 also
    // has per-node motion settings.
    class CGoalMdlBasket : public CGoalMdl {
        public:
            CGoalMdlBasket(CStgGmk *pParent);
            virtual ~CGoalMdlBasket();
            virtual void Create() { CGoalMdl::Create(); }
            virtual void Create(long res, const MDL_PARAM *pParam);
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual const bcs::CAnmObjMotBase::NODE_EX_INFO *GetNodeExInfo() const;
    };
}

#endif /* !MSW_CGOALMDLBASKET_HPP_ */
