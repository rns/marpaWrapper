#ifndef MARPAXML_INTERNAL_MESSAGEBUILDER_H
#define MARPAXML_INTERNAL_MESSAGEBUILDER_H

#include <stdarg.h>

/* Portable methods to build a message */

/* IF THE OUTPUT OF messageBuilder() or messageBuilder_ap() is    */
/* equal to messageBuilder_internalErrors() then the caller       */
/* must NOT call free(). In any other case the caller MUST CALL   */
/* free() on the returned value.                                  */
/* The method messageBuilder_internalErrors() is guaranteed to    */
/* never fail.                                                    */
/* The others are guaranteed to never return a NULL pointer, but  */
/* this pointer CAN be equal to messageBuilder_internalErrors()   */
/* output.                                                        */

char *messageBuilder_internalErrors(void); /* This is returning a STATIC adress */
char *messageBuilder(const char *fmts, ...);
char *messageBuilder_ap(const char *fmts, va_list ap);

#endif /* MARPAXML_INTERNAL_MESSAGEBUILDER_H */
