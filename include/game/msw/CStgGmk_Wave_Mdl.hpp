#ifndef MSW_CSTGGMK_WAVE_MDL_HPP_
#define MSW_CSTGGMK_WAVE_MDL_HPP_

#include "bcs/CState.hpp"
#include "game/msw/CStgMdl.hpp"

namespace msw {

    class CStgGmk;

    // The model of a wave ("s02_ef_sea01_*"). A DOL class, size 0x130
    // (constructor __ct__Q23msw16CStgGmk_Wave_MdlFPQ23msw7CStgGmkl). Create,
    // Main and NewMdl have DOL definitions without exported names, the
    // slots of the vtable after msw::CStgMdl's; SetMdlIdx and GetMdlIdx are
    // weak copies at the end of game/msw/CStgGmk_Wave.cpp, so inline.
    //
    // Declared before the actor and the other wave classes: the DOL file
    // emits the vtables in the reverse order of the class declarations,
    // and has this one last.
    class CStgGmk_Wave_Mdl : public CStgMdl {
        public:
            CStgGmk_Wave_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_Wave_Mdl();
            virtual void Create();
            // Draws the wave in front of the others the further its
            // animation has run.
            virtual void Main(float rate);
            virtual bcs::CMdlObj *NewMdl(long heap) const;

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; }
            virtual long GetMdlIdx() const { return m_nMdlIdx; }

        private:
            long m_nMdlIdx; // 0x12C, which model of the table (0 to 2)
    };
}

#endif /* !MSW_CSTGGMK_WAVE_MDL_HPP_ */
