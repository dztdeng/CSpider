#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "CS.h"
#include "downloader.h"
#include "pageProcesser.h"
#include "spider.h"
#include "cs_page_queue.h"

void watcher(uv_prepare_t *handle);

#endif
