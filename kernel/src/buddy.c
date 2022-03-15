#include <buddy.h>
#include <common.h>

// todo:impl

/**
 * @brief initialize the buddy system
 *
 * @param mm_pool pointer of pmm-pool
 * @param start_chunk start address of the first chunk
 * @param start_addr start address of whole mm.
 * @param page_num the number of page to be managed.
 */
void buddy_init(struct pmm_pool* mm_pool, struct chunk* start_chunk,
                void* start_addr, uint64_t page_num) {
  struct chunk* chunk;
  int order;
  int chunk_idx;
  mm_pool->begin_addr = start_addr;
  mm_pool->page_num = page_num;
  mm_pool->size = page_num * SZ_PAGE;

  for (order = 0; order < BUDDY_MAX_ORDER; order++) {
    mm_pool->free_lists[order].nr_free = 0;
    INIT_LIST_HEAD(&(mm_pool->free_lists[order].free_list));
  }

  memset((char*)start_chunk, 0, page_num * sizeof(struct chunk));

  for (chunk_idx = 0; chunk_idx < page_num; chunk_idx++) {
    chunk = start_chunk + chunk_idx;
    chunk->used = 1;
    chunk->order = 0;
  }

  for (chunk_idx = 0; chunk_idx < page_num; chunk_idx++) {
    chunk = start_chunk + chunk_idx;
    chunk_free(mm_pool, chunk);
  }
}

/**
 * @brief free a chunk and process in deep.
 *
 * @param mm_pool
 * @param chunk
 */
void chunk_free(struct pmm_pool* mm_pool, struct chunk* chunk) {
  if (!chunk->used) {
    // todo：无法回收空闲内存块
    return;
  }

  chunk->used = false;
  chunk_append(mm_pool, chunk);

  chunk_merge(mm_pool, chunk);
}

/**
 * @brief make a chunk added to free list.
 *
 * @param mm_pool
 * @param chunk
 */
void chunk_append(struct pmm_pool* mm_pool, struct chunk* chunk) {
  struct free_list* free_list = &mm_pool->free_lists[chunk->order];
  list_add(free_list, &chunk->node);
  free_list->nr_free++;
}

struct chunk* chunk_merge(struct pmm_pool* mm_pool, struct chunk* chunk) {
  // todo
}

struct chunk* get_buddy_chunk(struct pmm_pool* mm_pool, struct chunk* chunk) {
  // todo
}

struct chunk* chunk_alloc(struct pmm_pool* mm_pool, uint8_t order) {
  // todo
}

void chunk_del(struct pmm_pool* mm_pool, struct chunk* chunk) {}