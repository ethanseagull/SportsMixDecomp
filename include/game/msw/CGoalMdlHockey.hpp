#ifndef MSW_CGOALMDLHOCKEY_HPP_
#define MSW_CGOALMDLHOCKEY_HPP_

#include "game/msw/CGoalMdl.hpp"

namespace msw {

    // The goal model of the hockey game (HockeyF, game/hockey/CGoalMdlHockey.cpp,
    // not written yet), size 0x340 (CGoalObj17::NewMdl and
    // msw::CGoalObjHockey::NewMdl allocate it). Members not mapped.
    class CGoalMdlHockey : public CGoalMdl {
        public:
            CGoalMdlHockey(CStgGmk *pParent);
            virtual ~CGoalMdlHockey();
            virtual void Create();
            virtual void Create(long res, const MDL_PARAM *pParam);
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual const char *const *GetAnmNameTbl() const;
            virtual const ANM_PARAM *GetAnmParamTbl() const;

        private:
            unsigned char m_Unk12C[0x340 - 0x12C]; // 0x12C, from the end of msw::CStgMdl
    };
}

#endif /* !MSW_CGOALMDLHOCKEY_HPP_ */
