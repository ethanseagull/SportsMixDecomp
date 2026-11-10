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

            // Used by CEventDemoDodge (return types and parameter names
            // guessed). CreateObj hides CCockpit::CreateObj. The life
            // functions take the team and the character's number in it.
            static long CreateObj(long nObjId, long nParam);
            static void SetPartMove(long nPart, long nMove);
            static void SetRadarIconDisp(int bDisp);
            static void AddLifeCoder(long nTeam, long nNo, long nPrevLife, long nLife, long, long);
            static void SetLifeGaugeState(long nTeam, long nNo, long nState, long, long);
            static void SetLifePercent(long nTeam, long nNo, float, float);
            static void SetLifeValue(long nTeam, long nNo, unsigned short nLife);
            static void SetOutSign_ChrNo(long nObjId, long nChr, long nTeam);
    };
}

#endif /* !CCOCKPITDODGE_HPP_ */
