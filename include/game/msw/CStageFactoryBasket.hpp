#ifndef MSW_CSTAGEFACTORYBASKET_HPP_
#define MSW_CSTAGEFACTORYBASKET_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    class CStageManBasket;

    // Makes the stage object of a basket stage number (BasketF).
    class CStageFactoryBasket {
        public:
            // (return type guessed: CStageManBasket::CreateInstance hands it
            // on as its own result)
            static CStageManBasket *Create(long stageNo, bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSTAGEFACTORYBASKET_HPP_ */
