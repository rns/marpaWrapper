#ifdef _WIN32
#include <windows.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "marpaWrapper.h"
#include "genericLogger.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE };

typedef struct myStack {
  char *string;
  int   value;
} myStack_t;

static marpaWrapperBool_t      myValueRuleCallback(void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperRule_t *marpaWrapperRulep, size_t nValueInputi, void **valueInputArraypp, void **valueOutputpp);
static marpaWrapperBool_t      myValueSymbolCallback(void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, int value, void **valueOutputpp);
static marpaWrapperBool_t      myValueNullingCallback(void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, void **valueOutputpp);
static marpaWrapperValueBool_t myValueResultCallback(void *datavp, marpaWrapper_t *marpaWrapperp, void *valuep);

static marpaWrapperBool_t     myStackElementFreeCallback(void *datavp, marpaWrapper_t *marpaWrapperp, void *elementp);
static marpaWrapperBool_t     myStackElementCopyCallback(void *datavp, marpaWrapper_t *marpaWrapperp, void *elementDstp, void *elementSrcp);

static char *_make_str    (const char *fmt, ...);

int main() {
  size_t                     i;
  /* Object types */
  marpaWrapper_t            *marpaWrapperp;
  marpaWrapperSymbol_t      *symbolArrayp[MAX_SYMBOL];
  marpaWrapperSymbol_t      *rhsSymbolArrayp[3]; /* Just to have enough room */
  /* Options */
  marpaWrapperOption_t       marpaWrapperOption;
  marpaWrapperSymbolOption_t marpaWrapperSymbolOption;
  marpaWrapperRuleOption_t   marpaWrapperRuleOption;
  marpaWrapperValueOption_t  marpaWrapperValueOption;
  marpaWrapperStackOption_t  marpaWrapperStackOption;
  size_t                     nmarpaWrapperProgressi;
  marpaWrapperProgress_t   **marpaWrapperProgresspp;
  /* Token values */
  const char               *token_values[] = { NULL, "1", "2", "3", "0", "-", "+", "*" };
  int                       one                  = 1;      /* Indice 1 in token_values */
  int                       two                  = 2;      /* Indice 2 in token_values */
  int                       three                = 3;      /* Indice 3 in token_values */
  int                       zero                 = 4;      /* Indice 4 in token_values */
  int                       minus_token_value    = 5;      /* Indice 5 in token_values */
  int                       plus_token_value     = 6;      /* Indice 6 in token_values */
  int                       multiply_token_value = 7;      /* Indice 7 in token_values */
  genericLogger_t            *genericLoggerp = genericLogger_newp(NULL, NULL, GENERICLOGGER_LOGLEVEL_TRACE);

  /* We want TRACE log level */
  marpaWrapper_optionDefaultb(&marpaWrapperOption);
  marpaWrapperOption.genericLoggerp = genericLoggerp;

  /* Grammar */
  marpaWrapperp = marpaWrapper_newp(&marpaWrapperOption);

  /* Symbols */
  for (i = 0; i < MAX_SYMBOL; i++) {
    marpaWrapper_symbolOptionDefaultb(&(marpaWrapperSymbolOption));
    marpaWrapperSymbolOption.startb = (i == 0) ? MARPAWRAPPER_BOOL_TRUE : MARPAWRAPPER_BOOL_FALSE;
    symbolArrayp[i] = marpaWrapper_g_addSymbolp(marpaWrapperp, &marpaWrapperSymbolOption);
  }

  /* Rules */
  marpaWrapper_ruleOptionDefaultb(&marpaWrapperRuleOption);
  marpaWrapperRuleOption.rhsSymbolpp    = rhsSymbolArrayp;

  /* S ::= E */
  marpaWrapperRuleOption.lhsSymbolp     = symbolArrayp[S];
  rhsSymbolArrayp[0]                    = symbolArrayp[E];
  marpaWrapperRuleOption.nRhsSymboli    = 1;
  marpaWrapperRuleOption.datavp         = (void *) START_RULE;
  marpaWrapper_g_addRulep(marpaWrapperp, &marpaWrapperRuleOption);

  /* E ::= E op E */
  marpaWrapperRuleOption.lhsSymbolp     = symbolArrayp[ E];
  rhsSymbolArrayp[0]                    = symbolArrayp[ E];
  rhsSymbolArrayp[1]                    = symbolArrayp[op];
  rhsSymbolArrayp[2]                    = symbolArrayp[ E];
  marpaWrapperRuleOption.nRhsSymboli    = 3;
  marpaWrapperRuleOption.datavp         = (void *) OP_RULE;
  marpaWrapper_g_addRulep(marpaWrapperp, &marpaWrapperRuleOption);

  /* E ::= number */
  marpaWrapperRuleOption.lhsSymbolp     = symbolArrayp[     E];
  rhsSymbolArrayp[0]                    = symbolArrayp[number];
  marpaWrapperRuleOption.nRhsSymboli    = 1;
  marpaWrapperRuleOption.datavp         = (void *) NUMBER_RULE;
  marpaWrapper_g_addRulep(marpaWrapperp, &marpaWrapperRuleOption);
  marpaWrapper_g_precomputeb(marpaWrapperp);

  marpaWrapper_r_startb(marpaWrapperp);

  if (marpaWrapper_r_progressb(marpaWrapperp, -1, -1, &nmarpaWrapperProgressi, &marpaWrapperProgresspp) == MARPAWRAPPER_BOOL_TRUE) {
    for (i = 0; i < nmarpaWrapperProgressi; i++) {
      fprintf(stderr, "Earley Set Id: %4d, Origin Earley Set Id: %4d, Rule: %10p, Position: %3d\n",
              marpaWrapperProgresspp[i]->marpaEarleySetIdi,
              marpaWrapperProgresspp[i]->marpaEarleySetIdOrigini,
              (void *) marpaWrapperProgresspp[i]->marpaWrapperRulep,
              marpaWrapperProgresspp[i]->positioni);
    }
  }
  

  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[number],                  two, 1);
  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[    op],    minus_token_value, 1);
  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[number],                 zero, 1);
  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[    op], multiply_token_value, 1);
  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[number],                three, 1);

