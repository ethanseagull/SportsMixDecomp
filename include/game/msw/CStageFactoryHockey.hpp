#ifndef MSW_CSTAGEFACTORYHOCKEY_HPP_
#define MSW_CSTAGEFACTORYHOCKEY_HPP_

namespace bcs {
    class CTask;
}

namespace msw {

    class CStageManHockey;

    // Makes the stage object of a hockey stage number (HockeyF).
    class CStageFactoryHockey {
        public:
            // (return type guessed: CStageManHockey::CreateInstance hands it
            // on as its own result)
            static CStageManHockey *Create(long stageNo, bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSTAGEFACTORYHOCKEY_HPP_ */
