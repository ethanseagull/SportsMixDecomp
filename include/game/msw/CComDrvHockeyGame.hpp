#ifndef MSW_CCOMDRVHOCKEYGAME_HPP_
#define MSW_CCOMDRVHOCKEYGAME_HPP_

#include "game/msw/CComDrvHockey.hpp"

namespace msw {

    // INCOMPLETE: the three computer drivers the hockey character managers
    // make (game/hockey/CChrManHockey.cpp), all derived from CComDrvHockey
    // (their constructors call CComDrvHockey(long)). Only the constructors
    // and the allocation sizes are known here: CComDrvHockeyGame 0x494 (field
    // players), CComDrvHockeyKeeper 0x1B8 (keepers), CComDrvHockeyCS 0x1FC
    // (the party and mission game). The destructors are declared without
    // bodies so that no unit emits the vtables from this header.
    class CComDrvHockeyGame : public CComDrvHockey {
        public:
            CComDrvHockeyGame();
            virtual ~CComDrvHockeyGame();

            // Overrides, in the order of the vtable (added for
            // game/hockey/unit_AFCA0.cpp; return types as in CComDrv).
            virtual int MethdShoot();
            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void Reset();
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnDamage(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void ActInterrupt();
            virtual void ThkMain();

            // The skating moves (game/hockey/unit_AFCA0.cpp). Each returns 1
            // when the character took the action.
            int MethdMoveMain(float fRotY);
            int MethdMoveRevise(const bcs::FVEC3 &pos);
            int MethdRotY(float fRotY);
            int MethdFeintShoot();
            int MethdItemBallShoot();

            // The three attack parameters of a character for the computer
            // (game/hockey/unit_EDBF0.cpp; meaning of the three not known).
            void GetAttack1Param(long nChr, float &, float &, float &);
            void GetAttack2Param(long nChr, float &, float &, float &);
            void GetAttack3Param(long nChr, float &, float &, float &);
            // Whether the puck is being shot (passed) by the driver's own
            // team (nSide 0) or by the other team (1); rChr gets the shooter
            // (the pass's receiver).
            int AreYouShot(long nSide, long &rChr);
            int AreYouPass(long nSide, long &rChr);

            // 0 for the driver's own team, 1 for the other one (values
            // inferred from AreYouShot).
            long FriendOrEnemy(long nChr) const;
            long GetMyTeamNo() const;
            // The computer level as a float, for CComMan::GetDatF.
            float GetHockeyComLvF();

            // The frame's view of the game, filled by SetCommonData
            // (game/hockey/unit_AE7E8.cpp). "No" is a character index (for
            // CChrMan::GetChrCore), "Idx" the character's player number.
            void SetCommonData();
            CChrCore *GetHoldBallCore() const;
            long GetHoldBallCoreIndex() const;
            long GetHoldBallerTeamNo() const;
            long GetCharaNum() const;
            long GetFriendNum() const;
            long GetEnemyNum() const;
            CChrCore *GetCharaCore(long n) const;
            CChrCore *GetFriendCore(long n) const;
            CChrCore *GetEnemyCore(long n) const;
            bcs::FVEC3 GetFriendWorldPos(long n) const;
            bcs::FVEC3 GetEnemyWorldPos(long n) const;
            long GetFriendIdx(long n) const;
            long GetFriendNo(long n) const;
            long GetEnemyIdx(long n) const;
            // Whether a character of the other team has player number nIdx.
            int CheckEnemyCore(long nIdx) const;

            // (game/hockey/unit_AE7E8.cpp) What message 0x72 (a pass)
            // passes on: the passer and the receiver.
            void PassMsgJobs(long nPasser, long nReceiver);
            int MethdHockeyPass(const CChrCore *pChr);
            // Skate towards rDir until within fDis of rPos (MethdMove: of
            // rPos itself); 1 when there already, or when an invisible wall
            // is in the way.
            int MethdMove2(const bcs::FVEC3 &rDir, const bcs::FVEC3 &rPos, float fDis);
            int MethdMove(const bcs::FVEC3 &rPos, float fDis);

            // Defined in other files (used by game/hockey/unit_AE7E8.cpp;
            // return types guessed unless a caller shows them).
            long GetSpShootCharIdx();
            void ResetShotClock();
            int isResetShotClock();
            void SpCatchTimingInit();
            void SetThkStatus(long nStatus);
            long GetShootCourse();
            short GetShootKind() const;
            long GetFaceOffPower();
            void ThkFaceoffStart(long, long);
            int isSpCatchSuccess();
            long GetFigthPower();
            int CheckGoalArea(const CChrCore *pChr, float) const;
            int CheckShootCourse(const CChrCore *pChr, float);
            int ButToShootOutsideShots();
            int ButToShootGimmickShots();
            int isQuickShootOK();
            int CheckItemBallShootProbability();
            int isHitInvisibleWall(bcs::FVEC3 pos);
            void ActCallGetBall(long);
            void ActCallDamage();
            void ActCallDamageFall();

            // The actions, one per frame while running (12-byte member
            // function pointers).
            typedef void (CComDrvHockeyGame::*ActFunc)();
            static ActFunc g_ActFuncMap[];

        protected:
            // 0x1B0-0x354 (names from the getters): the ball holder and
            // the field players, all of them and split by team.
            CChrCore *m_pHoldBallCore;      // 0x1B0
            long m_nHoldBallCoreIndex;      // 0x1B4
            long m_nHoldBallerTeamNo;       // 0x1B8
            bcs::FVEC3 m_HoldBallWorldPos;  // 0x1BC
            long m_nCharaNum;               // 0x1C8
            long m_aCharaNo[8];             // 0x1CC
            long m_aCharaIdx[8];            // 0x1EC
            CChrCore *m_apCharaCore[8];     // 0x20C
            bcs::FVEC3 m_aCharaWorldPos[8]; // 0x22C
            long m_nFriendNum;              // 0x28C
            long m_aFriendNo[4];            // 0x290
            long m_aFriendIdx[4];           // 0x2A0
            CChrCore *m_apFriendCore[4];    // 0x2B0
            bcs::FVEC3 m_aFriendWorldPos[4]; // 0x2C0
            long m_nEnemyNum;               // 0x2F0
            long m_aEnemyNo[4];             // 0x2F4
            long m_aEnemyIdx[4];            // 0x304
            CChrCore *m_apEnemyCore[4];     // 0x314
            bcs::FVEC3 m_aEnemyWorldPos[4]; // 0x324
            // 0x354 (meaning not known): with bit 0 set the shot clock
            // (0x3F0) does not run.
            unsigned long m_nUnk354;
            float m_fUnk358;                // 0x358
            long m_nUnk35C;                 // 0x35C
            float m_fUnk360;                // 0x360
            unsigned char m_Unk364[0x368 - 0x364];
            long m_nLastRootActType;        // 0x368 (name guessed)
            // 0x36C, 0x370 (meaning not known): while 0x36C is set the
            // driver answers only message 0x23.
            long m_nUnk36C;
            long m_nUnk370;
            float m_fUnk374;                // 0x374
            long m_nUnk378;                 // 0x378
            // 0x37C (name guessed): set by MethdRotY.
            int m_bRotY;
            // 0x380-0x38C (names guessed): drawn once a second (m_fLotTime)
            // from the computer data.
            int m_bLot380;
            int m_bLot384;
            int m_bLot388;
            float m_fLotTime;               // 0x38C
            float m_fUnk390;                // 0x390
            long m_nUnk394;                 // 0x394
            unsigned char m_Unk398[0x39C - 0x398];
            long m_nUnk39C;                 // 0x39C
            float m_fUnk3A0;                // 0x3A0
            float m_fUnk3A4;                // 0x3A4
            float m_fUnk3A8;                // 0x3A8
            unsigned char m_Unk3AC[0x3BC - 0x3AC];
            long m_nUnk3BC;                 // 0x3BC
            long m_nUnk3C0;                 // 0x3C0
            unsigned char m_Unk3C4[0x3CC - 0x3C4];
            // 0x3CC-0x3E4 (meanings not known): the face-off messages.
            long m_nUnk3CC;
            long m_nUnk3D0;
            long m_nUnk3D4;
            long m_nUnk3D8;
            long m_nUnk3DC;
            long m_nUnk3E0;
            float m_fUnk3E4;
            // 0x3E8, 0x3EC (names guessed): whether the skater brakes to
            // turn, and whether that was drawn since it last could.
            int m_bSwitchBrake;
            int m_bSwitchBrakeLot;
            // 0x3F0, 0x3F4 (names guessed): count down every frame; the
            // first stops while m_nUnk354 bit 0 is set.
            float m_fShotClock;
            float m_fUnk3F4;
            unsigned char m_Unk3F8[0x3FC - 0x3F8];
            long m_nUnk3FC;                 // 0x3FC
            // 0x400, 0x404 (names guessed): a shot, or an item ball shot,
            // asked for on a pass (PassMsgJobs).
            long m_bShootReq;
            long m_bItemShootReq;
            float m_fUnk408;                // 0x408
            long m_nUnk40C;                 // 0x40C
            long m_nUnk410;                 // 0x410
            // 0x414: 1 until the game time reaches the computer data's
            // entry 0x111.
            long m_bEarlyGame;
            // 0x418 (name guessed): a wait after a shot, from the computer
            // data (id 0x119).
            float m_fShootWait;
            float m_fUnk41C;                // 0x41C
            float m_fUnk420;                // 0x420
            float m_fUnk424;                // 0x424
            long m_nUnk428;                 // 0x428
            long m_nUnk42C;                 // 0x42C
            long m_nUnk430;                 // 0x430
            unsigned char m_Unk434[0x43C - 0x434];
            float m_fUnk43C;                // 0x43C, computer data 0xF64
            unsigned char m_Unk440[0x454 - 0x440];
            // 0x454-0x45C: the character of a special shot (GetSpShootCharIdx;
            // -1 for none), set by messages 0x7C/0x7D.
            short m_nSpShootChr;
            long m_nUnk458;                 // 0x458
            long m_nUnk45C;                 // 0x45C
            short m_nStage;                 // 0x460, 0 above stage 0x11
            short m_nPassChr;               // 0x462, set by PassMsgJobs
            short m_nUnk464;                // 0x464
            unsigned char m_Unk466[0x494 - 0x466];
    };

