#ifndef CNAMEMESG_HPP_
#define CNAMEMESG_HPP_

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
    };
}

#endif /* !CNAMEMESG_HPP_ */
