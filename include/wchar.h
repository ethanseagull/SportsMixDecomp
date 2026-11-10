#ifndef WCHAR_H_
#define WCHAR_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far. The C library is in the DOL.
typedef unsigned long size_t;

size_t wcslen(const wchar_t *pStr);
wchar_t *wmemset(wchar_t *pDst, wchar_t c, size_t n);
int swprintf(wchar_t *pDst, size_t n, const wchar_t *pFormat, ...);

#ifdef __cplusplus
}
#endif

#endif /* !WCHAR_H_ */
