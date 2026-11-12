#ifndef CCOCKPITHOCKEYMAP_HPP_
#define CCOCKPITHOCKEYMAP_HPP_

#include "game/msw/CCockpitHockeyLayoutBase.hpp"
#include "nw4r/ut/LinkList.h"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

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

            // Puts player nPlayer's marker under pParent (the map base pane);
            // 0, or -1 for a missing argument, -2 for a character that has
            // no marker (none, disabled or the keeper), -10 when a pane
            // could not be made.
            long CreatePlayerMarker(long nPlayer, nw4r::lyt::Pane *pParent, bcs::G2D::CPaneCtrl *pPaneCtrl);
            // The same for the puck.
            long CreateBallMarker(nw4r::lyt::Pane *pParent, bcs::G2D::CPaneCtrl *pPaneCtrl);
            // Shows or hides the "COM" labels among the panes of rList.
            inline void SetVisibleChildCom(nw4r::ut::LinkList<nw4r::lyt::Pane, 4> &rList, int bVisible);

            void SetDispEnable(int bEnable);
            void SetCameraPosition(float fPos);
            void SetPlayerPosition(long nPlayer, float fX, float fY, float fZ, long nArg4, long nArg5);
            void SetPlayerDisp(long nPlayer, int bDisp);
            void SetBallDisp(long nBall, int bDisp);
            void SetDirection(int nDirection);
            void SetGoalTarget(long nTarget);

        private:
            // The map base pane's size (Set), 1 until then: positions in
            // -1 to 1 are scaled by half of it.
            float m_fMapWidth;  // 0x60
            float m_fMapHeight; // 0x64
    };
}

#endif /* !CCOCKPITHOCKEYMAP_HPP_ */
