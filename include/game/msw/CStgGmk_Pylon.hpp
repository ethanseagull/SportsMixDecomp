#ifndef MSW_CSTGGMK_PYLON_HPP_
#define MSW_CSTGGMK_PYLON_HPP_

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStgGmk.hpp"
#include "game/msw/CStgGmk_Pylon_Act.hpp"
#include "game/msw/CStgSrt.hpp"

namespace msw {

    // The model of a pylon (the red cone "s04_redcorn_00"). A DOL class, size
    // 0x130 (HockeyF's CStgGmk_Pylon_Hockey::NewMdl allocates it), vtable
    // lbl_80488E10. The virtual functions after msw::CStgMdl's are in the
    // order of that vtable; HockeyF emits weak copies of SetMdlIdx and
    // GetMdlIdx, so they are inline.
    class CStgGmk_Pylon_Mdl : public CStgMdl {
        public:
            CStgGmk_Pylon_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_Pylon_Mdl();
            virtual void Create();
            // Scales the model to the actor's GetParam_MdlRadius and
            // GetParam_MdlHeight (msw::CStgGmk_Pylon_Act, vtable slots 0x1B4
            // and 0x1B8), then runs CStgMdl::Main. Defined in the DOL right
            // after OnPostCreate (0x80218098).
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual void OnPostCreate();

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; }
            virtual long GetMdlIdx() const { return m_nMdlIdx; }
            // The size of the model at scale 1 (names guessed: Main divides
            // the actor's radius by half the first two and its height by the
            // third). All 1.0 in the DOL (fn_80218230 and the next two).
            virtual float GetParam_OrgWidth() const { return 1.0f; }
            virtual float GetParam_OrgDepth() const { return 1.0f; }
            virtual float GetParam_OrgHeight() const { return 1.0f; }

        private:
            long m_nMdlIdx; // 0x12C, which model of the table (only one)
    };

    // The driver of a pylon. A DOL class, size 0xC (nothing added to
    // msw::CStgDrv).
    class CStgGmk_Pylon_Drv : public CStgDrv {
        public:
            CStgGmk_Pylon_Drv(CStgGmk *pParent);
            virtual ~CStgGmk_Pylon_Drv();
            virtual void Main(float rate);
    };

    // One pylon, the gimmick that CStgGmk_PylonCluster places. A DOL class,
    // size 0x78 (CStgGmk_Pylon_Dodge, allocated at 0x78, adds nothing). The
    // order of the virtual functions follows __vt__Q23msw19CStgGmk_Pylon_Dodge
    // (DodgeF). DodgeF emits weak copies of NewDrv, GetRadius, GetVel,
    // GetPos, GetDefaultPos and SetDefaultPos, in reverse order of the
    // declarations here (so NewDrv, an override with no slot of its own, is
    // declared last). Return types not visible in a body are guesses.
    class CStgGmk_Pylon : public CStgGmk {
        public:
            CStgGmk_Pylon(bcs::CTask *pParent, int);
            virtual ~CStgGmk_Pylon();
            virtual void Main();
            virtual void Disp();
            virtual unsigned long CalcSum() const;
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos) { m_DefaultPos = rPos; }
            virtual const bcs::FVEC3 &GetDefaultPos() const { return m_DefaultPos; }
            virtual void Respawn();
            virtual bool IsActive() const;
            virtual void Inactivate();
            virtual bool IsLanding() const;
            virtual void SetAccel(const bcs::FVEC3 &rAccel);
            // The position of the actor.
            virtual bcs::FVEC3 GetPos() const
            {
                bcs::FVEC3 pos(0.0f, 0.0f, 0.0f);
                pos = GetSrt()->GetPos();
                return pos;
            }
            // The velocity of the actor.
            virtual bcs::FVEC3 GetVel() const
            {
                bcs::FVEC3 vel(0.0f, 0.0f, 0.0f);
                vel = GetSrt()->GetVel();
                return vel;
            }
            virtual float GetRadius() const { return GetPylonAct()->GetParam_Radius(); }

            virtual CStgDrv *NewDrv(long heap) { return new (heap) CStgGmk_Pylon_Drv(this); }

        private:
            // The actor is always a CStgGmk_Pylon_Act. (name guessed)
            const CStgGmk_Pylon_Act *GetPylonAct() const
            {
                return static_cast<const CStgGmk_Pylon_Act *>(GetAct());
            }

        private:
            unsigned long m_Unk68;   // 0x68
            bcs::FVEC3 m_DefaultPos; // 0x6C
    };
}

#endif /* !MSW_CSTGGMK_PYLON_HPP_ */
