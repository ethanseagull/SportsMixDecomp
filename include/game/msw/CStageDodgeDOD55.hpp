#ifndef MSW_CSTAGEDODGEDOD55_HPP_
#define MSW_CSTAGEDODGEDOD55_HPP_

#include "game/msw/CStageManDodgeDodgeOrDie.hpp"

namespace msw {

    // Dodge or Die stage 55. Size 0x244: nothing beyond its base. It flags
    // two kinds of models of its table, flashes the audience on entering a
    // scene and plays one stage sound.
    class CStageDodgeDOD55 : public CStageManDodgeDodgeOrDie {
        public:
            CStageDodgeDOD55(bcs::CTask *pParent);
            virtual ~CStageDodgeDOD55();

            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnSceneReentry();
            virtual void OnSoundEvent(long event);
    };
}

#endif /* !MSW_CSTAGEDODGEDOD55_HPP_ */
