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

// Inline, as in the compiler's own library: srawi/xor/subf, no branch
// (VolleyF CStageVolley11::SendMessage).
inline int abs(int n)
{
#ifdef __MWERKS__
    return __abs(n);
#else
    return n < 0 ? -n : n;
#endif
}

inline long abs(long n)
{
#ifdef __MWERKS__
    return __labs(n);
#else
    return n < 0 ? -n : n;
#endif
}

inline long labs(long n)
{
    return abs(n);
}
#endif

#endif /* !STDLIB_H_ */
