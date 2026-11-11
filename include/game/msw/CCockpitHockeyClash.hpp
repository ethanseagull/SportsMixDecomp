#ifndef CCOCKPITHOCKEYCLASH_HPP_
#define CCOCKPITHOCKEYCLASH_HPP_

#include "game/msw/CCockpitHockeyLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // Size 0xB8 (CCockpitHockey::CreateObjImpl allocates it): the board of
    // the hockey party game (the players' scores, point-up texts, crowns;
    // HockeyF game/hockey/CCockpitHockeyClash.cpp). Its constructor also
    // builds a cockpit::CEffectSet. Only what CCockpitHockey calls so far;
    // argument names are guesses.
    class CCockpitHockeyClash : public CCockpitHockeyLayoutBase {
        public:
            CCockpitHockeyClash(long nId, float fSortZ);
            virtual ~CCockpitHockeyClash();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            void SetTimeValue(float fFrame, int nArg, int nMode);
            void SetScore(long nPlayer, long nScore, int bAnim, int bBig);
            void SetPointUpText(long nPlayer, const bcs::FVEC3 &rPos, long nPoint);
            void ClearPointUpText(long nPlayer);
            void SetLeaveCountText(long nPlayer, const bcs::FVEC3 &rPos, long nCount, int bArg);
            void SetRimoconShakeIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, int bArg);
            void SetCrownIcon(long nPlayer, const bcs::FVEC3 &rPos, long nArg, long nAction);

        private:
            unsigned char m_Unk60[0xB8 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYCLASH_HPP_ */
