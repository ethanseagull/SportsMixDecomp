#ifndef MSW_CSTGGMKCLUSTERIMPL_HPP_
#define MSW_CSTGGMKCLUSTERIMPL_HPP_

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
            unsigned long m_Unk00; // 0x00
            bcs::CTask *m_pParent; // 0x04
            CStgGmk **m_ppGmk;     // 0x08, GetTblSize() gimmicks (type guessed)
            long m_nTblSize;       // 0x0C

        public:
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
            virtual void OnPostCreateGmk();
            virtual void OnPreDeleteGmk() {}
            virtual void OnUpdate(float rate);
            virtual unsigned long OnCalcSum() const { return 0; }
    };
}

#endif /* !MSW_CSTGGMKCLUSTERIMPL_HPP_ */
