#include "spider.h"
#include "downloader.h"
#include "utils.h"
#include "scheduler.h"
#include "cs_page_queue.h"

/**
  init_cspider : init the cspider

  return the cspider_t which is ready
**/

cspider_t *init_cspider() {
  cspider_t *spider = (cspider_t *)malloc(sizeof(cspider_t));
  PANIC(spider);
  spider->page_queue = new_page_queue();
  spider->threadpool_size = 4;
  spider->process = NULL;
  spider->save = NULL;
  spider->process_user_data = NULL;
  spider->save_user_data = NULL;
  spider->loop = uv_default_loop();

  spider->idler = (uv_prepare_t*)malloc(sizeof(uv_prepare_t));
  spider->save_lock = (uv_rwlock_t*)malloc(sizeof(uv_rwlock_t));
  uv_rwlock_init(spider->save_lock);
  spider->idler->data = spider;
  spider->site = (site_t*)malloc(sizeof(site_t));
  spider->site->user_agent = NULL;
  spider->site->proxy = NULL;
  spider->site->cookie = NULL;
  spider->site->timeout = 0;
  spider->log = NULL;
  spider->bloom = init_Bloom();
  return spider;
}
/**
 cs_setopt_url : add urls to task queue
 @cspider : the cspider
 @url : new task's url
**/
void cs_setopt_url(cspider_t *cspider, char *url){
  PANIC(cspider);
  PANIC(url);
  if (!bloom_check(cspider->bloom, url)) {
    //url no exits
    bloom_add(cspider->bloom, url);
    unsigned int len = strlen(url);
    char *reUrl = (char*)malloc(sizeof(char) * (len+1));
    PANIC(reUrl);
    strncpy(reUrl, url, len+1);
    /**
       get a new page, and set it's status url_add
     **/
    cs_page *page = get_page(cspider->page_queue);
    page->cspider = cspider;
    set_url(page, reUrl);
    /**
       change status (sleep) -> (url_add)
     **/
    set_status(cspider->page_queue, page, PAGE_DOWNLOAD_WAIT);
  }
}

/**
cs_setopt_cookie : set cookie
@cspider : the cspider
@cookie : the cookie string
**/
void cs_setopt_cookie(cspider_t *cspider, char *cookie) {
  PANIC(cspider);
  PANIC(cookie);
  ((site_t*)cspider->site)->cookie = cookie;
}

/**
  cs_setopt_useragent : set user agent
**/
void cs_setopt_useragent(cspider_t *cspider, char *agent) {
  PANIC(cspider);
  PANIC(agent);
  ((site_t*)cspider->site)->user_agent = agent;
}

/**
  cs_setopt_proxy : set proxy
**/
void cs_setopt_proxy(cspider_t *cspider, char *proxy) {
  PANIC(cspider);
  PANIC(proxy);
  ((site_t*)cspider->site)->proxy = proxy;
}

/**
 cs_setopt_timeout : set timeout(ms)
**/
void cs_setopt_timeout(cspider_t *cspider, long timeout) {
  PANIC(cspider);
  ((site_t*)cspider->site)->timeout = timeout;
}
/**
  cs_setopt_logfile : set log file
**/
void cs_setopt_logfile(cspider_t *cspider, FILE *log) {
  PANIC(cspider);
  PANIC(log);
  cspider->log = log;
  cspider->log_lock = (uv_rwlock_t*)malloc(sizeof(uv_rwlock_t));
  uv_rwlock_init(cspider->log_lock);
}
/**
cs_setopt_process : pass the costom process function to cspider
@cspider : the cspider
@process : the Pointer which point to costom process function
@user_data : Pointer to context data which you want to pass to @process
 **/
void cs_setopt_process(cspider_t *cspider, void (*process)(cspider_t *, char *, char*, void*), void *user_data) {
  PANIC(cspider);
  PANIC(process);
  cspider->process = process;
  cspider->process_user_data = user_data;
}
/**
cs_setopt_save : pass the costom save function to cspider
@cspider : the cspider
@save : the Pointer which point to costom save function
@user_data : Pointer to context data which you want to pass to @save
 **/
void cs_setopt_save(cspider_t *cspider, void (*save)(void*, void*), void *user_data){
  PANIC(cspider);
  PANIC(save);
  cspider->save = save;
  cspider->save_user_data = user_data;
}

/**
  cs_setopt_threadnum : set thread's number
  @cspider : the cspider
  @flag : thread's type
  @number : thread's max number which we want to set

  if @flag equal DOWNLOAD
  @number will be the max number of download thread
  else if @flag equal SAVE
  @number whill be the max number of save thread
**/
void cs_setopt_threadnum(cspider_t *cspider, int number) {
  PANIC(cspider);
  assert(number > 0);
  cspider->threadpool_size = number;
}
/**
   cs_run : start the cspider
**/
int cs_run(cspider_t *cspider) {
  if (cspider->process == NULL) {
    printf("warn : need to init process function(use cs_setopt_process)\n");
    return 0;
  }
  if (cspider->save == NULL) {
    printf("warn : need to init data persistence function(use cs_setopt_save)\n");
    return 0;
  }
  /*
    set the threadpool's size by setenv()
   */
  char threadpool_size[4] = {0};
  snprintf(threadpool_size, sizeof(threadpool_size), "%d", cspider->threadpool_size);
  setenv("UV_THREADPOOL_SIZE", threadpool_size, 1);

  uv_prepare_init(cspider->loop, cspider->idler);
  uv_prepare_start(cspider->idler, cspider_watcher);

  return uv_run(cspider->loop, UV_RUN_DEFAULT);
}
