#ifndef _WASM_LIBC_H
#define _WASM_LIBC_H

#include <stddef.h>

typedef long off_t;

/* stdio */
typedef struct _FILE FILE;
extern FILE *__stdinp, *__stdoutp, *__stderrp;
#define stdin __stdinp
#define stdout __stdoutp
#define stderr __stderrp

int printf(const char *, ...);
int fprintf(FILE *, const char *, ...);
int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int vsprintf(char *, const char *, __builtin_va_list);
int vsnprintf(char *, size_t, const char *, __builtin_va_list);
int sscanf(const char *, const char *, ...);
int fscanf(FILE *, const char *, ...);
int vfprintf(FILE *, const char *, __builtin_va_list);
void perror(const char *);
int putchar(int);
int puts(const char *);
int fputs(const char *, FILE *);
int fputc(int, FILE *);
int fgetc(FILE *);
char *fgets(char *, int, FILE *);
FILE *fopen(const char *, const char *);
FILE *freopen(const char *, const char *, FILE *);
int fclose(FILE *);
int fflush(FILE *);
int fseek(FILE *, long, int);
long ftell(FILE *);
void rewind(FILE *);
int feof(FILE *);
int ferror(FILE *);
void clearerr(FILE *);
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int remove(const char *);
int rename(const char *, const char *);
int fgetpos(FILE *, void *);
int fsetpos(FILE *, const void *);
int ungetc(int, FILE *);
void setbuf(FILE *, char *);
int setvbuf(FILE *, char *, int, size_t);

/* stdlib */
void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);
void exit(int);
void _Exit(int);
void abort(void);
int atexit(void (*)(void));
int abs(int);
long labs(long);
long long llabs(long long);
double atof(const char *);
int atoi(const char *);
long atol(const char *);
long long atoll(const char *);
long strtol(const char *, char **, int);
unsigned long strtoul(const char *, char **, int);
long long strtoll(const char *, char **, int);
unsigned long long strtoull(const char *, char **, int);
double strtod(const char *, char **);
float strtof(const char *, char **);
long double strtold(const char *, char **);
int system(const char *);
char *getenv(const char *);
int rand(void);
void srand(unsigned int);
int mkstemp(char *);
char *mktemp(char *);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
int mblen(const char *, size_t);
int mbtowc(wchar_t *, const char *, size_t);
int wctomb(char *, wchar_t);

/* string */
size_t strlen(const char *);
size_t strnlen(const char *, size_t);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
char *strchr(const char *, int);
char *strrchr(const char *, int);
char *strstr(const char *, const char *);
char *strpbrk(const char *, const char *);
char *strtok(char *, const char *);
char *strdup(const char *);
char *strndup(const char *, size_t);
size_t strspn(const char *, const char *);
size_t strcspn(const char *, const char *);
char *strerror(int);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t);
void *memchr(const void *, int, size_t);
void *memccpy(void *, const void *, int, size_t);

/* ctype */
int isalnum(int);
int isalpha(int);
int isascii(int);
int isblank(int);
int iscntrl(int);
int isdigit(int);
int isgraph(int);
int islower(int);
int isprint(int);
int ispunct(int);
int isspace(int);
int isupper(int);
int isxdigit(int);
int tolower(int);
int toupper(int);

/* math */
double acos(double);
double asin(double);
double atan(double);
double atan2(double, double);
double cos(double);
double cosh(double);
double sin(double);
double sinh(double);
double tan(double);
double tanh(double);
double exp(double);
double exp2(double);
double expm1(double);
double frexp(double, int *);
double ldexp(double, int);
double log(double);
double log10(double);
double log1p(double);
double log2(double);
double logb(double);
double modf(double, double *);
double scalbn(double, int);
double cbrt(double);
double fabs(double);
double hypot(double, double);
double pow(double, double);
double sqrt(double);
double erf(double);
double erfc(double);
double lgamma(double);
double tgamma(double);
double ceil(double);
double floor(double);
double nearbyint(double);
double rint(double);
double round(double);
double trunc(double);
double fmod(double, double);
double remainder(double, double);
double copysign(double, double);
double nan(const char *);
double fdim(double, double);
double fmax(double, double);
double fmin(double, double);
double fma(double, double, double);
double nextafter(double, double);
double nexttoward(double, long double);
int ilogb(double);
float fabsf(float);
float sqrtf(float);

/* time */
typedef long clock_t;
typedef long time_t;
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
clock_t clock(void);
time_t time(time_t *);
double difftime(time_t, time_t);
time_t mktime(struct tm *);
char *asctime(const struct tm *);
char *ctime(const time_t *);
struct tm *gmtime(const time_t *);
struct tm *localtime(const time_t *);
size_t strftime(char *, size_t, const char *, const struct tm *);

