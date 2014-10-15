#ifndef MARPAWRAPPER_INTERNAL_MARPAWRAPPER_H
#define MARPAWRAPPER_INTERNAL_MARPAWRAPPER_H

#include "marpaWrapper_Export.h"
#include <stddef.h>                  /* size_t definition */

/* Convention is:
   - all pointer symbols end with 'p'
   - all number  symbols end with 'i'
   - all boolean symbols end with 'b'
   - all string  symbols end with 's'
   - all void    symbols end with 'v'
   - all types    end with '_t'
*/

#include "genericLogger.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapper           marpaWrapper_t;
typedef struct marpaWrapperSymbol     marpaWrapperSymbol_t;
typedef struct marpaWrapperRule       marpaWrapperRule_t;
typedef struct marpaWrapperGrammar    marpaWrapperGrammar_t;
typedef struct marpaWrapperRecognizer marpaWrapperRecognizer_t;
typedef struct marpaWrapperValue      marpaWrapperValue_t;

/*******************/
/* Published types */
/*******************/

/* Boolean */
typedef enum marpaWrapperBool {
  MARPAWRAPPER_BOOL_FALSE =  0,
  MARPAWRAPPER_BOOL_TRUE  =  1
} marpaWrapperBool_t;

/* Value callback: boolean and stop directive */
typedef enum marpaWrapperValueBool {
  MARPAWRAPPER_VALUECALLBACKBOOL_STOP  = -1,     /* Used only when iterating over values via marpaWrapperValueCallback() */
  MARPAWRAPPER_VALUECALLBACKBOOL_FALSE =  0,
  MARPAWRAPPER_VALUECALLBACKBOOL_TRUE  =  1
} marpaWrapperValueBool_t;

/* Events */
typedef enum marpaWrapperEventType {
  MARPAWRAPPER_EVENTTYPE_COMPLETED = 0x01,
  MARPAWRAPPER_EVENTTYPE_NULLED    = 0x02,
  MARPAWRAPPER_EVENTTYPE_PREDICTED = 0x04,
} marpaWrapperEventType_t;

typedef struct marpaWrapperEvent {
  marpaWrapperEventType_t  eventType;
  marpaWrapperSymbol_t    *marpaWrapperSymbolp;
} marpaWrapperEvent_t;

/* Callbacks */

/* marpaWrapperEventCallback_t happen during lexing phase only */
typedef marpaWrapperBool_t      (*marpaWrapperEventCallback_t)           (void *datavp, marpaWrapper_t *marpaWrapperp, size_t nEventi, marpaWrapperEvent_t *marpaWrapperEventp);

/* Rule, symbol, nulling and result callbacks happen during value phase only. In this phase there is the notion of generic stack as well. */
typedef marpaWrapperBool_t      (*marpaWrapperValueRuleCallback_t)       (void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperRule_t *marpaWrapperRulep, size_t nValueInputi, void **valueInputArraypp, void **valueOutputpp);
typedef marpaWrapperBool_t      (*marpaWrapperValueSymbolCallback_t)     (void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, int value, void **valueOutputpp);
typedef marpaWrapperBool_t      (*marpaWrapperValueNullingCallback_t)    (void *datavp, marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, void **valueOutputpp);
typedef marpaWrapperValueBool_t (*marpaWrapperValueResultCallback_t)     (void *datavp, marpaWrapper_t *marpaWrapperp, void *valuep);
typedef marpaWrapperBool_t      (*marpaWrapperStackElementFreeCallback_t)(void *datavp, marpaWrapper_t *marpaWrapperp, void *elementp);
typedef marpaWrapperBool_t      (*marpaWrapperStackElementCopyCallback_t)(void *datavp, marpaWrapper_t *marpaWrapperp, void *elementDstp, void *elementSrcp);

