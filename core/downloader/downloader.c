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
size_t save_data(void *ptr, size_t size, size_t nmemb, void *ss) {
  //cs_task_t *save = (cs_task_t*)ss;
  cs_page *page = (cs_page*)ss;
  //size_t count = save->data->count;
  size_t all = size * nmemb;
  
  char* buf = (char*) malloc(all);
  if(buf == NULL) 
    return (size_t) -1;
  //save->data->data[count] = buf; // "char != 1" only appears in IBM machines.
  
  if(ptr == NULL)
    return (size_t) -1;
  //strncpy(save->data->data[count], (char*)ptr, all);
  
  set_page(page, ptr, all);
  //save->data->each[count] = all;
  //save->data->count = count + 1;
  //save->data->length += all;
  
  return all;
}

/**
  use curl to download
  @req : the worker
**/
void download(uv_work_t *req) {
  CURL *curl;
  CURLcode res;

  cs_page *page = (cs_page*)req->data;
  //cs_task_t *task = (cs_task_t*)req->data;
  cspider_t *cspider = page->cspider;
  site_t *site = (site_t*)cspider->site;
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
    /*支持重定向*/
    /*support redirection*/
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    
    curl_easy_setopt(curl, CURLOPT_URL, page->url);
    /*
      put url to cs_rawText_t
     */
    //task->data->url = task->url;
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req->data);
    res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
  }
}

/**
  work_done : it will be called after work thread finish
  @req : the worker
**/
void work_done(uv_work_t *req, int status) {
  cspider_t *cspider = ((cs_page*)req->data)->cspider;
  /*
    print log
   */
  logger(0, "%s download finish.\n", ((cs_page*)req->data)->url, cspider);
  /**
     change status (url_download) -> (page_add)
   **/
  set_status(cspider->page_queue, (cs_page*)req->data, page_add);
  free(req);
  return;
}



/*
int main(int argc, char **argv) {
  
}
*/
