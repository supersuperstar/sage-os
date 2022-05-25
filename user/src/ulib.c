
#include <ulib.h>
#include <utils.h>

char *strcpy(char *s, char *t) {
  char *os;

  os = s;
  while ((*s++ = *t++) != 0)
    ;
  return os;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i = 0;
  for (i = 0; i < n && src[i] != '\0'; ++i) {
    dst[i] = src[i];
  }
  dst[i] = '\0';
  return dst;
}

int strcmp(const char *p, const char *q) {
  while (*p && *p == *q)
    p++, q++;
  return (uint8_t)*p - (uint8_t)*q;
}

static inline void stosb(void *addr, int data, int cnt) {
  asm volatile("cld; rep stosb"
               : "=D"(addr), "=c"(cnt)
               : "0"(addr), "1"(cnt), "a"(data)
               : "memory", "cc");
}

void *memset(void *dst, int c, size_t n) {
  stosb(dst, c, n);
  return dst;
}

char *strchr(const char *s, char c) {
  for (; *s; s++)
    if (*s == c) return (char *)s;
  return 0;
}

int atoi(const char *s) {
  int n;

  n = 0;
  while ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  return n;
}

void *memmove(void *vdst, void *vsrc, int n) {
  char *dst, *src;

  dst = vdst;
  src = vsrc;
  while (n-- > 0)
    *dst++ = *src++;
  return vdst;
}
