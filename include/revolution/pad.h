#ifndef REVOLUTION_PAD_H_
#define REVOLUTION_PAD_H_

// Only what the game's code uses so far (added for DOL game/bcs/CMovie.cpp).

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*PADSamplingCallback)(void);

// Sets the function called after each controller sampling; returns the
// previous one.
PADSamplingCallback PADSetSamplingCallback(PADSamplingCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_PAD_H_ */
