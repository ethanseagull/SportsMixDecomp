#ifndef MSW_CCOMDRVBOSS_HPP_
#define MSW_CCOMDRVBOSS_HPP_

#include "game/msw/CComDrv.hpp"

namespace msw {

    class CNpcBehemothBoss;
    class CQPanel;

    // The computer driver of the boss battle (BossF), 0x1F8 bytes (what
    // CChrManBoss::CreateDriver_ allocates). The characters play against
    // the Behemoth: they keep to a home position around it, catch the ball
    // it throws and hit it with the stick, shots and items.
    //
    // Virtual functions in the order of __vt__Q23msw11CComDrvBoss: the
    // overrides of CComDrv's slots, then eleven of its own (0xE0-0x108).
    // Five files in the original: the actions (game/boss/unit_4F0C4.cpp),
    // game/boss/CComDrvBoss.cpp (constructor, home positions, vtable), the
    // frame and message handlers (unit_51CE4), the methods (unit_52300) and
    // ThkMain (unit_52680). Member names are guesses.
    class CComDrvBoss : public CComDrv {
        public:
            // An action: a member called each frame while it runs.
            typedef void (CComDrvBoss::*ActFunc)();

            // The actions of g_ActFuncMap after CComDrv's ten.
            enum {
                ACT_GO_HOME = 10,
                ACT_GO_RAND,
                ACT_GET_BALL,
                ACT_RECEIVE_GO,
                ACT_SPIKE_GO,
                ACT_STICK_ATTACK,
                ACT_SHOOT,
                ACT_THROW_ITEM,
                ACT_GET_QPANEL,
                ACT_NUM
            };

            CComDrvBoss();
            virtual ~CComDrvBoss();

            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void EntryActCallBack(long nAct);
            virtual void NormalizeCanMoveWorldPos(bcs::FVEC3 &rPos) const;
            virtual int IsActiveBallNothingThk() const;
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void OnAlarm0(long);
            virtual void ActInterrupt();
            virtual void ThkMain();
            virtual int MethdServe();
            virtual int CanbeReceive();
            virtual int MethdReceive();
            virtual int CanbeDiveReceive();
            virtual int MethdDiveReceive();
            virtual int CanbeShootStance();
            virtual int MethdShootStance();
            virtual int CanbeDunk();
            virtual int MethdDunk();
            virtual void ResetRelayRun();
            virtual int MethdRelayRun(const bcs::FVEC3 &rPos, float fRange);

            void ActCallGoHome();
            void ActLoopGoHome();
            void ActCallGoRand();
            void ActLoopGoRand();
            void ActLoopGetBall();
            void ActCallReceiveGo();
            void ActLoopReceiveGo();
            void ActLoopSpikeGo();
            void ActCallStickAttack();
            void ActLoopStickAttack();
            void ActCallShoot();
            void ActLoopShoot();
            void ActCallThrowItem();
            void ActLoopThrowItem();
            void ActCallGetQpanel();
            void ActLoopGetQpanel();

            CNpcBehemothBoss *GetBehemoth() const;
            void UpdateHomePos();
            void GetHomePos(bcs::FVEC3 &rPos);
            void GetHomeRandPos(bcs::FVEC3 &rPos);
            void GetHomeRandPosBase(bcs::FVEC3 &rPos, float fRotRange, float fDist);
            int ShoudReceiveGo();
            int IsShoudGetQPanel() const;
            long QPanel_GetNearMyTeamChr(CQPanel *pPanel) const;
            int ShoudEscFromBoss() const;
            int Util_GetRelayPosForBoss(bcs::FVEC3 &rOut, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo);
            int GetBallTargetPos(bcs::FVEC3 &rPos) const;
            int CanbeMyTouch() const;
            // (inline helpers, names guessed)
            float GetRelayDistXZ(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo);
            void GetEscPos(bcs::FVEC3 &rPos);

            static ActFunc g_ActFuncMap[ACT_NUM];

        protected:
            // 0x170, 0x1A0: the four home positions round the Behemoth (in
            // front, 72 degrees to its left and right, behind), and points
            // nearer to it in the same directions; [0] and [3] of the
            // nearer ones mark the line the characters go round.
            bcs::FVEC3 m_aHomePos[4];
            bcs::FVEC3 m_aHomeNearPos[4];
            // 0x1D0: the side home of the driver (1 or 2), UpdateHomePos.
            long m_nHomeIdx;
            // 0x1D4, 0x1E0: the Behemoth's position (on the ground) and yaw,
            // kept by UpdateInfo.
            bcs::FVEC3 m_BossPos;
            float m_fBossRotY;
            // 0x1E4, 0x1F0: where MethdRelayRun is running to (the relay
            // point or the target), and how: -1 not started (ResetRelayRun),
            // 0 straight to the target, 1 by way of the relay point.
            bcs::FVEC3 m_RelayPos;
            long m_nRelayStep;
            // 0x1F4: how far the stick attack is (0 going, 1 swinging, 2
            // done); cleared when an action starts.
            long m_nStickAttackStep;
    };
}

#endif /* !MSW_CCOMDRVBOSS_HPP_ */
