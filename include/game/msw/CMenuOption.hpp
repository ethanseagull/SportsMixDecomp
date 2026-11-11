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

            static CMenuOption s_Instance;
    };
}

#endif /* !CMENUOPTION_HPP_ */
