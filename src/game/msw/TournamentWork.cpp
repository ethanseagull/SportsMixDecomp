#include "game/msw/TournamentWork.hpp"

#include <string.h>

namespace msw {
    namespace TournamentWork {

        // The tournament's teams, matches and data. In the original these
        // are file statics of a wider file that also holds msw::CTournament
        // and the static initialiser constructing them (0x8017B8D0); the
        // compiler then reaches all three from one base address. Until that
        // file is one unit they are defined there and declared here.
        extern TEAM_INFO s_aTeamInfo[16];
        extern MATCH_INFO s_aMatchInfo[16];
        extern TOURNAMENT_INFO s_TournamentInfo;

        const TOURNAMENT_INFO &GetTournamentInfo()
        {
            return s_TournamentInfo;
        }

        TOURNAMENT_INFO &TournamentInfo()
        {
            return s_TournamentInfo;
        }

        const MATCH_INFO &GetMatchInfo(long nMatch)
        {
            if (nMatch >= 0) {
                return s_aMatchInfo[nMatch];
            }
            return s_aMatchInfo[0];
        }

        MATCH_INFO &MatchInfo(long nMatch)
        {
            if (nMatch >= 0) {
                return s_aMatchInfo[nMatch];
            }
            return s_aMatchInfo[0];
        }

        const TEAM_INFO &GetTeamInfo(long nTeam)
        {
            if (nTeam >= 0) {
                return s_aTeamInfo[nTeam];
            }
            return s_aTeamInfo[0];
        }

        TEAM_INFO &TeamInfo(long nTeam)
        {
            if (nTeam >= 0) {
                return s_aTeamInfo[nTeam];
            }
            return s_aTeamInfo[0];
        }

        static inline void ClearScriptFlags()
        {
            for (long i = 0; i < 0x40; i++) {
                s_TournamentInfo.aFlag[i] = 0;
            }
        }

        static inline void ClearFirstFlags()
        {
            for (long i = 0; i < 0x80; i++) {
                s_TournamentInfo.aFirstFlag[i] = 0;
            }
        }

        void ResetFlags(long nType)
        {
            switch (nType) {
            case 2:
                ClearScriptFlags();
                break;
            case 1:
                ClearScriptFlags();
                ClearFirstFlags();
                for (long i = 0; i < 4; i++) {
                    s_TournamentInfo.aFirstFlag[i * 0x20] = 0;
                }
                break;
            }
        }

        // Route nRoute of the map; a negative index gives the first. (The
        // header's RouteInfo does the same through one reference; this file
        // reads the static in each branch.)
        static inline ROUTE_INFO *Route(long nRoute)
        {
            if (nRoute >= 0) {
                return &s_TournamentInfo.pRouteInfo[nRoute];
            }
            return s_TournamentInfo.pRouteInfo;
        }

        // The flags of the routes, and the second flag of the map points 9 to
        // 17 that change during a tournament, are kept in the tournament's
        // data. (meaning of the point flag unknown)
        void Backup()
        {
            for (long i = 0; i < 0x100; i++) {
                s_TournamentInfo.aRouteFlagBackup[i] = Route(i)->nFlag;
            }
            TOURNAMENT_INFO &info = s_TournamentInfo;
            info.aFlag[0x3A] = GetPointInfo(9)->m_Unk0E;
            info.aFlag[0x39] = GetPointInfo(10)->m_Unk0E;
            info.aFlag[0x38] = GetPointInfo(11)->m_Unk0E;
            info.aFlag[0x37] = GetPointInfo(12)->m_Unk0E;
            info.aFlag[0x36] = GetPointInfo(14)->m_Unk0E;
            info.aFlag[0x35] = GetPointInfo(17)->m_Unk0E;
        }

        void Restore()
        {
            for (long i = 0; i < 0x100; i++) {
                Route(i)->nFlag = s_TournamentInfo.aRouteFlagBackup[i];
            }
            TOURNAMENT_INFO &info = s_TournamentInfo;
            PointInfo(9)->m_Unk0E = info.aFlag[0x3A];
            PointInfo(10)->m_Unk0E = info.aFlag[0x39];
            PointInfo(11)->m_Unk0E = info.aFlag[0x38];
            PointInfo(12)->m_Unk0E = info.aFlag[0x37];
            PointInfo(14)->m_Unk0E = info.aFlag[0x36];
            PointInfo(17)->m_Unk0E = info.aFlag[0x35];
        }

