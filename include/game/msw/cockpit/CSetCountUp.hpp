#ifndef MSW_COCKPIT_CSETCOUNTUP_HPP_
#define MSW_COCKPIT_CSETCOUNTUP_HPP_

#include "game/msw/menu/CDispTaskList.hpp"

namespace msw {
    namespace cockpit {

        // A display task that lights one won-set mark of a set counter after
        // a delay, with an effect. Lives in the DOL (constructor at
        // 0x802292C4). Size 0x44. Added for VolleyF
        // game/volley/CCockpitVolleyCoder.cpp, which fills the fields
        // directly before CDispTaskList::PushBack; their names are guesses
        // from that use.
        class CSetCountUp : public menu::CDispTaskLink {
            public:
                CSetCountUp();

                float m_fDelay;            // 0x30, frames before the mark lights
                unsigned short m_nUnk34;   // 0x34
                unsigned short m_nUnk36;   // 0x36
                long m_nUnk38;             // 0x38
                long m_nEffect;            // 0x3C, effect id
                unsigned char m_nAnim;     // 0x40
                unsigned char m_nWaitAnim; // 0x41
        };
    }
}

#endif /* !MSW_COCKPIT_CSETCOUNTUP_HPP_ */
