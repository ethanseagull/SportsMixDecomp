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
    // overrides the gauge slides of CCockpitDodgeLayoutBase. Only what
    // CCockpitDodge calls so far. Return types and argument names are
    // guesses; nIdx is a team colour.
    class CCockpitDodgeBallPower : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeBallPower(long nId);
            virtual ~CCockpitDodgeBallPower();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void SetBallPowerValue(long nIdx, short nValue);
            void SetBallPowerValueColor(long nIdx, int nColor);
            void SetBallPowerState(long nIdx, long nState);
            void SetBallPowerShift(long nIdx, long nShift);
            // Where a coin effect for team nTeam goes; negative without one.
            long GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);

        private:
            unsigned char m_Unk60[0x64 - 0x60];
    };
}

#endif /* !CCOCKPITDODGEBALLPOWER_HPP_ */
