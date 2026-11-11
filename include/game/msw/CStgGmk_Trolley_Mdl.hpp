#ifndef MSW_CSTGGMK_TROLLEY_MDL_HPP_
#define MSW_CSTGGMK_TROLLEY_MDL_HPP_

#include "game/msw/CStgMdl.hpp"

namespace msw {

    class CNpcHeyho;
    class CStgGmk;

    // (purpose guessed from the names) A car of the trolley of stage 17
    // (msw::CStgGmk_Trolley), with a "heyho" riding it (CreateHeyho,
    // DispHeyho, ...). A DOL class, size 0x13C, vtable lbl_80489960:
    // CStgMdl at 0x000, own members from 0x12C. Constructor
    // __ct__Q23msw19CStgGmk_Trolley_MdlFPQ23msw7CStgGmkl.
    //
    // The names of the new virtual functions are those of VolleyF's weak
    // copies (CStgGmk_Trolley_Mdl_Volley overrides the three
    // GetParam_Mdl* sizes); the inline ones are emitted after their first
    // caller in game/msw/CStgGmk_Trolley.cpp or in its weak tail.
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

            virtual void SetMdlIdx(long idx) { m_nMdlIdx = idx; }                     // 0x124
            // The car's type: 0 for the first car, 2 for the others; 3 is
            // a car whose heyho throws coins (see RebuildMdl). Types 2 and 3
            // carry a heyho.
            virtual void SetMdlType(long type) { m_nMdlType = type; }                 // 0x128
            virtual long GetMdlType() const { return m_nMdlType; }                    // 0x12C
            virtual void SetHeyhoParam(int bCoin, float distMin, float distMax);      // 0x130
            // The size of the model as built, which Main scales to the
            // actor's GetParam_Mdl* sizes.
            virtual float GetParam_MdlWidth() const;                                  // 0x134
            virtual float GetParam_MdlHeight() const;                                 // 0x138
            virtual float GetParam_MdlDepth() const;                                  // 0x13C
            virtual long GetMdlIdx() const { return m_nMdlIdx; }                      // 0x140
            virtual void CreateHeyho();                                               // 0x144
            virtual void DeleteHeyho();                                               // 0x148
            virtual void DispHeyho(int bDisp);                                        // 0x14C
            virtual CNpcHeyho *GetHeyho();                                            // 0x150

        private:
            long m_nMdlIdx;              // 0x12C (GetMdlIdx)
            long m_nMdlType;             // 0x130, 2 for every car but the first, which has 0
            unsigned long m_nHeyhoId;    // 0x134, task id of the heyho; 0 until CreateHeyho
            float m_fHeyhoWait;          // 0x138, frames before the heyho shows again after a scene entry
    };
}

#endif /* !MSW_CSTGGMK_TROLLEY_MDL_HPP_ */
