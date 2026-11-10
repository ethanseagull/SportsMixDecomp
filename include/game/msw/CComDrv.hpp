#ifndef MSW_CCOMDRV_HPP_
#define MSW_CCOMDRV_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrDrv.hpp"

namespace msw {

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
    // Data: only m_pChr is mapped. layout.py finds members up to 0x16C; the
    // size is not known, so the class must not be instantiated or embedded.
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
            virtual void OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
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

        protected:
            unsigned char m_Unk04[0x64];
            CChrCore *m_pChr; // 0x68, the character driven
    };
}

#endif /* !MSW_CCOMDRV_HPP_ */
