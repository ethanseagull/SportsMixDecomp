#ifndef MSW_CSTAGEANM_HPP_
#define MSW_CSTAGEANM_HPP_

namespace msw {

    // The animations of a stage model. Nothing of it has a name in the DOL
    // except the type below, which msw::CStgMdl's animation getters take
    // (GetAnmFrame__Q23msw7CStgMdlCFQ33msw9CStageAnm7ANM_IDX), so whether it
    // is a class or a namespace is a guess.
    class CStageAnm {
        public:
            // Which of the animations playing at once is meant. Values not
            // known.
            enum ANM_IDX {
            };
    };
}

#endif /* !MSW_CSTAGEANM_HPP_ */
