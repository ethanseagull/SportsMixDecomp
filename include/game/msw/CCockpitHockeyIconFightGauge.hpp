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

            void SetIcon(long nTeam, const bcs::FVEC3 &rPos);
            void SetGaugeBar(long nTeam, float fRatio);
            void ClearIcon(long nTeam, int bArg);

        private:
            unsigned char m_Unk60[0x64 - 0x60];
    };
}

#endif /* !CCOCKPITHOCKEYICONFIGHTGAUGE_HPP_ */
