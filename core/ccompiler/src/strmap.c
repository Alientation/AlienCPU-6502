#include "ccompiler/strmap.h"

#include <malloc.h>

typedef struct Entry
{
    const char *k;
    void *v;
} entry_t;

typedef struct Bucket
{
    entry_t *entries;
} bucket_t;

#define BUCKETS(map) ((bucket_t *) map->buckets)

static void bucket_init (bucket_t * const bucket)
{
    bucket->entries = NULL;
}

static void bucket_free (bucket_t * const bucket)
{

}

void strmap_init (strmap_t * const map, void (*free) (void *v))
{
    map->free = free;
    map->buckets = NULL;
    map->nbuckets = 0;
}

void strmap_free (strmap_t * const map)
{
    for (size_t i = 0; i < map->nbuckets; i++)
    {
        bucket_free (BUCKETS (map));
    }

    free (map->buckets);
    map->buckets = NULL;
    map->nbuckets = 0;
}
