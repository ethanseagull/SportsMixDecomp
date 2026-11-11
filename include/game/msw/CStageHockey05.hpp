#ifndef MSW_CSTAGEHOCKEY05_HPP_
#define MSW_CSTAGEHOCKEY05_HPP_

#include "game/msw/CStageManHockey.hpp"
#include "game/msw/CStageScroll.hpp"

namespace msw {

    // Hockey stage 05 (HockeyF), made by CStageFactoryHockey. Size 0xA94:
    // CStageManHockey (0x218), then a scrolling background (CStageScroll,
    // a DOL class) at 0x218. The stage's models are turned with the scroll
    // each frame; its "null_wavelog" models are floating ones
    // (CStageMdl_Raft).
    //
    // Overrides are declared in the order of their vtable slots.
    class CStageHockey05 : public CStageManHockey {
        public:
            CStageHockey05(bcs::CTask *pParent);
            virtual ~CStageHockey05();

            virtual CStageScroll *GetScroll() { return &m_Scroll; }
            virtual const CStageScroll *GetScroll() const { return &m_Scroll; }
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                        nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnSceneEntry();

            // The factory of the "null_wavelog" models.
            static CStageMdl *NewMdl_Raft(CStageMan *pStage, long n, bcs::CRegex &rRegex);

        private:
            CStageScroll m_Scroll; // 0x218
    };
}

#endif /* !MSW_CSTAGEHOCKEY05_HPP_ */
