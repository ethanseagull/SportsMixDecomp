#ifndef MSW_CSTAGEFACTORYVOLLEY_HPP_
#define MSW_CSTAGEFACTORYVOLLEY_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    class CStageManVolley;

    // Makes the stage object of a volley stage number (VolleyF).
    class CStageFactoryVolley {
        public:
            // (return type guessed: CStageManVolley::CreateInstance hands it
            // on as its own result)
            static CStageManVolley *Create(long stageNo, bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSTAGEFACTORYVOLLEY_HPP_ */