/* setjmp */
typedef struct { unsigned int __0[10]; } jmp_buf[1];
typedef struct { unsigned int __0[10]; } sigjmp_buf[1];
int setjmp(jmp_buf);
void longjmp(jmp_buf, int);
int sigsetjmp(sigjmp_buf, int);
void siglongjmp(sigjmp_buf, int);

/* signal */
#define SIG_ERR ((void(*)(int))-1)
#define SIG_DFL ((void(*)(int))0)
#define SIG_IGN ((void(*)(int))1)
#define SIGINT  2
#define SIGILL  4
#define SIGABRT 6
#define SIGFPE  8
#define SIGSEGV 11
#define SIGTERM 15
typedef int sig_atomic_t;
void (*signal(int, void (*)(int)))(int);
int raise(int);

/* errno */
extern int errno;
#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EEXIST 17
#define EXDEV 18
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EPIPE 32
#define ERANGE 34
#define EDOM 33

/* limits.h */
#define CHAR_BIT 8
#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#define MB_LEN_MAX 4
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN (-2147483647 - 1)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295U
#define LONG_MIN (-2147483647L - 1)
#define LONG_MAX 2147483647L
#define ULONG_MAX 4294967295UL
#define LLONG_MIN (-9223372036854775807LL - 1)
#define LLONG_MAX 9223372036854775807LL
#define ULLONG_MAX 18446744073709551615ULL

/* stdint */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef intptr_t intmax_t;
typedef uintptr_t uintmax_t;
typedef int8_t int_least8_t;
typedef uint8_t uint_least8_t;
typedef int16_t int_least16_t;
typedef uint16_t uint_least16_t;
typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;
typedef int32_t int_fast8_t;
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;
typedef uint32_t uint_fast8_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;
#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN (-9223372036854775807LL - 1)
#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL
#define UINT8_MAX 255
#define UINT16_MAX 65535
#define UINT32_MAX 4294967295U
#define UINT64_MAX 18446744073709551615ULL
#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX

/* unistd */
extern char **environ;
int close(int);
int unlink(const char *);
int chdir(const char *);
int rmdir(const char *);
char *getcwd(char *, size_t);
int isatty(int);
ssize_t read(int, void *, size_t);
ssize_t write(int, const void *, size_t);
off_t lseek(int, off_t, int);
int access(const char *, int);
unsigned int sleep(unsigned int);
int usleep(unsigned int);
int getpid(void);
long sysconf(int);
int pipe(int[2]);
ssize_t readlink(const char *, char *, size_t);

/* fcntl */
int open(const char *, int, ...);
int creat(const char *, int);
int fcntl(int, int, ...);
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 0x40
#define O_EXCL 0x80
#define O_TRUNC 0x200
#define O_APPEND 0x400
#define O_NONBLOCK 0x800

/* sys/types */
typedef int pid_t;
typedef unsigned int mode_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;

/* pthread */
typedef unsigned long pthread_t;
typedef struct { unsigned int __0[9]; } pthread_mutex_t;
typedef struct { unsigned int __0[9]; } pthread_cond_t;
typedef struct { unsigned int __0[16]; } pthread_attr_t;
int pthread_create(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *);
int pthread_join(pthread_t, void **);
void pthread_exit(void *);
int pthread_mutex_init(pthread_mutex_t *, const void *);
int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_unlock(pthread_mutex_t *);
int pthread_mutex_destroy(pthread_mutex_t *);
int pthread_cond_init(pthread_cond_t *, const void *);
int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
int pthread_cond_signal(pthread_cond_t *);
int pthread_cond_broadcast(pthread_cond_t *);
int pthread_cond_destroy(pthread_cond_t *);
int pthread_equal(pthread_t, pthread_t);
pthread_t pthread_self(void);

/* semaphore */
typedef struct { unsigned int __0[9]; } sem_t;
int sem_init(sem_t *, int, unsigned int);
int sem_wait(sem_t *);
int sem_post(sem_t *);
int sem_destroy(sem_t *);
int sem_trywait(sem_t *);
int sem_timedwait(sem_t *, const void *);

/* sys/mman */
void *mmap(void *, size_t, int, int, int, long);
int munmap(void *, size_t);
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 0x20

/* sys/wait */
int wait(int *);
int waitpid(int, int *, int);

/* getopt */
extern char *optarg;
extern int optind, opterr, optopt;
int getopt(int, char *const[], const char *);

#endif /* _WASM_LIBC_H */