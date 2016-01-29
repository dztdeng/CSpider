#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "CS.h"
#include "cs_page_queue.h"


void cspider_download(uv_work_t *req);
void cspider_download_done(uv_work_t *req, int status);

#endif
