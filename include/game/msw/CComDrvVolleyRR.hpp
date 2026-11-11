#ifndef MSW_CCOMDRVVOLLEYRR_HPP_
#define MSW_CCOMDRVVOLLEYRR_HPP_

#include "game/msw/CComDrvVolley.hpp"

namespace msw {

    class CBall;

    // The computer driver of the volleyball game's rainbow road stage
    // (VolleyF; __vt__Q23msw15CComDrvVolleyRR), 0x1A4 bytes (what
    // CHR_MAN_VOLLEY::CChrManVolleyRhythmReceive::CreateDriver_ allocates).
    // Several balls are in play: each character goes for its own ball (or
    // one of the shared rainbow balls), picks up coins, and otherwise waits
    // at a home position along the court.
    //
    // Two files in the original: game/volley/CComDrvVolleyRR.cpp (most of
    // it) and game/volley/unit_CA990.cpp (going round obstacles). Member
    // names are guesses.
    class CComDrvVolleyRR : public CComDrvVolley {
        public:
            // An action: a member called each frame while it runs.
            typedef void (CComDrvVolleyRR::*ActFunc)();

            // The actions of g_ActFuncMap after CComDrv's ten.
            enum {
                ACT_GO_HOME = 10,
                ACT_RECEIVE_READY,
                ACT_RECEIVE_GO,
                ACT_GET_DROP_COIN,
                ACT_NUM
            };

            CComDrvVolleyRR();
            virtual ~CComDrvVolleyRR();

            virtual void GetPosTurnAroundForRunEx(bcs::FVEC3 &rPos);
            virtual void GetViewPointPosForRunEx(bcs::FVEC3 &rPos);
            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual void NormalizeCanMoveWorldPos(bcs::FVEC3 &rPos) const;
            virtual int IsActiveBallNothingThk() const;
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual long OnGameMsgProc(MSG_ID id, _CHR_MSG *pMsg);
            virtual void OnAlarm1(long nAlarm);
            virtual void ActInterrupt();
            virtual void ThkMain();

            void ActCallGoHome();
            void ActLoopGoHome();
            void ActLoopReceiveReady();
            void ActCallReceiveGo();
            void ActLoopReceiveGo();
            void ActCallGetDropCoin();
            void ActLoopGetDropCoin();

            // The driver's home position, with room left for a ball or an
            // item coming down there.
            void GetHomePos(bcs::FVEC3 &rPos);
            // The ball the character should go for (its own, else a
            // rainbow ball given to it), and in rPos where it comes down;
            // null when none.
            CBall *GetMyBallPos(bcs::FVEC3 &rPos) const;
            // The rainbow ball (type 4) this character is the best one to
            // take, among the ones no other character is nearer to.
            CBall *GetAssignRainbowBall() const;
            // The task id of the nearest coin the character should pick up
            // (0: none).
            unsigned long GetShoudGetDropCoin() const;

            // Steers rPos round a character or item in the way; non-zero
            // while it is turning round one.
            int GetPosTurnAroundGimmick(bcs::FVEC3 &rPos);

            static ActFunc g_ActFuncMap[ACT_NUM];

        protected:
            // 0x170: the four home positions along the court, one per
            // place in the order of the characters' x positions (OnInit).
            bcs::FVEC3 m_aHomePos[4];
            // 0x1A0 (meaning not known): set by ClearAll and by alarm 1.
            long m_nUnk1A0;
    };
}

#endif /* !MSW_CCOMDRVVOLLEYRR_HPP_ */
