#ifndef MSW_CSTGGMK_WAVE_HOCKEY_HPP_
#define MSW_CSTGGMK_WAVE_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Wave.hpp"
#include "game/msw/CStgGmk_Wave_Act.hpp"

namespace msw {

    // The wave stage gimmick of the hockey mode: the actor, the wave and
    // the cluster with the HOCKEY_PRM_S02_* parameters, all three in
    // game/hockey/CStgGmk_Wave_Hockey.cpp. The classes are declared in the
    // reverse order of their vtables in that file's .data.
    //
    // The parameter getters are the only out-of-line functions. The
    // destructors and the factories (NewGmk, NewMdl, NewAct, NewDrv) are
    // inline: the original file emits each of them inside its class's group
    // of weak inline functions, after the base class's inline virtuals and
    // in reverse order of the declarations here, not where an out-of-line
    // definition would put them (HockeyF's copy of the file shows the same).

    // The actor, size 0x208 (nothing added to msw::CStgGmk_Wave_Act).
    class CStgGmk_Wave_Hockey_Act : public CStgGmk_Wave_Act {
        public:
            CStgGmk_Wave_Hockey_Act(CStgGmk *pParent) : CStgGmk_Wave_Act(pParent) {}
            virtual ~CStgGmk_Wave_Hockey_Act() {}

            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_PopCoinNum(long type) const;
            virtual void GetParam_CourtRect(float &rLeft, float &rRight, float &rFront, float &rBack) const;
            virtual long GetParam_PopItemNum(long type) const;
            virtual float GetParam_PopItemDelay(long type) const;
            virtual float GetParam_PopItemComeFrame(long type) const;
            virtual float GetParam_PopItemPopDist(long type) const;
            virtual float GetParam_ItemPopPosX(long type) const;
    };

    // One wave, size 0x84 (nothing added to msw::CStgGmk_Wave).
    class CStgGmk_Wave_Hockey : public CStgGmk_Wave {
        public:
            CStgGmk_Wave_Hockey(bcs::CTask *pParent) : CStgGmk_Wave(pParent) {}
            virtual ~CStgGmk_Wave_Hockey() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx)
            {
                return new (heap) CStgGmk_Wave_Mdl(this, idx);
            }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Wave_Hockey_Act(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Wave_Drv(this); }
    };

    // The cluster, size 0x4C (nothing added to msw::CStgGmk_WaveCluster).
    // Made by msw::CStageHockey02::NewWaveCluster.
    class CStgGmk_WaveCluster_Hockey : public CStgGmk_WaveCluster {
        public:
            CStgGmk_WaveCluster_Hockey(bcs::CTask *pParent) : CStgGmk_WaveCluster(pParent) {}
            virtual ~CStgGmk_WaveCluster_Hockey() {}

            virtual bool IsNowEnableBigWave() const;

            virtual float GetParam_PartCycle() const;
            virtual float GetParam_BigWaveCycle() const;
            virtual float GetParam_BigWaveCycleRnd() const;
            virtual float GetParam_OffsetX() const;
            virtual float GetParam_BigWaveOffsetX() const;
            virtual float GetParam_PartAnimTime() const;
            virtual float GetParam_BigPartAnimTime() const;
            virtual long GetParam_PopItemCycle(long type) const;
            virtual CStgGmk_Wave *NewGmk(long heap) const
            {
                return new (heap) CStgGmk_Wave_Hockey(GetParent());
            }
    };
}

#endif /* !MSW_CSTGGMK_WAVE_HOCKEY_HPP_ */
