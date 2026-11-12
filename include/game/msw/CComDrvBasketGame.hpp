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

            // (added for game/basket/unit_9AFD4.cpp; names from the loop
            // functions of g_ActFuncMap, which this file defines) The
            // action numbers EntryAct takes. 0-9 are CComDrv's common ones.
            enum ACT_TYPE {
                ACT_NOP,
                ACT_WAIT,
                ACT_WAIT_NEUTRAL,
                ACT_LOOK_TARGET,
                ACT_LOOK_FRONT,
                ACT_LOOK_BALL,
                ACT_RUN,
                ACT_RUN_TURN,
                ACT_JUMP,
                ACT_JUMP_BLOCK,
                ACT_JUMP_BALL,          // 0x0A
                ACT_GO_OFFENSE,
                ACT_GO_OFFENSE2,
                ACT_GO_DEFENSE,
                ACT_GO_DEFENSE2,
                ACT_GO_DEFENSE3,
                ACT_GO_DEF_PASS_COURSE, // 0x10
                ACT_GO_DEF_ON_BALLER,
                ACT_GO_HOME,
                ACT_GO_FREE_AREA,
                ACT_GET_BALL,
                ACT_GET_HATENA_PANEL,
                ACT_GO_RUN_RAND,
                ACT_GO_RUN,
                ACT_PASS,               // 0x18
                ACT_PASS_GHOST,
                ACT_SHOOT_CANCEL_PASS,
                ACT_CANCEL_PASS,
                ACT_PASS_SIGN,
                ACT_JUMP_SHOOT,
                ACT_DUNK,
                ACT_GO_REBOUND_DUNK,
                ACT_REBOUND_DUNK,       // 0x20
                ACT_ALLEYOOP,
                ACT_ASSIST_PLAY,
                ACT_BLOCKADE,
                ACT_GO_BACK,
                ACT_GO_ZIGUZAGU,
                ACT_GO_SIDE,
                ACT_SPIN_MOVE,
                ACT_AIR_TRICK,          // 0x28
                ACT_STICK_DASH,
                ACT_GO_TARGET_GOAL,
                ACT_BALL_KEEP,
                ACT_GO_DUNK_AREA,
                ACT_GO_DUNK,
                ACT_LAY_UP,
                ACT_CANCEL_LAY_UP,
                ACT_ITEM_SHOOT,         // 0x30
                ACT_COMBINATION,
                ACT_GO_ENEMY,
                ACT_GO_BALL,
                ACT_REBOUND,
                ACT_CUT,
                ACT_MISS_CUT,
                ACT_JUMP2,              // the driver's own jump (ActLoopJump again)
                ACT_DEF_JUMP,           // 0x38
                ACT_JUMP_CUT,
                ACT_JUMP_UPPER,
                ACT_DEF_ALLEYOOP,
                ACT_ITEM_ESCAPE,
                ACT_GO_ESCAPE,
                ACT_GO_ESCAPE2,
                ACT_ITEM_THROW,
                ACT_DEF_GHOST,          // 0x40
                ACT_CUT_CHANCE,
                ACT_GO_SMASH,
                ACT_MOVE_BLOCK,
                ACT_GO_DEFENSE_GOAL,
                ACT_SHOOT_BLOCK,
                ACT_JUMP_UPPER_CHANCE,
                ACT_JUMP_CUT_CHANCE,
                ACT_GO_POINT_PANEL,     // 0x48
                ACT_GET_G_SHELL,
                ACT_GO_ESP_BUBBLE,
                ACT_SCRIPT
            };

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

            // (added for game/basket/unit_B09B4.cpp; return types guessed)
            // The overrides of CComDrv's and CComDrvBasket's action slots.
            virtual int MethdPass(const bcs::FVEC3 &rPos);
            virtual int MethdPass(CChrCore *pChr);
            virtual int MethdAirTrick();
            virtual int MethdTrickMove();
            virtual int CanbeJumpBlock();
            // Runs to rPos (local coordinates); turns aside to a dropped
            // coin or a J-street panel on the way when the driver wants
            // one. Not zero when the driver has arrived.
            int MethdBKRunTest(bcs::FVEC3 &rPos, float fDist, float fDpm, int bKeepRunning);
            int MethdBKRun2(bcs::FVEC3 &rPos, float fDist, float fDpm, int bKeepRunning, int bTurnAround);
            int MethdPassBK(float fRot, int nArg);
            int MethdPassSign();
            int MethdReboundDunk();
            int MethdAlleyoop();
            // Trick move to the right (bRight set) or the left.
            int MethdSpinMove(int bRight);
            int CanbeItemThrow();
            int CanbeCombination() const;
            // (added for game/basket/unit_B09B4.cpp, defined elsewhere;
            // return types guessed) The radius the driver searches "?"
            // panels in; turns rPos aside around what stands in the way
            // (not zero when it did).
            float GetSearchHatenaPanelArea() const;
            int GetPosTurnAround(bcs::FVEC3 &rPos);
            // (inline: no symbol of its own; MethdDash has it inlined)
            int CanbeDash() const
            {
                if (m_pChr->IsMoveAssist()) {
                    return 0;
                }
                if (m_pChr->IsHoldBall()) {
                    return m_pChr->CanbeDash();
                }
                return 0;
            }
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
            int MethdDash();
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

            // (added for game/basket/unit_9AFD4.cpp; return types and
            // meanings guessed) More start actions, the per-frame loops of
            // all of them, and the helpers they use that are defined in
            // other units.
            void ActCallGoReboundDunk();
            void ActCallReboundDunk(long nArg);
            void ActCallRebound();
            void ActCallRebound(float fArg);
            void ActCallMissCut();
            void ActCallDefJump(float f0, float f1);
            void ActCallDefJumpRandWait(float fArg);
            void ActCallDefAlleyoop(const CChrCore *pChr);
            void ActCallGoEscape2(const bcs::FVEC3 &rPos);
            void ActCallCutChance();
            void ActCallGoSmash();
            void ActCallGoDefenseGoal();
            void ActCallShootBlock();
            void ActCallJumpUpperChance();
            void ActCallJumpCutChance();
            void ActLoopJumpBall();
            void ActLoopGoOffense();
            void ActLoopGoOffense2();
            void ActLoopGoDefense();
            void ActLoopGoDefense2();
            void ActLoopGoDefense3();
            void ActLoopGoDefPassCourse();
            void ActLoopGoDefOnBaller();
            void ActLoopGoHome();
            void ActLoopGoFreeArea();
            void ActLoopGetBall();
            void ActLoopGetHatenaPanel();
            void ActLoopGoRunRand();
            void ActLoopGoRun();
            void ActLoopPass();
            void ActLoopPassGhost();
            void ActLoopShootCancelPass();
            void ActLoopCancelPass();
            void ActLoopPassSign();
            void ActLoopJumpShoot();
            void ActLoopDunk();
            void ActLoopGoReboundDunk();
            void ActLoopReboundDunk();
            void ActLoopAlleyoop();
            void ActLoopAssistPlay();
            void ActLoopBlockade();
            void ActLoopGoBack();
            void ActLoopGoZiguzagu();
            void ActLoopGoSide();
            void ActLoopSpinMove();
            void ActLoopAirTrick();
            void ActLoopStickDash();
            void ActLoopGoTargetGoal();
            void ActLoopBallKeep();
            void ActLoopGoDunkArea();
            void ActLoopGoDunk();
            void ActLoopLayUp();
            void ActLoopCancelLayUp();
            void ActLoopItemShoot();
            void ActLoopCombination();
            void ActLoopGoEnemy();
            void ActLoopGoBall();
            void ActLoopRebound();
            void ActLoopCut();
            void ActLoopMissCut();
            void ActLoopJump();
            void ActLoopDefJump();
            void ActLoopJumpCut();
            void ActLoopJumpUpper();
            void ActLoopDefAlleyoop();
            void ActLoopItemEscape();
            void ActLoopGoEscape();
            void ActLoopGoEscape2();
            void ActLoopItemThrow();
            void ActLoopDefGhost();
            void ActLoopCutChance();
            void ActLoopGoSmash();
            void ActLoopMoveBlock();
            void ActLoopGoDefenseGoal();
            void ActLoopShootBlock();
            void ActLoopJumpUpperChance();
            void ActLoopJumpCutChance();
            void ActLoopGoPointPanel();
            void ActLoopGetGShell();
            void ActLoopGoEspBubble();
            void ActLoopScript();
            // Whether rPos (local) is far enough to run to.
            int CheckMoveToTargetDist(bcs::FVEC3 pos) const;
            int IsNoStopDefense() const;
            // Whether the character to mark changed.
            int CheckChangeMarkCore() const;
            int In3PointArea() const;
            float GetJumpShootJumpDir() const;
            void Get3PointAreaRandPos(bcs::FVEC3 &rPos, float f0, float f1, long nArg);

            // (const: the table is in .rodata)
            static const ActFunc g_ActFuncMap[];

        protected:
            // 0x170 (name guessed): the character the driver marks on
            // defence; null for none.
            const CChrCore *m_pMarkChr;
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
            float m_fUnk1B8;  // 0x1B8, the yaw to ActCallGoRun's target
            float m_fUnk1BC;  // 0x1BC, a timer counted down to 0
            float m_fUnk1C0;  // 0x1C0, the timer of the star state's panel draw
            float m_fUnk1C4;  // 0x1C4, counts down; at 0 restarts at 30
            float m_fUnk1C8;  // 0x1C8, set to -1 when m_fUnk1C4 runs out
            float m_fUnk1CC;  // 0x1CC, the timer of flag 0x20
            unsigned char m_Unk1D0[0x1D4 - 0x1D0];
            float m_fUnk1D4;  // 0x1D4, a timer counted down to 0
            float m_fUnk1D8;  // 0x1D8, the timer of the boss Packun jump shot
            float m_fUnk1DC;  // 0x1DC, the timer of flag 0x40
            unsigned char m_Unk1E0[0x1E8 - 0x1E0];
            // 0x1E8 (meaning not known): with 0 MethdBKRun2 measures the
            // arrival from m_MyPos to the asked position, else from the
            // character to the corrected one.
            long m_nUnk1E8;
            unsigned char m_Unk1EC[0x1FC - 0x1EC];
            float m_fUnk1FC;  // 0x1FC, a timer (message 0x26)
            unsigned char m_Unk200[0x688 - 0x200];
    };
}

#endif /* !MSW_CCOMDRVBASKETGAME_HPP_ */
