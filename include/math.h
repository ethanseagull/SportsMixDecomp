#ifndef MATH_H_
#define MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
double pow(double x, double y);
double ceil(double x);
double fmod(double x, double y);
double tan(double x);

#ifdef __cplusplus
}

// The float version (inline, as in the compiler's own library).
inline float fmodf(float x, float y)
{
    return (float)fmod(x, y);
}

// Single-precision absolute value (the compiler's builtin; fabs on a
// double would add a conversion).
#ifdef __MWERKS__
inline float fabsf(float x)
{
    return __fabsf(x);
}
#endif
// Inline, as in the compiler's own library: one fabs instruction.
inline double fabs(double x)
{
#ifdef __MWERKS__
    return __fabs(x);
#else
    return x < 0.0 ? -x : x;
#endif
}
#endif

#endif /* !MATH_H_ */
