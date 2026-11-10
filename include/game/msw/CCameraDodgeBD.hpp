#ifndef CCAMERADODGEBD_HPP_
#define CCAMERADODGEBD_HPP_

#include "game/msw/CCameraDodge.hpp"

namespace msw {

    // Size 0x290 (from msw::CCameraDodge::Initialize). The camera of the
    // dodgeball party game. Only what other units need so far; its own unit
    // is game/dodge/CCameraDodgeBD.cpp. Overrides from its vtable (DodgeF).
    class CCameraDodgeBD : public CCameraDodge {
        public:
            CCameraDodgeBD(long view);
            virtual ~CCameraDodgeBD();
            virtual void MainCamera();
            virtual void Reset();
            virtual bool CanbeMain() const;
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam);

        private:
            unsigned char m_Unk254[0x290 - 0x254];
    };
}

#endif /* !CCAMERADODGEBD_HPP_ */
