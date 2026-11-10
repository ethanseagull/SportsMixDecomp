#ifndef MSW_TOURNAMENTRULE_HPP_
#define MSW_TOURNAMENTRULE_HPP_

#include "game/msw/TournamentWork.hpp"

namespace msw {

    class CTmScript;

    // Sets CGame up for the next match of a tournament from the match's
    // data, its rule script and its missions (TourF
    // game/tour/TournamentRule.cpp). Whether TournamentRule is a namespace
    // or a class with static members is not known; a namespace is a guess,
    // as for TournamentWork.
    namespace TournamentRule {

        // What a match's rule script decides besides CGame's settings, filled
        // in by SetupGame and the per-sport _Setup...Script functions.
        // (member names guessed from their uses)
        struct _PARAM {
            long nMatch;        // 0x00, the match (TournamentWork::GetMatchInfo)
            long nTeam0;        // 0x04, the two teams (TournamentWork::GetTeamInfo)
            long nTeam1;        // 0x08
            long nPanel;        // 0x0C, for CTmScript::SetupPanel (rule 0x39)
            long nItemParam;    // 0x10, for CTmScript::SetupItemParam (rule 0x38)
            long nExclude;      // 0x14, for CTmScript::SetupExcludeStage (rule 0x0F)
            long nStage;        // 0x18, the stage asked for (rule 0x07)
            long nQPanelLevel;  // 0x1C, CGame::QPANEL_LEVEL (rules 0x03, 0x04)
            long nRule;         // 0x20, the mission texts' rule number (rule 0x3A)
        };

        // Fills the @...@ tag pTag in the tournament's mission texts with the
        // value of rule; does nothing when bSkip is set.
        void _BuildMissionMessage(TournamentWork::RULE_INFO &rule, const wchar_t *pTag, int bSkip);
        // Sets three characters of team nTeam up as game characters nChr,
        // nChr + nStep and nChr + 2 * nStep.
        void _SetupTeamChar(long nTeam, long nChr, long nStep);
        void _SetupGameParty(_PARAM &param, long nPlayType, long nStep);
        void _SetupGamePartyDodge(_PARAM &param, long nPlayType, long nStep);
        // Apply the tournament's rules (its missions) for each sport; with
        // bFirst set only what decides the stage is kept and no text is
        // built.
        void _SetupGameBasketScript(_PARAM &param, int bFirst);
        void _SetupGameVolleyScript(_PARAM &param, int bFirst);
        void _SetupGameDodgeScript(_PARAM &param, int bFirst);
        void _SetupGameHockeyScript(_PARAM &param, int bFirst);

        // Set the game of match nMatch (teams nTeam0 and nTeam1) or of the
        // boss match up; return the main step to go to next. (parameter
        // names of the last two guessed)
        long SetupGame(CTmScript *pScript, long nMatch, long nTeam0, long nTeam1, long nScriptArg, int bScriptFlag);
        long SetupGameBoss(long nTeam);
    }
}

#endif /* !MSW_TOURNAMENTRULE_HPP_ */
