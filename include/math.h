#ifndef MATH_H_
#define MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
double pow(double x, double y);
double fmod(double x, double y);

#ifdef __cplusplus
}

// The float version (inline, as in the compiler's own library).
inline float fmodf(float x, float y)
{
    return (float)fmod(x, y);
}
#endif

#endif /* !MATH_H_ */
