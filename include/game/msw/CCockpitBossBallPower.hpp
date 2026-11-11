#ifndef CCOCKPITBOSSBALLPOWER_HPP_
#define CCOCKPITBOSSBALLPOWER_HPP_

#include "game/msw/CCockpitBossLayoutBase.hpp"

namespace bcs {
    struct FVEC3;
}

namespace nw4r {
    namespace lyt {
        struct Size;
    }
}

namespace msw {

    // Size 0x64. The boss game's ball power counter, defined in
    // game/boss/CCockpitBossCoder.cpp. It slides with the life gauge part.
    // Member names and the return types of the setters are guesses.
    class CCockpitBossBallPower : public CCockpitBossLayoutBase {
        public:
            CCockpitBossBallPower(long nId);
            virtual ~CCockpitBossBallPower();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            virtual int LifeGaugeOut();
            virtual int LifeGaugeIn();
            virtual int LifeGaugeOutWait();
            virtual int LifeGaugeInWait();

            int SetBallPowerValue(long nTeam, short nValue);
            int SetBallPowerValueColor(long nTeam, int bColor);
            // nState 1 and 2 select the two "up" animations, others the
            // normal one.
            int SetBallPowerState(long nTeam, long nState);
            // Gives text box pane nPane the first (bColor set) or second
            // number font.
            void ChangeTextFont(long nPane, int bColor);
            // Screen position and size of team nTeam's counter: -1 for an
            // unknown team, -2 without the pane, else 0.
            long GetTargetInfo(long nTeam, bcs::FVEC3 &rPos, nw4r::lyt::Size &rSize);

        private:
            // 0x60 (meaning not known): 1, 3 or 5 after Set, from the stage
            // byte at g_GameWork + 0x0B. Nothing else reads it.
            unsigned char m_nUnk60;
    };
}

#endif /* !CCOCKPITBOSSBALLPOWER_HPP_ */
