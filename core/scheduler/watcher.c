#include "scheduler.h"

/**
  watcher : watch the work queues, to find out if there is work to do
  @handle : the uv_prepare_t
**/
void cspider_watcher(uv_prepare_t *handle) {
  cspider_t *cspider = (cspider_t*)handle->data;
  
  if (get_status_num(cspider->page_queue, url_add) != 0) {
    /*
      if there is task unhandled yet, start work thread
     */
    /* get new thread worker */
    uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
    PANIC(req);
    /* let thread worker point to the page which status is (url_add)*/
    req->data = get_status_page(cspider->page_queue, url_add);
    /* new page point to new thread worker too. */
    ((cs_page*)req->data)->worker = req;
    /* change status (url_add) -> (url_download)*/
    set_status(cspider->page_queue, (cs_page*)req->data, url_download);
    /* begin thread */
    uv_queue_work(cspider->loop, req, cspider_download, cspider_download_done);
    
  }
  
  if (get_status_num(cspider->page_queue, page_add) != 0) {
    /* if there is data required to be processed */
    /* get new thread worker */
    uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
    PANIC(req);
    /* let thread worker point to the page which status is (page_add) */
    req->data = (cs_page*)get_status_page(cspider->page_queue, page_add);
    /* new page point to new thread worker too. */
    ((cs_page*)req->data)->worker = req;
    /* change status (page_add) -> (url_process)*/
    set_status(cspider->page_queue, (cs_page*)req->data, page_process);
    /* begin thread */
    uv_queue_work(cspider->loop, req, cspider_process, cspider_process_done);
    
  }
  
  if (is_queue_empty(cspider->page_queue) == 0) {
    /* if page queue is empty,
     then stop cspider*/
    uv_prepare_stop(handle);
  }
  
}
