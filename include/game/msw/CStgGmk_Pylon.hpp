#ifndef MSW_CSTGGMK_PYLON_HPP_
#define MSW_CSTGGMK_PYLON_HPP_

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
}

#endif /* !MSW_CSTGGMK_PYLON_HPP_ */
