#ifndef MSW_CC_DATA_HPP_
#define MSW_CC_DATA_HPP_

#include "bcs/CAnmObjMotBase.hpp"

namespace msw {

    // The characters' data tables. Only what the modules use so far.
    class CC_DATA {
        public:
            // The node settings for the motions of non-player model `nNpc`.
            static const bcs::CAnmObjMotBase::NODE_EX_INFO *GetNpcMdl_NodeExInfo(long nNpc);
    };
}

#endif /* !MSW_CC_DATA_HPP_ */
