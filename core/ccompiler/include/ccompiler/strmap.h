#pragma once

#include <stddef.h>

typedef struct StringMap
{
    void (*free)(void *v);

    void *buckets;
    size_t nbuckets;
} strmap_t;

void strmap_init (strmap_t *map, void (*free)(void *v));
void strmap_free (strmap_t *map);
