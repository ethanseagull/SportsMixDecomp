#ifndef MSW_CANMTBLBASE_HPP_
#define MSW_CANMTBLBASE_HPP_

namespace msw {

    // (minimal declaration, added for VolleyF
    // game/volley/CStgGmk_WaterWorks_Volley_Act.cpp) A DOL class; only its
    // empty animation table is known: null_tbl__Q23msw11CAnmTblBase, two
    // words in .sdata, used as the animation suffix table
    // (msw::CStgMdl::MDL_PARAM::ppAnmSuffix) of models without animations.
    class CAnmTblBase {
        public:
            static const char *null_tbl[2];
    };
}

#endif /* !MSW_CANMTBLBASE_HPP_ */
