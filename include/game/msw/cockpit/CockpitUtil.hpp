#ifndef MSW_COCKPIT_COCKPITUTIL_HPP_
#define MSW_COCKPIT_COCKPITUTIL_HPP_

namespace msw {
    class CChrCore;

    namespace cockpit {

        // Free helpers of the cockpits. Live in the DOL (0x801DFE40); the
        // file name is a guess.

        // Added for BossF game/boss/CCockpitBossCoder.cpp: the leader of
        // team nTeam. (It only branches to CChrMan::GetTeamLeader.)
        CChrCore *GetTeamLeaderChrCore(long nTeam);
    }
}

#endif /* !MSW_COCKPIT_COCKPITUTIL_HPP_ */
