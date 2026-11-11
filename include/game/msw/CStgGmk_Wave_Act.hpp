#ifndef MSW_CSTGGMK_WAVE_ACT_HPP_
#define MSW_CSTGGMK_WAVE_ACT_HPP_

#include "bcs/CFsmQueue.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmkAct.hpp"

namespace msw {

    class CStgGmk;

    // The actor of one wave of the wave stage. A DOL class, size 0x208:
    // CStgGmkAct at 0x000 (vptrs at 0x008 and 0x014), its state queue
    // bcs::CFsmQueue at 0x174 (vptr at 0x1D0), own members from 0x1D4.
    // Constructor __ct__Q23msw16CStgGmk_Wave_ActFPQ23msw7CStgGmk. Each game
    // module derives its own class from it to supply the parameters.
    //
    // The order of the new virtual functions (GetAge to DtorState_WAVE)
    // follows __vt__Q23msw22CStgGmk_Wave_Dodge_Act (DodgeF); they are reached
    // through the vptr at 0x008, after the CFsmQueue part. SE_GetSlotNum
    // overrides CStgSeSlot's, so it also gets a slot of its own where it is
    // declared. The modules emit weak copies of the accessors with bodies
    // here, in reverse order of these declarations; the DOL also has
    // unnamed defaults of the GetParam_* functions a module class overrides.
    class CStgGmk_Wave_Act : public CStgGmkAct, public bcs::CFsmQueue<CStgGmk_Wave_Act> {
        public:
            // Returned by GetWavePos and GetWaveVelocity while there is no
            // wave.
            static bcs::FVEC3 vInvalidWavePos;
            static bcs::FVEC3 vInvalidWaveVel;

            // The states of the queue, in the order of the state hooks.
            enum STATE {
                STATE_DEFAULT,
                STATE_STANDBY, // asleep (Sleep), hidden
                STATE_WAVE     // the wave's animation runs
            };

            CStgGmk_Wave_Act(CStgGmk *pParent);
            virtual ~CStgGmk_Wave_Act();

            virtual void UpdateMdlMtx();
            virtual void OnInit();
            virtual void OnUpdate(float rate);

            virtual float GetAge() const { return m_fAge; }
            virtual long IsAlive() const { return m_bAlive; }
            virtual long GetWaveType() const { return m_nWaveType; }
            virtual void SetNextWaveType(long type) { m_nNextWaveType = type; }
            virtual long GetNextWaveType() const { return m_nNextWaveType; }
            virtual void Sleep(float time);
            virtual bool IsSleep() const;
            virtual float GetSleepTimer() const;
            virtual const bcs::FVEC3 &GetWavePos() const
            {
                if (m_bWave != 0) {
                    return m_WavePos;
                }
                return vInvalidWavePos;
            }
            virtual const bcs::FVEC3 &GetWaveVelocity() const
            {
                if (m_bWave != 0) {
                    return m_WaveVel;
                }
                return vInvalidWaveVel;
            }
            virtual bool IsBigWaveReady() const;
            virtual void Reset();
            virtual float GetParam_Interval() const;
            virtual float GetParam_Cycle() const;
            virtual float GetParam_AnimTime() const;
            virtual float GetParam_BigAnimTime() const;
            virtual long GetParam_Effect(long type) const;
            virtual long GetParam_SE(long type) const;
            virtual long GetParam_PopCoinNum(long type) const;
            virtual void GetParam_CourtRect(float &rLeft, float &rRight, float &rFront, float &rBack) const;
            // How far along x from the wave the coins come up.
            virtual float GetParam_CoinPopPosOfs() const;
            virtual long GetParam_PopItemNum(long type) const;
            virtual float GetParam_PopItemDelay(long type) const;
            virtual float GetParam_PopItemComeFrame(long type) const;
            virtual float GetParam_PopItemPopDist(long type) const;
            virtual float GetParam_ItemPopPosX(long type) const;
            virtual long Eff_GetSlotNum() const { return 1; }
            virtual long SE_GetSlotNum() const { return 1; }
            virtual void UpdateWavePos(float rate);
            virtual void DoPopCoin();
            virtual void DoPopItem();
            virtual void CtorState_DEFAULT();
            virtual void LoopState_DEFAULT();
            virtual void DtorState_DEFAULT();
            virtual void CtorState_STANDBY();
            virtual void LoopState_STANDBY();
            virtual void DtorState_STANDBY();
            virtual void CtorState_WAVE();
            virtual void LoopState_WAVE();
            virtual void DtorState_WAVE();

        private:
            float m_fAge;            // 0x1D4
            float m_fSleepTimer;     // 0x1D8, the frames given to Sleep (name guessed)
            long m_bAlive;           // 0x1DC
            long m_nWaveType;        // 0x1E0
            long m_nNextWaveType;    // 0x1E4
            long m_bWave;            // 0x1E8, not zero while m_WavePos/m_WaveVel are valid (name guessed)
            bcs::FVEC3 m_WavePos;    // 0x1EC
            bcs::FVEC3 m_WaveVel;    // 0x1F8
            // 0x204, the model's node "n_atari" the wave is at; -1 without
            // one (name guessed)
            long m_nAtariNode;
    };
}

#endif /* !MSW_CSTGGMK_WAVE_ACT_HPP_ */
