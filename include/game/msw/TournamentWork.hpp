#ifndef MSW_TOURNAMENTWORK_HPP_
#define MSW_TOURNAMENTWORK_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/FVEC3.hpp"
#include "revolution/rfl.h"

namespace msw {

    namespace TournamentData {
        struct CAMERA_INFO;
    }

    // The state of the running tournament. Lives in the DOL. Whether
    // TournamentWork is a namespace or a class with static members cannot be
    // told from the symbols (it has nested types, such as
    // msw::TournamentWork::TOURNAMENT_INFO); a namespace is a guess.
    namespace TournamentWork {

        // The tournament's data, one static object in the DOL. Only the
        // members used so far are mapped; msw::CTmGameHandler also reads and
        // writes bytes up to 0xFBB, and the object goes on past 0x1060.
        // One mission of a tournament match, 0x14 bytes, read and written by
        // msw::CTmGameHandler and the game modules' handlers. The type name
        // is RULE_INFO (msw::TournamentRule::_BuildMissionMessage takes one);
        // MISSION_INFO below is the name used before that was known.
        // (member names guessed)
        struct RULE_INFO {
            signed char nType;     // 0x00, what the mission asks for; values are per sport
            // 0x01, 0x40: shown on the cockpit's mission counter; the low four
            // bits say how TournamentRule::_BuildMissionMessage prints nTarget
            // (3: as a time, 2 and 4: as a stage name)
            unsigned char nFlags;
            short nTarget;         // 0x02, the count that clears the mission
            short nCount;          // 0x04, the current count
            short nPrevCount;      // 0x06, nCount before the last change
            unsigned char m_Unk08[4];
            float fCountTime;      // 0x0C, the float the count functions store
            unsigned char m_Unk10[4];
        };
        typedef RULE_INFO MISSION_INFO;

        // One match of the tournament, 0x10 bytes (GetMatchInfo indexes them
        // by 0x10). (member names guessed)
        struct MATCH_INFO {
            // The sport (CGame::GAME_MODE) of the match.
            long GetGameMode() const { return nFlags06 & 7; }

            unsigned char m_Unk00[6];
            // 0x06: bits 0-2 the sport; with 0x20 set the match is played on
            // stage 1.
            unsigned char nFlags06;
            signed char nRound;      // 0x07, passed to CGame::SetRound
            short nComLevel;         // 0x08, the computer teams' level in percent
            signed char nStage;      // 0x0A, the stage; TournamentRule::SetupGame writes back the one chosen
            unsigned char nRule;     // 0x0B, the match's rule script (CTmScript); 0 for none
            unsigned char m_Unk0C;
            // 0x0D, 0x0E: which side the user's team plays on, read by
            // TournamentRule (0 with 0x0D clear, 1 with 0x0D set and 0x0E
            // clear). (meaning guessed)
            signed char nSideFlag0D;
            signed char nSideFlag0E;
            unsigned char m_Unk0F;
        };

        // One team of the tournament, 0x24 bytes (GetTeamInfo indexes them by
        // 0x24). (member names guessed)
        struct TEAM_INFO {
            unsigned char m_Unk00[0x10];
            // 0x10: the last routes the team moved along, oldest first,
            // 0 for none (msw::CTournamentMan::PushMoveRoute). A route
            // number of 0x100 and up is route n - 0x100 run backwards.
            unsigned short aRoute[4];
            signed char aCharType[3];    // 0x18, the character of each member (0x13 and up: Mii)
            // 0x1B, per member: bits 0-1 the colour; with bit 7 set, bits 5-6
            // the player number that controls the member.
            unsigned char aCharFlag[3];
            unsigned char m_Unk1E;
            unsigned char nPoint;        // 0x1F, the map point the team is on; 0xFF: not on the map
            unsigned char m_Unk20[2];
            // 0x22, bit 1: set by TourF's Stage33 while a member of the
            // team rides an airship. (name and meaning guessed)
            unsigned char nFlag22;
            unsigned char m_Unk23;

            // (name guessed) The route the team moved along last, 0 for none.
            unsigned short GetLastRoute() const
            {
                unsigned short nRoute = 0;
                for (long i = 3; i >= 0; i--) {
                    if (aRoute[i] != 0) {
                        nRoute = aRoute[i];
                        break;
                    }
                }
                return nRoute;
            }
        };

        // A member of the user's team, 0x50 bytes. (member names guessed)
        struct PLAYER_INFO {
            signed char nCharType;   // 0x00
            signed char nCharColor;  // 0x01
            unsigned char m_Unk02[2];
            RFLStoreData miiData;    // 0x04, the Mii of a Mii member
        };

