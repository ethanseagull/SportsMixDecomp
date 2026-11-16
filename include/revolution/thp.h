#ifndef REVOLUTION_THP_H_
#define REVOLUTION_THP_H_

// Only what the game's code uses so far (added for DOL game/bcs/CMovie.cpp).
// The THP decoder library; bcs::CMovie keeps its own copies of the file's
// descriptions (include/bcs/CMovie.hpp).

#ifdef __cplusplus
extern "C" {
#endif

// Sets up the decoder's locked cache work areas and quantisation registers.
int THPInit(void);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_THP_H_ */
