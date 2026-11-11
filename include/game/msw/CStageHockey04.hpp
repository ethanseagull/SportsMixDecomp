#ifndef MSW_CSTAGEHOCKEY04_HPP_
#define MSW_CSTAGEHOCKEY04_HPP_

#include "game/msw/CStage04Base.hpp"
#include "game/msw/CStageManHockey.hpp"
#include "game/msw/CStgGmk_Pylon_Hockey.hpp"

namespace msw {

    class CStageMdlListNode;

    // Hockey stage 04 (HockeyF), the stage with the cart and the pylons, made
    // by CStageFactoryHockey. Size 0xC64: CStageManHockey (0x218), then
    // CStage04Base at 0x218 and the pylon cluster at 0x248. Unlike
    // DodgeF's stage it has no background node and no scene set-up.
    //
    // GetStageMan and OnSceneLeave are new virtual functions: they come
    // after CStage04Base's slots in the vtable and have no thunks.
    class CStageHockey04 : public CStageManHockey, public CStage04Base {
        public:
            CStageHockey04(bcs::CTask *pParent);
            virtual ~CStageHockey04();

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
            // (CComDrvHockeyGame's Stage04_* functions).
            static long GetPylonMaxNum();
            // Pylon idx if it is active, else 0.
            static const CStgGmk_Pylon *GetPylon(long idx);

        private:
            CStgGmk_PylonCluster_Hockey m_PylonCluster; // 0x248
    };
}

#endif /* !MSW_CSTAGEHOCKEY04_HPP_ */
