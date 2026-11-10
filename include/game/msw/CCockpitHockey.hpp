#ifndef CCOCKPITHOCKEY_HPP_
#define CCOCKPITHOCKEY_HPP_

namespace msw {

    // The hockey game's cockpit (size 0x21C, 82 member functions; HockeyF
    // game/hockey/CCockpitHockey.cpp). Only the static mission-count
    // functions used by CTmGameHandlerHockey so far; the first argument is 0
    // at every call seen (a team or a pane index).
    class CCockpitHockey {
        public:
            static void SetMissionCountDisp(long, int bDisp);
            static void SetMissionCountNum(long, long nCount, long nTarget);
            static void SetMissionCountUpNum(long, long, long);
            static void SetMissionCountActiveColor(int);
            // Used by CEventDemoHockey (static: the object id goes in r3;
            // return types guessed). Hides CCockpit::CreateObj.
            static long CreateObj(long nObjId, long nParam);
            static void SetPartMove(long, long nMove);
            static void SetRadarIconDisp(int bDisp);
    };
}

#endif /* !CCOCKPITHOCKEY_HPP_ */
