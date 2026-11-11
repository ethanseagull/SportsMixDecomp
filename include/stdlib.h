#ifndef STDLIB_H_
#define STDLIB_H_

// The parts of the C library's stdlib.h the game's code uses so far
// (declarations only; the library is in the DOL).

#ifdef __cplusplus
extern "C" {
#endif

double atof(const char *s);
int atoi(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* !STDLIB_H_ */
