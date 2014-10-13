#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "messageBuilder.h"

static const char *_messageBuilder_internalErrors = "Internal error";

#ifdef VA_COPY
#define REAL_AP ap2
#else
#define REAL_AP ap
#endif

/*********************************/
/* messageBuilder_internalErrors */
/*********************************/
char *messageBuilder_internalErrors(void) {
  return (char *) _messageBuilder_internalErrors;
}

/******************/
/* messageBuilder */
/******************/
char *messageBuilder(const char *fmts, ...) {
  va_list ap;
#ifdef VA_COPY
  va_list ap2;
#endif
  char   *msgs;

  va_start(ap, fmts);
#ifdef VA_COPY
  VA_COPY(ap2, ap);
#endif
  msgs = messageBuilder_ap(fmts,  REAL_AP);
#ifdef VA_COPY
  va_end(ap2);
#endif
  va_end(ap);

  return msgs;
}

/*********************/
/* messageBuilder_ap */
/*********************/
char *messageBuilder_ap(const char *fmts, va_list ap) {
  int     n;
  size_t  size = 4096;     /* Guess we need no more than 4096 bytes */
  char   *p, *np;
#ifdef VA_COPY
  va_list ap2;
#endif

  /* ----------------------------------------------------------------------------------------------------------------------- */
  /* Take care: Windows's vsnprintf is not like UNIX's, i.e:                                                                 */
  /*                                                                                                                         */
  /* Output:                                                                                                                 */
  /* [Windows] -1 if the number of characters if > count. Minus trailing null character                                      */
  /* [ UNIX  ] number of chars that would have been writen. Minus trailing null character                                    */
  /*                                                                                                                         */
  /* Argument:                                                                                                               */
  /* [Windows] number of characters wanted, does not include the trailing null character                                     */
  /* [ UNIX  ] number of characters wanted + the trailing null character                                                     */
  /* ----------------------------------------------------------------------------------------------------------------------- */

  p = malloc(size);
  if (p == NULL) {
    return messageBuilder_internalErrors();
  }

  while (1) {

    /* Try to print in the allocated space */

#ifdef VA_COPY
    VA_COPY(ap2, ap);
#endif
#ifdef _WIN32
    n = _vsnprintf(p, size, fmts, REAL_AP);   /* On Windows, argument does not include space for the NULL */
#else
    n = vsnprintf(p, size, fmts, REAL_AP);        /* On not-windows, argument include space for the NULL */
#endif
#ifdef VA_COPY
    va_end(ap2);
#endif

    /* Check error code */
#ifndef _WIN32
    /* On not-windows, if output is negative an output error is encountered */
    if (n < 0) {
      free(p);
      return messageBuilder_internalErrors();
    }
#endif

    /* If that worked, return the string, unless not enough space - in which case we retry -; */

    if
#ifdef _WIN32
      (n >= 0 && n < (int) size)
#else
      (n < size)
#endif
        {
      return p;
    }

    /* Else try again with more space */

#ifdef _WIN32
    size *= 2;          /* Maybe enough ? */
#else
    size = n + 1;       /* Precisely what is needed */
#endif

    np = realloc(p, size);
    if (np == NULL) {
      free(p);
      return messageBuilder_internalErrors();
    } else {
      p = np;
    }
  }

  /* Should never happen */
  return messageBuilder_internalErrors();
}

