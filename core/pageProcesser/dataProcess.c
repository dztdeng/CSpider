#include "pageProcesser.h"

/**
  dataproc : process data
  @req : the worker

  call this function to process the string we get
**/
void dataproc(uv_work_t *req) {
  cspider_t *cspider = ((cs_page*)req->data)->cspider;
  //cs_rawText_t *text = (cs_rawText_t*)req->data;
  cs_page *page = (cs_page*)req->data;
  /*
    call custom process function
   */
  (cspider->process)(cspider, (char*)page->data, page->url, cspider->process_user_data);
  /**
     change status page (page_process) -> (sleep)
   **/
  set_status(cspider->page_queue, page, sleep);
  clear_page_from_queue(cspider->page_queue, page);
}

/**
   datasave : call after data process worker finish
   @req : the worker

 **/
void datasave(uv_work_t *req, int status) {
  cspider_t *cspider = ((cs_page*)req->data)->cspider;
  //log
  logger(0, "%s save finish.\n", ((cs_page*)req->data)->url, cspider);
  free(req);
}

