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
            virtual void OnDamage(_CHR_MSG *pMsg);
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

        protected:
            unsigned char m_Unk1B0[0x37C - 0x1B0];
            // 0x37C (name guessed): set by MethdRotY.
            int m_bRotY;
            unsigned char m_Unk380[0x3E8 - 0x380];
            // 0x3E8, 0x3EC (names guessed): whether the skater brakes to
            // turn, and whether that was drawn since it last could.
            int m_bSwitchBrake;
            int m_bSwitchBrakeLot;
            unsigned char m_Unk3F0[0x418 - 0x3F0];
            // 0x418 (name guessed): a wait after a shot, from the computer
            // data (id 0x119).
            float m_fShootWait;
            unsigned char m_Unk41C[0x494 - 0x41C];
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
