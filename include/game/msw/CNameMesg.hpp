#ifndef CNAMEMESG_HPP_
#define CNAMEMESG_HPP_

#include "game/msw/CCockpitButtonInfo.hpp"

namespace msw {

    // Names and texts from the game's message data (DOL). Only the members
    // used so far; all are static. (return types and parameter names
    // guessed)
    class CNameMesg {
        public:
            // The name of character nType in colour nColor.
            static const wchar_t *GetCharName(long nType, long nColor, long);
            // The name of party stage nStage; nSide is -1 or 1.
            static const wchar_t *GetPartyStageName(long nSide, long nStage);
            // The mission text of sport nGameMode for nUnk and rule nRule,
            // with the suffix pSuffix ("_C", "_H", "_L").
            static const wchar_t *GetMissionStrings(long nGameMode, long nUnk, long nRule, const char *pSuffix);
            // The text of unlock nUnlock with the suffix pSuffix ("_S").
            static const wchar_t *GetMissionUnlockStrings(long nUnlock, const char *pSuffix);
            // Replaces every pFrom in pStr (nLen characters) by pTo.
            static void StrReplace(wchar_t *pStr, long nLen, const wchar_t *pFrom, const wchar_t *pTo);

            // (added for TourF game/tour/CTournamentStep.cpp; all static,
            // return types and parameter names guessed) The text of label
            // pLabel in message file pFile (rGroup and rLabel receive their
            // numbers); the names of a sport and of a match round; and the
            // one window of messages this class keeps for the map: opened
            // with one or two buttons, closed, removed.
            static const wchar_t *_GetMessageText(long &rGroup, long &rLabel, const char *pFile, const char *pLabel);
            static const wchar_t *GetGameModeName(long nGameMode);
            static const wchar_t *GetGameRoundName(long nRound, long &rLen);
            static const wchar_t *GetGameRoundRubiName(long nRound, long &rLen);
            static void Initialize();
            static void Remove();
            static void CreateButtonWindow(const char *pFile, const char *pLabel, long nType);
            static void CreateButtonWindow(const char *pFile);
            static void CreateButtonWindow2(const CCockpitButtonInfo::ButtonInfo *pInfo, const char *pFile,
                                            const char *pLabel);
            static void CreateGetTrophy();
            static void MenuOutWindow();
            static int IsMenuOutWindow();
            static void RemoveWindow();
    };
}

#endif /* !CNAMEMESG_HPP_ */
