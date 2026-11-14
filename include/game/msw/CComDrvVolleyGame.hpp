#ifndef MSW_CCOMDRVVOLLEYGAME_HPP_
#define MSW_CCOMDRVVOLLEYGAME_HPP_

#include "game/msw/CComDrvVolley.hpp"

namespace msw {

    class CComOSG;

    // The computer driver of the normal volleyball game (VolleyF), size
    // 0x1FC (what CChrManVolley::CreateDriver_ allocates). Most of its
    // functions are in game/volley/CComDrvVolleyGame.cpp; the actions of
    // the special shots, the interrupt and think functions and the gimmick
    // helpers are in four smaller files (game/volley/unit_AB1D8.cpp,
    // unit_BEF84.cpp, unit_BF960.cpp, unit_C5D08.cpp).
    //
    // Virtual functions: the overrides, then six of its own (0xF8-0x10C)
    // in the order of __vt__Q23msw17CComDrvVolleyGame. The member functions
    // are every CComDrvVolleyGame symbol of VolleyF; return types and the
    // meaning of the parameters are guesses unless a caller shows them
    // (Act* return nothing; Is*, Shoud*, Lot*, Canbe* and the Get*Pos ones
    // return a flag; Get*Idx an index; Get*Dis a distance).
    class CComDrvVolleyGame : public CComDrvVolley {
        public:
            CComDrvVolleyGame();
            virtual ~CComDrvVolleyGame();

            virtual int IsExistChrToTargetWorldForRunEx(const bcs::FVEC3 &rPos);
            virtual void GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos);
            virtual void GetViewPointPosForRunEx(bcs::FVEC3 &rPos);
            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void NormalizeCanMoveWorldPos(bcs::FVEC3 &rPos) const;
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void OnAlarm0(long);
            virtual void OnAlarm1(long);
            virtual void OnAlarm2(long);
            virtual void ActInterrupt();
            virtual void ThkMain();
            virtual int IsMyTurn() const;

            virtual int CanbeSpike() const;                                          // 0xF8
            virtual int MethdSpike();                                                // 0xFC
            // The special shot (the character's air trick).
            virtual int CanbeSp() const;                                             // 0x100
            virtual int MethdSp();                                                   // 0x104
            virtual int CanbeMoveTargetMarkerPos() const;                            // 0x108
            // Moves the target marker of a special shot towards rPos; returns
            // 1 while it is not there (closer than fRange) yet.
            virtual int MethdMoveTargetMarkerPos(const bcs::FVEC3 &rPos, float fRange) const; // 0x10C

