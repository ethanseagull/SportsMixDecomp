#ifndef CCOCKPITDODGEBALLPOWER_HPP_
#define CCOCKPITDODGEBALLPOWER_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace nw4r {
    namespace lyt {
        struct Size;
    }
}

namespace msw {

    // Size 0x64 (CCockpitDodge::CreateObjImpl allocates it). The power of
    // each team's ball (DodgeF game/dodge/CCockpitDodgeCoder.cpp). It
    // overrides the gauge slides of CCockpitDodgeLayoutBase. Return types
    // and argument names are guesses; nIdx is a team (0 or 1).
    class CCockpitDodgeBallPower : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeBallPower(long nId);
            virtual ~CCockpitDodgeBallPower();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int GaugeOut();
            virtual int GaugeIn();
            virtual int GaugeOutWait();
            virtual int GaugeInWait();

            int SetBallPowerValue(long nIdx, short nValue);
            int SetBallPowerValueColor(long nIdx, int nColor);
            // nState 1 and 2 select the two "up" animations, others the
            // normal one.
            int SetBallPowerState(long nIdx, long nState);
            int SetBallPowerShift(long nIdx, long nShift);
            // Gives text box pane nPane the first (bColor set) or second
            // number font.
            void ChangeTextFont(long nPane, int bColor);
            // Where a coin effect for team nTeam goes: -1 for an unknown
            // team, -2 without the pane, else 0.
            long GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);

        private:
            // 0x60: sets in the match (1, 3 or 5), from the byte at
            // g_GameWork + 0x0B. Nothing else reads it.
            unsigned char m_nSetNum;
    };
}

#endif /* !CCOCKPITDODGEBALLPOWER_HPP_ */