        struct POINT_INFO;
        struct ROUTE_INFO;

        struct TOURNAMENT_INFO {
            unsigned char m_Unk0000[0x20];
            // The texts of the next match's mission, from CNameMesg's
            // "_C", "_L" and "_H" mission strings, with their @...@ tags
            // replaced by TournamentRule. (names guessed)
            wchar_t szMissionTitle[0x100];   // 0x0020, "_C"
            wchar_t szMissionText[0x200];    // 0x0220, "_L", on one line
            wchar_t szMissionHelp[0x200];    // 0x0620, "_H"
            RULE_INFO aMission[16];          // 0x0A20
            unsigned char m_Unk0B60[0xCA1 - 0xB60];
            signed char nUnkCA1;             // 0x0CA1, passed to CGame::EnableRuleOptionFlag(0x100, ...)
            unsigned char m_Unk0CA2[4];
            signed char nUnkCA6;             // 0x0CA6, copied to CGame::GameWork 0x1C
            unsigned char m_Unk0CA7[0xCAC - 0xCA7];
            PLAYER_INFO aPlayer[3];          // 0x0CAC
            wchar_t aPlayerName[3][0x20];    // 0x0D9C
            unsigned char m_Unk0E5C[0x1052 - 0xE5C];
            // 0x1052, what CTournament::GetPlayerMax returns (set by
            // CTournament::SetPlayType)
            signed char nPlayerMax;
            signed char nLevel;              // 0x1053, what CTournament::GetLevel returns
            unsigned char m_Unk1054[2];
            // 0x1056, given to msw::CEvDemoRes as the stage of the map's
            // models (msw::CTournamentMan::_Init). (name guessed)
            signed char nStage;
            unsigned char m_Unk1057[0x105C - 0x1057];
            float fMarkerY;                  // 0x105C, the height of the player markers (name guessed)
            // The word msw::CTournament::SetDemoType stores (DOL): the demo
            // the tournament map is showing.
            short nDemoType; // 0x1060
            unsigned char m_Unk1062[2];
            // 0x1064: the colour the map is drawn in, faded towards by
            // msw::CTournamentMan::UpdateFrame. (name guessed)
            bcs::BCS_Color mapColor;
            const POINT_INFO *pPointInfo; // 0x1068, the map points of the stage
            const ROUTE_INFO *pRouteInfo; // 0x106C, the routes between them
            // 0x1070: the camera settings of the stage, two per camera kind
            // (the second for the other screen aspect). (name guessed)
            const TournamentData::CAMERA_INFO *pCameraInfo;
        };

        // One point of the tournament map, 0x14 bytes. (names guessed)
        struct POINT_INFO {
            bcs::FVEC3 vPos;          // 0x00
            // 0x0C, 0x0D: where the point's look-at spot is, on a grid of
            // 2 units with x from -16 and z negated; 0xFF for none.
            unsigned char nViewX;
            unsigned char nViewZ;
            unsigned char m_Unk0E[3];
            // 0x11: 0x10, 0x08 and 0x04 are passed on by CRouteBigGunOut
            // (TourF) as bits 0, 1 and 2 of a timer parameter.
            unsigned char nFlag;
            unsigned char m_Unk12;
            // 0x13: the first camera setting of the point
            // (msw::CTournamentMan::SetCameraPoint). (name guessed)
            unsigned char nCamera;

            int HasViewPos() const { return nViewX != 0xFF && nViewZ != 0xFF; }
            // The look-at spot relative to vPos.
            float GetViewOfsX() const { return 2.0f * (nViewX - 16); }
            float GetViewOfsZ() const { return 2.0f * -nViewZ; }
        };

        // One route of the tournament map, 8 bytes. (names guessed)
        struct ROUTE_INFO {
            // 0x00: negative for an unused route (MakePointData). (name guessed)
            signed char nType;
            // 0x01, 0x02: the map point at the route's start and at its
            // end, taken by a route run forwards and backwards.
            unsigned char nPoint1;
            unsigned char nPoint2;
            // 0x03: route flags; MakePointData passes them on to the links
            // and treats 0x20 as the direction the route may be run in.
            // (name guessed)
            unsigned char nFlag;
            // 0x04: passed on with the route to msw::CRouteBase::PushRoute.
            // (name guessed)
            unsigned char nParam;
            unsigned char nCamera;    // 0x05: what GetCameraDirect of the routes returns
            unsigned char m_Unk06[2];
        };