            void SetComLv();
            int IsSpMarkerMode() const;
            unsigned long GetMyTeamCanTouchEnableBit() const;
            unsigned long GetMyTeamPadDrvCanTouchEnableBit() const;
            void UpdateAPosition();
            void UpdateHomePos();
            void GetHomePos(bcs::FVEC3 &rPos, int) const;
            int IsExistPadChrHomePosIdx(long nIdx) const;
            void GetDefHomePos(bcs::FVEC3 &rPos) const;
            void GetSymmetryPos(bcs::FVEC3 &rPos) const;
            int IsExistCurveSpikeEmChrCore() const;
            CChrCore *GetEmTargetMarkerChrCore() const;
            int GetEmTargetMarkerPos(bcs::FVEC3 &rPos) const;
            long GetShoudFollowTargetChrIdx(const bcs::FVEC3 &rPos, int) const;
            int GetOpenSpaceInEmCourt(bcs::FVEC3 &rPos, const bcs::FVEC3 *pFrom, int) const;
            int GetOpenSpaceInMyCourt(bcs::FVEC3 &rPos, const bcs::FVEC3 *pFrom, int) const;
            int IsOpenSpaceForSpike(const bcs::FVEC3 &rPos, float) const;
            void LotRateTouch(long);
            void LotRateDefense(long);
            void LotDefPosKind(long);
            void LotDefPosHosei();
            int LotRateBlock(long) const;
            void LotRateOffense();
            void LotRateReceiveFinished();
            void LotRateTwoAttack();
            void LotRateQuickSpike();
            void LotRateSpikeBefore();
            int LotRateGetQPanelGood() const;
            int LotRateGetQPanelBad() const;
            int LotRateGetDropCoinGood() const;
            int LotRateGetDropCoinBad() const;
            int LotRateItemThrowTimingGood() const;
            int LotRateItemThrowTimingBad() const;
            void LotRateSpBefore();
            void GetServePos(bcs::FVEC3 &rPos) const;
            int GetServeTargetMarkerPos(bcs::FVEC3 &rPos) const;
            int GetSpikeTargetMarkerPos(bcs::FVEC3 &rPos) const;
            // Where to move a special shot's target marker (nKind from
            // GetDatSpTargetMarker).
            int GetMoveTargetMarkerPos(bcs::FVEC3 &rPos, long nKind) const;
            int ShoudTargetMarkerGo() const;
            int ShoudInterThrowItem() const;
            int ShoudSpDefensive() const;
            int ShoudReceiveSuddenGo() const;
            int ShoudDiveReceiveGo();
            long GetShoudTwoAttackChrIdx() const;
            long GetShoudQuickSpikeChrIdx() const;
            int ShoudBlockReady() const;
            int GetBlockReadyPos(bcs::FVEC3 &rPos) const;
            int ShoudReceiveGo() const;
            void GetTossReadyPos(bcs::FVEC3 &rPos) const;
            int ShoudTossGo() const;
            void GetSpikeReadyPos(bcs::FVEC3 &rPos) const;
            int ShoudSpikeGo() const;
            int IsShoudGetQPanel() const;
            float GetNearQPanelDis() const;
            int IsShoudGetFallGShell() const;
            float GetNearFallGShellDis() const;
            int IsShoudGetDropCoin() const;
            int GetShoudGetDropCoin() const;
            float GetNearDropCoinDis() const;
            int GetItemShootPos(bcs::FVEC3 &rPos) const;
            int IsHitSpikeShot(long, float, float, float) const;
            int IsOutBallJudge() const;
            int Util_GetOpenSpace(bcs::FVEC3 &rPos, const bcs::FVEC3 *pPos, long, long, const bcs::FVEC3 *, int) const;
            int Util_GetCornerOpenSpace(bcs::FVEC3 &rPos, const bcs::FVEC3 *pPos, long, long,
                                        const bcs::FVEC3 *) const;
            int IsShoudTargetMarkerGoSp() const;
            long GetNearChrBaseWithCondition(const bcs::FVEC3 &rPos, unsigned long, float) const;
            long GetNearChrBaseWithConditionNo2(const bcs::FVEC3 &rPos, unsigned long, float) const;
            int IsExistChrTargetWithCondition(const bcs::FVEC3 &rPos, float, unsigned long) const;
            int GetItemShootTarget(bcs::FVEC3 &rPos) const;
            int IsBanSpike() const;
            int IsSpikeCurveOutAtTargetMarker() const;
            long GetSpikeCurveInnerValueAtTargetMarker() const;

