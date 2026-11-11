#ifndef CCOCKPITHOCKEYVSCOUNT_HPP_
#define CCOCKPITHOCKEYVSCOUNT_HPP_

#include "game/msw/CCockpitFrameOutBase.hpp"
#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x80 (CCockpitHockey::CreateObjImpl allocates it): HockeyF
    // game/hockey/CCockpitHockeyVSCount.cpp. The party game's (crash skate)
    // final count board: the two teams' faces and the rounds each has won.
    // CCockpitFrameOutBase is at 0x60 (vptr at 0x68, the @96@ thunk).
    // Member names are guesses.
    class CCockpitHockeyVSCount : public CCockpitLayoutBase, public CCockpitFrameOutBase {
        public:
            CCockpitHockeyVSCount(long nId, float fSortZ);
            virtual ~CCockpitHockeyVSCount();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();

            // Puts character nChr's face in icon nIdx of team nTeam, with a
            // star when bStar is set. 0, or -1 to -3 when a pane is missing.
            long SetFaceIcon(long nTeam, long nIdx, long nChr, int bStar);
            // Moves the player labels ("1P" to "4P", 4 for "COM") under each
            // team's label pane; a negative number shows none.
            long AttachPlayerNo(long nTeam0, long nTeam1);
            // Shows each team's won rounds before this one.
            void InitSetCountPane(long nTeam0, long nTeam1);
            void InitParameters();

        private:
            // (an inline whose local static the original keeps) The
            // starting values of the members below.
            void Initialize();
            // Lights the nCount-th round icon of team nTeam (0 to 2).
            void SetCountEffect(long nTeam, long nCount);

            long m_nUnk6C;          // 0x6C, cleared by InitParameters
            float m_fDelay;         // 0x70, frames before the new round lights up
            long m_nNewCount[2];    // 0x74, per team the round icon to light; -1 for none
            long m_nResult;         // 0x7C, 0 draw, 1 team 1 won, 2 team 0 won (guess)
    };
}

#endif /* !CCOCKPITHOCKEYVSCOUNT_HPP_ */
