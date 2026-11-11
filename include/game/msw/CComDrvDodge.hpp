#ifndef MSW_CCOMDRVDODGE_HPP_
#define MSW_CCOMDRVDODGE_HPP_

#include "game/msw/CComDrv.hpp"

namespace msw {

    // The computer driver of the dodgeball game (DodgeF). Adds three
    // virtuals for the side-step roll; no members of its own were found.
    class CComDrvDodge : public CComDrv {
        public:
            CComDrvDodge(long nArg);
            virtual ~CComDrvDodge();

            virtual int CanbeDodgeRoll(float fRot);
            virtual int MethdDodgeRoll(float fRot);
            virtual int MethdDodgeRollTarget(const bcs::FVEC3 &pos);
    };

    // INCOMPLETE: the computer driver of the dodgeball game proper
    // (game/dodge/CComDrvDodgeGame.cpp), size 0x264 (what
    // CChrManDodge::CreateDriver_ allocates). Only the constructor and the
    // destructor are declared, so the vtable is not emitted from here.
    class CComDrvDodgeGame : public CComDrvDodge {
        public:
            CComDrvDodgeGame();
            virtual ~CComDrvDodgeGame();

        protected:
            unsigned char m_Unk170[0x264 - 0x170];
    };

    // INCOMPLETE: the computer driver of the party game "dodge or die"
    // (game/dodge/CComDrvDodgeDD.cpp), size 0xB7C (what
    // CChrManDodgeDodgeOrDie::CreateDriver_ allocates). As above.
    class CComDrvDodgeDD : public CComDrvDodge {
        public:
            CComDrvDodgeDD();
            virtual ~CComDrvDodgeDD();

        protected:
            unsigned char m_Unk170[0xB7C - 0x170];
    };
}

#endif /* !MSW_CCOMDRVDODGE_HPP_ */
