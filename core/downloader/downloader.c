#include "downloader.h"
#include "spider.h"
#include "pageProcesser.h"
#include "utils.h"


/**
  execute after curl get data.
  @ptr : point to string which curl get
  @size :
  @nmemb : @size * @nmemb equal the size of string
  @ss : input pointer
**/
static size_t save_data(void *ptr, size_t size, size_t nmemb, void *ss) {
  cs_page *page = (cs_page*)ss;
  /* get the total size*/
  size_t all = size * nmemb;
  
  if(ptr == NULL)
    return (size_t) -1;
  /* save data in page */
  set_page(page, ptr, all);
  /* must return the size of data which we get */
  return all;
}


/**
  work_done : it will be called after work thread finish
  @req : the worker
**/
void cspider_download_done(uv_work_t *req, int status) {
  cspider_t *cspider = ((cs_page*)req->data)->cspider;
  /* print log */
  logger(0, "%s download finish.\n", ((cs_page*)req->data)->url, cspider);
  /**
     change status (url_download) -> (page_add)
   **/
  set_status(cspider->page_queue, (cs_page*)req->data, page_add);
  free(req);
  return;
}

/**
  use curl to download
  @req : the worker
**/
void cspider_download(uv_work_t *req) {
  CURL *curl;
  CURLcode res;

  cs_page *page = (cs_page*)req->data;
  
  cspider_t *cspider = page->cspider;
  site_t *site = (site_t*)cspider->site;
  /* init curl */
  curl = curl_easy_init();
  PANIC(curl);
  
  if (curl) {
    if (site->user_agent != NULL) {
      curl_easy_setopt(curl, CURLOPT_USERAGENT, site->user_agent);
    }
    if (site->proxy != NULL) {
      curl_easy_setopt(curl, CURLOPT_PROXY, site->proxy);
    }
    if (site->timeout != 0) {
      curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, site->timeout);
    }
    if (site->cookie != NULL) {
      curl_easy_setopt(curl, CURLOPT_COOKIE, site->cookie);
    }
    /*support redirection*/
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    
    curl_easy_setopt(curl, CURLOPT_URL, page->url);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req->data);
    /* start the curl */
    res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
  }
}
