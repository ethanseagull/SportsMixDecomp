#ifndef MSW_CSTGGMKCLUSTERIMPL_HPP_
#define MSW_CSTGGMKCLUSTERIMPL_HPP_

#include "bcs/New.hpp"

namespace bcs {
    class CTask;
}

namespace msw {

    class CStgGmk;

    // A set of GetTblSize() stage gimmicks made and run together. A DOL
    // class, size 0x14: the data members come before the first virtual
    // function, so the vptr is at 0x10. Each kind of gimmick derives its
    // cluster from it and supplies NewGmk and GetMaxNum.
    //
    // The order of the virtual functions is that of
    // __vt__Q23msw33CStgGmk_WaterPillarCluster_Hockey (HockeyF). The modules
    // emit weak copies of GetParent, GetTblSize, OnPreDeleteGmk and
    // OnCalcSum, so those are inline. Return types not visible in a body
    // are guesses.
    class CStgGmkClusterImpl {
        private:
            unsigned long m_bCreated; // 0x00, set by CreateGmk, cleared by DeleteGmk (name guessed)
            bcs::CTask *m_pParent; // 0x04
            CStgGmk **m_ppGmk;     // 0x08, GetTblSize() gimmicks (type guessed)
            long m_nTblSize;       // 0x0C

        public:
            // (fn_8022A9C4, unnamed in the DOL)
            CStgGmkClusterImpl(bcs::CTask *pParent);
            virtual ~CStgGmkClusterImpl();
            virtual long GetMaxNum() const = 0;
            virtual void CreateGmk();
            virtual void DeleteGmk();
            virtual void ReleaseGmk(CStgGmk *pGmk);
            virtual void ReleaseGmk(long idx);
            virtual void Main(float rate);
            virtual unsigned long CalcSum() const;
            virtual bcs::CTask *GetParent() const { return m_pParent; }
            virtual CStgGmk *NewGmk(long heap) const = 0;
            virtual long GetTblSize() const { return m_nTblSize; }
            virtual CStgGmk *GetGmk_(long idx);
            virtual const CStgGmk *GetGmk_(long idx) const;
            // OnPostCreateGmk and OnUpdate are empty inline functions: the
            // DOL vtable of this class (lbl_8048A3F0) holds the weak copies
            // that game/msw/CStgGmk_WaterPillarCluster.cpp emits.
            virtual void OnPostCreateGmk() {}
            virtual void OnPreDeleteGmk() {}
            virtual void OnUpdate(float rate) {}
            virtual unsigned long OnCalcSum() const { return 0; }

        private:
            // (name guessed) Inline helper of the DOL bodies: gimmick idx
            // of the table. It calls GetTblSize and ignores the result,
            // what is left of a range check in this build.
            CStgGmk *At(long idx) const
            {
                (void)GetTblSize();
                return m_ppGmk[idx];
            }
    };

    // (name guessed) A cluster of gimmicks of class T: supplies NewGmk.
    // Evidence: msw::CStgGmk_WaterPillarCluster's constructor stores a
    // vtable between CStgGmkClusterImpl's and its own (DOL lbl_8047F5E0,
    // GetMaxNum still pure), whose NewGmk makes a CStgGmk_WaterPillar with
    // its constructor inlined, and the file emits the class's implicit
    // destructor (fn_80194A5C).
    template <class T>
    class CStgGmkCluster : public CStgGmkClusterImpl {
        public:
            CStgGmkCluster(bcs::CTask *pParent) : CStgGmkClusterImpl(pParent) {}

            virtual CStgGmk *NewGmk(long heap) const { return new (heap) T(GetParent()); }
    };
}

#endif /* !MSW_CSTGGMKCLUSTERIMPL_HPP_ */
