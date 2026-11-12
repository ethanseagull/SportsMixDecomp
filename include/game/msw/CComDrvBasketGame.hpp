#ifndef MSW_CCOMDRVBASKETGAME_HPP_
#define MSW_CCOMDRVBASKETGAME_HPP_

#include "game/msw/CComDrvBasket.hpp"

namespace msw {

    // INCOMPLETE: the computer driver of the normal basketball game
    // (BasketF; game/basket/CComDrvBasketGame.cpp and other units). Only
    // what the written units use is declared; the size, 0x688, is what
    // CChrManBasket::CreateDriver_ allocates.
    class CComDrvBasketGame : public CComDrvBasket {
        public:
            // The actions, one per frame while running (g_ActFuncMap, 12-byte
            // pointers to members, called by OnMainSub_).
            typedef void (CComDrvBasketGame::*ActFunc)();

            CComDrvBasketGame();
            virtual ~CComDrvBasketGame();

            // (added for game/basket/unit_AF3BC.cpp) The overrides of
            // CComDrv's message handlers, in the order of the vtable. The
            // others of the vtable (Reset, ClearAll, UpdateInfo, ...) are
            // in other units and not declared yet.
            virtual void BeforeMsgFunc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void EndMsgFunc(MSG_ID id, _CHR_MSG *pMsg);
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual void OnConnect(_CHR_MSG *pMsg);
            virtual void OnDisconnect(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);

            // (added for game/basket/CComManBasketGame.cpp; return types
            // guessed) The frame rate of a dunk, and the message that the
            // ghost caught a shot.
            float GetDunkFrameRate() const;
            void MsgShootCatchToGhost();

            // (added for game/basket/unit_AF3BC.cpp; return types and
            // meanings guessed from the names and how OnMainSub_ and
            // OnGameMsgProc use them)
            long OnMainSub_(_CHR_MSG *pMsg);
            void InitScript();
            // The "panels" of the court: a grid of scores the driver fills
            // each frame (higher nearer the goal, lower next to other
            // characters). The free area as a rectangle in world units
            // and in panel indices; one panel's rectangle; its score.
            void ClearPanelData();
            void ClearPanelDisableFlg();
            void GetFreeAreaRect(float &rLeft, float &rRight, float &rTop, float &rBottom);
            void GetFreeAreaRect(long &rLeft, long &rRight, long &rTop, long &rBottom);
            void GetPanelRect(long nW, long nH, float &rLeft, float &rRight, float &rTop, float &rBottom);
            signed char GetPanelDate(long nW, long nH) const;
            void SetPanelDate(long nW, long nH, char nDate);
            long GetPanelWIdx(float fX) const;
            long GetPanelHIdx(float fZ) const;
            // Answers to the game's messages.
            void MsgSomeBodyShoot(_CHR_MSG *pMsg);
            void MsgShootStart(_CHR_MSG *pMsg);
            void MsgReboundStart(_CHR_MSG *pMsg);
            int MsgArtsShootRelease(_CHR_MSG *pMsg);
            long MsgArtsDirInput(_CHR_MSG *pMsg);
            long LotRateJumpShoot(long n0, long n1, long n2);
            long LotRateLayUp();
            int LotRateSwatIt();
            long LotRateUseItem(long n0, long n1, long n2);
            long CheckFreeShootCourseToCurve(long nNum, const CChrCore **ppChr, float fCurve, float f1, float f2) const;
            long CheckArtsShootDir() const;
            int CanbeActCutEsp() const;
            float GetAttainmentMoveMinDist() const;
            // Start actions.
            void ActCallWaitRand(long nMin, long nMax);
            void ActCallGoRun(const bcs::FVEC3 &rPos, float fMinDist, float fMaxFrame, unsigned long nFlag);
            void ActCallPass(const CChrCore *pChr);
            void ActCallMoveBlock();
            void ActCallAssistPlay();
            void ActCallScriptI(const char *pName, long nArg);

            static ActFunc g_ActFuncMap[];

        protected:
            unsigned char m_Unk170[0x17C - 0x170];
            // 0x17C (name guessed): flag bits. 0x20 and 0x40 toggle on
            // timers (m_fUnk1CC, m_fUnk1DC), 0x400 is drawn while holding
            // the ball, 0x800 is set by message 0x18.
            unsigned long m_nFlag17C;
            unsigned char m_Unk180[0x188 - 0x180];
            long m_nUnk188;   // 0x188
            unsigned char m_Unk18C[0x190 - 0x18C];
            float m_fUnk190;  // 0x190, a timer counted down to 0
            unsigned char m_Unk194[0x1A0 - 0x194];
            float m_fUnk1A0;  // 0x1A0, frames the ball has been held
            unsigned char m_Unk1A4[0x1B0 - 0x1A4];
            float m_fUnk1B0;  // 0x1B0
            unsigned char m_Unk1B4[0x1BC - 0x1B4];
            float m_fUnk1BC;  // 0x1BC, a timer counted down to 0
            unsigned char m_Unk1C0[0x1C4 - 0x1C0];
            float m_fUnk1C4;  // 0x1C4, counts down; at 0 restarts at 30
            float m_fUnk1C8;  // 0x1C8, set to -1 when m_fUnk1C4 runs out
            float m_fUnk1CC;  // 0x1CC, the timer of flag 0x20
            unsigned char m_Unk1D0[0x1D4 - 0x1D0];
            float m_fUnk1D4;  // 0x1D4, a timer counted down to 0
            unsigned char m_Unk1D8[0x1DC - 0x1D8];
            float m_fUnk1DC;  // 0x1DC, the timer of flag 0x40
            unsigned char m_Unk1E0[0x1FC - 0x1E0];
            float m_fUnk1FC;  // 0x1FC, a timer (message 0x26)
            unsigned char m_Unk200[0x688 - 0x200];
    };
}

#endif /* !MSW_CCOMDRVBASKETGAME_HPP_ */
