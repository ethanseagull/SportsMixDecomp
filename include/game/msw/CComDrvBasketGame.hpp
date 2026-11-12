#ifndef MSW_CCOMDRVBASKETGAME_HPP_
#define MSW_CCOMDRVBASKETGAME_HPP_

#include "game/msw/CComDrvBasket.hpp"

namespace msw {

    struct MEMBER_INFO;

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

            // (added for game/basket/CComDrvBasketGame.cpp; return types
            // and meanings guessed from the names and how ActInterrupt and
            // ThkMain use them) The overrides that break off the running
            // action and pick the next one, and their parts.
            virtual void ActInterrupt();
            virtual void ThkMain();
            void ActInterrupLooseBall_();
            void ActInterruptOffense_();
            void ActInterruptItemShootCheck_();
            void ActInterruptDefense_();
            void ActInterrupSp_();
            // Not zero when they started an action (or one is running that
            // must not be broken off).
            int ActInterrupStateStar_();
            int ActInterrupGimmick_();
            int ActInterrupCommonThrowItem_();
            int ActInterrupAlleyoopforLuiMan_();
            void ThkMainJumpBall_();
            void ThkMainLooseBall_();
            void ThkMainOffense_();
            void ThkMainOffenseStar_();
            void ThkMainDefense_();
            void ThkMainDefenseStar_();
            void ThkMainSp_();
            int SubThkMainOffenseGoDunkAreaforLuiman_();
            // Tests and draws.
            int IsCombination() const;
            int IsAllComTeam() const;
            int JudgeItemThrow(bcs::FVEC3 &rPos);
            int JudgeShootCourseFree() const;
            int JudgeBombShootEsp();
            int JudgeReboundOrReboundDunk();
            int JudgeSmashCut();
            int JudgeCut();
            int JudgeMissCut();
            int JudgeShootBlock();
            int JudgeAlleyoopPassBlock();
            int JudgeDunkBlock();
            int JudgeAlleyoopBlock();
            int JudgeJumpUpper();
            int JudgeJumpCut();
            int JudgeCombinationPlay() const;
            int CanbeActCallJumpShoot();
            int CanbeJumpShoot();
            int CanbeActCallItemShoot();
            int CanbeActGetGShell() const;
            int CanbeActGetHatenaPanel() const;
            int CanbeAlleyoop() const;
            int CanbeAirTrick();
            int LotRateCancelPass();
            int LotRateCancelLayUp();
            int IsActCallDefenseExt();
            // -1 when no direction of a shot with the curve fCurve is free.
            long CheckItemShootDir(float fCurve, float fRange0, float fRange1) const;
            // The rate of the "?" panels of the stage (1 for most).
            float GetHatenaPanelStageRate_() const;
            // The holders of team nTeam with their scores; the number of them.
            long CalcHoldBallScore(long nTeam, MEMBER_INFO &rInfo);
            long GetBaseShootPointToGimmick() const;
            // Where the driver stands against the three-point area on the
            // cruise ship stage (-1, 0 or 1).
            long GetDCruise3PointAreaState() const;
            void GetHomePos(bcs::FVEC3 &rPos);
            void MethdDash();
            int MethdCombination();
            int MethdPassRequest();
            // More start actions.
            void ActCallItemThrow(const bcs::FVEC3 &rPos);
            void ActCallItemShoot(long nArg);
            void ActCallItemEscape();
            void ActCallGetBall();
            void ActCallDunk();
            void ActCallJumpShoot();
            void ActCallJump();
            void ActCallPass();
            void ActCallCancelPass(const CChrCore *pChr);
            void ActCallShootCancelPass(const CChrCore *pChr);
            void ActCallCancelLayUp();
            void ActCallBlockade();
            void ActCallSpinMove();
            void ActCallGoOffense();
            void ActCallGoOffense2();
            void ActCallGoDefense();
            void ActCallGoDefense2();
            void ActCallGoDefense3();
            void ActCallGoDefOnBaller();
            void ActCallGoDefPassCourse();
            void ActCallDefGhost();
            void ActCallGetGShell();
            void ActCallGetHatenaPanel();
            void ActCallGoEspBubble(unsigned long nObstacle);
            void ActCallGoEscape(unsigned long nItem);
            void ActCallGoRunRand();
            void ActCallGoHome();
            void ActCallGoBall();
            void ActCallGoEnemy();
            void ActCallGoDunkArea();
            void ActCallAlleyoop();
            void ActCallJumpBall();
            void ActCallCombination();
            void ActCallPassGhost();
            void ActCallAirTrick();
            void ActCallBallKeep();

            static ActFunc g_ActFuncMap[];

        protected:
            // 0x170 (name guessed): the character the driver marks on
            // defence; null for none.
            CChrCore *m_pMarkChr;
            unsigned char m_Unk174[0x17C - 0x174];
            // 0x17C (name guessed): flag bits. 0x20 and 0x40 toggle on
            // timers (m_fUnk1CC, m_fUnk1DC), 0x400 is drawn while holding
            // the ball, 0x800 is set by message 0x18.
            unsigned long m_nFlag17C;
            unsigned char m_Unk180[0x184 - 0x180];
            long m_nUnk184;   // 0x184, cleared with m_nUnk188 by ActInterrupt
            long m_nUnk188;   // 0x188
            float m_fUnk18C;  // 0x18C, counts up every ActInterrupt
            float m_fUnk190;  // 0x190, a timer counted down to 0
            float m_fUnk194;  // 0x194, the timer of the dunk draw
            unsigned char m_Unk198[0x1A0 - 0x198];
            float m_fUnk1A0;  // 0x1A0, frames the ball has been held
            float m_fUnk1A4;  // 0x1A4, the timer of the alley-oop draw
            float m_fUnk1A8;  // 0x1A8, the timer of the item shot draw
            unsigned char m_Unk1AC[0x1B0 - 0x1AC];
            float m_fUnk1B0;  // 0x1B0
            float m_fUnk1B4;  // 0x1B4, the timer of the thrown item check
            unsigned char m_Unk1B8[0x1BC - 0x1B8];
            float m_fUnk1BC;  // 0x1BC, a timer counted down to 0
            float m_fUnk1C0;  // 0x1C0, the timer of the star state's panel draw
            float m_fUnk1C4;  // 0x1C4, counts down; at 0 restarts at 30
            float m_fUnk1C8;  // 0x1C8, set to -1 when m_fUnk1C4 runs out
            float m_fUnk1CC;  // 0x1CC, the timer of flag 0x20
            unsigned char m_Unk1D0[0x1D4 - 0x1D0];
            float m_fUnk1D4;  // 0x1D4, a timer counted down to 0
            float m_fUnk1D8;  // 0x1D8, the timer of the boss Packun jump shot
            float m_fUnk1DC;  // 0x1DC, the timer of flag 0x40
            unsigned char m_Unk1E0[0x1FC - 0x1E0];
            float m_fUnk1FC;  // 0x1FC, a timer (message 0x26)
            unsigned char m_Unk200[0x688 - 0x200];
    };
}

#endif /* !MSW_CCOMDRVBASKETGAME_HPP_ */
