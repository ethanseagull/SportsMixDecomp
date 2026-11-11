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
    // The virtual functions follow __vt__Q23msw26CStgGmk_Trolley_Mdl_Volley
    // (VolleyF); return types of the DOL ones are guesses.
    class CStgGmk_Trolley_Mdl : public CStgMdl {
        public:
            CStgGmk_Trolley_Mdl(CStgGmk *pParent, long idx);
            virtual ~CStgGmk_Trolley_Mdl();
            virtual void Create();
            virtual void DeleteMdl();
            virtual void Main(float rate);
            virtual void SetVisible(int bVisible);
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual void OnPostCreate();

            // (weak in VolleyF's CStgGmk_Trolley_Act_Volley.cpp)
            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; }
            virtual void SetMdlType(long type) { m_Unk130 = type; }
            virtual long GetMdlType() const { return m_Unk130; }
            virtual void SetHeyhoParam(int, float, float);
            virtual float GetParam_MdlWidth() const;
            virtual float GetParam_MdlHeight() const;
            virtual float GetParam_MdlDepth() const;
            virtual long GetMdlIdx() const { return m_nMdlIdx; }
            virtual void CreateHeyho();
            virtual void DeleteHeyho();
            virtual void DispHeyho(int bDisp);
            virtual void *GetHeyho();

        private:
            long m_nMdlIdx;          // 0x12C (GetMdlIdx)
            long m_Unk130;           // 0x130, the model type (GetMdlType): 2 for every model but the first, which has 0
            void *m_pUnk134;         // 0x134, null until CreateHeyho
            float m_fUnk138;         // 0x138
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_MDL_HPP_ */
