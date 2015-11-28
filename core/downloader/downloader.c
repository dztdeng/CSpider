#include "downloader.h"
#include "spider.h"


//char *tmp;

size_t save_data(void *ptr, size_t size, size_t nmemb, void *ss) {
  cs_task_t *save = (cs_task_t*)ss;
  size_t current = strlen(save->data->data);
  size_t all = size * nmemb;
  save->data->data = (char*)realloc(save->data->data, all+current);
  strcpy(save->data->data+current, (char*)ptr);
  return all;
}

void download(uv_work_t *req) {
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, ((cs_task_t*)(req->data))->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req->data);
    
    res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
  }
}

/*
  work线程完成工作后
*/
void work_done(uv_work_t *req, int status) {
  cspider_t *cspider = ((cs_task_t*)req->data)->cspider;
  cspider->download_thread--;
  cs_task_queue *q = removeTask(cspider->task_queue_doing, req->data);
  assert(q != NULL);
  printf("%s\n", (((cs_task_t*)(req->data))->data->data));
  addData(cspider->data_queue, q->task->data);
  freeTask(q);
  return;
}

void watcher(uv_idle_t *handle) {
  cspider_t *cspider = (cspider_t*)handle->data;
  if (!isTaskQueueEmpty(cspider->task_queue)) {
    //还有未执行或未完成的任务
    //启动work线程
    if (cspider->download_thread <= cspider->download_thread_max) {
      /*
	当线程数目未达到上限
       */
      cs_task_queue *rem = removeTask(cspider->task_queue, cspider->task_queue->next->task);
    
      assert(rem != NULL);
    
      uv_work_t *req = (uv_work_t*)malloc(sizeof(uv_work_t));
      req->data = rem->task;
      //指向执行自己的工作线程handle
      rem->task->worker = req;
      rem->task->cspider = cspider;
      addTask(cspider->task_queue_doing, rem);
      uv_queue_work(cspider->loop, req, download, work_done);

      //数目加1
      cspider->download_thread++;
    }
  }
  
  if (!isTaskQueueEmpty(cspider->data_queue)) {
    
  }

  if (!isTaskQueueEmpty(cspider->task_queue_doing) ||
      !isTaskQueueEmpty(cspider->task_queue)) {

    
    
  } else {
    uv_idle_stop(handle);
  }
}

/*
int main(int argc, char **argv) {
  
}
*/