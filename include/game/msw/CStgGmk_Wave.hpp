#ifndef MSW_CSTGGMK_WAVE_HPP_
#define MSW_CSTGGMK_WAVE_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CStgGmk_Wave;

    // The set of waves of the wave stage (stage 02): makes GetTblSize()
    // msw::CStgGmk_Wave gimmicks through NewGmk and times them. A DOL class,
    // size 0x4C, constructor __ct__Q23msw19CStgGmk_WaveClusterFPQ23bcs5CTask.
    // No base class: the data members come before the first virtual
    // function, so the vptr is at 0x48. Each game module derives its own
    // class from it to supply the parameters and the gimmick class.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw25CStgGmk_WaveCluster_Dodge (DodgeF). The DOL has unnamed
    // copies of GetParent, GetMaxNum, GetWavePos, NewGmk, GetTblSize and the
    // GetParam_* functions a module class overrides, so those are inline;
    // bodies are given for the ones the modules emit.
    class CStgGmk_WaveCluster {
        private:
            bcs::CTask *m_pParent;   // 0x00
            unsigned long m_Unk04;   // 0x04
            long m_nTblSize;         // 0x08, number of gimmicks
            // The two waves on the court, as FindWave reports them.
            bcs::FVEC3 m_WavePos0;   // 0x0C
            bcs::FVEC3 m_WaveVel0;   // 0x18
            bcs::FVEC3 m_WavePos1;   // 0x24
            bcs::FVEC3 m_WaveVel1;   // 0x30
            float m_Unk3C;           // 0x3C
            unsigned long m_Unk40;   // 0x40
            unsigned long m_Unk44;   // 0x44

        public:
            CStgGmk_WaveCluster(bcs::CTask *pParent);
            virtual ~CStgGmk_WaveCluster();
            virtual bcs::CTask *GetParent() const { return m_pParent; }
            virtual long GetMaxNum() const { return 2; }
            virtual void CreateGmk();
            virtual void DeleteGmk();
            virtual void Main(float rate);
            virtual void Reset();
            virtual void TimerReset();
            virtual unsigned long CalcSum() const;
            // The position of the wave with the smaller x.
            virtual const bcs::FVEC3 &GetWavePos() const
            {
                return m_WavePos0.x < m_WavePos1.x ? m_WavePos0 : m_WavePos1;
            }
            // Returned by value (the DOL body fills the caller's copy).
            virtual bcs::FVEC3 GetWaveVelocityByPos(const bcs::FVEC3 &rPos) const;
            virtual CStgGmk_Wave *GetNewWave();
            virtual const CStgGmk_Wave *GetNewWave() const;
            virtual CStgGmk_Wave *GetOldWave();
            virtual const CStgGmk_Wave *GetOldWave() const;
            virtual CStgGmk_Wave *GetNextWave(const CStgGmk_Wave *pWave);
            virtual const CStgGmk_Wave *GetNextWave(const CStgGmk_Wave *pWave) const;
            virtual long GetItemPopCnt(long type) const;
            virtual void DecItemPopCnt(long type);
            virtual void ResetItemPopCnt(long type);
            virtual float GetParam_Cycle() const;
            virtual float GetParam_PartCycle() const;
            virtual float GetParam_BigWaveCycle() const;
            virtual float GetParam_BigWaveCycleRnd() const;
            virtual float GetParam_OffsetX() const;
            virtual float GetParam_BigWaveOffsetX() const;
            virtual float GetParam_AnimTime() const;
            virtual float GetParam_BigAnimTime() const;
            virtual float GetParam_PartAnimTime() const;
            virtual float GetParam_BigPartAnimTime() const;
            virtual long GetParam_PopItemCycle(long type) const;
            virtual CStgGmk_Wave *NewGmk(long heap) const;
            virtual long GetTblSize() const { return m_nTblSize; }
            virtual CStgGmk_Wave *GetGmk(long idx);
            virtual const CStgGmk_Wave *GetGmk(long idx) const;
            virtual void UpdateWavePos();
            virtual void FindWave(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float) const;
            virtual void UpdateWaveTimer();
            virtual void UpdateParam();
            virtual void SetNextBigWaveTimer();
            virtual bool IsNowEnableBigWave() const;
    };

    // One wave of the wave stage. A DOL class, size 0x84, constructor
    // __ct__Q23msw12CStgGmk_WaveFPQ23bcs5CTask.
    //
    // The order of the virtual functions follows
    // __vt__Q23msw18CStgGmk_Wave_Dodge (DodgeF). The modules emit weak copies
    // of the owner, default position and GetParam_/SetParam_ accessors, in
    // reverse order of the declarations here.
    class CStgGmk_Wave : public CStgGmk {
        public:
            CStgGmk_Wave(bcs::CTask *pParent);
            virtual ~CStgGmk_Wave();
            virtual void Main();
            virtual void Disp();
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();
            virtual unsigned long OnCalcSum() const;

            virtual void SetOwner(CStgGmk_WaveCluster *pOwner) { m_pOwner = pOwner; }
            virtual CStgGmk_WaveCluster *GetOwner() { return m_pOwner; }
            virtual const CStgGmk_WaveCluster *GetOwner() const { return m_pOwner; }
            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_DefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual bool IsActive() const;
            virtual void Inactivate();
            virtual void Sleep(float time);
            virtual bool IsSleep() const;
            virtual float GetSleepTimer() const;
            virtual const bcs::FVEC3 &GetWavePos() const;
            virtual const bcs::FVEC3 &GetWaveVelocity() const;
            virtual float GetAge() const;
            virtual long GetWaveType() const;
            virtual void SetNextWaveType(long type);
            virtual void Reset();
            virtual float GetParam_Cycle() const { return m_fCycle; }
            virtual void SetParam_Cycle(float cycle) { m_fCycle = cycle; }
            virtual float GetParam_AnimTime() const { return m_fAnimTime; }
            virtual float GetParam_BigAnimTime() const { return m_fBigAnimTime; }
            virtual void SetParam_AnimTime(float time) { m_fAnimTime = time; }
            virtual void SetParam_BigAnimTime(float time) { m_fBigAnimTime = time; }

        private:
            bcs::FVEC3 m_DefaultPos;       // 0x68
            float m_fCycle;                // 0x74
            float m_fAnimTime;             // 0x78
            float m_fBigAnimTime;          // 0x7C
            CStgGmk_WaveCluster *m_pOwner; // 0x80
    };

    // The driver and the model of a wave. DOL classes; only the constructors
    // the modules call are declared.
    class CStgGmk_Wave_Drv : public CStgDrv {
        public:
            CStgGmk_Wave_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Wave_Drv();
    };

    class CStgGmk_Wave_Mdl : public CStgMdl {
        public:
            CStgGmk_Wave_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_Wave_Mdl();

        private:
            long m_nIdx; // 0x12C (name guessed)
    };
}

#endif /* !MSW_CSTGGMK_WAVE_HPP_ */
