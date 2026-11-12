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

        protected:
            unsigned char m_Unk170[0x204 - 0x170];
            // 0x204 (name guessed): set before a shot at the end of the
            // charge run, 1 for the follow-up actions 2 and 6.
            long m_nShootKind;
            unsigned char m_Unk208[0x22C - 0x208];
            // 0x22C to 0x22F (names guessed): what the driver escapes:
            // bomb items, other items, obstacles; the last one is passed
            // negated to the obstacle helpers.
            char m_bEscItemBomb;    // 0x22C
            char m_bEscItem;        // 0x22D
            char m_bEscObstacle;    // 0x22E
            char m_bUnk22F;         // 0x22F
            unsigned char m_Unk230[0x250 - 0x230];
            // 0x250 (name guessed): not zero when the driver escapes
            // gimmicks at all.
            long m_nEscGimmick;
            unsigned char m_Unk254[0x264 - 0x254];
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
