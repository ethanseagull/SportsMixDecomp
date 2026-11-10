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

typedef float Mtx[3][4];

// dst = m * src (src as a point); src and dst may be the same.
void PSMTXMultVec(const Mtx m, const Vec *src, Vec *dst);

float PSVECSquareDistance(const Vec *pA, const Vec *pB);
void PSVECNormalize(const Vec *pIn, Vec *pOut);
float PSVECMag(const Vec *p);

typedef struct Quaternion {
    float x; // 0x00
    float y; // 0x04
    float z; // 0x08
    float w; // 0x0C
} Quaternion;

// pq = p * q.
void PSQUATMultiply(const Quaternion *p, const Quaternion *q, Quaternion *pq);
void PSQUATNormalize(const Quaternion *pSrc, Quaternion *pDst);

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_MTX_H_ */
