#ifndef CCOCKPITDODGE_HPP_
#define CCOCKPITDODGE_HPP_

namespace msw {

    // The dodge game's cockpit (module DodgeF; it is also a
    // bcs::CFsm<CCockpitDodge>). INCOMPLETE: only the static mission-count
    // functions CTmGameHandlerDodge uses, declared as in VolleyF's
    // CCockpitVolley; the first argument is 0 at every call seen (a team or
    // a pane index). No base, data or virtual functions yet, so the class
    // must not be instantiated or derived from.
    class CCockpitDodge {
        public:
            static void SetMissionCountDisp(long, int bDisp);
            static void SetMissionCountNum(long, long nCount, long nTarget);
            static void SetMissionCountUpNum(long, long, long);
            static void SetMissionCountActiveColor(int);
    };
}

#endif /* !CCOCKPITDODGE_HPP_ */
