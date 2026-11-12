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
// Added for MenuF game/menu/CBgDisplay.cpp.
typedef float Mtx44[4][4];

// m = an orthographic projection of the box (t, b, l, r, n, f).
void C_MTXOrtho(Mtx44 m, float t, float b, float l, float r, float n, float f);

void PSMTXCopy(const Mtx src, Mtx dst);
void PSMTXIdentity(Mtx m);
// ab = a * b.
void PSMTXConcat(const Mtx a, const Mtx b, Mtx ab);
// inv = the inverse of src; 0 when src has none.
unsigned long PSMTXInverse(const Mtx src, Mtx inv);
// dst = src with (x, y, z) added to the translation.
void PSMTXTransApply(const Mtx src, Mtx dst, float x, float y, float z);

// dst = m * src (src as a point); src and dst may be the same.
void PSMTXMultVec(const Mtx m, const Vec *src, Vec *dst);
// dst[i] = m * src[i] for count points; src and dst may be the same.
void PSMTXMultVecArray(const Mtx m, const Vec *src, Vec *dst, unsigned long count);

float PSVECSquareDistance(const Vec *pA, const Vec *pB);
void PSVECNormalize(const Vec *pIn, Vec *pOut);
float PSVECMag(const Vec *p);
void PSVECCrossProduct(const Vec *pA, const Vec *pB, Vec *pAxB);

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
