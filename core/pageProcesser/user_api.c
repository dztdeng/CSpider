#include "pageProcesser.h"
#include "utils.h"
/*
  provide two functions to users.
  saveString : save the string.
  addUrl : add the url back to the download task queue.
*/
/**
  saveString : api of data persistence
  @cspider : the cspider_t
  @data : the pointer point to data which is ready to be saved
  @flag : if the data persistence need to be locked
**/
void saveString(cspider_t *cspider, void *data, int flag) {
  assert(flag == LOCK || flag == NO_LOCK);
  if (flag == LOCK) {
    /* thread safe */
    uv_rwlock_wrlock(cspider->save_lock);
    (cspider->save)(data, cspider->save_user_data);
    uv_rwlock_wrunlock(cspider->save_lock);
  } else {
    (cspider->save)(data, cspider->save_user_data);
  }
}
/**
  saveStrings : save array of data
  @cspider : the cspider_t
  @datas : array of data pointer
  @size : size of @datas
  @flag : if the data persistence need to be locked

**/
void saveStrings(cspider_t *cspider, void **datas, int size, int flag) {
  int i;
  assert(flag == LOCK || flag == NO_LOCK);
  if (flag == LOCK) {
    /*thread safe*/
    uv_rwlock_wrlock(cspider->save_lock);
    for (i = 0; i < size; i++) {
      (cspider->save)(datas[i], cspider->save_user_data);
    }
    uv_rwlock_wrunlock(cspider->save_lock);
  } else {
    
    for (i = 0; i < size; i++) {
      (cspider->save)(datas[i], cspider->save_user_data);
    }
  }
}
/**
  addUrl : add url back to task queue
  @cspider : the cspider_t
  @url : url added back to task queue
**/
void addUrl(cspider_t *cspider, char *url) {
  if (!bloom_check(cspider->bloom, url)) {
    /*this url not exits*/
    bloom_add(cspider->bloom, url);
    unsigned int len = strlen(url) + 1;
    char *reUrl = (char*)malloc(sizeof(char) * len);
    PANIC(reUrl);
    strncpy(reUrl, url, len);
    cs_page *page = get_page(cspider->page_queue);
    set_url(page, reUrl);
    /**
       change status (sleep) -> (url_add)
     **/
    set_status(cspider->page_queue, page, url_add);
    page->cspider = cspider;
  }
}
/**
  addUrls : add many urls back to task queue
  @cspider : the cspider_t
  @urls : the array of url
  @size : the size of @urls
**/
void addUrls(cspider_t *cspider, char **urls, int size) {
  int i;
  char *reUrls[size];
  for (i = 0; i < size; i++) {
    if (!bloom_check(cspider->bloom, urls[i])) {
      /*url not exits*/
      bloom_add(cspider->bloom, urls[i]);
      unsigned int len = strlen(urls[i]);
      reUrls[i] = (char*)malloc(sizeof(char) * (len + 1));
      PANIC(reUrls[i]);
      strncpy(reUrls[i], urls[i], len+1);
    } else {
      reUrls[i] = NULL;
    }
  }
  for (i = 0; i < size; i++) {
    if (reUrls[i] != NULL) {
      cs_page *page = get_page(cspider->page_queue);
      set_url(page, reUrls[i]);
      set_status(cspider->page_queue, page, url_add);
      page->cspider = cspider;
    }
  }
}

/**
   freeString : free string
   @str : ready to be freed
**/
void freeString(char *str) {
  free(str);
}

/**
   freeStrings : free string array
   @strs : string array
   @size : size of @strs
**/
void freeStrings(char **strs, int size) {
  int i;
  for (i = 0; i < size; i++) {
    free(strs[i]);
  }
}
