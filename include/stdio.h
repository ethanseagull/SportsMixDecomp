#ifndef STDIO_H_
#define STDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Only what is used so far.
int sprintf(char *s, const char *format, ...);
typedef unsigned long size_t;
int snprintf(char *s, size_t n, const char *format, ...);
int sscanf(const char *s, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* !STDIO_H_ */
