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
    // CChrManDodgeDodgeOrDie::CreateDriver_ allocates). Driven by a script
    // (0x174). Return types of the undefined members are guesses.
    class CComDrvDodgeDD : public CComDrvDodge {
        public:
            // An action: a member called each frame while it runs.
            typedef void (CComDrvDodgeDD::*ActFunc)();

            CComDrvDodgeDD();
            virtual ~CComDrvDodgeDD();

            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void EntryActCallBack(long);
            virtual void NormalizeCanMoveWorldPos(bcs::FVEC3 &pos) const;
            virtual void Reset();
            virtual int IsActiveBallNothingThk() const;
            virtual void OnMain(_CHR_MSG *pMsg);
            virtual void OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void ActInterrupt();
            virtual void ThkMain();

            void ActLoopGoHome();
            void ActLoopScript();
            void DeleteScript();
            void ResetScript();

            // The actions by number: CComDrv's common ones, then this
            // driver's own.
            static ActFunc g_ActFuncMap[12];

        protected:
            long m_nUnk170;   // 0x170, cleared by the constructor
            void *m_pScript;  // 0x174 (name guessed): the script
            unsigned char m_Unk178[0xB78 - 0x178];
            long m_nUnkB78;   // 0xB78, used by CalcBombInfo
    };
}

#endif /* !MSW_CCOMDRVDODGE_HPP_ */