            // The actions (g_ActFuncMap): ActCall* starts one, ActLoop* runs
            // it every frame.
            void ActCallReceiveGo();
            void ActLoopReceiveGo();
            void ActCallReceiveSudden();
            void ActLoopReceiveSudden();
            void ActCallDiveReceive();
            void ActLoopDiveReceive();
            void ActCallTossReady();
            void ActLoopTossReady();
            void ActCallTossGo();
            void ActLoopTossGo();
            void ActCallSpikeReady();
            void ActLoopSpikeReady();
            void ActCallSpikeGo();
            void ActLoopSpikeGo();
            void ActLoopSpikeJump();
            void ActLoopBlockReady();
            void ActCallBlockGo();
            void ActLoopBlockGo();
            void ActCallServe();
            void ActLoopServe();
            void ActCallServeWait();
            void ActLoopServeWait();
            void ActCallGoHome();
            void ActLoopGoHome();
            void ActCallDefHome();
            void ActLoopDefHome();
            void ActLoopSymmetry();
            void ActLoopGoAway();
            void ActCallTargetMarkerGo();
            void ActLoopTargetMarkerGo();
            void ActCallGetQPanel();
            void ActLoopGetQPanel();
            void ActCallGetDropCoin();
            void ActLoopGetDropCoin();
            void ActCallItemShoot();
            void ActLoopItemShoot();
            void ActCallInterThrowItem();
            void ActLoopInterThrowItem();
            void ActCallEscGimmick();
            void ActLoopEscGimmick();
            void ActCallBaseWait(float fFrame);
            void ActChangeGoDef(long);

            // The special shots: going after the other team's (Off) and
            // defending against it (Def), per character.
            void ActCallSpOffMoveMarker();
            void ActLoopSpOffMoveMarker();
            void ActCallSpKoopaOff();
            void ActLoopSpKoopaOff();
            void WraperActCallOffSpStart();
            void WraperActCallOffSpSub();
            void ActLoopSpDaisyDef();
            void ActLoopSpKoopaDef();
            int ActSubfSpKoopaDef_GetEscPos(bcs::FVEC3 &rPos, const bcs::FVEC3 &rFire);
            void ActLoopSpNinjaDef();
            long ActSubfSpNinjaDef_GetTargetIdx(int bChange);
            int ActSubfSpNinjaDef_CareTarget(bcs::FVEC3 &rPos, long nTarget);
            void WraperActCallDefSpStart();

            // The stage gimmicks.
            int Stage10_IsMyBingoIfGet(long, long) const;
            int Stage10_GetGoodPos(bcs::FVEC3 &rPos, const CComOSG &rGrid, long, const bcs::FVEC3 *) const;
            int Stage10_SearchGoodPos(bcs::FVEC3 &rPos, bcs::FVEC3 &, const CComOSG &rGrid, long,
                                      const bcs::FVEC3 *, long) const;
            long Stage12_GetPinballToTarget(const bcs::FVEC3 &rPos, float) const;
            int Stage12_GetSafePosFromPinball(bcs::FVEC3 &rPos, long nPinball) const;
            long Stage15_GetCaseToTarget(const bcs::FVEC3 &rPos, float) const;
            int Stage15_GetSafePosFromCase(bcs::FVEC3 &rPos, long nCase) const;
            long Stage15_GetPosTurnAroundCase(bcs::FVEC3 &rPos, long nCase, long nDir) const;
            int Stage17_GetViewPointPosFromTrolley(bcs::FVEC3 &rPos) const;
            // How long to keep a special shot's marker moving (*pTime, *pDist)
            // and where (the result, for GetMoveTargetMarkerPos).
            long GetDatSpTargetMarker(float *pTime, float *pDist, int);
            int ShoudEscGimmick() const;
            int GetEscPosFromGimmick(bcs::FVEC3 &rPos) const;
            int GetSafePosFromGimmick(bcs::FVEC3 &rPos) const;
            int GetPosTurnAroundGimmick(bcs::FVEC3 &rPos);

