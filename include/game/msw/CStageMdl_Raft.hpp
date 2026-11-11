#ifndef MSW_CSTAGEMDL_RAFT_HPP_
#define MSW_CSTAGEMDL_RAFT_HPP_

#include "game/msw/CStageMdl.hpp"

namespace msw {

    class CStageMan;

    // (description guessed) A stage model that floats on the water, a DOL
    // class: HockeyF's CStageHockey05 makes its "null_wavelog" models with
    // it. Size 0x10F4 (CStageHockey05::NewMdl_Raft); only the constructor
    // is named.
    class CStageMdl_Raft : public CStageMdl {
        public:
            CStageMdl_Raft(CStageMan *pStage);

        private:
            unsigned char m_Unk10CC[0x28]; // 0x10CC
    };
}

#endif /* !MSW_CSTAGEMDL_RAFT_HPP_ */
