#ifndef CCAMERAHOCKEYCS_HPP_
#define CCAMERAHOCKEYCS_HPP_

#include "game/msw/CCameraHockey.hpp"

namespace msw {

    // Size 0x290 (from msw::CCameraHockey::Initialize). The camera of the
    // hockey party game (game types 2 and 3). Only what other units need so
    // far; its own unit is game/hockey/CCameraHockeyCS.cpp.
    class CCameraHockeyCS : public CCameraHockey {
        public:
            CCameraHockeyCS(long view);
            virtual ~CCameraHockeyCS();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            unsigned char m_Unk254[0x290 - 0x254];
    };
}

#endif /* !CCAMERAHOCKEYCS_HPP_ */
