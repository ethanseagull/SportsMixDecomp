#ifndef MSW_CCOMMANHOCKEY_HPP_
#define MSW_CCOMMANHOCKEY_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // The hockey computer manager (HockeyF), 0x12AC bytes; the one instance
    // is reached through GetInstance (null outside the hockey game).
    //
    // INCOMPLETE: only what game/hockey/CComDrvHockey.cpp calls is
    // declared, and no data, so it must not be instantiated yet.
    class CComManHockey : public CComMan {
        public:
            // A cell of the rink map (layout not mapped yet).
            struct MAP_STATUS_DATA;

            static CComManHockey *GetInstance();
            // Keeper of team nTeam; nSide 0 is the team's own keeper, 1 the
            // other team's (from the callers, CComDrvHockey's
            // InitMyTeamKeeperCore / InitEmTeamKeeperCore).
            static CChrCore *GetKeeperCore(long nTeam, long nSide);

            float GetElapsedTime();
            int CanReachTarget(long, int, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float);
            int isExistPillar();
            int GetMapStatus(short nX, short nZ, MAP_STATUS_DATA &rData);
            int GetPartyMapStatus(short nX, short nZ, unsigned char &, unsigned char &, char &);
            // (meaning of the floats not known) Whether pChr can reach
            // pTarget; static (CComManHockeyCS::IsExistTargetInArea passes
            // its own first argument in r3).
            static int IsExistTargetCore(const CChrCore *pChr, const CChrCore *pTarget, float, float, float);
            // (meaning guessed) Where the ball and pChr will be in fTime
            // frames, in the world.
            static bcs::FVEC3 GetBallWorldVelPos(const CBall *pBall, float fTime);
            static bcs::FVEC3 GetChrVelPos(const CChrCore *pChr, float fTime);
    };
}

#endif /* !MSW_CCOMMANHOCKEY_HPP_ */
