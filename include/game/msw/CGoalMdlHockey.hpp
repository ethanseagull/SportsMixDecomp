#ifndef MSW_CGOALMDLHOCKEY_HPP_
#define MSW_CGOALMDLHOCKEY_HPP_

#include "game/msw/CGoalMdl.hpp"

namespace msw {

    // The goal model of the hockey game (HockeyF, game/hockey/CGoalMdlHockey.cpp), size 0x340 (CGoalObj17::NewMdl and
    // msw::CGoalObjHockey::NewMdl allocate it), all of it msw::CGoalMdl's.
    class CGoalMdlHockey : public CGoalMdl {
        public:
            CGoalMdlHockey(CStgGmk *pParent);
            virtual ~CGoalMdlHockey();
            virtual void Create();
            virtual void Create(long res, const MDL_PARAM *pParam);
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            // New virtual functions of this class (msw::CGoalMdl's vtable
            // ends at 0x130): the suffixes of the animation names, ending
            // with null, and the animations.
            virtual const char *const *GetAnmNameTbl() const;       // 0x134
            virtual const ANM_PARAM *GetAnmParamTbl() const;        // 0x138
    };
}

#endif /* !MSW_CGOALMDLHOCKEY_HPP_ */
