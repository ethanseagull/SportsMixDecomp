#ifndef STRING_H_
#define STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
typedef unsigned long size_t;

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *pDst, int c, size_t n);
int memcmp(const void *p0, const void *p1, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* !STRING_H_ */
