#ifndef MSW_CSTGGMKMAN_HPP_
#define MSW_CSTGGMKMAN_HPP_

namespace msw {

    class CStgGmk;

    // (purpose guessed from the names) The manager of the stage gimmicks: a
    // DOL singleton that walks the live gimmicks of one type (CStgGmk's
    // GetType). Only the lookups are declared; the layout is not mapped.
    class CStgGmkMan {
        public:
            static CStgGmkMan *GetInstance();

            // The first gimmick of the type, null when there is none.
            CStgGmk *GetFirst(int type);
            const CStgGmk *GetFirst(int type) const;
            // The gimmick of the type after pGmk, null after the last.
            CStgGmk *GetNext(CStgGmk *pGmk, int type);
            const CStgGmk *GetNext(const CStgGmk *pGmk, int type) const;
    };
}

#endif /* !MSW_CSTGGMKMAN_HPP_ */
