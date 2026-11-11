#ifndef MSW_CSTAGEDODGE04_HPP_
#define MSW_CSTAGEDODGE04_HPP_

#include "game/msw/CStage04Base.hpp"
#include "game/msw/CStageManDodge.hpp"
#include "game/msw/CStgGmk_Pylon_Dodge.hpp"

namespace msw {

    class CStageMdlListNode;

    // Dodge stage 04 (DodgeF), the stage with the cart and the pylons, made
    // by CStageFactoryDodge. Size 0xC94: CStageManDodge (0x244), then
    // CStage04Base at 0x244, the background model's node at 0x274 and the
    // pylon cluster at 0x278. The same stage is HockeyF's CStageHockey04.
    //
    // GetStageMan and OnSceneLeave are new virtual functions: they come
    // after CStage04Base's slots in the vtable and have no thunks.
    class CStageDodge04 : public CStageManDodge, public CStage04Base {
        public:
            CStageDodge04(bcs::CTask *pParent);
            virtual ~CStageDodge04();

            virtual void Disp();
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &, CStageMdlListNode *pNode,
                                          nw4r::g3d::ResFile &, CArn &, long);
            virtual void OnSceneEntry();
            virtual void OnTurnInit();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStop();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            virtual const CStageMan *GetStageMan() const;
            virtual void OnSceneLeave();

            // The pylons of the running stage, which must be this one
            // (CComDrvDodgeGame's Stage04_* functions).
            static long GetPylonMaxNum();
            // Pylon idx if it is active, else 0.
            static const CStgGmk_Pylon *GetPylon(long idx);

        private:
            CStageMdlListNode *m_pBgNode;              // 0x274, "stage<n>_bg" (name guessed)
            CStgGmk_PylonCluster_Dodge m_PylonCluster; // 0x278
    };
}

#endif /* !MSW_CSTAGEDODGE04_HPP_ */
