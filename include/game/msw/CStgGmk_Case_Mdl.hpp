#ifndef MSW_CSTGGMK_CASE_MDL_HPP_
#define MSW_CSTGGMK_CASE_MDL_HPP_

#include "game/msw/CStgMdl.hpp"

namespace bcs {
    class CMdlObj;
    class CScnObj;
}

namespace msw {

    class CStgGmk;

    // The model of a case. A DOL class, size 0x134 (constructor
    // __ct__Q23msw16CStgGmk_Case_MdlFPQ23msw7CStgGmkl, the case's type as
    // the second argument); msw::CStgMdl's vptr stays at 0x128. The model
    // object is a msw::CFgMdl that fades by view, scaled to the actor's
    // size.
    class CStgGmk_Case_Mdl : public CStgMdl {
        public:
            CStgGmk_Case_Mdl(CStgGmk *pParent, long type);
            virtual ~CStgGmk_Case_Mdl();
            virtual void Create();
            virtual void Main(float rate);
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual void OnPostCreate();

            // A box of the actor's size round m_pDebugObj, in view nView
            // (the DOL name; a debug draw like
            // msw::CStgGmk_Pinball_Mdl::DrawDebug).
            virtual void Disp(long nView);                            // 0x124
            // (names guessed) The case's type (the actor's
            // GetParam_MdlIdx at a respawn).
            virtual void SetType(long type) { m_nType = type; }       // 0x128
            virtual long GetType() const { return m_nType; }          // 0x12C

        private:
            bcs::CScnObj *m_pDebugObj; // 0x12C (never set in the DOL)
            long m_nType;              // 0x130
    };
}

#endif /* !MSW_CSTGGMK_CASE_MDL_HPP_ */
