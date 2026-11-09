#ifndef REVOLUTION_MTX_H_
#define REVOLUTION_MTX_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
typedef struct Vec {
    float x; // 0x00
    float y; // 0x04
    float z; // 0x08
} Vec;

float PSVECSquareDistance(const Vec *pA, const Vec *pB);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_MTX_H_ */
