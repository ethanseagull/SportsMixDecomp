#ifndef MSW_CSTAGEHOCKEY01_HPP_
#define MSW_CSTAGEHOCKEY01_HPP_

#include "game/msw/CStage01Base.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    // Hockey stage 01 (HockeyF), made by CStageFactoryHockey. Size 0x224:
    // CStageManHockey, then CStage01Base at 0x218, then the background
    // model's node at 0x220. The background turns half a turn when the
    // camera looks at the other goal, and two kinds of audience models
    // are switched with the camera.
    class CStageHockey01 : public CStageManHockey, public CStage01Base {
        public:
            CStageHockey01(bcs::CTask *pParent);
            virtual ~CStageHockey01();

            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnActiveCameraChange(long, long nCamera);
            virtual void OnSoundEvent(long event);

            // Shows the audience models flagged 0x20 that go with bOn
            // (those also flagged 0x40) and hides the others, or the reverse.
            virtual void SwitchAudienceMdl_(int bOn);

        private:
            // 0x220 (name guessed): the node of the "stageNN_bg" model.
            CStageMdlListNode *m_pBgNode;
    };
}

#endif /* !MSW_CSTAGEHOCKEY01_HPP_ */
