#include<common.h>

#define BUDDY_MAX_ORDER 8
#define KB 1024
#define SZ_PAGE 4 * KB

struct page {
    void* start_addr;
    struct chunk* _chunk;
    //struct slab*
};


struct free_list{
    uint64_t nr_free;
    list_head* free_list;
};

struct list_head{
    struct chunk* _chunk;
    struct list_head* prev;
    struct list_head* next;
};

struct chunk {
    void* start_addr;
    uint8_t order;
    bool used;
    struct list_head* node;
};

struct pmm_pool{
    Area _area;
    uint64_t page_num;
    struct free_list free_lists[BUDDY_MAX_ORDER];
};

void list_add(struct chunk* chunk);

void list_del(struct chunk* chunk);

static struct pmm_pool* mm_pool = NULL;

void buddy_init(void *heap_start, void *heap_end);

struct chunk *get_buddy_chunk(struct chunk* chunk);

void chunk_append(struct chunk* chunk);

void chunk_del(struct chunk* chunk);

struct chunk* chunk_merge(struct chunk* chunk);

void chunk_free(struct chunk* chunk);

struct chunk* chunk_split(uint8_t order, struct chunk* chunk);

struct chunk* chunk_alloc(uint8_t order);



