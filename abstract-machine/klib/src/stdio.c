#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define OUTBUF_SIZE 65536
#define VBUF_SIZE   64
char outbuf[OUTBUF_SIZE];  // out buf,for stdout
char vbuf[VBUF_SIZE];      // num buf,store %d arg
char *argbuf;              // arg buf

/**
 * @brief
 *  print padding char
 * @param out buf
 * @param n
 * @param ret length of out
 * @param pad char for padding
 * @param num num of padding char
 */
void add_pad(char **out, size_t *n, int *ret, char pad, int num) {
  while (num--) {
    if (*n > 1) {
      *(*out)++ = pad;
      (*n)--;
    }
    (*ret)++;
  }
}

/**
 * @brief
 * print decimal into buffer,return length
 * @param buf
 * @param maxlen
 * @param value
 * @param format %d %i %p %x
 * @return int
 */
int print_num(char *buf, int value, char format) {
  const char *h = "0123456789abcdef";
  int maxlen = 0, values = value;
  int len = 0;
  int t;
  int atmo;
  if (format == 'x' || format == 'p')
    t = 16;
  else
    t = 10;
  for (; values; len++, maxlen++, values /= t)
    ;
  if (value == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    maxlen = 1;
  } else {
    buf[len] = '\0';
    while (value) {
      atmo       = value % t;
      atmo       = atmo > 0 ? atmo : -atmo;
      buf[--len] = h[atmo];
      value /= t;
    }
  }
  return maxlen;
}

int printf(const char *fmt, ...) {
  int ret = 0;
  va_list list;
  va_start(list, fmt);
  ret = vsprintf(outbuf, fmt, list);
  va_end(list);

  putstr(outbuf);
  return ret;
}

/**
 * @brief write bytes
       (including the terminating null byte ('\0')) to out.
 *
 * @param out output stream
 * @param fmt input format
 * @param ap variable list
 * @return int nums of characters
 */
int vsprintf(char *out, const char *fmt, va_list ap) {
  int ret = vsnprintf(out, INT32_MAX, fmt, ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  int ret = 0;
  va_list list;
  va_start(list, fmt);
  ret = vsnprintf(out, INT32_MAX, fmt, list);
  va_end(list);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  int ret = 0;
  va_list list;
  va_start(list, fmt);
  ret = vsnprintf(out, n, fmt, list);
  va_end(list);
  return ret;
}

/**
 * @brief write at most n bytes
       (including the terminating null byte ('\0')) to out.
 *
 * @param out output stream
 * @param n max output size
 * @param fmt input format
 * @param ap variable list
 * @return int nums of chars(except '\0')
 */
int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int ret          = 0;    // return length
  char *pout       = out;  // pointer
  const char *pfmt = fmt;  // pointer
  int spec;                // specifier
  int minw      = 0;       // width
  char pad      = ' ';     // padding char
  int space     = 0;       // space
  int precision = 0;       // precision,default length is 6
  int value;
  int negative = 0;

  while (*pfmt) {
    /*copy string until '%'*/
    while (*pfmt && n > 1 && *pfmt != '%') {
      *pout++ = *pfmt++;
      n--;
      ret++;
    }
    /*see if we are at end*/
    if (!*pfmt) break;
    /*fetch the next argument % designation from format string*/
    pfmt++;  // skip the '%'

    /*
    FORMAT:
      %[flags][width][.precision][length]specifier

    flags:-,+,space,0,#
    width
    .precision
    length:h,hh,l,ll,L
    specifier:c,d,i,e/E,f,g/G,o,s,u,x/X,p,n,%
    */
    spec      = 0;    // specifier
    minw      = 0;    // width
    pad       = ' ';  // padding char
    space     = 0;    // space
    precision = 0;    // precision,default length is 6
    /*flags*/
    while (1) {
      if (*pfmt == '-') continue;
      // minus = 1;
      else if (*pfmt == '+')
        continue;
      // plus = 1;
      else if (*pfmt == ' ')
        space = 1;
      else if (*pfmt == '0') {
        // zeropad = 1;
        pad = '0';
      } else if (*pfmt == '#')
        /*to do*/
        continue;
      else
        break;
      pfmt++;
    }

    /*width*/
    if (*pfmt == '*') {  // this means next args in va_list is minw
      pfmt++;            // skip char '*'
      minw = va_arg(ap, int);
      if (minw < 0) {
        // minus = 1;
        minw = -minw;
      }
    } else {
      while (*pfmt >= '0' && *pfmt <= '9')
        minw = minw * 10 + ((*pfmt++) - '0');
    }

    /*precision*/
    if (*pfmt == '.') {
      pfmt++; /* skip period */
      // pr_flag   = 1;
      precision = 0;
      if (*pfmt == '*') {
        pfmt++; /* skip char */
        precision = va_arg(ap, int);
        if (precision < 0) precision = 0;
      } else {
        while (*pfmt >= '0' && *pfmt <= '9') {
          precision = precision * 10 + (*pfmt++) - '0';
        }
      }
    }

    /*length*/
    do {
      if (*pfmt == 'l') {
        pfmt++;
        // length = 1;
        if (*pfmt == 'l') {
          pfmt++;
          // length = 2;
        }
      } else if (*pfmt == 'h') {
        pfmt++;
        // length = -1;
        if (*pfmt == 'h') {
          pfmt++;
          // length = -2;
        }
      } else if (*pfmt == 'L') {
        pfmt++;
        // lengthL = 1;
      }
    } while (0);

    /*get the specifier*/
    if (!*pfmt)
      spec = 0;
    else
      spec = *pfmt++;

    /**
     * @brief print the argument designation
     */
    int len = 1;
    if (n > 1) {
      switch (spec) {
        case 'c':
          *argbuf = va_arg(ap, int);
          len     = 1;
          break;
        case 's':
          argbuf = va_arg(ap, char *);
          // strcpy(outbuf, buf);
          len = strlen(argbuf);
          break;
        case 'p':
          precision = 8;
        case 'd':
        case 'x':
        case 'i':
          value    = va_arg(ap, int);
          negative = value < 0;
          len      = print_num(vbuf, value, spec);
          argbuf   = vbuf;
          break;
        default:
          break;
      }
      /*can hold the length of buf*/
      if (n > len) {
        /*need padding*/
        if (space) {  // space padding
          add_pad(&pout, &n, &ret, ' ', 1);
        }
        if (negative) {
          add_pad(&pout, &n, &ret, '-', 1);
        }
        if (precision > len) {  // width padding
          if (minw - space - precision > 0)
            add_pad(&pout, &n, &ret, pad, minw - space - precision);
          add_pad(&pout, &n, &ret, '0', precision - len);  // precision padding
        } else if (minw - space - len > 0) {
          add_pad(&pout, &n, &ret, pad, minw - space - len);
        }
        if (n > 1) {
          len = len > n - 1 ? n - 1 : len;
          ret += len;
          strncpy(pout, argbuf, len);
          pout += len;
          n -= len;
        }
      } else {
        if (negative) {
          add_pad(&pout, &n, &ret, '-', 1);
        }
        ret += n - 1;
        len = n - 1;
        strncpy(pout, argbuf, len);
        pout += len;
        n = 1;
      }
    }
  }
  *pout = '\0';
  return ret;
}
#endif
