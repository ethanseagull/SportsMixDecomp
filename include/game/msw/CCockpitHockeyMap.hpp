#ifndef CCOCKPITHOCKEYMAP_HPP_
#define CCOCKPITHOCKEYMAP_HPP_

#include "game/msw/CCockpitHockeyLayoutBase.hpp"

namespace msw {

    // Size 0x68 (CCockpitHockey::CreateObjImpl allocates it): the hockey
    // game's rink map (HockeyF game/hockey/CCockpitHockeyMap.cpp). Only what
    // CCockpitHockey calls so far; argument names are guesses.
    class CCockpitHockeyMap : public CCockpitHockeyLayoutBase {
        public:
            CCockpitHockeyMap(long nId);
            virtual ~CCockpitHockeyMap();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            void SetDispEnable(int bEnable);
            void SetCameraPosition(float fPos);
            void SetPlayerPosition(long nPlayer, float fX, float fY, float fZ, long nArg4, long nArg5);
            void SetPlayerDisp(long nPlayer, int bDisp);
            void SetBallDisp(long nBall, int bDisp);
            void SetDirection(int nDirection);
            void SetGoalTarget(long nTarget);

        private:
            unsigned char m_Unk60[0x68 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYMAP_HPP_ */
