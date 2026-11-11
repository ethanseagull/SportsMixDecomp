#ifndef MSW_CSTGGMK_WATERPILLAR_MDL_HPP_
#define MSW_CSTGGMK_WATERPILLAR_MDL_HPP_

#include "bcs/CState.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    class CStgGmk;

    // The model of a water pillar. A DOL class, size 0x130. Create and
    // GetParam_MdlHeight have DOL defaults without exported names
    // (fn_80190D9C and fn_8019101C, the vtable slots of
    // __vt__Q23msw23CStgGmk_WaterPillar_Mdl).
    //
    // Declared before the actor and the other pillar classes (and after
    // bcs::CState): game/msw/CStgGmk_WaterPillar.cpp emits the vtables in
    // the reverse order of the class declarations, and the DOL has this one
    // after the actor's.
    class CStgGmk_WaterPillar_Mdl : public CStgMdl {
        public:
            CStgGmk_WaterPillar_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_WaterPillar_Mdl();
            virtual void Create();
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; }
            virtual long GetMdlIdx() const { return m_nMdlIdx; }
            virtual float GetParam_MdlHeight() const;

        private:
            long m_nMdlIdx; // 0x12C, which model of the module's table
    };
}

#endif /* !MSW_CSTGGMK_WATERPILLAR_MDL_HPP_ */
