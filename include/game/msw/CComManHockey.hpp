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
            // A cell of the rink map: GetMapStatus copies 11 bytes of the
            // map's 12-byte cell (meanings not known; the computer driver
            // tests the low four bits of the first byte for 1, a blocked
            // cell).
            struct MAP_STATUS_DATA {
                unsigned char m_nAttr;
                unsigned char m_Unk01[0xB - 0x1];
            };

            static CComManHockey *GetInstance();
            // Keeper of team nTeam; nSide 0 is the team's own keeper, 1 the
            // other team's (from the callers, CComDrvHockey's
            // InitMyTeamKeeperCore / InitEmTeamKeeperCore).
            static CChrCore *GetKeeperCore(long nTeam, long nSide);
            // (added for HockeyF game/hockey/unit_B0048.cpp) A random number
            // from n1 to n2 (whether n2 is included is not known).
            static long GetRandValueI(long n1, long n2);

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

            // (added for HockeyF game/hockey/unit_EEA14.cpp; meanings
            // guessed from the names and the callers) The distance from
            // pChr to the nearest puck (rDist), false without one; whether
            // pChr's team has all its coins; where the boss Packun is
            // (false without one); the grid cell (rX, rZ) of rPos on a
            // grid of nX by nZ cells.
            static int GetDistanceFromBalls(const CChrCore *pChr, float, float &rDist);
            static int isTeamCoinMAX(const CChrCore *pChr);
            int isExistBossPackun(bcs::FVEC3 &rPos);
            static void GetGridIndex(long &rX, long &rZ, const bcs::FVEC3 &rPos, long nX, long nZ);
            // Whether pTarget is within reach of pChr's attack (the first
            // two or three parameters of the character's attack), or in
            // its third attack's area; whether an enemy stands within nDeg
            // of the line from pChr to pTarget; on which side of pChr
            // pTarget is (rPos and nDeg as the other checks; not positive
            // for neither).
            static int IsExistTargetCore(const CChrCore *pChr, const CChrCore *pTarget, float, float);
            static int IsExistInAttack3Area(const CChrCore *pChr, const CChrCore *pTarget, float, float, float);
            static int IsExistEnemyAngle(const CChrCore *pChr, const CChrCore *pTarget, long nDeg);
            static int IsExistToAngleLR(const CChrCore *pChr, const CChrCore *pTarget, long nDeg);
            static long IsExistTargetLR(const CChrCore *pChr, const CChrCore *pTarget, bcs::FVEC3 pos, long nDeg);
    };
}

#endif /* !MSW_CCOMMANHOCKEY_HPP_ */
