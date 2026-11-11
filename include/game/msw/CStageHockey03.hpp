#ifndef MSW_CSTAGEHOCKEY03_HPP_
#define MSW_CSTAGEHOCKEY03_HPP_

#include "game/msw/CStageManHockey.hpp"
#include "game/msw/CStgGmk_WaterWorks_Hockey.hpp"

namespace msw {

    // Basket stage 03, the fountain stage (BasketF), made by
    // CStageFactoryHockey. Size 0x2B8: CStageManHockey (0x218), then one
    // cluster of water pillars in front of each goal. A pillar cluster
    // stops while the team that owns its goal is not attacking it.
    // HockeyF's CStageHockey03 is the same stage on its own stage manager.
    //
    // Overrides are declared in the order of their vtable slots; the three
    // Water_* functions are new slots at the end of
    // __vt__Q23msw14CStageHockey03.
    class CStageHockey03 : public CStageManHockey {
        public:
            CStageHockey03(bcs::CTask *pParent);
            virtual ~CStageHockey03();

            virtual void Disp();
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                        nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPostInitialize();
            virtual unsigned long OnCalcSum() const;
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual void OnSoundEvent(long event);

            // Runs both clusters for one frame, stopping the ones whose goal
            // is not attacked.
            virtual void Water_Update(float rate);
            virtual void Water_StopAll();
            virtual void Water_ResetAll();

            // The number of pillars of the running stage (taken to be a
            // CStageHockey03), and its pillar `idx`, counted over both
            // clusters, if it is gushing; 0 otherwise.
            static long GetWaterPillarMaxNum();
            static CStgGmk_WaterPillar *GetWaterPillar(long idx);

        private:
            CStgGmk_WaterPillarCluster_Hockey m_Cluster0; // 0x218, at goal 0
            CStgGmk_WaterPillarCluster_Hockey m_Cluster1; // 0x25C, at goal 1
            CStgGmk_WaterPillarCluster *m_apCluster[2];   // 0x2A0, the two above
            float m_fUnk2A8;                              // 0x2A8, zeroed by the gimmick hooks, not read here
            long m_nUnk2AC;                               // 0x2AC, -1 at reset, not read here
            long m_nUnk2B0;                               // 0x2B0, 0, not read here
            // 0x2B4, set by message 0x27; -1 for none. Its guarded goal
            // keeps its pillars stopped. (meaning guessed)
            long m_nGuardTeam;
    };
}

#endif /* !MSW_CSTAGEHOCKEY03_HPP_ */
