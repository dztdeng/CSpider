#include "scheduler.h"

/**
  watcher : watch the work queues, to find out if there is work to do
  @handle : the uv_prepare_t
**/
void watcher(uv_prepare_t *handle) {
  cspider_t *cspider = (cspider_t*)handle->data;
  
  if (get_status_num(cspider->page_queue, url_add) != 0) {
    /*
      if there is task unhandled yet, start work thread
     */
    
    uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
    req->data = get_status_page(cspider->page_queue, url_add);
    ((cs_page*)req->data)->worker = req;
    set_status(cspider->page_queue, (cs_page*)req->data, url_download);
    uv_queue_work(cspider->loop, req, download, work_done);
    
  }
  
  if (get_status_num(cspider->page_queue, page_add) != 0) {
    //if there is data required to be processed,
    // start thread
    
    uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
    PANIC(req);
    req->data = (cs_page*)get_status_page(cspider->page_queue, page_add);
    ((cs_page*)req->data)->worker = req;
    set_status(cspider->page_queue, (cs_page*)req->data, page_process);
    uv_queue_work(cspider->loop, req, dataproc, datasave);
    
  }
  
  if (is_queue_empty(cspider->page_queue) == 0) {
    /* if page queue is empty,
     then stop cspider*/
    uv_prepare_stop(handle);
  }
  
}
