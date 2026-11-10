#include "game/msw/TournamentRule.hpp"

#include <wchar.h>

#include "bcs/BCS_Rand.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CNameMesg.hpp"
#include "game/msw/CSave.hpp"
#include "game/msw/CTmScript.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CUnlockFlags.hpp"
#include "game/msw/SAVE_DATA_PLAYER.hpp"
#include "game/msw/TournamentWork.hpp"

namespace msw {

    namespace {

        // The unlock whose text names a character, by character type and
        // colour (TEAM_INFO::aCharType, the colour bits of aCharFlag).
        // (name guessed)
        const signed char s_aCharUnlock[25][6] = {
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x02, 0x14, 0x00, 0x00, 0x00, 0x00 },
            { 0x03, 0x15, 0x00, 0x00, 0x00, 0x00 },
            { 0x04, 0x16, 0x17, 0x18, 0x00, 0x00 },
            { 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x07, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x08, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x09, 0x19, 0x1A, 0x1B, 0x00, 0x00 },
            { 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x0E, 0x1C, 0x00, 0x00, 0x00, 0x00 },
            { 0x0F, 0x1D, 0x00, 0x00, 0x00, 0x00 },
            { 0x10, 0x1E, 0x1F, 0x00, 0x00, 0x00 },
            { 0x11, 0x20, 0x00, 0x00, 0x00, 0x00 },
            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x13, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x13, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 },
            { 0x13, 0x00, 0x00, 0x00, 0x00, 0x00 },
        };

        // The first character type that is a Mii.
        const long CHAR_TYPE_MII = 0x13;

        // The player that controls a team member: bits 5-6 of its flag byte
        // when bit 7 is set, -1 otherwise. (name guessed)
        inline long GetCharPlayerNo(unsigned char nFlag) {
            return (nFlag & 0x80) ? ((nFlag >> 5) & 3) : -1;
        }

        // Which side the user's team plays on in match nMatch: 0 or 1, -1
        // when the match does not say. (name and meaning guessed)
        inline long GetUserSide(long nMatch) {
            const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(nMatch);
            // Fetched and not used, as in the original (its call is there).
            const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
            long nSide;
            if (match.nSideFlag0D == 0) {
                nSide = 0;
            } else {
                nSide = -1;
                if (match.nSideFlag0E == 0) {
                    nSide = 1;
                }
            }
            return nSide;
        }

        // A rule's value, with a negative one read as 0.
        inline long GetPositive(short nValue) {
            return nValue >= 0 ? nValue : 0;
        }
    }

    void TournamentRule::_BuildMissionMessage(TournamentWork::RULE_INFO &rule, const wchar_t *pTag, int bSkip) {
        if (bSkip) {
            return;
        }

        // As in the original, szValue is left unset when a stage has no
        // name.
        wchar_t szValue[0x100];
        switch (rule.nFlags & 0xF) {
            case 3:
                swprintf(szValue, 0x100, L"%d:%02d", rule.nTarget / 60, rule.nTarget % 60);
                break;
            case 2: {
                const wchar_t *pName = CNameMesg::GetPartyStageName(-1, rule.nTarget);
                if (pName != 0) {
                    swprintf(szValue, 0x100, L"%ls", pName);
                }
                break;
            }
            case 4: {
                const wchar_t *pName = CNameMesg::GetPartyStageName(1, rule.nTarget);
                if (pName != 0) {
                    swprintf(szValue, 0x100, L"%ls", pName);
                }
                break;
            }
            default:
                swprintf(szValue, 0x100, L"%d", rule.nTarget);
                break;
        }

        CNameMesg::StrReplace(TournamentWork::TournamentInfo().szMissionTitle, 0x100, pTag, szValue);
        CNameMesg::StrReplace(TournamentWork::TournamentInfo().szMissionHelp, 0x200, pTag, szValue);
        CNameMesg::StrReplace(TournamentWork::TournamentInfo().szMissionText, 0x200, pTag, szValue);
    }

    void TournamentRule::_SetupTeamChar(long nTeam, long nChr, long nStep) {
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
        // Members nobody is assigned to: player 0 on the user's team, player
        // 1 on the others.
        long nDefaultPlayer = nTeam != 0;

        for (long i = 0; i < 3; i++) {
            long nNo = nChr + i * nStep;
            if (nNo > 6) {
                continue;
            }

            CGame::SetCharType(nNo, team.aCharType[i]);
            CGame::SetCharColor(nNo, team.aCharFlag[i] & 3);
            CGame::ResetCharExFlag(nNo, ~4UL);

            unsigned char nFlag = team.aCharFlag[i];
            long nPlayer = GetCharPlayerNo(nFlag);
            if (GetCharPlayerNo(nFlag) == -1) {
                nPlayer = nDefaultPlayer;
            }
            CGame::SetCharAssignPlayerNo(nNo, nPlayer);
            CGame::SetCharAssignPadNo(nNo, nPlayer);

            if (team.aCharType[i] >= CHAR_TYPE_MII) {
                // Only the user's team (team 0) has Miis of its own.
                if (nTeam == 0) {
                    CGame::SetMiiStoreData(nNo, info.aPlayer[i].miiData);
                    CGame::SetCharName(nNo, info.aPlayerName[i], 0x20);
                }
            } else {
                CGame::SetCharName(nNo, CNameMesg::GetCharName(team.aCharType[i], team.aCharFlag[i] & 3, 0), 0x20);
            }
        }
    }

    void TournamentRule::_SetupGameParty(_PARAM &param, long nPlayType, long nStep) {
        CGame::SetPlayType(static_cast<CGame::GAME_PLAY_TYPE>(nPlayType));
        for (long i = 0; i < 4; i++) {
            CGame::SetTeamCtrlType(i, 1);
        }
        // Every team is the computer's (1), then the first teams go to the
        // players (0); both meanings guessed. The cases fall through.
        switch (nPlayType) {
            case 8:
                CGame::SetTeamCtrlType(3, 0);
            case 7:
                CGame::SetTeamCtrlType(2, 0);
            case 6:
                CGame::SetTeamCtrlType(1, 0);
            case 4:
                CGame::SetTeamCtrlType(0, 0);
        }

        long nSide = GetUserSide(param.nMatch);
        if (nSide == -1) {
            nSide = 0;
        }
        long nChr = nStep * CTournament::GetTeamMax();
        _SetupTeamChar(param.nTeam0, nSide != 0 ? nChr : 0, nStep);
        _SetupTeamChar(param.nTeam1, nSide != 0 ? 0 : nChr, nStep);
    }

    void TournamentRule::_SetupGamePartyDodge(_PARAM &param, long nPlayType, long nStep) {
        CGame::SetPlayType(static_cast<CGame::GAME_PLAY_TYPE>(nPlayType));
        for (long i = 0; i < 4; i++) {
            CGame::SetTeamCtrlType(i, 1);
        }
        // Every team is the computer's (1), then the first teams go to the
        // players (0); both meanings guessed. The cases fall through.
        switch (nPlayType) {
            case 8:
                CGame::SetTeamCtrlType(3, 0);
            case 7:
                CGame::SetTeamCtrlType(2, 0);
            case 6:
                CGame::SetTeamCtrlType(1, 0);
            case 4:
                CGame::SetTeamCtrlType(0, 0);
        }

        long nSide = GetUserSide(param.nMatch);
        if (nSide == -1) {
            nSide = 0;
        }
        long nChr = nStep * CTournament::GetTeamMax();
        if (nSide != 0) {
            _SetupTeamChar(param.nTeam1, 0, nStep);
            _SetupTeamChar(param.nTeam0, nChr, nStep);
        } else {
            _SetupTeamChar(param.nTeam0, 0, nStep);
            _SetupTeamChar(param.nTeam1, nChr, nStep);
        }
    }

    void TournamentRule::_SetupGameBasketScript(_PARAM &param, int bFirst) {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        param.nPanel = 0;
        param.nItemParam = 0;
        param.nExclude = 0;

        for (long i = 0; i < 16; i++) {
            TournamentWork::RULE_INFO &rule = info.aMission[i];
            if (rule.nType == 0) {
                continue;
            }
            switch (rule.nType) {
                case 0x01:
                    CGame::SetGameType(CGame::GAME_TYPE_PARTY);
                    CGame::SetPartyGameParam(0, GetPositive(rule.nTarget));
                    break;
                case 0x06:
                    CGame::SetMaxTime(60.0f * rule.nTarget);
                    _BuildMissionMessage(rule, L"@time@", bFirst);
                    break;
                case 0x08:
                    for (long nTeam = 0; nTeam < 4; nTeam++) {
                        CGame::SetTeamComLevel(nTeam, rule.nTarget / 100.0f);
                    }
                    break;
                case 0x07:
                    if (bFirst) {
                        param.nStage = rule.nTarget;
                    }
                    break;
                case 0x09:
                    CGame::SetMaxPeriod(rule.nTarget);
                    _BuildMissionMessage(rule, L"@period@", bFirst);
                    break;
                case 0x0D:
                    CGame::SetMaxShotTime(60.0f * rule.nTarget);
                    _BuildMissionMessage(rule, L"@shotclock@", bFirst);
                    break;
                case 0x04:
                    if (bFirst) {
                        param.nQPanelLevel = 0;
                    }
                    break;
                case 0x03:
                    if (bFirst) {
                        param.nQPanelLevel = 4;
                    }
                    break;
                case 0x05:
                    CGame::EnableRuleOptionFlag(8, 1);
                    break;
                case 0x0B:
                    CGame::SetPeriodStartScore(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@advantage_enemy@", bFirst);
                    break;
                case 0x15:
                    CGame::SetPeriodStartCoin(0, rule.nTarget);
                    _BuildMissionMessage(rule, L"@coin_player@", bFirst);
                    break;
                case 0x14:
                    CGame::SetPeriodStartCoin(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@coin_enemy@", bFirst);
                    break;
                case 0x3B:
                    CGame::SetGimmickParamIdx(rule.nTarget);
                    break;
                case 0x3A:
                    param.nRule = rule.nTarget;
                    break;
                case 0x39:
                    param.nPanel = rule.nTarget;
                    break;
                case 0x38:
                    param.nItemParam = rule.nTarget;
                    break;
                case 0x0F:
                    param.nExclude = rule.nTarget;
                    break;
                case 0x4F:
                    _BuildMissionMessage(rule, L"@steal@", bFirst);
                    break;
                case 0x1A:
                    _BuildMissionMessage(rule, L"@take_first@", bFirst);
                    break;
                case 0x46:
                    _BuildMissionMessage(rule, L"@throwing_shoot@", bFirst);
                    break;
                case 0x44:
                    _BuildMissionMessage(rule, L"@3P_shoot@", bFirst);
                    break;
            }
        }
    }

    void TournamentRule::_SetupGameVolleyScript(_PARAM &param, int bFirst) {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        param.nPanel = 0;
        param.nItemParam = 0;
        param.nExclude = 0;

        for (long i = 0; i < 16; i++) {
            TournamentWork::RULE_INFO &rule = info.aMission[i];
            if (rule.nType == 0) {
                continue;
            }
            switch (rule.nType) {
                case 0x01:
                    CGame::SetGameType(CGame::GAME_TYPE_PARTY);
                    CGame::SetPartyGameParam(1, GetPositive(rule.nTarget));
                    _BuildMissionMessage(rule, L"@rhythm_receive_clear@", bFirst);
                    break;
                case 0x02:
                    CGame::SetPartyGameParam(0, GetPositive(rule.nTarget));
                    break;
                case 0x08:
                    for (long nTeam = 0; nTeam < 4; nTeam++) {
                        CGame::SetTeamComLevel(nTeam, rule.nTarget / 100.0f);
                    }
                    break;
                case 0x07:
                    if (bFirst) {
                        param.nStage = rule.nTarget;
                    }
                    break;
                case 0x09:
                    CGame::SetMaxPeriod(rule.nTarget);
                    _BuildMissionMessage(rule, L"@set@", bFirst);
                    break;
                case 0x0A:
                    CGame::SetWinPoint(rule.nTarget);
                    _BuildMissionMessage(rule, L"@set_point@", bFirst);
                    break;
                case 0x04:
                    if (bFirst) {
                        param.nQPanelLevel = 0;
                    }
                    break;
                case 0x03:
                    if (bFirst) {
                        param.nQPanelLevel = 4;
                    }
                    break;
                case 0x05:
                    CGame::EnableRuleOptionFlag(8, 1);
                    break;
                case 0x0E:
                    CGame::g_GameWork.m_nUnk1C8 = 2;
                    break;
                case 0x0B:
                    CGame::SetPeriodStartScore(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@advantage_enemy@", bFirst);
                    break;
                case 0x0C:
                    CGame::SetPeriodStartScore(0, rule.nTarget);
                    CGame::SetPeriodStartScore(1, rule.nTarget);
                    break;
                case 0x15:
                    CGame::SetPeriodStartCoin(0, rule.nTarget);
                    _BuildMissionMessage(rule, L"@coin_player@", bFirst);
                    break;
                case 0x14:
                    CGame::SetPeriodStartCoin(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@coin_enemy@", bFirst);
                    break;
                case 0x3B:
                    CGame::SetGimmickParamIdx(rule.nTarget);
                    break;
                case 0x3A:
                    param.nRule = rule.nTarget;
                    break;
                case 0x39:
                    param.nPanel = rule.nTarget;
                    break;
                case 0x38:
                    param.nItemParam = rule.nTarget;
                    break;
                case 0x0F:
                    param.nExclude = rule.nTarget;
                    break;
                case 0x3F:
                    _BuildMissionMessage(rule, L"@bingo@", bFirst);
                    break;
                case 0x1E:
                    _BuildMissionMessage(rule, L"@superspike_count@", bFirst);
                    break;
                case 0x22:
                    _BuildMissionMessage(rule, L"@block_count@", bFirst);
                    break;
                case 0x06:
                    CGame::SetMaxTime(60.0f * rule.nTarget);
                    break;
            }
        }
    }

    void TournamentRule::_SetupGameDodgeScript(_PARAM &param, int bFirst) {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        param.nPanel = 0;
        param.nItemParam = 0;
        param.nExclude = 0;

        for (long i = 0; i < 16; i++) {
            TournamentWork::RULE_INFO &rule = info.aMission[i];
            if (rule.nType == 0) {
                continue;
            }
            switch (rule.nType) {
                case 0x01:
                    CGame::SetGameType(CGame::GAME_TYPE_PARTY);
                    CGame::SetPartyGameParam(0, GetPositive(rule.nTarget));
                    break;
                case 0x06:
                    CGame::SetMaxTime(60.0f * rule.nTarget);
                    _BuildMissionMessage(rule, L"@time@", bFirst);
                    break;
                case 0x08:
                    for (long nTeam = 0; nTeam < 4; nTeam++) {
                        CGame::SetTeamComLevel(nTeam, rule.nTarget / 100.0f);
                    }
                    break;
                case 0x07:
                    if (bFirst) {
                        param.nStage = rule.nTarget;
                    }
                    break;
                case 0x09:
                    CGame::SetMaxPeriod(rule.nTarget);
                    _BuildMissionMessage(rule, L"@set@", bFirst);
                    break;
                case 0x0D:
                    CGame::SetMaxShotTime(60.0f * rule.nTarget);
                    _BuildMissionMessage(rule, L"@shotclock@", bFirst);
                    break;
                case 0x0B:
                    CGame::SetTeamInitLife(0, rule.nTarget);
                    _BuildMissionMessage(rule, L"@life_player@", bFirst);
                    break;
                case 0x12:
                    CGame::SetTeamMaxLife(0, rule.nTarget);
                    break;
                case 0x3E:
                    CGame::SetTeamInitLife(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@life_enemy@", bFirst);
                    break;
                case 0x3F:
                    CGame::SetTeamMaxLife(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@max_life_enemy@", bFirst);
                    break;
                case 0x0A:
                    CGame::SetTeamMaxLife(0, rule.nTarget);
                    CGame::SetTeamMaxLife(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@life@", bFirst);
                    break;
                case 0x04:
                    if (bFirst) {
                        param.nQPanelLevel = 0;
                    }
                    break;
                case 0x03:
                    if (bFirst) {
                        param.nQPanelLevel = 4;
                    }
                    break;
                case 0x05:
                    CGame::EnableRuleOptionFlag(8, 1);
                    break;
                case 0x0E:
                    CGame::SetTeamRebirthLife(0, rule.nTarget);
                    CGame::SetTeamRebirthLife(1, rule.nTarget);
                    break;
                case 0x0C:
                    CGame::EnableRuleOptionFlag(0x10, 1);
                    break;
                case 0x10:
                    CGame::EnableRuleOptionFlag(0x20, 1);
                    break;
                case 0x15:
                    CGame::SetPeriodStartCoin(0, rule.nTarget);
                    _BuildMissionMessage(rule, L"@power_player@", bFirst);
                    break;
                case 0x14:
                    CGame::SetPeriodStartCoin(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@power_enemy@", bFirst);
                    break;
                case 0x3B:
                    CGame::SetGimmickParamIdx(rule.nTarget);
                    break;
                case 0x3A:
                    param.nRule = rule.nTarget;
                    break;
                case 0x39:
                    param.nPanel = rule.nTarget;
                    break;
                case 0x38:
                    param.nItemParam = rule.nTarget;
                    break;
                case 0x0F:
                    param.nExclude = rule.nTarget;
                    break;
                case 0x20:
                    _BuildMissionMessage(rule, L"@catch@", bFirst);
                    break;
            }
        }
    }

    void TournamentRule::_SetupGameHockeyScript(_PARAM &param, int bFirst) {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        param.nPanel = 0;
        param.nItemParam = 0;
        param.nExclude = 0;

        for (long i = 0; i < 16; i++) {
            TournamentWork::RULE_INFO &rule = info.aMission[i];
            if (rule.nType == 0) {
                continue;
            }
            switch (rule.nType) {
                case 0x01:
                    CGame::SetGameType(CGame::GAME_TYPE_PARTY);
                    CGame::SetPartyGameParam(0, GetPositive(rule.nTarget));
                    break;
                case 0x06:
                    CGame::SetMaxTime(60.0f * rule.nTarget);
                    _BuildMissionMessage(rule, L"@time@", bFirst);
                    break;
                case 0x08:
                    for (long nTeam = 0; nTeam < 4; nTeam++) {
                        CGame::SetTeamComLevel(nTeam, rule.nTarget / 100.0f);
                    }
                    break;
                case 0x07:
                    if (bFirst) {
                        param.nStage = rule.nTarget;
                    }
                    break;
                case 0x09:
                    CGame::SetMaxPeriod(rule.nTarget);
                    _BuildMissionMessage(rule, L"@period@", bFirst);
                    break;
                case 0x04:
                    if (bFirst) {
                        param.nQPanelLevel = 0;
                    }
                    break;
                case 0x03:
                    if (bFirst) {
                        param.nQPanelLevel = 4;
                    }
                    break;
                case 0x05:
                    CGame::EnableRuleOptionFlag(8, 1);
                    break;
                case 0x0B:
                    CGame::SetPeriodStartScore(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@advantage_enemy@", bFirst);
                    break;
                case 0x15:
                    CGame::SetPeriodStartCoin(0, rule.nTarget);
                    _BuildMissionMessage(rule, L"@coin_player@", bFirst);
                    break;
                case 0x14:
                    CGame::SetPeriodStartCoin(1, rule.nTarget);
                    _BuildMissionMessage(rule, L"@coin_enemy@", bFirst);
                    break;
                case 0x3B:
                    CGame::SetGimmickParamIdx(rule.nTarget);
                    break;
                case 0x3A:
                    param.nRule = rule.nTarget;
                    break;
                case 0x39:
                    param.nPanel = rule.nTarget;
                    break;
                case 0x38:
                    param.nItemParam = rule.nTarget;
                    break;
                case 0x0F:
                    param.nExclude = rule.nTarget;
                    break;
                case 0x1B:
                    _BuildMissionMessage(rule, L"@score_streak@", bFirst);
                    break;
                case 0x1A:
                    _BuildMissionMessage(rule, L"@take_first@", bFirst);
                    break;
                case 0x20:
                    _BuildMissionMessage(rule, L"@shoot_count@", bFirst);
                    break;
                case 0x21:
                    _BuildMissionMessage(rule, L"@super_shoot@", bFirst);
                    break;
                case 0x3F:
                    _BuildMissionMessage(rule, L"@steal@", bFirst);
                    break;
            }
        }
    }

    namespace {

        // The game of a sport played the normal way: teams 0 and 1 face each
        // other, their members interleaved as characters 0, 2, 4 and 1, 3, 5.
        // (name guessed)
        inline void SetupGameVs(TournamentRule::_PARAM &param, long nGameMode) {
            CGame::SetPlayType(static_cast<CGame::GAME_PLAY_TYPE>(
                CTournament::GetPlayerType(nGameMode, TournamentWork::GetTournamentInfo().nPlayerMax)));
            for (long i = 0; i < 4; i++) {
                CGame::SetTeamCtrlType(i, 1);
            }
            if (!(CGame::g_GameWork.GetUnk06() >= 11 && CGame::g_GameWork.GetUnk06() <= 13)) {
                CGame::SetTeamCtrlType(0, 0);
            }

            long nSide = GetUserSide(param.nMatch);
            if (nSide == -1) {
                nSide = 0;
            }
            TournamentRule::_SetupTeamChar(param.nTeam0, nSide, 2);
            TournamentRule::_SetupTeamChar(param.nTeam1, nSide ^ 1, 2);
        }
    }

    long TournamentRule::SetupGame(CTmScript *pScript, long nMatch, long nTeam0, long nTeam1, long nScriptArg, int bScriptFlag) {
        _PARAM param;
        param.nMatch = nMatch;
        param.nTeam0 = nTeam0;
        param.nTeam1 = nTeam1;

        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        long nLevel = info.nLevel;
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(nMatch);

        pScript->SetupRule(match.GetGameMode(), match.nRule, nScriptArg, nLevel, bScriptFlag);
        CGame::SetDefaultParam();
        for (long i = 0; i < 8; i++) {
            CGame::SetCharType(i, -1);
            CGame::SetCharColor(i, 0);
            CGame::SetCharExFlag(i, 0);
        }
        CGame::SetCup(static_cast<CGame::GAME_CUP>(CTournament::GetCup()));
        if (CTournament::IsMixTournamentGame()) {
            CGame::SetSportsMode(CGame::SPORTS_MODE_MIX);
        } else {
            CGame::SetSportsMode(CGame::SPORTS_MODE_SINGLE);
        }
        CGame::SetGameMode(static_cast<CGame::GAME_MODE>(match.GetGameMode()));
        CGame::SetGameType(CGame::GAME_TYPE_NORMAL);
        CGame::SetTournamentLevel(static_cast<CGame::GAME_TOURNAMENT_LEVEL>(nLevel));
        CGame::EnableRuleOptionFlag(0x100, TournamentWork::GetTournamentInfo().nUnkCA1);
        CGame::g_GameWork.m_nUnk1C = info.nUnkCA6;

        SAVE_DATA_PLAYER *pSave = CSave::GetPlayerData(-1);
        if (pSave != 0) {
            switch (CGame::g_GameWork.m_nGameMode) {
                case CGame::GAME_MODE_BASKET:
                    CGame::SetTeamChrChangeCtrlType(0, pSave->bBasketChrChange != 0);
                    break;
                case CGame::GAME_MODE_DODGE:
                    CGame::SetTeamChrChangeCtrlType(0, pSave->bDodgeChrChange != 0);
                    break;
                case CGame::GAME_MODE_HOCKEY:
                    CGame::SetTeamChrChangeCtrlType(0, pSave->bHockeyChrChange != 0);
                    break;
            }
        }

        switch (nLevel) {
            case 1:
                CGame::SetGameLevel(CGame::GAME_LEVEL_HARD);
                param.nQPanelLevel = 3;
                break;
            case 2:
            case 3:
                CGame::SetGameLevel(CGame::GAME_LEVEL_EXPERT);
                param.nQPanelLevel = 3;
                break;
            default:
                if (CTournament::GetCup() == 1) {
                    CGame::SetGameLevel(CGame::GAME_LEVEL_EASY);
                    if (match.nRound >= 4) {
                        param.nQPanelLevel = 3;
                    } else {
                        param.nQPanelLevel = 1;
                        SAVE_DATA_PLAYER *pPlayer = CSave::GetPlayerData(-1);
                        if (pPlayer != 0 && pPlayer->IsFirstPlay(1, match.GetGameMode())) {
                            param.nQPanelLevel = 0;
                        }
                    }
                } else {
                    CGame::SetGameLevel(CGame::GAME_LEVEL_NORMAL);
                    if (match.nRound >= 4) {
                        param.nQPanelLevel = 3;
                    } else {
                        param.nQPanelLevel = 2;
                    }
                }
                break;
        }

        param.nRule = match.nRule;
        if (match.nFlags06 & 0x20) {
            match.nStage = 1;
        }
        param.nStage = match.nStage;
        CGame::SetRound(match.nRound);
        for (long i = 0; i < 4; i++) {
            CGame::SetTeamComLevel(i, match.nComLevel / 100.0f);
        }
        for (long i = 0; i < 16; i++) {
            if (info.aMission[i].nType == 1) {
                CGame::SetGameType(CGame::GAME_TYPE_PARTY);
            }
        }

        switch (CGame::g_GameWork.m_nGameType) {
            case CGame::GAME_TYPE_NORMAL:
                if (CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_BASKET) {
                    CGame::SetMaxShotTime(1440.0f);
                }
                break;
            case CGame::GAME_TYPE_PARTY:
                // Each sport's party game is played on its own stage.
                switch (CGame::g_GameWork.m_nGameMode) {
                    case CGame::GAME_MODE_BASKET:
                        CGame::SetPartyGameParam(0, 0);
                        param.nStage = 0x46;
                        break;
                    case CGame::GAME_MODE_VOLLEY:
                        CGame::SetPartyGameParam(0, 0);
                        CGame::SetPartyGameParam(1, 1500);
                        param.nStage = 0x28;
                        break;
                    case CGame::GAME_MODE_DODGE:
                        CGame::SetPartyGameParam(0, 0);
                        param.nStage = 0x37;
                        break;
                    case CGame::GAME_MODE_HOCKEY:
                        CGame::SetMaxPeriod(5);
                        CGame::SetPartyGameParam(0, 0);
                        param.nStage = 0x55;
                        break;
                }
                break;
        }

        // First pass over the rules: only what decides the stage.
        switch (match.GetGameMode()) {
            case CGame::GAME_MODE_BASKET:
                _SetupGameBasketScript(param, 1);
                break;
            case CGame::GAME_MODE_VOLLEY:
                _SetupGameVolleyScript(param, 1);
                break;
            case CGame::GAME_MODE_DODGE:
                _SetupGameDodgeScript(param, 1);
                break;
            case CGame::GAME_MODE_HOCKEY:
                _SetupGameHockeyScript(param, 1);
                break;
        }

        switch (CGame::g_GameWork.m_nGameType) {
            case CGame::GAME_TYPE_NORMAL: {
                long nGameMode = CGame::g_GameWork.m_nGameMode;
                switch (nGameMode) {
                    case CGame::GAME_MODE_BASKET:
                        SetupGameVs(param, nGameMode);
                        break;
                    case CGame::GAME_MODE_VOLLEY:
                        SetupGameVs(param, nGameMode);
                        break;
                    case CGame::GAME_MODE_DODGE:
                        SetupGameVs(param, nGameMode);
                        break;
                    case CGame::GAME_MODE_HOCKEY:
                        SetupGameVs(param, nGameMode);
                        break;
                }
                break;
            }
            case CGame::GAME_TYPE_PARTY: {
                long nGameMode = CGame::g_GameWork.m_nGameMode;
                switch (nGameMode) {
                    case CGame::GAME_MODE_BASKET:
                        SetupGameVs(param, nGameMode);
                        break;
                    case CGame::GAME_MODE_VOLLEY:
                        _SetupGameParty(param, CTournament::GetPlayerTypeParty(nGameMode, TournamentWork::GetTournamentInfo().nPlayerMax), 2);
                        break;
                    case CGame::GAME_MODE_DODGE:
                        _SetupGamePartyDodge(param, CTournament::GetPlayerTypeParty(nGameMode, TournamentWork::GetTournamentInfo().nPlayerMax), 1);
                        break;
                    case CGame::GAME_MODE_HOCKEY:
                        SetupGameVs(param, nGameMode);
                        break;
                }
                break;
            }
        }

        long nStage = pScript->SetupExcludeStage(param.nStage, param.nExclude, nLevel, bScriptFlag);
        TournamentWork::MatchInfo(nMatch).nStage = nStage;
        CGame::SetStage(nStage);
        CGame::SetQPanelLevel(static_cast<CGame::QPANEL_LEVEL>(param.nQPanelLevel));
        CGame::SetStageParam(bcs::BCS_Rand());
        CGame::SetParamSemiAuto();

        // The arguments look swapped (fill value 0x100, count 0): as in the
        // original, these clear nothing. The texts are overwritten below
        // when the match has a rule.
        wmemset(info.szMissionTitle, 0x100, 0);
        wmemset(info.szMissionHelp, 0x200, 0);
        wmemset(info.szMissionText, 0x200, 0);
        if (param.nRule > 0) {
            const wchar_t *pStr = CNameMesg::GetMissionStrings(CGame::g_GameWork.m_nGameMode, -1, 1, "_C");
            if (pStr != 0 && wcslen(pStr) != 0) {
                swprintf(info.szMissionTitle, 0x100, L"%ls", pStr);
            }
            pStr = CNameMesg::GetMissionStrings(CGame::g_GameWork.m_nGameMode, CGame::g_GameWork.m_nUnk07, param.nRule, "_H");
            if (pStr != 0 && wcslen(pStr) != 0) {
                swprintf(info.szMissionHelp, 0x200, L"%ls", pStr);
            }
            pStr = CNameMesg::GetMissionStrings(CGame::g_GameWork.m_nGameMode, CGame::g_GameWork.m_nUnk07, param.nRule, "_L");
            if (pStr != 0 && wcslen(pStr) != 0) {
                swprintf(info.szMissionText, 0x200, L"%ls", pStr);
            }
        }

        // Second pass: everything else, with the mission texts' tags.
        switch (CGame::g_GameWork.m_nGameMode) {
            case CGame::GAME_MODE_BASKET:
                _SetupGameBasketScript(param, 0);
                break;
            case CGame::GAME_MODE_VOLLEY:
                _SetupGameVolleyScript(param, 0);
                break;
            case CGame::GAME_MODE_DODGE:
                _SetupGameDodgeScript(param, 0);
                break;
            case CGame::GAME_MODE_HOCKEY:
                _SetupGameHockeyScript(param, 0);
                break;
        }

        if (param.nRule > 0) {
            long nUnlock = -1;
            int bPartyStage = 0;
            long nCharType = -1;
            long nCharColor = 0;
            // Both fetched and not used further, as in the original (the
            // calls are there).
            const TournamentWork::TOURNAMENT_INFO &tournament = TournamentWork::GetTournamentInfo();
            CUnlockFlags unlock;
            unlock.SetPlayerData(-1);

            // The party stages of each sport; for three sports the stage's
            // own unlock when the party game parameter is set.
            switch (match.GetGameMode()) {
                case CGame::GAME_MODE_BASKET:
                    if (match.nStage >= 0x46 && match.nStage <= 0x54) {
                        bPartyStage = 1;
                        if (CGame::GetPartyGameParam(0) > 0) {
                            nUnlock = match.nStage - 0x43;
                        }
                    }
                    break;
                case CGame::GAME_MODE_VOLLEY:
                    if (match.nStage >= 0x28 && match.nStage <= 0x36) {
                        bPartyStage = 1;
                    }
                    break;
                case CGame::GAME_MODE_DODGE:
                    if (match.nStage >= 0x37 && match.nStage <= 0x45) {
                        bPartyStage = 1;
                        if (CGame::GetPartyGameParam(0) > 0) {
                            nUnlock = match.nStage - 0x34;
                        }
                    }
                    break;
                case CGame::GAME_MODE_HOCKEY:
                    if (match.nStage >= 0x55 && match.nStage <= 0x63) {
                        bPartyStage = 1;
                        if (CGame::GetPartyGameParam(0) > 0) {
                            nUnlock = match.nStage - 0x52;
                        }
                    }
                    break;
            }

            if (bPartyStage) {
                if (nUnlock != -1) {
                    const wchar_t *pName = CNameMesg::GetMissionUnlockStrings(match.nStage, "_S");
                    if (pName != 0 && wcslen(pName) != 0) {
                        CNameMesg::StrReplace(info.szMissionHelp, 0x200, L"@stage_name@", pName);
                        CNameMesg::StrReplace(info.szMissionText, 0x200, L"@stage_name@", pName);
                    }
                }
            } else {
                const wchar_t *pName = CNameMesg::GetMissionUnlockStrings(match.nStage, "_S");
                if (pName != 0 && wcslen(pName) != 0) {
                    CNameMesg::StrReplace(info.szMissionHelp, 0x200, L"@stage_name@", pName);
                    CNameMesg::StrReplace(info.szMissionText, 0x200, L"@stage_name@", pName);
                }
            }

            // The character named in the texts: the first member of the team
            // the user's team (team 0) plays against.
            if (param.nTeam0 == 0) {
                const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(param.nTeam1);
                nCharType = team.aCharType[0];
                nCharColor = team.aCharFlag[0] & 3;
            } else if (param.nTeam1 == 0) {
                const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(param.nTeam0);
                nCharType = team.aCharType[0];
                nCharColor = team.aCharFlag[0] & 3;
            }
            if (nCharType != -1) {
                const wchar_t *pName = CNameMesg::GetMissionUnlockStrings(s_aCharUnlock[nCharType][nCharColor], "_C");
                if (pName != 0 && wcslen(pName) != 0) {
                    CNameMesg::StrReplace(info.szMissionHelp, 0x200, L"@character_name@", pName);
                    CNameMesg::StrReplace(info.szMissionText, 0x200, L"@character_name@", pName);
                }
            }

            CNameMesg::StrReplace(info.szMissionText, 0x200, L"\n", L" ");
        }

        int bOption = 0;
        if (pSave != 0) {
            switch (CGame::g_GameWork.m_nGameMode) {
                case CGame::GAME_MODE_BASKET:
                    bOption = pSave->nBasketOpt == 0;
                    break;
                case CGame::GAME_MODE_VOLLEY:
                    bOption = pSave->nVolleyOpt == 0;
                    break;
                case CGame::GAME_MODE_DODGE:
                    bOption = pSave->nDodgeOpt == 0;
                    break;
                case CGame::GAME_MODE_HOCKEY:
                    bOption = pSave->nHockeyOpt == 0;
                    break;
            }
        }
        CGame::EnableRuleOptionFlag(0x200, bOption);
        CGame::EnableRuleOptionFlag(0x40, 0);

        if (param.nPanel != 0) {
            pScript->SetupPanel(param.nPanel, nLevel, bScriptFlag);
        }
        if (param.nItemParam != 0) {
            pScript->SetupItemParam(param.nItemParam, nLevel, bScriptFlag);
        }

        if (CGame::g_GameWork.GetGameType() == CGame::GAME_TYPE_PARTY) {
            CGame::SetGameLevel(CGame::GAME_LEVEL_HARD);
            CGame::SetQPanelLevel(static_cast<CGame::QPANEL_LEVEL>(2));
            if (CGame::g_GameWork.GetGameMode() == CGame::GAME_MODE_DODGE) {
                // Life of the user's team and of the computer's teams, by
                // the number of players.
                static const long s_nPlayerLife[4] = { 150, 150, 250, 350 };
                static const long s_nComLife[4] = { 150, 150, 200, 400 };

                long nLife = s_nPlayerLife[TournamentWork::GetTournamentInfo().nPlayerMax];
                CGame::SetTeamMaxLife(0, nLife);
                CGame::SetTeamInitLife(0, nLife);
                nLife = s_nComLife[TournamentWork::GetTournamentInfo().nPlayerMax];
                for (long nTeam = 1; nTeam <= 3; nTeam++) {
                    CGame::SetTeamMaxLife(nTeam, nLife);
                    CGame::SetTeamInitLife(nTeam, nLife);
                }
            }
        }

        CGame::SetReturnStep(1);
        static const long g_nNextGameStep[5] = { 5, 6, 7, 8, 9 };
        return g_nNextGameStep[CGame::g_GameWork.m_nGameMode];
    }

    long TournamentRule::SetupGameBoss(long nTeam) {
        CGame::SetDefaultParam();
        for (long i = 0; i < 8; i++) {
            CGame::SetCharType(i, -1);
            CGame::SetCharColor(i, 0);
            CGame::SetCharExFlag(i, 0);
        }
        CGame::SetCup(static_cast<CGame::GAME_CUP>(CTournament::GetCup()));
        CGame::EnableRuleOptionFlag(0x100, 0);
        CGame::EnableRuleOptionFlag(0x200, 0);
        CGame::SetGameMode(CGame::GAME_MODE_BOSS);
        CGame::SetGameType(CGame::GAME_TYPE_NORMAL);
        CGame::SetStage(0x14);
        CGame::SetRound(6);
        CGame::SetTournamentLevel(static_cast<CGame::GAME_TOURNAMENT_LEVEL>(3));
        for (long i = 0; i < 4; i++) {
            CGame::SetTeamComLevel(i, 7.0f);
            CGame::SetTeamCtrlType(i, 0);
            CGame::SetGoalIdx(i, i & 1);
        }
        CGame::SetPlayType(static_cast<CGame::GAME_PLAY_TYPE>(
            CTournament::GetPlayerType(CGame::GAME_MODE_BOSS, TournamentWork::GetTournamentInfo().nPlayerMax)));
        if (CTournament::IsMixTournamentGame()) {
            CGame::SetSportsMode(CGame::SPORTS_MODE_MIX);
            CGame::SetPartyGameParam(0, 2);
        } else {
            CGame::SetSportsMode(CGame::SPORTS_MODE_SINGLE);
            CGame::SetPartyGameParam(0, CTournament::GetLevel());
        }
        _SetupTeamChar(nTeam, 0, 2);
        CGame::SetStageParam(bcs::BCS_Rand());
        CGame::SetParamSemiAuto();
        CGame::SetReturnStep(1);
        return 9;
    }
}
