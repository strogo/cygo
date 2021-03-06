
#include <cxrtbase.h>
#include <collectc/array.h>

struct array_s {
    size_t   size;
    size_t   capacity;
    float    exp_factor;
    void   **buffer;

    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);

    size_t extra;
};
typedef struct array_s hkarray;

extern void* cxmalloc(size_t size);
extern void* cxcalloc(size_t blocks, size_t size);
extern void cxfree(void* ptr);

#define DEFAULT_CAPACITY 8
#define DEFAULT_EXPANSION_FACTOR 2
ArrayConf cxdftarconf = {.exp_factor = DEFAULT_EXPANSION_FACTOR,
                         .capacity   = DEFAULT_CAPACITY,
                         .mem_alloc = cxmalloc,
                         .mem_calloc = cxcalloc,
                         .mem_free = cxfree};

Array* cxarray_new() {
    return cxarray_new2(1);
}
Array* cxarray_new2(int cap) {
    Array* res = 0;
    ArrayConf arrconf = {0};
    arrconf.capacity = 1+cap;
    arrconf.mem_alloc = cxmalloc;
    arrconf.mem_calloc = cxcalloc;
    arrconf.mem_free = cxfree;

    int rv = array_new_conf(&arrconf, &res);
    assert(rv == CC_OK);
    return res;
}

Array* cxarray_slice(Array* a0, int start, int end) {
    Array* narr = cxmalloc(sizeof(hkarray));
    memcpy(narr, a0, sizeof(Array));
    hkarray* hkarr = (hkarray*)narr;
    hkarr->size = end - start;
    hkarr->capacity = array_capacity(a0) - start;
    hkarr->buffer = ((hkarray*)a0)->buffer + start;

    return narr;
}

void* cxarray_get_at(Array* a0, int idx) {
    void* out = 0;
    int rv = array_get_at(a0, idx, &out);
    assert(rv == CC_OK);
    return out;
}

Array* cxarray_append(Array* a0, void* v) {
    int rv = array_add(a0, v);
    assert(rv == CC_OK);
    return a0;
}

// with element size
cxarray2* cxarray2_new(int len, int elemsz) {
    assert(len >= 0);
    assert(elemsz > 0);
    cxarray2* arr = (cxarray2*)cxmalloc(sizeof(cxarray2));

    int cap = len;
    cap = cap < 8 ? 8 : cap;
    arr->len = len;
    arr->cap = cap;
    arr->elemsz = elemsz;
    arr->ptr = (uint8*)cxmalloc(cap*elemsz);
    return arr;
}

cxarray2* cxarray2_slice(cxarray2* a0, int start, int end) {
    assert(a0 != nilptr);
    assert(start >= 0);
    assert(end >= 0);
    assert(end >= start);

    cxarray2* narr = cxarray2_new(end-start+1, a0->elemsz);
    memcpy(narr->ptr, a0->ptr+start, end-start);
    narr->len = end-start;
    return narr;
}

void cxarray2_expend(cxarray2* a0, int n) {
    assert(n > 0);
    int sz = a0->len + n;
    if (sz >= a0->cap) {
        int cap = a0->cap*2;
        cap = cap <= 0 ? n : cap;
        cap = cap > sz ? cap : cap*2;
        uint8* ptr = cxmalloc(cap*a0->elemsz);
        memcpy(ptr, a0->ptr, a0->len*a0->elemsz);
        a0->ptr = ptr;
        a0->cap = cap;
    }
    assert(a0->cap >= a0->len+n);
}
cxarray2* cxarray2_append(cxarray2* a0, void* v) {
    assert(a0 != nilptr);
    assert( v != nilptr);
    cxarray2_expend(a0, 1);
    int offset = a0->len * a0->elemsz;
    memcpy(a0->ptr+offset, v, a0->elemsz);
    a0->len += 1;
    return a0;
}

voidptr* cxarray2_get_at(cxarray2* a0, int idx) {
    assert(a0 != nilptr);
    assert(idx < a0->len);

    int offset = idx * a0->elemsz;
    voidptr* out = 0;
    out = a0->ptr+offset;
    return out;
}
uint8* cxarray2_replace_at(cxarray2* a0, void* v, int idx, void*out) {
    assert(a0 != nilptr);
    assert(idx < a0->len);

    int offset = idx * a0->elemsz;
    if (out != nilptr) {
        memcpy(out, a0->ptr+offset, a0->elemsz);
    }
    memcpy(a0->ptr+offset, v, a0->elemsz);
    return (uint8*) out;
}
int cxarray2_size(cxarray2* a0) { return a0->len; }
int cxarray2_len(cxarray2* a0) { return a0->len; }
int cxarray2_capacity(cxarray2* a0) { return a0->cap; }
int cxarray2_cap(cxarray2* a0) { return a0->cap; }
int cxarray2_elemsz(cxarray2* a0) { return a0->elemsz; }

void cxarray2_appendn(cxarray2* a0, void* v, int n) {
    assert(a0 != nilptr);
    int tlen = n * a0->elemsz;
    cxarray2_expend(a0, n);
    memcpy(a0->ptr, v, tlen);
    a0->len += n;
}

