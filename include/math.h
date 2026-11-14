#ifndef MATH_H_
#define MATH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
double pow(double x, double y);
double ceil(double x);
double floor(double x);
double fmod(double x, double y);
double tan(double x);
// (added for DOL game/msw/CEventNodeBase.cpp; the DOL's copy is the unnamed
// fn_802AE8FC, a branch to the real routine)
double asin(double x);

#ifdef __cplusplus
}

// The float version (inline, as in the compiler's own library).
inline float fmodf(float x, float y)
{
    return (float)fmod(x, y);
}

// Inline, as in the compiler's own library: one fabs instruction.
inline double fabs(double x)
{
#ifdef __MWERKS__
    return __fabs(x);
#else
    return x < 0.0 ? -x : x;
#endif
}

// The float version. Inline, one fabs instruction without the rounding
// (frsp) the double version needs (DodgeF CChrManDodge::Util_SearchPassTo_).
inline float fabsf(float x)
{
#ifdef __MWERKS__
    return __fabsf(x);
#else
    return x < 0.0f ? -x : x;
#endif
}
#endif

#endif /* !MATH_H_ */
