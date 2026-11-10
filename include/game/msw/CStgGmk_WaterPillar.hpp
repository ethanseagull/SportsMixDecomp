#ifndef MSW_CSTGGMK_WATERPILLAR_HPP_
#define MSW_CSTGGMK_WATERPILLAR_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmkClusterImpl.hpp"
#include "game/msw/CStgGmk_WaterPillar_Act.hpp"
#include "game/msw/CStgSeSlot.hpp"
#include "game/msw/CStgSrt.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CChr;

    // The water pillars of the fountain stage (stage 03): a cluster of
    // msw::CStgGmk_WaterPillar gimmicks set out in zones around a centre. A
    // DOL class, size 0x44: CStgGmkClusterImpl at 0x00 (vptr at 0x10),
    // CStgSeSlot at 0x14 (vptr at 0x1C), own members from 0x20. Constructor
    // __ct__Q23msw26CStgGmk_WaterPillarClusterFPQ23bcs5CTask. Each game
    // module derives its own class from it to supply the parameters and the
    // gimmick class.
    //
    // The order of the virtual functions is that of
    // __vt__Q23msw33CStgGmk_WaterPillarCluster_Hockey (HockeyF). The modules
    // emit weak copies of the getters and setters with bodies here and of
    // GetParam_DivNum, IsReady and SE_GetSlotNum; the GetParam_* functions
    // a module class overrides have no DOL default with a name, so they are
    // declared pure. Return types not visible in a body are guesses.
    class CStgGmk_WaterPillarCluster : public CStgGmkClusterImpl, public CStgSeSlot {
        public:
            CStgGmk_WaterPillarCluster(bcs::CTask *pParent);
            virtual ~CStgGmk_WaterPillarCluster();
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
            virtual long GetZoneByPos(const bcs::FVEC3 &rPos, long, float) const;
            virtual CChr *GetOnBallChr() const;
            virtual float GetParam_LocOffsetZ() const = 0;
            virtual float GetParam_LocRadius() const = 0;
            virtual long GetParam_LocNum() const = 0;
            virtual long GetParam_NumPerGroup() const = 0;
            virtual long GetParam_DivNum() const { return 3; }
            virtual float GetParam_ActiveMargin() const = 0;
            virtual float GetParam_StopTimer() const = 0;
            virtual float GetParam_ZoneAngle() const = 0;
            virtual float GetParam_ZoneWidth() const = 0;
            virtual float GetParam_ZoneMargin() const = 0;
            virtual void ApplyLocate();
            virtual void StartZone(long zone);
            virtual bool IsEqualZone(long zone0, long zone1) const;
            virtual bool IsSwoosh() const;
            virtual long SE_GetSlotNum() const { return 1; }

        private:
            long m_nGuardGoalIdx;       // 0x20
            bcs::FVEC3 m_CenterBasePos; // 0x24
            bcs::FVEC3 m_Vec;           // 0x30
            unsigned long m_Unk3C;      // 0x3C
            float m_Unk40;              // 0x40
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
            CStgGmk_WaterPillar(bcs::CTask *pParent, int);
            virtual ~CStgGmk_WaterPillar();
            virtual void Main();
            virtual void Disp();
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

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

        private:
            // The actor is always a CStgGmk_WaterPillar_Act. (name guessed)
            const CStgGmk_WaterPillar_Act *GetPillarAct() const
            {
                return static_cast<const CStgGmk_WaterPillar_Act *>(GetAct());
            }

            bcs::FVEC3 m_DefaultPos; // 0x68
    };

    // The driver of a water pillar. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv).
    class CStgGmk_WaterPillar_Drv : public CStgDrv {
        public:
            CStgGmk_WaterPillar_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_WaterPillar_Drv();
            virtual void Main(float rate);
    };

    // The model of a water pillar. A DOL class, size 0x130. GetParam_MdlHeight
    // has no DOL default with a name, so it is declared pure.
    class CStgGmk_WaterPillar_Mdl : public CStgMdl {
        public:
            CStgGmk_WaterPillar_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_WaterPillar_Mdl();
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; }
            virtual long GetMdlIdx() const { return m_nMdlIdx; }
            virtual float GetParam_MdlHeight() const = 0;

        private:
            long m_nMdlIdx; // 0x12C, which model of the module's table
    };
}

#endif /* !MSW_CSTGGMK_WATERPILLAR_HPP_ */