/* Symbol options - This is an optional parameter, so there are defaults if is not supplied. */
typedef struct marpaWrapperSymbolOption {
  void              *datavp;      /* Default: NULL.                    User's opaque data pointer for this symbol     */
  marpaWrapperBool_t terminalb;   /* Default: MARPAWRAPPER_BOOL_FALSE. Eventually force symbol to be terminal         */
  marpaWrapperBool_t startb;      /* Default: MARPAWRAPPER_BOOL_FALSE. Eventually force symbol to be the start symbol */
  int                eventSeti;   /* Default: 0.                                                                      */
  size_t             sizel;       /* Default: 0. Used only if this is seen as an expected terminal and in LATM mode   */
  signed int         firstChari;  /* Default: -1. Used only if this is seen as an expected terminal and in LATM mode  */
} marpaWrapperSymbolOption_t;

/* Rule options - this is a required parameter, so the user is EXPECTED to overwrite some members */
/* with meaningful values. In particular:                                                         */
/* - lhsSymbolp                                                                                   */
/* - nRhsSymboli                                                                                  */
/* - rhsSymbolpp                                                                                  */
typedef struct marpaWrapperRuleOption {
  void                  *datavp;            /* Default: NULL.                    User's opaque data pointer for this rule                           */
  marpaWrapperSymbol_t  *lhsSymbolp;        /* Default: NULL.                    LHS symbol as returned by marpaWrapperSymbol_newp()                */
  size_t                 nRhsSymboli;       /* Default: 0.                       Number of RHS                                                      */
  marpaWrapperSymbol_t **rhsSymbolpp;       /* Default: NULL.                    RHS symbols as returned by marpaWrapperSymbol_newp()               */
  int                    ranki;             /* Default: 0.                       Rank                                                               */
  marpaWrapperBool_t     nullRanksHighb;    /* Default: MARPAWRAPPER_BOOL_FALSE. Null variant pattern                                               */

  marpaWrapperBool_t     sequenceb;         /* Default: MARPAWRAPPER_BOOL_FALSE. Sequence ?                                                         */
  marpaWrapperSymbol_t  *separatorSymbolp;  /* Default: NULL.                    Eventual separator symbol as returned by marpaWrapperSymbol_newp() */
  marpaWrapperBool_t     properb;           /* Default: MARPAWRAPPER_BOOL_FALSE. Proper flag                                                        */
  int                    minimumi;          /* Default: 0.                       Mininimum - must be 0 or 1                                         */
} marpaWrapperRuleOption_t;

/**************************/
/* Constructor/destructor */
/**************************/
typedef struct marpaWrapperOption {
  int                          (*versionip)[3];          /* Default: NULL                                                             */
  genericLogger_t                 *genericLoggerp;           /* Default: NULL                                                             */
  marpaWrapperEventCallback_t    eventCallbackp;         /* Default: NULL                                                             */
  void                          *eventCallbackDatavp;    /* Default: NULL                                                             */
  marpaWrapperBool_t             warningIsErrorb;        /* Default: MARPAWRAPPER_BOOL_FALSE. Have precedence over warningIsIgnoredb  */
  marpaWrapperBool_t             warningIsIgnoredb;      /* Default: MARPAWRAPPER_BOOL_FALSE                                          */
  marpaWrapperBool_t             unsortedEventsb;        /* Default: MARPAWRAPPER_BOOL_FALSE. Completed, then nulled, then predicted. */
} marpaWrapperOption_t;
MARPAWRAPPER_EXPORT marpaWrapper_t           *marpaWrapper_newp(marpaWrapperOption_t *marpaWrapperOptionp);
MARPAWRAPPER_EXPORT void                      marpaWrapper_destroyv(marpaWrapper_t **marpaWrapperpp);