#ifdef _WIN32
  // Sleep(10000);
#endif
  if (marpaWrapper_r_progressb(marpaWrapperp, 0, -1, &nmarpaWrapperProgressi, &marpaWrapperProgresspp) == MARPAWRAPPER_BOOL_TRUE) {
    for (i = 0; i < nmarpaWrapperProgressi; i++) {
      fprintf(stderr, "Earley Set Id: %4d, Origin Earley Set Id: %4d, Rule: %10p, Position: %3d\n",
              marpaWrapperProgresspp[i]->marpaEarleySetIdi,
              marpaWrapperProgresspp[i]->marpaEarleySetIdOrigini,
              (void *) marpaWrapperProgresspp[i]->marpaWrapperRulep,
              marpaWrapperProgresspp[i]->positioni);
    }
  }
  
  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[    op],     plus_token_value, 1);
  marpaWrapper_r_readb(marpaWrapperp, symbolArrayp[number],                  one, 1);

  marpaWrapper_valueOptionDefaultb(&marpaWrapperValueOption);
  marpaWrapperValueOption.valueRuleCallbackp         = &myValueRuleCallback;
  marpaWrapperValueOption.valueSymbolCallbackp       = &myValueSymbolCallback;
  marpaWrapperValueOption.valueSymbolCallbackDatavp  = (void *) token_values;
  marpaWrapperValueOption.valueNullingCallbackp      = &myValueNullingCallback;
  marpaWrapperValueOption.valueResultCallbackp       = &myValueResultCallback;

  marpaWrapper_stackOptionDefaultb(&marpaWrapperStackOption);
  marpaWrapperStackOption.stackElementSizei = sizeof(myStack_t);
  marpaWrapperStackOption.stackElementFreeCallbackp = &myStackElementFreeCallback;
  marpaWrapperStackOption.stackElementCopyCallbackp = &myStackElementCopyCallback;

#ifdef _WIN32
  // Sleep(10000);
#endif

  marpaWrapper_vb(marpaWrapperp, &marpaWrapperValueOption, &marpaWrapperStackOption);

  marpaWrapper_destroyv(&marpaWrapperp);

  genericLogger_freev(&genericLoggerp);

  return 0;
}