    class CComDrvHockeyKeeper : public CComDrvHockey {
        public:
            CComDrvHockeyKeeper();
            virtual ~CComDrvHockeyKeeper();

        protected:
            unsigned char m_Unk1B0[0x1B8 - 0x1B0];
    };

    class CComDrvHockeyCS : public CComDrvHockey {
        public:
            CComDrvHockeyCS();
            virtual ~CComDrvHockeyCS();

            // Overrides (added for game/hockey/unit_CE254.cpp).
            virtual void ActInterrupt();
            virtual void ThkMain();

            // The actions (game/hockey/CComDrvHockeyCS.cpp). The longs are
            // character indices.
            void ActCallAmbushSmashAttack();
            void ActCallChargeSmashAttack(long nChr);
            void ActCallIceAttack(short, short);
            void ActCallEscape(bcs::FVEC3 pos);
            void ActCallSmashAttack(long nChr);
            void ActCallBlockAttack(long nChr);
            void ActCallChase(long nChr);
            void ActCallFreeSpace();
            void ActCallWraps();
            void ActCallEscapeEruption();
            void ActCallStatusFire();
            void ActCallReflectionAttack();

            // Searches (game/hockey/CComDrvHockeyCS.cpp; meanings guessed
            // from the names). Each returns a character index, negative for
            // none.
            long SearchingForTheEenemy(float fDis, int, int, long nMask, int);
            long SearchingForTheEscapeEenemy(float fDis, int);
            long GetSmashChanceCore(float fDis);
            float GetEruptionRadius();
            int GetEruptionWarning();
            int GetStageTargetPosition(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, int, int);
            short isCallIceAttack(short &);
            long isChargeSmashAttack();
            long isRefrectionAttack(bcs::FVEC3 &, bcs::FVEC3 &);

