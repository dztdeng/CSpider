#ifndef CS_PAGE_QUEUE_H
#define CS_PAGE_QUEUE_H
#include "CS.h"

/**
page's five status
==================
@sleep : this page isn't used now.
@url_add : this page's task url is assigned now.
@url_download : this page's goal html is downloading.
@page_add : this page's goal html is waited to process.
@page_process : this page's goal html is processing now.
**/
typedef enum PAGE_STATUS {
  sleep = 0,
  url_add = 1,
  url_download = 2,
  page_add = 3,
  page_process = 4
} STATUS;

#define DEFAULT_QUEUE_SIZE 512

typedef struct cs_page_struct {
  void *data;
  unsigned int capacity;
  unsigned int used;
  char *url; /* the url where it is downloaded */
  uv_work_t *worker;/* Point to the worker */
  cspider_t *cspider;/* the Main cspider struct */
  STATUS status;
} cs_page;


typedef struct cs_page_queue_struct {
  //cs_page *pages;
  cs_page *pages;
  unsigned int capacity;
  unsigned int used;
  /*
    the number of each status
  */
  unsigned int status_num[5];
  /*
    locks
   */
  uv_rwlock_t *capacity_lock;
  uv_rwlock_t *used_lock;
  uv_rwlock_t *status_lock;
} cs_page_queue;

/*
   cs_page.c
*/
void clear_page(cs_page *p);
void destroy_page(cs_page *p);
int new_page(cs_page *p, unsigned int capacity);
int set_page(cs_page *p, char* context, unsigned int length);
/*
cs_page_queue.c
*/
cs_page_queue *new_page_queue();
cs_page *get_page(cs_page_queue *page_queue);
void set_status(cs_page_queue *queue, cs_page *page, STATUS status);
cs_page *get_status_page(cs_page_queue *queue, STATUS status);
unsigned int get_status_num(cs_page_queue *queue, STATUS status);
void set_url(cs_page *page, char *url);
void clear_page_from_queue(cs_page_queue *queue, cs_page *page);
unsigned int is_queue_empty(cs_page_queue *queue);

#endif
