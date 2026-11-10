#ifndef STRING_H_
#define STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
typedef unsigned long size_t;

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
void *memset(void *pDst, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* !STRING_H_ */
