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

            // Added for game/dodge/CComDrvDodgeGameOffRun.cpp. The off-ball
            // runs (to a charge spot, forward, back, sideways, to a
            // character) and waiting for a catch in the infield.
            void ActCallOffRunCharge();
            void ActLoopOffRunCharge();
            void ActCallOffRunFront();
            void ActLoopOffRunFront();
            void ActCallOffRunBack();
            void ActLoopOffRunBack();
            void ActCallOffRunSide();
            void ActLoopOffRunSide();
            void ActCallOffRunChr(long nChr, float fSpeed);
            void ActLoopOffRunChr();
            void ActCallInFieldCatchWait();
            void ActLoopInFieldCatchWait();

            // Defined in game/dodge/CComDrvDodgeGame.cpp and the files after
            // it (return types guessed).
            void ActCallBaseWait(float fFrame);
            void SetOffStanceMode(long nMode);
            void GetOffRunChargePos(bcs::FVEC3 &rPos) const;
            void GetOffRunFrontPos(bcs::FVEC3 &rPos) const;
            void GetOffRunBackPos(bcs::FVEC3 &rPos) const;
            void GetOffRunSidePos(bcs::FVEC3 &rPos) const;
            // What to do once at the charge spot: 0 shoot when charged,
            // 2..8 pass or shoot (see ActLoopOffRunCharge).
            long LotActChargeAfterOffRunCharge() const;
            CChrCore *GetPassTargetChrCore() const;

            // Added for game/dodge/CComDrvDodgeGameGimmick.cpp: escaping
            // and going round the stage gimmicks, bombs and obstacles.
            virtual long RunCorrect_GetFirstRotDirAroundChr(const bcs::FVEC3 &pos, float fLeft, float fRight) const;
            virtual void GetPosTurnAroundForRunEx(bcs::FVEC3 &pos);
            virtual void GetViewPointPosForRunEx(bcs::FVEC3 &pos);

            // (added for game/dodge/unit_D7800.cpp) The virtuals this class
            // adds, in the order of its vtable (slots 59 to 70, 0xEC to
            // 0x118, names from DodgeF's symbols; return types guessed).
            virtual int CanbePassBossPackun();
            virtual int MethdPassBossPackun();
            virtual int CanbeDivingCatch() const;
            virtual int MethdDivingCatch(const bcs::FVEC3 &pos);
            virtual int MethdSwitchAutoAimTarget();
            virtual int CanbeQuickTurn();
            virtual int MethdQuickTurn();
            virtual int MethdBallCatch();
            virtual int CanbeSp() const;
            virtual int MethdSp();
            // Whether the special shot's target marker can be moved now;
            // move it towards pos (fRate: how fast, negative for the
            // default, as in CComDrvVolleyGame).
            virtual int CanbeMoveSpTargetMarkerPos() const;
            virtual int MethdMoveSpTargetMarkerPos(const bcs::FVEC3 &pos, float fRate) const;

            // (game/dodge/unit_D7800.cpp and the files around it) Answering
            // a special shot of the driver's own team by moving its target
            // marker. Other return types guessed.
            void ActCallGoHome();
            void ActLoopSpOffMoveMarker();
            long GetDatSpTargetMarker(float *pTime, int bEarly);
            int GetMoveSpTargetMarkerPos(bcs::FVEC3 &rPos, long nKind, long &rChr);
            void WraperActCallOffSpStart();
            void WraperActCallOffSpSub();
            // Where the marker is now; zero when there is none.
            int GetSpTargetMarkerPos(bcs::FVEC3 &rPos) const;
            // Characters of the other team in the infield (index, negative
            // for none): the nearest to pos, the one in bad condition, the
            // ones with the least and the most life.
            long GetNearEmInnerChr(const bcs::FVEC3 &pos, int, int) const;
            long GetBadConditionEmInnerChr() const;
            long GetLifeFewEmInnerChr() const;
            long GetLifeLotEmInnerChr() const;

            int ShoudEscGimmick() const;
            int GetEscPosFromGimmick(bcs::FVEC3 &rPos) const;
            int GetSafePosFromGimmick(bcs::FVEC3 &rPos) const;
            int GetPosTurnAroundGimmick(bcs::FVEC3 &rPos);
            int ItemBall_GetEscPosFromBall(bcs::FVEC3 &rPos, long nBall) const;
            long RunCorrect_GetFirstRotDir(const bcs::FVEC3 &pos, float fLeft, float fRight) const;
            // The bomb ball about to blow up next to the character (its
            // number in CBallMan's table, negative for none); inline, both
            // ShoudEscGimmick and GetEscPosFromGimmick have it.
            inline long ItemBall_GetBallToHitMe() const;

            // (game/dodge/unit_BE1FC.cpp, return types guessed from how
            // the gimmick file uses them) The stage gimmicks: stage 4's
            // pylons, stage 7's mud bombs, stage 12's pinballs, stage 15's
            // cases, stage 17's trolleys.
            long Stage04_GetPylonToTarget(const bcs::FVEC3 &rPos) const;
            long Stage04_GetPosTurnAroundPylon(bcs::FVEC3 &rPos, long nPylon, long nDir) const;
            int Stage04_GetViewPointPosFromPylon(bcs::FVEC3 &rPos) const;
            unsigned long Stage07_GetMudBombToTarget(const bcs::FVEC3 &rPos) const;
            long Stage07_GetPosTurnAroundMudBomb(bcs::FVEC3 &rPos, unsigned long nBomb, long nDir) const;
            int Stage07_GetViewPointPosFromMudBomb(bcs::FVEC3 &rPos) const;
            long Stage12_GetPinballToHitMe(float) const;
            int Stage12_GetEscPosFromPinball(bcs::FVEC3 &rPos, long nPinball) const;
            long Stage12_GetPinballToTarget(const bcs::FVEC3 &rPos, float) const;
            int Stage12_GetSafePosFromPinball(bcs::FVEC3 &rPos, long nPinball) const;
            long Stage15_GetCaseToTarget(const bcs::FVEC3 &rPos, float) const;
            long Stage15_GetPosTurnAroundCase(bcs::FVEC3 &rPos, long nCase, long nDir) const;
            long Stage17_GetTrolleyToHitMe() const;
            int Stage17_GetEscPosFromTrolley(bcs::FVEC3 &rPos, long nTrolley) const;
            int Stage17_GetViewPointPosFromTrolley(bcs::FVEC3 &rPos) const;

            // (added for game/dodge/unit_A40EC.cpp) The frame, message and
            // alarm handlers this driver overrides.
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void OnAlarm0(long nAlarm);
            virtual void OnAlarm1(long nAlarm);
            virtual void OnAlarm2(long nAlarm);

            // (added for game/dodge/unit_A4AE8.cpp) The other overrides
            // defined there, and the think helpers ThkMain uses (return
            // types guessed).
            virtual int MethdRotateY(const bcs::FVEC3 &pos, long);
            virtual int MethdRotateY(float fRot, long);
            virtual int IsExistChrToTargetWorldForRunEx(const bcs::FVEC3 &pos);
            virtual int MethdShoot();
            virtual int MethdPass(const bcs::FVEC3 &pos);
            virtual int MethdPass(CChrCore *pChr);
            virtual int MethdPass(long nChr);
            virtual void ActInterrupt();
            virtual void ThkMain();

            // (game/dodge/unit_A4AE8.cpp) What ThkMain does in each case:
            // in the infield with the ball (and an enemy in the own court),
            // when the other team or a team mate in or out has it; in the
            // outfield with and without it. Each draws one action by the
            // level's rates.
            void ThkMain_InFieldOnBall();
            void ThkMain_InFieldOnBall_EmMyCourt(long nChr);
            void ThkMain_InFieldEmOnBall();
            void ThkMain_InFieldMyTeamInOnBall();
            void ThkMain_InFieldMyTeamOutOnBall();
            void ThkMain_OutFieldOnBall();
            void ThkMain_OutFieldOffBall();
            // The shot's rate nRate raised when the locked target is down
            // and close, kept within 0 to 1000.
            long ShootChanceHosei(long nRate);

            // (defined in the other files of the class; return types and
            // meanings guessed from the names)
            void ActCallJumpBall();
            void ActCallRunRand();
            void ActCallRunEsc();
            void ActCallGetBall();
            void ActCallGoBack();
            void ActCallOutFieldGetBall();
            void ActCallOutFieldOpposite(int);
            void ActCallOutFieldHome();
            void ActCallOutFieldRand();
            void ActCallOutFieldGetQpanel();
            void ActCallOutFieldGetDropCoin();
            void ActCallOnballAttack(long nKind);
            void ActCallChangeTarget();
            void ActCallGetQPanel();
            void ActCallGetDropCoin();
            void ActCallCombination();
            void ActCallThrowItem();
            void ActCallPassSteal(long nKind);
            void ActCallDodgeRoll(long nKind);
            void ActCallDivingCatch();
            void ActCallEscGimmick();
            int IsSpMode() const;
            int IsNoBorder() const;
            int IsShoudGetQPanelorFallGShell() const;
            int IsShoudGetDropCoin() const;
            int ShoudMissDodgeRoll() const;
            int ShoudDivingCatch() const;
            int ShoudGetBall() const;
            int ShoudItemThrowAndQPanel() const;
            void UpdateStance();
            void SetDefStanceMode(long nMode);
            // The other team's character in the driver's court nearest to
            // rPos, the one in the other zone to aim at, the outfield
            // character opposite the ball (indices, negative for none); the
            // free ball nearest (its index, negative for none; rPos is set).
            long GetNearEmChrCoreInMyCourt(const bcs::FVEC3 &rPos) const;
            long GetTargetChrInEmZone() const;
            long GetNearBallOppositeChr(int);
            long GetNearFreeBallPos(bcs::FVEC3 &rPos) const;
            // Where in the court the ball holder is (0 to 3, negative for
            // nowhere to throw from).
            long Util_GetOnBallArea() const;
            long Util_GetMaybeHitChrBall(CChrCore *pChr, float, float);

            // (game/dodge/unit_A40EC.cpp; defined in the other files of the
            // class, return types and meanings guessed from the names and
            // how the handlers use them)
            void UpdateAssignOuterChr();
            void SetAssignOuterChr();
            void UpdateHomePos();
            void GetHomePos(bcs::FVEC3 &rPos) const;
            void SetGetBallWait(float fFrame);
            void FeintInit();
            void ActCallOutFieldCatchWait();
            void ActCallOutFieldRunCatchPos();
            void ActCallPassCatch();
            // The feint the character falls for (negative for none), and
            // the reaction to it.
            long LotFeintDamage(long nChr);
            void ActCallFeintReaction(long nFeint);
            long GetStanceResult();
            long GetCatchResult(long, long);
            long GetLockPosValue(float);
            void LotPassStealRate();
            void LotDodgeRollRate(long);
            void LotShootDefStance();
            void LotDivingCatchRate();
            void LotRateSpBefore();
            void WraperActCallDefSpStart();
            // The outfield character of the driver's team nearest to pos.
            long Util_GetNearMyTeamOuterChr(const bcs::FVEC3 &pos);
            inline int IsChrHoldBall() const;

            // An action: a member called each frame while it runs.
            typedef void (CComDrvDodgeGame::*ActFunc)();
            // The actions by number (CComDrv's common ones, then this
            // driver's own); defined with the class.
            static ActFunc g_ActFuncMap[];

        protected:
            unsigned char m_Unk170[0x1E8 - 0x170];
            // 0x1E8, 0x1EC (set elsewhere; names guessed): a timer and its
            // flag; ActInterrupt counts the timer down and, when it runs
            // out, clears the flag and calls SetDefStanceMode(1).
            float m_fDefStanceTime;
            long m_bDefStanceTime;
            // 0x1F0 (meaning not known): passed to GetLockPosValue. A random
            // number from -1 to 1, drawn again by ActInterrupt every 30 to
            // 60 frames (0x1F4 counts them down).
            float m_fUnk1F0;
            float m_fUnk1F0Time;    // 0x1F4
            // 0x1F8 (name guessed): frames the ball has been free to catch
            // without flying at the driver's team.
            float m_fFreeBallTime;
            unsigned char m_Unk1FC[0x204 - 0x1FC];
            // 0x204 (name guessed): set before a shot at the end of the
            // charge run, 1 for the follow-up actions 2 and 6.
            long m_nShootKind;
            unsigned char m_Unk208[0x20C - 0x208];
            long m_nUnk20C;         // 0x20C, cleared when the driver gets the ball
            unsigned char m_Unk210[0x214 - 0x210];
            // 0x214, 0x21C (set elsewhere; meaning not known): a flag and a
            // timer; ActInterrupt clears both when the timer runs out.
            long m_nUnk214;
            unsigned char m_Unk218[0x21C - 0x218];
            float m_fUnk21C;
            // 0x220, 0x224 (set elsewhere; names guessed): the pass steal
            // and the dodge roll the driver is ready for (0 none, 1 or 2 a
            // kind), handed to ActCallPassSteal/ActCallDodgeRoll.
            long m_nPassStealKind;
            long m_nDodgeRollKind;
            // 0x228 (meaning not known): cleared when the ball is caught;
            // OnAlarm2 sets it to 1 by lot unless it is negative.
            long m_nUnk228;
            // 0x22C to 0x22F (names guessed): what the driver escapes:
            // bomb items, other items, obstacles; the last one is passed
            // negated to the obstacle helpers.
            char m_bEscItemBomb;    // 0x22C
            char m_bEscItem;        // 0x22D
            char m_bEscObstacle;    // 0x22E
            char m_bUnk22F;         // 0x22F
            char m_bUnk230;         // 0x230, also drawn by OnAlarm2
            unsigned char m_Unk231[0x234 - 0x231];
            // 0x234 (name guessed): how far the thrower was from the
            // character when a shot came.
            float m_fShooterDis;
            // 0x238 (meaning not known): passed to Util_GetNearMyTeamOuterChr
            // and set by ActCallOutFieldRunCatchPos.
            bcs::FVEC3 m_Unk238;
            // 0x244 (name guessed): set while the driver's character is
            // the outfield character its team has assigned.
            long m_bAssignOuter;
            unsigned char m_Unk248[0x250 - 0x248];
            // 0x250 (name guessed): not zero when the driver escapes
            // gimmicks at all.
            long m_nEscGimmick;
            long m_nUnk254;         // 0x254, set when a special shot starts
            // 0x258 (meaning not known): when set, the special shot's
            // target is answered by lot (OnGameMsgProc 0x21).
            char m_bUnk258;
            unsigned char m_Unk259[0x25C - 0x259];
            // 0x25C, 0x260 (set by ActCallCombination; meaning not known):
            // a flag and a timer that counts up from below zero.
            long m_nUnk25C;
            float m_fUnk260;
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
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
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
