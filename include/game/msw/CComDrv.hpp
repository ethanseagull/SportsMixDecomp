#ifndef MSW_CCOMDRV_HPP_
#define MSW_CCOMDRV_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrDrv.hpp"

namespace msw {

    class CBall;
    class CChrCore;

    // INCOMPLETE: the computer driver of one character. A DOL class; each
    // game module derives its own (CComDrvDodge in DodgeF).
    //
    // The virtual functions are in the order of the DOL's vtable
    // (lbl_8046EB08), with the names DodgeF's __vt__Q23msw12CComDrvDodge
    // gives them. The ones with a body are those DodgeF emits as weak
    // functions after CComDrvDodge's code (in reverse declaration order), so
    // they are inline. Other return types are guesses.
    //
    // Data: m_nMyTeam, m_pChr and m_pBall are mapped. layout.py finds
    // members up to 0x16C and CComDrvHockey's start at 0x170, so the size is
    // taken as 0x170 (no allocation of CComDrv itself was found).
    class CComDrv : public CChrDrv {
        public:
            // (added for VolleyF's CComDrvVolleyGame units; names guessed)
            // The start of every action's work (EntryAct, GetCurrentActWork):
            // the step the action is at and a timer the driver counts up;
            // each action's own members follow from 0x1C.
            struct ACT_WORK {
                unsigned long m_nUnk00;
                long m_nStep;     // 0x04
                float m_fTime;    // 0x08
                float m_fTime2;   // 0x0C
                unsigned char m_Unk10[0x18 - 0x10];
                // 0x18 (name guessed): frames since the action started;
                // BossF's CComDrvBoss gives up an action after a while.
                float m_fActTime;

            };

            CComDrv(long nArg);
            virtual ~CComDrv();

