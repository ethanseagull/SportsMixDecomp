#ifndef MSW_CSTAGEFACTORYDODGE_HPP_
#define MSW_CSTAGEFACTORYDODGE_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    class CStageManDodge;

    // Makes the stage object of a dodge stage number (DodgeF).
    class CStageFactoryDodge {
        public:
            // (return type guessed: CStageManDodge::CreateInstance hands it
            // on as its own result)
            static CStageManDodge *Create(long stageNo, bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSTAGEFACTORYDODGE_HPP_ */
