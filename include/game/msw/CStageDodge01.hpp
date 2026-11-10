#ifndef MSW_CSTAGEDODGE01_HPP_
#define MSW_CSTAGEDODGE01_HPP_

#include "game/msw/CStage01Base.hpp"
#include "game/msw/CStageManDodge.hpp"

namespace msw {

    // Dodge stage 01 (DodgeF), made by CStageFactoryDodge. Size 0x24C:
    // CStageManDodge (0x244), then CStage01Base at 0x244. It flags two kinds
    // of model, makes the crowd react to the game and plays the stage's
    // sounds in five sound slots.
    class CStageDodge01 : public CStageManDodge, public CStage01Base {
        public:
            CStageDodge01(bcs::CTask *pParent);
            virtual ~CStageDodge01();

            virtual void SendMessage(long msg, long, long, long);
            virtual void Main_(float);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const { return 5; }
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
                                          nw4r::g3d::ResFile &, CArn &, long);
            virtual void OnSoundEvent(long event);
    };
}

#endif /* !MSW_CSTAGEDODGE01_HPP_ */
