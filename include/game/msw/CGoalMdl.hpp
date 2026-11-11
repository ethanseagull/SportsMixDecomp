#ifndef MSW_CGOALMDL_HPP_
#define MSW_CGOALMDL_HPP_

#include "game/msw/CStgMdl.hpp"

namespace msw {

    class CStgGmk;

    // The model of a goal (CGoalObj's model 1). A DOL class, constructor
    // __ct__Q23msw8CGoalMdlFPQ23msw7CStgGmk; msw::CStgMdl's vptr stays at
    // 0x128. Its own members from 0x12C (HockeyF's CGoalMdlHockey ends
    // at 0x340). The new virtual functions follow
    // __vt__Q33msw14CStageHockey1710CGoalMdl17 (HockeyF); names of the
    // table types and return types not shown by a body are guesses.
    class CGoalMdl : public CStgMdl {
        public:
            // One animation of the goal (an entry of GetAnmParamTbl; the
            // table ends with id -1). Field meanings guessed from the values.
            struct ANM_PARAM {
                long nId;        // 0x00, index into GetAnmNameTbl
                long nLoop;      // 0x04
                float fSpeed;    // 0x08
                float fFrame;    // 0x0C
                float fBlend;    // 0x10
                long nFlag;      // 0x14
            };

            CGoalMdl(CStgGmk *pParent);
            virtual ~CGoalMdl();
            virtual void Main(float rate);
            virtual void Commit();
            virtual void OnPostCreate();

            // The four inline ones are emitted weak by HockeyF's
            // game/hockey/CGoalMdlHockey.cpp.
            virtual void SetSubType(long type) { m_nSubType = type; } // 0x124
            virtual long GetSubType() const { return m_nSubType; }    // 0x128
            virtual void SetScore(long score);                        // 0x12C
            virtual const void *GetNodeExInfo() const { return 0; }   // 0x130
            // The suffixes of the animation names, ending with null.
            virtual const char *const *GetAnmNameTbl() const;       // 0x134
            virtual const ANM_PARAM *GetAnmParamTbl() const;        // 0x138

        protected:
            // 0x12C, the goal's model variant; it numbers the model names
            // (CGoalObjHockey::NewMdl passes the goal's sub-type on).
            long m_nSubType;
    };
}

#endif /* !MSW_CGOALMDL_HPP_ */