            virtual void MsgFunc(CChrCore *pChr, MSG_ID id, _CHR_MSG *pMsg);
            virtual long GetDrvType() const { return 3; }
            virtual long GetCourtIdxByPosForArea(const bcs::FVEC3 &pos, int, int) const;
            virtual long RunCorrect_GetFirstRotDirAroundChr(const bcs::FVEC3 &pos, float, float) const { return -1; }
            virtual int Obstacle_IsIgnoreMyTeamSpObstacle() const;
            virtual void Obstacle_GetParamObstacleToHitMe(long, float &, float &) const;
            virtual void Obstacle_GetParamEscPosFromObstacle(long, float &, float &) const;
            virtual int ReturnAutoRunMode() const { return 0; }
            virtual int MethdJump();
            virtual int MethdRotateY(const bcs::FVEC3 &pos, long);
            virtual int MethdRotateY(float fRot, long);
            virtual int MethdRun(const bcs::FVEC3 &pos, float, int);
            virtual int MethdRunEx(const bcs::FVEC3 &pos, float, float, float, unsigned long, float);
            virtual int IsExistChrToTargetWorldForRunEx(const bcs::FVEC3 &pos);
            virtual void GetPosTurnAroundForRunEx(bcs::FVEC3 &pos);
            virtual void GetViewPointPosForRunEx(bcs::FVEC3 &pos);
            virtual int CanbeItemShoot() const;
            virtual int MethdItemShoot(const bcs::FVEC3 &pos);
            virtual int MethdItemShoot();
            virtual int MethdShoot(const bcs::FVEC3 &pos);
            virtual int MethdShoot();
            virtual int MethdPass(const bcs::FVEC3 &pos);
            virtual int MethdPass(const bcs::FVEC3 &pos, int) { return MethdPass(pos); }
            virtual int MethdPass();
            virtual int MethdPass(CChrCore *pChr);
            virtual int MethdPass(long);
            virtual int MethdAirTrick();
            virtual int MethdTrickMove();
            virtual int MethdSetMoveRevise(const bcs::FVEC3 &pos);
            // A name for an action number, for debugging.
            virtual const char *GetActName(long nAct) const { return "none"; }
            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void EntryActCallBack(long) {}
            virtual void NormalizeCanMovePos(bcs::FVEC3 &pos) const;
            virtual void NormalizeCanMoveWorldPos(bcs::FVEC3 &pos) const;
            virtual void Reset();
            virtual float GetActCountLimitFrame() const;
            virtual int IsActiveBallNothingThk() const { return 0; }
            virtual void BeforeMsgFunc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void EndMsgFunc(MSG_ID id, _CHR_MSG *pMsg);
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual void OnConnect(_CHR_MSG *pMsg);
            virtual void OnDisconnect(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            // Returns a long (HockeyF's CComDrvHockeyGame::OnDamage returns 0).
            virtual long OnDamage(_CHR_MSG *pMsg);
            // Returns the answer to the message (BasketF's CComDrvBasketHB
            // returns this function's result for the messages it does not
            // answer itself).
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void OnAlarm0(long) {}
            virtual void OnAlarm1(long) {}
            virtual void OnAlarm2(long) {}
            virtual void OnAlarm3(long) {}
            virtual void ActInterrupt() {}
            virtual void ThkMain() {}

            // DOL. pos (relative to the character, guessed from the name)
            // turned into a world position; passes the long on to
            // CComMan::Local2World.
            bcs::FVEC3 Local2World(const bcs::FVEC3 &pos, long) const;
            // DOL. The index of the ball in play (CComMan's); negative for
            // none.
            long GetActiveBallIdx() const;
            // DOL. Distance between two points in the XZ plane.
            float GetDisXZ(const bcs::FVEC3 &rPos0, const bcs::FVEC3 &rPos1) const;
            // DOL. Added for VolleyF game/volley/unit_CA990.cpp; return
            // types guessed from how it uses them. The character (index,
            // negative for none) or item (zero for none) in the way of
            // rPos, and the position to go round it, given the direction a
            // previous call returned (-1 for a new one).
            long RunCorrect_GetChrToTarget(const bcs::FVEC3 &rPos) const;
            long RunCorrect_GetPosTurnAroundChr(bcs::FVEC3 &rPos, long nChr, long nDir) const;
            long Item_GetItemToTarget(const bcs::FVEC3 &rPos) const;
            long Item_GetPosTurnAroundItem(bcs::FVEC3 &rPos, unsigned long nItem, long nDir, int) const;
            // (int: DodgeF's CComDrvDodgeGame::GetViewPointPosForRunEx tests it)
            int Item_GetViewPointPosFromItem(bcs::FVEC3 &rPos) const;
            // (DOL; added for DodgeF game/dodge/CComDrvDodgeGameGimmick.cpp,
            // return types guessed from how it uses them; the action, item
            // and obstacle helpers it also uses are declared with VolleyF's
            // below) The ball in play; the left and right edges of the
            // character's court; an escape position rPos corrected for what
            // is around it.
            CBall *GetActiveBall() const;
            float GetMyCourtLeft() const;
            float GetMyCourtRight() const;
            void RunCorrect_RenewEscPos(bcs::FVEC3 &rPos, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rDir, float fLen) const;
            // DOL. A world position brought into the court's own frame
            // (passes the long on as Local2World does).
            bcs::FVEC3 World2Local(const bcs::FVEC3 &rPos, long) const;
            // DOL. A random number below nMax. (range guessed from the name;
            // BasketF tests RandMaxI(999) < 20)
            long RandMaxI(long nMax) const;
            // DOL. A random number from nMin to nMax (added for DodgeF
            // game/dodge/unit_A40EC.cpp; whether nMax is included is not known).
            long RandRangeI(long nMin, long nMax) const;
            // DOL. A random percentage to test a rate against (added for
            // HockeyF game/hockey/unit_AFCA0.cpp, which compares it with a
            // rate clamped to 0-100).
            long LotRate() const;
            // DOL. The size of the per-action work CComDrv's own actions
            // need (0x5C); a derived driver passes the larger of this and
            // its own to the constructor (DodgeF CComDrvDodgeDD).
            unsigned long CalcWorkBaseMaxSize() const;
            // DOL. The work of the action running now (zero for none).
            void *GetCurrentActWork();
            // (DOL; added for DodgeF game/dodge/CComDrvDodgeGameOffRun.cpp)
            // Start the common actions: turn to the target or to rPos. (The
            // result EntryAct/ReturnAct pass, as DodgeF uses it: 0 done, 1
            // given up.)
            void ActCallLookTarget();
            void ActCallLookTarget(const bcs::FVEC3 &rPos);
            // (DOL; added for VolleyF game/volley/CComDrvVolleyRR.cpp) Turn
            // to face the front.
            void ActCallLookFront();
            // (DOL) A random number between fMin and fMax (RandRangeF below
            // is the other generator; which is which is not known).
            float RandRange(float fMin, float fMax) const;

            // (DOL; added for BossF game/boss/CComDrvBoss.cpp) How many actions
            // are entered (1: the one starting is the first); whether the
            // character holds an item it can throw.
            long GetEntryActNum() const;
            // (DOL; BossF tests its lowest bit for a coin toss) A random number.
            long RandI() const;
            // (DOL; added for BossF game/boss/unit_51CE4.cpp, meanings guessed)
            // A random number from 0 to 1; the action running now (negative
            // for none); runs the action machinery for a frame, not zero when
            // the action's loop function is to be called; whether team nTeam
            // (-1: the driver's) holds the ball; sets alarm nAlarm (OnAlarm0..3)
            // to go off after fFrame frames.
            float RandF() const;
            long GetCurrentActType() const;
            int ActMain();
            int IsMyTeamHoldBall(long nTeam) const;
            void SetAlarm(long nAlarm, float fFrame);
            int IsHaveMyStockItem() const;

            // (DOL; added for VolleyF's CComDrvVolleyGame units; return types
            // and meanings guessed from the names and the callers)
            // Starts action nAct and returns its work; ends the running one
            // with a result.
            void *EntryAct(long nAct);
            void ReturnAct(long nRet);
            // (DOL; added for HockeyF game/hockey/unit_CE254.cpp) Whether
            // action nAct is among the entered (running) actions.
            int IsEntryAct(long nAct) const;
            void ActCallWait(float fFrame);
            long GetRootActType() const;
            float GetComLvF() const;
            float GetComBaseLvF() const;
            int IsExistPadDrv() const;
            // (DOL; added for HockeyF game/hockey/unit_AE7E8.cpp) The number
            // of pad-driven characters (meaning guessed from the name).
            long GetExistPadDrvNum() const;
            CChrCore *GetLastSpChrCore() const;
            long GetMyStockItemType() const;
            float RandRangeF(float fMin, float fMax) const;
            long LotProbability(const long *pRate, long nNum) const;
            float GetEmCourtLeft() const;
            float GetEmCourtRight() const;
            float GetEmCourtFront() const;
            float GetEmCourtBack() const;
            float GetMyCourtCenterX() const;
            float GetMyCourtCenterZ() const;
            unsigned long Item_GetItemToHitMe() const;
            int Item_GetEscPosFromItem(bcs::FVEC3 &rPos, unsigned long nItem) const;
            unsigned long Obstacle_GetObstacleToHitMe(unsigned long nKindBits) const;
            int Obstacle_GetEscPosFromObstacle(bcs::FVEC3 &rPos, unsigned long nObstacle, int) const;
            unsigned long Obstacle_GetObstacleToTarget(const bcs::FVEC3 &rPos, unsigned long nKindBits) const;
            int Obstacle_IsSpObstacleHandle(unsigned long nObstacle) const;
            long Obstacle_GetPosTurnAroundObstacle(bcs::FVEC3 &rPos, unsigned long nObstacle, long nDir, int) const;
            int RunCorrect_GetViewPointPosFromChr(bcs::FVEC3 &rPos) const;
            // (DOL; added for VolleyF game/volley/unit_C0918.cpp; meaning
            // guessed) rPos (in the court's frame) as a world position in
            // rOut, and how far the character is from it; fArg is passed on.
            float GetWorldTargetDis(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, float fArg) const;

            // DOL. The common actions, one per frame while running; derived
            // drivers list them in their action tables (g_ActFuncMap).
            // Return type guessed (ActLoopNop tail-calls ReturnAct).
            void ActLoopNop();
            void ActLoopWait();
            void ActLoopWaitNeutral();
            void ActLoopLookTarget();
            void ActLoopLookFront();
            void ActLoopLookBall();
            void ActLoopRun();
            void ActLoopRunTurn();
            void ActLoopJump();
            void ActLoopJumpBlock();

        protected:
            // 0x04 (name guessed): flag bits; the volley driver's actions
            // and DodgeF CComDrvDodgeGame's off-ball runs clear 0x4 together
            // with m_fUnk54 before a wait or a turn to their target.
            unsigned long m_nFlag;
            unsigned char m_Unk08[0x54 - 0x08];
            float m_fUnk54;   // 0x54
            unsigned char m_Unk58[0x64 - 0x58];
            // 0x64 (name guessed): the driven character's team. HockeyF
            // passes it to CComManHockey::GetKeeperCore, VolleyF compares it
            // with GameWork::GetUserTeam.
            long m_nMyTeam;
            CChrCore *m_pChr; // 0x68, the character driven
            // 0x6C (name guessed): the driven character's index (the
            // volley driver compares it with GetShoudTwoAttackChrIdx).
            long m_nMyChrIdx;
            // 0x70 (name guessed): the driven character's position, kept
            // by ClearAll/UpdateInfo (VolleyF CComDrvVolley::CanbeDiveReceive
            // measures from it; DodgeF hands it to CComMan::GetZtoCenter).
            bcs::FVEC3 m_MyPos;
            // 0x7C (name guessed): the character's position in the world;
            // DodgeF's gimmick escapes measure from it.
            bcs::FVEC3 m_MyWorldPos;
            unsigned char m_Unk88[0x94 - 0x88];
            // 0x94, 0x98 (names guessed): passed to CComMan's
            // GetStageCourtCenterX/Z and to CComOSG::Init by the volley
            // driver; a court number and the grid's kind.
            long m_nCourtIdx;
            long m_nUnk98;
            unsigned char m_Unk9C[0xC0 - 0x9C];
            // 0xC0, 0xCC (name guessed): with m_MyPos the positions of the
            // driver's team (VolleyF's Ninja defence passes the three to
            // CComMan::GetNearBestPos2on3).
            bcs::FVEC3 m_aMatePos[2];
            unsigned char m_UnkD8[0x108 - 0xD8];
            // 0x108 (name guessed): how many team mates m_aMatePos holds.
            long m_nMateNum;
            // 0x10C (name guessed): the other team (CGame::GetTeamScore,
            // CComMan::GetChrCoreRand).
            long m_nEmTeam;
            // 0x110 (name guessed): the ball the driver follows, set by
            // UpdateInfo; VolleyF hands it to CComManVolley.
            CBall *m_pBall;
            // The size 0x170 is where CComDrvHockey's own members start;
            // the DOL's functions use members up to 0x16C.
            // 0x114 (name guessed): where that ball is (BossF's CComDrvBoss
            // runs to it).
            bcs::FVEC3 m_BallPos;
            unsigned char m_Unk120[0x160 - 0x120];
            // 0x160 to 0x16C (names guessed): the obstacle a gimmick
            // driver is turning round (VolleyF CComDrvVolleyRR::
            // GetPosTurnAroundGimmick). Kind: -1 none, 1 a character, 2 an
            // item; the target is the character index or the item.
            long m_nTurnKind;     // 0x160
            long m_nTurnTarget;   // 0x164
            long m_nTurnDir;      // 0x168, what the RunCorrect/Item helpers returned
            float m_fTurnFrame;   // 0x16C, set to 60 when a turn starts
    };
}

#endif /* !MSW_CCOMDRV_HPP_ */
