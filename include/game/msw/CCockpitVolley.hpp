#ifndef CCOCKPITVOLLEY_HPP_
#define CCOCKPITVOLLEY_HPP_

namespace msw {

    // The volley game's cockpit (size 0x21C, 78 member functions). Only the
    // static mission-count functions used by CTmGameHandlerVolley so far; the
    // first argument is 0 at every call seen (a team or a pane index).
    class CCockpitVolley {
        public:
            static void SetMissionCountDisp(long, int bDisp);
            static void SetMissionCountNum(long, long nCount, long nTarget);
            static void SetMissionCountUpNum(long, long, long);
            static void SetMissionCountActiveColor(int);
    };
}

#endif /* !CCOCKPITVOLLEY_HPP_ */
