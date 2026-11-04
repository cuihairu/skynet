#include <string.h>

#include "mem_info.h"

void
meminfo_init(MemInfo *info) {
    memset(info, 0, sizeof(*info));
}

void
atomic_meminfo_init(AtomicMemInfo *info) {
    /* Store free before alloc so a concurrent merge that reads free
     * first always sees free <= alloc.  ATOM_STORE is seq_cst, which
     * also provides the ordering needed on weak memory models. */
    ATOM_STORE(&info->free_count, 0);
    ATOM_STORE(&info->free, 0);
    ATOM_STORE(&info->alloc_count, 0);
    ATOM_STORE(&info->alloc, 0);
}

void
meminfo_alloc(MemInfo *info, size_t size) {
    info->alloc += size;
    ++info->alloc_count;
}

void
atomic_meminfo_alloc(AtomicMemInfo *info, size_t size) {
    ATOM_FADD(&info->alloc, size);
    ATOM_FADD(&info->alloc_count, 1);
}

void
meminfo_free(MemInfo *info, size_t size) {
    info->free += size;
    ++info->free_count;
}

void
atomic_meminfo_free(AtomicMemInfo *info, size_t size) {
    ATOM_FADD(&info->free, size);
    ATOM_FADD(&info->free_count, 1);
}

void
meminfo_merge(MemInfo *dest, const MemInfo *src) {
    dest->alloc += src->alloc;
    dest->alloc_count += src->alloc_count;
    dest->free += src->free;
    dest->free_count += src->free_count;
}

void
atomic_meminfo_merge(MemInfo *dest, const AtomicMemInfo *src) {
    MemInfo info;
    /* Read free before alloc.  For normal concurrent alloc/free,
     * free(Tf) <= free(Ta) <= alloc(Ta) for Tf < Ta, so this order
     * prevents underflow.  For slot reclaim, init writes free=0 before
     * alloc=0, so a merge that reads free=0 first is also safe. */
    info.free_count = ATOM_LOAD(&src->free_count);
    info.free = ATOM_LOAD(&src->free);
    info.alloc_count = ATOM_LOAD(&src->alloc_count);
    info.alloc = ATOM_LOAD(&src->alloc);
    meminfo_merge(dest, &info);
}
