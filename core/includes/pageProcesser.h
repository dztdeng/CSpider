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
void cspider_process(uv_work_t *req);
void cspider_process_done(uv_work_t *req, int status);

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
