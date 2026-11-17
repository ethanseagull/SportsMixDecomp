#ifndef REVOLUTION_SC_H_
#define REVOLUTION_SC_H_

#ifdef __cplusplus
extern "C" {
#endif

// The RVL SDK's system configuration functions; only what the game uses.
// Declarations written for this project from the calls in the DOL.

// (fn_802C2680, unnamed in the DOL; it sits with SCGetDisplayOffsetH and
// SCGetLanguage) The console's aspect ratio setting, 1 for 16:9.
unsigned char SCGetAspectRatio(void);
unsigned char SCGetLanguage(void);

// (added for the DOL's game/bcs/CMainApp.cpp) The values SCGetLanguage
// returns, the progressive scan setting (1 on) and whether parental
// controls forbid messaging (non-zero).
enum {
    SC_LANG_JAPANESE,
    SC_LANG_ENGLISH,
    SC_LANG_GERMAN,
    SC_LANG_FRENCH,
    SC_LANG_SPANISH,
    SC_LANG_ITALIAN,
    SC_LANG_DUTCH,
    SC_LANG_SIMP_CHINESE,
    SC_LANG_TRAD_CHINESE,
    SC_LANG_KOREAN
};

unsigned char SCGetProgressiveMode(void);
int SCCheckPCMessageRestriction(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_SC_H_ */
