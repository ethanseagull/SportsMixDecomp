#ifndef CCOCKPITTOURNAMENTTELOPLOGO_HPP_
#define CCOCKPITTOURNAMENTTELOPLOGO_HPP_

#include "game/msw/CCockpitModelTelopHold.hpp"
#include "game/msw/CCockpitTelopSwitchColor.hpp"

namespace msw {

    // The two classes are declared in this order because the unit emits
    // their vtables in the reverse order of declaration (TelopLogo first).

    // Size 0x84. The tournament's effect telop. Adds no data.
    class CCockpitTournamentModelTelopHold : public CCockpitModelTelopHold {
        public:
            CCockpitTournamentModelTelopHold(long nId, long nType, float fSortZ, long nUnk);
            virtual ~CCockpitTournamentModelTelopHold();
            virtual int Set(); // returns 0
    };

    // Size 0x498. The tournament's logo telop (layouts TO_logo_gn and
    // TO_cap_logo, chosen by nType).
    class CCockpitTournamentTelopLogo : public CCockpitTelopSwitchColor {
        public:
            CCockpitTournamentTelopLogo(long nId, long nType, float fSortZ);
            virtual ~CCockpitTournamentTelopLogo();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            // 0, or -1 / -2 when the layout cannot be built.
            virtual int Set();
            virtual void Remove();

            // Sends pText to telop nObjId for its text pane. -1 without a
            // cockpit task, else what the telop's SendEvent returns.
            static long SetParameter(long nObjId, const wchar_t *pText);

        private:
            unsigned char m_Unk478[0x20]; // 0x478, not used by this unit
    };
}

#endif /* !CCOCKPITTOURNAMENTTELOPLOGO_HPP_ */
