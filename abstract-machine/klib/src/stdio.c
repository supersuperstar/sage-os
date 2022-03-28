#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static char outbuf[65536];  // out buf
static char pbuf[32];       // store num

/**
 * @brief
 *  print padding char
 * @param out buf
 * @param n
 * @param ret length of out
 * @param pad char for padding
 * @param num num of padding char
 */
static void add_pad(char **out, size_t *n, int *ret, char pad, int num) {
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
 * print long long decimal into buffer,return length
 * @param buf
 * @param maxlen
 * @param value
 * @param format %d or %x
 * @return int
 */
static int print_dix(char *buf, int value, char format) {
  const char *h = "0123456789abcdef";
  int maxlen = 0, values = value;
  int len = 0;
  int i;
  char tmp;
  int t;
  if (format == 'x' || format == 'p')
    t = 16;
  else
    t = 10;
  for (; values; len++, maxlen++, values /= t)
    ;
  buf[len] = '\0';
  if (value == 0) {
    buf[0] = '0';
    maxlen = 1;
  } else {
    while (value) {
      buf[--len] = h[value & 0xF];
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

  for (char *c = outbuf; *c; c++) {
    _putc(*c);
  }
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
 * @return int nums of characters
 */
int myvsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int ret          = 0;    // return length
  char *pout       = out;  // pointer
  const char *pfmt = fmt;  // pointer
  int spec;                // specifier
  int minw      = 0;       // width
  int zeropad   = 0;       // pad
  char pad      = ' ';     // padding char
  int minus     = 0;       //-
  int plus      = 0;       //+
  int space     = 0;       // space
  int length    = 0;       // h,hh,l,ll
  int precision = 0;       // precision,default length is 6
  int pr_flag   = 0;       // precision use given value or default
  int lengthL   = 0;       // L
  int pad1, pad2, pad3;    // precision pad,width pad,space pad
  int value;
  char *buf;
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
    zeropad   = 0;    // pad
    pad       = ' ';  // padding char
    minus     = 0;    //-
    plus      = 0;    //+
    space     = 0;    // space
    length    = 0;    // h,hh,l,ll
    precision = 0;    // precision,default length is 6
    pr_flag   = 0;    // precision use given value or default
    lengthL   = 0;    // L
    /*flags*/
    while (1) {
      if (*pfmt == '-')
        minus = 1;
      else if (*pfmt == '+')
        plus = 1;
      else if (*pfmt == ' ')
        space = 1;
      else if (*pfmt == '0') {
        zeropad = 1;
        pad     = '0';
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
        minus = 1;
        minw  = -minw;
      }
    } else {
      while (*pfmt >= '0' && *pfmt <= '9')
        minw = minw * 10 + ((*pfmt++) - '0');
    }

    /*precision*/
    if (*pfmt == '.') {
      pfmt++; /* skip period */
      pr_flag   = 1;
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
        length = 1;
        if (*pfmt == 'l') {
          pfmt++;
          length = 2;
        }
      } else if (*pfmt == 'h') {
        pfmt++;
        length = -1;
        if (*pfmt == 'h') {
          pfmt++;
          length = -2;
        }
      } else if (*pfmt == 'L') {
        pfmt++;
        lengthL = 1;
      }
    } while (0);

    /*get the specifier*/
    if (!*pfmt)
      spec = 0;
    else
      spec = *pfmt++;

    /**
     * @brief print the argumentdesignation
     */
    int len = 1;
    if (n > 1) {
      switch (spec) {
        case 'c':
          outbuf[0] = va_arg(ap, int);
          len       = 1;
          break;
        case 's':
          buf = va_arg(ap, char *);
          strcpy(outbuf, buf);
          len = strlen(outbuf);
          break;
        case 'p':
          precision = 8;
        case 'd':
        case 'x':
        case 'i':
          value = va_arg(ap, int);
          len   = print_dix(outbuf, value, spec);
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
          strncpy(pout, outbuf, len);
          pout += len;
        }
      } else {
        ret += n - 1;
        len = n - 1;
        strncpy(pout, outbuf, len);
        pout += len;
      }
    }
  }
  return ret;
}

#endif
