#ifndef MSW_CSTGGMK_TROLLEY_MDL_HPP_
#define MSW_CSTGGMK_TROLLEY_MDL_HPP_

#include "game/msw/CStgMdl.hpp"

namespace msw {

    class CStgGmk;

    // (purpose guessed from the names) A model of the trolley of stage 17
    // (msw::CStgGmk_Trolley), with the "heyho" riders (CreateHeyho,
    // DispHeyho, ...). A DOL class, size 0x13C, vtable lbl_80489960:
    // CStgMdl at 0x000, own members from 0x12C. Constructor
    // __ct__Q23msw19CStgGmk_Trolley_MdlFPQ23msw7CStgGmkl.
    //
    // Only the constructor and destructor are declared so far; the other
    // virtual functions (Create, NewMdl, OnPostCreate, DeleteMdl, Main,
    // SetVisible and the new Heyho ones) wait for the model's source.
    class CStgGmk_Trolley_Mdl : public CStgMdl {
        public:
            CStgGmk_Trolley_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_Trolley_Mdl();

        private:
            long m_nMdlIdx;          // 0x12C (GetMdlIdx)
            long m_Unk130;           // 0x130, 2 for every model but the first, which has 0
            void *m_pUnk134;         // 0x134, null until CreateHeyho
            float m_fUnk138;         // 0x138
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_MDL_HPP_ */
