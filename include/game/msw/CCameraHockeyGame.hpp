#ifndef CCAMERAHOCKEYGAME_HPP_
#define CCAMERAHOCKEYGAME_HPP_

#include "game/msw/CCameraHockey.hpp"

namespace msw {

    // Size 0x900 (from msw::CCameraHockey::Initialize). The camera of a
    // hockey match. Only what other units need so far; its own unit is
    // game/hockey/CCameraHockeyGame.cpp.
    class CCameraHockeyGame : public CCameraHockey {
        public:
            CCameraHockeyGame(long view);
            virtual ~CCameraHockeyGame();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool IsNormalCamMode() const;

        private:
            unsigned char m_Unk254[0x900 - 0x254];
    };
}

#endif /* !CCAMERAHOCKEYGAME_HPP_ */
