#ifndef PAGEPROCESSER_H_DEF
#define PAGEPROCESSER_H_DEF

#include "CS.h"
#include "spider.h"
#include "downloader.h"
#include "cs_page_queue.h"

/*

*/
#define LOCK 1
#define NO_LOCK 0


/*
  dataProcess.c
*/
void dataproc(uv_work_t *req);
void datasave(uv_work_t *req, int status);
/**
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
/*
  user interface
*/
void saveString(cspider_t *cspider, void *data, int flag);
void saveStrings(cspider_t *cspider, void **datas, int size, int flag);
void addUrl(cspider_t *cspider, char *url);
void addUrls(cspider_t *cspider, char **urls, int size);
void freeString(char *str);
void freeStrings(char **strs, int size);

#endif
