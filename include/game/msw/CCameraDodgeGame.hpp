#ifndef CCAMERADODGEGAME_HPP_
#define CCAMERADODGEGAME_HPP_

#include "game/msw/CCameraDodge.hpp"

namespace msw {

    // Size 0x6D4 (from msw::CCameraDodge::Initialize). The camera of a
    // dodgeball match. Only what other units need so far; its own unit is
    // game/dodge/CCameraDodgeGame.cpp. Overrides from its vtable (DodgeF).
    class CCameraDodgeGame : public CCameraDodge {
        public:
            CCameraDodgeGame(long view);
            virtual ~CCameraDodgeGame();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool IsOffense() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            unsigned char m_Unk254[0x6D4 - 0x254];
    };
}

#endif /* !CCAMERADODGEGAME_HPP_ */
