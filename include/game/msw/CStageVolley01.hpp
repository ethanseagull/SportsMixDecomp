#ifndef MSW_CSTAGEVOLLEY01_HPP_
#define MSW_CSTAGEVOLLEY01_HPP_

#include "game/msw/CStage01Base.hpp"
#include "game/msw/CStageManVolley.hpp"

namespace msw {

    // Volley stage 01 (VolleyF), made by CStageFactoryVolley for stages 1
    // and 25. Size 0x208: CStageManVolley (0x200), then CStage01Base at 0x200. It flags two kinds
    // of model, makes the crowd react to the game and plays the stage's
    // sounds in five sound slots.
    class CStageVolley01 : public CStageManVolley, public CStage01Base {
        public:
            CStageVolley01(bcs::CTask *pParent);
            virtual ~CStageVolley01();

            virtual void SendMessage(long msg, long, long, long);
            virtual void Main_(float);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const { return 5; }
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
                                          nw4r::g3d::ResFile &, CArn &, long);
            virtual void OnSoundEvent(long event);
    };
}

#endif /* !MSW_CSTAGEVOLLEY01_HPP_ */
