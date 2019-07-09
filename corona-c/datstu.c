
#include <collectc/hashtable.h>
#include <collectc/array.h>
#include <collectc/queue.h>

#include "coronapriv.h"

// thread safe data struct

typedef struct crnmap crnmap;
struct crnmap {
    HashTable* ht;
    pmutex_t mu;
};
typedef struct crnarray crnarray;
struct crnarray {
    Array* arr;
    pmutex_t mu;
};
typedef struct crnqueue crnqueue;
struct crnqueue {
    Queue* qo;
    pmutex_t mu;
};

// from cxrt
extern HashTable* cxhashtable_new_uintptr();

crnmap* crnmap_new_uintptr() {
    crnmap* mp = (crnmap*)crn_gc_malloc(sizeof(crnmap));
    mp->ht = cxhashtable_new_uintptr();
    return mp;
}

void crnmap_free (crnmap *table) {
    pmutex_lock(&table->mu);
    hashtable_destroy(table->ht);
    table->ht = 0;
    pmutex_unlock(&table->mu);
}
enum cc_stat crnmap_add (crnmap *table, uintptr_t key, void *val) {
    pmutex_lock(&table->mu);
    enum cc_stat rv = hashtable_add(table->ht, (void*)key, val);
    pmutex_unlock(&table->mu);
    return rv;
}
enum cc_stat crnmap_get(crnmap *table, uintptr_t key, void **out) {
    pmutex_lock(&table->mu);
    enum cc_stat rv = hashtable_get(table->ht, (void*)key, out);
    pmutex_unlock(&table->mu);
    return rv;
}
enum cc_stat crnmap_remove(crnmap *table, uintptr_t key, void **out) {
    pmutex_lock(&table->mu);
    enum cc_stat rv = hashtable_remove(table->ht, (void*)key, out);
    pmutex_unlock(&table->mu);
    return rv;
}
void crnmap_remove_all(crnmap *table) {
    pmutex_lock(&table->mu);
    hashtable_remove_all(table->ht);
    pmutex_unlock(&table->mu);
}
bool crnmap_contains_key(crnmap *table, uintptr_t key) {
    pmutex_lock(&table->mu);
    bool rv = hashtable_contains_key(table->ht, (void*)key);
    pmutex_unlock(&table->mu);
    return rv;
}

size_t crnmap_size(crnmap *table){
    pmutex_lock(&table->mu);
    size_t rv = hashtable_size(table->ht);
    pmutex_unlock(&table->mu);
    return rv;
}
size_t crnmap_capacity(crnmap *table){
    pmutex_lock(&table->mu);
    size_t rv = hashtable_capacity(table->ht);
    pmutex_unlock(&table->mu);
    return rv;
}

enum cc_stat crnmap_get_keys(crnmap *table, Array **out){
    pmutex_lock(&table->mu);
    enum cc_stat rv = hashtable_get_keys(table->ht, out);
    pmutex_unlock(&table->mu);
    return rv;
}
enum cc_stat crnmap_get_values(crnmap *table, Array **out){
    pmutex_lock(&table->mu);
    enum cc_stat rv = hashtable_get_values(table->ht, out);
    pmutex_unlock(&table->mu);
    return rv;
}

/////
crnqueue* crnqueue_new() {
    crnqueue* q = crn_gc_malloc(sizeof(crnqueue));

    QueueConf qconf = {0};
    queue_conf_init(&qconf);
    qconf.mem_alloc = crn_gc_malloc;
    qconf.mem_free = crn_gc_free;
    qconf.mem_calloc = crn_gc_calloc;

    int rv = queue_new_conf(&qconf, &q->qo);
    assert(rv == CC_OK);
    return q;
}

void crnqueu_free(crnqueue* q) {
    pmutex_lock(&q->mu);
    queue_destroy(q->qo);
    q->qo = 0;
    pmutex_unlock(&q->mu);
}

enum cc_stat crnqueue_peek(crnqueue* q, void **out) {
    pmutex_lock(&q->mu);
    enum cc_stat rv = queue_peek(q->qo, out);
    pmutex_unlock(&q->mu);
    return rv;
}

enum cc_stat crnqueue_poll(crnqueue *q, void **out){
    pmutex_lock(&q->mu);
    enum cc_stat rv = queue_poll(q->qo, out);
    pmutex_unlock(&q->mu);
    return rv;
}

enum cc_stat crnqueue_enqueue(crnqueue *q, void *element){
    pmutex_lock(&q->mu);
    enum cc_stat rv = queue_enqueue(q->qo, element);
    pmutex_unlock(&q->mu);
    return rv;
}

size_t crnqueue_size(crnqueue* q){
    pmutex_lock(&q->mu);
    enum cc_stat rv = queue_size(q->qo);
    pmutex_unlock(&q->mu);
    return rv;
}
