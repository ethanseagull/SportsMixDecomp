#ifndef MSW_TOURNAMENTBOSS_HPP_
#define MSW_TOURNAMENTBOSS_HPP_

#include "game/msw/CTmGameHandler.hpp"

namespace msw {

    // The tournament handler of the boss game (BossF), size 0x54. The
    // destructor is the compiler's.
    class CTmGameHandlerBoss : public CTmGameHandler {
        public:
            CTmGameHandlerBoss(bcs::CTask *pParent) : CTmGameHandler(pParent) {}

            virtual void _SendMessage(long nMsg, long, long, long);
            virtual int _CheckWin();
            virtual int _CheckClear();
            virtual int _IsClear(long);
            virtual int _IsFaild(long);
            virtual void _SetCountDisp(int);
            virtual void _SetCountUp(long);

        private:
            // Cleared when the game starts (message 0xF); nothing else in
            // the class touches them.
            long m_Unk44; // 0x44
            long m_Unk48; // 0x48
            long m_Unk4C; // 0x4C
            long m_Unk50; // 0x50
    };

    // A namespace by the same guess as msw::Tournament: the name has no "C"
    // prefix.
    namespace TournamentBoss {
        // Makes sure there is a tournament handler: the one
        // CTmGameHandler::CreateInstance returns, or else a new
        // CTmGameHandlerBoss. Returns CTmGameHandler::m_pInstance.
        CTmGameHandler *CreateInstance(bcs::CTask *pParent);
    }
}

#endif /* !MSW_TOURNAMENTBOSS_HPP_ */
