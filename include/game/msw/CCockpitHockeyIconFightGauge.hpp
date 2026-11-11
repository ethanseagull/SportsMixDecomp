#ifndef CCOCKPITHOCKEYICONFIGHTGAUGE_HPP_
#define CCOCKPITHOCKEYICONFIGHTGAUGE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // Size 0x64 (CCockpitHockey::CreateObjImpl allocates it): the gauges
    // shown over two fighting players (HockeyF
    // game/hockey/CCockpitHockeyIconFightGauge.cpp). Only what CCockpitHockey
    // calls so far; argument names are guesses.
    class CCockpitHockeyIconFightGauge : public CCockpitLayoutBase {
        public:
            CCockpitHockeyIconFightGauge(long nId, float fSortZ, long nLayer);
            virtual ~CCockpitHockeyIconFightGauge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            // nTeam is 0 or 1; each returns -1 for another value, else 0.
            // CreateIcon makes the team's icon task (Set calls it for both
            // teams); SetIcon starts the icon at rPos (world space),
            // ClearIcon hides it (closing it first when bClose is set and
            // it is open), SetGaugeBar shows fRatio (0 to 1) on the bar.
            long CreateIcon(long nTeam);
            long SetIcon(long nTeam, const bcs::FVEC3 &rPos);
            long SetGaugeBar(long nTeam, float fRatio);
            long ClearIcon(long nTeam, int bClose);

        private:
            long m_nLayer; // 0x60, picks the layout layer; passed to DispLoop as the camera
    };
}

#endif /* !CCOCKPITHOCKEYICONFIGHTGAUGE_HPP_ */