            // The interrupt checks (game/hockey/unit_CE254.cpp): 1 when the
            // action they stand for runs (or has just been started).
            int ThkInterruptReflectionAttack();
            int ThkInterruptEruptionWarning();
            int ThkInterruptPriorityAttack();
            int ThkInterruptAttack_BlockAttack();
            int ThkInterruptAttack_ChargeSmashAttack();
            // (inline in game/hockey/unit_CE254.cpp)
            int ThkInterruptStatusFire();
            int ThkInterruptEscape();
            int ThkInterruptAttack();
            int ThkInterruptSearch();
            int ThkInterruptIceAttack();

        protected:
            // 0x1B0 (names guessed from game/hockey/unit_CE254.cpp): the
            // frame counters of the interrupt checks, each compared with an
            // interval from the computer data.
            float m_fReflectionTime;   // 0x1B0
            // 0x1B4, 0x1B8, 0x1BC: on stage 0x56, whether the character
            // keeps still (m_fFireWait counts it down), and the counter
            // that lots it.
            int m_bFireWait;
            float m_fFireLotTime;      // 0x1B8
            float m_fFireWait;         // 0x1BC
            unsigned char m_Unk1C0[0x1CC - 0x1C0];
            // 0x1CC, 0x1D0: cleared before an escape or an ice attack.
            long m_nUnk1CC;
            long m_nUnk1D0;
            unsigned char m_Unk1D4[0x1D8 - 0x1D4];
            float m_fChargeSmashTime;  // 0x1D8
            unsigned char m_Unk1DC[0x1E0 - 0x1DC];
            float m_fSearchTime;       // 0x1E0
            float m_fBlockTime;        // 0x1E4
            float m_fIceTime;          // 0x1E8
            float m_fPriorityTime;     // 0x1EC
            unsigned char m_Unk1F0[0x1FC - 0x1F0];
    };
}

#endif /* !MSW_CCOMDRVHOCKEYGAME_HPP_ */
