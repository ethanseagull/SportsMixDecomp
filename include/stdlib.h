#ifndef STDLIB_H_
#define STDLIB_H_

// Only what is used so far.

#ifdef __cplusplus
extern "C" {
#endif

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