static marpaWrapperBool_t myValueRuleCallback(void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperRule_t *marpaWrapperRulep, size_t nValueInputi, void **valueInputArraypp, void **valueOutputpp) {
  int rulei;
  myStack_t *outputp = malloc(sizeof(myStack_t));
  myStack_t **stackpp = (myStack_t **) valueInputArraypp;

  if (outputp == NULL) {
    return MARPAWRAPPER_BOOL_FALSE;
  }

  marpaWrapperRule_datavp_getb(marpaWrapperRulep, (void **) &rulei);

  switch (rulei) {
  case START_RULE:
    /* S ::= E */
    outputp->string = _make_str("%s == %d", stackpp[0]->string, stackpp[0]->value);
    if (outputp->string == NULL) {
      free(outputp);
      return MARPAWRAPPER_BOOL_FALSE;
    }
    outputp->value = stackpp[0]->value;
    break;

  case NUMBER_RULE:
    /* E ::= number */
    outputp->string = _make_str("%d", stackpp[0]->value);
    if (outputp->string == NULL) {
      free(outputp);
      return MARPAWRAPPER_BOOL_FALSE;
    }
    outputp->value = stackpp[0]->value;
    break;

  case OP_RULE:
    /* E ::= E op E */
    outputp->string = _make_str("(%s%s%s)", stackpp[0]->string, stackpp[1]->string, stackpp[2]->string);
    if (outputp->string == NULL) {
      free(outputp);
      return MARPAWRAPPER_BOOL_FALSE;
    }
    switch (stackpp[1]->string[0]) {
    case '+':
      outputp->value = stackpp[0]->value + stackpp[2]->value;
      break;
    case '-':
      outputp->value = stackpp[0]->value - stackpp[2]->value;
      break;
    case '*':
      outputp->value = stackpp[0]->value * stackpp[2]->value;
      break;
    default:
      free(outputp);
      return MARPAWRAPPER_BOOL_FALSE;
    }
    break;

  default:
    free(outputp);
    return MARPAWRAPPER_BOOL_FALSE;
  }

  *valueOutputpp = outputp;

  return MARPAWRAPPER_BOOL_TRUE;
}

static marpaWrapperBool_t myValueSymbolCallback(void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, int value, void **valueOutputpp) {
  const char **token_valuesp = (const char **) datavp;
  myStack_t *outputp = malloc(sizeof(myStack_t));

  if (outputp == NULL) {
    return MARPAWRAPPER_BOOL_FALSE;
  }
  outputp->string = strdup(token_valuesp[value]);
  if (outputp->string == NULL) {
    free(outputp);
    return MARPAWRAPPER_BOOL_FALSE;
  }
  outputp->value   = atoi(outputp->string);

  *valueOutputpp = outputp;

  return MARPAWRAPPER_BOOL_TRUE;
}

static marpaWrapperBool_t myValueNullingCallback(void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, void **valueOutputpp) {
  /* Should not happen with our example */
  return MARPAWRAPPER_BOOL_FALSE;
}

static marpaWrapperValueBool_t myValueResultCallback(void *datavp, marpaWrapper_t *marpaWrapperp, void *valuep) {
  myStack_t *outputp = (myStack_t *) valuep;

  fprintf(stderr, "Result: %s\n", outputp->string);

  return MARPAWRAPPER_BOOL_TRUE;
}

marpaWrapperBool_t myStackElementFreeCallback(void *datavp, marpaWrapper_t *marpaWrapperp, void *elementp) {
  myStack_t *p = (myStack_t *) elementp;

  if (p->string != NULL) {
    free(p->string);
    p->string = NULL;
  }

  return MARPAWRAPPER_BOOL_TRUE;
}

marpaWrapperBool_t myStackElementCopyCallback(void *datavp, marpaWrapper_t *marpaWrapperp, void *elementDstp, void *elementSrcp) {
  myStack_t *dstp = (myStack_t *) elementDstp;
  myStack_t *srcp = (myStack_t *) elementSrcp;

  *dstp = *srcp;

  if (dstp->string != NULL) {
    dstp->string = strdup(dstp->string);
    if (dstp->string == NULL) {
      return MARPAWRAPPER_BOOL_FALSE;
    }
  }
  
  return MARPAWRAPPER_BOOL_TRUE;
}

/* Copied almost verbatim from snprintf man page on Linux */
static char *_make_str(const char *fmt, ...) {
  int n;
  int size = 100;     /* Guess we need no more than 100 bytes */
  char *p, *np;
  va_list ap;

  p = malloc(size);
  if (p == NULL) {
    fprintf(stderr, "malloc() failure\n");
    exit(EXIT_FAILURE);
  }

  while (1) {

    /* Try to print in the allocated space */

    va_start(ap, fmt);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    /* Check error code */

#ifndef _WIN32
    if (n < 0) {
      free(p);
      fprintf(stderr, "vsnprintf() failure\n");
      exit(EXIT_FAILURE);
    }
#endif

    /* If that worked, return the string */

    if
#ifdef _WIN32
      (n >= 0 && n < size)
#else
      (n < size)
#endif
        {
      return p;
    }

    /* Else try again with more space */

    size = n + 1;       /* Precisely what is needed */

    np = realloc(p, size);
    if (np == NULL) {
      free(p);
      fprintf(stderr, "realloc() failure\n");
      exit(EXIT_FAILURE);
    } else {
      p = np;
    }
  }
}
