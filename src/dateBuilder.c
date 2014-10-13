#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "config.h"
#include "dateBuilder.h"

static const char *_dateBuilder_internalErrors = "Internal error";

#define DATEBUILDER_MAX_SIZE sizeof(char) * (1024+1)

/*********************************/
/* dateBuilder_internalErrors */
/*********************************/
char *dateBuilder_internalErrors(void) {
  return (char *) _dateBuilder_internalErrors;
}

/******************/
/* dateBuilder */
/******************/
char *dateBuilder(const char *fmts) {
  char      *dates;
  time_t     t;
#ifdef HAVE_LOCALTIME_R
  struct tm  tmpLocal;
#endif
  struct tm *tmp;

  /* We assume that a date should never exceed 1024 bytes isn't it */
  dates = malloc(DATEBUILDER_MAX_SIZE);
  if (dates == NULL) {
    dates = (char *) _dateBuilder_internalErrors;
  } else {
    t = time(NULL);
#ifdef HAVE_LOCALTIME_R
    tmp = localtime_r(&t, &tmpLocal);
#else
    tmp = localtime(&t);
#endif
    if (tmp == NULL) {
      dates = (char *) _dateBuilder_internalErrors;
    } else {
      if (strftime(dates, DATEBUILDER_MAX_SIZE, fmts, tmp) == 0) {
	dates = (char *) _dateBuilder_internalErrors;
      }
    }
  }

  return dates;
}
