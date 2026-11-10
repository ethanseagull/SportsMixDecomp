#ifndef CSTAGEMANDEMO_HPP_
#define CSTAGEMANDEMO_HPP_

#include "game/msw/CStageMan.hpp"

namespace msw {

    // The stage of the tournament map's event demos (DOL class, 0x1FC
    // bytes: CStageMan with nothing added). Only the constructor is called
    // so far (msw::CTournamentMan::SetupStageDemoModel, TourF).
    class CStageManDemo : public CStageMan {
        public:
            CStageManDemo(bcs::CTask *pParent);
    };
}

#endif /* !CSTAGEMANDEMO_HPP_ */