        // The route graph of the map, built by MakePointData and searched by
        // MakeRouteCost, GetPointFastRoute and SelectFastRouteBasic (in TourF,
        // game/tour/CTournamentMan.cpp). Names guessed.
        //
        // One way out of a map point, 8 bytes. The ways out of a point are a
        // list: the point's head entry, then nNext; 0 ends it.
        struct _LINK_POINT {
            short nNext;            // 0x00
            unsigned short nRoute;  // 0x02: the route; +0x100 when it is run backwards
            unsigned char nPoint;   // 0x04: the point it leads to
            unsigned char nFlag;    // 0x05: the route's flags
            short nCost;            // 0x06
        };

        // One step of a search for the fastest routes, 0x10 bytes. The
        // steps form a tree: nChild is the first step after this one,
        // nNext the next step after the same parent.
        struct _MOVE_POINT {
            short nNext;            // 0x00
            short nChild;           // 0x02
            short nParent;          // 0x04
            unsigned short nRoute;  // 0x06
            unsigned char nPoint;   // 0x08
            unsigned char nFlag;    // 0x09
            short nCost;            // 0x0A
            long m_Unk0C;
        };

        // Builds the ways out of every map point from the routes, from
        // entry nLink + 1 of pLink on; returns the last entry used.
        long MakePointData(short *pHead, _LINK_POINT *pLink, long nLinkMax, long nLink);
        // The cost of reaching every route from point nStart (pCost has
        // 0x200 entries, one per route and direction; -1: not reached),
        // along the routes whose flags have a bit of nMask. nCost is the
        // cost at the start.
        void MakeRouteCost(short *pCost, short *pHead, _LINK_POINT *pLink, unsigned char nStart,
                           unsigned char nMask, short nCost);
        // Grows the tree of the cheapest moves from point nStart at
        // pMove[nMove], by the costs MakeRouteCost left in pCost. nGoal and
        // nMoveMax are not read.
        void GetPointFastRoute(unsigned char nGoal, unsigned char nStart, unsigned char nMask, short *pCost,
                               short *pHead, _LINK_POINT *pLink, _MOVE_POINT *pMove, long nMoveMax, long nMove);
        // Writes the routes of the tree at pMove[nMove] that lead to point
        // nGoal to pRoute from entry nNum on; returns the new count. fRand
        // is not read.
        long SelectFastRouteBasic(unsigned char nGoal, unsigned short *pRoute, long nNum, long nMove,
                                  _MOVE_POINT *pMove, float fRand);

        // Both return the address of the same object. (reference or pointer
        // is a guess; so is the const of the first)
        const TOURNAMENT_INFO &GetTournamentInfo();
        TOURNAMENT_INFO &TournamentInfo();
        // Match nMatch and team nTeam (both clamp a negative index to 0),
        // read-only and writable as above.
        const MATCH_INFO &GetMatchInfo(long nMatch);
        MATCH_INFO &MatchInfo(long nMatch);
        const TEAM_INFO &GetTeamInfo(long nTeam);
        TEAM_INFO &TeamInfo(long nTeam);

        // (return type guessed) Starts the tournament map's music.
        void BgmPlay();

        // Map point nPoint and route nRoute; a negative index gives the
        // first. TourF has weak out-of-line copies of GetRouteInfo.
        // (names of GetPointInfo and of the helper below guessed)
        inline const POINT_INFO *GetPointInfo(long nPoint)
        {
            const TOURNAMENT_INFO &info = GetTournamentInfo();
            if (nPoint >= 0) {
                return &info.pPointInfo[nPoint];
            }
            return info.pPointInfo;
        }

        inline const ROUTE_INFO *GetRouteInfo(long nRoute)
        {
            const TOURNAMENT_INFO &info = GetTournamentInfo();
            if (nRoute >= 0) {
                return &info.pRouteInfo[nRoute];
            }
            return info.pRouteInfo;
        }

        // The look-at spot of map point nPoint; zero for point 0 and for a
        // point without one.
        inline void GetPointViewPos(bcs::FVEC3 &pos, long nPoint)
        {
            pos.Set(0.0f, 0.0f, 0.0f);
            if (nPoint > 0) {
                const POINT_INFO *pPoint = GetPointInfo(nPoint);
                if (pPoint->HasViewPos()) {
                    pos.Set(pPoint->GetViewOfsX() + pPoint->vPos.x, pPoint->vPos.y,
                            pPoint->GetViewOfsZ() + pPoint->vPos.z);
                }
            }
        }
    }
}

#endif /* !MSW_TOURNAMENTWORK_HPP_ */
