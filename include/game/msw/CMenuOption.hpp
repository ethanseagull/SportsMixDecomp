#ifndef CMENUOPTION_HPP_
#define CMENUOPTION_HPP_

namespace msw {

    // Lives in the DOL. The menu options (rules, levels) and the texts of
    // their settings. Only what MenuF game/menu/CCharaLevelStar.cpp needs;
    // s_Instance is 0x144 bytes. Argument names and return types are
    // guesses.
    class CMenuOption {
        public:
            // The number of settings of option nKind, the selected one, and
            // its text. nSub is 0 at every call seen so far.
            long GetMenuParameterNum(long nKind, long nSub);
            long GetMenuParameterId(long nKind, long nSub);
            void SetMenuParameterId(long nKind, long nSub, long nId);
            const wchar_t *GetMenuParameterText(long nKind, long nSub, long nId);
            // (added for MenuF game/menu/CTournamentLoad.cpp; names and
            // return types guessed) Loads the options of player nPlayer
            // at difficulty nLevel, and sets up the game from option kind
            // nKind.
            void GetPlayerSaveData(long nPlayer, long nLevel);
            void SetGameParameter(long nKind);
            // (added for MenuF game/menu/CRuleSel.cpp; names and return
            // types guessed) The number of options of kind nKind, the name
            // of option nOption and the help line of its setting nId; the
            // default setting of an option at difficulty nLevel (static:
            // the game passes no instance).
            long GetMenuNum(long nKind);
            const wchar_t *GetMenuName(long nKind, long nOption);
            const wchar_t *GetMenuParameterTelop(long nKind, long nOption, long nId);
            static long GetMenuDefaultId(long nKind, long nOption, long nLevel);
            // Stores the options of player nPlayer at difficulty nLevel
            // (nType: 0 exhibition, 1 tournament, 2 boss tournament).
            void SetPlayerSaveData(long nPlayer, long nType, long nLevel);
            // A copy of the options to go back to when the rules screen is
            // left with B: made, filled, put back and freed.
            void AllocTemporary();
            void BackupTemporary();
            void RestoreTemporary();
            void FreeTemporary();

            static CMenuOption s_Instance;
    };
}

#endif /* !CMENUOPTION_HPP_ */
