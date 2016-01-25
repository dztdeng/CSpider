#include "pageProcesser.h"
#include "cs_page_queue.h"

/**
   create a new page queue with size is default(512)
**/
cs_page_queue *new_page_queue() {
  cs_page_queue *page_queue = (cs_page_queue*) malloc(sizeof(cs_page_queue));
  PANIC(page_queue);
  /*
    the default queue size is 512
   */
  page_queue->pages = (cs_page*) malloc(sizeof(cs_page) * DEFAULT_QUEUE_SIZE);
  PANIC(page_queue->pages);
  memset(page_queue->pages, 0, sizeof(cs_page) * DEFAULT_QUEUE_SIZE);
  page_queue->capacity = DEFAULT_QUEUE_SIZE;
  page_queue->used = 0;
  page_queue->status_num[url_add] = 0;
  page_queue->status_num[url_download] = 0;
  page_queue->status_num[page_add] = 0;
  page_queue->status_num[page_process] = 0;
  /**
     init locks
   **/
  page_queue->capacity_lock = (uv_rwlock_t*) malloc(sizeof(uv_rwlock_t));
  page_queue->used_lock = (uv_rwlock_t*) malloc(sizeof(uv_rwlock_t));
  page_queue->status_lock = (uv_rwlock_t*) malloc(sizeof(uv_rwlock_t));
  uv_rwlock_init(page_queue->capacity_lock);
  uv_rwlock_init(page_queue->used_lock);
  uv_rwlock_init(page_queue->status_lock);
  return page_queue;
}
/**
   find a page to use;
  @page_queue : the page queue which we want to get page from
 
  notice : used++, after get a new cs_page

  @notice : thread safe
**/
cs_page *get_page(cs_page_queue *page_queue) {
  /* lock capacity*/
  uv_rwlock_wrlock(page_queue->capacity_lock);
  if (page_queue->used >= page_queue->capacity) {
    /* the capacity isn't enough now 
     expand it's size to double than before*/
    cs_page *new_pages = (cs_page*) malloc(sizeof(cs_page) * page_queue->capacity * 2);
    /* copy tp new place*/
    memcpy(new_pages, page_queue->pages, page_queue->capacity);
    free(page_queue->pages);
    page_queue->pages = new_pages;
    page_queue->capacity *= 2;
  }
  /* unlock capacity*/
  uv_rwlock_wrunlock(page_queue->capacity_lock);
  /* bruce force find*/
  unsigned int i = 0;
  while (i < page_queue->capacity) {
    //lock status
    uv_rwlock_rdlock(page_queue->status_lock);
    if (page_queue->pages[i].status == sleep) {
      /* unlock status */
      uv_rwlock_rdunlock(page_queue->status_lock);
      
      //lock used
      uv_rwlock_wrlock(page_queue->used_lock);
      page_queue->used++;
      /* unlock used*/
      uv_rwlock_wrunlock(page_queue->used_lock);
      
      
      return page_queue->pages + i;
    }
    //unlock status
    uv_rwlock_rdunlock(page_queue->status_lock);
    i++;
  }
  /* not found */
  return NULL;
}

/**
   change current page's status

   @notice : thread safe
**/
void set_status(cs_page_queue *queue, cs_page *page, STATUS status) {
  uv_rwlock_wrlock(queue->status_lock);
  queue->status_num[page->status]--;
  page->status = status;
  queue->status_num[status]++;
  uv_rwlock_wrunlock(queue->status_lock);
}
/**
   get page which status match

   @notice : thread safe
**/
cs_page *get_status_page(cs_page_queue *queue, STATUS status) {
  if (queue->status_num[status] == 0)
    return NULL;
  /* bruce force*/
  unsigned int i = 0;
  while (i < queue->capacity) {
    /* add status lock */
    uv_rwlock_rdlock(queue->status_lock);
    if (queue->pages[i].status == status) {
      /* delete status lock */
      uv_rwlock_rdunlock(queue->status_lock);
      return queue->pages + i;
    }
    /* delete status lock */
    uv_rwlock_rdunlock(queue->status_lock);
    i++;
  }
  /* return NULL */
  return NULL;
}
/**
   get the number of each status
**/
unsigned int get_status_num(cs_page_queue *queue, STATUS status) {
  return queue->status_num[status];
}

/**
  set url to page
 **/
void set_url(cs_page *page, char *url) {
  page->url = url;
}

/**
   clear page from page queue

   @notice : thread safe
**/
void clear_page_from_queue(cs_page_queue *queue, cs_page *page) {
  clear_page(page);
  uv_rwlock_wrlock(queue->used_lock);
  queue->used--;
  uv_rwlock_wrunlock(queue->used_lock);
}
/**
   is queue empty?
@notice : thread safe
**/
unsigned int is_queue_empty(cs_page_queue *queue) {
  uv_rwlock_rdlock(queue->used_lock);
  unsigned int res = queue->used;
  uv_rwlock_rdunlock(queue->used_lock);
  return res;
}