        void Load(TOURNAMENT_SAVE *pSave)
        {
            TOURNAMENT_INFO &info = s_TournamentInfo;
            for (long i = 0; i < 3; i++) {
                memcpy(&info.aPlayer[i], &pSave->aPlayer[i], sizeof(PLAYER_INFO));
                memcpy(info.aPlayerName[i], pSave->aPlayerName[i], sizeof(info.aPlayerName[i]));
            }
            info.nTurnIndex = pSave->nTurnIndex;
            info.nTurnMode = pSave->nTurnMode;
            info.nPlayerMax = pSave->nPlayerMax;
            info.nLevel = pSave->nLevel;
            info.m_Unk1054[0] = pSave->m_Unk1B4[0];
            info.m_Unk1054[1] = pSave->m_Unk1B4[1];
            info.nStage = pSave->nStage;
            info.nBgm = pSave->nBgm;
            info.nExitStep = pSave->nExitStep;
            info.nEventNextState = pSave->nEventNextState;
            info.nUnk105A = pSave->nUnk1BA;
            info.nMarkerFlag = pSave->nMarkerFlag;
            info.fMarkerY = pSave->fMarkerY;
            info.nDemoType = pSave->nDemoType;
            info.nUnk1062 = pSave->nUnk1C2;
            memcpy(&info.nUnk103C, pSave->aLeaderTeam, sizeof(pSave->aLeaderTeam));
            memcpy(info.aTurnTeam, pSave->aTurnTeam, sizeof(info.aTurnTeam));
            memcpy(info.aRouteFlag, pSave->aRouteFlag, sizeof(info.aRouteFlag));
            memcpy(info.aRouteFlagBackup, pSave->aRouteFlagBackup, sizeof(info.aRouteFlagBackup));
            memcpy(info.aFlag, pSave->aFlag, sizeof(info.aFlag));
            memcpy(info.aFirstFlag, pSave->aFirstFlag, sizeof(info.aFirstFlag));
            memcpy(info.aGameModeOrder, pSave->aGameModeOrder, sizeof(info.aGameModeOrder));
            memcpy(info.aUnk1078, pSave->aUnk1078, sizeof(info.aUnk1078));
            for (long i = 0; i < 16; i++) {
                memcpy(&s_aTeamInfo[i], &pSave->aTeam[i], sizeof(TEAM_INFO));
            }
            for (long i = 0; i < 16; i++) {
                memcpy(&s_aMatchInfo[i], &pSave->aMatch[i], sizeof(MATCH_INFO));
            }
        }

        void Save(TOURNAMENT_SAVE *pSave)
        {
            TOURNAMENT_INFO &info = s_TournamentInfo;
            for (long i = 0; i < 3; i++) {
                memcpy(&pSave->aPlayer[i], &info.aPlayer[i], sizeof(PLAYER_INFO));
                memcpy(pSave->aPlayerName[i], info.aPlayerName[i], sizeof(info.aPlayerName[i]));
            }
            pSave->nTurnIndex = info.nTurnIndex;
            pSave->nTurnMode = info.nTurnMode;
            pSave->nPlayerMax = info.nPlayerMax;
            pSave->nLevel = info.nLevel;
            pSave->m_Unk1B4[0] = info.m_Unk1054[0];
            pSave->m_Unk1B4[1] = info.m_Unk1054[1];
            pSave->nStage = info.nStage;
            pSave->nBgm = info.nBgm;
            pSave->nExitStep = info.nExitStep;
            pSave->nEventNextState = info.nEventNextState;
            pSave->nUnk1BA = info.nUnk105A;
            pSave->nMarkerFlag = info.nMarkerFlag;
            pSave->fMarkerY = info.fMarkerY;
            pSave->nDemoType = info.nDemoType;
            pSave->nUnk1C2 = info.nUnk1062;
            memcpy(pSave->aLeaderTeam, &info.nUnk103C, sizeof(pSave->aLeaderTeam));
            memcpy(pSave->aTurnTeam, info.aTurnTeam, sizeof(info.aTurnTeam));
            memcpy(pSave->aRouteFlag, info.aRouteFlag, sizeof(info.aRouteFlag));
            memcpy(pSave->aRouteFlagBackup, info.aRouteFlagBackup, sizeof(info.aRouteFlagBackup));
            memcpy(pSave->aFlag, info.aFlag, sizeof(info.aFlag));
            memcpy(pSave->aFirstFlag, info.aFirstFlag, sizeof(info.aFirstFlag));
            memcpy(pSave->aGameModeOrder, info.aGameModeOrder, sizeof(info.aGameModeOrder));
            memcpy(pSave->aUnk1078, info.aUnk1078, sizeof(info.aUnk1078));
            for (long i = 0; i < 16; i++) {
                memcpy(&pSave->aTeam[i], &s_aTeamInfo[i], sizeof(TEAM_INFO));
            }
            for (long i = 0; i < 16; i++) {
                memcpy(&pSave->aMatch[i], &s_aMatchInfo[i], sizeof(MATCH_INFO));
            }
        }
    }
}
