#ifndef MARPAXML_INTERNAL_BUFMANAGER_H
#define MARPAXML_INTERNAL_BUFMANAGER_H

#include <stddef.h>                  /* size_t definition */

void *manageBuf_createp(void ***ppp, size_t *sizeip, size_t wantedNumberi, size_t elementSizei);
void  manageBuf_freev  (void ***ppp, size_t *sizeip, size_t *usedNumberip);

#endif /* MARPAXML_INTERNAL_BUFMANAGER_H */

