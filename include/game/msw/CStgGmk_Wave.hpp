#ifndef MSW_CSTGGMK_WAVE_HPP_
#define MSW_CSTGGMK_WAVE_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmk_Wave_Mdl.hpp"
#include "game/msw/CStgGmk_Wave_Act.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CStgGmk_Wave;
    class CStgGmk_WaveCluster;

    // The driver of a wave. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv). Declared before the wave for the order of the vtables
    // (see CStgGmk_Wave_Mdl.hpp).
    class CStgGmk_Wave_Drv : public CStgDrv {
        public:
            CStgGmk_Wave_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Wave_Drv();
            virtual void Main(float rate);
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
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
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
            virtual long IsActive() const;
            virtual void Inactivate();
            virtual void Sleep(float time);
            virtual bool IsSleep() const;
            virtual float GetSleepTimer() const;
            // Copies of the actor's (returned by value).
            virtual bcs::FVEC3 GetWavePos() const;
            virtual bcs::FVEC3 GetWaveVelocity() const;
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

            // Weak copies at the end of game/msw/CStgGmk_Wave.cpp.
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Wave_Act(this); }
            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Wave_Drv(this); }

        private:
            // The actor is always a CStgGmk_Wave_Act. (name guessed)
            CStgGmk_Wave_Act *GetWaveAct() { return static_cast<CStgGmk_Wave_Act *>(GetAct()); }
            const CStgGmk_Wave_Act *GetWaveAct() const
            {
                return static_cast<const CStgGmk_Wave_Act *>(GetAct());
            }

        private:
            bcs::FVEC3 m_DefaultPos;       // 0x68
            float m_fCycle;                // 0x74
            float m_fAnimTime;             // 0x78
            float m_fBigAnimTime;          // 0x7C
            CStgGmk_WaveCluster *m_pOwner; // 0x80
    };


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
            CStgGmk_Wave **m_ppGmk;  // 0x04, m_nTblSize waves (name guessed)
            long m_nTblSize;         // 0x08, number of gimmicks
            // The two waves on the court, as FindWave reports them.
            bcs::FVEC3 m_WavePos0;   // 0x0C
            bcs::FVEC3 m_WaveVel0;   // 0x18
            bcs::FVEC3 m_WavePos1;   // 0x24
            bcs::FVEC3 m_WaveVel1;   // 0x30
            // 0x3C, frames to the next big wave; negative while none is
            // due (name guessed)
            float m_fBigWaveTimer;
            // Big waves left before items come up, for the normal (0x40)
            // and the big wave (0x44) (names guessed).
            long m_nItemPopCnt;      // 0x40
            long m_nBigItemPopCnt;   // 0x44

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
            virtual long DecItemPopCnt(long type); // returns the count left
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
            // Weak copy after CreateGmk in the DOL; defined below.
            virtual CStgGmk_Wave *NewGmk(long heap) const;
            virtual long GetTblSize() const { return m_nTblSize; }
            virtual CStgGmk_Wave *GetGmk(long idx);
            virtual const CStgGmk_Wave *GetGmk(long idx) const;
            virtual void UpdateWavePos();
            // The wave furthest to -x among those moving towards dir (its
            // velocity's x times dir positive); false when there is none.
            virtual bool FindWave(bcs::FVEC3 &rPos, bcs::FVEC3 &rVel, float dir) const;
            virtual void UpdateWaveTimer();
            virtual void UpdateParam();
            virtual void SetNextBigWaveTimer();
            virtual bool IsNowEnableBigWave() const;
    };

    inline CStgGmk_Wave *CStgGmk_WaveCluster::NewGmk(long heap) const
    {
        return new (heap) CStgGmk_Wave(GetParent());
    }
}

#endif /* !MSW_CSTGGMK_WAVE_HPP_ */
