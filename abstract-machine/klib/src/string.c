#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

/**
 * @brief Return the length of the string s.
 * @param s
 * @return size_t
 */
size_t strlen(const char *s) {
  size_t amount = 0;
    while (s[amount] != '\0')
    {
        amount++;
    }
    return amount;
}

/**
 * @brief Copy the string src to dst, returning a pointer to the start of dst.
 * @param dst
 * @param src
 * @return char*
 */
char *strcpy(char *dst, const char *src) {
  size_t i = 0;
    for (i = 0; src[i] != '\0'; i++)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return dst;
}

/**
 * @brief Copy at most n bytes from string src to dst, returning a pointer to the start of dst.
 * @param dst
 * @param src
 * @param n
 * @return char*
 */
char *strncpy(char *dst, const char *src, size_t n) {
  size_t i = 0;
    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
    return dst;
}

/**
 * @brief Append the string src to the string dst, returning a pointer dst.
 * @param dst
 * @param src
 * @return char*
 */
char *strcat(char *dst, const char *src) {
  size_t d_len = strlen(dst);
    size_t i = 0;
    for (i = 0; src[i] != '\0'; i++)
    {
        dst[d_len + i] = src[i];
    }
    dst[d_len + i] = '\0';
    return dst;
}

/**
 * @brief Compare the strings s1 with s2.
 * @param s1
 * @param s2
 * @return int
 */
int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
    for (i = 0; s1[i] != '\0' && s2[i] != '\0'; i++)
    {
        if (s1[i] != s2[i]) 
        {
            return s1[i] < s2[i] ? -1 : 1; 
            // if (s1[i] < s2[i])
            //     return -1;
            // else
            //     return 1;
        }
    }
    return s1[i] == s2[i] ? 0 : (s1[i] < s2[i] ? -1 : 1); 
}

/**
 * @brief Compare at most n bytes of the strings s1 and s2.
 * @param s1
 * @param s2
 * @param n
 * @return int
 */
int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;
    for (i = 0; i < n && s1[i] != '\0' && s2[i] != '\0'; i++)
    {
        if (s1[i] != s2[i])
        {
            if (s1[i] < s2[i])
                return -1;
            else
                return 1;
        }
    }
    if (i == n)
        return 0;
    else
    {
        if (s1[i] < s2[i])
            return -1;
        else
            return 1;
    }
}

/**
 * @brief Make n bits are the character c
 * @param s
 * @param c
 * @param n
 * @return void*
 */
void *memset(void *s, int c, size_t n) {
  if (s == NULL || n < 0) //wrong input
    {
        return NULL;
    }
    char *s1 = (char *)s;
    while (n-- > 0)
    {
        *s1++ = c;
        return s;
    }
    return NULL;
}

/**
 * @brief Copy at most n bytes from unknown data type src to dst, returning a pointer to the start of dst.
 * @param dst
 * @param src
 * @param n
 * @return void*
 */
void *memmove(void *dst, const void *src, size_t n) {
  void *ret = dst;
    if (dst <= src || (char *)dst >= ((char *)src + n)) //not overlap
    {
        while (n--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
    }
    else    //overlap
    {
        dst = (char *)dst + n - 1;
        src = (char *)src + n - 1;
        while (n--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst - 1;
            src = (char *)src - 1;
        }
    }
    return ret;
}

/**
 * @brief
 * Copy at most n bytes from unknown data type src to dst, returning a pointer to the start of dst.(The same as memmove without overlap)
 * @param out
 * @param in
 * @param n
 * @return void*
 */
void *memcpy(void *out, const void *in, size_t n) {
  void *dst;
    const void *src;
    if (out <= in || (char *)out >= ((char *)in + n))
    {
        dst = out;
        src = in;
        while (n--)
        {
            *(char *)dst = *(char *)src;
            dst = (char *)dst + 1;
            src = (char *)src + 1;
        }
    }
    return out;
}

/**
 * @brief Compare at most n bytes of the unknown data type s1 and s2.
 * @param s1
 * @param s2
 * @param n
 * @return int
 */
int memcmp(const void *s1, const void *s2, size_t n) {
  if (s1 != NULL && s2 != NULL && n > 0)
    {
        const char *dst = (char *)s1;
        const char *src = (char *)s2;
        while (*dst == *src && --n > 0)
        {
            dst++;
            src++;
        }
        return *dst - *src > 0 ? 1 : (*dst - *src < 0 ? -1 : 0);
    }
    else
        return -2;
}

#endif
