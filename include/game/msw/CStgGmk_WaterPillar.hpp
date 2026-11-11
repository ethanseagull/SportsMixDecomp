#ifndef MSW_CSTGGMK_WATERPILLAR_HPP_
#define MSW_CSTGGMK_WATERPILLAR_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmkClusterImpl.hpp"
#include "game/msw/CStgGmk_WaterPillar_Mdl.hpp"
#include "game/msw/CStgGmk_WaterPillar_Act.hpp"
#include "game/msw/CStgSeSlot.hpp"
#include "game/msw/CStgSrt.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CChrCore;

    // The driver of a water pillar. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv).
    class CStgGmk_WaterPillar_Drv : public CStgDrv {
        public:
            CStgGmk_WaterPillar_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_WaterPillar_Drv();
            virtual void Main(float rate);
    };

    // One water pillar. A DOL class, size 0x74, constructor
    // __ct__Q23msw19CStgGmk_WaterPillarFPQ23bcs5CTaski.
    //
    // The order of the virtual functions follows
    // __vt__Q23msw25CStgGmk_WaterWorks_Hockey (HockeyF). The modules emit
    // weak copies of GetMdlNum, GetPos, GetRadius and the default position
    // accessors.
    class CStgGmk_WaterPillar : public CStgGmk {
        public:
            CStgGmk_WaterPillar(bcs::CTask *pParent, int = 0);
            virtual ~CStgGmk_WaterPillar();
            virtual void Main();
            virtual void Disp();
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();
            virtual CStgMdl *NewMdl(long heap, unsigned long idx);

            // The position of the actor; zero without one.
            virtual bcs::FVEC3 GetPos() const
            {
                bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                pos = GetSrt()->GetPos();
                return pos;
            }
            virtual float GetRadius() const { return GetPillarAct()->GetParam_Radius2(); }
            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_DefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual void Respawn();
            virtual void Activate();
            virtual void Inactivate();
            virtual void Start();
            virtual void Stop();
            virtual void Reset();
            virtual bool IsSwoosh() const;
            virtual void SetAuto(int bAuto);
            virtual int IsAuto() const;
            virtual bool IsWarning() const;

            virtual unsigned long GetMdlNum() const { return 1; }

        protected:
            // The actor is always a CStgGmk_WaterPillar_Act. (name guessed)
            CStgGmk_WaterPillar_Act *GetPillarAct()
            {
                return static_cast<CStgGmk_WaterPillar_Act *>(GetAct());
            }
            const CStgGmk_WaterPillar_Act *GetPillarAct() const
            {
                return static_cast<const CStgGmk_WaterPillar_Act *>(GetAct());
            }

        private:
            bcs::FVEC3 m_DefaultPos; // 0x68
    };

    // The water pillars of the fountain stage (stage 03): a cluster of
    // msw::CStgGmk_WaterPillar gimmicks set out in zones around a centre. A
    // DOL class, size 0x44: CStgGmkClusterImpl at 0x00 (vptr at 0x10, by way
    // of CStgGmkCluster<CStgGmk_WaterPillar>), CStgSeSlot at 0x14 (vptr at
    // 0x1C), own members from 0x20. Constructor
    // __ct__Q23msw26CStgGmk_WaterPillarClusterFPQ23bcs5CTask. Each game
    // module derives its own class from it to supply the parameters and the
    // gimmick class.
    //
    // The order of the virtual functions is that of
    // __vt__Q23msw33CStgGmk_WaterPillarCluster_Hockey (HockeyF). The modules
    // emit weak copies of the getters and setters with bodies here and of
    // GetParam_DivNum, IsReady and SE_GetSlotNum. The DOL defaults of
    // GetMaxNum and the GetParam_* functions are the unnamed weak functions
    // of its vtable (lbl_8047F480); the modules override them all. Return
    // types not visible in a body are guesses.
    class CStgGmk_WaterPillarCluster : public CStgGmkCluster<CStgGmk_WaterPillar>, public CStgSeSlot {
        public:
            CStgGmk_WaterPillarCluster(bcs::CTask *pParent);
            virtual ~CStgGmk_WaterPillarCluster();
            virtual long GetMaxNum() const { return 10; }
            virtual void OnPostCreateGmk();
            virtual void OnUpdate(float rate);

            virtual void Reset();
            virtual void StopAll();
            virtual void SetGuardGoalIdx(long idx) { m_nGuardGoalIdx = idx; }
            virtual long GetGuardGoalIdx() const { return m_nGuardGoalIdx; }
            virtual void SetCenterBasePos(const bcs::FVEC3 &rPos) { m_CenterBasePos = rPos; }
            virtual const bcs::FVEC3 &GetCenterBasePos() const { return m_CenterBasePos; }
            virtual bcs::FVEC3 GetCenterPos() const;
            virtual void SetVec(const bcs::FVEC3 &rVec) { m_Vec = rVec; }
            virtual const bcs::FVEC3 &GetVec() const { return m_Vec; }
            virtual bcs::FVEC3 GetZoneCenter() const;
            // The pillars run only while the stage's gimmicks do.
            virtual bool IsReady() const { return CStageMan::GetInstance()->IsGimmickRunning(); }
            virtual long GetZoneByPos(const bcs::FVEC3 &rPos, long curZone, float margin) const;
            // The character of the guarding team that has the ball (the
            // return type is read from the DOL body: a CChrMan character).
            virtual const CChrCore *GetOnBallChr() const;
            virtual float GetParam_LocOffsetZ() const { return 0.0f; }
            virtual float GetParam_LocRadius() const { return 4.0f; }
            virtual long GetParam_LocNum() const { return 9; }
            virtual long GetParam_NumPerGroup() const { return 3; }
            virtual long GetParam_DivNum() const { return 3; }
            virtual float GetParam_ActiveMargin() const { return 1.0f; }
            virtual float GetParam_StopTimer() const { return 180.0f; }
            virtual float GetParam_ZoneAngle() const { return 1.0471976f; }
            virtual float GetParam_ZoneWidth() const { return 0.0f; }
            virtual float GetParam_ZoneMargin() const { return 0.5f; }
            virtual void ApplyLocate();
            virtual void StartZone(long zone);
            virtual bool IsEqualZone(long idx, long zone) const;
            virtual bool IsSwoosh() const;
            virtual long SE_GetSlotNum() const { return 1; }

        private:
            // (name guessed) The gimmick idx as a pillar.
            CStgGmk_WaterPillar *GetPillar(long idx)
            {
                return static_cast<CStgGmk_WaterPillar *>(GetGmk_(idx));
            }
            const CStgGmk_WaterPillar *GetPillar(long idx) const
            {
                return static_cast<const CStgGmk_WaterPillar *>(GetGmk_(idx));
            }

            long m_nGuardGoalIdx;       // 0x20
            bcs::FVEC3 m_CenterBasePos; // 0x24
            bcs::FVEC3 m_Vec;           // 0x30
            long m_nZone;               // 0x3C, the zone whose pillars run; negative while they stop
            float m_fStopTimer;         // 0x40, frames left before the next zone starts
    };
}

#endif /* !MSW_CSTGGMK_WATERPILLAR_HPP_ */
