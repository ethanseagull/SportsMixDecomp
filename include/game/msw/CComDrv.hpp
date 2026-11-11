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
            virtual void OnMain(_CHR_MSG *pMsg);
            virtual void OnConnect(_CHR_MSG *pMsg);
            virtual void OnDisconnect(_CHR_MSG *pMsg);
            virtual void OnInit(_CHR_MSG *pMsg);
            virtual void OnDamage(_CHR_MSG *pMsg);
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
            void Item_GetViewPointPosFromItem(bcs::FVEC3 &rPos) const;
            // DOL. A random number below nMax. (range guessed from the name;
            // BasketF tests RandMaxI(999) < 20)
            long RandMaxI(long nMax) const;

        protected:
            unsigned char m_Unk04[0x60];
            // 0x64 (name guessed): the driven character's team. HockeyF
            // passes it to CComManHockey::GetKeeperCore, VolleyF compares it
            // with GameWork::GetUserTeam.
            long m_nMyTeam;
            CChrCore *m_pChr; // 0x68, the character driven
            unsigned char m_Unk6C[0x110 - 0x6C];
            // 0x110 (name guessed): the ball the driver follows, set by
            // UpdateInfo; VolleyF hands it to CComManVolley.
            CBall *m_pBall;
            // The size 0x170 is where CComDrvHockey's own members start;
            // the DOL's functions use members up to 0x16C.
            unsigned char m_Unk114[0x160 - 0x114];
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
