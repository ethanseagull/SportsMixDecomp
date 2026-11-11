#ifndef MSW_CSTGGMK_PYLON_HPP_
#define MSW_CSTGGMK_PYLON_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStgGmk.hpp"

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

    // One pylon, the gimmick that CStgGmk_PylonCluster places. A DOL class,
    // size 0x78 (CStgGmk_Pylon_Dodge, allocated at 0x78, adds nothing). The
    // order of the virtual functions follows __vt__Q23msw19CStgGmk_Pylon_Dodge
    // (DodgeF). DodgeF emits weak copies of NewDrv, GetRadius, GetVel, GetPos
    // and GetDefaultPos, so those are inline in the original; their bodies
    // are not written yet. Return types not visible in a body are guesses.
    class CStgGmk_Pylon : public CStgGmk {
        public:
            CStgGmk_Pylon(bcs::CTask *pParent, int);
            virtual ~CStgGmk_Pylon();
            virtual void Main();
            virtual void Disp();
            virtual unsigned long CalcSum() const;
            virtual CStgDrv *NewDrv(long heap);
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();

            virtual void SetDefaultPos(const bcs::FVEC3 &rPos);
            virtual const bcs::FVEC3 &GetDefaultPos() const;
            virtual void Respawn();
            virtual bool IsActive() const;
            virtual void Inactivate();
            virtual bool IsLanding() const;
            virtual void SetAccel(const bcs::FVEC3 &rAccel);
            virtual bcs::FVEC3 GetPos() const;
            virtual bcs::FVEC3 GetVel() const;
            virtual float GetRadius() const;

        private:
            unsigned long m_Unk68;   // 0x68
            bcs::FVEC3 m_DefaultPos; // 0x6C
    };
}

#endif /* !MSW_CSTGGMK_PYLON_HPP_ */
