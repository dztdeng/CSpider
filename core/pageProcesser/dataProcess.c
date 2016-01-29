#include "pageProcesser.h"

/**
  dataproc : process data
  @req : the worker

  call this function to process the string we get
**/
void cspider_process(uv_work_t *req) {
  cspider_t *cspider = ((cs_page*)req->data)->cspider;
  /* get the page from worker */
  cs_page *page = (cs_page*)req->data;
  /*
    call custom process function
   */
  (cspider->process)(cspider, (char*)page->data, page->url, cspider->process_user_data);
  /**
     change status page (PAGE_PROCESS_RUNNING) -> (PAGE_SLEEP)
   **/
  set_status(cspider->page_queue, page, PAGE_SLEEP);
  clear_page_from_queue(cspider->page_queue, page);
}

/**
   datasave : call after data process worker finish
   @req : the worker

 **/
void cspider_process_done(uv_work_t *req, int status) {
  cspider_t *cspider = ((cs_page*)req->data)->cspider;
  logger(0, "%s save finish.\n", ((cs_page*)req->data)->url, cspider);
  free(req);
}

