#ifndef CLOCKCURSORDODGE_HPP_
#define CLOCKCURSORDODGE_HPP_

#include "game/msw/CCockpitLayoutBase.hpp"

namespace msw {

    // Size 0x214 (CCockpitDodge::CreateObjImpl allocates it). The dodge
    // game's lock-on cursors, one object per view (DodgeF). Only the
    // constructor and virtuals so far; the members are not mapped.
    class CLockCursorDodge : public CCockpitLayoutBase {
        public:
            CLockCursorDodge(long nId);
            virtual ~CLockCursorDodge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

        private:
            unsigned char m_Unk60[0x214 - 0x60];
    };
}

#endif /* !CLOCKCURSORDODGE_HPP_ */
