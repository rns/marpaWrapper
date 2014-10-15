#include "config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>               /*  size_t definition */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif

#include "messageBuilder.h"
#include "dateBuilder.h"
#include "genericLogger.h"

static const char *GENERICLOGGER_LOG_NO_MESSAGE = "No message";

struct genericLogger {
  genericLoggerCallback_t  logCallbackp;
  void                  *userDatavp;
  genericLoggerLevel_t     genericLoggerLeveli;
};

void _genericLogger_defaultCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

/*****************************/
/* genericLogger_logLevel_seti */
/*****************************/
genericLoggerLevel_t genericLogger_logLevel_seti(genericLogger_t *genericLoggerp, genericLoggerLevel_t logLeveli) {
  genericLoggerp->genericLoggerLeveli = logLeveli;
  return genericLoggerp->genericLoggerLeveli;
}

/*****************************/
/* genericLogger_logLevel_geti */
/*****************************/
genericLoggerLevel_t genericLogger_logLevel_geti(genericLogger_t *genericLoggerp) {
  return genericLoggerp->genericLoggerLeveli;
}

/**********************************/
/* genericLogger_defaultLogCallback */
/**********************************/
genericLoggerCallback_t genericLogger_defaultLogCallback(void) {
  return &_genericLogger_defaultCallback;
}

/********************************/
/* _genericLogger_defaultCallback */
/********************************/
void _genericLogger_defaultCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs) {
  /* We are NOT going to do a general log4c mechanism (this can come later) */
  /* I.e. we are fixing the default output to be: DD/MM/YYYY hh::mm::ss PREFIX MESSAGE */
  const char   *prefixs =
    (logLeveli == GENERICLOGGER_LOGLEVEL_TRACE    ) ? "TRACE"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_DEBUG    ) ? "DEBUG"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_INFO     ) ? "INFO"      :
    (logLeveli == GENERICLOGGER_LOGLEVEL_NOTICE   ) ? "NOTICE"    :
    (logLeveli == GENERICLOGGER_LOGLEVEL_WARNING  ) ? "WARNING"   :
    (logLeveli == GENERICLOGGER_LOGLEVEL_ERROR    ) ? "ERROR"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_CRITICAL ) ? "CRITICAL"  :
    (logLeveli == GENERICLOGGER_LOGLEVEL_ALERT    ) ? "ALERT"     :
    (logLeveli == GENERICLOGGER_LOGLEVEL_EMERGENCY) ? "EMERGENCY" :
    "UNKOWN";
  char   *dates = dateBuilder("%d/%m/%Y %H:%M:%S");
  char   *localMsgs = messageBuilder("%s %9s %s\n", dates, prefixs, (msgs != NULL) ? (char *) msgs : (char *) GENERICLOGGER_LOG_NO_MESSAGE);
#if (defined(_WIN32) || defined (__CYGWIN__) || (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE))
#ifdef _WIN32
  int filenoStderr = _fileno(stderr);
  size_t bytesWriten = 0;
#else
  int filenoStderr = fileno(stderr);
  ssize_t bytesWriten = 0;
#endif
  char *p = localMsgs;
  size_t  count;
#endif

#if (defined(_WIN32) || (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE))
  count = strlen(p);
  while (bytesWriten < count) {
#if defined(_WIN32)
    bytesWriten += _write(filenoStderr, p+bytesWriten, count-bytesWriten);
#else
    bytesWriten += write(filenoStderr, p+bytesWriten, count-bytesWriten);
#endif
  }
#else
  /* Note: this is not asynchroneous safe */
  fprintf(stderr, "%s", p);
#endif

  if (dates != dateBuilder_internalErrors()) {
    free(dates);
  }
  if (localMsgs != messageBuilder_internalErrors()) {
    free(localMsgs);
  }
}

/********************/
/* genericLogger_newp */
/********************/
genericLogger_t *genericLogger_newp(genericLoggerCallback_t logCallbackp, void *userDatavp, genericLoggerLevel_t genericLoggerLeveli) {
  genericLogger_t *genericLoggerp = malloc(sizeof(genericLogger_t));

  if (genericLoggerp == NULL) {
    /* Well, shall we log about this - a priori no: the caller wanted to set up a particular */
    /* logging system, and not use our default */
    return NULL;
  }

  genericLoggerp->logCallbackp      = logCallbackp;
  genericLoggerp->userDatavp        = userDatavp;
  genericLoggerp->genericLoggerLeveli = genericLoggerLeveli;

  return genericLoggerp;
}

/*********************/
/* genericLogger_freev */
/*********************/
void genericLogger_freev(genericLogger_t **genericLoggerpp) {
  if (genericLoggerpp != NULL) {
    if (*genericLoggerpp != NULL) {
      free(*genericLoggerpp);
      *genericLoggerpp = NULL;
    }
  }
}

/*********************/
/* genericLogger_log */
/*********************/
void genericLogger_log(genericLogger_t *genericLoggerp, genericLoggerLevel_t genericLoggerLeveli, const char *fmts, ...) {
  va_list                ap;
#ifdef VA_COPY
  va_list                ap2;
#endif
  char                  *msgs;
  static const char     *emptyMessages = "Empty message";
  genericLoggerCallback_t  logCallbackp;
  void                  *userDatavp;
  genericLoggerLevel_t     genericLoggerDefaultLogLeveli;

  if (genericLoggerp != NULL) {
    if (genericLoggerp->logCallbackp != NULL) {
      logCallbackp = genericLoggerp->logCallbackp;
    } else {
      logCallbackp = &_genericLogger_defaultCallback;
    }
    userDatavp = genericLoggerp->userDatavp;
    genericLoggerDefaultLogLeveli = genericLoggerp->genericLoggerLeveli;
  } else {
    userDatavp = NULL;
    logCallbackp = &_genericLogger_defaultCallback;
    genericLoggerDefaultLogLeveli = GENERICLOGGER_LOGLEVEL_WARNING;
  }

  if (genericLoggerLeveli >= genericLoggerDefaultLogLeveli) {

    va_start(ap, fmts);
#ifdef VA_COPY
    VA_COPY(ap2, ap);
    msgs = (fmts != NULL) ? messageBuilder_ap(fmts, ap2) : (char *) emptyMessages;
    va_end(ap2);
#else
    msgs = (fmts != NULL) ? messageBuilder_ap(fmts, ap) : (char *) emptyMessages;
#endif
    va_end(ap);

    if (msgs != messageBuilder_internalErrors()) {
      logCallbackp(userDatavp, genericLoggerLeveli, msgs);
    } else {
      logCallbackp(userDatavp, GENERICLOGGER_LOGLEVEL_ERROR, msgs);
    }

    if (msgs != emptyMessages && msgs != messageBuilder_internalErrors()) {
      /* No need to assign to NULL, this is a local variable and we will return just after */
      free(msgs);
    }
  }

}
