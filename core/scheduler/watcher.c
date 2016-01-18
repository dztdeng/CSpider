#include "scheduler.h"

/**
  watcher : watch the work queues, to find out if there is work to do
  @handle : the uv_idle_t
**/
void watcher(uv_prepare_t *handle) {
  printf("test !\n");
  cspider_t *cspider = (cspider_t*)handle->data;
  uv_rwlock_wrlock(cspider->lock);
  if (!isTaskQueueEmpty(cspider->task_queue)) {
    /*
      if there is task unhandled yet, start work thread
     */
    
      cs_task_queue *rem = removeTask(cspider->task_queue, cspider->task_queue->next->task);
      PANIC(rem);
    
      uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
      PANIC(req);
      
      req->data = rem->task;
      //Point to the worker
      cs_task_t* ptask = (cs_task_t*)rem->task;
      ptask->worker = req;
      ptask->cspider = cspider;
      addTask(cspider->task_queue_doing, rem);
      uv_queue_work(cspider->loop, req, download, work_done);
    
  }
  
  if (!isDataQueueEmpty(cspider->data_queue)) {
    //if there is data required to be processed,
    // start thread
    
      cs_rawText_queue *rem = removeData(cspider->data_queue, cspider->data_queue->next->data);
      PANIC(rem);
      	
      uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
      PANIC(req);

      req->data = rem->data;
      // points to working handle
      cs_rawText_t* pdata = (cs_rawText_t*)rem->data;
      pdata->worker = req;
      pdata->cspider = cspider;
      
      
      addData(cspider->data_queue_doing, rem);
      uv_queue_work(cspider->loop, req, dataproc, datasave);
    
  }

  if (!isTaskQueueEmpty(cspider->task_queue_doing) ||
      !isTaskQueueEmpty(cspider->task_queue) ||
      !isDataQueueEmpty(cspider->data_queue) ||
      !isDataQueueEmpty(cspider->data_queue_doing)) 
  {
    uv_rwlock_wrunlock(cspider->lock);
  } 
  else {
    uv_rwlock_wrunlock(cspider->lock);
    uv_prepare_stop(handle);
  }
  
}