        public:
            // An action: a member called each frame while it runs.
            typedef void (CComDrvVolleyGame::*ActFunc)();
            // The actions after CComDrv's ten (g_ActFuncMap). (names from
            // the ActLoop functions)
            enum {
                ACT_RECEIVE_GO = 10,
                ACT_RECEIVE_SUDDEN,
                ACT_DIVE_RECEIVE,
                ACT_TOSS_READY,
                ACT_TOSS_GO,
                ACT_SPIKE_READY,
                ACT_SPIKE_GO,
                ACT_SPIKE_JUMP,
                ACT_BLOCK_READY,
                ACT_BLOCK_GO,
                ACT_SERVE,
                ACT_SERVE_WAIT,
                ACT_GO_HOME,
                ACT_DEF_HOME,
                ACT_SYMMETRY,
                ACT_GO_AWAY,
                ACT_TARGET_MARKER_GO,
                ACT_GET_QPANEL,
                ACT_GET_DROP_COIN,
                ACT_ITEM_SHOOT,
                ACT_INTER_THROW_ITEM,
                ACT_ESC_GIMMICK,
                ACT_SP_OFF_MOVE_MARKER,
                ACT_SP_DAISY_DEF,
                ACT_SP_KOOPA_OFF,
                ACT_SP_KOOPA_DEF,
                ACT_SP_NINJA_DEF,
                ACT_NUM
            };

            static ActFunc g_ActFuncMap[ACT_NUM];

        protected:
            // 0x170 (names guessed from UpdateHomePos/GetHomePos): the four
            // home positions of the court (in the court's frame), front left,
            // front right, back left, back right.
            bcs::FVEC3 m_aHomePos[4];
            // 0x1A0-0x1A8 (UpdateAPosition): which of the team's two
            // characters plays in front (m_nFrontChrIdx) and behind
            // (m_nBackChrIdx), and the driver's place: 0 in front, 2 behind,
            // 1 neither (a third character).
            long m_nAPos;
            long m_nFrontChrIdx;
            long m_nBackChrIdx;
            // 0x1AC-0x1B1 (meanings guessed, see ClearAll and the Lot*
            // functions).
            signed char m_nUnk1AC;
            signed char m_nUnk1AD;
            // 0x1AE: the spike's curve side the driver prefers (message 0x91).
            signed char m_nSpikeCurve;
            signed char m_nUnk1AF;
            signed char m_nUnk1B0;
            // 0x1B1, 0x1B4: the defence leaves its place early; the frames
            // still to wait (counted up to zero, message 0x23).
            signed char m_bEarlyDef;
            float m_fEarlyDefTime;
            signed char m_nDefPosKind;   // 0x1B8 (LotDefPosKind)
            signed char m_nUnk1B9;
            // 0x1BC, 0x1C0: added to the home position (LotDefPosHosei).
            float m_fHomeOfsX;
            float m_fHomeOfsZ;
            float m_fUnk1C4;
            float m_fUnk1C8;
            // 0x1CC (message 0x87): the driver goes for the next touch at once.
            signed char m_bTouchGo;
            signed char m_nUnk1CD;
            signed char m_nUnk1CE;
            signed char m_nUnk1CF;
            // 0x1D0 and 0x1D9-0x1DD (names guessed): per-level switches,
            // read signed. 0x1D0 lets the driver spike on the second touch,
            // 0x1D9/0x1DA make it run from an item (0x1D9 for a bomb),
            // 0x1DB from an obstacle, 0x1DC picks the side to go round a
            // special-shot obstacle; MethdSpike sets 0x1DD to -1.
            signed char m_bTwoAttack;
            // 0x1D4: the team's touches in this rally (messages 0x85, 0x87).
            long m_nTouchCount;
            signed char m_nUnk1D8;       // 0x1D8, -1 for none
            signed char m_bEscBomb;      // 0x1D9
            signed char m_bEscItem;      // 0x1DA
            signed char m_bEscObstacle;  // 0x1DB
            signed char m_bSpObstacleSide; // 0x1DC
            signed char m_nUnk1DD;       // 0x1DD
            bcs::FVEC3 m_Unk1E0;         // 0x1E0
            // 0x1EC: set while the other team's special shot is answered.
            long m_bSpBefore;
            signed char m_nUnk1F0;       // 0x1F0
            float m_fUnk1F4;             // 0x1F4
            // 0x1F8 (name guessed): not zero on a stage with gimmicks to run
            // from.
            long m_bGimmick;
    };
}

#endif /* !MSW_CCOMDRVVOLLEYGAME_HPP_ */