/**************************************************/
/* Phase 1: Grammar definition and precomputation */
/**************************************************/
MARPAWRAPPER_EXPORT marpaWrapperSymbol_t     *marpaWrapper_g_addSymbolp (marpaWrapper_t *marpaWrapperp, marpaWrapperSymbolOption_t *marpaWrapperSymbolOption);
MARPAWRAPPER_EXPORT marpaWrapperRule_t       *marpaWrapper_g_addRulep   (marpaWrapper_t *marpaWrapperp, marpaWrapperRuleOption_t *marpaWrapperRuleOptionp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_g_precomputeb(marpaWrapper_t *marpaWrapperp);

/***********************/
/* Phase 2: Recognizer */
/***********************/
typedef struct marpaWrapperProgress {
  int                 marpaEarleySetIdi;
  int                 marpaEarleySetIdOrigini;
  marpaWrapperRule_t *marpaWrapperRulep;
  int                 positioni;
} marpaWrapperProgress_t;

/* For those wanting to have manual control on recognizer */
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_startb               (marpaWrapper_t *marpaWrapperp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_alternativeb         (marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, int value, int length);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_completeb            (marpaWrapper_t *marpaWrapperp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_readb                (marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, int value, int length);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_event_activateb      (marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, int eventSeti, marpaWrapperBool_t onb);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_terminals_expectedb  (marpaWrapper_t *marpaWrapperp, size_t *nMarpaWrapperSymbolip, marpaWrapperSymbol_t ***marpaWrapperSymbolppp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_terminal_is_expectedb(marpaWrapper_t *marpaWrapperp, marpaWrapperSymbol_t *marpaWrapperSymbolp, marpaWrapperBool_t *isExpectedbp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_r_progressb            (marpaWrapper_t *marpaWrapperp, int starti, int endi, size_t *nmarpaWrapperProgressip, marpaWrapperProgress_t ***marpaWrapperProgressppp);

typedef marpaWrapperBool_t (*marpaWrapper_readerCallback_t)(void *readerCallbackDatavp, marpaWrapperBool_t *endOfInputbp);
typedef marpaWrapperBool_t (*marpaWrapper_isLexemebCallback_t)(void *marpaWrapperSymbolOptionDatavp, size_t *lengthlp);
/* If there is more than one possible alternative, marpaWrapper_lexemeValuebCallback() will be called with one of the symbol candidates, once */
typedef marpaWrapperBool_t (*marpaWrapper_lexemeValuebCallback_t)(void *marpaWrapperSymbolOptionDatavp, int *lexemeValueip, int *lexemeLengthip);
typedef marpaWrapperBool_t (*marpaWrapper_ruleToCharsbCallback_t)(void *marpaWrapperRuleOptionDatavp, const char **rulesp);
typedef marpaWrapperBool_t (*marpaWrapper_symbolToCharsbCallback_t)(void *marpaWrapperSymbolOptionDatavp, const char **symbolsp);

/* Generic routine using all of the above. There is not default for the content of this typedef: user will have to provide a valid pointer. */
typedef struct marpaWrapperRecognizerOption {
  marpaWrapperBool_t                    remainingDataIsOkb;
  marpaWrapperBool_t                    longestAcceptableTokenMatchb;
  marpaWrapperBool_t                    longestAcceptableTokensShareTheSameValueAndLengthb;
  marpaWrapper_readerCallback_t         readerCallbackp;
  void                                 *readerDatavp;
  marpaWrapper_isLexemebCallback_t      isLexemebCallbackp;
  marpaWrapper_lexemeValuebCallback_t   lexemeValuebCallbackp;
  marpaWrapper_ruleToCharsbCallback_t   ruleToCharsbCallbackp;
  marpaWrapper_symbolToCharsbCallback_t symbolToCharsbCallbackp;
} marpaWrapperRecognizerOption_t;

MARPAWRAPPER_EXPORT marpaWrapperBool_t marpaWrapper_r_recognizeb(marpaWrapper_t *marpaWrapperp, marpaWrapperRecognizerOption_t *marpaWrapperRecognizerOptionp);

/******************/
/* Phase 3: Value */
/******************/
typedef struct marpaWrapperValueOption {
  /* Ordering options */
  marpaWrapperBool_t                     highRankOnlyb;                          /* Default: MARPAWRAPPER_BOOL_TRUE  */
  marpaWrapperBool_t                     orderByRankb;                           /* Default: MARPAWRAPPER_BOOL_TRUE  */
  marpaWrapperBool_t                     ambiguityb;                             /* Default: MARPAWRAPPER_BOOL_TRUE  */
  marpaWrapperBool_t                     nullb;                                  /* Default: MARPAWRAPPER_BOOL_TRUE  */
  /* Stack manipulation */
  marpaWrapperValueRuleCallback_t        valueRuleCallbackp;                     /* Default: NULL                    */
  void                                  *valueRuleCallbackDatavp;                /* Default: NULL                    */
  marpaWrapperValueSymbolCallback_t      valueSymbolCallbackp;                   /* Default: NULL                    */
  void                                  *valueSymbolCallbackDatavp;              /* Default: NULL                    */
  marpaWrapperValueNullingCallback_t     valueNullingCallbackp;                  /* Default: NULL                    */
  void                                  *valueNullingCallbackDatavp;             /* Default: NULL                    */
  marpaWrapperValueResultCallback_t      valueResultCallbackp;                   /* Default: NULL                    */
  void                                  *valueResultCallbackDatavp;              /* Default: NULL                    */
} marpaWrapperValueOption_t;

/* marpaWrapperStackOptionp is mandatory, so there is no default */
typedef struct marpaWrapperStackOption {
  size_t                                 stackElementSizei;                      /* Default: 0. Must be > 0          */
  marpaWrapperStackElementFreeCallback_t stackElementFreeCallbackp;              /* Default: NULL.                   */
  void                                  *stackElementFreeCallbackUserDatap;      /* Default: NULL                    */
  marpaWrapperStackElementCopyCallback_t stackElementCopyCallbackp;              /* Default: NULL                    */
  void                                  *stackElementCopyCallbackUserDatap;      /* Default: NULL                    */
} marpaWrapperStackOption_t;
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_vb(marpaWrapper_t *marpaWrapperp, marpaWrapperValueOption_t *marpaWrapperValueOptionp, marpaWrapperStackOption_t *marpaWrapperStackOptionp);

/*************************/
/* Default options helper */
/*************************/
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_optionDefaultb          (marpaWrapperOption_t *marpaWrapperOptionp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_symbolOptionDefaultb    (marpaWrapperSymbolOption_t *marpaWrapperSymbolOptionp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_ruleOptionDefaultb      (marpaWrapperRuleOption_t *marpaWrapperRuleOptionp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_valueOptionDefaultb     (marpaWrapperValueOption_t *marpaWrapperValueOptionp);
MARPAWRAPPER_EXPORT marpaWrapperBool_t        marpaWrapper_stackOptionDefaultb     (marpaWrapperStackOption_t *marpaWrapperStackOptionp);

/*****************/
/* Top accessors */
/*****************/
MARPAWRAPPER_EXPORT genericLogger_t *marpaWrapper_genericLoggerp(marpaWrapper_t *marpaWrapperp);

/* Generic getter definition */
#define MARPAWRAPPER_GENERATE_GETTER_DECLARATION(prefix, externalType, externalName) \
  MARPAWRAPPER_EXPORT marpaWrapperBool_t prefix##_##externalName##_getb(prefix##_t * prefix##p, externalType *externalName##p)

/***************************************************************************************/
/* From a wrapper opaque pointer, get size of/array of opaque symbol and rule pointers */
/***************************************************************************************/
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper,                  size_t, sizeMarpaWrapperSymboli);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper, marpaWrapperSymbol_t **, marpaWrapperSymbolpp);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper,                  size_t, sizeMarpaWrapperRulei);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper,   marpaWrapperRule_t **, marpaWrapperRulepp);

/******************************************************************/
/* From a symbol opaque pointer, get opaque user data, work flags */
/******************************************************************/
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapperSymbol, void *, datavp);

/****************************************************/
/* From a rule opaque pointer, get opaque user data */
/****************************************************/
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapperRule, void *, datavp);

/*************************************************************************************/
/* Avdanced users will want to play themselves with Marpa objects                    */
/* The user application will have to #include "marpa.h" and typecast correspondingly */
/*************************************************************************************/
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper, void *, Marpa_Config);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper, void *, Marpa_Grammar);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapper, void *, Marpa_Recognizer);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapperSymbol, unsigned int, Marpa_Symbol_ID);
MARPAWRAPPER_GENERATE_GETTER_DECLARATION(marpaWrapperRule, unsigned int, Marpa_Rule_ID);

#endif /* MARPAWRAPPER_INTERNAL_MARPAWRAPPER_H */
