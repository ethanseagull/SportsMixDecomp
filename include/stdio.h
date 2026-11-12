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

// The library's FILE is 0x50 bytes (__files in the DOL holds four:
// stdin, stdout, stderr and one spare); its fields are not used here.
typedef struct _FILE {
    unsigned char m_Data[0x50];
} FILE;

extern FILE __files[];
#define stdin (&__files[0])
#define stdout (&__files[1])
#define stderr (&__files[2])

int fprintf(FILE *stream, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* !STDIO_H_ */
