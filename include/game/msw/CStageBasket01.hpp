#ifndef MSW_CSTAGEBASKET01_HPP_
#define MSW_CSTAGEBASKET01_HPP_

#include "game/msw/CStage01Base.hpp"
#include "game/msw/CStageManBasket.hpp"

namespace msw {

    // Basket stage 01. Size 0x220: CStageManBasket, then CStage01Base at
    // 0x218. It flags two kinds of models of its table and plays four
    // stage sounds.
    class CStageBasket01 : public CStageManBasket, public CStage01Base {
        public:
            CStageBasket01(bcs::CTask *pParent);
            virtual ~CStageBasket01();

            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                          CStageMdlListNode *pNode, nw4r::g3d::ResFile &rFile, CArn &rArn, long n);
            virtual void OnSoundEvent(long event);
    };
}

#endif /* !MSW_CSTAGEBASKET01_HPP_ */
