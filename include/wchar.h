#ifndef WCHAR_H_
#define WCHAR_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far. The C library is in the DOL.
typedef unsigned long size_t;

size_t wcslen(const wchar_t *pStr);
wchar_t *wmemset(wchar_t *pDst, wchar_t c, size_t n);
int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...);
int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);
// Added for MenuF game/menu/CBackground.cpp.
wchar_t *wcscpy(wchar_t *pDst, const wchar_t *pSrc);
wchar_t *wcsncpy(wchar_t *pDst, const wchar_t *pSrc, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* !WCHAR_H_ */
