#ifndef CCOCKPITBOSS_HPP_
#define CCOCKPITBOSS_HPP_

namespace msw {

    // The boss game's cockpit (size 0x21C, 78 member functions). Only the
    // static functions CEventDemoBoss calls so far (static: the first
    // argument goes in r3; return types and parameter names guessed).
    class CCockpitBoss {
        public:
            // Hides CCockpit::CreateObj.
            static long CreateObj(long nObjId, long nParam);
            static void SetPartMove(long nPart, long nMove);
            static void SetRadarIconDisp(int bDisp);
    };
}

#endif /* !CCOCKPITBOSS_HPP_ */
