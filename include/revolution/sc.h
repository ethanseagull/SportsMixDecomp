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

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_SC_H_ */
