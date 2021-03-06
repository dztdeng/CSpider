#ifndef CSPIDER_H
#define CSPIDER_H

#include "CS.h"
#include "downloader.h"
#include "pageProcesser.h"
#include "utils.h"
#include "cJSON.h"
#include "uriparser.h"

typedef struct site_struct {
  char *user_agent;//user agent
  char *proxy;// proxy address
  char *cookie;//cookie string
  long timeout;// timeout (ms)
} site_t;

struct cspider_struct {
  uv_loop_t *loop;
  uv_prepare_t *idler;

  cs_page_queue *page_queue;
  // custom function
  void (*process)(struct cspider_struct *cspider, char *d, char *url, void *user_data);
  void (*save)(void *data, void *user_data);
  //
  void *process_user_data;
  void *save_user_data;
  //Thread pool's size
  int threadpool_size;
  //data persistence lock
  uv_rwlock_t *save_lock;
  //include useragent, cookie, timeout, proxy
  site_t *site;
  //log file
  FILE *log;
  uv_rwlock_t *log_lock;
  //bloom filter
  Bloom_t *bloom;
};

cspider_t *init_cspider();
void cs_setopt_url(cspider_t *cspider, char *url);
void cs_setopt_cookie(cspider_t *cspider, char *cookie);
void cs_setopt_useragent(cspider_t *cspider, char *agent);
void cs_setopt_proxy(cspider_t *cspider, char *proxy);
void cs_setopt_timeout(cspider_t *cspider, long timeout);
void cs_setopt_logfile(cspider_t *cspider, FILE *log);
void cs_setopt_process(cspider_t *cspider, void (*process)(cspider_t *, char *, char*, void*), void *user_data);
void cs_setopt_save(cspider_t *cspider, void (*save)(void*, void*), void *user_data);
void cs_setopt_threadnum(cspider_t *cspider, int number);
int cs_run(cspider_t *cspider);

#endif
